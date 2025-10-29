#include <stdio.h>
#include <string.h>
#include "ime/libime/ime_korean.h"

int main() {
    char strokes[] = "rk";
    char output[256] = "";
    
    printf("Testing Korean combination: 'rk' -> '가'\n");
    printf("Input strokes: %s\n", strokes);
    
    cb_hangul_match_keystrokes(strokes, output, sizeof(output), 0, 0);
    
    printf("Output: %s\n", output);
    printf("Expected: 가\n");
    
    // Test a few more combinations
    printf("\nTesting more combinations:\n");
    
    char test_cases[][10] = {"rks", "rksr", "rksrk", "dk", "dkd"};
    char expected[][10] = {"간", "간ㄱ", "간가", "안", "안ㄷ"};
    
    for (int i = 0; i < 5; i++) {
        char result[256] = "";
        cb_hangul_match_keystrokes(test_cases[i], result, sizeof(result), 0, 0);
        printf("%s -> %s (expected: %s)\n", test_cases[i], result, expected[i]);
    }
    
    return 0;
}
