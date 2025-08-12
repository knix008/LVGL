#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include "lvgl.h"
#include "lv_drivers.h"
#include "sdl/lv_sdl_window.h"
#include "sdl/lv_sdl_mouse.h"
#include "sdl/lv_sdl_keyboard.h"
#include "sdl/lv_sdl_mousewheel.h"

// Function to check if file exists
bool file_exists(const char* filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

// Function to display SVG files using ThorVG
void lv_show_svg_file(void) {
    lv_obj_t * scr = lv_scr_act();
    
    // Create a container for SVG images
    lv_obj_t * svg_container = lv_obj_create(scr);
    lv_obj_set_size(svg_container, 700, 500);
    lv_obj_align(svg_container, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_border_width(svg_container, 2, 0);
    lv_obj_set_style_border_color(svg_container, lv_color_hex(0x000000), 0);
    lv_obj_set_style_pad_all(svg_container, 20, 0);
    
    // Create a title label
    lv_obj_t * title_label = lv_label_create(svg_container);
    lv_label_set_text(title_label, "SVG Image Display Test with ThorVG");
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_14, 0);
    
    printf("=== SVG Display Test Started ===\n");
    printf("Current working directory: ");
    system("pwd");
    printf("Attempting to load SVG files...\n");

    // Create SVG image objects for different test files
    const char* svg_files[] = {
        "A:../assets/simple_circle.svg",
        "A:../assets/simple_rectangle.svg", 
        "A:../assets/simple_star.svg",
        "A:../assets/simple_triangle.svg",
        "A:../assets/simple_text.svg",
        "A:../assets/complex_design.svg"
    };
    
    // Also try absolute paths
    const char* svg_files_abs[] = {
        "A:/home/shkwon/Projects/LVGL/SVG/Source/assets/simple_circle.svg",
        "A:/home/shkwon/Projects/LVGL/SVG/Source/assets/simple_rectangle.svg", 
        "A:/home/shkwon/Projects/LVGL/SVG/Source/assets/simple_star.svg",
        "A:/home/shkwon/Projects/LVGL/SVG/Source/assets/simple_triangle.svg",
        "A:/home/shkwon/Projects/LVGL/SVG/Source/assets/simple_text.svg",
        "A:/home/shkwon/Projects/LVGL/SVG/Source/assets/complex_design.svg"
    };
    
    const char* svg_names[] = {
        "Circle",
        "Rectangle",
        "Star", 
        "Triangle",
        "Text",
        "Complex"
    };
    
    int num_files = sizeof(svg_files) / sizeof(svg_files[0]);
    int cols = 3; // 3 columns
    int rows = (num_files + cols - 1) / cols; // Calculate rows needed
    
    for (int i = 0; i < num_files; i++) {
        // Create a container for each SVG
        lv_obj_t * item_container = lv_obj_create(svg_container);
        lv_obj_set_size(item_container, 200, 150);
        
        // Calculate position (grid layout)
        int col = i % cols;
        int row = i / cols;
        int x_offset = (col - 1) * 220; // 200 width + 20 padding
        int y_offset = row * 170 + 50;  // 150 height + 20 padding + 50 for title
        
        lv_obj_align(item_container, LV_ALIGN_TOP_LEFT, x_offset, y_offset);
        lv_obj_set_style_border_width(item_container, 1, 0);
        lv_obj_set_style_border_color(item_container, lv_color_hex(0xCCCCCC), 0);
        lv_obj_set_style_pad_all(item_container, 10, 0);
        
        // Create label for SVG name
        lv_obj_t * name_label = lv_label_create(item_container);
        lv_label_set_text(name_label, svg_names[i]);
        lv_obj_align(name_label, LV_ALIGN_TOP_MID, 0, 5);
        lv_obj_set_style_text_font(name_label, &lv_font_montserrat_14, 0);
        
        // Create the SVG image object
        lv_obj_t * svg_img = lv_image_create(item_container);
        
        // Check if file exists first
        const char* relative_path = svg_files[i] + 2; // Remove "A:" prefix
        const char* abs_path = svg_files_abs[i] + 2;  // Remove "A:" prefix
        
        printf("Loading: %s\n", svg_names[i]);
        printf("  Relative path: %s (exists: %s)\n", relative_path, file_exists(relative_path) ? "Yes" : "No");
        printf("  Absolute path: %s (exists: %s)\n", abs_path, file_exists(abs_path) ? "Yes" : "No");
        
        // Try relative path first, then absolute path
        const char* path_to_use = svg_files[i];
        if (!file_exists(relative_path)) {
            path_to_use = svg_files_abs[i];
            printf("  Using absolute path\n");
        } else {
            printf("  Using relative path\n");
        }
        
        // Set SVG source
        lv_img_set_src(svg_img, path_to_use);
        
        // Set size based on SVG type
        int img_width, img_height;
        if (i == 5) { // Complex design - larger
            img_width = 120;
            img_height = 80;
        } else { // Simple SVGs - smaller
            img_width = 80;
            img_height = 80;
        }
        
        lv_obj_set_size(svg_img, img_width, img_height);
        lv_obj_align(svg_img, LV_ALIGN_CENTER, 0, 10);
        
        // Check SVG loading status with more details
        const void* svg_src = lv_image_get_src(svg_img);
        if (svg_src) {
            printf("✓ SVG loaded successfully: %s (size: %dx%d)\n", svg_names[i], img_width, img_height);
            
            // Try to get image descriptor for more info
            const lv_image_dsc_t* dsc = lv_image_get_src(svg_img);
            if (dsc) {
                printf("  - Image descriptor found: %dx%d pixels\n", dsc->header.w, dsc->header.h);
            }
        } else {
            printf("✗ SVG failed to load: %s\n", svg_names[i]);
            printf("  - Source pointer is NULL\n");
            printf("  - This might indicate SVG decoder is not properly configured\n");
            
            // Try alternative loading method - create a simple colored rectangle as fallback
            printf("  - Creating fallback colored rectangle\n");
            lv_obj_set_style_bg_color(svg_img, lv_color_hex(0xFF0000), 0);
            lv_obj_set_style_bg_opa(svg_img, LV_OPA_50, 0);
        }
    }
    
    // Add status information
    lv_obj_t * status_label = lv_label_create(svg_container);
    lv_label_set_text(status_label, "SVG files loaded using ThorVG library.\nCheck console for detailed loading status.");
    lv_obj_align(status_label, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_set_style_text_font(status_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(status_label, lv_color_hex(0x666666), 0);
    
    printf("=== SVG Display Setup Complete ===\n");
}

int main(void) {
    // Initialize LVGL
    lv_init();
    
    // Initialize SDL driver
    lv_display_t * display = lv_sdl_window_create(800, 600);
    lv_indev_t * mouse_indev = lv_sdl_mouse_create();
    lv_indev_t * keyboard_indev = lv_sdl_keyboard_create();
    lv_indev_t * mousewheel_indev = lv_sdl_mousewheel_create();
    
    // Initialize SVG decoder if available
    #if LV_USE_SVG
    printf("SVG support is enabled in LVGL configuration\n");
    #else
    printf("SVG support is NOT enabled in LVGL configuration\n");
    #endif
    
    #if LV_USE_THORVG
    printf("ThorVG support is enabled in LVGL configuration\n");
    #else
    printf("ThorVG support is NOT enabled in LVGL configuration\n");
    #endif
    
    #if LV_USE_VECTOR_GRAPHIC
    printf("Vector Graphic support is enabled in LVGL configuration\n");
    #else
    printf("Vector Graphic support is NOT enabled in LVGL configuration\n");
    #endif
    
    // Create the comprehensive SVG demonstration
    lv_show_svg_file();
    
    printf("SVG Display Test Started\n");
    printf("Press 'q' to quit\n");
    
    // Main loop
    while(1) {
        lv_timer_handler();
        usleep(5000);
    }
    
    return 0;
}