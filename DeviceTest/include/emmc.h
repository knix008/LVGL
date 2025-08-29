#ifndef EMMC_H
#define EMMC_H

#include "common.h"
#include <stddef.h> // for size_t

#ifdef __cplusplus
extern "C" {
#endif

// eMMC interface types
typedef enum {
    EMMC_INTERFACE_MMC,
    EMMC_INTERFACE_SDIO,
    EMMC_INTERFACE_UNKNOWN
} emmc_interface_t;

// eMMC test structure
typedef struct {
    emmc_interface_t interface;
    char device_path[64];
    char model_name[256];
    char serial_number[64];
    char firmware_version[32];
    char manufacturer[64];
    size_t total_size;
    size_t available_size;
    int block_size;
    int max_speed_class;
    bool is_initialized;
    bool is_mounted;
} emmc_test_t;

// Function declarations
int init_emmc_test(emmc_test_t* emmc, const char* device_path);
void cleanup_emmc_test(emmc_test_t* emmc);

// Test functions
test_result_t test_emmc_detection(emmc_test_t* emmc);
test_result_t test_emmc_capacity(emmc_test_t* emmc);
test_result_t test_emmc_read_speed(emmc_test_t* emmc);
test_result_t test_emmc_write_speed(emmc_test_t* emmc);
test_result_t test_emmc_random_access(emmc_test_t* emmc);
test_result_t test_emmc_sequential_access(emmc_test_t* emmc);
test_result_t test_emmc_integrity(emmc_test_t* emmc);
test_result_t test_emmc_io_operations(emmc_test_t* emmc);
test_result_t test_emmc_health(emmc_test_t* emmc);
test_result_t test_emmc_temperature(emmc_test_t* emmc);
test_result_t test_emmc_all_capabilities(emmc_test_t* emmc);

// Main test runner
test_summary_t run_all_emmc_tests(const char* device_path);

// Command handler
int handle_emmc_commands(const char* test_type, const char* device_path, bool interactive_mode);

#ifdef __cplusplus
}
#endif

#endif // EMMC_H
