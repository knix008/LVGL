#include "lvgl.h"
#include "lv_drivers.h"
#include "svg_example.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Test function to demonstrate SVG functionality
void test_svg_functionality(void) {
    printf("Testing SVG functionality...\n");
    
    // Create a simple SVG example
    printf("Creating SVG example...\n");
    create_svg_example(lv_scr_act());
    
    // Test loading SVG from file
    printf("Testing SVG file loading...\n");
    create_svg_from_file_example(lv_scr_act(), "../assets/calendar_icon.svg");
    
    // Test creating SVG widgets
    printf("Creating SVG widgets...\n");
    lv_obj_t * svg_widget1 = create_svg_widget(lv_scr_act(), "../assets/clock_icon.svg");
    lv_obj_align(svg_widget1, LV_ALIGN_TOP_LEFT, 10, 10);
    
    lv_obj_t * svg_widget2 = create_svg_widget(lv_scr_act(), "../assets/settings_icon.svg");
    lv_obj_align(svg_widget2, LV_ALIGN_TOP_RIGHT, -10, 10);
    
    lv_obj_t * svg_widget3 = create_svg_widget(lv_scr_act(), "../assets/minimal.svg");
    lv_obj_align(svg_widget3, LV_ALIGN_BOTTOM_LEFT, 10, -10);
    
    printf("SVG test completed.\n");
}

int main(void) {
    // Initialize LVGL
    lv_init();
    
    // Initialize SDL display driver
    lv_display_t * disp = lv_sdl_window_create(800, 600);
    if (disp == NULL) {
        printf("Failed to create SDL window!\n");
        return -1;
    }
    
    // Set window title
    lv_sdl_window_set_title(disp, "SVG Test Demo");
    
    // Create input devices
    lv_indev_t * mouse = lv_sdl_mouse_create();
    lv_indev_t * mousewheel = lv_sdl_mousewheel_create();
    lv_indev_t * keyboard = lv_sdl_keyboard_create();
    
    // Test SVG functionality
    test_svg_functionality();
    
    printf("SVG test window created successfully. Window should appear now.\n");
    printf("Press Ctrl+C to exit.\n");
    
    // Main loop
    while(1) {
        lv_timer_handler();
        usleep(5000); // 5ms delay
    }
    
    return 0;
} 