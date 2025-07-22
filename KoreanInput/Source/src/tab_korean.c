#include "tab_korean.h"
#include "lv_freetype.h"
#include <stdio.h>
#include <string.h>

// Korean choseong (consonants) characters
static const char* korean_choseong[] = {
    "ㄱ", "ㄲ", "ㄴ", "ㄷ", "ㄸ", "ㄹ", "ㅁ", "ㅂ", "ㅃ", "ㅅ", 
    "ㅆ", "ㅇ", "ㅈ", "ㅉ", "ㅊ", "ㅋ", "ㅌ", "ㅍ", "ㅎ"
};
// Korean jungseong (vowels) characters
static const char* korean_jungseong[] = {
    "ㅏ", "ㅐ", "ㅑ", "ㅒ", "ㅓ", "ㅔ", "ㅕ", "ㅖ", "ㅗ", "ㅘ", "ㅙ", "ㅚ", "ㅛ", "ㅜ", "ㅝ", "ㅞ", "ㅟ", "ㅠ", "ㅡ", "ㅢ", "ㅣ"
};
// Korean jongseong (final consonants) characters
static const char* korean_jongseong[] = {
    "", "ㄱ", "ㄲ", "ㄳ", "ㄴ", "ㄵ", "ㄶ", "ㄷ", "ㄹ", "ㄺ", "ㄻ", "ㄼ", "ㄽ", "ㄾ", "ㄿ", "ㅀ", "ㅁ", "ㅂ", "ㅄ", "ㅅ", "ㅆ", "ㅇ", "ㅈ", "ㅊ", "ㅋ", "ㅌ", "ㅍ", "ㅎ"
};

static int current_choseong_index = 0;
static int current_jungseong_index = 0;
static int current_jongseong_index = 0;
static lv_obj_t * current_char_btn = NULL;
static lv_obj_t * current_jung_btn = NULL;
static lv_obj_t * current_jong_btn = NULL;
static lv_obj_t * result_label = NULL;
static lv_obj_t * korean_display_label = NULL;
static char korean_buffer[64] = "";
static lv_font_t * korean_font = NULL;

// Function to load TrueType font
static void load_korean_font(void) {
    if (korean_font == NULL) {
        korean_font = lv_freetype_font_create("/home/shkwon/Projects/LVGL/KoreanInput/Source/assets/NanumGothic-Regular.ttf", 
                                             LV_FREETYPE_FONT_RENDER_MODE_BITMAP, 
                                             24, 
                                             LV_FREETYPE_FONT_STYLE_NORMAL);
        if (korean_font == NULL) {
            printf("Failed to load Korean font, falling back to built-in font\n");
            korean_font = (lv_font_t*)&lv_font_source_han_sans_sc_16_cjk;
        } else {
            printf("Korean TrueType font loaded successfully\n");
        }
    }
}

// Event handlers for the buttons
// Choseong
static void prev_btn_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        current_choseong_index--;
        if (current_choseong_index < 0) {
            current_choseong_index = sizeof(korean_choseong) / sizeof(korean_choseong[0]) - 1;
        }
        // Update the center button text
        if (current_char_btn != NULL) {
            lv_obj_t * label = lv_obj_get_child(current_char_btn, 0);
            if (label != NULL) {
                lv_label_set_text(label, korean_choseong[current_choseong_index]);
                lv_obj_set_style_text_font(label, korean_font, 0);
            }
        }
    }
}
static void next_btn_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        current_choseong_index++;
        if (current_choseong_index >= sizeof(korean_choseong) / sizeof(korean_choseong[0])) {
            current_choseong_index = 0;
        }
        // Update the center button text
        if (current_char_btn != NULL) {
            lv_obj_t * label = lv_obj_get_child(current_char_btn, 0);
            if (label != NULL) {
                lv_label_set_text(label, korean_choseong[current_choseong_index]);
                lv_obj_set_style_text_font(label, korean_font, 0);
            }
        }
    }
}
static void current_char_btn_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        // Add the current character to the buffer
        if (strlen(korean_buffer) < sizeof(korean_buffer) - 3) { // -3 for UTF-8 safety
            strcat(korean_buffer, korean_choseong[current_choseong_index]);
            lv_label_set_text(korean_display_label, korean_buffer);
        }
        // Update the result label on the screen
        if (result_label != NULL) {
            lv_label_set_text(result_label, korean_choseong[current_choseong_index]);
        }
    }
}
// Jungseong
static void prev_jung_btn_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        current_jungseong_index--;
        if (current_jungseong_index < 0) {
            current_jungseong_index = sizeof(korean_jungseong) / sizeof(korean_jungseong[0]) - 1;
        }
        if (current_jung_btn != NULL) {
            lv_obj_t * label = lv_obj_get_child(current_jung_btn, 0);
            if (label != NULL) {
                lv_label_set_text(label, korean_jungseong[current_jungseong_index]);
                lv_obj_set_style_text_font(label, korean_font, 0);
            }
        }
    }
}
static void next_jung_btn_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        current_jungseong_index++;
        if (current_jungseong_index >= sizeof(korean_jungseong) / sizeof(korean_jungseong[0])) {
            current_jungseong_index = 0;
        }
        if (current_jung_btn != NULL) {
            lv_obj_t * label = lv_obj_get_child(current_jung_btn, 0);
            if (label != NULL) {
                lv_label_set_text(label, korean_jungseong[current_jungseong_index]);
                lv_obj_set_style_text_font(label, korean_font, 0);
            }
        }
    }
}
static void current_jung_btn_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        if (strlen(korean_buffer) < sizeof(korean_buffer) - 3) {
            strcat(korean_buffer, korean_jungseong[current_jungseong_index]);
            lv_label_set_text(korean_display_label, korean_buffer);
        }
        if (result_label != NULL) {
            lv_label_set_text(result_label, korean_jungseong[current_jungseong_index]);
        }
    }
}
// Jongseong
static void prev_jong_btn_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        current_jongseong_index--;
        if (current_jongseong_index < 0) {
            current_jongseong_index = sizeof(korean_jongseong) / sizeof(korean_jongseong[0]) - 1;
        }
        if (current_jong_btn != NULL) {
            lv_obj_t * label = lv_obj_get_child(current_jong_btn, 0);
            if (label != NULL) {
                lv_label_set_text(label, korean_jongseong[current_jongseong_index]);
                lv_obj_set_style_text_font(label, korean_font, 0);
            }
        }
    }
}
static void next_jong_btn_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        current_jongseong_index++;
        if (current_jongseong_index >= sizeof(korean_jongseong) / sizeof(korean_jongseong[0])) {
            current_jongseong_index = 0;
        }
        if (current_jong_btn != NULL) {
            lv_obj_t * label = lv_obj_get_child(current_jong_btn, 0);
            if (label != NULL) {
                lv_label_set_text(label, korean_jongseong[current_jongseong_index]);
                lv_obj_set_style_text_font(label, korean_font, 0);
            }
        }
    }
}
static void current_jong_btn_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        if (strlen(korean_buffer) < sizeof(korean_buffer) - 3) {
            strcat(korean_buffer, korean_jongseong[current_jongseong_index]);
            lv_label_set_text(korean_display_label, korean_buffer);
        }
        if (result_label != NULL) {
            lv_label_set_text(result_label, korean_jongseong[current_jongseong_index]);
        }
    }
}

static void clear_korean_cb(lv_event_t * e) {
    korean_buffer[0] = '\0';
    lv_label_set_text(korean_display_label, "");
}

static void backspace_korean_cb(lv_event_t * e) {
    int len = strlen(korean_buffer);
    if (len > 0) {
        // Remove the last character (Korean characters are typically 3 bytes in UTF-8)
        // For simplicity, we'll remove the last 3 bytes
        if (len >= 3) {
            korean_buffer[len - 3] = '\0';
        } else {
            korean_buffer[0] = '\0';
        }
        lv_label_set_text(korean_display_label, korean_buffer);
    }
}

static void enter_korean_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        // Add the current character to the buffer
        if (strlen(korean_buffer) < sizeof(korean_buffer) - 3) { // -3 for UTF-8 safety
            strcat(korean_buffer, korean_choseong[current_choseong_index]);
            lv_label_set_text(korean_display_label, korean_buffer);
        }
        // Update the result label on the screen
        if (result_label != NULL) {
            lv_label_set_text(result_label, korean_choseong[current_choseong_index]);
        }
    }
}

// Simple Korean tab with basic FreeType font support
void create_korean_tab(lv_obj_t * parent) {
    // Load Korean TrueType font
    load_korean_font();
    
    // Simple header
    lv_obj_t * header = lv_label_create(parent);
    lv_label_set_text(header, "Korean Choseong");
    lv_obj_set_style_text_font(header, korean_font, 0);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 10);

    // Result label (just below the header)
    result_label = lv_label_create(parent);
    lv_label_set_text(result_label, "");
    lv_obj_set_style_text_font(result_label, korean_font, 0);
    lv_obj_align(result_label, LV_ALIGN_TOP_MID, 0, 40);

    // Display area for entered Korean characters (similar to number tab)
    lv_obj_t * korean_display = lv_label_create(parent);
    lv_label_set_text(korean_display, "");
    lv_obj_set_style_text_font(korean_display, korean_font, 0);
    lv_obj_set_style_bg_color(korean_display, lv_color_hex(0xE0E0E0), 0);
    lv_obj_set_style_bg_opa(korean_display, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(korean_display, 10, 0);
    lv_obj_set_size(korean_display, 260, 40);
    lv_obj_align(korean_display, LV_ALIGN_TOP_MID, 0, 70);
    korean_display_label = korean_display;
    
    // Choseong buttons (row 1)
    lv_obj_t * prev_btn = lv_btn_create(parent);
    lv_obj_set_size(prev_btn, 40, 40);
    lv_obj_align(prev_btn, LV_ALIGN_CENTER, -70, -20);
    lv_obj_set_style_bg_color(prev_btn, lv_color_hex(0x007ACC), 0);
    lv_obj_set_style_text_color(prev_btn, lv_color_hex(0xFFFFFF), 0);
    lv_obj_add_event_cb(prev_btn, prev_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * prev_label = lv_label_create(prev_btn);
    lv_label_set_text(prev_label, "<");
    lv_obj_center(prev_label);
    current_char_btn = lv_btn_create(parent);
    lv_obj_set_size(current_char_btn, 50, 40);
    lv_obj_align(current_char_btn, LV_ALIGN_CENTER, 0, -20);
    lv_obj_set_style_bg_color(current_char_btn, lv_color_hex(0x28A745), 0);
    lv_obj_set_style_text_color(current_char_btn, lv_color_hex(0xFFFFFF), 0);
    lv_obj_add_event_cb(current_char_btn, current_char_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * current_label = lv_label_create(current_char_btn);
    lv_label_set_text(current_label, korean_choseong[current_choseong_index]);
    lv_obj_set_style_text_font(current_label, korean_font, 0);
    lv_obj_center(current_label);
    lv_obj_t * next_btn = lv_btn_create(parent);
    lv_obj_set_size(next_btn, 40, 40);
    lv_obj_align(next_btn, LV_ALIGN_CENTER, 70, -20);
    lv_obj_set_style_bg_color(next_btn, lv_color_hex(0x007ACC), 0);
    lv_obj_set_style_text_color(next_btn, lv_color_hex(0xFFFFFF), 0);
    lv_obj_add_event_cb(next_btn, next_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * next_label = lv_label_create(next_btn);
    lv_label_set_text(next_label, ">");
    lv_obj_center(next_label);
    // Jungseong buttons (row 2)
    lv_obj_t * prev_jung_btn = lv_btn_create(parent);
    lv_obj_set_size(prev_jung_btn, 40, 40);
    lv_obj_align(prev_jung_btn, LV_ALIGN_CENTER, -70, 40);
    lv_obj_set_style_bg_color(prev_jung_btn, lv_color_hex(0x007ACC), 0);
    lv_obj_set_style_text_color(prev_jung_btn, lv_color_hex(0xFFFFFF), 0);
    lv_obj_add_event_cb(prev_jung_btn, prev_jung_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * prev_jung_label = lv_label_create(prev_jung_btn);
    lv_label_set_text(prev_jung_label, "<");
    lv_obj_center(prev_jung_label);
    current_jung_btn = lv_btn_create(parent);
    lv_obj_set_size(current_jung_btn, 50, 40);
    lv_obj_align(current_jung_btn, LV_ALIGN_CENTER, 0, 40);
    lv_obj_set_style_bg_color(current_jung_btn, lv_color_hex(0x28A745), 0);
    lv_obj_set_style_text_color(current_jung_btn, lv_color_hex(0xFFFFFF), 0);
    lv_obj_add_event_cb(current_jung_btn, current_jung_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * current_jung_label = lv_label_create(current_jung_btn);
    lv_label_set_text(current_jung_label, korean_jungseong[current_jungseong_index]);
    lv_obj_set_style_text_font(current_jung_label, korean_font, 0);
    lv_obj_center(current_jung_label);
    lv_obj_t * next_jung_btn = lv_btn_create(parent);
    lv_obj_set_size(next_jung_btn, 40, 40);
    lv_obj_align(next_jung_btn, LV_ALIGN_CENTER, 70, 40);
    lv_obj_set_style_bg_color(next_jung_btn, lv_color_hex(0x007ACC), 0);
    lv_obj_set_style_text_color(next_jung_btn, lv_color_hex(0xFFFFFF), 0);
    lv_obj_add_event_cb(next_jung_btn, next_jung_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * next_jung_label = lv_label_create(next_jung_btn);
    lv_label_set_text(next_jung_label, ">");
    lv_obj_center(next_jung_label);
    // Jongseong buttons (row 3)
    lv_obj_t * prev_jong_btn = lv_btn_create(parent);
    lv_obj_set_size(prev_jong_btn, 40, 40);
    lv_obj_align(prev_jong_btn, LV_ALIGN_CENTER, -70, 100);
    lv_obj_set_style_bg_color(prev_jong_btn, lv_color_hex(0x007ACC), 0);
    lv_obj_set_style_text_color(prev_jong_btn, lv_color_hex(0xFFFFFF), 0);
    lv_obj_add_event_cb(prev_jong_btn, prev_jong_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * prev_jong_label = lv_label_create(prev_jong_btn);
    lv_label_set_text(prev_jong_label, "<");
    lv_obj_center(prev_jong_label);
    current_jong_btn = lv_btn_create(parent);
    lv_obj_set_size(current_jong_btn, 50, 40);
    lv_obj_align(current_jong_btn, LV_ALIGN_CENTER, 0, 100);
    lv_obj_set_style_bg_color(current_jong_btn, lv_color_hex(0x28A745), 0);
    lv_obj_set_style_text_color(current_jong_btn, lv_color_hex(0xFFFFFF), 0);
    lv_obj_add_event_cb(current_jong_btn, current_jong_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * current_jong_label = lv_label_create(current_jong_btn);
    lv_label_set_text(current_jong_label, korean_jongseong[current_jongseong_index]);
    lv_obj_set_style_text_font(current_jong_label, korean_font, 0);
    lv_obj_center(current_jong_label);
    lv_obj_t * next_jong_btn = lv_btn_create(parent);
    lv_obj_set_size(next_jong_btn, 40, 40);
    lv_obj_align(next_jong_btn, LV_ALIGN_CENTER, 70, 100);
    lv_obj_set_style_bg_color(next_jong_btn, lv_color_hex(0x007ACC), 0);
    lv_obj_set_style_text_color(next_jong_btn, lv_color_hex(0xFFFFFF), 0);
    lv_obj_add_event_cb(next_jong_btn, next_jong_btn_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_t * next_jong_label = lv_label_create(next_jong_btn);
    lv_label_set_text(next_jong_label, ">");
    lv_obj_center(next_jong_label);

    // Function buttons (Clear, Backspace, and Enter)
    lv_obj_t * clear_btn = lv_btn_create(parent);
    lv_obj_set_size(clear_btn, 50, 35);
    lv_obj_align(clear_btn, LV_ALIGN_CENTER, -80, 160);
    lv_obj_set_style_bg_color(clear_btn, lv_color_hex(0xFF6B6B), 0); // Red background
    lv_obj_set_style_text_color(clear_btn, lv_color_hex(0xFFFFFF), 0); // White text
    
    lv_obj_t * clear_label = lv_label_create(clear_btn);
    lv_label_set_text(clear_label, "Clear");
    lv_obj_center(clear_label);
    lv_obj_add_event_cb(clear_btn, clear_korean_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * backspace_btn = lv_btn_create(parent);
    lv_obj_set_size(backspace_btn, 50, 35);
    lv_obj_align(backspace_btn, LV_ALIGN_CENTER, 0, 160);
    lv_obj_set_style_bg_color(backspace_btn, lv_color_hex(0xFFA500), 0); // Orange background
    lv_obj_set_style_text_color(backspace_btn, lv_color_hex(0xFFFFFF), 0); // White text
    
    lv_obj_t * backspace_label = lv_label_create(backspace_btn);
    lv_label_set_text(backspace_label, "⌫");
    lv_obj_center(backspace_label);
    lv_obj_add_event_cb(backspace_btn, backspace_korean_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * enter_btn = lv_btn_create(parent);
    lv_obj_set_size(enter_btn, 50, 35);
    lv_obj_align(enter_btn, LV_ALIGN_CENTER, 80, 160);
    lv_obj_set_style_bg_color(enter_btn, lv_color_hex(0x28A745), 0); // Green background
    lv_obj_set_style_text_color(enter_btn, lv_color_hex(0xFFFFFF), 0); // White text
    
    lv_obj_t * enter_label = lv_label_create(enter_btn);
    lv_label_set_text(enter_label, "Enter");
    lv_obj_center(enter_label);
    lv_obj_add_event_cb(enter_btn, enter_korean_cb, LV_EVENT_CLICKED, NULL);
} 