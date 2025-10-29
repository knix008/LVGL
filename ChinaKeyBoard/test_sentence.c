#include <stdio.h>
#include <string.h>
#include "ime/libime/ime_korean.h"

int main() {
    printf("Testing sentence accumulation:\n");
    
    char committed_text[2048] = "";
    char stroke_buffer[256] = "";
    char output_buffer[256] = "";
    
    // Simulate typing "rk" (가)
    printf("Typing 'r':\n");
    stroke_buffer[0] = 'r';
    stroke_buffer[1] = '\0';
    cb_hangul_match_keystrokes(stroke_buffer, output_buffer, sizeof(output_buffer), 0, 0);
    printf("  Output: '%s'\n", output_buffer);
    if (strlen(output_buffer) > 0) {
        strcat(committed_text, output_buffer);
    }
    stroke_buffer[0] = '\0';
    printf("  Committed text: '%s'\n", committed_text);
    
    printf("Typing 'k':\n");
    stroke_buffer[0] = 'k';
    stroke_buffer[1] = '\0';
    cb_hangul_match_keystrokes(stroke_buffer, output_buffer, sizeof(output_buffer), 0, 0);
    printf("  Output: '%s'\n", output_buffer);
    if (strlen(output_buffer) > 0) {
        strcat(committed_text, output_buffer);
    }
    stroke_buffer[0] = '\0';
    printf("  Committed text: '%s'\n", committed_text);
    
    // Simulate typing "rk" again (가)
    printf("Typing 'r' again:\n");
    stroke_buffer[0] = 'r';
    stroke_buffer[1] = '\0';
    cb_hangul_match_keystrokes(stroke_buffer, output_buffer, sizeof(output_buffer), 0, 0);
    printf("  Output: '%s'\n", output_buffer);
    if (strlen(output_buffer) > 0) {
        strcat(committed_text, output_buffer);
    }
    stroke_buffer[0] = '\0';
    printf("  Committed text: '%s'\n", committed_text);
    
    printf("Typing 'k' again:\n");
    stroke_buffer[0] = 'k';
    stroke_buffer[1] = '\0';
    cb_hangul_match_keystrokes(stroke_buffer, output_buffer, sizeof(output_buffer), 0, 0);
    printf("  Output: '%s'\n", output_buffer);
    if (strlen(output_buffer) > 0) {
        strcat(committed_text, output_buffer);
    }
    stroke_buffer[0] = '\0';
    printf("  Committed text: '%s'\n", committed_text);
    
    printf("\nFinal result: '%s'\n", committed_text);
    printf("Expected: '가가' (two syllables)\n");
    
    return 0;
}
