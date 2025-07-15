#include <stdio.h>
#include <unistd.h>
#include "lvgl.h"
#include "lv_drivers.h"
#include "ui_components.h"
#include "sqlite_test.h"

// In your main loop, call lv_timer_handler() periodically, e.g., every 5-10 ms
int main(void)
{
    lv_init();
    
    // Initialize SDL display driver
    lv_display_t * disp = lv_sdl_window_create(320, 480);
    if (disp == NULL) {
        printf("Failed to create SDL window!\n");
        return -1;
    }
    
    // Set window title
    lv_sdl_window_set_title(disp, "SQLite Test with LVGL");
    
    // Create input devices
    lv_indev_t * mouse = lv_sdl_mouse_create();
    lv_indev_t * mousewheel = lv_sdl_mousewheel_create();
    lv_indev_t * keyboard = lv_sdl_keyboard_create();

    // Run SQLite demonstration
    printf("Running SQLite demonstration...\n");
    sqlite_demo();

    // Initialize UI
    lv_example_simple_gui();

    printf("GUI created successfully. Window should appear now.\n");
    printf("Press Ctrl+C to exit.\n");

    while(1) {
        lv_timer_handler();
        usleep(5000);
    }

    return 0;
}