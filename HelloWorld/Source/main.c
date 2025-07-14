#include <stdio.h>
#include <unistd.h>
#include "lvgl.h"
#include "lv_drivers.h"

static void btn_event_cb(lv_event_t * e) {
    lv_obj_t * label = lv_event_get_user_data(e);
    lv_label_set_text(label, "Button Clicked!");
}

void lv_example_simple_gui(void) {
    // Create a screen
    lv_obj_t * scr = lv_scr_act();

    // Create a label
    lv_obj_t * label = lv_label_create(scr);
    lv_label_set_text(label, "Hello LVGL!");
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 20);

    // Create a button
    lv_obj_t * btn = lv_btn_create(scr);
    lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);

    // Create a label on the button
    lv_obj_t * btn_label = lv_label_create(btn);
    lv_label_set_text(btn_label, "Click me!");

    // Assign event callback to the button, pass label as user data
    lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, label);
}

// In your main loop, call lv_timer_handler() periodically, e.g., every 5-10 ms
int main(void)
{
    lv_init();
    
    // Initialize SDL display driver
    lv_display_t * disp = lv_sdl_window_create(800, 480);
    if (disp == NULL) {
        printf("Failed to create SDL window!\n");
        return -1;
    }
    
    // Set window title
    lv_sdl_window_set_title(disp, "LVGL Hello World");
    
    // Create input devices
    lv_indev_t * mouse = lv_sdl_mouse_create();
    lv_indev_t * mousewheel = lv_sdl_mousewheel_create();
    lv_indev_t * keyboard = lv_sdl_keyboard_create();

    lv_example_simple_gui();

    printf("GUI created successfully. Window should appear now.\n");
    printf("Press Ctrl+C to exit.\n");

    while(1) {
        lv_timer_handler();
        usleep(5000);
    }

    return 0;
}