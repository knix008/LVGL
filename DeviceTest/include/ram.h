#ifndef RAM_H
#define RAM_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

// RAM type enumeration
typedef enum {
    RAM_TYPE_DDR,
    RAM_TYPE_DDR2,
    RAM_TYPE_DDR3,
    RAM_TYPE_DDR4,
    RAM_TYPE_DDR5,
    RAM_TYPE_LPDDR,
    RAM_TYPE_LPDDR2,
    RAM_TYPE_LPDDR3,
    RAM_TYPE_LPDDR4,
    RAM_TYPE_LPDDR5,
    RAM_TYPE_UNKNOWN
} ram_type_t;

// RAM test structure
typedef struct {
    ram_type_t type;
    char model_name[256];
    size_t total_memory;
    size_t available_memory;
    int num_modules;
    double frequency;
    int channels;
    int ranks;
    bool is_initialized;
} ram_test_t;

// Function declarations
int init_ram_test(ram_test_t* ram);
void cleanup_ram_test(ram_test_t* ram);

// Test functions
test_result_t test_ram_capacity(ram_test_t* ram);
test_result_t test_ram_availability(ram_test_t* ram);
test_result_t test_ram_frequency(ram_test_t* ram);
test_result_t test_ram_bandwidth(ram_test_t* ram);
test_result_t test_ram_latency(ram_test_t* ram);
test_result_t test_ram_integrity(ram_test_t* ram);
test_result_t test_ram_stress(ram_test_t* ram);
test_result_t test_ram_pattern(ram_test_t* ram);
test_result_t test_ram_random_access(ram_test_t* ram);
test_result_t test_ram_sequential_access(ram_test_t* ram);
test_result_t test_ram_all_capabilities(ram_test_t* ram);

// Main test runner
test_summary_t run_all_ram_tests(void);

// Command handler
int handle_ram_commands(const char* test_type, bool interactive_mode);

#ifdef __cplusplus
}
#endif

#endif // RAM_H
