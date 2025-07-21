#include "ui_database.h"

// Create database operations UI
void create_database_ui(lv_obj_t * parent) {
    // Database operations title
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