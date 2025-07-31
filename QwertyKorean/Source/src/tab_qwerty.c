#include "tab_qwerty.h"
#include "font_config.h"
#include "ui_components.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "qwerty_korean.h"

// QWERTY Korean input system - using korean.c functions
static char g_qwerty_input_buffer[MAX_OUTPUT_LEN] = "";
static size_t g_qwerty_input_len = 0;
static wchar_t g_qwerty_output_buffer[MAX_OUTPUT_LEN] = L"";

// Function to map Korean characters to English keys for process_input()
static char map_korean_to_english(const char* korean_char) {
    // Map based on Korean system's key mappings
    if (strcmp(korean_char, "ㅂ") == 0) return 'q';  // ㅂ
    if (strcmp(korean_char, "ㅈ") == 0) return 'w';  // ㅈ
    if (strcmp(korean_char, "ㄷ") == 0) return 'e';  // ㄷ
    if (strcmp(korean_char, "ㄱ") == 0) return 'r';  // ㄱ
    if (strcmp(korean_char, "ㅅ") == 0) return 't';  // ㅅ
    if (strcmp(korean_char, "ㅛ") == 0) return 'y';  // ㅛ
    if (strcmp(korean_char, "ㅕ") == 0) return 'u';  // ㅕ
    if (strcmp(korean_char, "ㅑ") == 0) return 'i';  // ㅑ
    if (strcmp(korean_char, "ㅐ") == 0) return 'o';  // ㅐ
    if (strcmp(korean_char, "ㅔ") == 0) return 'p';  // ㅔ
    if (strcmp(korean_char, "ㅁ") == 0) return 'a';  // ㅁ
    if (strcmp(korean_char, "ㄴ") == 0) return 's';  // ㄴ
    if (strcmp(korean_char, "ㅇ") == 0) return 'd';  // ㅇ
    if (strcmp(korean_char, "ㄹ") == 0) return 'f';  // ㄹ
    if (strcmp(korean_char, "ㅎ") == 0) return 'g';  // ㅎ
    if (strcmp(korean_char, "ㅗ") == 0) return 'h';  // ㅗ
    if (strcmp(korean_char, "ㅓ") == 0) return 'j';  // ㅓ
    if (strcmp(korean_char, "ㅏ") == 0) return 'k';  // ㅏ
    if (strcmp(korean_char, "ㅣ") == 0) return 'l';  // ㅣ
    if (strcmp(korean_char, "ㅋ") == 0) return 'z';  // ㅋ
    if (strcmp(korean_char, "ㅌ") == 0) return 'x';  // ㅌ
    if (strcmp(korean_char, "ㅊ") == 0) return 'c';  // ㅊ
    if (strcmp(korean_char, "ㅍ") == 0) return 'v';  // ㅍ
    if (strcmp(korean_char, "ㅠ") == 0) return 'b';  // ㅠ
    if (strcmp(korean_char, "ㅜ") == 0) return 'n';  // ㅜ
    if (strcmp(korean_char, "ㅡ") == 0) return 'm';  // ㅡ
    // Shifted characters (Korean double consonants/vowels -> uppercase English)
    if (strcmp(korean_char, "ㅃ") == 0) return 'Q';  // ㅃ -> Q
    if (strcmp(korean_char, "ㅉ") == 0) return 'W';  // ㅉ -> W
    if (strcmp(korean_char, "ㄸ") == 0) return 'E';  // ㄸ -> E
    if (strcmp(korean_char, "ㄲ") == 0) return 'R';  // ㄲ -> R
    if (strcmp(korean_char, "ㅆ") == 0) return 'T';  // ㅆ -> T
    if (strcmp(korean_char, "ㅒ") == 0) return 'O';  // ㅒ -> O
    if (strcmp(korean_char, "ㅖ") == 0) return 'P';  // ㅖ -> P
    
    return '\0'; // Unknown character
}

// Korean QWERTY keyboard layout (Korean characters for display)
static const char* qwerty_keys[3][10] = {
    {"ㅂ", "ㅈ", "ㄷ", "ㄱ", "ㅅ", "ㅛ", "ㅕ", "ㅑ", "ㅐ", "ㅔ"},
    {"ㅁ", "ㄴ", "ㅇ", "ㄹ", "ㅎ", "ㅗ", "ㅓ", "ㅏ", "ㅣ", ""},
    {"ㅋ", "ㅌ", "ㅊ", "ㅍ", "ㅠ", "ㅜ", "ㅡ", "", "", ""}
};

// Shifted Korean QWERTY keyboard layout (double consonants and vowels)
static const char* qwerty_keys_shifted[3][10] = {
    {"ㅃ", "ㅉ", "ㄸ", "ㄲ", "ㅆ", "ㅛ", "ㅕ", "ㅑ", "ㅒ", "ㅖ"},
    {"ㅁ", "ㄴ", "ㅇ", "ㄹ", "ㅎ", "ㅗ", "ㅓ", "ㅏ", "ㅣ", ""},
    {"ㅋ", "ㅌ", "ㅊ", "ㅍ", "ㅠ", "ㅜ", "ㅡ", "", "", ""}
};

// Special keys for the bottom row
static const char* special_keys[] = {"Shift", "Space", "←", "Enter"};

// Global variables for text display
static lv_obj_t* g_qwerty_display_label = NULL;
static bool g_qwerty_shift_mode = false;
static lv_obj_t* g_qwerty_buttons[3][10] = {NULL}; // Store button references for shift functionality

// Callback functions
static void qwerty_key_cb(lv_event_t* e);
static void qwerty_shift_cb(lv_event_t* e);
static void qwerty_space_cb(lv_event_t* e);
static void qwerty_backspace_cb(lv_event_t* e);
static void qwerty_enter_cb(lv_event_t* e);
static void popup_close_cb(lv_event_t* e);
static void popup_click_cb(lv_event_t* e);
static void qwerty_popup_close_cb(lv_event_t* e);

// Function to update the display using Korean system
static void update_qwerty_display() {
    if (g_qwerty_display_label) {
        // Convert wide string to UTF-8 for display
        char display_text[512] = "";
        wcstombs(display_text, g_qwerty_output_buffer, sizeof(display_text) - 1);
        lv_label_set_text(g_qwerty_display_label, display_text);
    }
}

// QWERTY key callback using process_input() from korean.h
static void qwerty_key_cb(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        lv_obj_t* btn = lv_event_get_target(e);
        lv_obj_t* label = lv_obj_get_child(btn, 0);
        const char* key_text = lv_label_get_text(label);
        
        if (key_text && strlen(key_text) > 0) {
            // Map Korean character to English key for process_input()
            char input_char = map_korean_to_english(key_text);

            if (input_char != '\0') {
                qwerty_process_input(g_qwerty_input_buffer, &g_qwerty_input_len, g_qwerty_output_buffer, input_char);
                update_qwerty_display();
            }
        }
    }
}

// Shift key callback for Korean double consonants and vowels
static void qwerty_shift_cb(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        g_qwerty_shift_mode = !g_qwerty_shift_mode;
        
        lv_obj_t* btn = lv_event_get_target(e);
        lv_obj_t* label = lv_obj_get_child(btn, 0);
        
        // Update shift button appearance
        if (g_qwerty_shift_mode) {
            lv_obj_set_style_bg_color(btn, lv_color_make(100, 150, 255), 0);
            lv_label_set_text(label, "SHIFT");
        } else {
            lv_obj_set_style_bg_color(btn, lv_color_make(200, 200, 200), 0);
            lv_label_set_text(label, "Shift");
        }
        
        // Update all button labels based on shift state
        for (int row = 0; row < 3; row++) {
            for (int col = 0; col < 10; col++) {
                if (g_qwerty_buttons[row][col] != NULL) {
                    lv_obj_t* key_label = lv_obj_get_child(g_qwerty_buttons[row][col], 0);
                    if (key_label != NULL) {
                        const char* new_text = g_qwerty_shift_mode ? 
                            qwerty_keys_shifted[row][col] : qwerty_keys[row][col];
                        if (strlen(new_text) > 0) {
                            lv_label_set_text(key_label, new_text);
                        }
                    }
                }
            }
        }
    }
}

// Space key callback using process_input() from korean.h
static void qwerty_space_cb(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        qwerty_process_input(g_qwerty_input_buffer, &g_qwerty_input_len, g_qwerty_output_buffer, ' ');
        update_qwerty_display();
    }
}

// Backspace key callback using process_input() from korean.h
static void qwerty_backspace_cb(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        qwerty_process_input(g_qwerty_input_buffer, &g_qwerty_input_len, g_qwerty_output_buffer, 0x7f); // Backspace
        update_qwerty_display();
    }
}

// Enter key callback using process_input() from korean.h
static void qwerty_enter_cb(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        // Get current text from the display label BEFORE processing
        const char* current_text = lv_label_get_text(g_qwerty_display_label);
        
        // Create popup dialog to show the result BEFORE clearing
        lv_obj_t* parent = lv_obj_get_parent(lv_event_get_target(e));
        lv_obj_t* dialog = lv_obj_create(parent);
        lv_obj_set_size(dialog, 250, 150);
        lv_obj_align(dialog, LV_ALIGN_CENTER, 0, 0);
        lv_obj_set_style_bg_color(dialog, lv_color_white(), 0);
        lv_obj_set_style_bg_opa(dialog, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(dialog, 2, 0);
        lv_obj_set_style_border_color(dialog, lv_color_hex(0x333333), 0);
        lv_obj_set_style_radius(dialog, 10, 0);
        
        // Title label
        lv_obj_t* title_label = lv_label_create(dialog);
        lv_label_set_text(title_label, "QWERTY Korean Input Result");
        lv_obj_set_style_text_font(title_label, &lv_font_montserrat_14, 0);
        lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 10);
        
        // Result text label
        lv_obj_t* popup_result_label = lv_label_create(dialog);
        lv_obj_set_style_text_font(popup_result_label, get_korean_font_small(), 0);
        lv_obj_set_style_text_color(popup_result_label, lv_color_hex(0x00AA00), 0); // Green text
        lv_obj_align(popup_result_label, LV_ALIGN_CENTER, 0, 0);
        
        char popup_text[300];
        if (strlen(current_text) > 0) {
            snprintf(popup_text, sizeof(popup_text), "Result: %s", current_text);
        } else {
            snprintf(popup_text, sizeof(popup_text), "Result: (Empty)");
        }
        lv_label_set_text(popup_result_label, popup_text);
        
        // Close button
        lv_obj_t* close_btn = lv_btn_create(dialog);
        lv_obj_set_size(close_btn, 80, 30);
        lv_obj_align(close_btn, LV_ALIGN_BOTTOM_MID, 0, -10);
        lv_obj_t* close_label = lv_label_create(close_btn);
        lv_label_set_text(close_label, "Close");
        lv_obj_center(close_label);
        
        // Close button callback - clear buffers when dialog closes
        lv_obj_add_event_cb(close_btn, qwerty_popup_close_cb, LV_EVENT_CLICKED, NULL);
        
        // Don't clear the result immediately - keep it visible until Close is clicked
        // The result will be cleared when the Close button is clicked
    }
}

// Create QWERTY tab
lv_obj_t* create_qwerty_tab(lv_obj_t* parent) {
    // Initialize Korean input system
    qwerty_korean_init();
    
    // Initialize buffers and shift mode
    memset(g_qwerty_input_buffer, 0, sizeof(g_qwerty_input_buffer));
    memset(g_qwerty_output_buffer, 0, sizeof(g_qwerty_output_buffer));
    g_qwerty_input_len = 0;
    g_qwerty_shift_mode = false;
    
    // Get font with fallback
    lv_font_t* font = get_korean_font_small();
    printf("QWERTY tab: Small font pointer: %p\n", (void*)font);
    if (font == NULL) {
        font = get_korean_font();
        printf("QWERTY tab: Regular font pointer: %p\n", (void*)font);
    }
    if (font == NULL) {
        // Use built-in font as final fallback
        font = (lv_font_t*)&lv_font_montserrat_14;
        printf("QWERTY tab: Using fallback font: %p\n", (void*)font);
    }
    
    // Create tab container
    lv_obj_t* tab = lv_obj_create(parent);
    lv_obj_set_size(tab, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_all(tab, 10, 0);
    
    // Create display label
    g_qwerty_display_label = lv_label_create(tab);
    lv_obj_set_size(g_qwerty_display_label, 650, 80);
    lv_obj_align(g_qwerty_display_label, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_set_style_bg_color(g_qwerty_display_label, lv_color_make(240, 240, 240), 0);
    lv_obj_set_style_bg_opa(g_qwerty_display_label, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(g_qwerty_display_label, lv_color_make(128, 128, 128), 0);
    lv_obj_set_style_border_width(g_qwerty_display_label, 2, 0);
    lv_obj_set_style_pad_all(g_qwerty_display_label, 10, 0);
    lv_obj_set_style_text_font(g_qwerty_display_label, font, 0);
    lv_obj_set_style_text_color(g_qwerty_display_label, lv_color_make(0, 0, 0), 0);
    lv_label_set_text(g_qwerty_display_label, "");
    
    // Button dimensions and spacing - further reduced size
    int btn_width = 50;
    int btn_height = 30;
    int btn_spacing = 8;
    int start_y = 120;
    
    // Create QWERTY keyboard layout
    for (int row = 0; row < 3; row++) {
        // Count valid keys in this row
        int valid_keys = 0;
        for (int col = 0; col < 10; col++) {
            if (strlen(qwerty_keys[row][col]) > 0) {
                valid_keys++;
            }
        }
        
        // Calculate center offset for this row (moved 5px to the right)
        int total_width = valid_keys * btn_width + (valid_keys - 1) * btn_spacing;
        int center_offset = -total_width / 2 + 25;
        
        int key_index = 0;
        for (int col = 0; col < 10; col++) {
            // Skip empty keys (punctuation that was removed)
            if (strlen(qwerty_keys[row][col]) == 0) {
                continue;
            }
            
            lv_obj_t* btn = lv_btn_create(tab);
            lv_obj_set_size(btn, btn_width, btn_height);
            lv_obj_align(btn, LV_ALIGN_TOP_MID, 
                        center_offset + key_index * (btn_width + btn_spacing), 
                        start_y + row * (btn_height + btn_spacing));
            
            lv_obj_t* label = lv_label_create(btn);
            lv_label_set_text(label, qwerty_keys[row][col]);
            lv_obj_set_style_text_font(label, font, 0);
            lv_obj_center(label);
            
            lv_obj_add_event_cb(btn, qwerty_key_cb, LV_EVENT_CLICKED, NULL);
            
            // Store button reference for shift functionality
            g_qwerty_buttons[row][col] = btn;
            
            key_index++;
        }
    }
    
    // Create special keys row
    int special_start_y = start_y + 3 * (btn_height + btn_spacing) + 10;
    
    // Calculate center positions for special keys row
    int shift_width = 50;
    int space_width = 100;
    int backspace_width = 50;
    int enter_width = 50;
    int special_spacing = 8;
    
    // Calculate total width and center offset (moved 5px to the right)
    int total_special_width = shift_width + special_spacing + space_width + special_spacing + backspace_width + special_spacing + enter_width;
    int special_center_offset = -total_special_width / 2 + 5;
    
    // Shift key
    lv_obj_t* shift_btn = lv_btn_create(tab);
    lv_obj_set_size(shift_btn, shift_width, btn_height);
    lv_obj_align(shift_btn, LV_ALIGN_TOP_MID, special_center_offset, special_start_y);
    lv_obj_t* shift_label = lv_label_create(shift_btn);
    lv_label_set_text(shift_label, "Shift");
    lv_obj_set_style_text_font(shift_label, font, 0);
    lv_obj_center(shift_label);
    lv_obj_add_event_cb(shift_btn, qwerty_shift_cb, LV_EVENT_CLICKED, NULL);
    
    // Space key (same height as other special keys, moved 15px to the right)
    lv_obj_t* space_btn = lv_btn_create(tab);
    lv_obj_set_size(space_btn, space_width, btn_height);
    lv_obj_align(space_btn, LV_ALIGN_TOP_MID, special_center_offset + shift_width + special_spacing + 35, special_start_y);
    lv_obj_t* space_label = lv_label_create(space_btn);
    lv_label_set_text(space_label, "Space");
    lv_obj_set_style_text_font(space_label, font, 0);
    lv_obj_center(space_label);
    lv_obj_add_event_cb(space_btn, qwerty_space_cb, LV_EVENT_CLICKED, NULL);
    
    // Backspace key
    lv_obj_t* backspace_btn = lv_btn_create(tab);
    lv_obj_set_size(backspace_btn, backspace_width, btn_height);
    lv_obj_align(backspace_btn, LV_ALIGN_TOP_MID, special_center_offset + shift_width + special_spacing + space_width + special_spacing + 15, special_start_y);
    lv_obj_t* backspace_label = lv_label_create(backspace_btn);
    lv_label_set_text(backspace_label, "←");
    lv_obj_set_style_text_font(backspace_label, font, 0);
    lv_obj_center(backspace_label);
    lv_obj_add_event_cb(backspace_btn, qwerty_backspace_cb, LV_EVENT_CLICKED, NULL);
    
    // Enter key
    lv_obj_t* enter_btn = lv_btn_create(tab);
    lv_obj_set_size(enter_btn, enter_width, btn_height);
    lv_obj_align(enter_btn, LV_ALIGN_TOP_MID, special_center_offset + shift_width + special_spacing + space_width + special_spacing + backspace_width + special_spacing + 15, special_start_y);
    lv_obj_t* enter_label = lv_label_create(enter_btn);
    lv_label_set_text(enter_label, "Enter");
    lv_obj_set_style_text_font(enter_label, font, 0);
    lv_obj_center(enter_label);
    lv_obj_add_event_cb(enter_btn, qwerty_enter_cb, LV_EVENT_CLICKED, NULL);
    
    return tab;
}

// Popup callback functions
static void popup_close_cb(lv_event_t* e) {
    lv_obj_t* btn = lv_event_get_target(e);
    lv_obj_t* popup = lv_obj_get_parent(btn);
    lv_obj_del(popup);
}

static void popup_click_cb(lv_event_t* e) {
    lv_obj_t* popup = lv_event_get_target(e);
    lv_obj_del(popup);
}

static void qwerty_popup_close_cb(lv_event_t* e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        // Clear the result when Close button is clicked
        qwerty_process_input(g_qwerty_input_buffer, &g_qwerty_input_len, g_qwerty_output_buffer, '\n');
        update_qwerty_display();
        
        // Close the dialog
        lv_obj_t* btn = lv_event_get_target(e);
        lv_obj_t* dialog = lv_obj_get_parent(btn);
        lv_obj_del(dialog);
    }
} 