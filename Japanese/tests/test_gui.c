/*
 * Test cases for Japanese Input GUI Functions
 * Tests the GUI-related functionality
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

// Test button text generation for GUI
void test_gui_button_text() {
    printf("\n=== Testing GUI Button Text Generation ===\n");
    
    // Test Hiragana mode - should show only first character
    const wchar_t* hiragana_text = get_button_text(MODE_HIRAGANA, 0);
    TEST_ASSERT(hiragana_text != NULL, "Hiragana button text should not be NULL");
    TEST_ASSERT(wcslen(hiragana_text) == 1, "Hiragana button text should be single character");
    TEST_ASSERT(hiragana_text[0] == L'あ', "First Hiragana button should show あ");
    
    // Test Katakana mode
    const wchar_t* katakana_text = get_button_text(MODE_KATAKANA, 0);
    TEST_ASSERT(katakana_text != NULL, "Katakana button text should not be NULL");
    TEST_ASSERT(wcslen(katakana_text) == 1, "Katakana button text should be single character");
    TEST_ASSERT(katakana_text[0] == L'ア', "First Katakana button should show ア");
    
    // Test Alphabet mode
    const wchar_t* alphabet_text = get_button_text(MODE_ALPHABET, 0);
    TEST_ASSERT(alphabet_text != NULL, "Alphabet button text should not be NULL");
    TEST_ASSERT(wcslen(alphabet_text) == 1, "Alphabet button text should be single character");
    TEST_ASSERT(alphabet_text[0] == L'a', "First Alphabet button should show 'a'");
    
    // Test Number mode
    const wchar_t* number_text = get_button_text(MODE_NUMBER, 0);
    TEST_ASSERT(number_text != NULL, "Number button text should not be NULL");
    TEST_ASSERT(wcslen(number_text) == 1, "Number button text should be single character");
    TEST_ASSERT(number_text[0] == L'1', "First Number button should show '1'");
    
    // Test Symbol mode
    const wchar_t* symbol_text = get_button_text(MODE_SYMBOL, 0);
    TEST_ASSERT(symbol_text != NULL, "Symbol button text should not be NULL");
    TEST_ASSERT(wcslen(symbol_text) == 1, "Symbol button text should be single character");
    TEST_ASSERT(symbol_text[0] == L'！', "First Symbol button should show ！");
}

// Test button text for different button numbers
void test_different_buttons() {
    printf("\n=== Testing Different Button Numbers ===\n");
    
    // Test Hiragana buttons
    const wchar_t* button1 = get_button_text(MODE_HIRAGANA, 1);
    TEST_ASSERT(button1[0] == L'か', "Hiragana button 1 should show か");
    
    const wchar_t* button2 = get_button_text(MODE_HIRAGANA, 2);
    TEST_ASSERT(button2[0] == L'さ', "Hiragana button 2 should show さ");
    
    // Test Katakana buttons
    const wchar_t* katakana_btn1 = get_button_text(MODE_KATAKANA, 1);
    TEST_ASSERT(katakana_btn1[0] == L'カ', "Katakana button 1 should show カ");
    
    // Test Alphabet buttons
    const wchar_t* alphabet_btn1 = get_button_text(MODE_ALPHABET, 1);
    TEST_ASSERT(alphabet_btn1[0] == L'd', "Alphabet button 1 should show 'd'");
    
    // Test Number buttons
    const wchar_t* number_btn1 = get_button_text(MODE_NUMBER, 1);
    TEST_ASSERT(number_btn1[0] == L'2', "Number button 1 should show '2'");
    
    // Test Symbol buttons
    const wchar_t* symbol_btn1 = get_button_text(MODE_SYMBOL, 1);
    TEST_ASSERT(symbol_btn1[0] == L'（', "Symbol button 1 should show （");
}

// Test edge cases for GUI functions
void test_gui_edge_cases() {
    printf("\n=== Testing GUI Edge Cases ===\n");
    
    // Test invalid button numbers
    const wchar_t* invalid_neg = get_button_text(MODE_HIRAGANA, -1);
    TEST_ASSERT(wcslen(invalid_neg) == 0, "Invalid negative button should return empty string");
    
    const wchar_t* invalid_large = get_button_text(MODE_HIRAGANA, 12);
    TEST_ASSERT(wcslen(invalid_large) == 0, "Invalid large button number should return empty string");
    
    // Test invalid modes (though this shouldn't happen in practice)
    const wchar_t* invalid_mode = get_button_text(MODE_COUNT, 0);
    TEST_ASSERT(wcslen(invalid_mode) == 0, "Invalid mode should return empty string");
}

// Test consistency between GUI and core functions
void test_consistency() {
    printf("\n=== Testing Consistency Between GUI and Core ===\n");
    
    // Test that GUI button text matches first character of core flick characters
    for (int mode = MODE_HIRAGANA; mode < MODE_COUNT; mode++) {
        for (int button = 0; button < 12; button++) {
            const wchar_t* gui_text = get_button_text(mode, button);
            const wchar_t* core_chars = get_button_flick_chars(mode, button);
            
            if (core_chars && wcslen(core_chars) > 0) {
                TEST_ASSERT(gui_text && wcslen(gui_text) == 1, 
                    "GUI text should be single character");
                TEST_ASSERT(gui_text[0] == core_chars[0], 
                    "GUI text should match first core character");
            } else {
                // For alphabet and number modes, test differently
                if (mode == MODE_ALPHABET) {
                    const char* alphabet_chars = get_button_alphabet_chars(button);
                    if (alphabet_chars && strlen(alphabet_chars) > 0) {
                        TEST_ASSERT(gui_text && wcslen(gui_text) == 1,
                            "GUI text should be single character for alphabet");
                        TEST_ASSERT(gui_text[0] == (wchar_t)alphabet_chars[0],
                            "GUI text should match first alphabet character");
                    }
                } else if (mode == MODE_NUMBER) {
                    const char* number_chars = get_button_number_chars(button);
                    if (number_chars && strlen(number_chars) > 0) {
                        TEST_ASSERT(gui_text && wcslen(gui_text) == 1,
                            "GUI text should be single character for number");
                        TEST_ASSERT(gui_text[0] == (wchar_t)number_chars[0],
                            "GUI text should match first number character");
                    }
                }
            }
        }
    }
}

// Test button text for all modes
void test_all_modes() {
    printf("\n=== Testing All Input Modes ===\n");
    
    // Test that all modes return valid button text
    for (int mode = MODE_HIRAGANA; mode < MODE_COUNT; mode++) {
        for (int button = 0; button < 12; button++) {
            const wchar_t* text = get_button_text(mode, button);
            TEST_ASSERT(text != NULL, "Button text should not be NULL for any valid mode/button");
            
            // Most buttons should have text, but some might be empty
            if (wcslen(text) > 0) {
                TEST_ASSERT(wcslen(text) == 1, "Button text should be single character when not empty");
            }
        }
    }
}

// Main test runner
int main() {
    printf("Japanese Input GUI Tests\n");
    printf("========================\n");
    
    test_gui_button_text();
    test_different_buttons();
    test_gui_edge_cases();
    test_consistency();
    test_all_modes();
    
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
