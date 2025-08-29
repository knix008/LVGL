#ifndef COMMON_H
#define COMMON_H

#include <stdbool.h>

// Common test result structure used by all device tests
typedef struct {
    bool success;
    char message[256];
    double performance_score;
} test_result_t;

// Test summary structure used by all device tests
typedef struct {
    int total_tests;
    int passed_tests;
    int failed_tests;
    double average_score;
    char summary[1024];
} test_summary_t;

#endif // COMMON_H
