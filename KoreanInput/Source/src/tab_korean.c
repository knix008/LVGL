#include "tab_korean.h"
#include "korean_input.h"
#include "lv_freetype.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>

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

// Function to get previous index with wraparound
static int get_prev_index(int current, int size) {
    return (current - 1 + size) % size;
}

// Function to get next index with wraparound
static int get_next_index(int current, int size) {
    return (current + 1) % size;
}

// Function to update choseong label with 3 characters
static void update_choseong_label() {
    if (choseong_diplay_label == NULL) return;
    
    const char** choseong_array = get_korean_choseong();
    size_t choseong_size = get_korean_choseong_size();
    
    int prev_idx = get_prev_index(current_choseong_index, choseong_size);
    int next_idx = get_next_index(current_choseong_index, choseong_size);
    
    char label_text[32];
    snprintf(label_text, sizeof(label_text), "%s %s %s", 
             choseong_array[prev_idx], 
             choseong_array[current_choseong_index], 
             choseong_array[next_idx]);
    
    lv_label_set_text(choseong_diplay_label, label_text);
}

// Function to update jungseong label with 3 characters
static void update_jungseong_label() {
    if (jungseong_display_label == NULL) return;
    
    const char** jungseong_array = get_korean_jungseong();
    size_t jungseong_size = get_korean_jungseong_size();
    
    int prev_idx = get_prev_index(current_jungseong_index, jungseong_size);
    int next_idx = get_next_index(current_jungseong_index, jungseong_size);
    
    char label_text[32];
    snprintf(label_text, sizeof(label_text), "%s %s %s", 
             jungseong_array[prev_idx], 
             jungseong_array[current_jungseong_index], 
             jungseong_array[next_idx]);
    
    lv_label_set_text(jungseong_display_label, label_text);
}

// Function to update jongseong label with 3 characters
static void update_jongseong_label() {
    if (jongseong_display_label == NULL) return;
    
    const char** jongseong_array = get_korean_jongseong();
    size_t jongseong_size = get_korean_jongseong_size();
    
    int prev_idx = get_prev_index(current_jongseong_index, jongseong_size);
    int next_idx = get_next_index(current_jongseong_index, jongseong_size);
    
    char label_text[32];
    snprintf(label_text, sizeof(label_text), "%s %s %s", 
             jongseong_array[prev_idx], 
             jongseong_array[current_jongseong_index], 
             jongseong_array[next_idx]);
    
    lv_label_set_text(jongseong_display_label, label_text);
}

// Callback functions for Korean input
static void choseong_next_korean_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("Next choseong button clicked, current index: %d\n", current_choseong_index);
        size_t choseong_size = get_korean_choseong_size();
        current_choseong_index = (current_choseong_index + 1) % choseong_size;
        printf("New index: %d, array size: %zu\n", current_choseong_index, choseong_size);
        
        // Update label with 3 characters
        update_choseong_label();
    }
}

static void choseong_prev_korean_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("Previous choseoung button clicked, current index: %d\n", current_choseong_index);
        size_t choseong_size = get_korean_choseong_size();
        current_choseong_index = (current_choseong_index - 1 + choseong_size) % choseong_size;
        printf("New index: %d\n", current_choseong_index);
        
        // Update label with 3 characters
        update_choseong_label();
    }
}

static void jungseong_next_korean_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("Next jungseong button clicked, current index: %d\n", current_jungseong_index);
        size_t jungseong_size = get_korean_jungseong_size();
        current_jungseong_index = (current_jungseong_index + 1) % jungseong_size;
        printf("New jungseong index: %d, array size: %zu\n", current_jungseong_index, jungseong_size);
        
        // Update label with 3 characters
        update_jungseong_label();
    }
}

static void jungseong_prev_korean_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("Previous jungseong button clicked, current index: %d\n", current_jungseong_index);
        size_t jungseong_size = get_korean_jungseong_size();
        current_jungseong_index = (current_jungseong_index - 1 + jungseong_size) % jungseong_size;
        printf("New jungseong index: %d\n", current_jungseong_index);
        
        // Update label with 3 characters
        update_jungseong_label();
    }
}

static void jongseong_next_korean_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("Next jongseong button clicked, current index: %d\n", current_jongseong_index);
        size_t jongseong_size = get_korean_jongseong_size();
        current_jongseong_index = (current_jongseong_index + 1) % jongseong_size;
        printf("New jongseong index: %d, array size: %zu\n", current_jongseong_index, jongseong_size);
        
        // Update label with 3 characters
        update_jongseong_label();
    }
}

static void jongseong_prev_korean_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("Previous jongseong button clicked, current index: %d\n", current_jongseong_index);
        size_t jongseong_size = get_korean_jongseong_size();
        current_jongseong_index = (current_jongseong_index - 1 + jongseong_size) % jongseong_size;
        printf("New jongseong index: %d\n", current_jongseong_index);
        
        // Update label with 3 characters
        update_jongseong_label();
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
        update_choseong_label();
        update_jungseong_label();
        update_jongseong_label();
    }
}

static void enter_korean_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("Enter button clicked - choseong: %d, jungseong: %d, jongseong: %d\n", 
               current_choseong_index, current_jungseong_index, current_jongseong_index);
        
                 // Safety check for all array bounds
        const char** choseong_array = get_korean_choseong();
        const char** jungseong_array = get_korean_jungseong();
        const char** jongseong_array = get_korean_jongseong();
        size_t choseong_size = get_korean_choseong_size();
        size_t jungseong_size = get_korean_jungseong_size();
        size_t jongseong_size = get_korean_jongseong_size();
        
        if (current_choseong_index >= 0 && current_choseong_index < choseong_size && 
            choseong_array[current_choseong_index] != NULL &&
            current_jungseong_index >= 0 && current_jungseong_index < jungseong_size && 
            jungseong_array[current_jungseong_index] != NULL &&
            current_jongseong_index >= 0 && current_jongseong_index < jongseong_size && 
            jongseong_array[current_jongseong_index] != NULL) {
             
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
        update_choseong_label();
        update_jungseong_label();
        update_jongseong_label();
    }
}

static void clear_korean_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("Clear button clicked\n");
        // Clear the buffer
        korean_buffer[0] = '\0';
        // Reset display to show current characters instead of buffer
        update_choseong_label();
        update_jungseong_label();
        update_jongseong_label();
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
    lv_obj_set_size(prev_btn, 30, 30);
    lv_obj_t * prev_label = lv_label_create(prev_btn);
    lv_label_set_text(prev_label, "<");
    lv_obj_add_event_cb(prev_btn, choseong_prev_korean_cb, LV_EVENT_CLICKED, NULL);
    
    // Current Korean character display (center label)
    choseong_diplay_label = lv_label_create(nav_container);
    lv_obj_set_style_text_font(choseong_diplay_label, korean_font, 0);
    lv_obj_set_style_bg_color(choseong_diplay_label, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(choseong_diplay_label, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(choseong_diplay_label, 2, 0);
    lv_obj_set_style_border_color(choseong_diplay_label, lv_color_hex(0x333333), 0);
    lv_obj_set_style_text_align(choseong_diplay_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_size(choseong_diplay_label, 120, 40);
    
    // Next button
    lv_obj_t * next_btn = lv_btn_create(nav_container);
    lv_obj_set_size(next_btn, 30, 30);
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
    lv_obj_set_size(jungseong_prev_btn, 30, 30);
    lv_obj_t * jungseong_prev_label = lv_label_create(jungseong_prev_btn);
    lv_label_set_text(jungseong_prev_label, "<");
    lv_obj_add_event_cb(jungseong_prev_btn, jungseong_prev_korean_cb, LV_EVENT_CLICKED, NULL);
    
    // Current Korean jungseong display (center label)
    jungseong_display_label = lv_label_create(jungseong_container);
    lv_obj_set_style_text_font(jungseong_display_label, korean_font, 0);
    lv_obj_set_style_bg_color(jungseong_display_label, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(jungseong_display_label, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(jungseong_display_label, 2, 0);
    lv_obj_set_style_border_color(jungseong_display_label, lv_color_hex(0x333333), 0);
    lv_obj_set_style_text_align(jungseong_display_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_size(jungseong_display_label, 120, 40);
    
    // Next jungseong button
    lv_obj_t * jungseong_next_btn = lv_btn_create(jungseong_container);
    lv_obj_set_size(jungseong_next_btn, 30, 30);
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
    lv_obj_set_size(jongseong_prev_btn, 30, 30);
    lv_obj_t * jongseong_prev_label = lv_label_create(jongseong_prev_btn);
    lv_label_set_text(jongseong_prev_label, "<");
    lv_obj_add_event_cb(jongseong_prev_btn, jongseong_prev_korean_cb, LV_EVENT_CLICKED, NULL);
    
    // Current Korean jongseong display (center label)
    jongseong_display_label = lv_label_create(jongseong_container);
    lv_obj_set_style_text_font(jongseong_display_label, korean_font, 0);
    lv_obj_set_style_bg_color(jongseong_display_label, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(jongseong_display_label, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(jongseong_display_label, 2, 0);
    lv_obj_set_style_border_color(jongseong_display_label, lv_color_hex(0x333333), 0);
    lv_obj_set_style_text_align(jongseong_display_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_size(jongseong_display_label, 120, 40);
    
    // Next jongseong button
    lv_obj_t * jongseong_next_btn = lv_btn_create(jongseong_container);
    lv_obj_set_size(jongseong_next_btn, 30, 30);
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
    
    // Initialize labels with 3-character display
    update_choseong_label();
    update_jungseong_label();
    update_jongseong_label();
    
    printf("Korean tab created successfully\n");
}