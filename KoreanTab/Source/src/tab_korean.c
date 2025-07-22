#include "tab_korean.h"
#include "nanum_gothic_16.h"

// Create Korean tab
void create_korean_tab(lv_obj_t * parent) {
    // Tab 5: Korean Tab
    lv_obj_t * label5 = lv_label_create(parent);
    lv_label_set_text(label5, "KOR Tab");
    lv_obj_align(label5, LV_ALIGN_TOP_MID, 0, 10);
    
    // Set Korean font for the header
    lv_obj_set_style_text_font(label5, &nanum_gothic_16, 0);

    lv_obj_t * korean_content = lv_label_create(parent);
    lv_label_set_text(korean_content, 
        "Korean Font Test\n\n"
        "This is the Korean tab.\n"
        "Testing Korean font support\n"
        "in LVGL.\n\n"
        "If Korean fonts work,\n"
        "you should see Korean text below:");
    lv_obj_align(korean_content, LV_ALIGN_CENTER, 0, 20);
    lv_obj_set_width(korean_content, 280);
    lv_label_set_long_mode(korean_content, LV_LABEL_LONG_WRAP);
    
    // Korean text test
    lv_obj_t * korean_test = lv_label_create(parent);
    lv_label_set_text(korean_test, 
        "Hello\n"
        "안녕하세요\n"
        "한국어");
    lv_obj_align(korean_test, LV_ALIGN_CENTER, 0, 80);
    lv_obj_set_width(korean_test, 280);
    lv_label_set_long_mode(korean_test, LV_LABEL_LONG_WRAP);
    
    // Set Korean font for the Korean text
    lv_obj_set_style_text_font(korean_test, &nanum_gothic_16, 0);

    // Korean button
    lv_obj_t * korean_btn = lv_btn_create(parent);
    lv_obj_set_size(korean_btn, 120, 40);
    lv_obj_align(korean_btn, LV_ALIGN_CENTER, 0, 140);
    
    lv_obj_t * korean_btn_label = lv_label_create(korean_btn);
    lv_label_set_text(korean_btn_label, "Click Me!");
    lv_obj_center(korean_btn_label);
    
    // Set Korean font for the button
    lv_obj_set_style_text_font(korean_btn, &nanum_gothic_16, 0);
} 