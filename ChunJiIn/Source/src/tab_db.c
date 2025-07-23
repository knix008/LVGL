#include "tab_db.h"
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

// Create Database tab
void create_db_tab(lv_obj_t * parent) {
    // Tab 1: Database operations
    lv_obj_t * label1 = lv_label_create(parent);
    lv_label_set_text(label1, "Database Operations");
    lv_obj_align(label1, LV_ALIGN_TOP_MID, 0, 10);
    
    // Create database button
    lv_obj_t * btn1 = lv_btn_create(parent);
    lv_obj_set_size(btn1, 120, 40);
    lv_obj_align(btn1, LV_ALIGN_CENTER, -70, 20);
    
    lv_obj_t * btn1_label = lv_label_create(btn1);
    lv_label_set_text(btn1_label, "Create DB");
    
    // Query database button
    lv_obj_t * btn2 = lv_btn_create(parent);
    lv_obj_set_size(btn2, 120, 40);
    lv_obj_align(btn2, LV_ALIGN_CENTER, 70, 20);
    
    lv_obj_t * btn2_label = lv_label_create(btn2);
    lv_label_set_text(btn2_label, "Query DB");
    
    // Result label for database operations
    lv_obj_t * result_label = lv_label_create(parent);
    lv_label_set_text(result_label, "Click buttons to perform database operations");
    lv_obj_align(result_label, LV_ALIGN_CENTER, 0, 80);
    lv_obj_set_width(result_label, 280);
    lv_label_set_long_mode(result_label, LV_LABEL_LONG_WRAP);
    
    // Assign event callbacks
    lv_obj_add_event_cb(btn1, btn_event_cb, LV_EVENT_CLICKED, result_label);
    lv_obj_add_event_cb(btn2, btn2_event_cb, LV_EVENT_CLICKED, result_label);
} 