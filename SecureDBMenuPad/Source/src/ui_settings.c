#include "ui_settings.h"

// Create settings UI
void create_settings_ui(lv_obj_t * parent) {
    // Settings title
    lv_obj_t * label2 = lv_label_create(parent);
    lv_label_set_text(label2, "Settings");
    lv_obj_align(label2, LV_ALIGN_TOP_MID, 0, 10);
    
    // Create a slider
    lv_obj_t * slider = lv_slider_create(parent);
    lv_obj_set_size(slider, 200, 10);
    lv_obj_align(slider, LV_ALIGN_CENTER, 0, 0);
    lv_slider_set_range(slider, 0, 100);
    lv_slider_set_value(slider, 50, LV_ANIM_OFF);
    
    // Slider label
    lv_obj_t * slider_label = lv_label_create(parent);
    lv_label_set_text(slider_label, "Brightness: 50%");
    lv_obj_align(slider_label, LV_ALIGN_CENTER, 0, 30);
    
    // Slider event callback
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, slider_label);
    
    // Create a switch
    lv_obj_t * sw = lv_switch_create(parent);
    lv_obj_align(sw, LV_ALIGN_CENTER, 0, 80);
    
    lv_obj_t * sw_label = lv_label_create(parent);
    lv_label_set_text(sw_label, "Auto-save: OFF");
    lv_obj_align(sw_label, LV_ALIGN_CENTER, 0, 110);
    
    // Switch event callback
    lv_obj_add_event_cb(sw, switch_event_cb, LV_EVENT_VALUE_CHANGED, sw_label);
} 