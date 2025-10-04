#include "tab_english.h"
#include "ui_components.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdbool.h>

// Static variables for English T9 input
static char english_buffer[256] = "";
static lv_obj_t * english_display_label = NULL;

// Button labels for T9-style English input (lowercase)
static const char* english_button_labels[] = {
    "abc",    // Button 1
    "def",    // Button 2
    "ghi",    // Button 3
    "jkl",    // Button 4
    "mno",    // Button 5
    "pqr",    // Button 6
    "stu",    // Button 7
    "vwx",    // Button 8
    "yz",     // Button 9
    "Space"   // Button 0
};

// Button labels for T9-style English input (uppercase)
static const char* english_button_labels_upper[] = {
    "ABC",    // Button 1
    "DEF",    // Button 2
    "GHI",    // Button 3
    "JKL",    // Button 4
    "MNO",    // Button 5
    "PQR",    // Button 6
    "STU",    // Button 7
    "VWX",    // Button 8
    "YZ",     // Button 9
    "Space"   // Button 0
};

// Actual character mappings (lowercase)
static const char* english_char_maps[] = {
    "abc",    // Button 1
    "def",    // Button 2
    "ghi",    // Button 3
    "jkl",    // Button 4
    "mno",    // Button 5
    "pqr",    // Button 6
    "stu",    // Button 7
    "vwx",    // Button 8
    "yz",     // Button 9
    " "       // Button 0 - space
};

// Actual character mappings (uppercase)
static const char* english_char_maps_upper[] = {
    "ABC",    // Button 1
    "DEF",    // Button 2
    "GHI",    // Button 3
    "JKL",    // Button 4
    "MNO",    // Button 5
    "PQR",    // Button 6
    "STU",    // Button 7
    "VWX",    // Button 8
    "YZ",     // Button 9
    " "       // Button 0 - space
};

// Track current button press counts for T9 cycling
static int button_press_counts[10] = {0};
static int last_button_pressed = -1;
static bool shift_active = false;  // Track shift state

// Store references to button labels for updating when shift changes
static lv_obj_t * english_button_label_refs[9] = {NULL}; // Only 9 main buttons (0-8)
static lv_obj_t * shift_button_ref = NULL; // Reference to shift button for updating appearance

// Update display function
static void update_english_display(void) {
    if (english_display_label) {
        lv_label_set_text(english_display_label, english_buffer);
    }
}

// Update button labels based on shift state
static void update_button_labels(void) {
    for (int i = 0; i < 9; i++) {
        if (english_button_label_refs[i] != NULL) {
            const char* label_text = shift_active ? english_button_labels_upper[i] : english_button_labels[i];
            lv_label_set_text(english_button_label_refs[i], label_text);
        }
    }
}

// Update shift button appearance based on shift state
static void update_shift_button(void) {
    if (shift_button_ref != NULL) {
        lv_obj_t * label = lv_obj_get_child(shift_button_ref, 0);
        if (shift_active) {
            lv_label_set_text(label, "SHIFT");
            lv_obj_set_style_bg_color(shift_button_ref, lv_color_hex(0x4CAF50), 0); // Green when active
        } else {
            lv_label_set_text(label, "shift");
            lv_obj_set_style_bg_color(shift_button_ref, lv_color_hex(0x808080), 0); // Gray when inactive
        }
    }
}

// English button callback - T9 style cycling
static void english_button_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        int* button_index = (int*)lv_event_get_user_data(e);
        int btn_idx = *button_index;

        // Choose character set based on shift state
        const char* chars = shift_active ? english_char_maps_upper[btn_idx] : english_char_maps[btn_idx];
        int char_count = strlen(chars);

        if (char_count == 0) return;

        // If same button pressed again, cycle through characters
        if (last_button_pressed == btn_idx) {
            button_press_counts[btn_idx] = (button_press_counts[btn_idx] + 1) % char_count;
            // Replace last character
            if (strlen(english_buffer) > 0) {
                english_buffer[strlen(english_buffer) - 1] = '\0';
            }
        } else {
            // Different button, reset counter
            button_press_counts[btn_idx] = 0;
            last_button_pressed = btn_idx;
        }

        // Add the current character
        char current_char = chars[button_press_counts[btn_idx]];
        size_t len = strlen(english_buffer);
        if (len < sizeof(english_buffer) - 1) {
            english_buffer[len] = current_char;
            english_buffer[len + 1] = '\0';
        }

        update_english_display();
    }
}

// Next button - move to next character (finalize current)
static void english_next_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        // Reset button tracking to allow new character
        last_button_pressed = -1;
        for (int i = 0; i < 10; i++) {
            button_press_counts[i] = 0;
        }
    }
}

// Backspace callback
static void english_backspace_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        int len = strlen(english_buffer);
        if (len > 0) {
            english_buffer[len - 1] = '\0';
            update_english_display();
        }
        // Reset button tracking
        last_button_pressed = -1;
        for (int i = 0; i < 10; i++) {
            button_press_counts[i] = 0;
        }
    }
}

// Clear callback
static void english_clear_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        english_buffer[0] = '\0';
        update_english_display();
        last_button_pressed = -1;
        shift_active = false;  // Reset shift state when clearing
        update_button_labels(); // Update button labels to reflect shift reset
        update_shift_button();  // Update shift button appearance
        for (int i = 0; i < 10; i++) {
            button_press_counts[i] = 0;
        }
    }
}

// Shift callback - toggle between uppercase and lowercase
static void english_shift_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        shift_active = !shift_active;
        
        // Update all button labels and shift button appearance
        update_button_labels();
        update_shift_button();
        
        // Reset button tracking when shift changes
        last_button_pressed = -1;
        for (int i = 0; i < 10; i++) {
            button_press_counts[i] = 0;
        }
    }
}

// Enter callback
static void english_enter_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("English: Enter button clicked\n");

        const char* current_text = lv_label_get_text(english_display_label);

        // Create popup dialog
        lv_obj_t * parent = lv_obj_get_parent(lv_event_get_target(e));
        lv_obj_t * dialog = lv_obj_create(parent);
        lv_obj_set_size(dialog, 250, 150);
        lv_obj_align(dialog, LV_ALIGN_CENTER, 0, 0);
        lv_obj_set_style_bg_color(dialog, lv_color_white(), 0);
        lv_obj_set_style_bg_opa(dialog, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(dialog, 2, 0);
        lv_obj_set_style_border_color(dialog, lv_color_hex(0x333333), 0);
        lv_obj_set_style_radius(dialog, 10, 0);

        // Title label
        lv_obj_t * title_label = lv_label_create(dialog);
        lv_label_set_text(title_label, "English Input Result");
        lv_obj_set_style_text_font(title_label, get_korean_font_small(), 0); // Use Korean font for consistency
        lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 10);

        // Result text label
        lv_obj_t * popup_result_label = lv_label_create(dialog);
        lv_obj_set_style_text_font(popup_result_label, get_korean_font_small(), 0); // Use Korean font for mixed text
        lv_obj_set_style_text_color(popup_result_label, lv_color_hex(0x00AA00), 0);
        lv_obj_align(popup_result_label, LV_ALIGN_CENTER, 0, 0);

        char popup_text[300];
        snprintf(popup_text, sizeof(popup_text), "Result: %s", current_text);
        lv_label_set_text(popup_result_label, popup_text);

        // Close button
        lv_obj_t * close_btn = lv_btn_create(dialog);
        lv_obj_set_size(close_btn, 80, 30);
        lv_obj_align(close_btn, LV_ALIGN_BOTTOM_MID, 0, -10);
        lv_obj_t * close_label = lv_label_create(close_btn);
        lv_label_set_text(close_label, "Close");
        lv_obj_center(close_label);

        // Close button callback
        lv_obj_add_event_cb(close_btn, english_close_dialog_cb, LV_EVENT_CLICKED, NULL);
    }
}

// Close dialog callback
void english_close_dialog_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        // Clear buffer
        english_buffer[0] = '\0';
        update_english_display();

        // Reset tracking
        last_button_pressed = -1;
        shift_active = false;  // Reset shift state
        update_button_labels(); // Update button labels to reflect shift reset
        update_shift_button();  // Update shift button appearance
        for (int i = 0; i < 10; i++) {
            button_press_counts[i] = 0;
        }

        // Close the dialog
        lv_obj_t * btn = lv_event_get_target(e);
        lv_obj_t * dialog = lv_obj_get_parent(btn);
        lv_obj_del(dialog);
    }
}

// Create English T9-style input tab
lv_obj_t* create_english_tab(lv_obj_t* parent) {
    // Reset buffer and tracking
    english_buffer[0] = '\0';
    last_button_pressed = -1;
    shift_active = false;  // Reset shift state
    for (int i = 0; i < 10; i++) {
        button_press_counts[i] = 0;
    }
    
    // Initialize button label references
    for (int i = 0; i < 9; i++) {
        english_button_label_refs[i] = NULL;
    }
    shift_button_ref = NULL;

    // Create tab container
    lv_obj_t* tab = lv_obj_create(parent);
    lv_obj_set_size(tab, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_all(tab, 10, 0);

    // Create display label for 320x640
    english_display_label = lv_label_create(tab);
    lv_obj_set_size(english_display_label, 280, 50);
    lv_obj_align(english_display_label, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_style_bg_color(english_display_label, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_bg_opa(english_display_label, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(english_display_label, lv_color_make(128, 128, 128), 0);
    lv_obj_set_style_border_width(english_display_label, 2, 0);
    lv_obj_set_style_pad_all(english_display_label, 10, 0);
    lv_obj_set_style_text_color(english_display_label, lv_color_make(0, 0, 0), 0);
    lv_obj_set_style_text_font(english_display_label, get_korean_font_small(), 0); // Use Korean font for mixed text support
    lv_label_set_text(english_display_label, "");

    // Standard button dimensions (same for all modes)
    int btn_width = 85;
    int btn_height = 70;
    int btn_spacing = 8;
    int start_y = 70;  // Standard start position for all modes

    // Create 3x3 grid for letters (buttons 1-9)
    for (int i = 0; i < 9; i++) {
        lv_obj_t * btn = lv_btn_create(tab);
        lv_obj_set_size(btn, btn_width, btn_height);

        int row = i / 3;
        int col = i % 3;

        int x = (col - 1) * (btn_width + btn_spacing);
        int y = start_y + row * (btn_height + btn_spacing);

        lv_obj_align(btn, LV_ALIGN_TOP_MID, x, y);

        lv_obj_t * label = lv_label_create(btn);
        lv_label_set_text(label, english_button_labels[i]); // Start with lowercase
        lv_obj_center(label);
        
        // Store label reference for updating when shift changes
        english_button_label_refs[i] = label;

        int* btn_idx = malloc(sizeof(int));
        *btn_idx = i;
        lv_obj_add_event_cb(btn, english_button_cb, LV_EVENT_CLICKED, btn_idx);
    }

    // Row 4: Special buttons (Next, Space, Backspace)
    int row4_y = start_y + 3 * (btn_height + btn_spacing);

    // Next button (left)
    lv_obj_t * next_btn = lv_btn_create(tab);
    lv_obj_set_size(next_btn, btn_width, btn_height);
    lv_obj_align(next_btn, LV_ALIGN_TOP_MID, -(btn_width + btn_spacing), row4_y);
    lv_obj_t * next_label = lv_label_create(next_btn);
    lv_label_set_text(next_label, "Next");
    lv_obj_center(next_label);
    lv_obj_add_event_cb(next_btn, english_next_cb, LV_EVENT_CLICKED, NULL);

    // Space button (center) - button 9 (index 9)
    lv_obj_t * space_btn = lv_btn_create(tab);
    lv_obj_set_size(space_btn, btn_width, btn_height);
    lv_obj_align(space_btn, LV_ALIGN_TOP_MID, 0, row4_y);
    lv_obj_t * space_label = lv_label_create(space_btn);
    lv_label_set_text(space_label, "Space");
    lv_obj_center(space_label);
    int* space_idx = malloc(sizeof(int));
    *space_idx = 9;
    lv_obj_add_event_cb(space_btn, english_button_cb, LV_EVENT_CLICKED, space_idx);

    // Backspace button (right)
    lv_obj_t * backspace_btn = lv_btn_create(tab);
    lv_obj_set_size(backspace_btn, btn_width, btn_height);
    lv_obj_align(backspace_btn, LV_ALIGN_TOP_MID, (btn_width + btn_spacing), row4_y);
    lv_obj_t * backspace_label = lv_label_create(backspace_btn);
    lv_label_set_text(backspace_label, "←");
    lv_obj_set_style_text_font(backspace_label, get_korean_font_small(), 0); // Use Korean font for symbols
    lv_obj_center(backspace_label);
    lv_obj_add_event_cb(backspace_btn, english_backspace_cb, LV_EVENT_CLICKED, NULL);

    // Row 5: Shift, Clear, and Enter buttons
    int row5_y = start_y + 4 * (btn_height + btn_spacing);

    // Shift button (left)
    lv_obj_t * shift_btn = lv_btn_create(tab);
    lv_obj_set_size(shift_btn, btn_width, btn_height);
    lv_obj_align(shift_btn, LV_ALIGN_TOP_MID, -(btn_width + btn_spacing), row5_y);
    lv_obj_t * shift_label = lv_label_create(shift_btn);
    lv_label_set_text(shift_label, "shift");
    lv_obj_center(shift_label);
    lv_obj_set_style_bg_color(shift_btn, lv_color_hex(0x808080), 0); // Gray when inactive
    lv_obj_add_event_cb(shift_btn, english_shift_cb, LV_EVENT_CLICKED, NULL);
    
    // Store shift button reference for updating appearance
    shift_button_ref = shift_btn;

    // Clear button (center)
    lv_obj_t * clear_btn = lv_btn_create(tab);
    lv_obj_set_size(clear_btn, btn_width, btn_height);
    lv_obj_align(clear_btn, LV_ALIGN_TOP_MID, 0, row5_y);
    lv_obj_t * clear_label = lv_label_create(clear_btn);
    lv_label_set_text(clear_label, "Clear");
    lv_obj_center(clear_label);
    lv_obj_add_event_cb(clear_btn, english_clear_cb, LV_EVENT_CLICKED, NULL);

    // Enter button (right)
    lv_obj_t * enter_btn = lv_btn_create(tab);
    lv_obj_set_size(enter_btn, btn_width, btn_height);
    lv_obj_align(enter_btn, LV_ALIGN_TOP_MID, (btn_width + btn_spacing), row5_y);
    lv_obj_t * enter_label = lv_label_create(enter_btn);
    lv_label_set_text(enter_label, "Enter");
    lv_obj_center(enter_label);
    lv_obj_add_event_cb(enter_btn, english_enter_cb, LV_EVENT_CLICKED, NULL);

    return tab;
}
