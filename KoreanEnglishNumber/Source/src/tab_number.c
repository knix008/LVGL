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
    // Display area for entered numbers (same position as other modes)
    lv_obj_t * keypad_display = lv_label_create(parent);
    lv_label_set_text(keypad_display, "0");
    lv_obj_set_size(keypad_display, 280, 50);  // Same size as other modes
    lv_obj_align(keypad_display, LV_ALIGN_TOP_MID, 0, 5);  // Same position as other modes
    lv_obj_set_style_bg_color(keypad_display, lv_color_hex(0x00FF00), 0); // Green background
    lv_obj_set_style_bg_opa(keypad_display, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(keypad_display, lv_color_make(128, 128, 128), 0);
    lv_obj_set_style_border_width(keypad_display, 2, 0);
    lv_obj_set_style_pad_all(keypad_display, 10, 0);
    lv_obj_set_style_text_color(keypad_display, lv_color_make(0, 0, 0), 0);  // Black text
    keypad_display_label = keypad_display;

    // Standard button dimensions (same for all modes)
    int btn_width = 85;
    int btn_height = 70;
    int btn_spacing = 8;
    int start_y = 70;  // Standard start position for all modes

    // Row 1: 1, 2, 3
    for (int i = 0; i < 3; i++) {
        lv_obj_t * btn = lv_btn_create(parent);
        lv_obj_set_size(btn, btn_width, btn_height);
        lv_obj_align(btn, LV_ALIGN_TOP_MID, (i - 1) * (btn_width + btn_spacing), start_y);
        lv_obj_t * label = lv_label_create(btn);
        char num[2] = {0};
        num[0] = '1' + i;
        lv_label_set_text(label, num);
        lv_obj_center(label);
        lv_obj_add_event_cb(btn, keypad_number_cb, LV_EVENT_CLICKED, NULL);
    }

    // Row 2: 4, 5, 6
    for (int i = 0; i < 3; i++) {
        lv_obj_t * btn = lv_btn_create(parent);
        lv_obj_set_size(btn, btn_width, btn_height);
        lv_obj_align(btn, LV_ALIGN_TOP_MID, (i - 1) * (btn_width + btn_spacing), start_y + (btn_height + btn_spacing));
        lv_obj_t * label = lv_label_create(btn);
        char num[2] = {0};
        num[0] = '4' + i;
        lv_label_set_text(label, num);
        lv_obj_center(label);
        lv_obj_add_event_cb(btn, keypad_number_cb, LV_EVENT_CLICKED, NULL);
    }

    // Row 3: 7, 8, 9
    for (int i = 0; i < 3; i++) {
        lv_obj_t * btn = lv_btn_create(parent);
        lv_obj_set_size(btn, btn_width, btn_height);
        lv_obj_align(btn, LV_ALIGN_TOP_MID, (i - 1) * (btn_width + btn_spacing), start_y + 2 * (btn_height + btn_spacing));
        lv_obj_t * label = lv_label_create(btn);
        char num[2] = {0};
        num[0] = '7' + i;
        lv_label_set_text(label, num);
        lv_obj_center(label);
        lv_obj_add_event_cb(btn, keypad_number_cb, LV_EVENT_CLICKED, NULL);
    }

    // Row 4: Clear, 0, Back
    lv_obj_t * clear_btn = lv_btn_create(parent);
    lv_obj_set_size(clear_btn, btn_width, btn_height);
    lv_obj_align(clear_btn, LV_ALIGN_TOP_MID, -(btn_width + btn_spacing), start_y + 3 * (btn_height + btn_spacing));
    lv_obj_t * clear_label = lv_label_create(clear_btn);
    lv_label_set_text(clear_label, "Clear");
    lv_obj_center(clear_label);
    lv_obj_add_event_cb(clear_btn, keypad_clear_cb, LV_EVENT_CLICKED, NULL);

    // 0 button in center
    lv_obj_t * zero_btn = lv_btn_create(parent);
    lv_obj_set_size(zero_btn, btn_width, btn_height);
    lv_obj_align(zero_btn, LV_ALIGN_TOP_MID, 0, start_y + 3 * (btn_height + btn_spacing));
    lv_obj_t * zero_label = lv_label_create(zero_btn);
    lv_label_set_text(zero_label, "0");
    lv_obj_center(zero_label);
    lv_obj_add_event_cb(zero_btn, keypad_number_cb, LV_EVENT_CLICKED, NULL);

    // Backspace button
    lv_obj_t * backspace_btn = lv_btn_create(parent);
    lv_obj_set_size(backspace_btn, btn_width, btn_height);
    lv_obj_align(backspace_btn, LV_ALIGN_TOP_MID, (btn_width + btn_spacing), start_y + 3 * (btn_height + btn_spacing));
    lv_obj_t * backspace_label = lv_label_create(backspace_btn);
    lv_label_set_text(backspace_label, "back");
    lv_obj_center(backspace_label);
    lv_obj_add_event_cb(backspace_btn, keypad_backspace_cb, LV_EVENT_CLICKED, NULL);

    // Row 5: Enter button (centered, wider)
    lv_obj_t * enter_btn = lv_btn_create(parent);
    lv_obj_set_size(enter_btn, 120, 60);
    lv_obj_align(enter_btn, LV_ALIGN_TOP_MID, 0, start_y + 4 * (btn_height + btn_spacing));
    lv_obj_t * enter_label = lv_label_create(enter_btn);
    lv_label_set_text(enter_label, "Enter");
    lv_obj_center(enter_label);
    lv_obj_add_event_cb(enter_btn, keypad_enter_cb, LV_EVENT_CLICKED, NULL);
} 