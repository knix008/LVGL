#include <stdio.h>
#include <string.h>
#include "ime/libime/ime_korean.h"

int main() {
    printf("Testing intermediate character display:\n");
    
    char test_strokes[] = "r";
    char output[256] = "";
    char intermediate[256] = "";
    
    printf("Testing stroke: '%s'\n", test_strokes);
    
    // Test IME output
    cb_hangul_match_keystrokes(test_strokes, output, sizeof(output), 0, 0);
    printf("IME output: '%s'\n", output);
    
    // Test intermediate character conversion
    for (int i = 0; test_strokes[i] != '\0'; i++) {
        char stroke_char = test_strokes[i];
        int jamo_type, jamo_index;
        
        if (korean_char_to_jamo(stroke_char, &jamo_type, &jamo_index)) {
            if (jamo_type == 0) { // consonant
                const char* consonant_chars[] = {
                    "ㄱ", "ㄲ", "ㄴ", "ㄷ", "ㄸ", "ㄹ", "ㅁ", "ㅂ", "ㅃ", "ㅅ", 
                    "ㅆ", "ㅇ", "ㅈ", "ㅉ", "ㅊ", "ㅋ", "ㅌ", "ㅍ", "ㅎ"
                };
                if (jamo_index >= 0 && jamo_index < 19) {
                    strcat(intermediate, consonant_chars[jamo_index]);
                }
            } else if (jamo_type == 1) { // vowel
                const char* vowel_chars[] = {
                    "ㅏ", "ㅐ", "ㅑ", "ㅒ", "ㅓ", "ㅔ", "ㅕ", "ㅖ", "ㅗ", "ㅘ",
                    "ㅙ", "ㅚ", "ㅛ", "ㅜ", "ㅝ", "ㅞ", "ㅟ", "ㅠ", "ㅡ", "ㅢ", "ㅣ"
                };
                if (jamo_index >= 0 && jamo_index < 21) {
                    strcat(intermediate, vowel_chars[jamo_index]);
                }
            }
        }
    }
    
    printf("Intermediate chars: '%s'\n", intermediate);
    
    // Test with "rk"
    printf("\nTesting stroke: 'rk'\n");
    strcpy(test_strokes, "rk");
    output[0] = '\0';
    intermediate[0] = '\0';
    
    cb_hangul_match_keystrokes(test_strokes, output, sizeof(output), 0, 0);
    printf("IME output: '%s'\n", output);
    
    // Convert each stroke
    for (int i = 0; test_strokes[i] != '\0'; i++) {
        char stroke_char = test_strokes[i];
        int jamo_type, jamo_index;
        
        if (korean_char_to_jamo(stroke_char, &jamo_type, &jamo_index)) {
            if (jamo_type == 0) { // consonant
                const char* consonant_chars[] = {
                    "ㄱ", "ㄲ", "ㄴ", "ㄷ", "ㄸ", "ㄹ", "ㅁ", "ㅂ", "ㅃ", "ㅅ", 
                    "ㅆ", "ㅇ", "ㅈ", "ㅉ", "ㅊ", "ㅋ", "ㅌ", "ㅍ", "ㅎ"
                };
                if (jamo_index >= 0 && jamo_index < 19) {
                    strcat(intermediate, consonant_chars[jamo_index]);
                }
            } else if (jamo_type == 1) { // vowel
                const char* vowel_chars[] = {
                    "ㅏ", "ㅐ", "ㅑ", "ㅒ", "ㅓ", "ㅔ", "ㅕ", "ㅖ", "ㅗ", "ㅘ",
                    "ㅙ", "ㅚ", "ㅛ", "ㅜ", "ㅝ", "ㅞ", "ㅟ", "ㅠ", "ㅡ", "ㅢ", "ㅣ"
                };
                if (jamo_index >= 0 && jamo_index < 21) {
                    strcat(intermediate, vowel_chars[jamo_index]);
                }
            }
        }
    }
    
    printf("Intermediate chars: '%s'\n", intermediate);
    
    return 0;
}
