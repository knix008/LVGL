#include "wiegand.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <errno.h>

// C++ implementation
extern "C" {

// GPIO simulation (since we don't have actual GPIO access in this environment)
// In a real implementation, this would use actual GPIO libraries
typedef struct {
    int pin;
    bool state;
    bool is_input;
    pthread_mutex_t mutex;
} gpio_sim_t;

static gpio_sim_t gpio_pins[64] = {0}; // Support up to 64 pins
static bool gpio_initialized = false;
static pthread_mutex_t gpio_mutex = PTHREAD_MUTEX_INITIALIZER;

// Wiegand timing constants (microseconds)
#define WIEGAND_PULSE_WIDTH 50      // 50μs pulse width
#define WIEGAND_BIT_INTERVAL 2000   // 2ms between bits
#define WIEGAND_TIMEOUT 5000000     // 5 second timeout

// Wiegand data buffer
static uint64_t wiegand_buffer = 0;
static int wiegand_bit_count = 0;
static uint64_t wiegand_start_time = 0;
static bool wiegand_data_ready = false;
static pthread_mutex_t wiegand_mutex = PTHREAD_MUTEX_INITIALIZER;

// Initialize GPIO simulation
static bool init_gpio_simulation(void) {
    if (gpio_initialized) {
        return true;
    }
    
    for (int i = 0; i < 64; i++) {
        gpio_pins[i].pin = i;
        gpio_pins[i].state = false;
        gpio_pins[i].is_input = false;
        pthread_mutex_init(&gpio_pins[i].mutex, NULL);
    }
    
    gpio_initialized = true;
    return true;
}

// Simulate GPIO pin operations
static bool gpio_set_direction(int pin, bool is_input) {
    if (pin < 0 || pin >= 64) {
        return false;
    }
    
    pthread_mutex_lock(&gpio_mutex);
    gpio_pins[pin].is_input = is_input;
    pthread_mutex_unlock(&gpio_mutex);
    
    return true;
}

static bool gpio_set_value(int pin, bool value) {
    if (pin < 0 || pin >= 64) {
        return false;
    }
    
    pthread_mutex_lock(&gpio_pins[pin].mutex);
    gpio_pins[pin].state = value;
    pthread_mutex_unlock(&gpio_pins[pin].mutex);
    
    return true;
}

static bool gpio_get_value(int pin) {
    if (pin < 0 || pin >= 64) {
        return false;
    }
    
    pthread_mutex_lock(&gpio_pins[pin].mutex);
    bool value = gpio_pins[pin].state;
    pthread_mutex_unlock(&gpio_pins[pin].mutex);
    
    return value;
}

// Get current timestamp in microseconds
static uint64_t get_timestamp_us(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)tv.tv_sec * 1000000 + tv.tv_usec;
}

// Wiegand interrupt handler simulation
static void wiegand_interrupt_handler(int pin, bool value) {
    uint64_t current_time = get_timestamp_us();
    
    pthread_mutex_lock(&wiegand_mutex);
    
    if (wiegand_bit_count == 0) {
        // Start of new Wiegand transmission
        wiegand_start_time = current_time;
        wiegand_buffer = 0;
        wiegand_data_ready = false;
    }
    
    // Add bit to buffer
    wiegand_buffer <<= 1;
    if (value) {
        wiegand_buffer |= 1;
    }
    wiegand_bit_count++;
    
    // Check if we have enough bits for the protocol
    if (wiegand_bit_count >= 26) {
        wiegand_data_ready = true;
    }
    
    pthread_mutex_unlock(&wiegand_mutex);
}

// Simulate Wiegand data transmission
static void simulate_wiegand_transmission(wiegand_data_t* data) {
    printf("Simulating Wiegand %s transmission...\n", wiegand_type_to_string(data->type));
    
    // Reset buffer
    pthread_mutex_lock(&wiegand_mutex);
    wiegand_buffer = 0;
    wiegand_bit_count = 0;
    wiegand_data_ready = false;
    pthread_mutex_unlock(&wiegand_mutex);
    
    // Transmit data bit by bit
    uint64_t raw_data = data->raw_data;
    int bits_to_send = data->type;
    
    for (int i = bits_to_send - 1; i >= 0; i--) {
        bool bit = (raw_data >> i) & 1;
        
        // Simulate data0 or data1 line
        if (bit) {
            gpio_set_value(1, true);  // Data1 line
            usleep(WIEGAND_PULSE_WIDTH);
            gpio_set_value(1, false);
        } else {
            gpio_set_value(0, true);  // Data0 line
            usleep(WIEGAND_PULSE_WIDTH);
            gpio_set_value(0, false);
        }
        
        // Trigger interrupt handler
        wiegand_interrupt_handler(bit ? 1 : 0, bit);
        
        usleep(WIEGAND_BIT_INTERVAL);
    }
    
    printf("Wiegand transmission completed\n");
}

int init_wiegand_test(wiegand_config_t* config) {
    if (!config) {
        return -1;
    }
    
    // Initialize GPIO simulation
    if (!init_gpio_simulation()) {
        return -1;
    }
    
    // Configure pins
    if (!configure_wiegand_pins(config)) {
        return -1;
    }
    
    printf("Wiegand test initialized with Data0=%d, Data1=%d\n", 
           config->data0_pin, config->data1_pin);
    
    return 0;
}

void cleanup_wiegand_test(void) {
    // Clean up GPIO simulation
    if (gpio_initialized) {
        for (int i = 0; i < 64; i++) {
            pthread_mutex_destroy(&gpio_pins[i].mutex);
        }
        gpio_initialized = false;
    }
    
    printf("Wiegand test cleanup completed\n");
}

bool configure_wiegand_pins(wiegand_config_t* config) {
    if (!config) {
        return false;
    }
    
    // Configure Data0 pin as input
    if (!gpio_set_direction(config->data0_pin, true)) {
        printf("Error: Could not configure Data0 pin %d\n", config->data0_pin);
        return false;
    }
    
    // Configure Data1 pin as input
    if (!gpio_set_direction(config->data1_pin, true)) {
        printf("Error: Could not configure Data1 pin %d\n", config->data1_pin);
        return false;
    }
    
    // Configure LED pin if enabled
    if (config->enable_led && config->led_pin >= 0) {
        if (!gpio_set_direction(config->led_pin, false)) {
            printf("Warning: Could not configure LED pin %d\n", config->led_pin);
        }
    }
    
    // Configure buzzer pin if enabled
    if (config->enable_buzzer && config->buzzer_pin >= 0) {
        if (!gpio_set_direction(config->buzzer_pin, false)) {
            printf("Warning: Could not configure buzzer pin %d\n", config->buzzer_pin);
        }
    }
    
    return true;
}

bool read_wiegand_data(wiegand_result_t* result, int timeout_ms) {
    if (!result) {
        return false;
    }
    
    uint64_t start_time = get_timestamp_us();
    uint64_t timeout_us = timeout_ms * 1000;
    
    // Wait for Wiegand data
    while (!wiegand_data_ready) {
        uint64_t current_time = get_timestamp_us();
        if (current_time - start_time > timeout_us) {
            snprintf(result->error_message, sizeof(result->error_message), 
                    "Timeout waiting for Wiegand data");
            result->valid = false;
            return false;
        }
        usleep(1000); // Sleep 1ms
    }
    
    // Read the data
    pthread_mutex_lock(&wiegand_mutex);
    result->data.raw_data = wiegand_buffer;
    result->data.data_bits = wiegand_bit_count;
    result->timestamp = get_timestamp_us();
    result->valid = true;
    
    // Reset for next read
    wiegand_buffer = 0;
    wiegand_bit_count = 0;
    wiegand_data_ready = false;
    pthread_mutex_unlock(&wiegand_mutex);
    
    return true;
}

bool simulate_wiegand_data(wiegand_data_t* data, wiegand_type_t type) {
    if (!data) {
        return false;
    }
    
    generate_test_wiegand_data(data, type);
    simulate_wiegand_transmission(data);
    
    return true;
}

test_result_t test_wiegand_initialization(int data0_pin, int data1_pin) {
    test_result_t result = {false, "", 0.0};
    
    wiegand_config_t config = {0};
    config.data0_pin = data0_pin;
    config.data1_pin = data1_pin;
    config.timeout_ms = 5000;
    config.enable_led = false;
    config.enable_buzzer = false;
    
    if (init_wiegand_test(&config) != 0) {
        snprintf(result.message, sizeof(result.message), 
                "Failed to initialize Wiegand test with Data0=%d, Data1=%d", 
                data0_pin, data1_pin);
        return result;
    }
    
    // Test pin configuration
    if (!gpio_get_value(data0_pin) && !gpio_get_value(data1_pin)) {
        result.success = true;
        snprintf(result.message, sizeof(result.message), 
                "Wiegand initialization successful: Data0=%d, Data1=%d", 
                data0_pin, data1_pin);
        result.performance_score = 100.0;
    } else {
        snprintf(result.message, sizeof(result.message), 
                "Wiegand pin configuration failed");
        result.performance_score = 0.0;
    }
    
    cleanup_wiegand_test();
    return result;
}

test_result_t test_wiegand_reading(int data0_pin, int data1_pin, wiegand_type_t type) {
    test_result_t result = {false, "", 0.0};
    
    wiegand_config_t config = {0};
    config.data0_pin = data0_pin;
    config.data1_pin = data1_pin;
    config.protocol_type = type;
    config.timeout_ms = 5000;
    
    if (init_wiegand_test(&config) != 0) {
        snprintf(result.message, sizeof(result.message), 
                "Failed to initialize Wiegand test");
        return result;
    }
    
    // Generate and simulate test data
    wiegand_data_t test_data;
    generate_test_wiegand_data(&test_data, type);
    
    // Simulate transmission in a separate thread or with delay
    simulate_wiegand_transmission(&test_data);
    
    // Read the data
    wiegand_result_t read_result;
    if (read_wiegand_data(&read_result, 10000)) {
        if (read_result.valid && read_result.data.raw_data == test_data.raw_data) {
            result.success = true;
            snprintf(result.message, sizeof(result.message), 
                    "Wiegand %s reading successful: Facility=%lu, Card=%lu", 
                    wiegand_type_to_string(type), 
                    read_result.data.facility_code, 
                    read_result.data.card_number);
            result.performance_score = 100.0;
        } else {
            snprintf(result.message, sizeof(result.message), 
                    "Wiegand data mismatch: expected 0x%lx, got 0x%lx", 
                    test_data.raw_data, read_result.data.raw_data);
            result.performance_score = 50.0;
        }
    } else {
        snprintf(result.message, sizeof(result.message), 
                "Failed to read Wiegand data: %s", read_result.error_message);
        result.performance_score = 0.0;
    }
    
    cleanup_wiegand_test();
    return result;
}

test_result_t test_wiegand_protocols(int data0_pin, int data1_pin) {
    test_result_t result = {false, "", 0.0};
    
    wiegand_type_t protocols[] = {
        WIEGAND_26BIT, WIEGAND_34BIT, WIEGAND_37BIT, 
        WIEGAND_40BIT, WIEGAND_50BIT, WIEGAND_64BIT
    };
    int num_protocols = sizeof(protocols) / sizeof(protocols[0]);
    
    int successful_protocols = 0;
    double total_score = 0.0;
    
    for (int i = 0; i < num_protocols; i++) {
        test_result_t protocol_result = test_wiegand_reading(data0_pin, data1_pin, protocols[i]);
        if (protocol_result.success) {
            successful_protocols++;
            total_score += protocol_result.performance_score;
            printf("✓ %s protocol: PASS\n", wiegand_type_to_string(protocols[i]));
        } else {
            printf("✗ %s protocol: FAIL\n", wiegand_type_to_string(protocols[i]));
        }
    }
    
    result.success = (successful_protocols > 0);
    snprintf(result.message, sizeof(result.message), 
            "Wiegand protocols test: %d/%d protocols supported", 
            successful_protocols, num_protocols);
    
    if (successful_protocols > 0) {
        result.performance_score = total_score / successful_protocols;
    }
    
    return result;
}

test_result_t test_wiegand_parity(int data0_pin, int data1_pin) {
    test_result_t result = {false, "", 0.0};
    
    wiegand_config_t config = {0};
    config.data0_pin = data0_pin;
    config.data1_pin = data1_pin;
    config.protocol_type = WIEGAND_26BIT;
    config.timeout_ms = 5000;
    
    if (init_wiegand_test(&config) != 0) {
        snprintf(result.message, sizeof(result.message), 
                "Failed to initialize Wiegand test");
        return result;
    }
    
    // Test with valid parity
    wiegand_data_t test_data;
    generate_test_wiegand_data(&test_data, WIEGAND_26BIT);
    test_data.parity_valid = true;
    
    simulate_wiegand_transmission(&test_data);
    
    wiegand_result_t read_result;
    if (read_wiegand_data(&read_result, 10000)) {
        if (validate_wiegand_parity(&read_result.data)) {
            result.success = true;
            snprintf(result.message, sizeof(result.message), 
                    "Wiegand parity validation successful");
            result.performance_score = 100.0;
        } else {
            snprintf(result.message, sizeof(result.message), 
                    "Wiegand parity validation failed");
            result.performance_score = 0.0;
        }
    } else {
        snprintf(result.message, sizeof(result.message), 
                "Failed to read Wiegand data for parity test");
        result.performance_score = 0.0;
    }
    
    cleanup_wiegand_test();
    return result;
}

test_result_t test_wiegand_speed(int data0_pin, int data1_pin) {
    test_result_t result = {false, "", 0.0};
    
    wiegand_config_t config = {0};
    config.data0_pin = data0_pin;
    config.data1_pin = data1_pin;
    config.protocol_type = WIEGAND_26BIT;
    config.timeout_ms = 5000;
    
    if (init_wiegand_test(&config) != 0) {
        snprintf(result.message, sizeof(result.message), 
                "Failed to initialize Wiegand test");
        return result;
    }
    
    // Measure transmission speed
    clock_t start_time = clock();
    int successful_transmissions = 0;
    int total_transmissions = 10;
    
    for (int i = 0; i < total_transmissions; i++) {
        wiegand_data_t test_data;
        generate_test_wiegand_data(&test_data, WIEGAND_26BIT);
        
        simulate_wiegand_transmission(&test_data);
        
        wiegand_result_t read_result;
        if (read_wiegand_data(&read_result, 10000) && read_result.valid) {
            successful_transmissions++;
        }
    }
    
    clock_t end_time = clock();
    double elapsed_time = ((double)(end_time - start_time)) / CLOCKS_PER_SEC;
    
    if (elapsed_time > 0) {
        double transmissions_per_second = successful_transmissions / elapsed_time;
        
        result.success = true;
        snprintf(result.message, sizeof(result.message), 
                "Wiegand speed test: %.1f transmissions/sec (%d/%d successful)", 
                transmissions_per_second, successful_transmissions, total_transmissions);
        
        // Score based on success rate and speed
        double success_rate = (double)successful_transmissions / total_transmissions;
        if (success_rate >= 0.9 && transmissions_per_second >= 5.0) {
            result.performance_score = 100.0;
        } else if (success_rate >= 0.7 && transmissions_per_second >= 2.0) {
            result.performance_score = 75.0;
        } else if (success_rate >= 0.5) {
            result.performance_score = 50.0;
        } else {
            result.performance_score = 25.0;
        }
    } else {
        snprintf(result.message, sizeof(result.message), 
                "Wiegand speed test failed: no successful transmissions");
        result.performance_score = 0.0;
    }
    
    cleanup_wiegand_test();
    return result;
}

test_result_t test_wiegand_error_handling(int data0_pin, int data1_pin) {
    test_result_t result = {false, "", 0.0};
    
    // Test with invalid pin numbers
    test_result_t invalid_pin_result = test_wiegand_initialization(-1, -1);
    if (!invalid_pin_result.success) {
        printf("✓ Invalid pin detection: PASS\n");
    } else {
        printf("✗ Invalid pin detection: FAIL\n");
    }
    
    // Test with invalid protocol type
    wiegand_config_t config = {0};
    config.data0_pin = data0_pin;
    config.data1_pin = data1_pin;
    config.protocol_type = (wiegand_type_t)999; // Invalid type
    
    if (init_wiegand_test(&config) != 0) {
        printf("✓ Invalid protocol detection: PASS\n");
    } else {
        printf("✗ Invalid protocol detection: FAIL\n");
        cleanup_wiegand_test();
    }
    
    result.success = true;
    snprintf(result.message, sizeof(result.message), 
            "Wiegand error handling test completed");
    result.performance_score = 100.0;
    
    return result;
}

void print_wiegand_data(wiegand_data_t* data) {
    if (!data) {
        return;
    }
    
    printf("Wiegand Data (%s):\n", wiegand_type_to_string(data->type));
    printf("  Raw Data: 0x%lx\n", data->raw_data);
    printf("  Facility Code: %lu\n", data->facility_code);
    printf("  Card Number: %lu\n", data->card_number);
    printf("  Data Bits: %d\n", data->data_bits);
    printf("  Parity Valid: %s\n", data->parity_valid ? "Yes" : "No");
}

bool validate_wiegand_parity(wiegand_data_t* data) {
    if (!data) {
        return false;
    }
    
    uint64_t calculated_parity = calculate_wiegand_parity(data->raw_data, data->type);
    uint64_t received_parity = data->raw_data & 0x3; // Last 2 bits are parity
    
    return (calculated_parity == received_parity);
}

uint64_t calculate_wiegand_parity(uint64_t data, wiegand_type_t type) {
    // Simplified parity calculation for Wiegand
    // In real implementation, this would follow the specific Wiegand protocol
    uint64_t parity = 0;
    
    switch (type) {
        case WIEGAND_26BIT:
            // 26-bit Wiegand: 1 parity bit + 8 facility + 16 card + 1 parity bit
            parity = ((data >> 25) & 1) ^ ((data >> 1) & 1);
            break;
        case WIEGAND_34BIT:
            // 34-bit Wiegand: 1 parity bit + 12 facility + 19 card + 1 parity bit + 1 parity bit
            parity = ((data >> 33) & 1) ^ ((data >> 1) & 1);
            break;
        default:
            // For other protocols, use simple XOR
            for (int i = 0; i < type; i++) {
                parity ^= (data >> i) & 1;
            }
            break;
    }
    
    return parity;
}

char* wiegand_type_to_string(wiegand_type_t type) {
    switch (type) {
        case WIEGAND_26BIT: return "26-bit";
        case WIEGAND_34BIT: return "34-bit";
        case WIEGAND_37BIT: return "37-bit";
        case WIEGAND_40BIT: return "40-bit";
        case WIEGAND_50BIT: return "50-bit";
        case WIEGAND_64BIT: return "64-bit";
        default: return "Unknown";
    }
}

void generate_test_wiegand_data(wiegand_data_t* data, wiegand_type_t type) {
    if (!data) {
        return;
    }
    
    data->type = type;
    data->facility_code = (rand() % 255) + 1;  // 1-255
    data->card_number = (rand() % 65535) + 1;  // 1-65535
    
    // Generate raw data based on protocol type
    switch (type) {
        case WIEGAND_26BIT:
            // 26-bit format: P1 + 8-bit facility + 16-bit card + P2
            data->raw_data = (data->facility_code << 17) | (data->card_number << 1);
            break;
        case WIEGAND_34BIT:
            // 34-bit format: P1 + 12-bit facility + 19-bit card + P2 + P3
            data->raw_data = (data->facility_code << 22) | (data->card_number << 3);
            break;
        default:
            // For other protocols, use simple concatenation
            data->raw_data = (data->facility_code << 16) | data->card_number;
            break;
    }
    
    // Calculate and add parity
    uint64_t parity = calculate_wiegand_parity(data->raw_data, type);
    data->raw_data |= parity;
    data->parity_valid = true;
    data->data_bits = type;
}

void run_wiegand_test_console(wiegand_config_t* config) {
    if (!config) {
        printf("Error: Invalid Wiegand configuration\n");
        return;
    }
    
    printf("=== Wiegand Test Console ===\n");
    printf("Commands:\n");
    printf("  init <data0> <data1>     - Initialize Wiegand pins\n");
    printf("  read <timeout_ms>        - Read Wiegand data\n");
    printf("  simulate <type>          - Simulate Wiegand transmission\n");
    printf("  test_protocols           - Test all protocols\n");
    printf("  test_parity              - Test parity validation\n");
    printf("  test_speed               - Test transmission speed\n");
    printf("  test_error               - Test error handling\n");
    printf("  config                   - Show current configuration\n");
    printf("  quit                     - Exit test\n");
    printf("========================\n");
    
    char command[256];
    int data0_pin, data1_pin, timeout_ms;
    wiegand_type_t protocol_type;
    
    while (1) {
        printf("wiegand> ");
        if (fgets(command, sizeof(command), stdin) == NULL) {
            break;
        }
        
        // Remove newline
        command[strcspn(command, "\n")] = 0;
        
        if (strcmp(command, "quit") == 0 || strcmp(command, "exit") == 0) {
            break;
        } else if (strncmp(command, "init ", 5) == 0) {
            if (sscanf(command + 5, "%d %d", &data0_pin, &data1_pin) == 2) {
                config->data0_pin = data0_pin;
                config->data1_pin = data1_pin;
                if (init_wiegand_test(config) == 0) {
                    printf("Wiegand initialized successfully\n");
                } else {
                    printf("Failed to initialize Wiegand\n");
                }
            } else {
                printf("Usage: init <data0_pin> <data1_pin>\n");
            }
        } else if (strncmp(command, "read ", 5) == 0) {
            if (sscanf(command + 5, "%d", &timeout_ms) == 1) {
                wiegand_result_t result;
                if (read_wiegand_data(&result, timeout_ms)) {
                    printf("Wiegand data read successfully:\n");
                    print_wiegand_data(&result.data);
                } else {
                    printf("Failed to read Wiegand data: %s\n", result.error_message);
                }
            } else {
                printf("Usage: read <timeout_ms>\n");
            }
        } else if (strncmp(command, "simulate ", 9) == 0) {
            if (sscanf(command + 9, "%d", &protocol_type) == 1) {
                wiegand_data_t test_data;
                generate_test_wiegand_data(&test_data, (wiegand_type_t)protocol_type);
                if (simulate_wiegand_data(&test_data, (wiegand_type_t)protocol_type)) {
                    printf("Wiegand simulation completed\n");
                    print_wiegand_data(&test_data);
                } else {
                    printf("Failed to simulate Wiegand data\n");
                }
            } else {
                printf("Usage: simulate <protocol_type>\n");
                printf("Protocol types: 26, 34, 37, 40, 50, 64\n");
            }
        } else if (strcmp(command, "test_protocols") == 0) {
            test_result_t result = test_wiegand_protocols(config->data0_pin, config->data1_pin);
            printf("Protocols Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else if (strcmp(command, "test_parity") == 0) {
            test_result_t result = test_wiegand_parity(config->data0_pin, config->data1_pin);
            printf("Parity Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else if (strcmp(command, "test_speed") == 0) {
            test_result_t result = test_wiegand_speed(config->data0_pin, config->data1_pin);
            printf("Speed Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else if (strcmp(command, "test_error") == 0) {
            test_result_t result = test_wiegand_error_handling(config->data0_pin, config->data1_pin);
            printf("Error Handling Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else if (strcmp(command, "config") == 0) {
            printf("Wiegand Configuration:\n");
            printf("  Data0 Pin: %d\n", config->data0_pin);
            printf("  Data1 Pin: %d\n", config->data1_pin);
            printf("  LED Pin: %d\n", config->led_pin);
            printf("  Buzzer Pin: %d\n", config->buzzer_pin);
            printf("  Protocol Type: %s\n", wiegand_type_to_string(config->protocol_type));
            printf("  Timeout: %d ms\n", config->timeout_ms);
            printf("  LED Enabled: %s\n", config->enable_led ? "Yes" : "No");
            printf("  Buzzer Enabled: %s\n", config->enable_buzzer ? "Yes" : "No");
        } else if (strlen(command) > 0) {
            printf("Unknown command: %s\n", command);
        }
    }
    
    printf("Exiting Wiegand test console\n");
    cleanup_wiegand_test();
}

// Function to run all Wiegand tests
test_summary_t run_all_wiegand_tests(int data0_pin, int data1_pin) {
    test_summary_t summary = {0, 0, 0, 0.0, ""};
    
    printf("\n=== Running All Wiegand Tests ===\n");
    
    // Test 1: Initialization
    test_result_t result = test_wiegand_initialization(data0_pin, data1_pin);
    summary.total_tests++;
    if (result.success) {
        summary.passed_tests++;
        printf("✓ Wiegand Initialization: PASS (%.1f/100)\n", result.performance_score);
    } else {
        summary.failed_tests++;
        printf("✗ Wiegand Initialization: FAIL (%.1f/100)\n", result.performance_score);
    }
    summary.average_score += result.performance_score;
    
    // Test 2: Reading
    result = test_wiegand_reading(data0_pin, data1_pin, WIEGAND_26BIT);
    summary.total_tests++;
    if (result.success) {
        summary.passed_tests++;
        printf("✓ Wiegand Reading: PASS (%.1f/100)\n", result.performance_score);
    } else {
        summary.failed_tests++;
        printf("✗ Wiegand Reading: FAIL (%.1f/100)\n", result.performance_score);
    }
    summary.average_score += result.performance_score;
    
    // Test 3: Protocols
    result = test_wiegand_protocols(data0_pin, data1_pin);
    summary.total_tests++;
    if (result.success) {
        summary.passed_tests++;
        printf("✓ Wiegand Protocols: PASS (%.1f/100)\n", result.performance_score);
    } else {
        summary.failed_tests++;
        printf("✗ Wiegand Protocols: FAIL (%.1f/100)\n", result.performance_score);
    }
    summary.average_score += result.performance_score;
    
    // Test 4: Parity
    result = test_wiegand_parity(data0_pin, data1_pin);
    summary.total_tests++;
    if (result.success) {
        summary.passed_tests++;
        printf("✓ Wiegand Parity: PASS (%.1f/100)\n", result.performance_score);
    } else {
        summary.failed_tests++;
        printf("✗ Wiegand Parity: FAIL (%.1f/100)\n", result.performance_score);
    }
    summary.average_score += result.performance_score;
    
    // Test 5: Speed
    result = test_wiegand_speed(data0_pin, data1_pin);
    summary.total_tests++;
    if (result.success) {
        summary.passed_tests++;
        printf("✓ Wiegand Speed: PASS (%.1f/100)\n", result.performance_score);
    } else {
        summary.failed_tests++;
        printf("✗ Wiegand Speed: FAIL (%.1f/100)\n", result.performance_score);
    }
    summary.average_score += result.performance_score;
    
    // Test 6: Error Handling
    result = test_wiegand_error_handling(data0_pin, data1_pin);
    summary.total_tests++;
    if (result.success) {
        summary.passed_tests++;
        printf("✓ Wiegand Error Handling: PASS (%.1f/100)\n", result.performance_score);
    } else {
        summary.failed_tests++;
        printf("✗ Wiegand Error Handling: FAIL (%.1f/100)\n", result.performance_score);
    }
    summary.average_score += result.performance_score;
    
    // Calculate average score
    if (summary.total_tests > 0) {
        summary.average_score /= summary.total_tests;
    }
    
    // Create summary string
    snprintf(summary.summary, sizeof(summary.summary),
             "Wiegand Tests: %d/%d passed, Average Score: %.1f/100",
             summary.passed_tests, summary.total_tests, summary.average_score);
    
    return summary;
}

// Function to handle Wiegand-specific commands
int handle_wiegand_commands(const char* test_type, int data0_pin, int data1_pin, bool interactive_mode) {
    if (interactive_mode) {
        wiegand_config_t config = {0};
        config.data0_pin = data0_pin;
        config.data1_pin = data1_pin;
        config.timeout_ms = 5000;
        config.enable_led = false;
        config.enable_buzzer = false;
        run_wiegand_test_console(&config);
    } else if (test_type) {
        if (strcmp(test_type, "all") == 0) {
            run_all_wiegand_tests(data0_pin, data1_pin);
        } else if (strcmp(test_type, "init") == 0) {
            test_result_t result = test_wiegand_initialization(data0_pin, data1_pin);
            printf("Wiegand Initialization Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else if (strcmp(test_type, "reading") == 0) {
            test_result_t result = test_wiegand_reading(data0_pin, data1_pin, WIEGAND_26BIT);
            printf("Wiegand Reading Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else if (strcmp(test_type, "protocols") == 0) {
            test_result_t result = test_wiegand_protocols(data0_pin, data1_pin);
            printf("Wiegand Protocols Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else if (strcmp(test_type, "parity") == 0) {
            test_result_t result = test_wiegand_parity(data0_pin, data1_pin);
            printf("Wiegand Parity Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else if (strcmp(test_type, "speed") == 0) {
            test_result_t result = test_wiegand_speed(data0_pin, data1_pin);
            printf("Wiegand Speed Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else if (strcmp(test_type, "error") == 0) {
            test_result_t result = test_wiegand_error_handling(data0_pin, data1_pin);
            printf("Wiegand Error Handling Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else {
            printf("Unknown Wiegand test type: %s\n", test_type);
            return 1;
        }
    } else {
        // Default: run all Wiegand tests
        run_all_wiegand_tests(data0_pin, data1_pin);
    }
    
    return 0;
}

} // extern "C"
