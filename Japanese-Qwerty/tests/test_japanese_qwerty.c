/*
 * test_japanese_qwerty.c
 * Unit tests for Japanese QWERTY Input Method
 */

#include "../japanese_qwerty.h"
#include <stdio.h>
#include <string.h>

// Test counter
static int tests_passed = 0;
static int tests_failed = 0;

// Test helper macros
#define TEST_START(name) \
    printf("\n=== Testing: %s ===\n", name);

#define TEST_ASSERT(condition, message) \
    do { \
        if (condition) { \
            printf("✓ PASS: %s\n", message); \
            tests_passed++; \
        } else { \
            printf("✗ FAIL: %s\n", message); \
            tests_failed++; \
        } \
    } while(0)

// Test 1: IME Initialization
void test_ime_initialization(void) {
    TEST_START("IME Initialization");
    
    IMEState state;
    ime_init(&state);
    
    TEST_ASSERT(state.buffer_pos == 0, "Buffer position initialized to 0");
    TEST_ASSERT(state.output_pos == 0, "Output position initialized to 0");
    TEST_ASSERT(state.display_pos == 0, "Display position initialized to 0");
    TEST_ASSERT(state.mode == MODE_HIRAGANA, "Default mode is Hiragana");
    TEST_ASSERT(state.buffer[0] == '\0', "Buffer is empty");
}

// Test 2: Mode Switching
void test_mode_switching(void) {
    TEST_START("Mode Switching");
    
    IMEState state;
    ime_init(&state);
    
    TEST_ASSERT(state.mode == MODE_HIRAGANA, "Initial mode is Hiragana");
    
    ime_set_mode(&state, MODE_KATAKANA);
    TEST_ASSERT(state.mode == MODE_KATAKANA, "Mode switched to Katakana");
    
    ime_set_mode(&state, MODE_ENGLISH);
    TEST_ASSERT(state.mode == MODE_ENGLISH, "Mode switched to English");
}

// Test 3: Mode String Display
void test_mode_strings(void) {
    TEST_START("Mode String Display");
    
    IMEState state;
    ime_init(&state);
    
    ime_set_mode(&state, MODE_HIRAGANA);
    const char *mode1 = ime_get_mode_string(&state);
    TEST_ASSERT(strcmp(mode1, "ひらがな") == 0, "Hiragana mode string correct");
    
    ime_set_mode(&state, MODE_KATAKANA);
    const char *mode2 = ime_get_mode_string(&state);
    TEST_ASSERT(strcmp(mode2, "カタカナ") == 0, "Katakana mode string correct");
    
    ime_set_mode(&state, MODE_ENGLISH);
    const char *mode3 = ime_get_mode_string(&state);
    TEST_ASSERT(strcmp(mode3, "English") == 0, "English mode string correct");
}

// Test 4: Basic Character Input
void test_basic_input(void) {
    TEST_START("Basic Character Input");
    
    IMEState state;
    ime_init(&state);
    ime_set_mode(&state, MODE_HIRAGANA);
    
    ime_process_char(&state, 'a');
    TEST_ASSERT(state.buffer_pos == 1, "Character added to buffer");
    TEST_ASSERT(state.buffer[0] == 'a', "Buffer contains 'a'");
}

// Test 5: Romaji Conversion
void test_romaji_conversion(void) {
    TEST_START("Romaji Conversion");
    
    IMEState state;
    ime_init(&state);
    ime_set_mode(&state, MODE_HIRAGANA);
    
    ime_process_char(&state, 'k');
    ime_process_char(&state, 'a');
    
    TEST_ASSERT(state.buffer_pos == 2, "Buffer has 2 characters");
    TEST_ASSERT(state.output_pos > 0, "Output generated from romaji");
}

// Test 6: Backspace
void test_backspace(void) {
    TEST_START("Backspace");
    
    IMEState state;
    ime_init(&state);
    
    ime_process_char(&state, 'k');
    ime_process_char(&state, 'a');
    TEST_ASSERT(state.buffer_pos == 2, "Buffer has 2 characters");
    
    ime_process_backspace(&state);
    TEST_ASSERT(state.buffer_pos == 1, "Backspace reduces buffer");
}

// Test 7: Space Processing
void test_space(void) {
    TEST_START("Space Processing");
    
    IMEState state;
    ime_init(&state);
    ime_set_mode(&state, MODE_HIRAGANA);
    
    ime_process_char(&state, 'a');
    ime_process_space(&state);
    
    TEST_ASSERT(state.buffer_pos == 0, "Buffer cleared after space");
    TEST_ASSERT(state.display_pos > 0, "Content moved to display");
}

// Test 8: Enter Processing
void test_enter(void) {
    TEST_START("Enter Processing");
    
    IMEState state;
    ime_init(&state);
    
    ime_process_char(&state, 'a');
    ime_process_enter(&state);
    
    TEST_ASSERT(state.buffer_pos == 0, "Buffer cleared after enter");
    TEST_ASSERT(state.display_pos > 0, "Content in display");
}

// Test 9: Clear
void test_clear(void) {
    TEST_START("Clear");
    
    IMEState state;
    ime_init(&state);
    
    ime_process_char(&state, 'a');
    ime_clear(&state);
    
    TEST_ASSERT(state.buffer_pos == 0, "Buffer cleared");
    TEST_ASSERT(state.output_pos == 0, "Output cleared");
    TEST_ASSERT(state.display_pos == 0, "Display cleared");
}

// Test 10: English Mode
void test_english_mode(void) {
    TEST_START("English Mode");
    
    IMEState state;
    ime_init(&state);
    ime_set_mode(&state, MODE_ENGLISH);
    
    ime_process_char(&state, 'h');
    ime_process_char(&state, 'i');
    
    TEST_ASSERT(state.buffer_pos == 2, "English characters in buffer");
    TEST_ASSERT(strcmp(state.output, "hi") == 0, "English output correct");
}

// Main test runner
int main(void) {
    printf("\n");
    printf("================================================\n");
    printf("  Japanese QWERTY Input Method - Test Suite\n");
    printf("================================================\n");
    
    // Run all tests
    test_ime_initialization();
    test_mode_switching();
    test_mode_strings();
    test_basic_input();
    test_romaji_conversion();
    test_backspace();
    test_space();
    test_enter();
    test_clear();
    test_english_mode();
    
    // Print summary
    printf("\n");
    printf("================================================\n");
    printf("  Test Results\n");
    printf("================================================\n");
    printf("Total tests run: %d\n", tests_passed + tests_failed);
    printf("✓ Passed: %d\n", tests_passed);
    printf("✗ Failed: %d\n", tests_failed);
    printf("\n");
    
    if (tests_failed == 0) {
        printf("🎉 All tests passed!\n\n");
        return 0;
    } else {
        printf("❌ Some tests failed!\n\n");
        return 1;
    }
}
