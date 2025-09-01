#include <stdio.h>
#include <unistd.h>
#include <SDL2/SDL.h>
#include "lvgl.h"
#include "lv_drivers.h"
#include "ui_components.h"
#include "mariadb_test.h"
#include "lv_freetype.h"
#include "lv_ffmpeg.h"
#include "web_server.h"

// Global quit flag
static volatile bool quit_requested = false;

// Custom keyboard event handler for quit key
void handle_quit_key(SDL_Event *event) {
    if (event->type == SDL_KEYDOWN) {
        // Check for Q key to quit
        if (event->key.keysym.sym == SDLK_q) {
            printf("\nQuit key (Q) pressed. Exiting...\n");
            quit_requested = true;
        }
        // Also support Escape key
        else if (event->key.keysym.sym == SDLK_ESCAPE) {
            printf("\nEscape key pressed. Exiting...\n");
            quit_requested = true;
        }
    }
}

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
    lv_sdl_window_set_title(disp, "FaceRecognition 2.0 Simulator");
    
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
    printf("Press Q or Escape to exit.\n");

    while(!quit_requested) {
        // Handle SDL events for quit keys
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            handle_quit_key(&event);
        }
        
        lv_timer_handler();
        web_server_poll();  // Poll web server
        usleep(5000);
    }

    printf("Shutting down...\n");
    return 0;
}