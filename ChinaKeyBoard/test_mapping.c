#include <stdio.h>
#include <string.h>
#include "ime/libime/ime_korean.h"

int main() {
    printf("Testing Korean IME mapping:\n");
    
    // Test individual characters
    int jamo_type, jamo_index;
    
    printf("Testing 'q' (should be ㅂ):\n");
    if (korean_char_to_jamo('q', &jamo_type, &jamo_index)) {
        printf("  Type: %s, Index: %d\n", jamo_type == 0 ? "consonant" : "vowel", jamo_index);
    }
    
    printf("Testing 'j' (should be ㅔ):\n");
    if (korean_char_to_jamo('j', &jamo_type, &jamo_index)) {
        printf("  Type: %s, Index: %d\n", jamo_type == 0 ? "consonant" : "vowel", jamo_index);
    }
    
    printf("Testing 'O' (should be ㅓ):\n");
    if (korean_char_to_jamo('O', &jamo_type, &jamo_index)) {
        printf("  Type: %s, Index: %d\n", jamo_type == 0 ? "consonant" : "vowel", jamo_index);
    }
    
    // Test combinations
    printf("\nTesting combinations:\n");
    char test_cases[][10] = {"qj", "qO", "qk"};
    for (int i = 0; i < 3; i++) {
        char output[256] = "";
        cb_hangul_match_keystrokes(test_cases[i], output, sizeof(output), 0, 0);
        printf("'%s' -> '%s'\n", test_cases[i], output);
    }
    
    return 0;
}
