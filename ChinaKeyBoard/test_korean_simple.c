/*
 * Simple test for Korean IME
 * Tests basic Korean character composition
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#include "ime/libime/ime_korean.h"

// Convert wide character to UTF-8 for printing
void wchar_to_utf8_print(wchar_t* wstr) {
    if (wstr == NULL || wcslen(wstr) == 0) {
        printf("(empty)");
        return;
    }
    
    char utf8[256] = {0};
    unicode_to_utf8(wstr, utf8, sizeof(utf8));
    printf("%s", utf8);
}

int main(void) {
    printf("Korean IME Simple Test\n");
    printf("======================\n\n");
    
    // Initialize Korean IME
    qwerty_korean_init();
    
    // Test cases
    char* test_inputs[] = {
        "r",      // ㄱ
        "rk",     // 가
        "sj",     // 너
        "el",     // 디
        "fh",     // 로
        "an",     // 무
        "Rk",     // 까
        "Ej",     // 떠
        "rhk",    // 과
        "rkr",    // 각
    };
    
    int num_tests = sizeof(test_inputs) / sizeof(test_inputs[0]);
    
    printf("Testing Korean character composition:\n\n");
    for (int i = 0; i < num_tests; i++) {
        wchar_t output[MAX_OUTPUT_LEN];
        output[0] = L'\0';
        
        qwerty_compose_korean_characters(test_inputs[i], strlen(test_inputs[i]), output);
        
        printf("Input: %s -> Output: ", test_inputs[i]);
        wchar_to_utf8_print(output);
        printf("\n");
    }
    
    printf("\n✓ Test complete\n");
    
    // Cleanup
    qwerty_korean_cleanup();
    
    return 0;
}

