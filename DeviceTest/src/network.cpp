#include "network.h"
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

// C++ implementation
extern "C" {

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

} // extern "C"
