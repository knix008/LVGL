#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "lvgl.h"
#include "lv_drivers.h"


// Function to display SVG files using ThorVG
void lv_show_svg_file(void) {
    lv_obj_t * scr = lv_scr_act();
    
    // Create an image widget to display the SVG
    lv_obj_t * svg_screen = lv_obj_create(scr);
    lv_obj_set_size(svg_screen, 400, 300);
    lv_obj_align(svg_screen, LV_ALIGN_CENTER, 0, 0);
    
    // Add a border to make the image more visible
    lv_obj_set_style_border_width(svg_screen, 2, 0);
    lv_obj_set_style_border_color(svg_screen, lv_color_hex(0x000000), 0);
    
    printf("Attempting to load SVG file: simple_circle.svg\n");
    
    // Create the SVG image object for settings info (left side)
    lv_obj_t * svg_img = lv_image_create(svg_screen);
    lv_image_set_src(svg_img, "A:../assets/rectangle.svg");
    lv_obj_set_size(svg_img, 64, 64);
    lv_obj_align(svg_img, LV_ALIGN_TOP_MID, -40, 50); // Position to the left
    
    // Check SVG loading status
    const void* svg_src = lv_image_get_src(svg_img);
    if (svg_src) {
        printf("Settings tab: SVG loaded successfully: A:../assets/rectangle.svg\n");
        printf("Settings tab: SVG source pointer: %p\n", svg_src);
    } else {
        printf("Settings tab: SVG failed to load: A:../assets/rectangle.svg\n");
        printf("Settings tab: SVG source is NULL\n");
    }
}

int main(void) {
    // Initialize LVGL
    lv_init();
    // Initialize SDL driver
    lv_display_t * display = lv_sdl_window_create(800, 600);
    lv_indev_t * mouse_indev = lv_sdl_mouse_create();
    lv_indev_t * keyboard_indev = lv_sdl_keyboard_create();
    lv_indev_t * mousewheel_indev = lv_sdl_mousewheel_create();
    
    // Create the comprehensive SVG demonstration
    lv_show_svg_file();
    
    // Main loop
    while(1) {
        lv_timer_handler();
        usleep(5000);
    }
    
    return 0;
}