#include "tab_info.h"

// Create Info tab
void create_info_tab(lv_obj_t * parent) {
    // Tab 3: Info
    lv_obj_t * label3 = lv_label_create(parent);
    lv_label_set_text(label3, "Application Info");
    lv_obj_align(label3, LV_ALIGN_TOP_MID, 0, 10);
    
    lv_obj_t * info_label = lv_label_create(parent);
    lv_label_set_text(info_label, 
        "LVGL Number Keypad Demo\n\n"
        "Version: 1.0.0\n"
        "Built with LVGL\n\n"
        "Features:\n"
        "- Number keypad interface\n"
        "- Database operations\n"
        "- Tabbed interface\n"
        "- Settings panel\n"
        "- Modern UI design\n"
        "- Korean font support");
    lv_obj_align(info_label, LV_ALIGN_CENTER, 0, 20);
    lv_obj_set_width(info_label, 280);
    lv_label_set_long_mode(info_label, LV_LABEL_LONG_WRAP);
} 