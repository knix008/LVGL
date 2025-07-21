#include "ui_keypad.h"
#include <string.h>

// Global variables for number keypad
char keypad_buffer[32] = "";
lv_obj_t * keypad_display_label = NULL;

// Initialize keypad buffer
void init_keypad_buffer(void) {
    keypad_buffer[0] = '\0';
}

// Create keypad UI components
void create_keypad_ui(lv_obj_t * parent) {
    // Keypad title
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
    int btn_width = 70;
    int btn_height = 50;
    int start_x = 35; // Move the grid further to the right to center it
    int start_y = 100;

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
        
        lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 
                    start_x + col * (btn_width + 10), 
                    start_y + row * (btn_height + 10));

        lv_obj_t * btn_label = lv_label_create(btn);
        lv_label_set_text(btn_label, number_labels[i]);
        lv_obj_center(btn_label);

        lv_obj_add_event_cb(btn, keypad_number_cb, LV_EVENT_CLICKED, NULL);
    }

    // Function buttons
    lv_obj_t * clear_btn = lv_btn_create(parent);
    lv_obj_set_size(clear_btn, btn_width, btn_height);
    lv_obj_align(clear_btn, LV_ALIGN_TOP_LEFT, start_x, start_y + 3 * (btn_height + 10) + 20);
    
    lv_obj_t * clear_label = lv_label_create(clear_btn);
    lv_label_set_text(clear_label, "C");
    lv_obj_center(clear_label);
    lv_obj_add_event_cb(clear_btn, keypad_clear_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * backspace_btn = lv_btn_create(parent);
    lv_obj_set_size(backspace_btn, btn_width, btn_height);
    lv_obj_align(backspace_btn, LV_ALIGN_TOP_LEFT, start_x + btn_width + 10, start_y + 3 * (btn_height + 10) + 20);
    
    lv_obj_t * backspace_label = lv_label_create(backspace_btn);
    lv_label_set_text(backspace_label, "⌫");
    lv_obj_center(backspace_label);
    lv_obj_add_event_cb(backspace_btn, keypad_backspace_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * enter_btn = lv_btn_create(parent);
    lv_obj_set_size(enter_btn, btn_width, btn_height);
    lv_obj_align(enter_btn, LV_ALIGN_TOP_LEFT, start_x + 2 * (btn_width + 10), start_y + 3 * (btn_height + 10) + 20);
    
    lv_obj_t * enter_label = lv_label_create(enter_btn);
    lv_label_set_text(enter_label, "Enter");
    lv_obj_center(enter_label);
    lv_obj_add_event_cb(enter_btn, keypad_enter_cb, LV_EVENT_CLICKED, NULL);
} 