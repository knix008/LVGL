#ifndef MAC_INPUT_H
#define MAC_INPUT_H

#include "lvgl/lvgl.h"

// MAC address data structure
typedef struct {
    char mac_buffer[18];  // XX:XX:XX:XX:XX:XX + null terminator
    int digit_count;      // Number of hex digits entered (0-12)
} MacAddress;

// Initialize MAC address structure
void mac_init(MacAddress *mac);

// Add a hex digit to the MAC address
void mac_add_digit(MacAddress *mac, char digit);

// Delete last hex digit
void mac_delete_last(MacAddress *mac);

// Clear all input
void mac_clear(MacAddress *mac);

// Get formatted MAC address string (XX:XX:XX:XX:XX:XX)
void mac_get_formatted(const MacAddress *mac, char *output, size_t output_size);

// Get raw MAC address buffer (12 hex digits without colons)
const char* mac_get_raw(const MacAddress *mac);

// Get number of digits entered
int mac_get_digit_count(const MacAddress *mac);

// Check if MAC address is complete and valid
int mac_is_valid(const MacAddress *mac);

// Validate MAC address and get error message if invalid
// Returns 1 if valid, 0 if invalid (error_msg will be set)
int mac_validate(const MacAddress *mac, char *error_msg, size_t error_msg_size);

#endif // MAC_INPUT_H

