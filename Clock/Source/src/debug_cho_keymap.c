#include "qwerty_korean.h"
#include <stdio.h>

int main() {
    printf("Debug cho_keymap array:\n");
    printf("======================\n");
    
    // Print all entries in cho_keymap
    for (int i = 0; i < 28; i++) {
        printf("Index %d: key='%s', jamo='%s'\n", i, cho_keymap[i].key, cho_keymap[i].jamo);
    }
    
    // Test specific characters
    printf("\nTesting specific characters:\n");
    printf("'g' -> %s\n", qwerty_get_jamo_buffer("g", cho_keymap, 27));
    printf("'G' -> %s\n", qwerty_get_jamo_buffer("G", cho_keymap, 27));
    printf("'r' -> %s\n", qwerty_get_jamo_buffer("r", cho_keymap, 27));
    printf("'R' -> %s\n", qwerty_get_jamo_buffer("R", cho_keymap, 27));
    
    return 0;
} 