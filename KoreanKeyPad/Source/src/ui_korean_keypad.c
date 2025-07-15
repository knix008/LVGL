#include "ui_korean_keypad.h"
#include <string.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include "../lvgl/src/libs/freetype/lv_freetype.h"

// Forward declaration
static void create_hangul_buttons(lv_obj_t * parent);

// Global variables for Korean keypad
char korean_keypad_buffer[128] = "";
lv_obj_t * korean_keypad_display_label = NULL;
korean_keypad_state_t korean_keypad_state;
lv_font_t * korean_font = NULL;

// Hangul Unicode ranges
#define HANGUL_CHOSEONG_START 0x1100
#define HANGUL_JUNGSEONG_START 0x1161
#define HANGUL_JONGSEONG_START 0x11A8
#define HANGUL_SYLLABLE_START 0xAC00

// Korean character mappings
static const char* hangul_choseong[] = {
    "ㄱ", "ㄲ", "ㄴ", "ㄷ", "ㄸ", "ㄹ", "ㅁ", "ㅂ", "ㅃ", "ㅅ", "ㅆ", "ㅇ", "ㅈ", "ㅉ", "ㅊ", "ㅋ", "ㅌ", "ㅍ", "ㅎ"
};

static const char* hangul_jungseong[] = {
    "ㅏ", "ㅐ", "ㅑ", "ㅒ", "ㅓ", "ㅔ", "ㅕ", "ㅖ", "ㅗ", "ㅘ", "ㅙ", "ㅚ", "ㅛ", "ㅜ", "ㅝ", "ㅞ", "ㅟ", "ㅠ", "ㅡ", "ㅢ", "ㅣ"
};

static const char* hangul_jongseong[] = {
    "", "ㄱ", "ㄲ", "ㄳ", "ㄴ", "ㄵ", "ㄶ", "ㄷ", "ㄹ", "ㄺ", "ㄻ", "ㄼ", "ㄽ", "ㄾ", "ㄿ", "ㅀ", "ㅁ", "ㅂ", "ㅄ", "ㅅ", "ㅆ", "ㅇ", "ㅈ", "ㅊ", "ㅋ", "ㅌ", "ㅍ", "ㅎ"
};

static const char* english_chars[] = {
    "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P",
    "A", "S", "D", "F", "G", "H", "J", "K", "L",
    "Z", "X", "C", "V", "B", "N", "M"
};

static const char* number_chars[] = {
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "0"
};

// Initialize FreeType font for Korean text
static void init_korean_font(void) {
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

// Create Korean keypad UI components
void create_korean_keypad_ui(lv_obj_t * parent) {
    // Safety check for parent object
    if (parent == NULL) {
        printf("Error: Korean keypad parent is NULL\n");
        return;
    }
    
    printf("Creating Korean keypad UI with parent: %p\n", (void*)parent);
    
    // Keypad title
    lv_obj_t * keypad_label = lv_label_create(parent);
    if (keypad_label == NULL) {
        printf("Error: Failed to create keypad label\n");
        return;
    }
    lv_label_set_text(keypad_label, "한글 키패드");
    if (korean_font != NULL) {
        lv_obj_set_style_text_font(keypad_label, korean_font, 0);
    }
    lv_obj_align(keypad_label, LV_ALIGN_TOP_MID, 0, 10);

    // Display area for entered text
    lv_obj_t * keypad_display = lv_label_create(parent);
    if (keypad_display == NULL) {
        printf("Error: Failed to create keypad display\n");
        return;
    }
    lv_label_set_text(keypad_display, "");
    lv_obj_set_style_bg_color(keypad_display, lv_color_hex(0xF0F0F0), 0);
    lv_obj_set_style_bg_opa(keypad_display, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(keypad_display, lv_color_hex(0xCCCCCC), 0);
    lv_obj_set_style_border_width(keypad_display, 1, 0);
    lv_obj_set_style_pad_all(keypad_display, 10, 0);
    lv_obj_set_size(keypad_display, 280, 50);
    lv_obj_align(keypad_display, LV_ALIGN_TOP_MID, 0, 40);
    if (korean_font != NULL) {
        lv_obj_set_style_text_font(keypad_display, korean_font, 0);
    }
    korean_keypad_display_label = keypad_display;
    korean_keypad_state.display_label = keypad_display;

    // Mode switch button
    lv_obj_t * mode_btn = lv_btn_create(parent);
    if (mode_btn == NULL) {
        printf("Error: Failed to create mode button\n");
        return;
    }
    lv_obj_set_size(mode_btn, 80, 30);
    lv_obj_align(mode_btn, LV_ALIGN_TOP_LEFT, 10, 100);
    lv_obj_set_style_bg_color(mode_btn, lv_color_hex(0x4CAF50), 0);
    
    lv_obj_t * mode_label = lv_label_create(mode_btn);
    if (mode_label == NULL) {
        printf("Error: Failed to create mode label\n");
        return;
    }
    lv_label_set_text(mode_label, "한글");
    if (korean_font != NULL) {
        lv_obj_set_style_text_font(mode_label, korean_font, 0);
    }
    lv_obj_center(mode_label);
    lv_obj_add_event_cb(mode_btn, korean_keypad_mode_cb, LV_EVENT_CLICKED, NULL);
    korean_keypad_state.mode_btn = mode_btn;

    printf("Creating Hangul buttons...\n");
    // Create character buttons based on current mode
    create_hangul_buttons(parent);

    // Function buttons row
    int btn_width = 60;
    int btn_height = 40;
    int start_x = 10;
    int start_y = 280;

    // Clear button
    lv_obj_t * clear_btn = lv_btn_create(parent);
    if (clear_btn == NULL) {
        printf("Error: Failed to create clear button\n");
        return;
    }
    lv_obj_set_size(clear_btn, btn_width, btn_height);
    lv_obj_align(clear_btn, LV_ALIGN_TOP_LEFT, start_x, start_y);
    lv_obj_set_style_bg_color(clear_btn, lv_color_hex(0xFF5722), 0);
    
    lv_obj_t * clear_label = lv_label_create(clear_btn);
    if (clear_label == NULL) {
        printf("Error: Failed to create clear label\n");
        return;
    }
    lv_label_set_text(clear_label, "Clear");
    lv_obj_center(clear_label);
    lv_obj_add_event_cb(clear_btn, korean_keypad_clear_cb, LV_EVENT_CLICKED, NULL);

    // Backspace button
    lv_obj_t * backspace_btn = lv_btn_create(parent);
    if (backspace_btn == NULL) {
        printf("Error: Failed to create backspace button\n");
        return;
    }
    lv_obj_set_size(backspace_btn, btn_width, btn_height);
    lv_obj_align(backspace_btn, LV_ALIGN_TOP_LEFT, start_x + btn_width + 5, start_y);
    lv_obj_set_style_bg_color(backspace_btn, lv_color_hex(0xFF9800), 0);
    
    lv_obj_t * backspace_label = lv_label_create(backspace_btn);
    if (backspace_label == NULL) {
        printf("Error: Failed to create backspace label\n");
        return;
    }
    lv_label_set_text(backspace_label, "⌫");
    lv_obj_center(backspace_label);
    lv_obj_add_event_cb(backspace_btn, korean_keypad_backspace_cb, LV_EVENT_CLICKED, NULL);

    // Space button
    lv_obj_t * space_btn = lv_btn_create(parent);
    if (space_btn == NULL) {
        printf("Error: Failed to create space button\n");
        return;
    }
    lv_obj_set_size(space_btn, btn_width * 2 + 5, btn_height);
    lv_obj_align(space_btn, LV_ALIGN_TOP_LEFT, start_x + 2 * (btn_width + 5), start_y);
    lv_obj_set_style_bg_color(space_btn, lv_color_hex(0x2196F3), 0);
    
    lv_obj_t * space_label = lv_label_create(space_btn);
    if (space_label == NULL) {
        printf("Error: Failed to create space label\n");
        return;
    }
    lv_label_set_text(space_label, "Space");
    lv_obj_center(space_label);
    lv_obj_add_event_cb(space_btn, korean_keypad_space_cb, LV_EVENT_CLICKED, NULL);

    // Enter button
    lv_obj_t * enter_btn = lv_btn_create(parent);
    if (enter_btn == NULL) {
        printf("Error: Failed to create enter button\n");
        return;
    }
    lv_obj_set_size(enter_btn, btn_width, btn_height);
    lv_obj_align(enter_btn, LV_ALIGN_TOP_LEFT, start_x + 4 * (btn_width + 5), start_y);
    lv_obj_set_style_bg_color(enter_btn, lv_color_hex(0x4CAF50), 0);
    
    lv_obj_t * enter_label = lv_label_create(enter_btn);
    if (enter_label == NULL) {
        printf("Error: Failed to create enter label\n");
        return;
    }
    lv_label_set_text(enter_label, "Enter");
    lv_obj_center(enter_label);
    lv_obj_add_event_cb(enter_btn, korean_keypad_enter_cb, LV_EVENT_CLICKED, NULL);
    
    printf("Korean keypad UI created successfully\n");
}

// Create Hangul character buttons
static void create_hangul_buttons(lv_obj_t * parent) {
    // Safety check for parent object
    if (parent == NULL) {
        printf("Error: Hangul buttons parent is NULL\n");
        return;
    }
    
    int btn_width = 35;
    int btn_height = 35;
    int start_x = 10;
    int start_y = 140;

    // First row: ㄱㄲㄴㄷㄸㄹㅁㅂㅃ
    for (int i = 0; i < 9; i++) {
        lv_obj_t * btn = lv_btn_create(parent);
        if (btn == NULL) {
            printf("Error: Failed to create button %d in first row\n", i);
            continue;
        }
        lv_obj_set_size(btn, btn_width, btn_height);
        lv_obj_align(btn, LV_ALIGN_TOP_LEFT, start_x + i * (btn_width + 2), start_y);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0xE3F2FD), 0);
        
        lv_obj_t * btn_label = lv_label_create(btn);
        if (btn_label == NULL) {
            printf("Error: Failed to create label for button %d in first row\n", i);
            continue;
        }
        lv_label_set_text(btn_label, hangul_choseong[i]);
        if (korean_font != NULL) {
            lv_obj_set_style_text_font(btn_label, korean_font, 0);
        }
        lv_obj_center(btn_label);
        lv_obj_add_event_cb(btn, korean_keypad_char_cb, LV_EVENT_CLICKED, (void*)(uintptr_t)i);
    }

    // Second row: ㅅㅆㅇㅈㅉㅊㅋㅌㅍ
    for (int i = 9; i < 18; i++) {
        lv_obj_t * btn = lv_btn_create(parent);
        if (btn == NULL) {
            printf("Error: Failed to create button %d in second row\n", i);
            continue;
        }
        lv_obj_set_size(btn, btn_width, btn_height);
        lv_obj_align(btn, LV_ALIGN_TOP_LEFT, start_x + (i - 9) * (btn_width + 2), start_y + btn_height + 2);
        lv_obj_set_style_bg_color(btn, lv_color_hex(0xE3F2FD), 0);
        
        lv_obj_t * btn_label = lv_label_create(btn);
        if (btn_label == NULL) {
            printf("Error: Failed to create label for button %d in second row\n", i);
            continue;
        }
        lv_label_set_text(btn_label, hangul_choseong[i]);
        if (korean_font != NULL) {
            lv_obj_set_style_text_font(btn_label, korean_font, 0);
        }
        lv_obj_center(btn_label);
        lv_obj_add_event_cb(btn, korean_keypad_char_cb, LV_EVENT_CLICKED, (void*)(uintptr_t)i);
    }

    // Third row: ㅎㅏㅐㅑㅒㅓㅔㅕㅖ
    lv_obj_t * btn = lv_btn_create(parent);
    if (btn == NULL) {
        printf("Error: Failed to create first button in third row\n");
    } else {
        lv_obj_set_size(btn, btn_width, btn_height);
        lv_obj_align(btn, LV_ALIGN_TOP_LEFT, start_x, start_y + 2 * (btn_height + 2));
        lv_obj_set_style_bg_color(btn, lv_color_hex(0xE3F2FD), 0);
        
        lv_obj_t * btn_label = lv_label_create(btn);
        if (btn_label == NULL) {
            printf("Error: Failed to create label for first button in third row\n");
        } else {
            lv_label_set_text(btn_label, hangul_choseong[18]);
            if (korean_font != NULL) {
                lv_obj_set_style_text_font(btn_label, korean_font, 0);
            }
            lv_obj_center(btn_label);
            lv_obj_add_event_cb(btn, korean_keypad_char_cb, LV_EVENT_CLICKED, (void*)(uintptr_t)18);
        }
    }

    for (int i = 0; i < 8; i++) {
        lv_obj_t * btn = lv_btn_create(parent);
        if (btn == NULL) {
            printf("Error: Failed to create button %d in third row vowels\n", i);
            continue;
        }
        lv_obj_set_size(btn, btn_width, btn_height);
        lv_obj_align(btn, LV_ALIGN_TOP_LEFT, start_x + (i + 1) * (btn_width + 2), start_y + 2 * (btn_height + 2));
        lv_obj_set_style_bg_color(btn, lv_color_hex(0xFFEBEE), 0);
        
        lv_obj_t * btn_label = lv_label_create(btn);
        if (btn_label == NULL) {
            printf("Error: Failed to create label for button %d in third row vowels\n", i);
            continue;
        }
        lv_label_set_text(btn_label, hangul_jungseong[i]);
        if (korean_font != NULL) {
            lv_obj_set_style_text_font(btn_label, korean_font, 0);
        }
        lv_obj_center(btn_label);
        lv_obj_add_event_cb(btn, korean_keypad_char_cb, LV_EVENT_CLICKED, (void*)(uintptr_t)(i + 100));
    }

    // Fourth row: ㅗㅘㅙㅚㅛㅜㅝㅞㅟ
    for (int i = 8; i < 17; i++) {
        lv_obj_t * btn = lv_btn_create(parent);
        if (btn == NULL) {
            printf("Error: Failed to create button %d in fourth row\n", i);
            continue;
        }
        lv_obj_set_size(btn, btn_width, btn_height);
        lv_obj_align(btn, LV_ALIGN_TOP_LEFT, start_x + (i - 8) * (btn_width + 2), start_y + 3 * (btn_height + 2));
        lv_obj_set_style_bg_color(btn, lv_color_hex(0xFFEBEE), 0);
        
        lv_obj_t * btn_label = lv_label_create(btn);
        if (btn_label == NULL) {
            printf("Error: Failed to create label for button %d in fourth row\n", i);
            continue;
        }
        lv_label_set_text(btn_label, hangul_jungseong[i]);
        if (korean_font != NULL) {
            lv_obj_set_style_text_font(btn_label, korean_font, 0);
        }
        lv_obj_center(btn_label);
        lv_obj_add_event_cb(btn, korean_keypad_char_cb, LV_EVENT_CLICKED, (void*)(uintptr_t)(i + 100));
    }

    // Fifth row: ㅠㅡㅢㅣ
    for (int i = 17; i < 21; i++) {
        lv_obj_t * btn = lv_btn_create(parent);
        if (btn == NULL) {
            printf("Error: Failed to create button %d in fifth row\n", i);
            continue;
        }
        lv_obj_set_size(btn, btn_width, btn_height);
        lv_obj_align(btn, LV_ALIGN_TOP_LEFT, start_x + (i - 17) * (btn_width + 2), start_y + 4 * (btn_height + 2));
        lv_obj_set_style_bg_color(btn, lv_color_hex(0xFFEBEE), 0);
        
        lv_obj_t * btn_label = lv_label_create(btn);
        if (btn_label == NULL) {
            printf("Error: Failed to create label for button %d in fifth row\n", i);
            continue;
        }
        lv_label_set_text(btn_label, hangul_jungseong[i]);
        if (korean_font != NULL) {
            lv_obj_set_style_text_font(btn_label, korean_font, 0);
        }
        lv_obj_center(btn_label);
        lv_obj_add_event_cb(btn, korean_keypad_char_cb, LV_EVENT_CLICKED, (void*)(uintptr_t)(i + 100));
    }
    
    printf("Hangul buttons created successfully\n");
}

// Korean keypad character callback
void korean_keypad_char_cb(lv_event_t * e) {
    lv_obj_t * btn = lv_event_get_target(e);
    void * user_data = lv_event_get_user_data(e);
    int char_index = (int)(uintptr_t)user_data;
    
    if (korean_keypad_state.mode == KOREAN_MODE_HANGUL) {
        if (char_index < 19) {
            // Choseong (initial consonant)
            add_choseong(&korean_keypad_state.composition, HANGUL_CHOSEONG_START + char_index);
        } else if (char_index >= 100 && char_index < 121) {
            // Jungseong (vowel)
            add_jungseong(&korean_keypad_state.composition, HANGUL_JUNGSEONG_START + (char_index - 100));
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

// Korean keypad mode callback
void korean_keypad_mode_cb(lv_event_t * e) {
    korean_keypad_state.mode = (korean_keypad_state.mode + 1) % 3;
    
    lv_obj_t * mode_btn = korean_keypad_state.mode_btn;
    lv_obj_t * mode_label = lv_obj_get_child(mode_btn, 0);
    
    switch (korean_keypad_state.mode) {
        case KOREAN_MODE_HANGUL:
            lv_label_set_text(mode_label, "한글");
            lv_obj_set_style_bg_color(mode_btn, lv_color_hex(0x4CAF50), 0);
            break;
        case KOREAN_MODE_ENGLISH:
            lv_label_set_text(mode_label, "ABC");
            lv_obj_set_style_bg_color(mode_btn, lv_color_hex(0x2196F3), 0);
            break;
        case KOREAN_MODE_NUMBER:
            lv_label_set_text(mode_label, "123");
            lv_obj_set_style_bg_color(mode_btn, lv_color_hex(0xFF9800), 0);
            break;
    }
}

// Korean keypad clear callback
void korean_keypad_clear_cb(lv_event_t * e) {
    korean_keypad_buffer[0] = '\0';
    reset_hangul_composition(&korean_keypad_state.composition);
    update_korean_display();
}

// Korean keypad backspace callback
void korean_keypad_backspace_cb(lv_event_t * e) {
    remove_char_from_buffer();
}

// Korean keypad enter callback
void korean_keypad_enter_cb(lv_event_t * e) {
    if (strlen(korean_keypad_buffer) > 0) {
        printf("Entered text: %s\n", korean_keypad_buffer);
        // Here you can add logic to process the entered text
        korean_keypad_buffer[0] = '\0';
        reset_hangul_composition(&korean_keypad_state.composition);
        update_korean_display();
    }
}

// Korean keypad space callback
void korean_keypad_space_cb(lv_event_t * e) {
    add_char_to_buffer(" ");
} 