#include "network.h"
#include "camera.h"
#include "serial.h"
#include "wiegand.h"
#include "lcd.h"
#include "cpu.h"
#include "emmc.h"
#include "speaker.h"
#include "led.h"
#include "bluetooth.h"
#include "nfc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>
#include <pthread.h>
#include <sys/select.h>
#include <termios.h>

// C++ implementation
extern "C" {

// Global server instance for signal handling
static network_server_t* g_server = NULL;

int init_network_test(network_test_t* network, const char* interface_name) {
    if (!network || !interface_name) {
        return -1;
    }
    
    // Initialize network structure
    memset(network, 0, sizeof(network_test_t));
    strncpy(network->interface_name, interface_name, sizeof(network->interface_name) - 1);
    
    // Get network interface information
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        printf("Error: Could not create socket for network interface %s\n", interface_name);
        return -1;
    }
    
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, interface_name, IFNAMSIZ - 1);
    
    // Get interface flags
    if (ioctl(sock, SIOCGIFFLAGS, &ifr) < 0) {
        printf("Error: Could not get flags for interface %s\n", interface_name);
        close(sock);
        return -1;
    }
    
    network->is_up = (ifr.ifr_flags & IFF_UP) != 0;
    
    // Get IP address
    if (ioctl(sock, SIOCGIFADDR, &ifr) >= 0) {
        struct sockaddr_in* addr = (struct sockaddr_in*)&ifr.ifr_addr;
        strcpy(network->ip_address, inet_ntoa(addr->sin_addr));
    } else {
        strcpy(network->ip_address, "0.0.0.0");
    }
    
    // Get MTU
    if (ioctl(sock, SIOCGIFMTU, &ifr) >= 0) {
        network->mtu = ifr.ifr_mtu;
    } else {
        network->mtu = 1500; // Default MTU
    }
    
    close(sock);
    
    // Get MAC address from /sys/class/net
    char mac_path[256];
    snprintf(mac_path, sizeof(mac_path), "/sys/class/net/%s/address", interface_name);
    
    FILE* mac_file = fopen(mac_path, "r");
    if (mac_file) {
        if (fgets(network->mac_address, sizeof(network->mac_address), mac_file)) {
            // Remove newline
            network->mac_address[strcspn(network->mac_address, "\n")] = 0;
        } else {
            strcpy(network->mac_address, "00:00:00:00:00:00");
        }
        fclose(mac_file);
    } else {
        strcpy(network->mac_address, "00:00:00:00:00:00");
    }
    
    // Get speed from /sys/class/net
    char speed_path[256];
    snprintf(speed_path, sizeof(speed_path), "/sys/class/net/%s/speed", interface_name);
    
    FILE* speed_file = fopen(speed_path, "r");
    if (speed_file) {
        if (fscanf(speed_file, "%d", &network->speed_mbps) != 1) {
            network->speed_mbps = 0; // Unknown speed
        }
        fclose(speed_file);
    } else {
        network->speed_mbps = 0; // Unknown speed
    }
    
    printf("Network interface %s initialized successfully\n", interface_name);
    printf("IP Address: %s\n", network->ip_address);
    printf("MAC Address: %s\n", network->mac_address);
    printf("Status: %s\n", network->is_up ? "UP" : "DOWN");
    printf("Speed: %d Mbps\n", network->speed_mbps);
    printf("MTU: %d\n", network->mtu);
    
    return 0;
}

void cleanup_network_test(network_test_t* network) {
    if (network) {
        // No specific cleanup needed for network interface
        printf("Network test cleanup completed\n");
    }
}

bool get_network_stats(network_test_t* network) {
    if (!network) {
        return false;
    }
    
    char stats_path[256];
    snprintf(stats_path, sizeof(stats_path), "/sys/class/net/%s/statistics/", network->interface_name);
    
    // Read RX bytes
    char rx_bytes_path[256];
    snprintf(rx_bytes_path, sizeof(rx_bytes_path), "%srx_bytes", stats_path);
    FILE* rx_file = fopen(rx_bytes_path, "r");
    if (rx_file) {
        fscanf(rx_file, "%llu", &network->rx_bytes);
        fclose(rx_file);
    }
    
    // Read TX bytes
    char tx_bytes_path[256];
    snprintf(tx_bytes_path, sizeof(tx_bytes_path), "%stx_bytes", stats_path);
    FILE* tx_file = fopen(tx_bytes_path, "r");
    if (tx_file) {
        fscanf(tx_file, "%llu", &network->tx_bytes);
        fclose(tx_file);
    }
    
    // Read RX packets
    char rx_packets_path[256];
    snprintf(rx_packets_path, sizeof(rx_packets_path), "%srx_packets", stats_path);
    FILE* rx_packets_file = fopen(rx_packets_path, "r");
    if (rx_packets_file) {
        fscanf(rx_packets_file, "%llu", &network->rx_packets);
        fclose(rx_packets_file);
    }
    
    // Read TX packets
    char tx_packets_path[256];
    snprintf(tx_packets_path, sizeof(tx_packets_path), "%stx_packets", stats_path);
    FILE* tx_packets_file = fopen(tx_packets_path, "r");
    if (tx_packets_file) {
        fscanf(tx_packets_file, "%llu", &network->tx_packets);
        fclose(tx_packets_file);
    }
    
    return true;
}

bool test_network_connectivity_internal(network_test_t* network, const char* target_host) {
    if (!network || !target_host) {
        return false;
    }
    
    // Resolve hostname to IP address
    struct hostent* host = gethostbyname(target_host);
    if (!host) {
        printf("Error: Could not resolve hostname %s\n", target_host);
        return false;
    }
    
    // Create socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printf("Error: Could not create socket\n");
        return false;
    }
    
    // Set timeout
    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
    
    // Connect to target
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(80); // HTTP port
    memcpy(&server_addr.sin_addr, host->h_addr, host->h_length);
    
    int result = connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr));
    close(sock);
    
    return result == 0;
}

void run_network_test_console(network_test_t* network) {
    if (!network) {
        printf("Error: Invalid network object\n");
        return;
    }
    
    printf("=== Network Test Console ===\n");
    printf("Commands:\n");
    printf("  stats   - Show network statistics\n");
    printf("  ping <host> - Test connectivity to host\n");
    printf("  info    - Show network information\n");
    printf("  quit    - Exit test\n");
    printf("==========================\n");
    
    char command[256];
    char host[256];
    
    while (1) {
        printf("network> ");
        if (fgets(command, sizeof(command), stdin) == NULL) {
            break;
        }
        
        // Remove newline
        command[strcspn(command, "\n")] = 0;
        
        if (strcmp(command, "quit") == 0 || strcmp(command, "exit") == 0) {
            break;
        } else if (strcmp(command, "stats") == 0) {
            if (get_network_stats(network)) {
                printf("Network Statistics:\n");
                printf("  RX Bytes: %llu\n", network->rx_bytes);
                printf("  TX Bytes: %llu\n", network->tx_bytes);
                printf("  RX Packets: %llu\n", network->rx_packets);
                printf("  TX Packets: %llu\n", network->tx_packets);
            } else {
                printf("Error: Could not get network statistics\n");
            }
        } else if (strncmp(command, "ping ", 5) == 0) {
            strcpy(host, command + 5);
            if (strlen(host) == 0) {
                printf("Error: Please specify a host to ping\n");
                continue;
            }
            
            printf("Testing connectivity to %s...\n", host);
            if (test_network_connectivity_internal(network, host)) {
                printf("✓ Connectivity to %s successful\n", host);
            } else {
                printf("✗ Connectivity to %s failed\n", host);
            }
        } else if (strcmp(command, "info") == 0) {
            printf("Network Information:\n");
            printf("  Interface: %s\n", network->interface_name);
            printf("  IP Address: %s\n", network->ip_address);
            printf("  MAC Address: %s\n", network->mac_address);
            printf("  Status: %s\n", network->is_up ? "UP" : "DOWN");
            printf("  Speed: %d Mbps\n", network->speed_mbps);
            printf("  MTU: %d\n", network->mtu);
        } else if (strlen(command) > 0) {
            printf("Unknown command: %s\n", command);
        }
    }
    
    printf("Exiting network test console\n");
}

test_result_t test_network_initialization(const char* interface_name) {
    test_result_t result = {false, "", 0.0};
    
    if (!interface_name) {
        snprintf(result.message, sizeof(result.message), "Invalid interface name");
        return result;
    }
    
    network_test_t network;
    int ret = init_network_test(&network, interface_name);
    
    if (ret == 0) {
        result.success = true;
        snprintf(result.message, sizeof(result.message), 
                "Network interface %s initialized successfully", interface_name);
        
        // Score based on interface status and speed
        if (network.is_up) {
            if (network.speed_mbps >= 1000) {
                result.performance_score = 100.0;
            } else if (network.speed_mbps >= 100) {
                result.performance_score = 80.0;
            } else if (network.speed_mbps >= 10) {
                result.performance_score = 60.0;
            } else {
                result.performance_score = 40.0;
            }
        } else {
            result.performance_score = 20.0; // Interface down
        }
        
        cleanup_network_test(&network);
    } else {
        snprintf(result.message, sizeof(result.message), 
                "Failed to initialize network interface %s", interface_name);
        result.performance_score = 0.0;
    }
    
    return result;
}

test_result_t test_network_connectivity(const char* interface_name, const char* target_host) {
    test_result_t result = {false, "", 0.0};
    
    if (!interface_name || !target_host) {
        snprintf(result.message, sizeof(result.message), "Invalid parameters");
        return result;
    }
    
    network_test_t network;
    if (init_network_test(&network, interface_name) != 0) {
        snprintf(result.message, sizeof(result.message), 
                "Network interface %s initialization failed", interface_name);
        return result;
    }
    
    if (!network.is_up) {
        snprintf(result.message, sizeof(result.message), 
                "Network interface %s is down", interface_name);
        cleanup_network_test(&network);
        return result;
    }
    
    // Test connectivity to multiple hosts
    const char* test_hosts[] = {"8.8.8.8", "1.1.1.1", "google.com"};
    int successful_tests = 0;
    int total_tests = sizeof(test_hosts) / sizeof(test_hosts[0]);
    
    for (int i = 0; i < total_tests; i++) {
                    if (test_network_connectivity_internal(&network, test_hosts[i])) {
            successful_tests++;
        }
    }
    
    if (successful_tests > 0) {
        result.success = true;
        snprintf(result.message, sizeof(result.message), 
                "Network connectivity test: %d/%d hosts reachable", successful_tests, total_tests);
        result.performance_score = (double)successful_tests / total_tests * 100.0;
    } else {
        snprintf(result.message, sizeof(result.message), 
                "Network connectivity test failed: no hosts reachable");
        result.performance_score = 0.0;
    }
    
    cleanup_network_test(&network);
    return result;
}

test_result_t test_network_speed(const char* interface_name) {
    test_result_t result = {false, "", 0.0};
    
    if (!interface_name) {
        snprintf(result.message, sizeof(result.message), "Invalid interface name");
        return result;
    }
    
    network_test_t network;
    if (init_network_test(&network, interface_name) != 0) {
        snprintf(result.message, sizeof(result.message), 
                "Network interface %s initialization failed", interface_name);
        return result;
    }
    
    if (network.speed_mbps > 0) {
        result.success = true;
        snprintf(result.message, sizeof(result.message), 
                "Network interface %s speed: %d Mbps", interface_name, network.speed_mbps);
        
        // Score based on speed
        if (network.speed_mbps >= 1000) {
            result.performance_score = 100.0;
        } else if (network.speed_mbps >= 100) {
            result.performance_score = 80.0;
        } else if (network.speed_mbps >= 10) {
            result.performance_score = 60.0;
        } else {
            result.performance_score = 40.0;
        }
    } else {
        snprintf(result.message, sizeof(result.message), 
                "Network interface %s speed unknown", interface_name);
        result.performance_score = 30.0; // Unknown speed
    }
    
    cleanup_network_test(&network);
    return result;
}

test_result_t test_network_packet_loss(const char* interface_name, const char* target_host) {
    test_result_t result = {false, "", 0.0};
    
    if (!interface_name || !target_host) {
        snprintf(result.message, sizeof(result.message), "Invalid parameters");
        return result;
    }
    
    network_test_t network;
    if (init_network_test(&network, interface_name) != 0) {
        snprintf(result.message, sizeof(result.message), 
                "Network interface %s initialization failed", interface_name);
        return result;
    }
    
    if (!network.is_up) {
        snprintf(result.message, sizeof(result.message), 
                "Network interface %s is down", interface_name);
        cleanup_network_test(&network);
        return result;
    }
    
    // Simple packet loss test using connectivity
    int successful_pings = 0;
    int total_pings = 5;
    
    for (int i = 0; i < total_pings; i++) {
        if (test_network_connectivity_internal(&network, target_host)) {
            successful_pings++;
        }
        usleep(100000); // 100ms delay
    }
    
    double packet_loss_rate = 1.0 - ((double)successful_pings / total_pings);
    
    result.success = successful_pings > 0;
    snprintf(result.message, sizeof(result.message), 
            "Packet loss test: %d/%d successful (%.1f%% loss)", 
            successful_pings, total_pings, packet_loss_rate * 100.0);
    
    // Score based on packet loss
    if (packet_loss_rate == 0.0) {
        result.performance_score = 100.0;
    } else if (packet_loss_rate <= 0.1) {
        result.performance_score = 80.0;
    } else if (packet_loss_rate <= 0.3) {
        result.performance_score = 60.0;
    } else if (packet_loss_rate <= 0.5) {
        result.performance_score = 40.0;
    } else {
        result.performance_score = 20.0;
    }
    
    cleanup_network_test(&network);
    return result;
}

// Function to run all network tests
test_summary_t run_all_network_tests(const char* interface_name) {
    test_summary_t summary = {0, 0, 0, 0.0, ""};
    
    printf("\n=== Running All Network Tests ===\n");
    
    // Test 1: Initialization
    test_result_t result = test_network_initialization(interface_name);
    summary.total_tests++;
    if (result.success) {
        summary.passed_tests++;
        printf("✓ Network Initialization: PASS (%.1f/100)\n", result.performance_score);
    } else {
        summary.failed_tests++;
        printf("✗ Network Initialization: FAIL (%.1f/100)\n", result.performance_score);
    }
    summary.average_score += result.performance_score;
    
    // Test 2: Connectivity
    result = test_network_connectivity(interface_name, "8.8.8.8");
    summary.total_tests++;
    if (result.success) {
        summary.passed_tests++;
        printf("✓ Network Connectivity: PASS (%.1f/100)\n", result.performance_score);
    } else {
        summary.failed_tests++;
        printf("✗ Network Connectivity: FAIL (%.1f/100)\n", result.performance_score);
    }
    summary.average_score += result.performance_score;
    
    // Test 3: Speed
    result = test_network_speed(interface_name);
    summary.total_tests++;
    if (result.success) {
        summary.passed_tests++;
        printf("✓ Network Speed: PASS (%.1f/100)\n", result.performance_score);
    } else {
        summary.failed_tests++;
        printf("✗ Network Speed: FAIL (%.1f/100)\n", result.performance_score);
    }
    summary.average_score += result.performance_score;
    
    // Test 4: Packet Loss
    result = test_network_packet_loss(interface_name, "8.8.8.8");
    summary.total_tests++;
    if (result.success) {
        summary.passed_tests++;
        printf("✓ Network Packet Loss: PASS (%.1f/100)\n", result.performance_score);
    } else {
        summary.failed_tests++;
        printf("✗ Network Packet Loss: FAIL (%.1f/100)\n", result.performance_score);
    }
    summary.average_score += result.performance_score;
    
    // Calculate average score
    if (summary.total_tests > 0) {
        summary.average_score /= summary.total_tests;
    }
    
    // Create summary string
    snprintf(summary.summary, sizeof(summary.summary),
             "Network Tests: %d/%d passed, Average Score: %.1f/100",
             summary.passed_tests, summary.total_tests, summary.average_score);
    
    return summary;
}

// Function to handle network-specific commands
int handle_network_commands(const char* test_type, const char* interface_name, bool interactive_mode) {
    if (interactive_mode) {
        network_test_t network;
        if (init_network_test(&network, interface_name) == 0) {
            run_network_test_console(&network);
            cleanup_network_test(&network);
        } else {
            printf("Error: Could not initialize network interface %s\n", interface_name);
            return 1;
        }
    } else if (test_type) {
        if (strcmp(test_type, "all") == 0) {
            run_all_network_tests(interface_name);
        } else if (strcmp(test_type, "init") == 0) {
            test_result_t result = test_network_initialization(interface_name);
            printf("Network Initialization Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else if (strcmp(test_type, "connectivity") == 0) {
            test_result_t result = test_network_connectivity(interface_name, "8.8.8.8");
            printf("Network Connectivity Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else if (strcmp(test_type, "speed") == 0) {
            test_result_t result = test_network_speed(interface_name);
            printf("Network Speed Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else if (strcmp(test_type, "packet_loss") == 0) {
            test_result_t result = test_network_packet_loss(interface_name, "8.8.8.8");
            printf("Network Packet Loss Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else {
            printf("Unknown network test type: %s\n", test_type);
            return 1;
        }
    } else {
        // Default: run all network tests
        run_all_network_tests(interface_name);
    }
    
    return 0;
}

// Network server implementation
int init_network_server(network_server_t* server, int port, const char* bind_address) {
    if (!server) {
        printf("Error: Invalid server object\n");
        return -1;
    }
    
    memset(server, 0, sizeof(network_server_t));
    server->port = port;
    server->running = false;
    server->server_socket = -1;
    
    if (bind_address) {
        strncpy(server->bind_address, bind_address, sizeof(server->bind_address) - 1);
    } else {
        strcpy(server->bind_address, "0.0.0.0");
    }
    
    // Create socket
    server->server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server->server_socket < 0) {
        printf("Error: Could not create server socket: %s\n", strerror(errno));
        return -1;
    }
    
    // Set socket options to reuse address
    int reuse = 1;
    if (setsockopt(server->server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        printf("Warning: Could not set socket option SO_REUSEADDR: %s\n", strerror(errno));
    }
    
    // Bind socket
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(server->bind_address);
    
    if (bind(server->server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printf("Error: Could not bind server socket to %s:%d: %s\n", server->bind_address, port, strerror(errno));
        close(server->server_socket);
        server->server_socket = -1;
        return -1;
    }
    
    // Listen for connections
    if (listen(server->server_socket, 5) < 0) {
        printf("Error: Could not listen on server socket: %s\n", strerror(errno));
        close(server->server_socket);
        server->server_socket = -1;
        return -1;
    }
    
    printf("Network server initialized on %s:%d\n", server->bind_address, port);
    return 0;
}

void cleanup_network_server(network_server_t* server) {
    if (server) {
        server->running = false;
        if (server->server_socket >= 0) {
            close(server->server_socket);
            server->server_socket = -1;
        }
        printf("Network server cleanup completed\n");
    }
}

int start_network_server(network_server_t* server) {
    if (!server || server->server_socket < 0) {
        printf("Error: Server not properly initialized\n");
        return -1;
    }
    
    server->running = true;
    printf("Network server started, listening on %s:%d\n", server->bind_address, server->port);
    printf("Waiting for client connections...\n");
    
    // Set socket to non-blocking mode for better signal handling
    int flags = fcntl(server->server_socket, F_GETFL, 0);
    fcntl(server->server_socket, F_SETFL, flags | O_NONBLOCK);
    
    while (server->running) {
        fd_set read_fds;
        struct timeval timeout;
        
        FD_ZERO(&read_fds);
        FD_SET(server->server_socket, &read_fds);
        FD_SET(STDIN_FILENO, &read_fds);
        
        timeout.tv_sec = 0;
        timeout.tv_usec = 500000; // 500ms timeout
        
        int max_fd = server->server_socket > STDIN_FILENO ? server->server_socket : STDIN_FILENO;
        int select_result = select(max_fd + 1, &read_fds, NULL, NULL, &timeout);
        
        if (select_result < 0) {
            if (errno == EINTR) {
                continue; // Interrupted by signal, check running flag
            }
            printf("Error in select(): %s\n", strerror(errno));
            break;
        }
        
        // Check for keyboard input
        if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            char c = getchar();
            if (c == 'q' || c == 'Q' || c == 27) { // 27 is ESC key
                printf("\nShutdown requested via keyboard. Stopping server...\n");
                server->running = false;
                break;
            }
        }
        
        // Check for incoming client connections
        if (FD_ISSET(server->server_socket, &read_fds)) {
            struct sockaddr_in client_addr;
            socklen_t client_len = sizeof(client_addr);
            
            int client_socket = accept(server->server_socket, (struct sockaddr*)&client_addr, &client_len);
            if (client_socket < 0) {
                if (server->running) {
                    printf("Error: Could not accept client connection: %s\n", strerror(errno));
                }
                continue;
            }
            
            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
            printf("Client connected from %s:%d\n", client_ip, ntohs(client_addr.sin_port));
            
            // Handle client connection
            handle_client_connection(server, client_socket);
            
            close(client_socket);
            printf("Client %s:%d disconnected\n", client_ip, ntohs(client_addr.sin_port));
        }
    }
    
    return 0;
}

void stop_network_server(network_server_t* server) {
    if (server) {
        server->running = false;
        printf("Network server stop requested\n");
    }
}

int handle_client_connection(network_server_t* server, int client_socket) {
    char buffer[1024];
    char response[2048];
    
    while (server->running) {
        memset(buffer, 0, sizeof(buffer));
        
        // Receive data from client
        ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            if (bytes_received == 0) {
                printf("Client disconnected\n");
            } else {
                printf("Error receiving data: %s\n", strerror(errno));
            }
            break;
        }
        
        printf("Received command: %s\n", buffer);
        
        // Parse and process command
        remote_command_t cmd;
        memset(&cmd, 0, sizeof(cmd));
        cmd.client_socket = client_socket;
        
        if (parse_command_json(buffer, &cmd) == 0) {
            memset(response, 0, sizeof(response));
            if (process_remote_command(&cmd, response, sizeof(response)) == 0) {
                // Send response back to client
                ssize_t bytes_sent = send(client_socket, response, strlen(response), 0);
                if (bytes_sent < 0) {
                    printf("Error sending response: %s\n", strerror(errno));
                } else {
                    printf("Response sent: %s\n", response);
                }
            } else {
                // Send error response
                snprintf(response, sizeof(response), 
                         "{\"success\":false,\"message\":\"Command execution failed\",\"score\":0.0}");
                send(client_socket, response, strlen(response), 0);
            }
        } else {
            // Send error response for invalid command
            snprintf(response, sizeof(response), 
                     "{\"success\":false,\"message\":\"Invalid command format\",\"score\":0.0}");
            send(client_socket, response, strlen(response), 0);
        }
    }
    
    return 0;
}

int parse_command_json(const char* json_str, remote_command_t* cmd) {
    if (!json_str || !cmd) {
        return -1;
    }
    
    // Simple JSON parsing (for production, consider using a proper JSON library)
    // Expected format: {"command":"test","device":"camera","parameters":"init"}
    
    // Find command field
    const char* cmd_start = strstr(json_str, "\"command\":");
    if (!cmd_start) {
        return -1;
    }
    cmd_start = strchr(cmd_start, ':') + 1;
    while (*cmd_start == ' ' || *cmd_start == '\t') cmd_start++;
    if (*cmd_start == '"') cmd_start++;
    
    const char* cmd_end = strchr(cmd_start, '"');
    if (!cmd_end || cmd_end - cmd_start >= sizeof(cmd->command)) {
        return -1;
    }
    strncpy(cmd->command, cmd_start, cmd_end - cmd_start);
    
    // Find device field
    const char* dev_start = strstr(json_str, "\"device\":");
    if (dev_start) {
        dev_start = strchr(dev_start, ':') + 1;
        while (*dev_start == ' ' || *dev_start == '\t') dev_start++;
        if (*dev_start == '"') dev_start++;
        
        const char* dev_end = strchr(dev_start, '"');
        if (dev_end && dev_end - dev_start < sizeof(cmd->device_type)) {
            strncpy(cmd->device_type, dev_start, dev_end - dev_start);
        }
    }
    
    // Find parameters field
    const char* param_start = strstr(json_str, "\"parameters\":");
    if (param_start) {
        param_start = strchr(param_start, ':') + 1;
        while (*param_start == ' ' || *param_start == '\t') param_start++;
        if (*param_start == '"') param_start++;
        
        const char* param_end = strchr(param_start, '"');
        if (param_end && param_end - param_start < sizeof(cmd->parameters)) {
            strncpy(cmd->parameters, param_start, param_end - param_start);
        }
    }
    
    return 0;
}

int create_response_json(const char* command, bool success, const char* message, double score, char* response, size_t response_size) {
    if (!response || response_size == 0) {
        return -1;
    }
    
    snprintf(response, response_size,
             "{\"command\":\"%s\",\"success\":%s,\"message\":\"%s\",\"score\":%.2f}",
             command ? command : "unknown",
             success ? "true" : "false",
             message ? message : "",
             score);
    
    return 0;
}

int process_remote_command(const remote_command_t* cmd, char* response, size_t response_size) {
    if (!cmd || !response) {
        return -1;
    }
    
    printf("Processing command: %s, device: %s, parameters: %s\n", 
           cmd->command, cmd->device_type, cmd->parameters);
    
    test_result_t result = {false, "", 0.0};
    
    if (strcmp(cmd->command, "test") == 0) {
        // Handle different device types
        if (strcmp(cmd->device_type, "camera") == 0) {
            if (strcmp(cmd->parameters, "init") == 0) {
                result = test_camera_initialization(0);
            } else if (strcmp(cmd->parameters, "capture") == 0) {
                result = test_camera_capture(0);
            } else if (strcmp(cmd->parameters, "all") == 0) {
                test_summary_t summary = run_all_camera_tests(0);
                result.success = summary.passed_tests > 0;
                snprintf(result.message, sizeof(result.message), "%s", summary.summary);
                result.performance_score = summary.average_score;
            } else {
                snprintf(result.message, sizeof(result.message), "Unknown camera test: %s", cmd->parameters);
            }
        } else if (strcmp(cmd->device_type, "network") == 0) {
            if (strcmp(cmd->parameters, "init") == 0) {
                result = test_network_initialization("lo");
            } else if (strcmp(cmd->parameters, "connectivity") == 0) {
                result = test_network_connectivity("lo", "8.8.8.8");
            } else if (strcmp(cmd->parameters, "all") == 0) {
                test_summary_t summary = run_all_network_tests("lo");
                result.success = summary.passed_tests > 0;
                snprintf(result.message, sizeof(result.message), "%s", summary.summary);
                result.performance_score = summary.average_score;
            } else {
                snprintf(result.message, sizeof(result.message), "Unknown network test: %s", cmd->parameters);
            }
        } else if (strcmp(cmd->device_type, "cpu") == 0) {
            if (strcmp(cmd->parameters, "all") == 0) {
                test_summary_t summary = run_all_cpu_tests();
                result.success = summary.passed_tests > 0;
                snprintf(result.message, sizeof(result.message), "%s", summary.summary);
                result.performance_score = summary.average_score;
            } else {
                snprintf(result.message, sizeof(result.message), "Unknown CPU test: %s", cmd->parameters);
            }
        } else if (strcmp(cmd->device_type, "bluetooth") == 0) {
            if (strcmp(cmd->parameters, "all") == 0) {
                test_summary_t summary = run_all_bluetooth_tests();
                result.success = summary.passed_tests > 0;
                snprintf(result.message, sizeof(result.message), "%s", summary.summary);
                result.performance_score = summary.average_score;
            } else {
                snprintf(result.message, sizeof(result.message), "Unknown Bluetooth test: %s", cmd->parameters);
            }
        } else if (strcmp(cmd->device_type, "nfc") == 0) {
            if (strcmp(cmd->parameters, "all") == 0) {
                test_summary_t summary = run_all_nfc_tests();
                result.success = summary.passed_tests > 0;
                snprintf(result.message, sizeof(result.message), "%s", summary.summary);
                result.performance_score = summary.average_score;
            } else {
                snprintf(result.message, sizeof(result.message), "Unknown NFC test: %s", cmd->parameters);
            }
        } else if (strcmp(cmd->device_type, "serial") == 0) {
            // Parse serial parameters: "device:/dev/ttyUSB0,baud:115200,test:all"
            char device_path[64] = "/dev/ttyUSB0"; // default
            int baud_rate = 115200; // default
            char test_name[32] = "all"; // default
            
            // Parse parameters string
            char params_copy[256];
            strncpy(params_copy, cmd->parameters, sizeof(params_copy) - 1);
            params_copy[sizeof(params_copy) - 1] = '\0';
            
            char* token = strtok(params_copy, ",");
            while (token != NULL) {
                if (strncmp(token, "device:", 7) == 0) {
                    strncpy(device_path, token + 7, sizeof(device_path) - 1);
                    device_path[sizeof(device_path) - 1] = '\0';
                } else if (strncmp(token, "baud:", 5) == 0) {
                    baud_rate = atoi(token + 5);
                } else if (strncmp(token, "test:", 5) == 0) {
                    strncpy(test_name, token + 5, sizeof(test_name) - 1);
                    test_name[sizeof(test_name) - 1] = '\0';
                }
                token = strtok(NULL, ",");
            }
            
            // Handle simple format for backward compatibility
            if (strcmp(cmd->parameters, "all") == 0) {
                strcpy(test_name, "all");
            } else if (strcmp(cmd->parameters, "init") == 0) {
                strcpy(test_name, "init");
            } else if (strcmp(cmd->parameters, "comm") == 0) {
                strcpy(test_name, "comm");
            } else if (strcmp(cmd->parameters, "loopback") == 0) {
                strcpy(test_name, "loopback");
            }
            
            // Execute serial tests
            if (strcmp(test_name, "init") == 0) {
                result = test_serial_initialization(device_path, baud_rate);
            } else if (strcmp(test_name, "comm") == 0) {
                result = test_serial_communication(device_path, baud_rate);
            } else if (strcmp(test_name, "loopback") == 0) {
                result = test_serial_loopback(device_path, baud_rate);
            } else if (strcmp(test_name, "speed") == 0) {
                result = test_serial_speed(device_path, baud_rate);
            } else if (strcmp(test_name, "error") == 0) {
                result = test_serial_error_handling(device_path, baud_rate);
            } else if (strcmp(test_name, "config") == 0) {
                result = test_serial_configuration(device_path, baud_rate);
            } else if (strcmp(test_name, "all") == 0) {
                test_summary_t summary = run_all_serial_tests(device_path, baud_rate);
                result.success = summary.passed_tests > 0;
                snprintf(result.message, sizeof(result.message), "%s", summary.summary);
                result.performance_score = summary.average_score;
            } else {
                snprintf(result.message, sizeof(result.message), "Unknown serial test: %s", test_name);
            }
        } else {
            snprintf(result.message, sizeof(result.message), "Unknown device type: %s", cmd->device_type);
        }
    } else if (strcmp(cmd->command, "status") == 0) {
        result.success = true;
        snprintf(result.message, sizeof(result.message), "Device test server is running");
        result.performance_score = 100.0;
    } else if (strcmp(cmd->command, "shutdown") == 0) {
        result.success = true;
        snprintf(result.message, sizeof(result.message), "Server shutdown requested");
        result.performance_score = 100.0;
        // Signal the global server to stop
        if (g_server) {
            stop_network_server(g_server);
        }
    } else {
        snprintf(result.message, sizeof(result.message), "Unknown command: %s", cmd->command);
    }
    
    return create_response_json(cmd->command, result.success, result.message, result.performance_score, response, response_size);
}

void signal_handler(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        printf("\nReceived signal %d, shutting down server...\n", sig);
        if (g_server) {
            stop_network_server(g_server);
        }
    }
}

int run_command_server(int port, const char* bind_address) {
    network_server_t server;
    g_server = &server;
    
    // Set up signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    if (init_network_server(&server, port, bind_address) != 0) {
        printf("Error: Failed to initialize network server\n");
        return -1;
    }
    
    printf("=== Device Test Command Server ===\n");
    printf("Server listening on %s:%d\n", server.bind_address, port);
    printf("Supported commands:\n");
    printf("  {\"command\":\"test\",\"device\":\"camera\",\"parameters\":\"all\"}\n");
    printf("  {\"command\":\"test\",\"device\":\"network\",\"parameters\":\"all\"}\n");
    printf("  {\"command\":\"test\",\"device\":\"cpu\",\"parameters\":\"all\"}\n");
    printf("  {\"command\":\"test\",\"device\":\"bluetooth\",\"parameters\":\"all\"}\n");
    printf("  {\"command\":\"test\",\"device\":\"nfc\",\"parameters\":\"all\"}\n");
    printf("  {\"command\":\"test\",\"device\":\"serial\",\"parameters\":\"all\"}\n");
    printf("  {\"command\":\"test\",\"device\":\"serial\",\"parameters\":\"device:/dev/ttyUSB0,baud:115200,test:all\"}\n");
    printf("  {\"command\":\"test\",\"device\":\"serial\",\"parameters\":\"device:/dev/ttyUSB0,baud:9600,test:init\"}\n");
    printf("  {\"command\":\"status\",\"device\":\"\",\"parameters\":\"\"}\n");
    printf("  {\"command\":\"shutdown\",\"device\":\"\",\"parameters\":\"\"}\n");
    printf("Press Ctrl+C, Q, or Escape to stop the server\n");
    printf("Remote clients can also send 'shutdown' command\n");
    printf("=====================================\n\n");
    
    int result = start_network_server(&server);
    
    cleanup_network_server(&server);
    g_server = NULL;
    
    return result;
}

} // extern "C"
