/**
 * @file main.c
 * @brief Main application for IP Address Input GUI
 * @details LVGL-based GUI for IPv4 and IPv6 address input with validation
 */

#include "ip_address_gui.h"
#include "lvgl.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* SDL drivers */
#include "lvgl/src/drivers/sdl/lv_sdl_window.h"
#include "lvgl/src/drivers/sdl/lv_sdl_mouse.h"
#include "lvgl/src/drivers/sdl/lv_sdl_mousewheel.h"
#include "lvgl/src/drivers/sdl/lv_sdl_keyboard.h"

/**
 * @brief Main function
 * @param argc Argument count
 * @param argv Argument vector
 * @return Exit status
 */
int main(int argc, char *argv[]) {
    (void)argc; /* Unused parameter */
    (void)argv; /* Unused parameter */
    
    printf("IP Address Input GUI - Starting...\n");
    
    /* Initialize LVGL */
    lv_init();
    
    /* Initialize SDL display and input */
    lv_display_t *disp = lv_sdl_window_create(WINDOW_WIDTH, WINDOW_HEIGHT);
    lv_indev_t *mouse = lv_sdl_mouse_create();
    lv_indev_t *mousewheel = lv_sdl_mousewheel_create();
    lv_indev_t *keyboard = lv_sdl_keyboard_create();
    
    (void)mouse;      /* Suppress unused variable warning */
    (void)mousewheel; /* Suppress unused variable warning */
    (void)keyboard;   /* Suppress unused variable warning */
    
    if (disp == NULL) {
        printf("Error: Failed to create SDL window\n");
        return -1;
    }
    
    printf("SDL window created: %dx%d\n", WINDOW_WIDTH, WINDOW_HEIGHT);
    
    /* Initialize IP GUI */
    ip_gui_state_t *gui_state = ip_gui_init();
    if (gui_state == NULL) {
        printf("Error: Failed to initialize GUI\n");
        return -1;
    }
    
    /* Create UI */
    ip_gui_create_ui(gui_state);
    
    /* Load the screen */
    lv_screen_load(gui_state->main_screen);
    
    printf("GUI initialized successfully\n");
    printf("Features:\n");
    printf("  - IPv4 address input and validation\n");
    printf("  - IPv6 address input and validation\n");
    printf("  - Mode switching between IPv4/IPv6\n");
    printf("  - Custom keyboard layouts\n");
    printf("  - Real-time validation feedback\n");
    printf("\nClick on the white input area to show/hide keyboard.\n");
    printf("Use the toggle switch to change between IPv4 and IPv6 modes.\n\n");
    
    /* Main loop */
    while (1) {
        /* Handle LVGL tasks */
        uint32_t time_till_next = lv_timer_handler();
        
        /* Sleep for a short time */
        usleep(time_till_next * 1000);
    }
    
    /* Cleanup */
    ip_gui_cleanup(gui_state);
    
    return 0;
}
