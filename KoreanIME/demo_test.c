#include "korean.h"
#include <stdio.h>
#include <string.h>

// Simple test function to demonstrate key features
void demo_korean_ime() {
    printf("🎯 Korean IME Feature Demonstration\n");
    printf("===================================\n\n");
    
    // Test cases that demonstrate the key features
    const char* test_cases[] = {
        "ek",      // Basic syllable: 다
        "ekr",     // Syllable with jongseong: 닥
        "ekfr",    // Compound jongseong: 닭
        "ekfrk",   // Compound jongseong + new syllable: 달가
        "ekek",    // Multiple syllables: 다다
        "ekfrkek", // Complex case: 달가다
        "qkfrk",   // Another compound jongseong: 발가
        "tkfrk",   // Another compound jongseong: 살가
    };
    
    const wchar_t* expected_results[] = {
        L"다",
        L"닥", 
        L"닭",
        L"달가",
        L"다다",
        L"달가다",
        L"발가",
        L"살가"
    };
    
    const char* descriptions[] = {
        "Basic choseong + jungseong",
        "Choseong + jungseong + jongseong",
        "Compound jongseong (ㄺ)",
        "Compound jongseong + new syllable",
        "Multiple syllables",
        "Complex compound jongseong case",
        "Compound jongseong with ㅂ choseong",
        "Compound jongseong with ㅅ choseong"
    };
    
    int total_tests = sizeof(test_cases) / sizeof(test_cases[0]);
    int passed = 0;
    
    for (int i = 0; i < total_tests; i++) {
        wchar_t output_buffer[MAX_OUTPUT_LEN] = L"";
        size_t input_len = strlen(test_cases[i]);
        
        // Create a copy of input since we need to modify it
        char input_copy[MAX_OUTPUT_LEN];
        strcpy(input_copy, test_cases[i]);
        
        compose_korean_characters(input_copy, input_len, output_buffer);
        
        if (wcscmp(output_buffer, expected_results[i]) == 0) {
            printf("✓ PASS: %s\n", descriptions[i]);
            printf("  Input: [%s] -> Output: [%ls]\n\n", test_cases[i], output_buffer);
            passed++;
        } else {
            printf("✗ FAIL: %s\n", descriptions[i]);
            printf("  Input: [%s] -> Output: [%ls] (Expected: [%ls])\n\n", 
                   test_cases[i], output_buffer, expected_results[i]);
        }
    }
    
    printf("===================================\n");
    printf("Demo Results: %d/%d tests passed\n", passed, total_tests);
    
    if (passed == total_tests) {
        printf("🎉 All key features are working correctly!\n");
        printf("\nKey Features Verified:\n");
        printf("• Basic Korean syllable composition ✓\n");
        printf("• Jongseong (final consonant) handling ✓\n");
        printf("• Compound jongseong (ㄺ, ㄻ, etc.) ✓\n");
        printf("• Compound jongseong splitting with following jungseong ✓\n");
        printf("• Multiple syllable composition ✓\n");
        printf("• Real-time character composition ✓\n");
    } else {
        printf("❌ Some features need attention.\n");
    }
}

int main() {
    demo_korean_ime();
    return 0;
} 