#include "tab_settings.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "lvgl.h"
#include "ui_components.h"

// Callback functions for widgets
static void brightness_slider_cb(lv_event_t * e) {
    lv_obj_t * slider = lv_event_get_target(e);
    int32_t value = lv_slider_get_value(slider);
    printf("Settings: Brightness slider value: %ld\n", (long)value);
}

static void volume_slider_cb(lv_event_t * e) {
    lv_obj_t * slider = lv_event_get_target(e);
    int32_t value = lv_slider_get_value(slider);
    printf("Settings: Volume slider value: %ld\n", (long)value);
}

static void wifi_switch_cb(lv_event_t * e) {
    lv_obj_t * sw = lv_event_get_target(e);
    bool checked = lv_obj_has_state(sw, LV_STATE_CHECKED);
    printf("Settings: WiFi switch %s\n", checked ? "ON" : "OFF");
}

static void bluetooth_switch_cb(lv_event_t * e) {
    lv_obj_t * sw = lv_event_get_target(e);
    bool checked = lv_obj_has_state(sw, LV_STATE_CHECKED);
    printf("Settings: Bluetooth switch %s\n", checked ? "ON" : "OFF");
}

static void theme_dropdown_cb(lv_event_t * e) {
    lv_obj_t * dropdown = lv_event_get_target(e);
    uint16_t selected = lv_dropdown_get_selected(dropdown);
    printf("Settings: Theme dropdown selected: %d\n", selected);
}

static void language_dropdown_cb(lv_event_t * e) {
    lv_obj_t * dropdown = lv_event_get_target(e);
    uint16_t selected = lv_dropdown_get_selected(dropdown);
    printf("Settings: Language dropdown selected: %d\n", selected);
}

static void save_button_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("Settings: Save button clicked\n");
    }
}

static void reset_button_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("Settings: Reset button clicked\n");
    }
}

static void about_button_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("Settings: About button clicked\n");
    }
}

// Create Settings tab
void create_settings_tab(lv_obj_t * parent) {
    // Initialize decoders
    lv_tjpgd_init(); // Initialize JPG decoder
    
    // Create main title
    lv_obj_t * title = lv_label_create(parent);
    lv_label_set_text(title, "Settings Demo");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(0x0066CC), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    
    // Create image row container
    lv_obj_t * image_container = lv_obj_create(parent);
    lv_obj_set_size(image_container, 300, 80);
    lv_obj_align(image_container, LV_ALIGN_TOP_MID, 0, 30);
    lv_obj_set_style_pad_all(image_container, 5, 0);
    lv_obj_set_style_bg_color(image_container, lv_color_hex(0xF0F0F0), 0);
    lv_obj_set_style_border_width(image_container, 1, 0);
    lv_obj_set_style_border_color(image_container, lv_color_hex(0xCCCCCC), 0);
    
    // Create the JPG image object (center)
    lv_obj_t * jpg_img = lv_img_create(image_container);
    lv_img_set_src(jpg_img, "A:../assets/setting.jpg");
    lv_obj_set_size(jpg_img, 64, 64);
    lv_obj_align(jpg_img, LV_ALIGN_CENTER, 0, 0);
           
    // Create the GIF image object (left side)
    lv_obj_t * gif_img = lv_gif_create(image_container);
    lv_gif_set_src(gif_img, "A:../assets/bulb.gif");
    lv_obj_set_size(gif_img, 64, 64);
    lv_obj_align(gif_img, LV_ALIGN_CENTER, -80, 0);
    
    // Create the hourglass GIF image object (right side)
    lv_obj_t * hourglass_gif_img = lv_gif_create(image_container);
    lv_gif_set_src(hourglass_gif_img, "A:../assets/hourglass_small.gif");
    lv_obj_set_size(hourglass_gif_img, 64, 64);
    lv_obj_align(hourglass_gif_img, LV_ALIGN_CENTER, 80, 0);
    
    // Create sliders container
    lv_obj_t * sliders_container = lv_obj_create(parent);
    lv_obj_set_size(sliders_container, 280, 120);
    lv_obj_align(sliders_container, LV_ALIGN_TOP_MID, 0, 120);
    lv_obj_set_style_pad_all(sliders_container, 10, 0);
    lv_obj_set_style_bg_color(sliders_container, lv_color_hex(0xF8F8F8), 0);
    lv_obj_set_style_border_width(sliders_container, 1, 0);
    lv_obj_set_style_border_color(sliders_container, lv_color_hex(0xDDDDDD), 0);
    
    // Brightness slider
    lv_obj_t * brightness_label = lv_label_create(sliders_container);
    lv_label_set_text(brightness_label, "Brightness");
    lv_obj_align(brightness_label, LV_ALIGN_TOP_LEFT, 0, 0);
    
    lv_obj_t * brightness_slider = lv_slider_create(sliders_container);
    lv_obj_set_size(brightness_slider, 200, 15);
    lv_obj_align(brightness_slider, LV_ALIGN_TOP_LEFT, 0, 25);
    lv_slider_set_range(brightness_slider, 0, 100);
    lv_slider_set_value(brightness_slider, 75, LV_ANIM_ON);
    lv_obj_add_event_cb(brightness_slider, brightness_slider_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Volume slider
    lv_obj_t * volume_label = lv_label_create(sliders_container);
    lv_label_set_text(volume_label, "Volume");
    lv_obj_align(volume_label, LV_ALIGN_TOP_LEFT, 0, 50);
    
    lv_obj_t * volume_slider = lv_slider_create(sliders_container);
    lv_obj_set_size(volume_slider, 200, 15);
    lv_obj_align(volume_slider, LV_ALIGN_TOP_LEFT, 0, 75);
    lv_slider_set_range(volume_slider, 0, 100);
    lv_slider_set_value(volume_slider, 60, LV_ANIM_ON);
    lv_obj_add_event_cb(volume_slider, volume_slider_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Create switches container
    lv_obj_t * switches_container = lv_obj_create(parent);
    lv_obj_set_size(switches_container, 280, 80);
    lv_obj_align(switches_container, LV_ALIGN_TOP_MID, 0, 260);
    lv_obj_set_style_pad_all(switches_container, 10, 0);
    lv_obj_set_style_bg_color(switches_container, lv_color_hex(0xF8F8F8), 0);
    lv_obj_set_style_border_width(switches_container, 1, 0);
    lv_obj_set_style_border_color(switches_container, lv_color_hex(0xDDDDDD), 0);
    
    // WiFi switch
    lv_obj_t * wifi_label = lv_label_create(switches_container);
    lv_label_set_text(wifi_label, "WiFi");
    lv_obj_align(wifi_label, LV_ALIGN_TOP_LEFT, 0, 0);
    
    lv_obj_t * wifi_switch = lv_switch_create(switches_container);
    lv_obj_align(wifi_switch, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_add_state(wifi_switch, LV_STATE_CHECKED);
    lv_obj_add_event_cb(wifi_switch, wifi_switch_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Bluetooth switch
    lv_obj_t * bluetooth_label = lv_label_create(switches_container);
    lv_label_set_text(bluetooth_label, "Bluetooth");
    lv_obj_align(bluetooth_label, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    
    lv_obj_t * bluetooth_switch = lv_switch_create(switches_container);
    lv_obj_align(bluetooth_switch, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_obj_add_event_cb(bluetooth_switch, bluetooth_switch_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Create dropdowns container
    lv_obj_t * dropdowns_container = lv_obj_create(parent);
    lv_obj_set_size(dropdowns_container, 280, 100);
    lv_obj_align(dropdowns_container, LV_ALIGN_TOP_MID, 0, 360);
    lv_obj_set_style_pad_all(dropdowns_container, 10, 0);
    lv_obj_set_style_bg_color(dropdowns_container, lv_color_hex(0xF8F8F8), 0);
    lv_obj_set_style_border_width(dropdowns_container, 1, 0);
    lv_obj_set_style_border_color(dropdowns_container, lv_color_hex(0xDDDDDD), 0);
    
    // Theme dropdown
    lv_obj_t * theme_label = lv_label_create(dropdowns_container);
    lv_label_set_text(theme_label, "Theme");
    lv_obj_align(theme_label, LV_ALIGN_TOP_LEFT, 0, 0);
    
    lv_obj_t * theme_dropdown = lv_dropdown_create(dropdowns_container);
    lv_obj_set_size(theme_dropdown, 150, 30);
    lv_obj_align(theme_dropdown, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_dropdown_set_options(theme_dropdown, "Light\nDark\nAuto\nCustom");
    lv_dropdown_set_selected(theme_dropdown, 0);
    lv_obj_add_event_cb(theme_dropdown, theme_dropdown_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Language dropdown
    lv_obj_t * language_label = lv_label_create(dropdowns_container);
    lv_label_set_text(language_label, "Language");
    lv_obj_align(language_label, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    
    lv_obj_t * language_dropdown = lv_dropdown_create(dropdowns_container);
    lv_obj_set_size(language_dropdown, 150, 30);
    lv_obj_align(language_dropdown, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    lv_dropdown_set_options(language_dropdown, "English\nKorean\nJapanese\nChinese");
    lv_dropdown_set_selected(language_dropdown, 0);
    lv_obj_add_event_cb(language_dropdown, language_dropdown_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Create buttons container
    lv_obj_t * buttons_container = lv_obj_create(parent);
    lv_obj_set_size(buttons_container, 280, 80);
    lv_obj_align(buttons_container, LV_ALIGN_TOP_MID, 0, 480);
    lv_obj_set_style_pad_all(buttons_container, 10, 0);
    lv_obj_set_style_bg_color(buttons_container, lv_color_hex(0xF8F8F8), 0);
    lv_obj_set_style_border_width(buttons_container, 1, 0);
    lv_obj_set_style_border_color(buttons_container, lv_color_hex(0xDDDDDD), 0);
    
    // Save button
    lv_obj_t * save_btn = lv_btn_create(buttons_container);
    lv_obj_set_size(save_btn, 80, 35);
    lv_obj_align(save_btn, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_set_style_bg_color(save_btn, lv_color_hex(0x28A745), 0);
    lv_obj_add_event_cb(save_btn, save_button_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * save_label = lv_label_create(save_btn);
    lv_label_set_text(save_label, "Save");
    lv_obj_center(save_label);
    
    // Reset button
    lv_obj_t * reset_btn = lv_btn_create(buttons_container);
    lv_obj_set_size(reset_btn, 80, 35);
    lv_obj_align(reset_btn, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(reset_btn, lv_color_hex(0xDC3545), 0);
    lv_obj_add_event_cb(reset_btn, reset_button_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * reset_label = lv_label_create(reset_btn);
    lv_label_set_text(reset_label, "Reset");
    lv_obj_center(reset_label);
    
    // About button
    lv_obj_t * about_btn = lv_btn_create(buttons_container);
    lv_obj_set_size(about_btn, 80, 35);
    lv_obj_align(about_btn, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_set_style_bg_color(about_btn, lv_color_hex(0x17A2B8), 0);
    lv_obj_add_event_cb(about_btn, about_button_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * about_label = lv_label_create(about_btn);
    lv_label_set_text(about_label, "About");
    lv_obj_center(about_label);
    
    // Create status bar
    lv_obj_t * status_bar = lv_label_create(parent);
    lv_label_set_text(status_bar, "Settings Demo - All widgets functional");
    lv_obj_set_style_text_color(status_bar, lv_color_hex(0x666666), 0);
    lv_obj_set_style_text_font(status_bar, &lv_font_montserrat_14, 0);
    lv_obj_align(status_bar, LV_ALIGN_BOTTOM_MID, 0, 0);
    
    printf("Settings tab: Created comprehensive demo with multiple widgets\n");
    
    // Force a timer handler to process the image loading
    lv_timer_handler();
}   