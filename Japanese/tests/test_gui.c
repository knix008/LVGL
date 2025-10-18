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
    
    // Test Japanese mode - should show only first character
    const wchar_t* japanese_text = get_button_text(MODE_JAPANESE, 0);
    TEST_ASSERT(japanese_text != NULL, "Japanese button text should not be NULL");
    TEST_ASSERT(wcslen(japanese_text) == 1, "Japanese button text should be single character");
    TEST_ASSERT(japanese_text[0] == L'あ', "First Japanese button should show あ");
    
    // Test Alphabet mode - now shows all characters
    const wchar_t* alphabet_text = get_button_text(MODE_ALPHABET, 0);
    TEST_ASSERT(alphabet_text != NULL, "Alphabet button text should not be NULL");
    TEST_ASSERT(wcslen(alphabet_text) > 1, "Alphabet button text should show all characters");
    TEST_ASSERT(wcschr(alphabet_text, L'a') != NULL, "Alphabet button should contain 'a'");
    
    // Test Number mode
    const wchar_t* number_text = get_button_text(MODE_NUMBER, 0);
    TEST_ASSERT(number_text != NULL, "Number button text should not be NULL");
    TEST_ASSERT(wcslen(number_text) == 1, "Number button text should be single character");
    TEST_ASSERT(number_text[0] == L'1', "First Number button should show '1'");
    
    // Test Symbol mode - now shows all characters
    const wchar_t* symbol_text = get_button_text(MODE_SYMBOL, 0);
    TEST_ASSERT(symbol_text != NULL, "Symbol button text should not be NULL");
    TEST_ASSERT(wcslen(symbol_text) > 1, "Symbol button text should show all characters");
    TEST_ASSERT(wcschr(symbol_text, L'！') != NULL, "Symbol button should contain ！");
}

// Test button text for different button numbers
void test_different_buttons() {
    printf("\n=== Testing Different Button Numbers ===\n");
    
    // Test Japanese buttons
    const wchar_t* button1 = get_button_text(MODE_JAPANESE, 1);
    TEST_ASSERT(button1[0] == L'か', "Japanese button 1 should show か");
    
    const wchar_t* button2 = get_button_text(MODE_JAPANESE, 2);
    TEST_ASSERT(button2[0] == L'さ', "Japanese button 2 should show さ");
    
    // Test Alphabet buttons (now shows all characters)
    const wchar_t* alphabet_btn1 = get_button_text(MODE_ALPHABET, 1);
    TEST_ASSERT(wcschr(alphabet_btn1, L'd') != NULL, "Alphabet button 1 should contain 'd'");
    
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
    const wchar_t* invalid_neg = get_button_text(MODE_JAPANESE, -1);
    TEST_ASSERT(wcslen(invalid_neg) == 0, "Invalid negative button should return empty string");
    
    const wchar_t* invalid_large = get_button_text(MODE_JAPANESE, 12);
    TEST_ASSERT(wcslen(invalid_large) == 0, "Invalid large button number should return empty string");
    
    // Test invalid modes (though this shouldn't happen in practice)
    const wchar_t* invalid_mode = get_button_text(MODE_COUNT, 0);
    TEST_ASSERT(wcslen(invalid_mode) == 0, "Invalid mode should return empty string");
}

// Test consistency between GUI and core functions
void test_consistency() {
    printf("\n=== Testing Consistency Between GUI and Core ===\n");
    
    // Test that GUI button text matches first character of core flick characters
    for (int mode = MODE_JAPANESE; mode < MODE_COUNT; mode++) {
        for (int button = 0; button < 12; button++) {
            const wchar_t* gui_text = get_button_text(mode, button);
            const wchar_t* core_chars = get_button_flick_chars(mode, button);
            
            if (core_chars && wcslen(core_chars) > 0) {
                // For symbol mode, expect all characters to be shown (if multiple characters)
                if (mode == MODE_SYMBOL) {
                    if (wcslen(core_chars) > 1) {
                        TEST_ASSERT(gui_text && wcslen(gui_text) > 1,
                            "GUI text should show all characters for multi-character symbol buttons");
                        TEST_ASSERT(wcschr(gui_text, core_chars[0]) != NULL,
                            "GUI text should contain first symbol character");
                    } else {
                        TEST_ASSERT(gui_text && wcslen(gui_text) == 1,
                            "GUI text should show single character for single-character symbol buttons");
                        TEST_ASSERT(gui_text[0] == core_chars[0],
                            "GUI text should match symbol character");
                    }
                } else {
                    TEST_ASSERT(gui_text && wcslen(gui_text) == 1, 
                        "GUI text should be single character");
                    TEST_ASSERT(gui_text[0] == core_chars[0], 
                        "GUI text should match first core character");
                }
            } else {
                // For alphabet and number modes, test differently
                if (mode == MODE_ALPHABET) {
                    const char* alphabet_chars = get_button_alphabet_chars(button);
                    if (alphabet_chars && strlen(alphabet_chars) > 0) {
                        if (strlen(alphabet_chars) > 1) {
                            TEST_ASSERT(gui_text && wcslen(gui_text) > 1,
                                "GUI text should show all characters for multi-character alphabet buttons");
                        } else {
                            TEST_ASSERT(gui_text && wcslen(gui_text) == 1,
                                "GUI text should show single character for single-character alphabet buttons");
                        }
                        TEST_ASSERT(wcschr(gui_text, (wchar_t)alphabet_chars[0]) != NULL,
                            "GUI text should contain first alphabet character");
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
    for (int mode = MODE_JAPANESE; mode < MODE_COUNT; mode++) {
        for (int button = 0; button < 12; button++) {
            const wchar_t* text = get_button_text(mode, button);
            TEST_ASSERT(text != NULL, "Button text should not be NULL for any valid mode/button");
            
            // Most buttons should have text, but some might be empty
            if (wcslen(text) > 0) {
                if (mode == MODE_ALPHABET) {
                    const char* alphabet_chars = get_button_alphabet_chars(button);
                    if (alphabet_chars && strlen(alphabet_chars) > 1) {
                        TEST_ASSERT(wcslen(text) > 1, "Multi-character alphabet button should show all characters");
                    } else {
                        TEST_ASSERT(wcslen(text) == 1, "Single-character alphabet button should show single character");
                    }
                } else if (mode == MODE_SYMBOL) {
                    const wchar_t* symbol_chars = get_button_flick_chars(mode, button);
                    if (symbol_chars && wcslen(symbol_chars) > 1) {
                        TEST_ASSERT(wcslen(text) > 1, "Multi-character symbol button should show all characters");
                    } else {
                        TEST_ASSERT(wcslen(text) == 1, "Single-character symbol button should show single character");
                    }
                } else {
                    TEST_ASSERT(wcslen(text) == 1, "Button text should be single character when not empty");
                }
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
