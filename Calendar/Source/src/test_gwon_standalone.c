#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <assert.h>
#include "../include/chunjiin_input.h"

// External declarations for global variables from chunjiin_input.c
extern wchar_t g_output_buffer[1024];

// Function declarations from chunjiin_input.c
void initialize(void);
void process_input(char key);
void reset_current_syllable(void);
void chunjiin_enter_key_handler(void);
void chunjiin_get_current_text(wchar_t * buffer);
int wchar_to_utf8(wchar_t wc, char *utf8_buffer, size_t buffer_size);

// Test result tracking
typedef struct {
    int total_tests;
    int passed_tests;
    int failed_tests;
} test_stats_t;

test_stats_t test_stats = {0, 0, 0};

// Test helper functions
void test_init() {
    test_stats.total_tests = 0;
    test_stats.passed_tests = 0;
    test_stats.failed_tests = 0;
    initialize();
}

void test_assert(int condition, const char* test_name) {
    test_stats.total_tests++;
    if (condition) {
        test_stats.passed_tests++;
        printf("✓ PASS: %s\n", test_name);
    } else {
        test_stats.failed_tests++;
        printf("✗ FAIL: %s\n", test_name);
    }
}

void test_print_stats() {
    printf("\n=== GWON TEST SUMMARY ===\n");
    printf("Total tests: %d\n", test_stats.total_tests);
    printf("Passed: %d\n", test_stats.passed_tests);
    printf("Failed: %d\n", test_stats.failed_tests);
    printf("Success rate: %.1f%%\n", 
           (test_stats.total_tests > 0) ? 
           (float)test_stats.passed_tests / test_stats.total_tests * 100 : 0);
}

// Helper function to get current output as string
char* get_current_output() {
    wchar_t buffer[1024];
    chunjiin_get_current_text(buffer);
    
    // Convert wchar_t to UTF-8 char* for proper Korean character handling
    char* result = malloc(1024);
    int pos = 0;
    int i = 0;
    while (buffer[i] != L'\0' && pos < 1023) {
        char utf8_buffer[8];
        int utf8_len = wchar_to_utf8(buffer[i], utf8_buffer, sizeof(utf8_buffer));
        if (utf8_len > 0 && pos + utf8_len < 1023) {
            memcpy(result + pos, utf8_buffer, utf8_len);
            pos += utf8_len;
        }
        i++;
    }
    result[pos] = '\0';
    return result;
}

// Helper function to clear output
void clear_output() {
    // Reset the current syllable and clear output buffer
    reset_current_syllable();
    wmemset(g_output_buffer, 0, 1024);
}

// Test the basic "권" syllable formation
void test_basic_gwon() {
    printf("\n=== TESTING BASIC GWON FORMATION ===\n");
    
    // Test 1: Standard input sequence for "권"
    clear_output();
    process_input('g');  // ㄱ
    process_input('u');  // ㅡ
    process_input('e');  // ㆍ
    process_input('a');  // ㅣ
    process_input('i');  // ㅣ
    
    char* output = get_current_output();
    printf("Input: gueai -> Output: %s\n", output);
    test_assert(strlen(output) > 0, "Basic gwon formation produces output");
    free(output);
    
    chunjiin_enter_key_handler();
}

// Test alternative input sequences for "권"
void test_alternative_gwon_sequences() {
    printf("\n=== TESTING ALTERNATIVE GWON SEQUENCES ===\n");
    
    struct {
        char* input;
        char* description;
    } sequences[] = {
        {"gean", "ㄱ + ㅡ + ㆍ + ㄴ = 권 (with final consonant ㄴ)"},
        {"geaain", "ㄱ + ㅡ + ㆍ + ㆍ + ㅣ + ㄴ = 권 (dot + dot + ㅣ + ㄴ)"},
        {"gueaig", "ㄱ + ㅡ + ㆍ + ㅣ + ㄱ = 권ㄱ (with final ㄱ)"},
        {"gueais", "ㄱ + ㅡ + ㆍ + ㅣ + ㅅ = 권ㅅ (with final ㅅ)"},
        {"gueaij", "ㄱ + ㅡ + ㆍ + ㅣ + ㅈ = 권ㅈ (with final ㅈ)"},
        {"gueaib", "ㄱ + ㅡ + ㆍ + ㅣ + ㅂ = 권ㅂ (with final ㅂ)"},
        {"gueaim", "ㄱ + ㅡ + ㆍ + ㅣ + ㅇ = 권ㅇ (with final ㅇ)"}
    };
    
    for (int i = 0; i < 7; i++) {
        clear_output();
        
        // Input the sequence
        for (int j = 0; sequences[i].input[j] != '\0'; j++) {
            process_input(sequences[i].input[j]);
        }
        
        char* output = get_current_output();
        printf("Input: %s -> Output: %s\n", sequences[i].input, output);
        test_assert(strlen(output) > 0, sequences[i].description);
        free(output);
        
        chunjiin_enter_key_handler();
    }
}

// Test double consonant variations of "권"
void test_double_consonant_gwon() {
    printf("\n=== TESTING DOUBLE CONSONANT GWON ===\n");
    
    struct {
        char* input;
        char* description;
    } double_tests[] = {
        {"ggueai", "ㄲ + ㅡ + ㆍ + ㅣ = 꿘 (double ㄱ)"},
        {"ggueaig", "ㄲ + ㅡ + ㆍ + ㅣ + ㄱ = 꿘ㄱ (double ㄱ with final ㄱ)"},
        {"ggueain", "ㄲ + ㅡ + ㆍ + ㄴ = 꿘 (double ㄱ with final ㄴ)"}
    };
    
    for (int i = 0; i < 3; i++) {
        clear_output();
        
        // Input the sequence
        for (int j = 0; double_tests[i].input[j] != '\0'; j++) {
            process_input(double_tests[i].input[j]);
        }
        
        char* output = get_current_output();
        printf("Input: %s -> Output: %s\n", double_tests[i].input, output);
        test_assert(strlen(output) > 0, double_tests[i].description);
        free(output);
        
        chunjiin_enter_key_handler();
    }
}

// Test complex final consonant combinations with "권"
void test_complex_final_gwon() {
    printf("\n=== TESTING COMPLEX FINAL CONSONANTS WITH GWON ===\n");
    
    struct {
        char* input;
        char* description;
    } complex_tests[] = {
        {"gueaigs", "ㄱ + ㅡ + ㆍ + ㅣ + ㄴ + ㅎ = 권ㄶ (with ㄶ)"},
        {"gueailg", "ㄱ + ㅡ + ㆍ + ㅣ + ㄹ + ㄱ = 권ㄺ (with ㄺ)"},
        {"gueaigj", "ㄱ + ㅡ + ㆍ + ㅣ + ㄴ + ㅈ = 권ㄵ (with ㄵ)"}
    };
    
    for (int i = 0; i < 3; i++) {
        clear_output();
        
        // Input the sequence
        for (int j = 0; complex_tests[i].input[j] != '\0'; j++) {
            process_input(complex_tests[i].input[j]);
        }
        
        char* output = get_current_output();
        printf("Input: %s -> Output: %s\n", complex_tests[i].input, output);
        test_assert(strlen(output) > 0, complex_tests[i].description);
        free(output);
        
        chunjiin_enter_key_handler();
    }
}

// Test cycling final consonants with "권"
void test_cycling_final_gwon() {
    printf("\n=== TESTING CYCLING FINAL CONSONANTS WITH GWON ===\n");
    
    struct {
        char* input;
        char* description;
    } cycling_tests[] = {
        {"gueaiggg", "ㄱ + ㅡ + ㆍ + ㅣ + ㄱ + ㄱ + ㄱ = 권ㄲ (cycling to ㄲ)"},
        {"gueaiddd", "ㄱ + ㅡ + ㆍ + ㅣ + ㄷ + ㄷ + ㄷ = 권ㄸ (cycling to ㄸ)"},
        {"gueaibbb", "ㄱ + ㅡ + ㆍ + ㅣ + ㅂ + ㅂ + ㅂ = 권ㅃ (cycling to ㅃ)"},
        {"gueaiss", "ㄱ + ㅡ + ㆍ + ㅣ + ㅅ + ㅅ = 권ㅆ (cycling to ㅆ)"},
        {"gueaijj", "ㄱ + ㅡ + ㆍ + ㅣ + ㅈ + ㅈ = 권ㅉ (cycling to ㅉ)"}
    };
    
    for (int i = 0; i < 5; i++) {
        clear_output();
        
        // Input the sequence
        for (int j = 0; cycling_tests[i].input[j] != '\0'; j++) {
            process_input(cycling_tests[i].input[j]);
        }
        
        char* output = get_current_output();
        printf("Input: %s -> Output: %s\n", cycling_tests[i].input, output);
        test_assert(strlen(output) > 0, cycling_tests[i].description);
        free(output);
        
        chunjiin_enter_key_handler();
    }
}

// Test edge cases for "권"
void test_edge_cases_gwon() {
    printf("\n=== TESTING EDGE CASES FOR GWON ===\n");
    
    struct {
        char* input;
        char* description;
    } edge_tests[] = {
        {"gueaia", "ㄱ + ㅡ + ㆍ + ㅣ + ㆍ = 권 (with extra dot)"},
        {"gueaiaa", "ㄱ + ㅡ + ㆍ + ㅣ + ㆍ + ㆍ = 권 (with multiple dots)"},
        {"gueaiai", "ㄱ + ㅡ + ㆍ + ㅣ + ㆍ + ㅣ = 권 (with dot + ㅣ)"},
        {"gueiaeia", "ㄱ + ㅡ + ㆍ + ㅣ + ㅣ + ㆍ + ㅣ + ㆍ = 권ㅏ (with additional vowels)"},
        {"gueiaeg", "ㄱ + ㅡ + ㆍ + ㅣ + ㅣ + ㆍ + ㄱ = 권ㄱ (with vowel + final consonant)"}
    };
    
    for (int i = 0; i < 5; i++) {
        clear_output();
        
        // Input the sequence
        for (int j = 0; edge_tests[i].input[j] != '\0'; j++) {
            process_input(edge_tests[i].input[j]);
        }
        
        char* output = get_current_output();
        printf("Input: %s -> Output: %s\n", edge_tests[i].input, output);
        test_assert(strlen(output) > 0, edge_tests[i].description);
        free(output);
        
        chunjiin_enter_key_handler();
    }
}

// Main test runner
int main() {
    // Set locale for wide character support
    setlocale(LC_ALL, "");
    
    printf("=== GWON (권) SYLLABLE TEST SUITE ===\n");
    printf("Testing various input sequences for the Korean syllable '권'\n\n");
    
    test_init();
    
    // Run all gwon-specific test suites
    test_basic_gwon();
    test_alternative_gwon_sequences();
    test_double_consonant_gwon();
    test_complex_final_gwon();
    test_cycling_final_gwon();
    test_edge_cases_gwon();
    
    // Print final statistics
    test_print_stats();
    
    return (test_stats.failed_tests == 0) ? 0 : 1;
} 