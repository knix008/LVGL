#include "serial.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <termios.h>

// C++ implementation
extern "C" {

// Common baud rates for testing
static const int common_baud_rates[] = {
    9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600
};
static const int num_baud_rates = sizeof(common_baud_rates) / sizeof(common_baud_rates[0]);

// Baud rate to termios constant mapping
static speed_t get_baud_rate_constant(int baud_rate) {
    switch (baud_rate) {
        case 50: return B50;
        case 75: return B75;
        case 110: return B110;
        case 134: return B134;
        case 150: return B150;
        case 200: return B200;
        case 300: return B300;
        case 600: return B600;
        case 1200: return B1200;
        case 1800: return B1800;
        case 2400: return B2400;
        case 4800: return B4800;
        case 9600: return B9600;
        case 19200: return B19200;
        case 38400: return B38400;
        case 57600: return B57600;
        case 115200: return B115200;
        case 230400: return B230400;
        case 460800: return B460800;
        case 500000: return B500000;
        case 576000: return B576000;
        case 921600: return B921600;
        case 1000000: return B1000000;
        case 1152000: return B1152000;
        case 1500000: return B1500000;
        case 2000000: return B2000000;
        case 2500000: return B2500000;
        case 3000000: return B3000000;
        case 3500000: return B3500000;
        case 4000000: return B4000000;
        default: return B9600; // Default fallback
    }
}

int init_serial_test(serial_test_t* serial, const char* device_path, int baud_rate) {
    if (!serial || !device_path) {
        return -1;
    }
    
    // Initialize serial structure
    memset(serial, 0, sizeof(serial_test_t));
    strncpy(serial->device_path, device_path, sizeof(serial->device_path) - 1);
    serial->baud_rate = baud_rate;
    serial->data_bits = 8;
    serial->stop_bits = 1;
    serial->parity = 'N';
    serial->flow_control = 0;
    serial->is_open = false;
    serial->fd = -1;
    
    return 0;
}

void cleanup_serial_test(serial_test_t* serial) {
    if (serial) {
        if (serial->is_open) {
            close_serial_port(serial);
        }
    }
}

bool open_serial_port(serial_test_t* serial) {
    if (!serial) {
        return false;
    }
    
    // Open the serial port
    serial->fd = open(serial->device_path, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (serial->fd < 0) {
        printf("Error: Could not open %s: %s\n", serial->device_path, strerror(errno));
        return false;
    }
    
    // Save original configuration
    if (tcgetattr(serial->fd, &serial->original_config) != 0) {
        printf("Warning: Could not get original terminal attributes: %s\n", strerror(errno));
    }
    
    serial->is_open = true;
    return true;
}

bool close_serial_port(serial_test_t* serial) {
    if (!serial || !serial->is_open) {
        return false;
    }
    
    // Restore original configuration
    restore_serial_config(serial);
    
    // Close the file descriptor
    if (close(serial->fd) != 0) {
        printf("Warning: Error closing serial port: %s\n", strerror(errno));
    }
    
    serial->fd = -1;
    serial->is_open = false;
    return true;
}

bool configure_serial_port(serial_test_t* serial) {
    if (!serial || !serial->is_open) {
        return false;
    }
    
    struct termios config;
    memset(&config, 0, sizeof(config));
    
    // Get current configuration
    if (tcgetattr(serial->fd, &config) != 0) {
        printf("Error: Could not get terminal attributes: %s\n", strerror(errno));
        return false;
    }
    
    // Set baud rate
    speed_t baud_constant = get_baud_rate_constant(serial->baud_rate);
    if (cfsetispeed(&config, baud_constant) != 0 || cfsetospeed(&config, baud_constant) != 0) {
        printf("Error: Could not set baud rate %d: %s\n", serial->baud_rate, strerror(errno));
        return false;
    }
    
    // Set data bits, parity, and stop bits
    config.c_cflag &= ~CSIZE;
    switch (serial->data_bits) {
        case 5: config.c_cflag |= CS5; break;
        case 6: config.c_cflag |= CS6; break;
        case 7: config.c_cflag |= CS7; break;
        case 8: config.c_cflag |= CS8; break;
        default: config.c_cflag |= CS8; break;
    }
    
    // Set parity
    config.c_cflag &= ~(PARENB | PARODD);
    switch (serial->parity) {
        case 'E': config.c_cflag |= PARENB; break;
        case 'O': config.c_cflag |= PARENB | PARODD; break;
        case 'N': default: break;
    }
    
    // Set stop bits
    if (serial->stop_bits == 2) {
        config.c_cflag |= CSTOPB;
    } else {
        config.c_cflag &= ~CSTOPB;
    }
    
    // Set flow control
    config.c_cflag &= ~CRTSCTS; // Disable hardware flow control by default
    if (serial->flow_control == 1) {
        config.c_cflag |= CRTSCTS;
    }
    
    // Set input flags
    config.c_iflag &= ~(IXON | IXOFF | IXANY); // Disable software flow control
    if (serial->flow_control == 2) {
        config.c_iflag |= IXON | IXOFF;
    }
    
    // Set output flags
    config.c_oflag &= ~OPOST; // Raw output
    
    // Set local flags
    config.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // Raw input
    
    // Set control characters
    config.c_cc[VMIN] = 0;  // Non-blocking
    config.c_cc[VTIME] = 1; // 0.1 second timeout
    
    // Apply configuration
    if (tcsetattr(serial->fd, TCSANOW, &config) != 0) {
        printf("Error: Could not set terminal attributes: %s\n", strerror(errno));
        return false;
    }
    
    return true;
}

bool restore_serial_config(serial_test_t* serial) {
    if (!serial || !serial->is_open) {
        return false;
    }
    
    if (tcsetattr(serial->fd, TCSANOW, &serial->original_config) != 0) {
        printf("Warning: Could not restore terminal attributes: %s\n", strerror(errno));
        return false;
    }
    
    return true;
}

int write_serial_data(serial_test_t* serial, const char* data, int length) {
    if (!serial || !serial->is_open || !data) {
        return -1;
    }
    
    int bytes_written = write(serial->fd, data, length);
    if (bytes_written < 0) {
        printf("Error writing to serial port: %s\n", strerror(errno));
    }
    
    return bytes_written;
}

int read_serial_data(serial_test_t* serial, char* buffer, int max_length, int timeout_ms) {
    if (!serial || !serial->is_open || !buffer) {
        return -1;
    }
    
    fd_set read_fds;
    struct timeval timeout;
    
    FD_ZERO(&read_fds);
    FD_SET(serial->fd, &read_fds);
    
    timeout.tv_sec = timeout_ms / 1000;
    timeout.tv_usec = (timeout_ms % 1000) * 1000;
    
    int select_result = select(serial->fd + 1, &read_fds, NULL, NULL, &timeout);
    if (select_result < 0) {
        printf("Error in select: %s\n", strerror(errno));
        return -1;
    } else if (select_result == 0) {
        // Timeout
        return 0;
    }
    
    int bytes_read = read(serial->fd, buffer, max_length);
    if (bytes_read < 0) {
        printf("Error reading from serial port: %s\n", strerror(errno));
    }
    
    return bytes_read;
}

bool flush_serial_buffers(serial_test_t* serial) {
    if (!serial || !serial->is_open) {
        return false;
    }
    
    if (tcflush(serial->fd, TCIOFLUSH) != 0) {
        printf("Warning: Could not flush serial buffers: %s\n", strerror(errno));
        return false;
    }
    
    return true;
}

void list_available_serial_ports(void) {
    printf("Available serial ports:\n");
    
    DIR* dir = opendir("/dev");
    if (!dir) {
        printf("Error: Could not open /dev directory\n");
        return;
    }
    
    struct dirent* entry;
    int count = 0;
    
    while ((entry = readdir(dir)) != NULL) {
        if (strncmp(entry->d_name, "tty", 3) == 0) {
            char full_path[128];
            snprintf(full_path, sizeof(full_path), "/dev/%s", entry->d_name);
            
            struct stat st;
            if (stat(full_path, &st) == 0 && S_ISCHR(st.st_mode)) {
                printf("  %s\n", full_path);
                count++;
            }
        }
    }
    
    closedir(dir);
    
    if (count == 0) {
        printf("  No serial ports found\n");
    } else {
        printf("Found %d serial port(s)\n", count);
    }
}

bool is_serial_port_available(const char* device_path) {
    if (!device_path) {
        return false;
    }
    
    struct stat st;
    if (stat(device_path, &st) != 0) {
        return false;
    }
    
    return S_ISCHR(st.st_mode);
}

int get_serial_port_info(const char* device_path, char* info, int max_length) {
    if (!device_path || !info) {
        return -1;
    }
    
    struct stat st;
    if (stat(device_path, &st) != 0) {
        snprintf(info, max_length, "Error: %s", strerror(errno));
        return -1;
    }
    
    snprintf(info, max_length, "Device: %s, Type: Character device, Size: %ld bytes", 
             device_path, st.st_size);
    
    return 0;
}

void print_serial_configuration(serial_test_t* serial) {
    if (!serial) {
        return;
    }
    
    printf("Serial Configuration:\n");
    printf("  Device: %s\n", serial->device_path);
    printf("  Baud Rate: %d\n", serial->baud_rate);
    printf("  Data Bits: %d\n", serial->data_bits);
    printf("  Stop Bits: %d\n", serial->stop_bits);
    printf("  Parity: %c\n", serial->parity);
    printf("  Flow Control: %d\n", serial->flow_control);
    printf("  Status: %s\n", serial->is_open ? "Open" : "Closed");
}

test_result_t test_serial_initialization(const char* device_path, int baud_rate) {
    test_result_t result = {false, "", 0.0};
    
    if (!device_path) {
        snprintf(result.message, sizeof(result.message), "Invalid device path");
        return result;
    }
    
    serial_test_t serial;
    if (init_serial_test(&serial, device_path, baud_rate) != 0) {
        snprintf(result.message, sizeof(result.message), 
                "Failed to initialize serial test for %s", device_path);
        return result;
    }
    
    if (!is_serial_port_available(device_path)) {
        snprintf(result.message, sizeof(result.message), 
                "Serial port %s is not available", device_path);
        cleanup_serial_test(&serial);
        return result;
    }
    
    if (!open_serial_port(&serial)) {
        snprintf(result.message, sizeof(result.message), 
                "Failed to open serial port %s", device_path);
        cleanup_serial_test(&serial);
        return result;
    }
    
    if (!configure_serial_port(&serial)) {
        snprintf(result.message, sizeof(result.message), 
                "Failed to configure serial port %s", device_path);
        cleanup_serial_test(&serial);
        return result;
    }
    
    result.success = true;
    snprintf(result.message, sizeof(result.message), 
            "Serial port %s initialized successfully at %d baud", device_path, baud_rate);
    result.performance_score = 100.0;
    
    cleanup_serial_test(&serial);
    return result;
}

test_result_t test_serial_communication(const char* device_path, int baud_rate) {
    test_result_t result = {false, "", 0.0};
    
    serial_test_t serial;
    if (init_serial_test(&serial, device_path, baud_rate) != 0) {
        snprintf(result.message, sizeof(result.message), 
                "Failed to initialize serial test for %s", device_path);
        return result;
    }
    
    if (!open_serial_port(&serial)) {
        snprintf(result.message, sizeof(result.message), 
                "Failed to open serial port %s", device_path);
        cleanup_serial_test(&serial);
        return result;
    }
    
    if (!configure_serial_port(&serial)) {
        snprintf(result.message, sizeof(result.message), 
                "Failed to configure serial port %s", device_path);
        cleanup_serial_test(&serial);
        return result;
    }
    
    // Test data
    const char* test_data = "Hello, Serial World!\n";
    int test_data_len = strlen(test_data);
    char read_buffer[256];
    
    // Flush buffers
    flush_serial_buffers(&serial);
    
    // Write test data
    int bytes_written = write_serial_data(&serial, test_data, test_data_len);
    if (bytes_written < 0) {
        snprintf(result.message, sizeof(result.message), 
                "Failed to write to serial port %s", device_path);
        cleanup_serial_test(&serial);
        return result;
    }
    
    // Small delay
    usleep(100000); // 100ms
    
    // Try to read data (this might not work without loopback)
    int bytes_read = read_serial_data(&serial, read_buffer, sizeof(read_buffer) - 1, 1000);
    read_buffer[bytes_read > 0 ? bytes_read : 0] = '\0';
    
    result.success = true;
    snprintf(result.message, sizeof(result.message), 
            "Serial communication test: wrote %d bytes, read %d bytes", 
            bytes_written, bytes_read);
    
    // Score based on successful write
    if (bytes_written == test_data_len) {
        result.performance_score = 100.0;
    } else if (bytes_written > 0) {
        result.performance_score = 50.0;
    } else {
        result.performance_score = 0.0;
    }
    
    cleanup_serial_test(&serial);
    return result;
}

test_result_t test_serial_loopback(const char* device_path, int baud_rate) {
    test_result_t result = {false, "", 0.0};
    
    serial_test_t serial;
    if (init_serial_test(&serial, device_path, baud_rate) != 0) {
        snprintf(result.message, sizeof(result.message), 
                "Failed to initialize serial test for %s", device_path);
        return result;
    }
    
    if (!open_serial_port(&serial)) {
        snprintf(result.message, sizeof(result.message), 
                "Failed to open serial port %s", device_path);
        cleanup_serial_test(&serial);
        return result;
    }
    
    if (!configure_serial_port(&serial)) {
        snprintf(result.message, sizeof(result.message), 
                "Failed to configure serial port %s", device_path);
        cleanup_serial_test(&serial);
        return result;
    }
    
    // Test data
    const char* test_data = "Loopback Test Data\n";
    int test_data_len = strlen(test_data);
    char read_buffer[256];
    
    // Flush buffers
    flush_serial_buffers(&serial);
    
    // Write test data
    int bytes_written = write_serial_data(&serial, test_data, test_data_len);
    if (bytes_written < 0) {
        snprintf(result.message, sizeof(result.message), 
                "Failed to write to serial port %s", device_path);
        cleanup_serial_test(&serial);
        return result;
    }
    
    // Wait for data to be available
    usleep(200000); // 200ms
    
    // Read data back
    int bytes_read = read_serial_data(&serial, read_buffer, sizeof(read_buffer) - 1, 1000);
    read_buffer[bytes_read > 0 ? bytes_read : 0] = '\0';
    
    // Check if loopback worked
    if (bytes_read > 0 && strstr(read_buffer, "Loopback Test Data") != NULL) {
        result.success = true;
        snprintf(result.message, sizeof(result.message), 
                "Loopback test successful: sent %d bytes, received %d bytes", 
                bytes_written, bytes_read);
        result.performance_score = 100.0;
    } else {
        snprintf(result.message, sizeof(result.message), 
                "Loopback test failed: sent %d bytes, received %d bytes (no loopback detected)", 
                bytes_written, bytes_read);
        result.performance_score = 0.0;
    }
    
    cleanup_serial_test(&serial);
    return result;
}

test_result_t test_serial_speed(const char* device_path, int baud_rate) {
    test_result_t result = {false, "", 0.0};
    
    serial_test_t serial;
    if (init_serial_test(&serial, device_path, baud_rate) != 0) {
        snprintf(result.message, sizeof(result.message), 
                "Failed to initialize serial test for %s", device_path);
        return result;
    }
    
    if (!open_serial_port(&serial)) {
        snprintf(result.message, sizeof(result.message), 
                "Failed to open serial port %s", device_path);
        cleanup_serial_test(&serial);
        return result;
    }
    
    if (!configure_serial_port(&serial)) {
        snprintf(result.message, sizeof(result.message), 
                "Failed to configure serial port %s", device_path);
        cleanup_serial_test(&serial);
        return result;
    }
    
    // Generate test data
    char test_data[1024];
    for (int i = 0; i < sizeof(test_data) - 1; i++) {
        test_data[i] = 'A' + (i % 26);
    }
    test_data[sizeof(test_data) - 1] = '\0';
    
    // Measure write speed
    clock_t start_time = clock();
    int total_bytes = 0;
    int iterations = 10;
    
    for (int i = 0; i < iterations; i++) {
        int bytes_written = write_serial_data(&serial, test_data, sizeof(test_data) - 1);
        if (bytes_written > 0) {
            total_bytes += bytes_written;
        }
    }
    
    clock_t end_time = clock();
    double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    
    if (elapsed_time > 0) {
        double bytes_per_second = total_bytes / elapsed_time;
        double theoretical_speed = baud_rate / 10.0; // Rough estimate
        
        result.success = true;
        snprintf(result.message, sizeof(result.message), 
                "Speed test: %.1f bytes/sec (theoretical: %.1f bytes/sec)", 
                bytes_per_second, theoretical_speed);
        
        // Score based on efficiency
        double efficiency = (bytes_per_second / theoretical_speed) * 100.0;
        if (efficiency > 80.0) {
            result.performance_score = 100.0;
        } else if (efficiency > 50.0) {
            result.performance_score = 75.0;
        } else if (efficiency > 25.0) {
            result.performance_score = 50.0;
        } else {
            result.performance_score = 25.0;
        }
    } else {
        snprintf(result.message, sizeof(result.message), 
                "Speed test failed: no data written");
        result.performance_score = 0.0;
    }
    
    cleanup_serial_test(&serial);
    return result;
}

test_result_t test_serial_error_handling(const char* device_path, int baud_rate) {
    test_result_t result = {false, "", 0.0};
    
    // Test with invalid device path
    serial_test_t serial;
    if (init_serial_test(&serial, "/dev/nonexistent", baud_rate) == 0) {
        if (open_serial_port(&serial)) {
            snprintf(result.message, sizeof(result.message), 
                    "Error: Should not be able to open nonexistent device");
            cleanup_serial_test(&serial);
            return result;
        }
    }
    
    // Test with valid device but invalid baud rate
    if (init_serial_test(&serial, device_path, 999999) == 0) {
        if (open_serial_port(&serial)) {
            if (configure_serial_port(&serial)) {
                snprintf(result.message, sizeof(result.message), 
                        "Warning: Invalid baud rate was accepted");
                result.performance_score = 50.0;
            } else {
                snprintf(result.message, sizeof(result.message), 
                        "Error handling test passed: invalid baud rate rejected");
                result.performance_score = 100.0;
            }
            cleanup_serial_test(&serial);
        }
    }
    
    result.success = true;
    if (strlen(result.message) == 0) {
        snprintf(result.message, sizeof(result.message), 
                "Error handling test completed");
        result.performance_score = 100.0;
    }
    
    return result;
}

test_result_t test_serial_configuration(const char* device_path, int baud_rate) {
    test_result_t result = {false, "", 0.0};
    
    serial_test_t serial;
    if (init_serial_test(&serial, device_path, baud_rate) != 0) {
        snprintf(result.message, sizeof(result.message), 
                "Failed to initialize serial test for %s", device_path);
        return result;
    }
    
    if (!open_serial_port(&serial)) {
        snprintf(result.message, sizeof(result.message), 
                "Failed to open serial port %s", device_path);
        cleanup_serial_test(&serial);
        return result;
    }
    
    // Test different configurations
    int config_score = 0;
    int total_configs = 0;
    
    // Test different data bits
    for (int data_bits = 5; data_bits <= 8; data_bits++) {
        serial.data_bits = data_bits;
        if (configure_serial_port(&serial)) {
            config_score += 25;
        }
        total_configs++;
    }
    
    // Test different parity settings
    char parity_options[] = {'N', 'E', 'O'};
    for (int i = 0; i < 3; i++) {
        serial.parity = parity_options[i];
        if (configure_serial_port(&serial)) {
            config_score += 25;
        }
        total_configs++;
    }
    
    result.success = true;
    snprintf(result.message, sizeof(result.message), 
            "Configuration test: %d/%d configurations successful", 
            config_score / 25, total_configs);
    
    if (total_configs > 0) {
        result.performance_score = (double)config_score / total_configs;
    }
    
    cleanup_serial_test(&serial);
    return result;
}

void run_serial_test_console(serial_test_t* serial) {
    if (!serial) {
        printf("Error: Invalid serial object\n");
        return;
    }
    
    printf("=== Serial Test Console ===\n");
    printf("Commands:\n");
    printf("  open <device> <baud> - Open serial port\n");
    printf("  close                - Close serial port\n");
    printf("  config               - Show current configuration\n");
    printf("  write <data>         - Write data to serial port\n");
    printf("  read <timeout_ms>    - Read data from serial port\n");
    printf("  flush                - Flush serial buffers\n");
    printf("  list                 - List available serial ports\n");
    printf("  test_init            - Test initialization\n");
    printf("  test_comm            - Test communication\n");
    printf("  test_loopback        - Test loopback\n");
    printf("  test_speed           - Test speed\n");
    printf("  test_config          - Test configuration\n");
    printf("  quit                 - Exit test\n");
    printf("========================\n");
    
    char command[256];
    char device_path[64];
    int baud_rate;
    
    while (1) {
        printf("serial> ");
        if (fgets(command, sizeof(command), stdin) == NULL) {
            break;
        }
        
        // Remove newline
        command[strcspn(command, "\n")] = 0;
        
        if (strcmp(command, "quit") == 0 || strcmp(command, "exit") == 0) {
            break;
        } else if (strncmp(command, "open ", 5) == 0) {
            if (sscanf(command + 5, "%s %d", device_path, &baud_rate) == 2) {
                if (init_serial_test(serial, device_path, baud_rate) == 0) {
                    if (open_serial_port(serial) && configure_serial_port(serial)) {
                        printf("Serial port opened successfully\n");
                    } else {
                        printf("Failed to open or configure serial port\n");
                    }
                } else {
                    printf("Failed to initialize serial test\n");
                }
            } else {
                printf("Usage: open <device> <baud_rate>\n");
            }
        } else if (strcmp(command, "close") == 0) {
            if (close_serial_port(serial)) {
                printf("Serial port closed successfully\n");
            } else {
                printf("Failed to close serial port\n");
            }
        } else if (strcmp(command, "config") == 0) {
            print_serial_configuration(serial);
        } else if (strncmp(command, "write ", 6) == 0) {
            if (!serial->is_open) {
                printf("Error: Serial port is not open\n");
                continue;
            }
            
            const char* data = command + 6;
            int bytes_written = write_serial_data(serial, data, strlen(data));
            if (bytes_written > 0) {
                printf("Wrote %d bytes\n", bytes_written);
            } else {
                printf("Failed to write data\n");
            }
        } else if (strncmp(command, "read ", 5) == 0) {
            if (!serial->is_open) {
                printf("Error: Serial port is not open\n");
                continue;
            }
            
            int timeout_ms;
            if (sscanf(command + 5, "%d", &timeout_ms) == 1) {
                char buffer[256];
                int bytes_read = read_serial_data(serial, buffer, sizeof(buffer) - 1, timeout_ms);
                if (bytes_read > 0) {
                    buffer[bytes_read] = '\0';
                    printf("Read %d bytes: %s\n", bytes_read, buffer);
                } else if (bytes_read == 0) {
                    printf("No data received (timeout)\n");
                } else {
                    printf("Error reading data\n");
                }
            } else {
                printf("Usage: read <timeout_ms>\n");
            }
        } else if (strcmp(command, "flush") == 0) {
            if (!serial->is_open) {
                printf("Error: Serial port is not open\n");
                continue;
            }
            
            if (flush_serial_buffers(serial)) {
                printf("Serial buffers flushed\n");
            } else {
                printf("Failed to flush serial buffers\n");
            }
        } else if (strcmp(command, "list") == 0) {
            list_available_serial_ports();
        } else if (strcmp(command, "test_init") == 0) {
            if (!serial->is_open) {
                printf("Error: Serial port is not open\n");
                continue;
            }
            
            test_result_t result = test_serial_initialization(serial->device_path, serial->baud_rate);
            printf("Initialization Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else if (strcmp(command, "test_comm") == 0) {
            if (!serial->is_open) {
                printf("Error: Serial port is not open\n");
                continue;
            }
            
            test_result_t result = test_serial_communication(serial->device_path, serial->baud_rate);
            printf("Communication Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else if (strcmp(command, "test_loopback") == 0) {
            if (!serial->is_open) {
                printf("Error: Serial port is not open\n");
                continue;
            }
            
            test_result_t result = test_serial_loopback(serial->device_path, serial->baud_rate);
            printf("Loopback Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else if (strcmp(command, "test_speed") == 0) {
            if (!serial->is_open) {
                printf("Error: Serial port is not open\n");
                continue;
            }
            
            test_result_t result = test_serial_speed(serial->device_path, serial->baud_rate);
            printf("Speed Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else if (strcmp(command, "test_config") == 0) {
            if (!serial->is_open) {
                printf("Error: Serial port is not open\n");
                continue;
            }
            
            test_result_t result = test_serial_configuration(serial->device_path, serial->baud_rate);
            printf("Configuration Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else if (strlen(command) > 0) {
            printf("Unknown command: %s\n", command);
        }
    }
    
    printf("Exiting serial test console\n");
}

// Function to run all serial tests
test_summary_t run_all_serial_tests(const char* device_path, int baud_rate) {
    test_summary_t summary = {0, 0, 0, 0.0, ""};
    
    printf("\n=== Running All Serial Tests ===\n");
    
    // Test 1: Initialization
    test_result_t result = test_serial_initialization(device_path, baud_rate);
    summary.total_tests++;
    if (result.success) {
        summary.passed_tests++;
        printf("✓ Serial Initialization: PASS (%.1f/100)\n", result.performance_score);
    } else {
        summary.failed_tests++;
        printf("✗ Serial Initialization: FAIL (%.1f/100)\n", result.performance_score);
    }
    summary.average_score += result.performance_score;
    
    // Test 2: Communication
    result = test_serial_communication(device_path, baud_rate);
    summary.total_tests++;
    if (result.success) {
        summary.passed_tests++;
        printf("✓ Serial Communication: PASS (%.1f/100)\n", result.performance_score);
    } else {
        summary.failed_tests++;
        printf("✗ Serial Communication: FAIL (%.1f/100)\n", result.performance_score);
    }
    summary.average_score += result.performance_score;
    
    // Test 3: Loopback
    result = test_serial_loopback(device_path, baud_rate);
    summary.total_tests++;
    if (result.success) {
        summary.passed_tests++;
        printf("✓ Serial Loopback: PASS (%.1f/100)\n", result.performance_score);
    } else {
        summary.failed_tests++;
        printf("✗ Serial Loopback: FAIL (%.1f/100)\n", result.performance_score);
    }
    summary.average_score += result.performance_score;
    
    // Test 4: Speed
    result = test_serial_speed(device_path, baud_rate);
    summary.total_tests++;
    if (result.success) {
        summary.passed_tests++;
        printf("✓ Serial Speed: PASS (%.1f/100)\n", result.performance_score);
    } else {
        summary.failed_tests++;
        printf("✗ Serial Speed: FAIL (%.1f/100)\n", result.performance_score);
    }
    summary.average_score += result.performance_score;
    
    // Test 5: Error Handling
    result = test_serial_error_handling(device_path, baud_rate);
    summary.total_tests++;
    if (result.success) {
        summary.passed_tests++;
        printf("✓ Serial Error Handling: PASS (%.1f/100)\n", result.performance_score);
    } else {
        summary.failed_tests++;
        printf("✗ Serial Error Handling: FAIL (%.1f/100)\n", result.performance_score);
    }
    summary.average_score += result.performance_score;
    
    // Test 6: Configuration
    result = test_serial_configuration(device_path, baud_rate);
    summary.total_tests++;
    if (result.success) {
        summary.passed_tests++;
        printf("✓ Serial Configuration: PASS (%.1f/100)\n", result.performance_score);
    } else {
        summary.failed_tests++;
        printf("✗ Serial Configuration: FAIL (%.1f/100)\n", result.performance_score);
    }
    summary.average_score += result.performance_score;
    
    // Calculate average score
    if (summary.total_tests > 0) {
        summary.average_score /= summary.total_tests;
    }
    
    // Create summary string
    snprintf(summary.summary, sizeof(summary.summary),
             "Serial Tests: %d/%d passed, Average Score: %.1f/100",
             summary.passed_tests, summary.total_tests, summary.average_score);
    
    return summary;
}

// Function to handle serial-specific commands
int handle_serial_commands(const char* test_type, const char* device_path, int baud_rate, bool interactive_mode) {
    if (interactive_mode) {
        serial_test_t serial;
        if (init_serial_test(&serial, device_path, baud_rate) == 0) {
            run_serial_test_console(&serial);
            cleanup_serial_test(&serial);
        } else {
            printf("Error: Could not initialize serial device %s\n", device_path);
            return 1;
        }
    } else if (test_type) {
        if (strcmp(test_type, "all") == 0) {
            run_all_serial_tests(device_path, baud_rate);
        } else if (strcmp(test_type, "init") == 0) {
            test_result_t result = test_serial_initialization(device_path, baud_rate);
            printf("Serial Initialization Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else if (strcmp(test_type, "comm") == 0) {
            test_result_t result = test_serial_communication(device_path, baud_rate);
            printf("Serial Communication Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else if (strcmp(test_type, "loopback") == 0) {
            test_result_t result = test_serial_loopback(device_path, baud_rate);
            printf("Serial Loopback Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else if (strcmp(test_type, "speed") == 0) {
            test_result_t result = test_serial_speed(device_path, baud_rate);
            printf("Serial Speed Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else if (strcmp(test_type, "error") == 0) {
            test_result_t result = test_serial_error_handling(device_path, baud_rate);
            printf("Serial Error Handling Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else if (strcmp(test_type, "config") == 0) {
            test_result_t result = test_serial_configuration(device_path, baud_rate);
            printf("Serial Configuration Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else {
            printf("Unknown serial test type: %s\n", test_type);
            return 1;
        }
    } else {
        // Default: run all serial tests
        run_all_serial_tests(device_path, baud_rate);
    }
    
    return 0;
}

} // extern "C"
