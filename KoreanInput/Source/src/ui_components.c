#include "ui_components.h"
#include "tab_db.h"
#include "tab_settings.h"
#include "tab_info.h"
#include "tab_number.h"
#include "tab_korean.h"
#include "ui_callbacks.h"
#include <stdio.h>

// UI initialization and setup with tab menu
void lv_example_tab_menu(void) {
    // Create a screen
    lv_obj_t * scr = lv_scr_act();
    
    // Create tabview (크기 원래대로)
    lv_obj_t * tabview = lv_tabview_create(scr);
    lv_obj_set_size(tabview, 300, 400);  // 전체 탭뷰 크기 유지
    lv_obj_center(tabview);
    
    // 탭 바(탭 버튼 영역) 높이만 줄임
    lv_tabview_set_tab_bar_size(tabview, 32); // 예시: 32픽셀로 설정
    
    // Add event callback for tab changes
    lv_obj_add_event_cb(tabview, tab_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Create tabs
    lv_obj_t * tab1 = lv_tabview_add_tab(tabview, "DB");
    lv_obj_t * tab2 = lv_tabview_add_tab(tabview, "Settings");
    lv_obj_t * tab3 = lv_tabview_add_tab(tabview, "Info");
    lv_obj_t * tab4 = lv_tabview_add_tab(tabview, "NUM"); // Changed from Keypad to Number
    lv_obj_t * tab5 = lv_tabview_add_tab(tabview, "KOR"); // Added new tab
    
    // Create tab contents using separate functions
    create_db_tab(tab1);
    create_settings_tab(tab2);
    create_info_tab(tab3);
    create_number_tab(tab4);
    create_korean_tab(tab5);
}