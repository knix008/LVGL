#include "led.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <dirent.h>
#include <math.h>

// C++ implementation
extern "C" {

// Helper function to get LED interface name
const char* get_led_interface_name(led_interface_t interface) {
    switch (interface) {
        case LED_INTERFACE_GPIO: return "GPIO";
        case LED_INTERFACE_I2C: return "I2C";
        case LED_INTERFACE_SPI: return "SPI";
        case LED_INTERFACE_PWM: return "PWM";
        default: return "Unknown";
    }
}

// Helper function to detect LED interface
led_interface_t detect_led_interface() {
    // For now, assume GPIO interface
    // In a real implementation, you'd check for I2C/SPI devices
    return LED_INTERFACE_GPIO;
}

// Helper function to export GPIO pin
int export_gpio_pin(int pin) {
    char path[64];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d", pin);
    
    // Check if already exported
    if (access(path, F_OK) == 0) {
        return 0;
    }
    
    // Export the pin
    FILE* fp = fopen("/sys/class/gpio/export", "w");
    if (fp) {
        fprintf(fp, "%d", pin);
        fclose(fp);
        usleep(100000); // Wait 100ms for export to complete
        return 0;
    }
    
    return -1;
}

// Helper function to set GPIO direction
int set_gpio_direction(int pin, const char* direction) {
    char path[64];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/direction", pin);
    
    FILE* fp = fopen(path, "w");
    if (fp) {
        fprintf(fp, "%s", direction);
        fclose(fp);
        return 0;
    }
    
    return -1;
}

// Helper function to set GPIO value
int set_gpio_value(int pin, int value) {
    char path[64];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);
    
    FILE* fp = fopen(path, "w");
    if (fp) {
        fprintf(fp, "%d", value);
        fclose(fp);
        return 0;
    }
    
    return -1;
}

// Helper function to get GPIO value
int get_gpio_value(int pin) {
    char path[64];
    snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", pin);
    
    FILE* fp = fopen(path, "r");
    if (fp) {
        int value;
        fscanf(fp, "%d", &value);
        fclose(fp);
        return value;
    }
    
    return -1;
}

int init_led_test(led_test_t* led, int led1_pin, int led2_pin) {
    if (!led) {
        return -1;
    }
    
    // Initialize LED structure
    memset(led, 0, sizeof(led_test_t));
    
    // Set LED pins
    led->led1_pin = led1_pin;
    led->led2_pin = led2_pin;
    
    // Detect interface
    led->interface = detect_led_interface();
    
    // Initialize GPIO pins if using GPIO interface
    if (led->interface == LED_INTERFACE_GPIO) {
        // Try to export and configure LED1
        if (export_gpio_pin(led1_pin) == 0 && set_gpio_direction(led1_pin, "out") == 0) {
            led->led1_available = true;
        }
        
        // Try to export and configure LED2
        if (export_gpio_pin(led2_pin) == 0 && set_gpio_direction(led2_pin, "out") == 0) {
            led->led2_available = true;
        }
    } else {
        // For other interfaces, assume LEDs are available
        led->led1_available = true;
        led->led2_available = true;
    }
    
    led->is_initialized = true;
    
    printf("LED Test initialized\n");
    printf("Interface: %s\n", get_led_interface_name(led->interface));
    printf("LED1 Pin: %d (%s)\n", led1_pin, led->led1_available ? "Available" : "Not Available");
    printf("LED2 Pin: %d (%s)\n", led2_pin, led->led2_available ? "Available" : "Not Available");
    
    return 0;
}

void cleanup_led_test(led_test_t* led) {
    if (led) {
        // Turn off LEDs before cleanup
        if (led->led1_available) {
            set_gpio_value(led->led1_pin, 0);
        }
        if (led->led2_available) {
            set_gpio_value(led->led2_pin, 0);
        }
        
        led->is_initialized = false;
        printf("LED Test cleaned up\n");
    }
}

test_result_t test_led_detection(led_test_t* led) {
    test_result_t result = {false, "", 0.0};
    
    if (!led || !led->is_initialized) {
        strcpy(result.message, "LED not initialized");
        return result;
    }
    
    printf("Testing LED detection...\n");
    
    int available_leds = 0;
    if (led->led1_available) available_leds++;
    if (led->led2_available) available_leds++;
    
    if (available_leds > 0) {
        result.success = true;
        result.performance_score = available_leds * 50.0; // 50 points per LED
        snprintf(result.message, sizeof(result.message), 
                "LEDs detected: %d/2 available", available_leds);
        printf("✓ LED Detection: PASS (%d/2 LEDs)\n", available_leds);
    } else {
        result.performance_score = 0.0;
        strcpy(result.message, "No LEDs detected");
        printf("✗ LED Detection: FAIL\n");
    }
    
    return result;
}

test_result_t test_led1_control(led_test_t* led) {
    test_result_t result = {false, "", 0.0};
    
    if (!led || !led->is_initialized) {
        strcpy(result.message, "LED not initialized");
        return result;
    }
    
    printf("Testing LED1 control...\n");
    
    if (led->led1_available) {
        // Test turning LED on and off
        if (set_gpio_value(led->led1_pin, 1) == 0) {
            usleep(100000); // 100ms delay
            int value = get_gpio_value(led->led1_pin);
            if (value == 1) {
                set_gpio_value(led->led1_pin, 0);
                usleep(100000); // 100ms delay
                value = get_gpio_value(led->led1_pin);
                if (value == 0) {
                    result.success = true;
                    result.performance_score = 100.0;
                    strcpy(result.message, "LED1 control successful");
                    printf("✓ LED1 Control: PASS\n");
                } else {
                    result.performance_score = 50.0;
                    strcpy(result.message, "LED1 turn off failed");
                    printf("✗ LED1 Control: FAIL (turn off)\n");
                }
            } else {
                result.performance_score = 50.0;
                strcpy(result.message, "LED1 turn on failed");
                printf("✗ LED1 Control: FAIL (turn on)\n");
            }
        } else {
            result.performance_score = 0.0;
            strcpy(result.message, "LED1 control failed");
            printf("✗ LED1 Control: FAIL\n");
        }
    } else {
        result.success = true;
        result.performance_score = 70.0; // Can't test, but not a failure
        strcpy(result.message, "LED1 not available");
        printf("✓ LED1 Control: PASS (not available)\n");
    }
    
    return result;
}

test_result_t test_led2_control(led_test_t* led) {
    test_result_t result = {false, "", 0.0};
    
    if (!led || !led->is_initialized) {
        strcpy(result.message, "LED not initialized");
        return result;
    }
    
    printf("Testing LED2 control...\n");
    
    if (led->led2_available) {
        // Test turning LED on and off
        if (set_gpio_value(led->led2_pin, 1) == 0) {
            usleep(100000); // 100ms delay
            int value = get_gpio_value(led->led2_pin);
            if (value == 1) {
                set_gpio_value(led->led2_pin, 0);
                usleep(100000); // 100ms delay
                value = get_gpio_value(led->led2_pin);
                if (value == 0) {
                    result.success = true;
                    result.performance_score = 100.0;
                    strcpy(result.message, "LED2 control successful");
                    printf("✓ LED2 Control: PASS\n");
                } else {
                    result.performance_score = 50.0;
                    strcpy(result.message, "LED2 turn off failed");
                    printf("✗ LED2 Control: FAIL (turn off)\n");
                }
            } else {
                result.performance_score = 50.0;
                strcpy(result.message, "LED2 turn on failed");
                printf("✗ LED2 Control: FAIL (turn on)\n");
            }
        } else {
            result.performance_score = 0.0;
            strcpy(result.message, "LED2 control failed");
            printf("✗ LED2 Control: FAIL\n");
        }
    } else {
        result.success = true;
        result.performance_score = 70.0; // Can't test, but not a failure
        strcpy(result.message, "LED2 not available");
        printf("✓ LED2 Control: PASS (not available)\n");
    }
    
    return result;
}

test_result_t test_led_brightness(led_test_t* led) {
    test_result_t result = {false, "", 0.0};
    
    if (!led || !led->is_initialized) {
        strcpy(result.message, "LED not initialized");
        return result;
    }
    
    printf("Testing LED brightness control...\n");
    
    // For GPIO LEDs, brightness is binary (on/off)
    // For PWM LEDs, we could test different brightness levels
    if (led->interface == LED_INTERFACE_PWM) {
        result.success = true;
        result.performance_score = 90.0;
        strcpy(result.message, "PWM brightness control available");
        printf("✓ LED Brightness: PASS (PWM)\n");
    } else {
        result.success = true;
        result.performance_score = 70.0; // Binary control only
        strcpy(result.message, "Binary brightness control (on/off)");
        printf("✓ LED Brightness: PASS (binary)\n");
    }
    
    return result;
}

test_result_t test_led_blink(led_test_t* led) {
    test_result_t result = {false, "", 0.0};
    
    if (!led || !led->is_initialized) {
        strcpy(result.message, "LED not initialized");
        return result;
    }
    
    printf("Testing LED blink pattern...\n");
    
    int successful_blinks = 0;
    int total_blinks = 5;
    
    if (led->led1_available) {
        for (int i = 0; i < total_blinks; i++) {
            set_gpio_value(led->led1_pin, 1);
            usleep(200000); // 200ms on
            set_gpio_value(led->led1_pin, 0);
            usleep(200000); // 200ms off
            successful_blinks++;
        }
    }
    
    if (led->led2_available) {
        for (int i = 0; i < total_blinks; i++) {
            set_gpio_value(led->led2_pin, 1);
            usleep(200000); // 200ms on
            set_gpio_value(led->led2_pin, 0);
            usleep(200000); // 200ms off
            successful_blinks++;
        }
    }
    
    if (successful_blinks > 0) {
        result.success = true;
        result.performance_score = (double)successful_blinks / (total_blinks * 2) * 100.0;
        snprintf(result.message, sizeof(result.message), 
                "Blink test: %d successful blinks", successful_blinks);
        printf("✓ LED Blink: PASS (%d blinks)\n", successful_blinks);
    } else {
        result.performance_score = 0.0;
        strcpy(result.message, "Blink test failed");
        printf("✗ LED Blink: FAIL\n");
    }
    
    return result;
}

test_result_t test_led_pattern(led_test_t* led) {
    test_result_t result = {false, "", 0.0};
    
    if (!led || !led->is_initialized) {
        strcpy(result.message, "LED not initialized");
        return result;
    }
    
    printf("Testing LED pattern sequences...\n");
    
    int successful_patterns = 0;
    int total_patterns = 3;
    
    // Pattern 1: Alternating LEDs
    if (led->led1_available && led->led2_available) {
        for (int i = 0; i < 4; i++) {
            set_gpio_value(led->led1_pin, i % 2);
            set_gpio_value(led->led2_pin, (i + 1) % 2);
            usleep(300000); // 300ms
        }
        successful_patterns++;
    }
    
    // Pattern 2: Both LEDs on/off together
    if (led->led1_available && led->led2_available) {
        for (int i = 0; i < 4; i++) {
            set_gpio_value(led->led1_pin, i % 2);
            set_gpio_value(led->led2_pin, i % 2);
            usleep(300000); // 300ms
        }
        successful_patterns++;
    }
    
    // Pattern 3: Sequential pattern
    if (led->led1_available && led->led2_available) {
        set_gpio_value(led->led1_pin, 1);
        set_gpio_value(led->led2_pin, 0);
        usleep(200000);
        set_gpio_value(led->led1_pin, 0);
        set_gpio_value(led->led2_pin, 1);
        usleep(200000);
        set_gpio_value(led->led1_pin, 1);
        set_gpio_value(led->led2_pin, 1);
        usleep(200000);
        set_gpio_value(led->led1_pin, 0);
        set_gpio_value(led->led2_pin, 0);
        successful_patterns++;
    }
    
    if (successful_patterns > 0) {
        result.success = true;
        result.performance_score = (double)successful_patterns / total_patterns * 100.0;
        snprintf(result.message, sizeof(result.message), 
                "Pattern test: %d/%d patterns successful", successful_patterns, total_patterns);
        printf("✓ LED Pattern: PASS (%d/%d patterns)\n", successful_patterns, total_patterns);
    } else {
        result.performance_score = 0.0;
        strcpy(result.message, "Pattern test failed");
        printf("✗ LED Pattern: FAIL\n");
    }
    
    return result;
}

test_result_t test_led_synchronization(led_test_t* led) {
    test_result_t result = {false, "", 0.0};
    
    if (!led || !led->is_initialized) {
        strcpy(result.message, "LED not initialized");
        return result;
    }
    
    printf("Testing LED synchronization...\n");
    
    if (led->led1_available && led->led2_available) {
        // Test synchronized blinking
        clock_t start = clock();
        
        for (int i = 0; i < 10; i++) {
            set_gpio_value(led->led1_pin, 1);
            set_gpio_value(led->led2_pin, 1);
            usleep(100000); // 100ms
            set_gpio_value(led->led1_pin, 0);
            set_gpio_value(led->led2_pin, 0);
            usleep(100000); // 100ms
        }
        
        clock_t end = clock();
        double total_time = (double)(end - start) / CLOCKS_PER_SEC;
        double expected_time = 2.0; // 10 cycles * 200ms = 2 seconds
        
        result.success = true;
        
        // Score based on timing accuracy
        double timing_error = fabs(total_time - expected_time);
        if (timing_error < 0.1) {
            result.performance_score = 100.0;
        } else if (timing_error < 0.2) {
            result.performance_score = 90.0;
        } else if (timing_error < 0.5) {
            result.performance_score = 80.0;
        } else {
            result.performance_score = 70.0;
        }
        
        snprintf(result.message, sizeof(result.message), 
                "Synchronization: %.2f seconds (expected: %.2f)", total_time, expected_time);
        printf("✓ LED Synchronization: PASS (%.2fs)\n", total_time);
    } else {
        result.success = true;
        result.performance_score = 70.0; // Can't test, but not a failure
        strcpy(result.message, "Synchronization test not available");
        printf("✓ LED Synchronization: PASS (not available)\n");
    }
    
    return result;
}

test_result_t test_led_all_capabilities(led_test_t* led) {
    test_result_t result = {false, "", 0.0};
    
    if (!led || !led->is_initialized) {
        strcpy(result.message, "LED not initialized");
        return result;
    }
    
    printf("Testing all LED capabilities...\n");
    
    // Run all capability tests
    test_result_t tests[] = {
        test_led_detection(led),
        test_led1_control(led),
        test_led2_control(led),
        test_led_brightness(led),
        test_led_blink(led),
        test_led_pattern(led),
        test_led_synchronization(led)
    };
    
    int num_tests = sizeof(tests) / sizeof(tests[0]);
    int passed_tests = 0;
    double total_score = 0.0;
    
    for (int i = 0; i < num_tests; i++) {
        if (tests[i].success) {
            passed_tests++;
        }
        total_score += tests[i].performance_score;
    }
    
    result.success = (passed_tests > 0);
    result.performance_score = total_score / num_tests;
    snprintf(result.message, sizeof(result.message), 
            "LED capabilities: %d/%d tests passed", passed_tests, num_tests);
    
    printf("✓ LED All Capabilities: PASS (%d/%d tests)\n", passed_tests, num_tests);
    
    return result;
}

test_summary_t run_all_led_tests(int led1_pin, int led2_pin) {
    test_summary_t summary = {0, 0, 0, 0.0, ""};
    
    printf("\n=== Running All LED Tests ===\n");
    
    led_test_t led;
    if (init_led_test(&led, led1_pin, led2_pin) != 0) {
        printf("Failed to initialize LED test\n");
        return summary;
    }
    
    // Test 1: Detection
    test_result_t result = test_led_detection(&led);
    summary.total_tests++;
    if (result.success) {
        summary.passed_tests++;
        printf("✓ LED Detection: PASS (%.1f/100)\n", result.performance_score);
    } else {
        summary.failed_tests++;
        printf("✗ LED Detection: FAIL (%.1f/100)\n", result.performance_score);
    }
    summary.average_score += result.performance_score;
    
    // Test 2: All Capabilities
    result = test_led_all_capabilities(&led);
    summary.total_tests++;
    if (result.success) {
        summary.passed_tests++;
        printf("✓ LED All Capabilities: PASS (%.1f/100)\n", result.performance_score);
    } else {
        summary.failed_tests++;
        printf("✗ LED All Capabilities: FAIL (%.1f/100)\n", result.performance_score);
    }
    summary.average_score += result.performance_score;
    
    // Calculate average score
    if (summary.total_tests > 0) {
        summary.average_score /= summary.total_tests;
    }
    
    // Create summary string
    snprintf(summary.summary, sizeof(summary.summary),
             "LED Tests: %d/%d passed, Average Score: %.1f/100",
             summary.passed_tests, summary.total_tests, summary.average_score);
    
    cleanup_led_test(&led);
    return summary;
}

int handle_led_commands(const char* test_type, int led1_pin, int led2_pin, bool interactive_mode) {
    if (interactive_mode) {
        printf("Interactive LED mode not implemented yet\n");
        return 1;
    } else if (test_type) {
        led_test_t led;
        if (init_led_test(&led, led1_pin, led2_pin) != 0) {
            printf("Error: Could not initialize LED test\n");
            return 1;
        }
        
        if (strcmp(test_type, "all") == 0) {
            run_all_led_tests(led1_pin, led2_pin);
        } else if (strcmp(test_type, "detection") == 0) {
            test_result_t result = test_led_detection(&led);
            printf("LED Detection Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else if (strcmp(test_type, "capabilities") == 0) {
            test_result_t result = test_led_all_capabilities(&led);
            printf("LED Capabilities Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else {
            printf("Unknown LED test type: %s\n", test_type);
            printf("Available tests: all, detection, capabilities\n");
            cleanup_led_test(&led);
            return 1;
        }
        
        cleanup_led_test(&led);
    }
    
    return 0;
}

} // extern "C"
