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

// Test 11: Vowel Conversion (a, i, u, e, o)
void test_vowel_conversion(void) {
    TEST_START("Vowel Conversion");
    
    IMEState state;
    
    // Test 'a'
    ime_init(&state);
    ime_set_mode(&state, MODE_HIRAGANA);
    ime_process_char(&state, 'a');
    TEST_ASSERT(strcmp(state.output, "あ") == 0, "'a' converts to 'あ'");
    
    // Test 'i'
    ime_init(&state);
    ime_set_mode(&state, MODE_HIRAGANA);
    ime_process_char(&state, 'i');
    TEST_ASSERT(strcmp(state.output, "い") == 0, "'i' converts to 'い'");
    
    // Test 'u'
    ime_init(&state);
    ime_set_mode(&state, MODE_HIRAGANA);
    ime_process_char(&state, 'u');
    TEST_ASSERT(strcmp(state.output, "う") == 0, "'u' converts to 'う'");
    
    // Test 'e'
    ime_init(&state);
    ime_set_mode(&state, MODE_HIRAGANA);
    ime_process_char(&state, 'e');
    TEST_ASSERT(strcmp(state.output, "え") == 0, "'e' converts to 'え'");
    
    // Test 'o'
    ime_init(&state);
    ime_set_mode(&state, MODE_HIRAGANA);
    ime_process_char(&state, 'o');
    TEST_ASSERT(strcmp(state.output, "お") == 0, "'o' converts to 'お'");
}

// Test 12: Katakana Vowel Conversion
void test_katakana_vowels(void) {
    TEST_START("Katakana Vowel Conversion");
    
    IMEState state;
    
    // Test katakana 'a'
    ime_init(&state);
    ime_set_mode(&state, MODE_KATAKANA);
    ime_process_char(&state, 'a');
    TEST_ASSERT(strcmp(state.output, "ア") == 0, "'a' converts to 'ア' in katakana");
    
    // Test katakana 'i'
    ime_init(&state);
    ime_set_mode(&state, MODE_KATAKANA);
    ime_process_char(&state, 'i');
    TEST_ASSERT(strcmp(state.output, "イ") == 0, "'i' converts to 'イ' in katakana");
}

// Test 13: Y-row Characters (ya, yu, yo)
void test_y_row_conversion(void) {
    TEST_START("Y-row Character Conversion");
    
    IMEState state;
    
    // Test 'ya'
    ime_init(&state);
    ime_set_mode(&state, MODE_HIRAGANA);
    ime_process_char(&state, 'y');
    ime_process_char(&state, 'a');
    TEST_ASSERT(strcmp(state.output, "や") == 0, "'ya' converts to 'や'");
    
    // Test 'yu'
    ime_init(&state);
    ime_set_mode(&state, MODE_HIRAGANA);
    ime_process_char(&state, 'y');
    ime_process_char(&state, 'u');
    TEST_ASSERT(strcmp(state.output, "ゆ") == 0, "'yu' converts to 'ゆ'");
    
    // Test 'yo'
    ime_init(&state);
    ime_set_mode(&state, MODE_HIRAGANA);
    ime_process_char(&state, 'y');
    ime_process_char(&state, 'o');
    TEST_ASSERT(strcmp(state.output, "よ") == 0, "'yo' converts to 'よ'");
}

// Test 14: W-row Character (wa)
void test_w_row_conversion(void) {
    TEST_START("W-row Character Conversion");
    
    IMEState state;
    ime_init(&state);
    ime_set_mode(&state, MODE_HIRAGANA);
    
    ime_process_char(&state, 'w');
    ime_process_char(&state, 'a');
    
    TEST_ASSERT(strcmp(state.output, "わ") == 0, "'wa' converts to 'わ'");
}

// Test 15: Additional Consonants (nu, fu, ho, he)
void test_additional_consonants(void) {
    TEST_START("Additional Consonant Conversions");
    
    IMEState state;
    
    // Test 'nu'
    ime_init(&state);
    ime_set_mode(&state, MODE_HIRAGANA);
    ime_process_char(&state, 'n');
    ime_process_char(&state, 'u');
    TEST_ASSERT(strcmp(state.output, "ぬ") == 0, "'nu' converts to 'ぬ'");
    
    // Test 'fu'
    ime_init(&state);
    ime_set_mode(&state, MODE_HIRAGANA);
    ime_process_char(&state, 'f');
    ime_process_char(&state, 'u');
    TEST_ASSERT(strcmp(state.output, "ふ") == 0, "'fu' converts to 'ふ'");
    
    // Test 'ho'
    ime_init(&state);
    ime_set_mode(&state, MODE_HIRAGANA);
    ime_process_char(&state, 'h');
    ime_process_char(&state, 'o');
    TEST_ASSERT(strcmp(state.output, "ほ") == 0, "'ho' converts to 'ほ'");
    
    // Test 'he'
    ime_init(&state);
    ime_set_mode(&state, MODE_HIRAGANA);
    ime_process_char(&state, 'h');
    ime_process_char(&state, 'e');
    TEST_ASSERT(strcmp(state.output, "へ") == 0, "'he' converts to 'へ'");
}

// Test 16: Complete Word - Arigatou
void test_word_arigatou(void) {
    TEST_START("Complete Word - ありがとう");
    
    IMEState state;
    ime_init(&state);
    ime_set_mode(&state, MODE_HIRAGANA);
    
    // Type "arigatou"
    const char *word = "arigatou";
    for (size_t i = 0; i < strlen(word); i++) {
        ime_process_char(&state, word[i]);
    }
    
    TEST_ASSERT(strstr(state.output, "あ") != NULL, "Contains 'あ'");
    TEST_ASSERT(strstr(state.output, "り") != NULL, "Contains 'り'");
    TEST_ASSERT(strstr(state.output, "が") != NULL, "Contains 'が'");
    TEST_ASSERT(strstr(state.output, "と") != NULL, "Contains 'と'");
    TEST_ASSERT(strstr(state.output, "う") != NULL, "Contains 'う'");
}

// Test 17: Complete Word - Watashi
void test_word_watashi(void) {
    TEST_START("Complete Word - わたし");
    
    IMEState state;
    ime_init(&state);
    ime_set_mode(&state, MODE_HIRAGANA);
    
    // Type "watashi"
    const char *word = "watashi";
    for (size_t i = 0; i < strlen(word); i++) {
        ime_process_char(&state, word[i]);
    }
    
    TEST_ASSERT(strstr(state.output, "わ") != NULL, "Contains 'わ'");
    TEST_ASSERT(strstr(state.output, "た") != NULL, "Contains 'た'");
    TEST_ASSERT(strstr(state.output, "し") != NULL, "Contains 'し'");
}

// Test 18: Mode Preservation
void test_mode_preservation(void) {
    TEST_START("Mode Preservation");
    
    IMEState state;
    ime_init(&state);
    
    // Set to Katakana
    ime_set_mode(&state, MODE_KATAKANA);
    TEST_ASSERT(state.mode == MODE_KATAKANA, "Mode set to Katakana");
    
    // Type something
    ime_process_char(&state, 'a');
    
    // Mode should still be Katakana
    TEST_ASSERT(state.mode == MODE_KATAKANA, "Mode preserved after input");
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
    test_vowel_conversion();
    test_katakana_vowels();
    test_y_row_conversion();
    test_w_row_conversion();
    test_additional_consonants();
    test_word_arigatou();
    test_word_watashi();
    test_mode_preservation();
    
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
