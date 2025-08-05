#include "tab_settings.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include "lvgl.h"
#include "ui_components.h"


// Create Settings tab
void create_settings_tab(lv_obj_t * parent) {
    // Tab 2: Settings
    lv_obj_t * label2 = lv_label_create(parent);
    lv_label_set_text(label2, "Settings");
    lv_obj_align(label2, LV_ALIGN_TOP_MID, 0, 10);

    // Initialize decoders for JPG only to support Turbo JPEG image decoding.
    lv_tjpgd_init(); // Initialize JPG decoder
    // Create the JPG image object for settings info (center)
    lv_obj_t * jpg_img = lv_image_create(parent);
    lv_image_set_src(jpg_img, "A:../assets/setting.jpg");
    lv_obj_set_size(jpg_img, 64, 64);
    lv_obj_align(jpg_img, LV_ALIGN_CENTER, 0, -100); // Position in center of parent
           
    // Create the GIF image object for settings info (left side)
    lv_obj_t * gif_img = lv_gif_create(parent);
    lv_gif_set_src(gif_img, "A:../assets/bulb.gif");
    lv_obj_set_size(gif_img, 64, 64);
    lv_obj_align(gif_img, LV_ALIGN_CENTER, -80, -100); // Position to the left of center
    
    // Create the hourglass GIF image object (right side)
    lv_obj_t * hourglass_gif_img = lv_gif_create(parent);
    lv_gif_set_src(hourglass_gif_img, "A:../assets/hourglass_small.gif");
    lv_obj_set_size(hourglass_gif_img, 64, 64);
    lv_obj_align(hourglass_gif_img, LV_ALIGN_CENTER, 80, -100); // Position to the right of center

    // Create SVG image object
    //lv_obj_t * svg_img = lv_image_create(parent);
    //lv_image_set_src(svg_img, "A:../assets/simple_test.svg");
    //lv_obj_set_size(svg_img, 64, 64);
    //lv_obj_align(svg_img, LV_ALIGN_CENTER, 0, ); // Position to the right of center

    // Check SVG loading status
    //const void* svg_src = lv_image_get_src(svg_img);
    //if (svg_src) {
    //    printf("Settings tab: SVG loaded successfully: A:../assets/simple_test.svg\n");
    //    printf("Settings tab: SVG source pointer: %p\n", svg_src);
    //} else {
    //    printf("Settings tab: SVG failed to load: A:../assets/simple_test.svg\n");
    //    printf("Settings tab: SVG source is NULL\n");
    //}
    
    // Force a timer handler to process the image loading
    lv_timer_handler();
}   