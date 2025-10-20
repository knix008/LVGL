/*
 * LVGL Initialization Module Implementation
 * Handles LVGL system initialization, SDL setup, and display configuration
 */

#include "lvgl_init.h"
#include "lvgl/src/drivers/sdl/lv_sdl_window.h"
#include "lvgl/src/drivers/sdl/lv_sdl_mouse.h"
#include "lvgl/src/drivers/sdl/lv_sdl_mousewheel.h"
#include "lvgl/src/drivers/sdl/lv_sdl_keyboard.h"
#include <stdio.h>
#include <time.h>
#include <locale.h>

// Static variables for display and input devices
static lv_display_t *g_display = NULL;
static lv_indev_t *g_mouse = NULL;
static lv_indev_t *g_mousewheel = NULL;
static lv_indev_t *g_keyboard = NULL;

// Initialize LVGL system
int lvgl_init_system(void) {
    // Set locale for proper character handling
    setlocale(LC_ALL, "");
    
    // Initialize LVGL
    lv_init();
    
    // Create SDL window and display
    g_display = lv_sdl_window_create(800, 600);
    if (g_display == NULL) {
        printf("Failed to create SDL window!\n");
        return -1;
    }
    
    // Create input devices
    g_mouse = lv_sdl_mouse_create();
    g_mousewheel = lv_sdl_mousewheel_create();
    g_keyboard = lv_sdl_keyboard_create();
    
    // Check if input devices were created successfully
    if (!g_mouse || !g_mousewheel || !g_keyboard) {
        printf("Warning: Some input devices failed to initialize\n");
    }
    
    return 0;
}

// Cleanup LVGL system
void lvgl_cleanup(void) {
    // LVGL cleanup is handled automatically when the program exits
    // SDL cleanup is also handled automatically
    g_display = NULL;
    g_mouse = NULL;
    g_mousewheel = NULL;
    g_keyboard = NULL;
}

// Get the display object
lv_display_t* lvgl_get_display(void) {
    return g_display;
}

// Set window title
void lvgl_set_window_title(const char* title) {
    if (g_display) {
        lv_sdl_window_set_title(g_display, title);
    }
}

// Main loop - call lv_timer_handler periodically (every 5ms)
void lvgl_main_loop(void) {
    while(1) {
        lv_timer_handler();
        struct timespec ts = {0, 5000000}; // 5ms in nanoseconds
        nanosleep(&ts, NULL);
    }
}
