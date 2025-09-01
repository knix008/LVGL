#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include "lvgl.h"
#include "lv_drivers.h"
#include "ui_components.h"
#include "mariadb_test.h"
#include "lv_freetype.h"
#include "lv_ffmpeg.h"
#include "web_server.h"

// Global quit flag
static bool quit_requested = false;

// Signal handler for Ctrl+C
static void signal_handler(int sig)
{
    if (sig == SIGINT) {
        printf("\nQuit requested by user (Ctrl+C pressed)\n");
        quit_requested = true;
    }
}

// Keyboard event handler for quit functionality
static void keyboard_event_cb(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_KEY) {
        uint32_t key = lv_event_get_key(e);
        if (key == LV_KEY_ESC || key == 'q' || key == 'Q') {
            printf("Quit requested by user (ESC/Q key pressed)\n");
            quit_requested = true;
        }
    }
}

// In your main loop, call lv_timer_handler() periodically, e.g., every 5-10 ms
int main(void)
{
    // Set up signal handler for Ctrl+C
    signal(SIGINT, signal_handler);
    
    lv_init();
    
    // Initialize SDL display driver
    lv_display_t * disp = lv_sdl_window_create(800, 600);
    if (disp == NULL) {
        printf("Failed to create SDL window!\n");
        return -1;
    }
    
    // Set window title
    lv_sdl_window_set_title(disp, "FaceRecognition 2.0 Simulator");
    
    // Create input devices
    lv_indev_t * mouse = lv_sdl_mouse_create();
    lv_indev_t * mousewheel = lv_sdl_mousewheel_create();
    lv_indev_t * keyboard = lv_sdl_keyboard_create();

    // Add keyboard event handler for quit functionality
    lv_obj_add_event_cb(lv_scr_act(), keyboard_event_cb, LV_EVENT_KEY, NULL);

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
    printf("Ctrl+C to exit.\n");

    while(!quit_requested) {
        lv_timer_handler();
        web_server_poll();  // Poll web server
        usleep(5000);
    }

    printf("Shutting down application...\n");
    return 0;
}