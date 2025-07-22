#include "ui_korean.h"
#include <wchar.h>
#include <locale.h>
#include <string.h>
#include <stdlib.h>

// Global variables for Korean input
lv_obj_t *korean_display_label = NULL;
lv_obj_t *korean_input_label = NULL;
char korean_input_buffer[KOREAN_DISPLAY_MAX] = {0};
wchar_t korean_output_buffer[KOREAN_DISPLAY_MAX] = {0};
size_t korean_input_len = 0;

// Korean QWERTY keyboard layout
static const char korean_qwerty_layout[4][13] = {
    {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', 0, 0, 0},
    {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 0, 0, 0, 0},
    {'z', 'x', 'c', 'v', 'b', 'n', 'm', 0, 0, 0, 0, 0, 0},
    {0}
};

// Convert wide character string to UTF-8 for LVGL display
static void wchar_to_utf8(const wchar_t *wstr, char *utf8_str, size_t utf8_size) {
    if (!wstr || !utf8_str || utf8_size == 0) return;
    
    size_t len = wcslen(wstr);
    if (len == 0) {
        utf8_str[0] = '\0';
        return;
    }
    
    // Convert using wcstombs
    size_t result = wcstombs(utf8_str, wstr, utf8_size - 1);
    if (result == (size_t)-1) {
        utf8_str[0] = '\0';
    } else {
        utf8_str[result] = '\0';
    }
}

// Update the Korean display
static void update_korean_display(void) {
    if (!korean_display_label || !korean_input_label) return;
    
    // Update input buffer display
    lv_label_set_text(korean_input_label, korean_input_buffer);
    
    // Convert wide char output to UTF-8 and update display
    char utf8_output[KOREAN_DISPLAY_MAX * 4]; // UTF-8 can use up to 4 bytes per character
    wchar_to_utf8(korean_output_buffer, utf8_output, sizeof(utf8_output));
    lv_label_set_text(korean_display_label, utf8_output);
}

// Korean key event callback
void korean_key_event_cb(lv_event_t *e) {
    lv_obj_t *btn = lv_event_get_target(e);
    const char *key_char = (const char *)lv_event_get_user_data(e);
    
    if (key_char && *key_char) {
        // Process the input through Korean IME GUI function
        process_input_gui(korean_input_buffer, &korean_input_len, korean_output_buffer, *key_char);
        update_korean_display();
    }
}

// Clear button callback
void korean_clear_cb(lv_event_t *e) {
    korean_input_buffer[0] = '\0';
    korean_output_buffer[0] = L'\0';
    korean_input_len = 0;
    update_korean_display();
}

// Backspace button callback
void korean_backspace_cb(lv_event_t *e) {
    process_input_gui(korean_input_buffer, &korean_input_len, korean_output_buffer, 0x7f);
    update_korean_display();
}

// Space button callback
void korean_space_cb(lv_event_t *e) {
    process_input_gui(korean_input_buffer, &korean_input_len, korean_output_buffer, ' ');
    update_korean_display();
}

// Enter button callback
void korean_enter_cb(lv_event_t *e) {
    process_input_gui(korean_input_buffer, &korean_input_len, korean_output_buffer, '\n');
    update_korean_display();
}

// Create Korean input tab
void create_korean_input_tab(lv_obj_t *parent) {
    // Initialize locale for Korean
    setlocale(LC_ALL, "ko_KR.UTF-8");
    
    // Create title label
    lv_obj_t *title_label = lv_label_create(parent);
    lv_label_set_text(title_label, "한글 입력기 (Korean Input)");
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 5);
    
    // Create input buffer display (raw keystrokes)
    lv_obj_t *input_title = lv_label_create(parent);
    lv_label_set_text(input_title, "Input:");
    lv_obj_align(input_title, LV_ALIGN_TOP_LEFT, 10, 30);
    
    korean_input_label = lv_label_create(parent);
    lv_label_set_text(korean_input_label, "");
    lv_obj_set_style_bg_color(korean_input_label, lv_color_hex(0xF0F0F0), 0);
    lv_obj_set_style_bg_opa(korean_input_label, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(korean_input_label, 5, 0);
    lv_obj_set_size(korean_input_label, 260, 25);
    lv_obj_align(korean_input_label, LV_ALIGN_TOP_LEFT, 50, 30);
    
    // Create output display (composed Korean characters)
    lv_obj_t *output_title = lv_label_create(parent);
    lv_label_set_text(output_title, "Output:");
    lv_obj_align(output_title, LV_ALIGN_TOP_LEFT, 10, 60);
    
    korean_display_label = lv_label_create(parent);
    lv_label_set_text(korean_display_label, "");
    lv_obj_set_style_bg_color(korean_display_label, lv_color_hex(0xE0E0E0), 0);
    lv_obj_set_style_bg_opa(korean_display_label, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(korean_display_label, 8, 0);
    lv_obj_set_size(korean_display_label, 260, 35);
    lv_obj_align(korean_display_label, LV_ALIGN_TOP_LEFT, 50, 60);
    
    // Create Korean QWERTY keyboard
    static char key_chars[26]; // Static storage for key characters
    int key_index = 0;
    
    int btn_width = 25;
    int btn_height = 30;
    int start_x = 20;
    int start_y = 110;
    int spacing = 3;
    
    // First row: q w e r t y u i o p
    for (int i = 0; i < 10; i++) {
        if (korean_qwerty_layout[0][i] == 0) break;
        
        lv_obj_t *btn = lv_btn_create(parent);
        lv_obj_set_size(btn, btn_width, btn_height);
        lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 
                    start_x + i * (btn_width + spacing), start_y);
        
        key_chars[key_index] = korean_qwerty_layout[0][i];
        
        lv_obj_t *btn_label = lv_label_create(btn);
        char label_text[2] = {korean_qwerty_layout[0][i], '\0'};
        lv_label_set_text(btn_label, label_text);
        lv_obj_center(btn_label);
        
        lv_obj_add_event_cb(btn, korean_key_event_cb, LV_EVENT_CLICKED, &key_chars[key_index]);
        key_index++;
    }
    
    // Second row: a s d f g h j k l
    for (int i = 0; i < 9; i++) {
        if (korean_qwerty_layout[1][i] == 0) break;
        
        lv_obj_t *btn = lv_btn_create(parent);
        lv_obj_set_size(btn, btn_width, btn_height);
        lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 
                    start_x + 15 + i * (btn_width + spacing), start_y + btn_height + spacing);
        
        key_chars[key_index] = korean_qwerty_layout[1][i];
        
        lv_obj_t *btn_label = lv_label_create(btn);
        char label_text[2] = {korean_qwerty_layout[1][i], '\0'};
        lv_label_set_text(btn_label, label_text);
        lv_obj_center(btn_label);
        
        lv_obj_add_event_cb(btn, korean_key_event_cb, LV_EVENT_CLICKED, &key_chars[key_index]);
        key_index++;
    }
    
    // Third row: z x c v b n m
    for (int i = 0; i < 7; i++) {
        if (korean_qwerty_layout[2][i] == 0) break;
        
        lv_obj_t *btn = lv_btn_create(parent);
        lv_obj_set_size(btn, btn_width, btn_height);
        lv_obj_align(btn, LV_ALIGN_TOP_LEFT, 
                    start_x + 30 + i * (btn_width + spacing), start_y + 2 * (btn_height + spacing));
        
        key_chars[key_index] = korean_qwerty_layout[2][i];
        
        lv_obj_t *btn_label = lv_label_create(btn);
        char label_text[2] = {korean_qwerty_layout[2][i], '\0'};
        lv_label_set_text(btn_label, label_text);
        lv_obj_center(btn_label);
        
        lv_obj_add_event_cb(btn, korean_key_event_cb, LV_EVENT_CLICKED, &key_chars[key_index]);
        key_index++;
    }
    
    // Function buttons
    int func_btn_y = start_y + 3 * (btn_height + spacing) + 10;
    
    // Space bar
    lv_obj_t *space_btn = lv_btn_create(parent);
    lv_obj_set_size(space_btn, 120, btn_height);
    lv_obj_align(space_btn, LV_ALIGN_TOP_LEFT, start_x + 50, func_btn_y);
    
    lv_obj_t *space_label = lv_label_create(space_btn);
    lv_label_set_text(space_label, "Space");
    lv_obj_center(space_label);
    lv_obj_add_event_cb(space_btn, korean_space_cb, LV_EVENT_CLICKED, NULL);
    
    // Backspace button
    lv_obj_t *backspace_btn = lv_btn_create(parent);
    lv_obj_set_size(backspace_btn, 50, btn_height);
    lv_obj_align(backspace_btn, LV_ALIGN_TOP_LEFT, start_x + 180, func_btn_y);
    
    lv_obj_t *backspace_label = lv_label_create(backspace_btn);
    lv_label_set_text(backspace_label, "⌫");
    lv_obj_center(backspace_label);
    lv_obj_add_event_cb(backspace_btn, korean_backspace_cb, LV_EVENT_CLICKED, NULL);
    
    // Clear button
    lv_obj_t *clear_btn = lv_btn_create(parent);
    lv_obj_set_size(clear_btn, 40, btn_height);
    lv_obj_align(clear_btn, LV_ALIGN_TOP_LEFT, start_x, func_btn_y);
    
    lv_obj_t *clear_label = lv_label_create(clear_btn);
    lv_label_set_text(clear_label, "CLR");
    lv_obj_center(clear_label);
    lv_obj_add_event_cb(clear_btn, korean_clear_cb, LV_EVENT_CLICKED, NULL);
    
    // Enter button
    lv_obj_t *enter_btn = lv_btn_create(parent);
    lv_obj_set_size(enter_btn, 50, btn_height);
    lv_obj_align(enter_btn, LV_ALIGN_TOP_LEFT, start_x + 240, func_btn_y);
    
    lv_obj_t *enter_label = lv_label_create(enter_btn);
    lv_label_set_text(enter_label, "Enter");
    lv_obj_center(enter_label);
    lv_obj_add_event_cb(enter_btn, korean_enter_cb, LV_EVENT_CLICKED, NULL);
}
