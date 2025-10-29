#include <stdio.h>
#include <string.h>
#include "ime/libime/ime_korean.h"

int main() {
    printf("Testing Korean character mapping:\n");
    
    // Test individual character mapping
    int jamo_type, jamo_index;
    
    printf("Testing 'r' (should be consonant ㄱ):\n");
    if (korean_char_to_jamo('r', &jamo_type, &jamo_index)) {
        printf("  Type: %s, Index: %d\n", jamo_type == 0 ? "consonant" : "vowel", jamo_index);
    } else {
        printf("  Not recognized as Korean character\n");
    }
    
    printf("Testing 'k' (should be vowel ㅏ):\n");
    if (korean_char_to_jamo('k', &jamo_type, &jamo_index)) {
        printf("  Type: %s, Index: %d\n", jamo_type == 0 ? "consonant" : "vowel", jamo_index);
    } else {
        printf("  Not recognized as Korean character\n");
    }
    
    // Test syllable composition
    printf("\nTesting syllable composition:\n");
    int syllable = hangul_compose_syllable(0, 0, -1); // ㄱ + ㅏ + no final
    printf("ㄱ(0) + ㅏ(0) + no final = syllable code: %d (0x%04X)\n", syllable, syllable);
    
    // Test the actual IME function
    printf("\nTesting cb_hangul_match_keystrokes:\n");
    char strokes[] = "r";
    char output[256] = "";
    cb_hangul_match_keystrokes(strokes, output, sizeof(output), 0, 0);
    printf("'r' -> '%s'\n", output);
    
    strcpy(strokes, "rk");
    cb_hangul_match_keystrokes(strokes, output, sizeof(output), 0, 0);
    printf("'rk' -> '%s'\n", output);
    
    return 0;
}
