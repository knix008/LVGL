#include "ui_components.h"
#include "sqlite_test.h"
#include <string.h>

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

// UI initialization and setup
void lv_example_simple_gui(void) {
    // Create a screen
    lv_obj_t * scr = lv_scr_act();

    // Create a label
    lv_obj_t * label = lv_label_create(scr);
    lv_label_set_text(label, "SQLite Test with LVGL");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 20);

    // Create first button
    lv_obj_t * btn = lv_btn_create(scr);
    lv_obj_align(btn, LV_ALIGN_CENTER, -60, 0);

    // Create a label on the first button
    lv_obj_t * btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Create DB");

    // Create second button
    lv_obj_t * btn2 = lv_btn_create(scr);
    lv_obj_align(btn2, LV_ALIGN_CENTER, 60, 0);

    // Create a label on the second button
    lv_obj_t * btn2_label = lv_label_create(btn2);
    lv_label_set_text(btn2_label, "Query DB");

    // Assign event callbacks to the buttons, pass label as user data
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, label);
    lv_obj_add_event_cb(btn2, btn2_event_cb, LV_EVENT_CLICKED, label);
} 