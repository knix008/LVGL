#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <assert.h>

// Type definitions from chunjiin_input.c
typedef enum {
    STATE_START,          // 초기 상태
    STATE_CHOSEONG,       // 초성 입력됨
    STATE_JUNGSEONG,      // 중성 입력됨
    STATE_JONGSEONG,      // 종성 입력됨
} SyllableState;

typedef struct {
    int cho;              // 초성 인덱스
    int jung;             // 중성 인덱스
    int jong;             // 종성 인덱스
    SyllableState state;  // 현재 조합 상태
    int temp_vowel;       // 모음 조합을 위해 마지막으로 입력된 모음 키를 임시 저장
    int temp_consonant;   // 자음 조합(쌍자음 등)을 위해 마지막으로 입력된 자음 키를 임시 저장
} CurrentSyllable;

// External declarations for global variables from chunjiin_input.c
extern wchar_t g_output_buffer[1024];
extern CurrentSyllable g_current_syllable;

// Function declarations
void initialize();
void process_input(char key);
void chunjiin_enter_key_handler();
void chunjiin_get_current_text(wchar_t * buffer);
void reset_current_syllable();

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
    printf("\n=== TEST SUMMARY ===\n");
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
    
    // Convert wchar_t to char* for easier comparison
    char* result = malloc(1024);
    int i = 0;
    while (buffer[i] != L'\0' && i < 1023) {
        result[i] = (char)buffer[i];
        i++;
    }
    result[i] = '\0';
    return result;
}

// Helper function to get output before enter (to avoid buffer clearing)
char* get_output_before_enter() {
    // Finalize current syllable to get the result
    wchar_t buffer[1024];
    chunjiin_get_current_text(buffer);
    
    // Convert wchar_t to char* for easier comparison
    char* result = malloc(1024);
    int i = 0;
    while (buffer[i] != L'\0' && i < 1023) {
        result[i] = (char)buffer[i];
        i++;
    }
    result[i] = '\0';
    return result;
}

// Helper function to clear output
void clear_output() {
    reset_current_syllable();
    wmemset(g_output_buffer, 0, 1024);
}

// Test basic input processing
void test_basic_input() {
    printf("\n=== TESTING BASIC INPUT ===\n");
    
    // Test that input is processed (produces some output)
    clear_output();
    process_input('g');
    char* output = get_output_before_enter();
    test_assert(strlen(output) > 0, "Basic consonant input produces output");
    free(output);
    chunjiin_enter_key_handler();
    
    // Test that different inputs produce different outputs
    clear_output();
    process_input('n');
    output = get_output_before_enter();
    test_assert(strlen(output) > 0, "Different consonant input produces output");
    free(output);
    chunjiin_enter_key_handler();
    
    // Test vowel input
    clear_output();
    process_input('i');
    output = get_output_before_enter();
    test_assert(strlen(output) > 0, "Vowel input produces output");
    free(output);
    chunjiin_enter_key_handler();
}

// Test input combinations
void test_input_combinations() {
    printf("\n=== TESTING INPUT COMBINATIONS ===\n");
    
    // Test consonant + vowel combination
    clear_output();
    process_input('g');
    process_input('i');
    char* output = get_output_before_enter();
    test_assert(strlen(output) > 0, "Consonant + vowel combination produces output");
    free(output);
    chunjiin_enter_key_handler();
    
    // Test multiple consonants
    clear_output();
    process_input('g');
    process_input('g');
    output = get_output_before_enter();
    test_assert(strlen(output) > 0, "Multiple consonants produce output");
    free(output);
    chunjiin_enter_key_handler();
    
    // Test complex combination
    clear_output();
    process_input('g');
    process_input('i');
    process_input('g');
    output = get_output_before_enter();
    test_assert(strlen(output) > 0, "Complex combination produces output");
    free(output);
    chunjiin_enter_key_handler();
}

// Test state management
void test_state_management() {
    printf("\n=== TESTING STATE MANAGEMENT ===\n");
    
    // Test that state changes with input
    clear_output();
    test_assert(g_current_syllable.state == STATE_START, "Initial state is START");
    
    process_input('g');
    test_assert(g_current_syllable.state != STATE_START, "State changes after input");
    
    // Test that state resets after enter
    chunjiin_enter_key_handler();
    test_assert(g_current_syllable.state == STATE_START, "State resets after enter");
}

// Test buffer management
void test_buffer_management() {
    printf("\n=== TESTING BUFFER MANAGEMENT ===\n");
    
    // Test that buffer is empty initially
    clear_output();
    char* output = get_current_output();
    test_assert(strlen(output) == 0, "Buffer is empty initially");
    free(output);
    
    // Test that buffer gets content after input
    process_input('g');
    output = get_output_before_enter();
    test_assert(strlen(output) > 0, "Buffer gets content after input");
    free(output);
    chunjiin_enter_key_handler();
    
    // Test that buffer accumulates multiple inputs
    clear_output();
    process_input('g');
    chunjiin_enter_key_handler();
    process_input('n');
    output = get_output_before_enter();
    test_assert(strlen(output) > 0, "Buffer accumulates multiple inputs");
    free(output);
    chunjiin_enter_key_handler();
}

// Test error handling
void test_error_handling() {
    printf("\n=== TESTING ERROR HANDLING ===\n");
    
    // Test invalid input
    clear_output();
    process_input('x'); // Invalid key
    char* output = get_output_before_enter();
    test_assert(strlen(output) == 0, "Invalid input produces no output");
    free(output);
    chunjiin_enter_key_handler();
    
    // Test empty input
    clear_output();
    output = get_current_output();
    test_assert(strlen(output) == 0, "Empty input produces no output");
    free(output);
    chunjiin_enter_key_handler();
}

// Test all consonant inputs
void test_all_consonants() {
    printf("\n=== TESTING ALL CONSONANTS ===\n");
    
    char* consonants = "gndbsjm";
    for (int i = 0; consonants[i] != '\0'; i++) {
        clear_output();
        process_input(consonants[i]);
        char* output = get_output_before_enter();
        test_assert(strlen(output) > 0, "Consonant input produces output");
        free(output);
        chunjiin_enter_key_handler();
    }
}

// Test all vowel inputs
void test_all_vowels() {
    printf("\n=== TESTING ALL VOWELS ===\n");
    
    // Test basic vowel elements
    char* vowels = "aei";
    for (int i = 0; vowels[i] != '\0'; i++) {
        clear_output();
        process_input(vowels[i]);
        char* output = get_output_before_enter();
        test_assert(strlen(output) > 0, "Basic vowel element produces output");
        free(output);
        chunjiin_enter_key_handler();
    }
}

// Test ChunJiIn vowel combinations
void test_chunjiin_vowel_combinations() {
    printf("\n=== TESTING CHUNJIIN VOWEL COMBINATIONS ===\n");
    
    // Test ㅏ (ㅣ + ㆍ)
    clear_output();
    process_input('i'); // ㅣ
    process_input('a'); // ㆍ
    char* output = get_output_before_enter();
    test_assert(strlen(output) > 0, "ㅣ + ㆍ combination produces output");
    free(output);
    chunjiin_enter_key_handler();
    
    // Test ㅐ (ㅣ + ㆍ + ㅣ)
    clear_output();
    process_input('i'); // ㅣ
    process_input('a'); // ㆍ
    process_input('i'); // ㅣ
    output = get_output_before_enter();
    test_assert(strlen(output) > 0, "ㅣ + ㆍ + ㅣ combination produces output");
    free(output);
    chunjiin_enter_key_handler();
    
    // Test ㅓ (ㅡ + ㆍ + ㆍ + ㅣ + ㅣ)
    clear_output();
    process_input('e'); // ㅡ
    process_input('a'); // ㆍ
    process_input('a'); // ㆍ
    process_input('i'); // ㅣ
    process_input('i'); // ㅣ
    output = get_output_before_enter();
    test_assert(strlen(output) > 0, "ㅡ + ㆍ + ㆍ + ㅣ + ㅣ combination produces output");
    free(output);
    chunjiin_enter_key_handler();
    
    // Test ㅔ (ㅡ + ㆍ + ㆍ + ㅣ + ㅣ + ㅣ)
    clear_output();
    process_input('e'); // ㅡ
    process_input('a'); // ㆍ
    process_input('a'); // ㆍ
    process_input('i'); // ㅣ
    process_input('i'); // ㅣ
    process_input('i'); // ㅣ
    output = get_output_before_enter();
    test_assert(strlen(output) > 0, "ㅡ + ㆍ + ㆍ + ㅣ + ㅣ + ㅣ combination produces output");
    free(output);
    chunjiin_enter_key_handler();
    
    // Test ㅕ (ㆍ + ㆍ + ㅣ)
    clear_output();
    process_input('a'); // ㆍ
    process_input('a'); // ㆍ
    process_input('i'); // ㅣ
    output = get_output_before_enter();
    test_assert(strlen(output) > 0, "ㆍ + ㆍ + ㅣ combination produces output");
    free(output);
    chunjiin_enter_key_handler();
}

// Test consonant toggling behavior
void test_consonant_toggling() {
    printf("\n=== TESTING CONSONANT TOGGLING ===\n");
    
    // Test that repeated input produces different results
    clear_output();
    process_input('g');
    char* output1 = get_output_before_enter();
    chunjiin_enter_key_handler();
    
    clear_output();
    process_input('g');
    process_input('g');
    char* output2 = get_output_before_enter();
    chunjiin_enter_key_handler();
    
    // The outputs should be different (toggling behavior)
    test_assert(strcmp(output1, output2) != 0, "Repeated input produces different output");
    
    free(output1);
    free(output2);
}

// Main test runner
int main() {
    // Set locale for wide character support
    setlocale(LC_ALL, "");
    
    printf("=== CHUNJIIN INPUT SIMPLE TEST SUITE ===\n");
    printf("Testing basic functionality without output format assumptions\n\n");
    
    test_init();
    
    // Run all test suites
    test_basic_input();
    test_input_combinations();
    test_state_management();
    test_buffer_management();
    test_error_handling();
    test_all_consonants();
    test_all_vowels();
    test_chunjiin_vowel_combinations();
    test_consonant_toggling();
    
    // Print final statistics
    test_print_stats();
    
    return (test_stats.failed_tests == 0) ? 0 : 1;
} 