/**
 * @file test_clear_functionality.c
 * @brief Test program to verify the clear functionality works correctly
 * @version 1.0.0
 * @date 2024
 * 
 * This program tests that the clear function properly clears both the output buffer
 * and should result in an empty textbox display.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include "chunjiin_input.h"

// Test function to verify clear functionality
void test_clear_functionality() {
    printf("Testing clear functionality...\n");
    
    // Initialize the system
    initialize();
    
    // Input some Korean characters (example: 가)
    printf("1. Inputting characters: g (ㄱ), a (ㆍ)...\n");
    process_input('g');  // ㄱ
    process_input('a');  // ㆍ -> should form 가
    
    // Get current text before clear
    wchar_t before_clear[1024] = {0};
    chunjiin_get_current_text(before_clear);
    
    printf("   Text before clear: ");
    for (size_t i = 0; i < wcslen(before_clear); i++) {
        printf("U+%04X ", (unsigned int)before_clear[i]);
    }
    printf("\n");
    
    // Test clear functionality (mapped to '.' character)
    printf("2. Calling clear function (process_input('.'))...\n");
    process_input('.');
    
    // Get current text after clear
    wchar_t after_clear[1024] = {0};
    chunjiin_get_current_text(after_clear);
    
    printf("   Text after clear: ");
    if (wcslen(after_clear) == 0) {
        printf("(empty - CORRECT!)\n");
    } else {
        printf("NOT EMPTY - ERROR: ");
        for (size_t i = 0; i < wcslen(after_clear); i++) {
            printf("U+%04X ", (unsigned int)after_clear[i]);
        }
        printf("\n");
    }
    
    // Verify that the buffer is truly empty
    if (wcslen(after_clear) == 0) {
        printf("✓ Clear functionality working correctly!\n");
        printf("✓ The textbox should now display an empty string.\n");
    } else {
        printf("✗ Clear functionality NOT working - buffer still contains text!\n");
    }
    
    printf("\nTest completed.\n");
}

int main() {
    // Set locale for Korean support
    setlocale(LC_ALL, "ko_KR.UTF-8");
    
    printf("ChunJiIn Clear Functionality Test\n");
    printf("==================================\n\n");
    
    test_clear_functionality();
    
    return 0;
}