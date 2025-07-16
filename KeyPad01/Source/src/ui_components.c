#include "ui_components.h"
#include "sqlite_test.h"
#include <string.h>
#include <stdio.h>

// Global variables for number keypad
char keypad_buffer[32] = "";
lv_obj_t * keypad_display_label = NULL;

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

// UI initialization and setup with tab menu
void lv_example_tab_menu(void) {
    // Create a screen
    lv_obj_t * scr = lv_scr_act();
    
    // Create tabview (크기 원래대로)
    lv_obj_t * tabview = lv_tabview_create(scr);
    lv_obj_set_size(tabview, 300, 400);  // 전체 탭뷰 크기 유지
    lv_obj_align(tabview, LV_ALIGN_TOP_MID, 0, 0); // 최상단으로 이동
    
    // 탭 바(탭 버튼 영역) 높이만 줄임
    lv_tabview_set_tab_bar_size(tabview, 32); // 예시: 32픽셀로 설정
    
    // Add event callback for tab changes
    lv_obj_add_event_cb(tabview, tab_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Create tabs
    lv_obj_t * tab1 = lv_tabview_add_tab(tabview, "Database");
    lv_obj_t * tab2 = lv_tabview_add_tab(tabview, "Settings");
    lv_obj_t * tab3 = lv_tabview_add_tab(tabview, "Info");
    lv_obj_t * tab4 = lv_tabview_add_tab(tabview, "Keypad"); // Added Keypad tab
    
    // Tab 1: Database operations
    lv_obj_t * label1 = lv_label_create(tab1);
    lv_label_set_text(label1, "Database Operations");
    lv_obj_align(label1, LV_ALIGN_TOP_MID, 0, 10);
    
    // Create database button
    lv_obj_t * btn1 = lv_btn_create(tab1);
    lv_obj_set_size(btn1, 120, 40);
    lv_obj_align(btn1, LV_ALIGN_CENTER, -70, 20);
    
    lv_obj_t * btn1_label = lv_label_create(btn1);
    lv_label_set_text(btn1_label, "Create DB");
    
    // Query database button
    lv_obj_t * btn2 = lv_btn_create(tab1);
    lv_obj_set_size(btn2, 120, 40);
    lv_obj_align(btn2, LV_ALIGN_CENTER, 70, 20);
    
    lv_obj_t * btn2_label = lv_label_create(btn2);
    lv_label_set_text(btn2_label, "Query DB");
    
    // Result label for database operations
    lv_obj_t * result_label = lv_label_create(tab1);
    lv_label_set_text(result_label, "Click buttons to perform database operations");
    lv_obj_align(result_label, LV_ALIGN_CENTER, 0, 80);
    lv_obj_set_width(result_label, 280);
    lv_label_set_long_mode(result_label, LV_LABEL_LONG_WRAP);
    
    // Assign event callbacks
    lv_obj_add_event_cb(btn1, btn_event_cb, LV_EVENT_CLICKED, result_label);
    lv_obj_add_event_cb(btn2, btn2_event_cb, LV_EVENT_CLICKED, result_label);
    
    // Tab 2: Settings
    lv_obj_t * label2 = lv_label_create(tab2);
    lv_label_set_text(label2, "Settings");
    lv_obj_align(label2, LV_ALIGN_TOP_MID, 0, 10);
    
    // Create a slider
    lv_obj_t * slider = lv_slider_create(tab2);
    lv_obj_set_size(slider, 200, 10);
    lv_obj_align(slider, LV_ALIGN_CENTER, 0, 0);
    lv_slider_set_range(slider, 0, 100);
    lv_slider_set_value(slider, 50, LV_ANIM_OFF);
    
    // Slider label
    lv_obj_t * slider_label = lv_label_create(tab2);
    lv_label_set_text(slider_label, "Brightness: 50%");
    lv_obj_align(slider_label, LV_ALIGN_CENTER, 0, 30);
    
    // Slider event callback
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, slider_label);
    
    // Create a switch
    lv_obj_t * sw = lv_switch_create(tab2);
    lv_obj_align(sw, LV_ALIGN_CENTER, 0, 80);
    
    lv_obj_t * sw_label = lv_label_create(tab2);
    lv_label_set_text(sw_label, "Auto-save: OFF");
    lv_obj_align(sw_label, LV_ALIGN_CENTER, 0, 110);
    
    // Switch event callback
    lv_obj_add_event_cb(sw, switch_event_cb, LV_EVENT_VALUE_CHANGED, sw_label);
    
    // Tab 3: Info
    lv_obj_t * label3 = lv_label_create(tab3);
    lv_label_set_text(label3, "Application Info");
    lv_obj_align(label3, LV_ALIGN_TOP_MID, 0, 10);
    
    lv_obj_t * info_label = lv_label_create(tab3);
    lv_label_set_text(info_label, 
        "LVGL Number Keypad Demo\n\n"
        "Version: 1.0.0\n"
        "Built with LVGL\n\n"
        "Features:\n"
        "• Number keypad interface\n"
        "• Database operations\n"
        "• Tabbed interface\n"
        "• Settings panel\n"
        "• Modern UI design");
    lv_obj_align(info_label, LV_ALIGN_CENTER, 0, 20);
    lv_obj_set_width(info_label, 280);
    lv_label_set_long_mode(info_label, LV_LABEL_LONG_WRAP);

    // Tab 4: Number Keypad - Centered layout
    lv_obj_t * keypad_label = lv_label_create(tab4);
    lv_label_set_text(keypad_label, "Number Keypad");
    lv_obj_align(keypad_label, LV_ALIGN_CENTER, 0, -120); // Centered title

    // Display area for entered numbers - centered above the keypad
    lv_obj_t * keypad_display = lv_label_create(tab4);
    lv_label_set_text(keypad_display, "0");
    lv_obj_set_style_bg_color(keypad_display, lv_color_hex(0xE0E0E0), 0);
    lv_obj_set_style_bg_opa(keypad_display, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(keypad_display, 10, 0);
    lv_obj_set_size(keypad_display, 260, 40);
    lv_obj_align(keypad_display, LV_ALIGN_CENTER, 0, -60); // Centered display above keypad
    keypad_display_label = keypad_display;

    // Create number buttons in a 3x4 grid layout
    const char * number_labels[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"};
    int btn_width = 70;
    int btn_height = 50;
    int gap = 10;

    for (int i = 0; i < 10; i++) {
        lv_obj_t * btn = lv_btn_create(tab4);
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
        
        // Calculate relative position from center for perfect centering
        int offset_x = (col - 1) * (btn_width + gap); // Center column is 1
        int offset_y = (row - 1) * (btn_height + gap) - (btn_height + gap); // Center the grid vertically
        
        lv_obj_align(btn, LV_ALIGN_CENTER, offset_x, offset_y); // Perfect center alignment

        lv_obj_t * btn_label = lv_label_create(btn);
        lv_label_set_text(btn_label, number_labels[i]);
        lv_obj_center(btn_label);

        lv_obj_add_event_cb(btn, keypad_number_cb, LV_EVENT_CLICKED, NULL);
    }

    // Function buttons - positioned below the number grid with relative coordinates
    lv_obj_t * clear_btn = lv_btn_create(tab4);
    lv_obj_set_size(clear_btn, btn_width, btn_height);
    lv_obj_align(clear_btn, LV_ALIGN_CENTER, -btn_width - gap, 2 * (btn_height + gap));
    
    lv_obj_t * clear_label = lv_label_create(clear_btn);
    lv_label_set_text(clear_label, "C");
    lv_obj_center(clear_label);
    lv_obj_add_event_cb(clear_btn, keypad_clear_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * backspace_btn = lv_btn_create(tab4);
    lv_obj_set_size(backspace_btn, btn_width, btn_height);
    lv_obj_align(backspace_btn, LV_ALIGN_CENTER, 0, 2 * (btn_height + gap));
    
    lv_obj_t * backspace_label = lv_label_create(backspace_btn);
    lv_label_set_text(backspace_label, "⌫");
    lv_obj_center(backspace_label);
    lv_obj_add_event_cb(backspace_btn, keypad_backspace_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * enter_btn = lv_btn_create(tab4);
    lv_obj_set_size(enter_btn, btn_width, btn_height);
    lv_obj_align(enter_btn, LV_ALIGN_CENTER, btn_width + gap, 2 * (btn_height + gap));
    
    lv_obj_t * enter_label = lv_label_create(enter_btn);
    lv_label_set_text(enter_label, "Enter");
    lv_obj_center(enter_label);
    lv_obj_add_event_cb(enter_btn, keypad_enter_cb, LV_EVENT_CLICKED, NULL);
}

// UI initialization and setup (original simple GUI)
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