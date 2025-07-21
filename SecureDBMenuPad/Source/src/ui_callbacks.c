#include "ui_callbacks.h"
#include "ui_keypad.h"
#include "sqlite_test.h"
#include <string.h>
#include <stdio.h>

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

// Button event callback for creating database
void btn_event_cb(lv_event_t * e) {
    lv_obj_t * label = lv_event_get_user_data(e);
    
    char result_buffer[256];
    int ret = sqlite_create_table_and_insert_data(result_buffer, sizeof(result_buffer));
    
    if (ret == 0) {
        lv_label_set_text(label, result_buffer);
    } else {
        lv_label_set_text(label, "Database operation failed!");
    }
}

// Button event callback for querying database
void btn2_event_cb(lv_event_t * e) {
    lv_obj_t * label = lv_event_get_user_data(e);
    
    char result_buffer[256];
    int ret = sqlite_query_and_display_data(result_buffer, sizeof(result_buffer));
    
    if (ret == 0) {
        lv_label_set_text(label, result_buffer);
    } else {
        lv_label_set_text(label, "Database query failed!");
    }
}

// Tab event callback
void tab_event_cb(lv_event_t * e) {
    lv_obj_t * tabview = lv_event_get_target(e);
    uint32_t active_tab = lv_tabview_get_tab_act(tabview);
    
    printf("Active tab changed to: %lu\n", (unsigned long)active_tab);
}

// Slider event callback
void slider_event_cb(lv_event_t * e) {
    lv_obj_t * slider = lv_event_get_target(e);
    lv_obj_t * label = lv_event_get_user_data(e);
    int32_t value = lv_slider_get_value(slider);
    char buf[32];
    lv_snprintf(buf, sizeof(buf), "Brightness: %ld%%", (long)value);
    lv_label_set_text(label, buf);
}

// Switch event callback
void switch_event_cb(lv_event_t * e) {
    lv_obj_t * sw = lv_event_get_target(e);
    lv_obj_t * label = lv_event_get_user_data(e);
    bool checked = lv_obj_has_state(sw, LV_STATE_CHECKED);
    lv_label_set_text(label, checked ? "Auto-save: ON" : "Auto-save: OFF");
} 