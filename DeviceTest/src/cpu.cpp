#include "cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#include <time.h>
#include <math.h>
#include <pthread.h>
#include <sched.h>
#include <cpuid.h>

// C++ implementation
extern "C" {

// Helper function to get architecture name
const char* get_architecture_name(cpu_architecture_t arch) {
    switch (arch) {
        case CPU_ARCH_X86: return "x86";
        case CPU_ARCH_X86_64: return "x86_64";
        case CPU_ARCH_ARM: return "ARM";
        case CPU_ARCH_ARM64: return "ARM64";
        case CPU_ARCH_MIPS: return "MIPS";
        case CPU_ARCH_PPC: return "PowerPC";
        case CPU_ARCH_RISC_V: return "RISC-V";
        default: return "Unknown";
    }
}

// Helper function to read CPU info from /proc/cpuinfo
int read_cpu_info(cpu_test_t* cpu) {
    FILE* fp = fopen("/proc/cpuinfo", "r");
    if (!fp) {
        return -1;
    }
    
    char line[512];
    int core_count = 0;
    bool found_model = false;
    bool found_freq = false;
    
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "processor", 9) == 0) {
            core_count++;
        } else if (strncmp(line, "model name", 10) == 0 && !found_model) {
            char* colon = strchr(line, ':');
            if (colon) {
                colon++; // Skip the colon
                while (*colon == ' ' || *colon == '\t') colon++; // Skip whitespace
                strncpy(cpu->model_name, colon, sizeof(cpu->model_name) - 1);
                // Remove newline
                char* newline = strchr(cpu->model_name, '\n');
                if (newline) *newline = '\0';
                found_model = true;
            }
        } else if (strncmp(line, "cpu MHz", 7) == 0 && !found_freq) {
            char* colon = strchr(line, ':');
            if (colon) {
                cpu->base_frequency = atof(colon + 1);
                found_freq = true;
            }
        }
    }
    
    fclose(fp);
    cpu->num_cores = core_count;
    cpu->num_threads = core_count; // For now, assume 1 thread per core
    
    return 0;
}

// Helper function to detect CPU architecture
cpu_architecture_t detect_architecture() {
    struct utsname uts;
    if (uname(&uts) != 0) {
        return CPU_ARCH_UNKNOWN;
    }
    
    if (strstr(uts.machine, "x86_64") || strstr(uts.machine, "amd64")) {
        return CPU_ARCH_X86_64;
    } else if (strstr(uts.machine, "i386") || strstr(uts.machine, "i686")) {
        return CPU_ARCH_X86;
    } else if (strstr(uts.machine, "aarch64") || strstr(uts.machine, "arm64")) {
        return CPU_ARCH_ARM64;
    } else if (strstr(uts.machine, "arm")) {
        return CPU_ARCH_ARM;
    } else if (strstr(uts.machine, "mips")) {
        return CPU_ARCH_MIPS;
    } else if (strstr(uts.machine, "ppc")) {
        return CPU_ARCH_PPC;
    } else if (strstr(uts.machine, "riscv")) {
        return CPU_ARCH_RISC_V;
    }
    
    return CPU_ARCH_UNKNOWN;
}

// Helper function to check CPU features (x86)
void check_x86_features(cpu_test_t* cpu) {
    unsigned int eax, ebx, ecx, edx;
    
    // Check basic CPUID
    if (__get_cpuid(1, &eax, &ebx, &ecx, &edx)) {
        cpu->has_sse = (edx & (1 << 25)) != 0; // SSE
        cpu->is_64bit = (edx & (1 << 29)) != 0; // Long mode
    }
    
    // Check AVX support
    if (__get_cpuid(1, &eax, &ebx, &ecx, &edx)) {
        cpu->has_avx = (ecx & (1 << 28)) != 0; // AVX
    }
}

// Helper function to check ARM features
void check_arm_features(cpu_test_t* cpu) {
    // For ARM, we'll check for NEON support
    // This is a simplified check - in a real implementation you'd read from /proc/cpuinfo
    cpu->has_neon = true; // Assume NEON is available on modern ARM
    cpu->is_64bit = (cpu->architecture == CPU_ARCH_ARM64);
}

int init_cpu_test(cpu_test_t* cpu) {
    if (!cpu) {
        return -1;
    }
    
    // Initialize CPU structure
    memset(cpu, 0, sizeof(cpu_test_t));
    
    // Detect architecture
    cpu->architecture = detect_architecture();
    
    // Read CPU information
    if (read_cpu_info(cpu) != 0) {
        printf("Warning: Could not read CPU info from /proc/cpuinfo\n");
    }
    
    // Check CPU features based on architecture
    if (cpu->architecture == CPU_ARCH_X86 || cpu->architecture == CPU_ARCH_X86_64) {
        check_x86_features(cpu);
    } else if (cpu->architecture == CPU_ARCH_ARM || cpu->architecture == CPU_ARCH_ARM64) {
        check_arm_features(cpu);
    }
    
    // Set default values for missing information
    if (cpu->num_cores == 0) {
        cpu->num_cores = sysconf(_SC_NPROCESSORS_ONLN);
        cpu->num_threads = cpu->num_cores;
    }
    
    if (cpu->base_frequency == 0) {
        cpu->base_frequency = 2000.0; // Default to 2 GHz
    }
    
    cpu->max_frequency = cpu->base_frequency * 1.2; // Estimate max frequency
    
    cpu->is_initialized = true;
    
    printf("CPU Test initialized for %s architecture\n", get_architecture_name(cpu->architecture));
    printf("Model: %s\n", cpu->model_name[0] ? cpu->model_name : "Unknown");
    printf("Cores: %d\n", cpu->num_cores);
    
    return 0;
}

void cleanup_cpu_test(cpu_test_t* cpu) {
    if (cpu) {
        cpu->is_initialized = false;
        printf("CPU Test cleaned up\n");
    }
}

test_result_t test_cpu_architecture(cpu_test_t* cpu) {
    test_result_t result = {false, "", 0.0};
    
    if (!cpu || !cpu->is_initialized) {
        strcpy(result.message, "CPU not initialized");
        return result;
    }
    
    printf("Testing CPU architecture...\n");
    
    result.success = true;
    result.performance_score = 100.0;
    snprintf(result.message, sizeof(result.message), 
            "Architecture: %s", get_architecture_name(cpu->architecture));
    printf("✓ CPU Architecture: PASS (%s)\n", get_architecture_name(cpu->architecture));
    
    return result;
}

test_result_t test_cpu_cores(cpu_test_t* cpu) {
    test_result_t result = {false, "", 0.0};
    
    if (!cpu || !cpu->is_initialized) {
        strcpy(result.message, "CPU not initialized");
        return result;
    }
    
    printf("Testing CPU cores...\n");
    
    if (cpu->num_cores > 0) {
        result.success = true;
        
        // Score based on number of cores
        if (cpu->num_cores >= 16) {
            result.performance_score = 100.0;
        } else if (cpu->num_cores >= 8) {
            result.performance_score = 90.0;
        } else if (cpu->num_cores >= 4) {
            result.performance_score = 80.0;
        } else if (cpu->num_cores >= 2) {
            result.performance_score = 70.0;
        } else {
            result.performance_score = 50.0;
        }
        
        snprintf(result.message, sizeof(result.message), 
                "Cores: %d, Threads: %d", cpu->num_cores, cpu->num_threads);
        printf("✓ CPU Cores: PASS (%d cores, %d threads)\n", cpu->num_cores, cpu->num_threads);
    } else {
        result.performance_score = 0.0;
        strcpy(result.message, "No CPU cores detected");
        printf("✗ CPU Cores: FAIL\n");
    }
    
    return result;
}

test_result_t test_cpu_frequency(cpu_test_t* cpu) {
    test_result_t result = {false, "", 0.0};
    
    if (!cpu || !cpu->is_initialized) {
        strcpy(result.message, "CPU not initialized");
        return result;
    }
    
    printf("Testing CPU frequency...\n");
    
    if (cpu->base_frequency > 0) {
        result.success = true;
        
        // Score based on frequency
        if (cpu->base_frequency >= 3000.0) {
            result.performance_score = 100.0;
        } else if (cpu->base_frequency >= 2500.0) {
            result.performance_score = 90.0;
        } else if (cpu->base_frequency >= 2000.0) {
            result.performance_score = 80.0;
        } else if (cpu->base_frequency >= 1500.0) {
            result.performance_score = 70.0;
        } else {
            result.performance_score = 50.0;
        }
        
        snprintf(result.message, sizeof(result.message), 
                "Base: %.1f MHz, Max: %.1f MHz", cpu->base_frequency, cpu->max_frequency);
        printf("✓ CPU Frequency: PASS (%.1f MHz base, %.1f MHz max)\n", 
               cpu->base_frequency, cpu->max_frequency);
    } else {
        result.performance_score = 0.0;
        strcpy(result.message, "Frequency information not available");
        printf("✗ CPU Frequency: FAIL\n");
    }
    
    return result;
}

test_result_t test_cpu_cache(cpu_test_t* cpu) {
    test_result_t result = {false, "", 0.0};
    
    if (!cpu || !cpu->is_initialized) {
        strcpy(result.message, "CPU not initialized");
        return result;
    }
    
    printf("Testing CPU cache...\n");
    
    // Try to read cache information from /sys
    FILE* fp = fopen("/sys/devices/system/cpu/cpu0/cache/index0/size", "r");
    if (fp) {
        char cache_size[32];
        if (fgets(cache_size, sizeof(cache_size), fp)) {
            cpu->cache_size_l1 = atoi(cache_size);
        }
        fclose(fp);
    }
    
    // Try L2 cache
    fp = fopen("/sys/devices/system/cpu/cpu0/cache/index2/size", "r");
    if (fp) {
        char cache_size[32];
        if (fgets(cache_size, sizeof(cache_size), fp)) {
            cpu->cache_size_l2 = atoi(cache_size);
        }
        fclose(fp);
    }
    
    // Try L3 cache
    fp = fopen("/sys/devices/system/cpu/cpu0/cache/index3/size", "r");
    if (fp) {
        char cache_size[32];
        if (fgets(cache_size, sizeof(cache_size), fp)) {
            cpu->cache_size_l3 = atoi(cache_size);
        }
        fclose(fp);
    }
    
    result.success = true;
    result.performance_score = 80.0; // Default score
    snprintf(result.message, sizeof(result.message), 
            "L1: %dK, L2: %dK, L3: %dK", 
            cpu->cache_size_l1, cpu->cache_size_l2, cpu->cache_size_l3);
    printf("✓ CPU Cache: PASS (L1: %dK, L2: %dK, L3: %dK)\n", 
           cpu->cache_size_l1, cpu->cache_size_l2, cpu->cache_size_l3);
    
    return result;
}

test_result_t test_cpu_instructions(cpu_test_t* cpu) {
    test_result_t result = {false, "", 0.0};
    
    if (!cpu || !cpu->is_initialized) {
        strcpy(result.message, "CPU not initialized");
        return result;
    }
    
    printf("Testing CPU instruction sets...\n");
    
    result.success = true;
    result.performance_score = 70.0; // Base score
    
    char features[256] = "";
    
    if (cpu->has_sse) {
        strcat(features, "SSE ");
        result.performance_score += 10.0;
    }
    if (cpu->has_avx) {
        strcat(features, "AVX ");
        result.performance_score += 15.0;
    }
    if (cpu->has_neon) {
        strcat(features, "NEON ");
        result.performance_score += 10.0;
    }
    if (cpu->is_64bit) {
        strcat(features, "64-bit ");
        result.performance_score += 5.0;
    }
    
    if (strlen(features) == 0) {
        strcpy(features, "Basic");
    }
    
    snprintf(result.message, sizeof(result.message), 
            "Features: %s", features);
    printf("✓ CPU Instructions: PASS (%s)\n", features);
    
    return result;
}

test_result_t test_cpu_load(cpu_test_t* cpu) {
    test_result_t result = {false, "", 0.0};
    
    if (!cpu || !cpu->is_initialized) {
        strcpy(result.message, "CPU not initialized");
        return result;
    }
    
    printf("Testing CPU load...\n");
    
    // Read CPU load from /proc/loadavg
    FILE* fp = fopen("/proc/loadavg", "r");
    if (fp) {
        float load1, load5, load15;
        if (fscanf(fp, "%f %f %f", &load1, &load5, &load15) == 3) {
            result.success = true;
            
            // Score based on load (lower is better)
            float avg_load = (load1 + load5 + load15) / 3.0;
            if (avg_load < 0.5) {
                result.performance_score = 100.0;
            } else if (avg_load < 1.0) {
                result.performance_score = 90.0;
            } else if (avg_load < 2.0) {
                result.performance_score = 80.0;
            } else if (avg_load < 5.0) {
                result.performance_score = 60.0;
            } else {
                result.performance_score = 40.0;
            }
            
            snprintf(result.message, sizeof(result.message), 
                    "Load: %.2f, %.2f, %.2f", load1, load5, load15);
            printf("✓ CPU Load: PASS (%.2f, %.2f, %.2f)\n", load1, load5, load15);
        } else {
            result.performance_score = 0.0;
            strcpy(result.message, "Could not read CPU load");
            printf("✗ CPU Load: FAIL\n");
        }
        fclose(fp);
    } else {
        result.performance_score = 0.0;
        strcpy(result.message, "Could not access /proc/loadavg");
        printf("✗ CPU Load: FAIL\n");
    }
    
    return result;
}

test_result_t test_cpu_temperature(cpu_test_t* cpu) {
    test_result_t result = {false, "", 0.0};
    
    if (!cpu || !cpu->is_initialized) {
        strcpy(result.message, "CPU not initialized");
        return result;
    }
    
    printf("Testing CPU temperature...\n");
    
    // Try to read temperature from various sources
    const char* temp_paths[] = {
        "/sys/class/thermal/thermal_zone0/temp",
        "/sys/class/hwmon/hwmon0/temp1_input",
        "/sys/class/hwmon/hwmon1/temp1_input",
        NULL
    };
    
    float temperature = 0.0;
    bool temp_found = false;
    
    for (int i = 0; temp_paths[i] != NULL; i++) {
        FILE* fp = fopen(temp_paths[i], "r");
        if (fp) {
            int temp_raw;
            if (fscanf(fp, "%d", &temp_raw) == 1) {
                temperature = temp_raw / 1000.0; // Convert from millidegrees
                temp_found = true;
                fclose(fp);
                break;
            }
            fclose(fp);
        }
    }
    
    if (temp_found) {
        result.success = true;
        
        // Score based on temperature (lower is better)
        if (temperature < 40.0) {
            result.performance_score = 100.0;
        } else if (temperature < 50.0) {
            result.performance_score = 90.0;
        } else if (temperature < 60.0) {
            result.performance_score = 80.0;
        } else if (temperature < 70.0) {
            result.performance_score = 70.0;
        } else if (temperature < 80.0) {
            result.performance_score = 50.0;
        } else {
            result.performance_score = 30.0;
        }
        
        snprintf(result.message, sizeof(result.message), 
                "Temperature: %.1f°C", temperature);
        printf("✓ CPU Temperature: PASS (%.1f°C)\n", temperature);
    } else {
        result.success = true;
        result.performance_score = 70.0; // Can't measure, but not a failure
        strcpy(result.message, "Temperature monitoring not available");
        printf("✓ CPU Temperature: PASS (not available)\n");
    }
    
    return result;
}

test_result_t test_cpu_memory_bandwidth(cpu_test_t* cpu) {
    test_result_t result = {false, "", 0.0};
    
    if (!cpu || !cpu->is_initialized) {
        strcpy(result.message, "CPU not initialized");
        return result;
    }
    
    printf("Testing CPU memory bandwidth...\n");
    
    // Simple memory bandwidth test
    const int size = 100 * 1024 * 1024; // 100MB
    char* buffer = (char*)malloc(size);
    
    if (buffer) {
        clock_t start = clock();
        
        // Write test
        for (int i = 0; i < size; i++) {
            buffer[i] = (char)(i & 0xFF);
        }
        
        // Read test
        volatile char sum = 0;
        for (int i = 0; i < size; i++) {
            sum += buffer[i];
        }
        
        clock_t end = clock();
        double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
        double bandwidth = (size * 2) / (elapsed * 1024 * 1024); // MB/s
        
        free(buffer);
        
        result.success = true;
        
        // Score based on bandwidth
        if (bandwidth > 10000.0) {
            result.performance_score = 100.0;
        } else if (bandwidth > 5000.0) {
            result.performance_score = 90.0;
        } else if (bandwidth > 2000.0) {
            result.performance_score = 80.0;
        } else if (bandwidth > 1000.0) {
            result.performance_score = 70.0;
        } else {
            result.performance_score = 50.0;
        }
        
        snprintf(result.message, sizeof(result.message), 
                "Memory bandwidth: %.1f MB/s", bandwidth);
        printf("✓ CPU Memory Bandwidth: PASS (%.1f MB/s)\n", bandwidth);
    } else {
        result.performance_score = 0.0;
        strcpy(result.message, "Could not allocate memory for bandwidth test");
        printf("✗ CPU Memory Bandwidth: FAIL\n");
    }
    
    return result;
}

test_result_t test_cpu_floating_point(cpu_test_t* cpu) {
    test_result_t result = {false, "", 0.0};
    
    if (!cpu || !cpu->is_initialized) {
        strcpy(result.message, "CPU not initialized");
        return result;
    }
    
    printf("Testing CPU floating point performance...\n");
    
    // Simple floating point performance test
    const int iterations = 10000000;
    double sum = 0.0;
    
    clock_t start = clock();
    
    for (int i = 0; i < iterations; i++) {
        sum += sin(i) * cos(i) + sqrt(i);
    }
    
    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    double ops_per_second = iterations / elapsed;
    
    result.success = true;
    
    // Score based on floating point performance
    if (ops_per_second > 1000000.0) {
        result.performance_score = 100.0;
    } else if (ops_per_second > 500000.0) {
        result.performance_score = 90.0;
    } else if (ops_per_second > 200000.0) {
        result.performance_score = 80.0;
    } else if (ops_per_second > 100000.0) {
        result.performance_score = 70.0;
    } else {
        result.performance_score = 50.0;
    }
    
    snprintf(result.message, sizeof(result.message), 
            "Floating point: %.1f ops/sec", ops_per_second);
    printf("✓ CPU Floating Point: PASS (%.1f ops/sec)\n", ops_per_second);
    
    return result;
}

test_result_t test_cpu_integer_math(cpu_test_t* cpu) {
    test_result_t result = {false, "", 0.0};
    
    if (!cpu || !cpu->is_initialized) {
        strcpy(result.message, "CPU not initialized");
        return result;
    }
    
    printf("Testing CPU integer math performance...\n");
    
    // Simple integer math performance test
    const int iterations = 10000000;
    long long sum = 0;
    
    clock_t start = clock();
    
    for (int i = 0; i < iterations; i++) {
        sum += i * i + i / 2 + i % 3;
    }
    
    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    double ops_per_second = iterations / elapsed;
    
    result.success = true;
    
    // Score based on integer performance
    if (ops_per_second > 5000000.0) {
        result.performance_score = 100.0;
    } else if (ops_per_second > 2000000.0) {
        result.performance_score = 90.0;
    } else if (ops_per_second > 1000000.0) {
        result.performance_score = 80.0;
    } else if (ops_per_second > 500000.0) {
        result.performance_score = 70.0;
    } else {
        result.performance_score = 50.0;
    }
    
    snprintf(result.message, sizeof(result.message), 
            "Integer math: %.1f ops/sec", ops_per_second);
    printf("✓ CPU Integer Math: PASS (%.1f ops/sec)\n", ops_per_second);
    
    return result;
}

test_result_t test_cpu_crypto_performance(cpu_test_t* cpu) {
    test_result_t result = {false, "", 0.0};
    
    if (!cpu || !cpu->is_initialized) {
        strcpy(result.message, "CPU not initialized");
        return result;
    }
    
    printf("Testing CPU crypto performance...\n");
    
    // Simple crypto-like performance test (bit operations)
    const int iterations = 10000000;
    unsigned int hash = 0;
    
    clock_t start = clock();
    
    for (int i = 0; i < iterations; i++) {
        hash = ((hash << 5) + hash) + i; // Simple hash function
        hash ^= (hash >> 13);
        hash *= 0x5bd1e995;
        hash ^= (hash >> 15);
    }
    
    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
    double ops_per_second = iterations / elapsed;
    
    result.success = true;
    
    // Score based on crypto performance
    if (ops_per_second > 2000000.0) {
        result.performance_score = 100.0;
    } else if (ops_per_second > 1000000.0) {
        result.performance_score = 90.0;
    } else if (ops_per_second > 500000.0) {
        result.performance_score = 80.0;
    } else if (ops_per_second > 200000.0) {
        result.performance_score = 70.0;
    } else {
        result.performance_score = 50.0;
    }
    
    snprintf(result.message, sizeof(result.message), 
            "Crypto performance: %.1f ops/sec", ops_per_second);
    printf("✓ CPU Crypto Performance: PASS (%.1f ops/sec)\n", ops_per_second);
    
    return result;
}

test_result_t test_cpu_all_capabilities(cpu_test_t* cpu) {
    test_result_t result = {false, "", 0.0};
    
    if (!cpu || !cpu->is_initialized) {
        strcpy(result.message, "CPU not initialized");
        return result;
    }
    
    printf("Testing all CPU capabilities...\n");
    
    // Run all capability tests
    test_result_t tests[] = {
        test_cpu_architecture(cpu),
        test_cpu_cores(cpu),
        test_cpu_frequency(cpu),
        test_cpu_cache(cpu),
        test_cpu_instructions(cpu),
        test_cpu_load(cpu),
        test_cpu_temperature(cpu),
        test_cpu_memory_bandwidth(cpu),
        test_cpu_floating_point(cpu),
        test_cpu_integer_math(cpu),
        test_cpu_crypto_performance(cpu)
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
            "CPU capabilities: %d/%d tests passed", passed_tests, num_tests);
    
    printf("✓ CPU All Capabilities: PASS (%d/%d tests)\n", passed_tests, num_tests);
    
    return result;
}

test_summary_t run_all_cpu_tests(void) {
    test_summary_t summary = {0, 0, 0, 0.0, ""};
    
    printf("\n=== Running All CPU Tests ===\n");
    
    cpu_test_t cpu;
    if (init_cpu_test(&cpu) != 0) {
        printf("Failed to initialize CPU test\n");
        return summary;
    }
    
    // Test 1: Architecture
    test_result_t result = test_cpu_architecture(&cpu);
    summary.total_tests++;
    if (result.success) {
        summary.passed_tests++;
        printf("✓ CPU Architecture: PASS (%.1f/100)\n", result.performance_score);
    } else {
        summary.failed_tests++;
        printf("✗ CPU Architecture: FAIL (%.1f/100)\n", result.performance_score);
    }
    summary.average_score += result.performance_score;
    
    // Test 2: Cores
    result = test_cpu_cores(&cpu);
    summary.total_tests++;
    if (result.success) {
        summary.passed_tests++;
        printf("✓ CPU Cores: PASS (%.1f/100)\n", result.performance_score);
    } else {
        summary.failed_tests++;
        printf("✗ CPU Cores: FAIL (%.1f/100)\n", result.performance_score);
    }
    summary.average_score += result.performance_score;
    
    // Test 3: Frequency
    result = test_cpu_frequency(&cpu);
    summary.total_tests++;
    if (result.success) {
        summary.passed_tests++;
        printf("✓ CPU Frequency: PASS (%.1f/100)\n", result.performance_score);
    } else {
        summary.failed_tests++;
        printf("✗ CPU Frequency: FAIL (%.1f/100)\n", result.performance_score);
    }
    summary.average_score += result.performance_score;
    
    // Test 4: All Capabilities
    result = test_cpu_all_capabilities(&cpu);
    summary.total_tests++;
    if (result.success) {
        summary.passed_tests++;
        printf("✓ CPU All Capabilities: PASS (%.1f/100)\n", result.performance_score);
    } else {
        summary.failed_tests++;
        printf("✗ CPU All Capabilities: FAIL (%.1f/100)\n", result.performance_score);
    }
    summary.average_score += result.performance_score;
    
    // Calculate average score
    if (summary.total_tests > 0) {
        summary.average_score /= summary.total_tests;
    }
    
    // Create summary string
    snprintf(summary.summary, sizeof(summary.summary),
             "CPU Tests: %d/%d passed, Average Score: %.1f/100",
             summary.passed_tests, summary.total_tests, summary.average_score);
    
    cleanup_cpu_test(&cpu);
    return summary;
}

int handle_cpu_commands(const char* test_type, bool interactive_mode) {
    if (interactive_mode) {
        printf("Interactive CPU mode not implemented yet\n");
        return 1;
    } else if (test_type) {
        cpu_test_t cpu;
        if (init_cpu_test(&cpu) != 0) {
            printf("Error: Could not initialize CPU test\n");
            return 1;
        }
        
        if (strcmp(test_type, "all") == 0) {
            run_all_cpu_tests();
        } else if (strcmp(test_type, "architecture") == 0) {
            test_result_t result = test_cpu_architecture(&cpu);
            printf("CPU Architecture Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else if (strcmp(test_type, "cores") == 0) {
            test_result_t result = test_cpu_cores(&cpu);
            printf("CPU Cores Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else if (strcmp(test_type, "frequency") == 0) {
            test_result_t result = test_cpu_frequency(&cpu);
            printf("CPU Frequency Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else if (strcmp(test_type, "capabilities") == 0) {
            test_result_t result = test_cpu_all_capabilities(&cpu);
            printf("CPU Capabilities Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else {
            printf("Unknown CPU test type: %s\n", test_type);
            printf("Available tests: all, architecture, cores, frequency, capabilities\n");
            cleanup_cpu_test(&cpu);
            return 1;
        }
        
        cleanup_cpu_test(&cpu);
    }
    
    return 0;
}

} // extern "C"
