#include "tab_settings.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "lvgl.h"
#include "ui_components.h"

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
    
    // Create the JPG image object for settings info (left side)
    lv_obj_t * jpg_img = lv_image_create(parent);
    lv_image_set_src(jpg_img, "A:../assets/setting.jpg");
    lv_obj_set_size(jpg_img, 64, 64);
    lv_obj_align(jpg_img, LV_ALIGN_TOP_MID, -40, 50); // Position to the left
    
    // Check JPG loading status
    const void* jpg_src = lv_image_get_src(jpg_img);
    if (jpg_src) {
        printf("Settings tab: JPG loaded successfully: A:../assets/setting.jpg\n");
        printf("Settings tab: JPG source pointer: %p\n", jpg_src);
    } else {
        printf("Settings tab: JPG failed to load: A:../assets/setting.jpg\n");
        printf("Settings tab: JPG source is NULL\n");
    }

    // Create the GIF image object for settings info (right side)
    lv_obj_t * gif_img = lv_gif_create(parent);
    lv_gif_set_src(gif_img, "A:../assets/bulb.gif");
    lv_obj_set_size(gif_img, 64, 64);
    lv_obj_align(gif_img, LV_ALIGN_TOP_MID, 40, 50); // Position to the right
    
    // Force a timer handler to process the image loading
    lv_timer_handler();
    
    // Check GIF loading status
    const void* gif_src = lv_image_get_src(gif_img);
    if (gif_src) {
        printf("Settings tab: GIF loaded successfully: A:../assets/setting_simple.gif\n");
        printf("Settings tab: GIF source pointer: %p\n", gif_src);
    } else {
        printf("Settings tab: GIF failed to load: A:../assets/setting_simple.gif\n");
        printf("Settings tab: GIF source is NULL\n");
    }
    
    // Create a slider
    lv_obj_t * slider = lv_slider_create(parent);
    lv_obj_set_size(slider, 200, 10);
    lv_obj_align(slider, LV_ALIGN_CENTER, 0, 20);
    lv_slider_set_range(slider, 0, 100);
    lv_slider_set_value(slider, 50, LV_ANIM_OFF);
    
    // Slider label
    lv_obj_t * slider_label = lv_label_create(parent);
    lv_label_set_text(slider_label, "Brightness: 50%");
    lv_obj_align(slider_label, LV_ALIGN_CENTER, 0, 50);
    
    // Slider event callback
    lv_obj_add_event_cb(slider, slider_event_cb, LV_EVENT_VALUE_CHANGED, slider_label);
    
    // Create a switch
    lv_obj_t * sw = lv_switch_create(parent);
    lv_obj_align(sw, LV_ALIGN_CENTER, 0, 100);
    
    lv_obj_t * sw_label = lv_label_create(parent);
    lv_label_set_text(sw_label, "Auto-save: OFF");
    lv_obj_align(sw_label, LV_ALIGN_CENTER, 0, 130);
    
    // Switch event callback
    lv_obj_add_event_cb(sw, switch_event_cb, LV_EVENT_VALUE_CHANGED, sw_label);
} 