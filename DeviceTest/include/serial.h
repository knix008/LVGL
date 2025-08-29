#ifndef SERIAL_H
#define SERIAL_H

#include "common.h"
#include <termios.h>
#include <sys/select.h>

#ifdef __cplusplus
extern "C" {
#endif

// Serial port configuration structure
typedef struct {
    int fd;                     // File descriptor
    char device_path[64];       // Device path (e.g., "/dev/ttyUSB0")
    int baud_rate;              // Baud rate
    int data_bits;              // Data bits (5, 6, 7, 8)
    int stop_bits;              // Stop bits (1, 2)
    char parity;                // Parity ('N', 'E', 'O')
    int flow_control;           // Flow control (0 = none, 1 = hardware, 2 = software)
    struct termios original_config; // Original terminal configuration
    bool is_open;               // Whether the port is open
} serial_test_t;

// Serial test parameters
typedef struct {
    char device_path[64];       // Device path
    int baud_rate;              // Baud rate to test
    int timeout_ms;             // Timeout in milliseconds
    int test_data_size;         // Size of test data to send
    bool loopback_test;         // Whether to perform loopback test
} serial_test_params_t;

// Function declarations
int init_serial_test(serial_test_t* serial, const char* device_path, int baud_rate);
void cleanup_serial_test(serial_test_t* serial);
bool open_serial_port(serial_test_t* serial);
bool close_serial_port(serial_test_t* serial);
bool configure_serial_port(serial_test_t* serial);
bool restore_serial_config(serial_test_t* serial);

// Basic serial operations
int write_serial_data(serial_test_t* serial, const char* data, int length);
int read_serial_data(serial_test_t* serial, char* buffer, int max_length, int timeout_ms);
bool flush_serial_buffers(serial_test_t* serial);

// Test functions
test_result_t test_serial_initialization(const char* device_path, int baud_rate);
test_result_t test_serial_communication(const char* device_path, int baud_rate);
test_result_t test_serial_loopback(const char* device_path, int baud_rate);
test_result_t test_serial_speed(const char* device_path, int baud_rate);
test_result_t test_serial_error_handling(const char* device_path, int baud_rate);
test_result_t test_serial_configuration(const char* device_path, int baud_rate);

// Utility functions
void list_available_serial_ports(void);
bool is_serial_port_available(const char* device_path);
int get_serial_port_info(const char* device_path, char* info, int max_length);
void print_serial_configuration(serial_test_t* serial);

// Interactive console
void run_serial_test_console(serial_test_t* serial);

// Test suite function
test_summary_t run_all_serial_tests(const char* device_path, int baud_rate);

// Command handler function
int handle_serial_commands(const char* test_type, const char* device_path, int baud_rate, bool interactive_mode);

#ifdef __cplusplus
}
#endif

#endif // SERIAL_H
