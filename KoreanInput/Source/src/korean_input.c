#include "korean_input.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Korean choseong (consonants) characters
static const char* korean_choseong[] = {
    "ㄱ", "ㄲ", "ㄴ", "ㄷ", "ㄸ", "ㄹ", "ㅁ", "ㅂ", "ㅃ", "ㅅ", 
    "ㅆ", "ㅇ", "ㅈ", "ㅉ", "ㅊ", "ㅋ", "ㅌ", "ㅍ", "ㅎ"
};

// Korean jungseong (vowels) characters
static const char* korean_jungseong[] = {
    "ㅏ", "ㅐ", "ㅑ", "ㅒ", "ㅓ", "ㅔ", "ㅕ", "ㅖ", "ㅗ", "ㅘ", 
    "ㅙ", "ㅚ", "ㅛ", "ㅜ", "ㅝ", "ㅞ", "ㅟ", "ㅠ", "ㅡ", "ㅢ", "ㅣ"
};

// Korean jongseong (final consonants) characters
static const char* korean_jongseong[] = {
    " ", "ㄱ", "ㄲ", "ㄳ", "ㄴ", "ㄵ", "ㄶ", "ㄷ", "ㄹ", "ㄺ", 
    "ㄻ", "ㄼ", "ㄽ", "ㄾ", "ㄿ", "ㅀ", "ㅁ", "ㅂ", "ㅄ", "ㅅ", 
    "ㅆ", "ㅇ", "ㅈ", "ㅊ", "ㅋ", "ㅌ", "ㅍ", "ㅎ"
};

// Function to convert Korean jamo to UTF-8 syllable
int korean_jamo_to_utf8_syllable(int choseong_idx, int jungseong_idx, int jongseong_idx, char* output, size_t output_size) {
    // Korean Unicode constants
    const int HANGUL_BASE = 0xAC00;  // 가 (first Korean syllable)
    const int CHOSEONG_BASE = 0x1100; // ㄱ (first choseong)
    const int JUNGSEONG_BASE = 0x1161; // ㅏ (first jungseong)
    const int JONGSEONG_BASE = 0x11A8; // ㄱ (first jongseong)
    
    // Choseong mapping (0-18 to Unicode choseong)
    const int choseong_unicode[] = {
        0x1100, 0x1101, 0x1102, 0x1103, 0x1104, 0x1105, 0x1106, 0x1107, 0x1108, 0x1109,
        0x110A, 0x110B, 0x110C, 0x110D, 0x110E, 0x110F, 0x1110, 0x1111, 0x1112
    };
    
    // Jungseong mapping (0-20 to Unicode jungseong)
    const int jungseong_unicode[] = {
        0x1161, 0x1162, 0x1163, 0x1164, 0x1165, 0x1166, 0x1167, 0x1168, 0x1169, 0x116A,
        0x116B, 0x116C, 0x116D, 0x116E, 0x116F, 0x1170, 0x1171, 0x1172, 0x1173, 0x1174,
        0x1175
    };
    
    // Jongseong mapping (0-26 to Unicode jongseong, 0 = no jongseong)
    const int jongseong_unicode[] = {
        0x0000, 0x11A8, 0x11A9, 0x11AA, 0x11AB, 0x11AC, 0x11AD, 0x11AE, 0x11AF, 0x11B0,
        0x11B1, 0x11B2, 0x11B3, 0x11B4, 0x11B5, 0x11B6, 0x11B7, 0x11B8, 0x11B9, 0x11BA,
        0x11BB, 0x11BC, 0x11BD, 0x11BE, 0x11BF, 0x11C0, 0x11C1, 0x11C2
    };
    
    // Validate indices
    if (choseong_idx < 0 || choseong_idx >= 19 || 
        jungseong_idx < 0 || jungseong_idx >= 21 ||
        jongseong_idx < 0 || jongseong_idx >= 28) {
        printf("Invalid Korean jamo indices: choseong=%d, jungseong=%d, jongseong=%d\n", 
               choseong_idx, jungseong_idx, jongseong_idx);
        return -1;
    }
    
    // Get Unicode values
    int choseong_uni = choseong_unicode[choseong_idx];
    int jungseong_uni = jungseong_unicode[jungseong_idx];
    int jongseong_uni = jongseong_unicode[jongseong_idx];
    
    // Calculate Korean syllable Unicode (fixed: use jongseong_idx directly)
    int syllable_uni = HANGUL_BASE + (choseong_idx * 21 + jungseong_idx) * 28 + jongseong_idx;
    
    printf("Converting jamo to UTF-8: choseong=%d(%04X), jungseong=%d(%04X), jongseong=%d(%04X) -> syllable=%04X\n",
           choseong_idx, choseong_uni, jungseong_idx, jungseong_uni, jongseong_idx, jongseong_uni, syllable_uni);
    
    // Convert Unicode to UTF-8
    if (syllable_uni <= 0x7F) {
        // 1-byte UTF-8
        if (output_size < 2) return -1;
        output[0] = (char)syllable_uni;
        output[1] = '\0';
    } else if (syllable_uni <= 0x7FF) {
        // 2-byte UTF-8
        if (output_size < 3) return -1;
        output[0] = 0xC0 | ((syllable_uni >> 6) & 0x1F);
        output[1] = 0x80 | (syllable_uni & 0x3F);
        output[2] = '\0';
    } else if (syllable_uni <= 0xFFFF) {
        // 3-byte UTF-8
        if (output_size < 4) return -1;
        output[0] = 0xE0 | ((syllable_uni >> 12) & 0x0F);
        output[1] = 0x80 | ((syllable_uni >> 6) & 0x3F);
        output[2] = 0x80 | (syllable_uni & 0x3F);
        output[3] = '\0';
    } else {
        // 4-byte UTF-8 (not needed for Korean)
        if (output_size < 5) return -1;
        output[0] = 0xF0 | ((syllable_uni >> 18) & 0x07);
        output[1] = 0x80 | ((syllable_uni >> 12) & 0x3F);
        output[2] = 0x80 | ((syllable_uni >> 6) & 0x3F);
        output[3] = 0x80 | (syllable_uni & 0x3F);
        output[4] = '\0';
    }
    
    printf("Generated UTF-8 syllable: %s (length: %zu)\n", output, strlen(output));
    return 0;
}

// Get Korean character arrays
const char** get_korean_choseong(void) {
    return korean_choseong;
}

const char** get_korean_jungseong(void) {
    return korean_jungseong;
}

const char** get_korean_jongseong(void) {
    return korean_jongseong;
}

// Get array sizes
size_t get_korean_choseong_size(void) {
    return sizeof(korean_choseong) / sizeof(korean_choseong[0]);
}

size_t get_korean_jungseong_size(void) {
    return sizeof(korean_jungseong) / sizeof(korean_jungseong[0]);
}

size_t get_korean_jongseong_size(void) {
    return sizeof(korean_jongseong) / sizeof(korean_jongseong[0]);
}

// Initialize Korean input state
void korean_input_init(korean_input_state_t* state) {
    if (state == NULL) return;
    
    state->choseong_index = 0;
    state->jungseong_index = 0;
    state->jongseong_index = 0;
    state->buffer[0] = '\0';
}

// Korean input operations
int korean_input_add_syllable(korean_input_state_t* state, char* output, size_t output_size) {
    if (state == NULL || output == NULL) return -1;
    
    // Create a complete Korean syllable using UTF-8 conversion
    char syllable[16] = "";
    int result = korean_jamo_to_utf8_syllable(state->choseong_index, state->jungseong_index, state->jongseong_index, syllable, sizeof(syllable));
    
    if (result == 0) {
        // Add the UTF-8 syllable to the buffer
        if (strlen(state->buffer) < sizeof(state->buffer) - strlen(syllable) - 1) {
            strcat(state->buffer, syllable);
            printf("Added UTF-8 syllable '%s', buffer now: '%s'\n", syllable, state->buffer);
            
            // Copy the current buffer to output
            if (strlen(state->buffer) < output_size) {
                strcpy(output, state->buffer);
                return 0;
            } else {
                printf("Output buffer too small\n");
                return -1;
            }
        } else {
            printf("Buffer full! Cannot add more characters.\n");
            return -1;
        }
    } else {
        printf("Error: Failed to convert jamo to UTF-8 syllable\n");
        return -1;
    }
}

int korean_input_backspace(korean_input_state_t* state) {
    if (state == NULL) return -1;
    
    printf("Backspace button clicked\n");
    int len = strlen(state->buffer);
    if (len > 0) {
        // Handle UTF-8 backspace properly
        // Find the start of the last UTF-8 character
        int i = len - 1;
        while (i > 0 && (state->buffer[i] & 0xC0) == 0x80) {
            i--; // Go back to find the start of the UTF-8 sequence
        }
        state->buffer[i] = '\0';
        printf("Removed UTF-8 character, buffer now: '%s'\n", state->buffer);
        return 0;
    }
    return -1;
}

void korean_input_clear(korean_input_state_t* state) {
    if (state == NULL) return;
    
    printf("Clear button clicked\n");
    state->buffer[0] = '\0';
}

// Navigation functions
void korean_input_next_choseong(korean_input_state_t* state) {
    if (state == NULL) return;
    
    printf("Next choseong button clicked, current index: %d\n", state->choseong_index);
    state->choseong_index = (state->choseong_index + 1) % get_korean_choseong_size();
    printf("New index: %d, array size: %zu\n", state->choseong_index, get_korean_choseong_size());
}

void korean_input_prev_choseong(korean_input_state_t* state) {
    if (state == NULL) return;
    
    printf("Previous choseong button clicked, current index: %d\n", state->choseong_index);
    state->choseong_index = (state->choseong_index - 1 + get_korean_choseong_size()) % get_korean_choseong_size();
    printf("New index: %d\n", state->choseong_index);
}

void korean_input_next_jungseong(korean_input_state_t* state) {
    if (state == NULL) return;
    
    printf("Next jungseong button clicked, current index: %d\n", state->jungseong_index);
    state->jungseong_index = (state->jungseong_index + 1) % get_korean_jungseong_size();
    printf("New jungseong index: %d, array size: %zu\n", state->jungseong_index, get_korean_jungseong_size());
}

void korean_input_prev_jungseong(korean_input_state_t* state) {
    if (state == NULL) return;
    
    printf("Previous jungseong button clicked, current index: %d\n", state->jungseong_index);
    state->jungseong_index = (state->jungseong_index - 1 + get_korean_jungseong_size()) % get_korean_jungseong_size();
    printf("New jungseong index: %d\n", state->jungseong_index);
}

void korean_input_next_jongseong(korean_input_state_t* state) {
    if (state == NULL) return;
    
    printf("Next jongseong button clicked, current index: %d\n", state->jongseong_index);
    state->jongseong_index = (state->jongseong_index + 1) % get_korean_jongseong_size();
    printf("New jongseong index: %d, array size: %zu\n", state->jongseong_index, get_korean_jongseong_size());
}

void korean_input_prev_jongseong(korean_input_state_t* state) {
    if (state == NULL) return;
    
    printf("Previous jongseong button clicked, current index: %d\n", state->jongseong_index);
    state->jongseong_index = (state->jongseong_index - 1 + get_korean_jongseong_size()) % get_korean_jongseong_size();
    printf("New jongseong index: %d\n", state->jongseong_index);
} 