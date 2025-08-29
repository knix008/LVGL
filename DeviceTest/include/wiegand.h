#ifndef WIEGAND_H
#define WIEGAND_H

#include "common.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Wiegand protocol types
typedef enum {
    WIEGAND_26BIT = 26,
    WIEGAND_34BIT = 34,
    WIEGAND_37BIT = 37,
    WIEGAND_40BIT = 40,
    WIEGAND_50BIT = 50,
    WIEGAND_64BIT = 64
} wiegand_type_t;

// Wiegand data structure
typedef struct {
    uint64_t facility_code;
    uint64_t card_number;
    uint64_t raw_data;
    wiegand_type_t type;
    bool parity_valid;
    uint8_t data_bits;
} wiegand_data_t;

// Wiegand test configuration
typedef struct {
    int data0_pin;           // Data 0 pin (usually green wire)
    int data1_pin;           // Data 1 pin (usually white wire)
    int led_pin;             // LED pin (optional)
    int buzzer_pin;          // Buzzer pin (optional)
    wiegand_type_t protocol_type; // Wiegand protocol type
    int timeout_ms;          // Timeout for reading data
    bool enable_led;         // Enable LED feedback
    bool enable_buzzer;      // Enable buzzer feedback
} wiegand_config_t;

// Wiegand test results
typedef struct {
    wiegand_data_t data;
    uint64_t timestamp;
    bool valid;
    char error_message[256];
} wiegand_result_t;

// Function declarations
int init_wiegand_test(wiegand_config_t* config);
void cleanup_wiegand_test(void);
bool configure_wiegand_pins(wiegand_config_t* config);
bool read_wiegand_data(wiegand_result_t* result, int timeout_ms);
bool simulate_wiegand_data(wiegand_data_t* data, wiegand_type_t type);

// Test functions
test_result_t test_wiegand_initialization(int data0_pin, int data1_pin);
test_result_t test_wiegand_reading(int data0_pin, int data1_pin, wiegand_type_t type);
test_result_t test_wiegand_protocols(int data0_pin, int data1_pin);
test_result_t test_wiegand_parity(int data0_pin, int data1_pin);
test_result_t test_wiegand_speed(int data0_pin, int data1_pin);
test_result_t test_wiegand_error_handling(int data0_pin, int data1_pin);

// Utility functions
void print_wiegand_data(wiegand_data_t* data);
bool validate_wiegand_parity(wiegand_data_t* data);
uint64_t calculate_wiegand_parity(uint64_t data, wiegand_type_t type);
char* wiegand_type_to_string(wiegand_type_t type);
void generate_test_wiegand_data(wiegand_data_t* data, wiegand_type_t type);

// Interactive console
void run_wiegand_test_console(wiegand_config_t* config);

// Test suite function
test_summary_t run_all_wiegand_tests(int data0_pin, int data1_pin);

// Command handler function
int handle_wiegand_commands(const char* test_type, int data0_pin, int data1_pin, bool interactive_mode);

#ifdef __cplusplus
}
#endif

#endif // WIEGAND_H
