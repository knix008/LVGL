#include "mac_input.h"
#include <string.h>
#include <stdio.h>

// Initialize MAC address structure
void mac_init(MacAddress *mac) {
    if (mac) {
        memset(mac->mac_buffer, 0, sizeof(mac->mac_buffer));
        mac->digit_count = 0;
    }
}

// Add a hex digit to the MAC address
void mac_add_digit(MacAddress *mac, char digit) {
    if (mac && mac->digit_count < 12) {
        mac->mac_buffer[mac->digit_count] = digit;
        mac->digit_count++;
    }
}

// Delete last hex digit
void mac_delete_last(MacAddress *mac) {
    if (mac && mac->digit_count > 0) {
        mac->digit_count--;
        mac->mac_buffer[mac->digit_count] = '\0';
    }
}

// Clear all input
void mac_clear(MacAddress *mac) {
    if (mac) {
        mac->digit_count = 0;
        memset(mac->mac_buffer, 0, sizeof(mac->mac_buffer));
    }
}

// Get formatted MAC address string (XX:XX:XX:XX:XX:XX)
void mac_get_formatted(const MacAddress *mac, char *output, size_t output_size) {
    if (!mac || !output || output_size < 18) {
        return;
    }
    
    int pos = 0;
    for (int i = 0; i < mac->digit_count && i < 12 && pos < (int)output_size - 1; i++) {
        output[pos++] = mac->mac_buffer[i];
        // Add colon after every 2 digits (except after the last pair)
        if (i % 2 == 1 && i < 11 && pos < (int)output_size - 1) {
            output[pos++] = ':';
        }
    }
    output[pos] = '\0';
}

// Get raw MAC address buffer (12 hex digits without colons)
const char* mac_get_raw(const MacAddress *mac) {
    return mac ? mac->mac_buffer : NULL;
}

// Get number of digits entered
int mac_get_digit_count(const MacAddress *mac) {
    return mac ? mac->digit_count : 0;
}

// Check if MAC address is complete and valid
int mac_is_valid(const MacAddress *mac) {
    return mac && mac->digit_count == 12;
}

// Validate MAC address and get error message if invalid
// Returns 1 if valid, 0 if invalid (error_msg will be set)
int mac_validate(const MacAddress *mac, char *error_msg, size_t error_msg_size) {
    if (!mac || !error_msg) {
        return 0;
    }
    
    // Check if complete
    if (mac->digit_count == 12) {
        return 1;  // Valid
    }
    
    // Generate error message
    if (mac->digit_count == 0) {
        snprintf(error_msg, error_msg_size, 
                 "Please enter a MAC address.\n\nRequired: 12 hex digits");
    } else {
        char formatted[18];
        mac_get_formatted(mac, formatted, sizeof(formatted));
        snprintf(error_msg, error_msg_size, 
                 "Incomplete MAC address:\n%s\n\nEntered: %d/12 digits\nPlease complete the address.", 
                 formatted, mac->digit_count);
    }
    
    return 0;  // Invalid
}

