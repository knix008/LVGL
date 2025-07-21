#include "ui_info.h"

// Create info display UI
void create_info_ui(lv_obj_t * parent) {
    // Info title
    lv_obj_t * label3 = lv_label_create(parent);
    lv_label_set_text(label3, "Application Info");
    lv_obj_align(label3, LV_ALIGN_TOP_MID, 0, 10);
    
    // Info content
    lv_obj_t * info_label = lv_label_create(parent);
    lv_label_set_text(info_label, 
        "LVGL SQLCipher Demo\n\n"
        "Version: 1.0.0\n"
        "Built with LVGL + SQLCipher\n\n"
        "Features:\n"
        "• Encrypted database operations\n"
        "• Number keypad interface\n"
        "• Tabbed interface\n"
        "• Settings panel\n"
        "• Modern UI design\n"
        "• AES-256 encryption");
    lv_obj_align(info_label, LV_ALIGN_CENTER, 0, 20);
    lv_obj_set_width(info_label, 280);
    lv_label_set_long_mode(info_label, LV_LABEL_LONG_WRAP);
} 