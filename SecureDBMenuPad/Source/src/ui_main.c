#include "ui_main.h"
#include "ui_keypad.h"

// UI initialization and setup with tab menu
void lv_example_tab_menu(void) {
    // Create a screen
    lv_obj_t * scr = lv_scr_act();
    
    // Create tabview
    lv_obj_t * tabview = lv_tabview_create(scr);
    lv_obj_set_size(tabview, 300, 400);
    lv_obj_center(tabview);
    
    // Set tab bar height
    lv_tabview_set_tab_bar_size(tabview, 32);
    
    // Add event callback for tab changes
    lv_obj_add_event_cb(tabview, tab_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Create tabs
    lv_obj_t * tab1 = lv_tabview_add_tab(tabview, "Database");
    lv_obj_t * tab2 = lv_tabview_add_tab(tabview, "Settings");
    lv_obj_t * tab3 = lv_tabview_add_tab(tabview, "Info");
    lv_obj_t * tab4 = lv_tabview_add_tab(tabview, "Keypad");
    
    // Initialize keypad buffer
    init_keypad_buffer();
    
    // Create UI components for each tab
    create_database_ui(tab1);
    create_settings_ui(tab2);
    create_info_ui(tab3);
    create_keypad_ui(tab4);
}

// UI initialization and setup (original simple GUI)
void lv_example_simple_gui(void) {
    // Create a screen
    lv_obj_t * scr = lv_scr_act();

    // Create a label
    lv_obj_t * label = lv_label_create(scr);
    lv_label_set_text(label, "SQLCipher Test with LVGL");
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