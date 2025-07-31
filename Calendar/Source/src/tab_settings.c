#include "tab_settings.h"
#include <stdio.h>

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

// Create Settings tab
void create_settings_tab(lv_obj_t * parent) {
    // Tab 2: Settings
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