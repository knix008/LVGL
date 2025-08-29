#ifndef LED_H
#define LED_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

// LED interface types
typedef enum {
    LED_INTERFACE_GPIO,
    LED_INTERFACE_I2C,
    LED_INTERFACE_SPI,
    LED_INTERFACE_PWM,
    LED_INTERFACE_UNKNOWN
} led_interface_t;

// LED test structure
typedef struct {
    led_interface_t interface;
    char device_path[64];
    int led1_pin;
    int led2_pin;
    bool led1_available;
    bool led2_available;
    bool is_initialized;
} led_test_t;

// Function declarations
int init_led_test(led_test_t* led, int led1_pin, int led2_pin);
void cleanup_led_test(led_test_t* led);

// Test functions
test_result_t test_led_detection(led_test_t* led);
test_result_t test_led1_control(led_test_t* led);
test_result_t test_led2_control(led_test_t* led);
test_result_t test_led_brightness(led_test_t* led);
test_result_t test_led_blink(led_test_t* led);
test_result_t test_led_pattern(led_test_t* led);
test_result_t test_led_synchronization(led_test_t* led);
test_result_t test_led_all_capabilities(led_test_t* led);

// Main test runner
test_summary_t run_all_led_tests(int led1_pin, int led2_pin);

// Command handler
int handle_led_commands(const char* test_type, int led1_pin, int led2_pin, bool interactive_mode);

#ifdef __cplusplus
}
#endif

#endif // LED_H
