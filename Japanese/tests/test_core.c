/*
 * Test cases for Japanese Input Core Logic
 * Tests the core input functionality without GUI dependencies
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <assert.h>
#include "../japanese_input.h"

// Test result tracking
static int tests_run = 0;
static int tests_passed = 0;

#define TEST_ASSERT(condition, message) \
    do { \
        tests_run++; \
        if (condition) { \
            tests_passed++; \
            printf("✓ PASS: %s\n", message); \
        } else { \
            printf("✗ FAIL: %s\n", message); \
        } \
    } while(0)

// Test initialization
void test_initialization() {
    printf("\n=== Testing Initialization ===\n");
    
    JapaneseInputState state;
    japanese_input_init(&state);
    
    TEST_ASSERT(state.now_mode == MODE_JAPANESE, "Initial mode should be Hiragana");
    TEST_ASSERT(state.cursor_pos == 0, "Initial cursor position should be 0");
    TEST_ASSERT(state.shift_mode == false, "Initial shift mode should be false");
    TEST_ASSERT(state.composing_len == 0, "Initial composing length should be 0");
    TEST_ASSERT(state.flick_button == -1, "Initial flick button should be -1");
    TEST_ASSERT(state.flick_active == false, "Initial flick active should be false");
    TEST_ASSERT(state.text_buffer[0] == L'\0', "Initial text buffer should be empty");
}

// Test mode switching
void test_mode_switching() {
    printf("\n=== Testing Mode Switching ===\n");
    
    JapaneseInputState state;
    japanese_input_init(&state);
    
    // Test cycling through all modes
    TEST_ASSERT(state.now_mode == MODE_JAPANESE, "Initial mode should be Japanese");
    
    change_input_mode(&state);
    TEST_ASSERT(state.now_mode == MODE_ALPHABET, "Second mode should be Alphabet");
    
    change_input_mode(&state);
    TEST_ASSERT(state.now_mode == MODE_NUMBER, "Third mode should be Number");
    
    change_input_mode(&state);
    TEST_ASSERT(state.now_mode == MODE_SYMBOL, "Fourth mode should be Symbol");
    
    change_input_mode(&state);
    TEST_ASSERT(state.now_mode == MODE_JAPANESE, "Should cycle back to Japanese");
}

// Test button text generation
void test_button_text() {
    printf("\n=== Testing Button Text Generation ===\n");
    
    // Test Japanese mode (defaults to Hiragana)
    const wchar_t* japanese_text = get_button_text(MODE_JAPANESE, 0);
    TEST_ASSERT(japanese_text != NULL && wcslen(japanese_text) > 0, "Japanese button text should not be empty");
    TEST_ASSERT(japanese_text[0] == L'あ', "First Japanese button should show あ");
    
    // Test Alphabet mode (now shows all characters)
    const wchar_t* alphabet_text = get_button_text(MODE_ALPHABET, 0);
    TEST_ASSERT(alphabet_text != NULL && wcslen(alphabet_text) > 0, "Alphabet button text should not be empty");
    // For English mode, we now show all characters like "abc", so check if it contains 'a'
    TEST_ASSERT(wcschr(alphabet_text, L'a') != NULL, "Alphabet button should contain 'a'");
    
    // Test Number mode
    const wchar_t* number_text = get_button_text(MODE_NUMBER, 0);
    TEST_ASSERT(number_text != NULL && wcslen(number_text) > 0, "Number button text should not be empty");
    TEST_ASSERT(number_text[0] == L'1', "First Number button should show '1'");
    
    // Test Symbol mode
    const wchar_t* symbol_text = get_button_text(MODE_SYMBOL, 0);
    TEST_ASSERT(symbol_text != NULL && wcslen(symbol_text) > 0, "Symbol button text should not be empty");
    TEST_ASSERT(symbol_text[0] == L'！', "First Symbol button should show ！");
}

// Test character count functions
void test_character_counts() {
    printf("\n=== Testing Character Count Functions ===\n");
    
    // Test Japanese character counts
    int japanese_count = get_button_char_count(MODE_JAPANESE, 0);
    TEST_ASSERT(japanese_count == 5, "Japanese button 0 should have 5 characters");
    
    int japanese_count_7 = get_button_char_count(MODE_JAPANESE, 7);
    TEST_ASSERT(japanese_count_7 == 3, "Japanese button 7 (や) should have 3 characters");
    
    // Test Alphabet character counts
    int alphabet_count = get_button_char_count(MODE_ALPHABET, 0);
    TEST_ASSERT(alphabet_count == 3, "Alphabet button 0 should have 3 characters");
    
    // Test Number character counts
    int number_count = get_button_char_count(MODE_NUMBER, 0);
    TEST_ASSERT(number_count == 1, "Number button 0 should have 1 character");
}

// Test flick character functions
void test_flick_characters() {
    printf("\n=== Testing Flick Character Functions ===\n");
    
    // Test Japanese flick characters (defaults to Hiragana)
    const wchar_t* japanese_flick = get_button_flick_chars(MODE_JAPANESE, 0);
    TEST_ASSERT(japanese_flick != NULL, "Japanese flick characters should not be NULL");
    TEST_ASSERT(wcscmp(japanese_flick, L"あいうえお") == 0, "Japanese button 0 should have あいうえお");
    
    // Test Alphabet flick characters
    const char* alphabet_flick = get_button_alphabet_chars(0);
    TEST_ASSERT(alphabet_flick != NULL, "Alphabet flick characters should not be NULL");
    TEST_ASSERT(strcmp(alphabet_flick, "abc") == 0, "Alphabet button 0 should have 'abc'");
    
    // Test Number flick characters
    const char* number_flick = get_button_number_chars(0);
    TEST_ASSERT(number_flick != NULL, "Number flick characters should not be NULL");
    TEST_ASSERT(strcmp(number_flick, "1") == 0, "Number button 0 should have '1'");
}

// Test mode names
void test_mode_names() {
    printf("\n=== Testing Mode Names ===\n");
    
    const char* japanese_name = get_mode_name(MODE_JAPANESE);
    TEST_ASSERT(japanese_name != NULL, "Mode name should not be NULL");
    TEST_ASSERT(strcmp(japanese_name, "日本語") == 0, "Japanese mode name should be 日本語");
    
    const char* alphabet_name = get_mode_name(MODE_ALPHABET);
    TEST_ASSERT(strcmp(alphabet_name, "ABC") == 0, "Alphabet mode name should be ABC");
    
    const char* number_name = get_mode_name(MODE_NUMBER);
    TEST_ASSERT(strcmp(number_name, "123") == 0, "Number mode name should be 123");
    
    const char* symbol_name = get_mode_name(MODE_SYMBOL);
    TEST_ASSERT(strcmp(symbol_name, "記号") == 0, "Symbol mode name should be 記号");
}

// Test UTF-8 conversion
void test_utf8_conversion() {
    printf("\n=== Testing UTF-8 Conversion ===\n");
    
    // Test simple ASCII conversion
    const wchar_t* ascii_text = L"Hello";
    char* utf8_result = wchar_to_utf8(ascii_text, 10);
    TEST_ASSERT(utf8_result != NULL, "UTF-8 conversion should not return NULL");
    TEST_ASSERT(strcmp(utf8_result, "Hello") == 0, "ASCII conversion should match");
    
    // Test Japanese character conversion
    const wchar_t* japanese_text = L"あいうえお";
    char* japanese_utf8 = wchar_to_utf8(japanese_text, 20);
    TEST_ASSERT(japanese_utf8 != NULL, "Japanese UTF-8 conversion should not return NULL");
    TEST_ASSERT(strlen(japanese_utf8) > 0, "Japanese UTF-8 result should not be empty");
    
    // Test NULL input
    char* null_result = wchar_to_utf8(NULL, 10);
    TEST_ASSERT(null_result == NULL, "NULL input should return NULL");
}

// Test flick input functionality
void test_flick_input() {
    printf("\n=== Testing Flick Input Functionality ===\n");
    
    JapaneseInputState state;
    japanese_input_init(&state);
    
    // Test showing flick input
    japanese_input_show_flick(&state, 0);
    TEST_ASSERT(state.flick_button == 0, "Flick button should be set to 0");
    TEST_ASSERT(state.flick_active == true, "Flick should be active");
    
    // Test selecting a character from flick input
    japanese_input_select_flick_char(&state, 0, 0); // Select first character (あ)
    TEST_ASSERT(state.text_buffer[0] == L'あ', "First character should be あ");
    TEST_ASSERT(state.cursor_pos == 1, "Cursor position should be 1");
    TEST_ASSERT(state.flick_button == -1, "Flick button should be reset");
    TEST_ASSERT(state.flick_active == false, "Flick should be inactive");
    
    // Test selecting another character
    japanese_input_select_flick_char(&state, 0, 1); // Select second character (い)
    TEST_ASSERT(state.text_buffer[1] == L'い', "Second character should be い");
    TEST_ASSERT(state.cursor_pos == 2, "Cursor position should be 2");
}

// Test input processing
void test_input_processing() {
    printf("\n=== Testing Input Processing ===\n");
    
    JapaneseInputState state;
    japanese_input_init(&state);
    
    // Test initial flick input activation
    japanese_input_process_input(&state, 0);
    TEST_ASSERT(state.flick_button == 0, "Processing input should set flick button");
    TEST_ASSERT(state.flick_active == true, "Processing input should activate flick");
    
    // Test selecting first character when flick is already active
    japanese_input_process_input(&state, 0);
    TEST_ASSERT(state.text_buffer[0] == L'あ', "Processing same input should select first character");
    TEST_ASSERT(state.flick_active == false, "Flick should be deactivated after selection");
}

// Test edge cases
void test_edge_cases() {
    printf("\n=== Testing Edge Cases ===\n");
    
    // Test invalid button numbers
    const wchar_t* invalid_text = get_button_text(MODE_JAPANESE, -1);
    TEST_ASSERT(wcslen(invalid_text) == 0, "Invalid button number should return empty string");
    
    invalid_text = get_button_text(MODE_JAPANESE, 12);
    TEST_ASSERT(wcslen(invalid_text) == 0, "Button number >= 12 should return empty string");
    
    // Test invalid mode
    const char* invalid_mode = get_mode_name(MODE_COUNT);
    TEST_ASSERT(strcmp(invalid_mode, "Unknown") == 0, "Invalid mode should return 'Unknown'");
    
    // Test character count for invalid buttons
    int invalid_count = get_button_char_count(MODE_JAPANESE, -1);
    TEST_ASSERT(invalid_count == 0, "Invalid button should have 0 characters");
}

// Main test runner
int main() {
    printf("Japanese Input Core Logic Tests\n");
    printf("===============================\n");
    
    test_initialization();
    test_mode_switching();
    test_button_text();
    test_character_counts();
    test_flick_characters();
    test_mode_names();
    test_utf8_conversion();
    test_flick_input();
    test_input_processing();
    test_edge_cases();
    
    printf("\n=== Test Results ===\n");
    printf("Tests run: %d\n", tests_run);
    printf("Tests passed: %d\n", tests_passed);
    printf("Tests failed: %d\n", tests_run - tests_passed);
    
    if (tests_passed == tests_run) {
        printf("\n✓ ALL TESTS PASSED!\n");
        return 0;
    } else {
        printf("\n✗ SOME TESTS FAILED!\n");
        return 1;
    }
}
