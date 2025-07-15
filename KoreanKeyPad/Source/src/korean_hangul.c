#include "korean_hangul.h"
#include <string.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include "../lvgl/src/libs/freetype/lv_freetype.h"

// Global variables for Korean keypad
char korean_keypad_buffer[128] = "";
korean_keypad_state_t korean_keypad_state;
lv_font_t * korean_font = NULL;

// Korean character mappings
const char* hangul_choseong[] = {
    "ㄱ", "ㄲ", "ㄴ", "ㄷ", "ㄸ", "ㄹ", "ㅁ", "ㅂ", "ㅃ", "ㅅ", "ㅆ", "ㅇ", "ㅈ", "ㅉ", "ㅊ", "ㅋ", "ㅌ", "ㅍ", "ㅎ"
};

const char* hangul_jungseong[] = {
    "ㅏ", "ㅐ", "ㅑ", "ㅒ", "ㅓ", "ㅔ", "ㅕ", "ㅖ", "ㅗ", "ㅘ", "ㅙ", "ㅚ", "ㅛ", "ㅜ", "ㅝ", "ㅞ", "ㅟ", "ㅠ", "ㅡ", "ㅢ", "ㅣ"
};

const char* hangul_jongseong[] = {
    "", "ㄱ", "ㄲ", "ㄳ", "ㄴ", "ㄵ", "ㄶ", "ㄷ", "ㄹ", "ㄺ", "ㄻ", "ㄼ", "ㄽ", "ㄾ", "ㄿ", "ㅀ", "ㅁ", "ㅂ", "ㅄ", "ㅅ", "ㅆ", "ㅇ", "ㅈ", "ㅊ", "ㅋ", "ㅌ", "ㅍ", "ㅎ"
};

const char* english_chars[] = {
    "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P",
    "A", "S", "D", "F", "G", "H", "J", "K", "L",
    "Z", "X", "C", "V", "B", "N", "M"
};

const char* number_chars[] = {
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "0"
};

// Initialize FreeType font for Korean text
void init_korean_font(void) {
    if (korean_font == NULL) {
        lv_freetype_init(256); // cache size
        
        korean_font = lv_freetype_font_create("../assets/NanumGothic-Regular.ttf", 
                                             LV_FREETYPE_FONT_RENDER_MODE_BITMAP, 
                                             16, // font size in px
                                             LV_FREETYPE_FONT_STYLE_NORMAL);
        
        if (korean_font == NULL) {
            printf("Warning: Failed to load NanumGothic font, using default font\n");
        } else {
            printf("Successfully loaded NanumGothic font\n");
        }
    }
}

// Initialize Korean keypad
void init_korean_keypad(void) {
    memset(&korean_keypad_state, 0, sizeof(korean_keypad_state));
    korean_keypad_state.mode = KOREAN_MODE_HANGUL;
    korean_keypad_buffer[0] = '\0';
    reset_hangul_composition(&korean_keypad_state.composition);
    
    // Initialize FreeType font
    init_korean_font();
}

// Reset Hangul composition
void reset_hangul_composition(hangul_composition_t * comp) {
    comp->choseong = 0;
    comp->jungseong = 0;
    comp->jongseong = 0;
    comp->has_choseong = false;
    comp->has_jungseong = false;
    comp->has_jongseong = false;
}

// Add choseong (initial consonant)
void add_choseong(hangul_composition_t * comp, uint16_t choseong) {
    if (!comp->has_choseong) {
        comp->choseong = choseong;
        comp->has_choseong = true;
    }
}

// Add jungseong (vowel)
void add_jungseong(hangul_composition_t * comp, uint16_t jungseong) {
    if (!comp->has_jungseong) {
        comp->jungseong = jungseong;
        comp->has_jungseong = true;
    }
}

// Add jongseong (final consonant)
void add_jongseong(hangul_composition_t * comp, uint16_t jongseong) {
    if (!comp->has_jongseong && comp->has_choseong && comp->has_jungseong) {
        comp->jongseong = jongseong;
        comp->has_jongseong = true;
    }
}

// Compose Hangul syllable from components
uint32_t compose_hangul_syllable(hangul_composition_t * comp) {
    if (!comp->has_choseong || !comp->has_jungseong) {
        return 0;
    }
    
    // Calculate syllable code
    uint32_t choseong_idx = comp->choseong - HANGUL_CHOSEONG_START;
    uint32_t jungseong_idx = comp->jungseong - HANGUL_JUNGSEONG_START;
    uint32_t jongseong_idx = comp->has_jongseong ? (comp->jongseong - HANGUL_JONGSEONG_START + 1) : 0;
    
    return HANGUL_SYLLABLE_START + (choseong_idx * 21 + jungseong_idx) * 28 + jongseong_idx;
}

// Update Korean display
void update_korean_display(void) {
    if (korean_keypad_state.display_label) {
        lv_label_set_text(korean_keypad_state.display_label, korean_keypad_buffer);
    }
}

// Add character to buffer
void add_char_to_buffer(const char * str) {
    int len = strlen(korean_keypad_buffer);
    int str_len = strlen(str);
    
    if (len + str_len < sizeof(korean_keypad_buffer) - 1) {
        strcat(korean_keypad_buffer, str);
        update_korean_display();
    }
}

// Remove character from buffer
void remove_char_from_buffer(void) {
    int len = strlen(korean_keypad_buffer);
    if (len > 0) {
        // Handle UTF-8 multi-byte characters
        while (len > 0 && (korean_keypad_buffer[len - 1] & 0xC0) == 0x80) {
            len--;
        }
        if (len > 0) {
            korean_keypad_buffer[len - 1] = '\0';
        }
        update_korean_display();
    }
}

// Process Hangul character input
void process_hangul_character(int char_index) {
    if (korean_keypad_state.mode == KOREAN_MODE_HANGUL) {
        if (char_index < 19) {
            // Choseong (initial consonant)
            add_choseong(&korean_keypad_state.composition, HANGUL_CHOSEONG_START + char_index);
        } else if (char_index >= 100 && char_index < 121) {
            // Jungseong (vowel)
            add_jungseong(&korean_keypad_state.composition, HANGUL_JUNGSEONG_START + (char_index - 100));
        } else if (char_index >= 200 && char_index < 229) {
            // Jongseong (final consonant)
            add_jongseong(&korean_keypad_state.composition, HANGUL_JONGSEONG_START + (char_index - 200));
        }
        
        // Try to compose syllable
        uint32_t syllable = compose_hangul_syllable(&korean_keypad_state.composition);
        if (syllable > 0) {
            char utf8_char[8];
            int len = 0;
            
            // Convert Unicode to UTF-8
            if (syllable < 0x80) {
                utf8_char[len++] = syllable;
            } else if (syllable < 0x800) {
                utf8_char[len++] = 0xC0 | (syllable >> 6);
                utf8_char[len++] = 0x80 | (syllable & 0x3F);
            } else if (syllable < 0x10000) {
                utf8_char[len++] = 0xE0 | (syllable >> 12);
                utf8_char[len++] = 0x80 | ((syllable >> 6) & 0x3F);
                utf8_char[len++] = 0x80 | (syllable & 0x3F);
            }
            utf8_char[len] = '\0';
            
            add_char_to_buffer(utf8_char);
            reset_hangul_composition(&korean_keypad_state.composition);
        }
    }
} 