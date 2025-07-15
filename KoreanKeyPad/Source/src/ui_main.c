#include "ui_main.h"
#include "ui_keypad.h"
#include "ui_korean_keypad.h"
#include <stdio.h>

// UI initialization and setup with tab menu
void lv_example_tab_menu(void) {
    // Create a screen
    lv_obj_t * scr = lv_scr_act();
    printf("Screen created: %p\n", (void*)scr);
    
    // Create tabview
    lv_obj_t * tabview = lv_tabview_create(scr);
    if (tabview == NULL) {
        printf("Error: Failed to create tabview\n");
        return;
    }
    printf("Tabview created: %p\n", (void*)tabview);
    
    lv_obj_set_size(tabview, 320, 480);
    lv_obj_center(tabview);
    
    // Set tab bar height
    lv_tabview_set_tab_bar_size(tabview, 32);
    
    // Add event callback for tab changes
    lv_obj_add_event_cb(tabview, tab_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Create tabs
    printf("Creating tabs...\n");
    lv_obj_t * tab1 = lv_tabview_add_tab(tabview, "Database");
    printf("Database tab: %p\n", (void*)tab1);
    
    lv_obj_t * tab2 = lv_tabview_add_tab(tabview, "Settings");
    printf("Settings tab: %p\n", (void*)tab2);
    
    lv_obj_t * tab3 = lv_tabview_add_tab(tabview, "Info");
    printf("Info tab: %p\n", (void*)tab3);
    
    lv_obj_t * tab4 = lv_tabview_add_tab(tabview, "Keypad");
    printf("Keypad tab: %p\n", (void*)tab4);
    
    lv_obj_t * tab5 = lv_tabview_add_tab(tabview, "한글");
    printf("Korean tab: %p\n", (void*)tab5);
    
    // Initialize keypad buffers
    printf("Initializing keypad buffers...\n");
    init_keypad_buffer();
    init_korean_keypad();
    
    // Create UI components for each tab
    printf("Creating UI components...\n");
    create_database_ui(tab1);
    printf("Database UI created\n");
    
    create_settings_ui(tab2);
    printf("Settings UI created\n");
    
    create_info_ui(tab3);
    printf("Info UI created\n");
    
    create_keypad_ui(tab4);
    printf("Keypad UI created\n");
    
    printf("Creating Korean keypad UI...\n");
    // Re-enable Korean keypad with Unicode support
    create_korean_keypad_ui(tab5);
    printf("Korean keypad UI creation completed\n");
    
    // Add a simple Unicode test label to verify Korean character display
    lv_obj_t * unicode_test = lv_label_create(scr);
    lv_label_set_text(unicode_test, "Unicode Test: 한글 테스트");
    // Use Korean font if available (will be set by Korean keypad initialization)
    extern lv_font_t * korean_font;
    if (korean_font != NULL) {
        lv_obj_set_style_text_font(unicode_test, korean_font, 0);
    }
    lv_obj_align(unicode_test, LV_ALIGN_BOTTOM_MID, 0, -10);
    printf("Unicode test label created\n");
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