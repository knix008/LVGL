#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

// Bluetooth interface types
typedef enum {
    BLUETOOTH_INTERFACE_BLUEZ,
    BLUETOOTH_INTERFACE_DBUS,
    BLUETOOTH_INTERFACE_HCI,
    BLUETOOTH_INTERFACE_UNKNOWN
} bluetooth_interface_t;

// Bluetooth device types
typedef enum {
    BLUETOOTH_DEVICE_PHONE,
    BLUETOOTH_DEVICE_HEADSET,
    BLUETOOTH_DEVICE_SPEAKER,
    BLUETOOTH_DEVICE_KEYBOARD,
    BLUETOOTH_DEVICE_MOUSE,
    BLUETOOTH_DEVICE_UNKNOWN
} bluetooth_device_type_t;

// Bluetooth test structure
typedef struct {
    bluetooth_interface_t interface;
    char adapter_name[64];
    char adapter_address[18];
    bool is_powered;
    bool is_discoverable;
    bool is_pairable;
    int device_count;
    bluetooth_device_type_t device_types[10];
    bool is_initialized;
} bluetooth_test_t;

// Function declarations
int init_bluetooth_test(bluetooth_test_t* bluetooth);
void cleanup_bluetooth_test(bluetooth_test_t* bluetooth);

// Test functions
test_result_t test_bluetooth_detection(bluetooth_test_t* bluetooth);
test_result_t test_bluetooth_adapter(bluetooth_test_t* bluetooth);
test_result_t test_bluetooth_power(bluetooth_test_t* bluetooth);
test_result_t test_bluetooth_discovery(bluetooth_test_t* bluetooth);
test_result_t test_bluetooth_pairing(bluetooth_test_t* bluetooth);
test_result_t test_bluetooth_connectivity(bluetooth_test_t* bluetooth);
test_result_t test_bluetooth_data_transfer(bluetooth_test_t* bluetooth);
test_result_t test_bluetooth_security(bluetooth_test_t* bluetooth);
test_result_t test_bluetooth_range(bluetooth_test_t* bluetooth);
test_result_t test_bluetooth_all_capabilities(bluetooth_test_t* bluetooth);

// Main test runner
test_summary_t run_all_bluetooth_tests(void);

// Command handler
int handle_bluetooth_commands(const char* test_type, bool interactive_mode);

#ifdef __cplusplus
}
#endif

#endif // BLUETOOTH_H
