#include "tab_number.h"
#include <string.h>
#include <stdio.h>
#include "ui_components.h"

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
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("Number: Enter 버튼 클릭\n");
        
        // Get current text from the display label BEFORE clearing
        const char* current_text = lv_label_get_text(keypad_display_label);
        
        // Create popup dialog to show the result BEFORE clearing
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
        lv_label_set_text(title_label, "Number Input Result");
        lv_obj_set_style_text_font(title_label, &lv_font_montserrat_14, 0);
        lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 10);
        
        // Result text label
        lv_obj_t * popup_result_label = lv_label_create(dialog);
        lv_obj_set_style_text_font(popup_result_label, &lv_font_montserrat_14, 0);
        lv_obj_set_style_text_color(popup_result_label, lv_color_hex(0x00AA00), 0); // Green text
        lv_obj_align(popup_result_label, LV_ALIGN_CENTER, 0, 0);
        
        char popup_text[300];
        snprintf(popup_text, sizeof(popup_text), "Result: %s", current_text);
        lv_label_set_text(popup_result_label, popup_text);
        
        // Close button
        lv_obj_t * close_btn = lv_btn_create(dialog);
        lv_obj_set_size(close_btn, 80, 40);
        lv_obj_align(close_btn, LV_ALIGN_BOTTOM_MID, 0, 0);
        lv_obj_t * close_label = lv_label_create(close_btn);
        lv_label_set_text(close_label, "Close");
        lv_obj_center(close_label);
        
        // Close button callback
        lv_obj_add_event_cb(close_btn, keypad_close_dialog_cb, LV_EVENT_CLICKED, NULL);
        
        // Don't clear the result immediately - keep it visible until Close is clicked
        // The result will be cleared when the Close button is clicked
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

// Callback function for closing popup dialog
void keypad_close_dialog_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        // Clear the buffer when Close button is clicked
        keypad_buffer[0] = '\0';
        lv_label_set_text(keypad_display_label, "0");
        
        // Close the dialog
        lv_obj_t * btn = lv_event_get_target(e);
        lv_obj_t * dialog = lv_obj_get_parent(btn);
        lv_obj_del(dialog);
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
    lv_obj_set_style_bg_color(keypad_display, lv_color_hex(0x00FF00), 0); // Green background like QWERTY tab
    lv_obj_set_style_bg_opa(keypad_display, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_main_stop(keypad_display, 0, 0); // Ensure main color is applied
    lv_obj_set_style_bg_grad_color(keypad_display, lv_color_hex(0x00FF00), 0); // Set gradient color to same green
    lv_obj_set_style_bg_grad_dir(keypad_display, LV_GRAD_DIR_NONE, 0); // No gradient, solid color
    lv_obj_set_style_border_color(keypad_display, lv_color_make(128, 128, 128), 0); // Gray border like QWERTY tab
    lv_obj_set_style_border_width(keypad_display, 3, 0); // Thicker border like QWERTY tab
    lv_obj_set_style_pad_all(keypad_display, 15, 0); // More padding like QWERTY tab
    lv_obj_set_size(keypad_display, 400, 60); // Match QWERTY tab size
    lv_obj_align(keypad_display, LV_ALIGN_TOP_MID, 0, 10); // Match QWERTY tab position
    keypad_display_label = keypad_display;

    // Create number buttons in a 3x4 grid layout
    const char * number_labels[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"};
    int btn_width = 60;  // Increased from 50 to 80
    int btn_height = 40; // Increased from 30 to 50
    int btn_spacing = 8; // Increased from 5 to 8
    int grid_width = 3 * btn_width + 2 * btn_spacing; // Total width of 3 buttons with spacing
    int grid_height = 4 * btn_height + 3 * btn_spacing; // Total height of 4 rows with spacing
    
    // Calculate center offsets to position the grid in the center
    int center_x_offset = -(grid_width / 3);
    int center_y_offset = -120; // Move up from center for better positioning

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

    int y_offset = 50;
    // Function buttons
    lv_obj_t * clear_btn = lv_btn_create(parent);
    lv_obj_set_size(clear_btn, btn_width, btn_height);
    lv_obj_align(clear_btn, LV_ALIGN_CENTER, center_x_offset, center_y_offset + 3 * (btn_height + btn_spacing) + y_offset);
    
    lv_obj_t * clear_label = lv_label_create(clear_btn);
    lv_label_set_text(clear_label, "Clear");
    lv_obj_center(clear_label);
    lv_obj_add_event_cb(clear_btn, keypad_clear_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * backspace_btn = lv_btn_create(parent);
    lv_obj_set_size(backspace_btn, btn_width, btn_height);
    lv_obj_align(backspace_btn, LV_ALIGN_CENTER, center_x_offset + btn_width + btn_spacing, center_y_offset + 3 * (btn_height + btn_spacing) + y_offset);
    
    lv_obj_t * backspace_label = lv_label_create(backspace_btn);
    lv_label_set_text(backspace_label, "Back");
    lv_obj_center(backspace_label);
    lv_obj_add_event_cb(backspace_btn, keypad_backspace_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * enter_btn = lv_btn_create(parent);
    lv_obj_set_size(enter_btn, btn_width, btn_height);
    lv_obj_align(enter_btn, LV_ALIGN_CENTER, center_x_offset + 2 * (btn_width + btn_spacing), center_y_offset + 3 * (btn_height + btn_spacing) + y_offset);
    
    lv_obj_t * enter_label = lv_label_create(enter_btn);
    lv_label_set_text(enter_label, "Enter");
    lv_obj_center(enter_label);
    lv_obj_add_event_cb(enter_btn, keypad_enter_cb, LV_EVENT_CLICKED, NULL);
} 