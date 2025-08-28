#include <stdio.h>
#include <unistd.h>
#include "lvgl.h"
#include "lv_drivers.h"
#include "ui_components.h"
#include "mariadb_test.h"
#include "lv_freetype.h"
#include "lv_ffmpeg.h"
#include "web_server.h"

// In your main loop, call lv_timer_handler() periodically, e.g., every 5-10 ms
int main(void)
{
    lv_init();
    
    // Initialize SDL display driver
    lv_display_t * disp = lv_sdl_window_create(800, 600);
    if (disp == NULL) {
        printf("Failed to create SDL window!\n");
        return -1;
    }
    
    // Set window title
    lv_sdl_window_set_title(disp, "Video Input Demo");
    
    // Create input devices
    lv_indev_t * mouse = lv_sdl_mouse_create();
    lv_indev_t * mousewheel = lv_sdl_mousewheel_create();
    lv_indev_t * keyboard = lv_sdl_keyboard_create();

    // Initialize FreeType
    printf("Initializing FreeType...\n");
    lv_freetype_init(256);
    printf("FreeType initialized successfully.\n");

    // Initialize FFmpeg
    printf("Initializing FFmpeg...\n");
    lv_ffmpeg_init();
    printf("FFmpeg initialized successfully.\n");

    // Run MariaDB demonstration
    printf("Running MariaDB demonstration...\n");
    mariadb_demo();

    // Initialize web server
    printf("Initializing web server...\n");
    web_server_init();
    printf("Web server initialized successfully.\n");

    // Initialize UI with tab menu
    lv_example_tab_menu();

    printf("Tab menu GUI created successfully. Window should appear now.\n");
    printf("Web interface available at: http://localhost:8080\n");
    printf("Press Ctrl+C to exit.\n");

    while(1) {
        lv_timer_handler();
        web_server_poll();  // Poll web server
        usleep(5000);
    }

    return 0;
}