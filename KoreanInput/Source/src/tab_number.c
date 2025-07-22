#include "tab_number.h"
#include <string.h>
#include <stdio.h>

// Global variables for number keypad
static char keypad_buffer[32] = "";
static lv_obj_t * keypad_display_label = NULL;

// Number keypad callback functions
void keypad_number_cb(lv_event_t * e) {
    lv_obj_t * btn = lv_event_get_target(e);
    lv_obj_t * label = lv_obj_get_child(btn, 0);
    const char * number = lv_label_get_text(label);
    
    // Add number to buffer if there's space
    if (strlen(keypad_buffer) < sizeof(keypad_buffer) - 1) {
        strcat(keypad_buffer, number);
        lv_label_set_text(keypad_display_label, keypad_buffer);
    }
}

void keypad_clear_cb(lv_event_t * e) {
    keypad_buffer[0] = '\0';
    lv_label_set_text(keypad_display_label, "0");
}

void keypad_enter_cb(lv_event_t * e) {
    if (strlen(keypad_buffer) > 0) {
        printf("Entered number: %s\n", keypad_buffer);
        // Here you can add logic to process the entered number
        // For now, just clear the buffer
        keypad_buffer[0] = '\0';
        lv_label_set_text(keypad_display_label, "0");
    }
}

void keypad_backspace_cb(lv_event_t * e) {
    int len = strlen(keypad_buffer);
    if (len > 0) {
        keypad_buffer[len - 1] = '\0';
        if (strlen(keypad_buffer) == 0) {
            lv_label_set_text(keypad_display_label, "0");
        } else {
            lv_label_set_text(keypad_display_label, keypad_buffer);
        }
    }
}

// Create Number tab
void create_number_tab(lv_obj_t * parent) {
    // Tab 4: Number Keypad
    lv_obj_t * keypad_label = lv_label_create(parent);
    lv_label_set_text(keypad_label, "Number Keypad");
    lv_obj_align(keypad_label, LV_ALIGN_TOP_MID, 0, 10);

    // Display area for entered numbers
    lv_obj_t * keypad_display = lv_label_create(parent);
    lv_label_set_text(keypad_display, "0");
    lv_obj_set_style_bg_color(keypad_display, lv_color_hex(0xE0E0E0), 0);
    lv_obj_set_style_bg_opa(keypad_display, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(keypad_display, 10, 0);
    lv_obj_set_size(keypad_display, 260, 40);
    lv_obj_align(keypad_display, LV_ALIGN_TOP_MID, 0, 40);
    keypad_display_label = keypad_display;

    // Create number buttons in a 3x4 grid layout
    const char * number_labels[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"};
    int btn_width = 50;
    int btn_height = 30;
    int btn_spacing = 5;
    int grid_width = 3 * btn_width + 2 * btn_spacing; // Total width of 3 buttons with spacing
    int grid_height = 4 * btn_height + 3 * btn_spacing; // Total height of 4 rows with spacing
    
    // Calculate center offsets to position the grid in the center
    int center_x_offset = -(grid_width / 3);
    int center_y_offset = -50; // Move up from center for better positioning

    for (int i = 0; i < 10; i++) {
        lv_obj_t * btn = lv_btn_create(parent);
        lv_obj_set_size(btn, btn_width, btn_height);
        
        // Position buttons in a 3x4 grid (1-9 in 3x3, 0 at bottom center)
        int row, col;
        if (i == 9) { // 0 button
            row = 3;
            col = 1;
        } else {
            row = i / 3;
            col = i % 3;
        }
        
        // Calculate relative position from center
        int rel_x = center_x_offset + col * (btn_width + btn_spacing);
        int rel_y = center_y_offset + row * (btn_height + btn_spacing);
        
        lv_obj_align(btn, LV_ALIGN_CENTER, rel_x, rel_y);

        lv_obj_t * btn_label = lv_label_create(btn);
        lv_label_set_text(btn_label, number_labels[i]);
        lv_obj_center(btn_label);

        lv_obj_add_event_cb(btn, keypad_number_cb, LV_EVENT_CLICKED, NULL);
    }

    // Function buttons
    lv_obj_t * clear_btn = lv_btn_create(parent);
    lv_obj_set_size(clear_btn, btn_width, btn_height);
    lv_obj_align(clear_btn, LV_ALIGN_CENTER, center_x_offset, center_y_offset + 3 * (btn_height + btn_spacing) + 10);
    
    lv_obj_t * clear_label = lv_label_create(clear_btn);
    lv_label_set_text(clear_label, "C");
    lv_obj_center(clear_label);
    lv_obj_add_event_cb(clear_btn, keypad_clear_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * backspace_btn = lv_btn_create(parent);
    lv_obj_set_size(backspace_btn, btn_width, btn_height);
    lv_obj_align(backspace_btn, LV_ALIGN_CENTER, center_x_offset + btn_width + btn_spacing, center_y_offset + 3 * (btn_height + btn_spacing) + 10);
    
    lv_obj_t * backspace_label = lv_label_create(backspace_btn);
    lv_label_set_text(backspace_label, "⌫");
    lv_obj_center(backspace_label);
    lv_obj_add_event_cb(backspace_btn, keypad_backspace_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * enter_btn = lv_btn_create(parent);
    lv_obj_set_size(enter_btn, btn_width, btn_height);
    lv_obj_align(enter_btn, LV_ALIGN_CENTER, center_x_offset + 2 * (btn_width + btn_spacing), center_y_offset + 3 * (btn_height + btn_spacing) + 10);
    
    lv_obj_t * enter_label = lv_label_create(enter_btn);
    lv_label_set_text(enter_label, "Enter");
    lv_obj_center(enter_label);
    lv_obj_add_event_cb(enter_btn, keypad_enter_cb, LV_EVENT_CLICKED, NULL);
} 