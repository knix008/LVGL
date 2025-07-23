#include "tab_korean.h"
#include "lv_freetype.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>

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

// Global variables for Korean input
static lv_obj_t * choseong_diplay_label = NULL;
static lv_obj_t * jungseong_display_label = NULL;
static lv_obj_t * jongseong_display_label = NULL;
static lv_obj_t * result_label = NULL;
static char korean_buffer[64] = "";
static int current_choseong_index = 0;
static int current_jungseong_index = 0;
static int current_jongseong_index = 0;
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

// Function to convert Korean jamo to UTF-8 syllable
static int korean_jamo_to_utf8_syllable(int choseong_idx, int jungseong_idx, int jongseong_idx, char* output, size_t output_size) {
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
    
    // Jongseong mapping (0-27 to Unicode jongseong, 0 = no jongseong)
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

// Callback functions for Korean input
static void choseong_next_korean_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("Next choseong button clicked, current index: %d\n", current_choseong_index);
        current_choseong_index = (current_choseong_index + 1) % (sizeof(korean_choseong) / sizeof(korean_choseong[0]));
        printf("New index: %d, array size: %zu\n", current_choseong_index, sizeof(korean_choseong) / sizeof(korean_choseong[0]));
        
        // Safety check for array bounds and label
        if (current_choseong_index >= 0 && current_choseong_index < sizeof(korean_choseong) / sizeof(korean_choseong[0]) && 
            korean_choseong[current_choseong_index] != NULL && choseong_diplay_label != NULL) {
            printf("Would update label to: %s\n", korean_choseong[current_choseong_index]);
            lv_label_set_text(choseong_diplay_label, korean_choseong[current_choseong_index]);
        } else {
            printf("Error: Invalid index %d or null label\n", current_choseong_index);
        }
    }
}

static void choseong_prev_korean_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("Previous choseoung button clicked, current index: %d\n", current_choseong_index);
        current_choseong_index = (current_choseong_index - 1 + sizeof(korean_choseong) / sizeof(korean_choseong[0])) % (sizeof(korean_choseong) / sizeof(korean_choseong[0]));
        printf("New index: %d\n", current_choseong_index);
        
        // Safety check for array bounds and label
        if (current_choseong_index >= 0 && current_choseong_index < sizeof(korean_choseong) / sizeof(korean_choseong[0]) && 
            korean_choseong[current_choseong_index] != NULL && choseong_diplay_label != NULL) {
            printf("Would update label to: %s\n", korean_choseong[current_choseong_index]);
            lv_label_set_text(choseong_diplay_label, korean_choseong[current_choseong_index]);
        } else {
            printf("Error: Invalid index %d or null label\n", current_choseong_index);
        }
    }
}

static void jungseong_next_korean_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("Next jungseong button clicked, current index: %d\n", current_jungseong_index);
        current_jungseong_index = (current_jungseong_index + 1) % (sizeof(korean_jungseong) / sizeof(korean_jungseong[0]));
        printf("New jungseong index: %d, array size: %zu\n", current_jungseong_index, sizeof(korean_jungseong) / sizeof(korean_jungseong[0]));
        
        // Safety check for array bounds and label
        if (current_jungseong_index >= 0 && current_jungseong_index < sizeof(korean_jungseong) / sizeof(korean_jungseong[0]) && 
            korean_jungseong[current_jungseong_index] != NULL && jungseong_display_label != NULL) {
            printf("Would update jungseong label to: %s\n", korean_jungseong[current_jungseong_index]);
            lv_label_set_text(jungseong_display_label, korean_jungseong[current_jungseong_index]);
        } else {
            printf("Error: Invalid jungseong index %d or null label\n", current_jungseong_index);
        }
    }
}

static void jungseong_prev_korean_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("Previous jungseong button clicked, current index: %d\n", current_jungseong_index);
        current_jungseong_index = (current_jungseong_index - 1 + sizeof(korean_jungseong) / sizeof(korean_jungseong[0])) % (sizeof(korean_jungseong) / sizeof(korean_jungseong[0]));
        printf("New jungseong index: %d\n", current_jungseong_index);
        
        // Safety check for array bounds and label
        if (current_jungseong_index >= 0 && current_jungseong_index < sizeof(korean_jungseong) / sizeof(korean_jungseong[0]) && 
            korean_jungseong[current_jungseong_index] != NULL && jungseong_display_label != NULL) {
            printf("Would update jungseong label to: %s\n", korean_jungseong[current_jungseong_index]);
            lv_label_set_text(jungseong_display_label, korean_jungseong[current_jungseong_index]);
        } else {
            printf("Error: Invalid jungseong index %d or null label\n", current_jungseong_index);
        }
    }
}

static void jongseong_next_korean_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("Next jongseong button clicked, current index: %d\n", current_jongseong_index);
        current_jongseong_index = (current_jongseong_index + 1) % (sizeof(korean_jongseong) / sizeof(korean_jongseong[0]));
        printf("New jongseong index: %d, array size: %zu\n", current_jongseong_index, sizeof(korean_jongseong) / sizeof(korean_jongseong[0]));
        
        // Safety check for array bounds and label
        if (current_jongseong_index >= 0 && current_jongseong_index < sizeof(korean_jongseong) / sizeof(korean_jongseong[0]) && 
            korean_jongseong[current_jongseong_index] != NULL && jongseong_display_label != NULL) {
            printf("Would update jongseong label to: %s\n", korean_jongseong[current_jongseong_index]);
            lv_label_set_text(jongseong_display_label, korean_jongseong[current_jongseong_index]);
        } else {
            printf("Error: Invalid jongseong index %d or null label\n", current_jongseong_index);
        }
    }
}

static void jongseong_prev_korean_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("Previous jongseong button clicked, current index: %d\n", current_jongseong_index);
        current_jongseong_index = (current_jongseong_index - 1 + sizeof(korean_jongseong) / sizeof(korean_jongseong[0])) % (sizeof(korean_jongseong) / sizeof(korean_jongseong[0]));
        printf("New jongseong index: %d\n", current_jongseong_index);
        
        // Safety check for array bounds and label
        if (current_jongseong_index >= 0 && current_jongseong_index < sizeof(korean_jongseong) / sizeof(korean_jongseong[0]) && 
            korean_jongseong[current_jongseong_index] != NULL && jongseong_display_label != NULL) {
            printf("Would update jongseong label to: %s\n", korean_jongseong[current_jongseong_index]);
            lv_label_set_text(jongseong_display_label, korean_jongseong[current_jongseong_index]);
        } else {
            printf("Error: Invalid jongseong index %d or null label\n", current_jongseong_index);
        }
    }
}

static void backspace_korean_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
                 printf("Backspace button clicked\n");
         int len = strlen(korean_buffer);
         if (len > 0) {
             // Handle UTF-8 backspace properly
             // Find the start of the last UTF-8 character
             int i = len - 1;
             while (i > 0 && (korean_buffer[i] & 0xC0) == 0x80) {
                 i--; // Go back to find the start of the UTF-8 sequence
             }
             korean_buffer[i] = '\0';
             printf("Removed UTF-8 character, buffer now: '%s'\n", korean_buffer);
            // Update result label to show current buffer
            if (result_label != NULL) {
                if (strlen(korean_buffer) > 0) {
                    char result_text[80];
                    snprintf(result_text, sizeof(result_text), "Result: %s", korean_buffer);
                    lv_label_set_text(result_label, result_text);
                } else {
                    lv_label_set_text(result_label, "Result: ");
                }
            }
        }
        // Keep displaying the current characters in all center labels regardless of buffer
        if (current_choseong_index >= 0 && current_choseong_index < sizeof(korean_choseong) / sizeof(korean_choseong[0]) && 
            korean_choseong[current_choseong_index] != NULL && choseong_diplay_label != NULL) {
            lv_label_set_text(choseong_diplay_label, korean_choseong[current_choseong_index]);
        }
        if (current_jungseong_index >= 0 && current_jungseong_index < sizeof(korean_jungseong) / sizeof(korean_jungseong[0]) && 
            korean_jungseong[current_jungseong_index] != NULL && jungseong_display_label != NULL) {
            lv_label_set_text(jungseong_display_label, korean_jungseong[current_jungseong_index]);
        }
        if (current_jongseong_index >= 0 && current_jongseong_index < sizeof(korean_jongseong) / sizeof(korean_jongseong[0]) && 
            korean_jongseong[current_jongseong_index] != NULL && jongseong_display_label != NULL) {
            lv_label_set_text(jongseong_display_label, korean_jongseong[current_jongseong_index]);
        }
    }
}

static void enter_korean_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("Enter button clicked - choseong: %d, jungseong: %d, jongseong: %d\n", 
               current_choseong_index, current_jungseong_index, current_jongseong_index);
        
                 // Safety check for all array bounds
         if (current_choseong_index >= 0 && current_choseong_index < sizeof(korean_choseong) / sizeof(korean_choseong[0]) && 
             korean_choseong[current_choseong_index] != NULL &&
             current_jungseong_index >= 0 && current_jungseong_index < sizeof(korean_jungseong) / sizeof(korean_jungseong[0]) && 
             korean_jungseong[current_jungseong_index] != NULL &&
             current_jongseong_index >= 0 && current_jongseong_index < sizeof(korean_jongseong) / sizeof(korean_jongseong[0]) && 
             korean_jongseong[current_jongseong_index] != NULL) {
             
             // Create a complete Korean syllable using UTF-8 conversion
             char syllable[16] = "";
             int result = korean_jamo_to_utf8_syllable(current_choseong_index, current_jungseong_index, current_jongseong_index, syllable, sizeof(syllable));
             
             if (result == 0) {
                 // Add the UTF-8 syllable to the buffer
                 if (strlen(korean_buffer) < sizeof(korean_buffer) - strlen(syllable) - 1) {
                     strcat(korean_buffer, syllable);
                     printf("Added UTF-8 syllable '%s', buffer now: '%s'\n", syllable, korean_buffer);
                     
                     // Update the result label to show the accumulated text
                     if (result_label != NULL) {
                         char result_text[80];
                         snprintf(result_text, sizeof(result_text), "Result: %s", korean_buffer);
                         lv_label_set_text(result_label, result_text);
                     }
                 } else {
                     printf("Buffer full! Cannot add more characters.\n");
                 }
             } else {
                 printf("Error: Failed to convert jamo to UTF-8 syllable\n");
             }
        } else {
            printf("Error: Invalid index in enter callback\n");
        }
        
        // Keep all center labels showing the current characters
        if (current_choseong_index >= 0 && current_choseong_index < sizeof(korean_choseong) / sizeof(korean_choseong[0]) && 
            korean_choseong[current_choseong_index] != NULL && choseong_diplay_label != NULL) {
            lv_label_set_text(choseong_diplay_label, korean_choseong[current_choseong_index]);
        }
        if (current_jungseong_index >= 0 && current_jungseong_index < sizeof(korean_jungseong) / sizeof(korean_jungseong[0]) && 
            korean_jungseong[current_jungseong_index] != NULL && jungseong_display_label != NULL) {
            lv_label_set_text(jungseong_display_label, korean_jungseong[current_jungseong_index]);
        }
        if (current_jongseong_index >= 0 && current_jongseong_index < sizeof(korean_jongseong) / sizeof(korean_jongseong[0]) && 
            korean_jongseong[current_jongseong_index] != NULL && jongseong_display_label != NULL) {
            lv_label_set_text(jongseong_display_label, korean_jongseong[current_jongseong_index]);
        }
    }
}

static void clear_korean_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("Clear button clicked\n");
        // Clear the buffer
        korean_buffer[0] = '\0';
        // Reset display to show current characters instead of buffer
        if (current_choseong_index >= 0 && current_choseong_index < sizeof(korean_choseong) / sizeof(korean_choseong[0]) && 
            korean_choseong[current_choseong_index] != NULL && choseong_diplay_label != NULL) {
            lv_label_set_text(choseong_diplay_label, korean_choseong[current_choseong_index]);
        }
        if (current_jungseong_index >= 0 && current_jungseong_index < sizeof(korean_jungseong) / sizeof(korean_jungseong[0]) && 
            korean_jungseong[current_jungseong_index] != NULL && jungseong_display_label != NULL) {
            lv_label_set_text(jungseong_display_label, korean_jungseong[current_jungseong_index]);
        }
        if (current_jongseong_index >= 0 && current_jongseong_index < sizeof(korean_jongseong) / sizeof(korean_jongseong[0]) && 
            korean_jongseong[current_jongseong_index] != NULL && jongseong_display_label != NULL) {
            lv_label_set_text(jongseong_display_label, korean_jongseong[current_jongseong_index]);
        }
        // Clear the result label
        if (result_label != NULL) {
            lv_label_set_text(result_label, "Result: ");
        }
    }
}

// Create Korean tab
void create_korean_tab(lv_obj_t * parent) {
    // Load Korean font first
    load_korean_font();
    
    // Create a container for the Korean input interface
    lv_obj_t * container = lv_obj_create(parent);
    lv_obj_set_size(container, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(container, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
    // Create title label
    lv_obj_t * title = lv_label_create(container);
    lv_label_set_text(title, "Korean Input");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
    
    // Create first row container for navigation (Prev | Character | Next)
    lv_obj_t * nav_container = lv_obj_create(container);
    lv_obj_set_size(nav_container, LV_PCT(90), 50);
    lv_obj_set_flex_flow(nav_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(nav_container, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_border_width(nav_container, 0, 0);
    lv_obj_set_style_bg_opa(nav_container, LV_OPA_TRANSP, 0);
    
    // Previous button
    lv_obj_t * prev_btn = lv_btn_create(nav_container);
    lv_obj_set_size(prev_btn, 70, 40);
    lv_obj_t * prev_label = lv_label_create(prev_btn);
    lv_label_set_text(prev_label, "<");
    lv_obj_add_event_cb(prev_btn, choseong_prev_korean_cb, LV_EVENT_CLICKED, NULL);
    
    // Current Korean character display (center label)
    choseong_diplay_label = lv_label_create(nav_container);
    lv_label_set_text(choseong_diplay_label, korean_choseong[0]); // Start with first character
    lv_obj_set_style_text_font(choseong_diplay_label, korean_font, 0);
    lv_obj_set_style_bg_color(choseong_diplay_label, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(choseong_diplay_label, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(choseong_diplay_label, 2, 0);
    lv_obj_set_style_border_color(choseong_diplay_label, lv_color_hex(0x333333), 0);
    lv_obj_set_style_text_align(choseong_diplay_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_size(choseong_diplay_label, 80, 40);
    
    // Next button
    lv_obj_t * next_btn = lv_btn_create(nav_container);
    lv_obj_set_size(next_btn, 70, 40);
    lv_obj_t * next_label = lv_label_create(next_btn);
    lv_label_set_text(next_label, ">");
    lv_obj_add_event_cb(next_btn, choseong_next_korean_cb, LV_EVENT_CLICKED, NULL);

    // Create second row container for jungseong navigation (Prev | Jungseong | Next)
    lv_obj_t * jungseong_container = lv_obj_create(container);
    lv_obj_set_size(jungseong_container, LV_PCT(90), 50);
    lv_obj_set_flex_flow(jungseong_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(jungseong_container, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_border_width(jungseong_container, 0, 0);
    lv_obj_set_style_bg_opa(jungseong_container, LV_OPA_TRANSP, 0);
    
    // Previous jungseong button
    lv_obj_t * jungseong_prev_btn = lv_btn_create(jungseong_container);
    lv_obj_set_size(jungseong_prev_btn, 70, 40);
    lv_obj_t * jungseong_prev_label = lv_label_create(jungseong_prev_btn);
    lv_label_set_text(jungseong_prev_label, "<");
    lv_obj_add_event_cb(jungseong_prev_btn, jungseong_prev_korean_cb, LV_EVENT_CLICKED, NULL);
    
    // Current Korean jungseong display (center label)
    jungseong_display_label = lv_label_create(jungseong_container);
    lv_label_set_text(jungseong_display_label, korean_jungseong[0]); // Start with first jungseong character
    lv_obj_set_style_text_font(jungseong_display_label, korean_font, 0);
    lv_obj_set_style_bg_color(jungseong_display_label, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(jungseong_display_label, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(jungseong_display_label, 2, 0);
    lv_obj_set_style_border_color(jungseong_display_label, lv_color_hex(0x333333), 0);
    lv_obj_set_style_text_align(jungseong_display_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_size(jungseong_display_label, 80, 40);
    
    // Next jungseong button
    lv_obj_t * jungseong_next_btn = lv_btn_create(jungseong_container);
    lv_obj_set_size(jungseong_next_btn, 70, 40);
    lv_obj_t * jungseong_next_label = lv_label_create(jungseong_next_btn);
    lv_label_set_text(jungseong_next_label, ">");
    lv_obj_add_event_cb(jungseong_next_btn, jungseong_next_korean_cb, LV_EVENT_CLICKED, NULL);

    // Create third row container for jongseong navigation (Prev | Jongseong | Next)
    lv_obj_t * jongseong_container = lv_obj_create(container);
    lv_obj_set_size(jongseong_container, LV_PCT(90), 50);
    lv_obj_set_flex_flow(jongseong_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(jongseong_container, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_border_width(jongseong_container, 0, 0);
    lv_obj_set_style_bg_opa(jongseong_container, LV_OPA_TRANSP, 0);
    
    // Previous jongseong button
    lv_obj_t * jongseong_prev_btn = lv_btn_create(jongseong_container);
    lv_obj_set_size(jongseong_prev_btn, 70, 40);
    lv_obj_t * jongseong_prev_label = lv_label_create(jongseong_prev_btn);
    lv_label_set_text(jongseong_prev_label, "<");
    lv_obj_add_event_cb(jongseong_prev_btn, jongseong_prev_korean_cb, LV_EVENT_CLICKED, NULL);
    
    // Current Korean jongseong display (center label)
    jongseong_display_label = lv_label_create(jongseong_container);
    lv_label_set_text(jongseong_display_label, korean_jongseong[0]); // Start with first jongseong character
    lv_obj_set_style_text_font(jongseong_display_label, korean_font, 0);
    lv_obj_set_style_bg_color(jongseong_display_label, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(jongseong_display_label, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(jongseong_display_label, 2, 0);
    lv_obj_set_style_border_color(jongseong_display_label, lv_color_hex(0x333333), 0);
    lv_obj_set_style_text_align(jongseong_display_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_size(jongseong_display_label, 80, 40);
    
    // Next jongseong button
    lv_obj_t * jongseong_next_btn = lv_btn_create(jongseong_container);
    lv_obj_set_size(jongseong_next_btn, 70, 40);
    lv_obj_t * jongseong_next_label = lv_label_create(jongseong_next_btn);
    lv_label_set_text(jongseong_next_label, ">");
    lv_obj_add_event_cb(jongseong_next_btn, jongseong_next_korean_cb, LV_EVENT_CLICKED, NULL);

    // Create fourth row container for actions (Clear | Enter | Backspace)
    lv_obj_t * action_container = lv_obj_create(container);
    lv_obj_set_size(action_container, LV_PCT(90), 50);
    lv_obj_set_flex_flow(action_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(action_container, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_border_width(action_container, 0, 0);
    lv_obj_set_style_bg_opa(action_container, LV_OPA_TRANSP, 0);
    
    // Clear button
    lv_obj_t * clear_btn = lv_btn_create(action_container);
    lv_obj_set_size(clear_btn, 70, 40);
    lv_obj_t * clear_label = lv_label_create(clear_btn);
    lv_label_set_text(clear_label, "Clear");
    lv_obj_add_event_cb(clear_btn, clear_korean_cb, LV_EVENT_CLICKED, NULL);
    
    // Enter button
    lv_obj_t * enter_btn = lv_btn_create(action_container);
    lv_obj_set_size(enter_btn, 70, 40);
    lv_obj_t * enter_label = lv_label_create(enter_btn);
    lv_label_set_text(enter_label, "Enter");
    lv_obj_add_event_cb(enter_btn, enter_korean_cb, LV_EVENT_CLICKED, NULL);
    
    // Backspace button
    lv_obj_t * back_btn = lv_btn_create(action_container);
    lv_obj_set_size(back_btn, 70, 40);
    lv_obj_t * back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "Back");
    lv_obj_add_event_cb(back_btn, backspace_korean_cb, LV_EVENT_CLICKED, NULL);
    
    // Create result display area
    result_label = lv_label_create(container);
    lv_label_set_text(result_label, "Result: ");
    lv_obj_set_style_text_font(result_label, korean_font, 0);
    lv_obj_set_style_bg_color(result_label, lv_color_hex(0xf0f0f0), 0);
    lv_obj_set_style_bg_opa(result_label, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(result_label, 1, 0);
    lv_obj_set_size(result_label, 200, 60);
    
    printf("Korean tab created successfully\n");
}