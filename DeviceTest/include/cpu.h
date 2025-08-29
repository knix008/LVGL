#ifndef CPU_H
#define CPU_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

// CPU architecture types
typedef enum {
    CPU_ARCH_X86,
    CPU_ARCH_X86_64,
    CPU_ARCH_ARM,
    CPU_ARCH_ARM64,
    CPU_ARCH_MIPS,
    CPU_ARCH_PPC,
    CPU_ARCH_RISC_V,
    CPU_ARCH_UNKNOWN
} cpu_architecture_t;

// CPU test structure
typedef struct {
    cpu_architecture_t architecture;
    char model_name[256];
    int num_cores;
    int num_threads;
    double base_frequency;
    double max_frequency;
    int cache_size_l1;
    int cache_size_l2;
    int cache_size_l3;
    bool is_64bit;
    bool has_sse;
    bool has_avx;
    bool has_neon;
    bool is_initialized;
} cpu_test_t;

// Function declarations
int init_cpu_test(cpu_test_t* cpu);
void cleanup_cpu_test(cpu_test_t* cpu);

// Test functions
test_result_t test_cpu_architecture(cpu_test_t* cpu);
test_result_t test_cpu_cores(cpu_test_t* cpu);
test_result_t test_cpu_frequency(cpu_test_t* cpu);
test_result_t test_cpu_cache(cpu_test_t* cpu);
test_result_t test_cpu_instructions(cpu_test_t* cpu);
test_result_t test_cpu_load(cpu_test_t* cpu);
test_result_t test_cpu_temperature(cpu_test_t* cpu);
test_result_t test_cpu_memory_bandwidth(cpu_test_t* cpu);
test_result_t test_cpu_floating_point(cpu_test_t* cpu);
test_result_t test_cpu_integer_math(cpu_test_t* cpu);
test_result_t test_cpu_crypto_performance(cpu_test_t* cpu);
test_result_t test_cpu_all_capabilities(cpu_test_t* cpu);

// Main test runner
test_summary_t run_all_cpu_tests(void);

// Command handler
int handle_cpu_commands(const char* test_type, bool interactive_mode);

#ifdef __cplusplus
}
#endif

#endif // CPU_H
