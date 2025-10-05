/**
 * @file main.c
 * @brief Main application for Chunjiin Korean input keyboard
 */

#include "lvgl.h"
#include "lv_drivers.h"
#include "keyboard_ui.h"
#include "lv_freetype.h"
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

static volatile int keep_running = 1;

void signal_handler(int sig) {
    keep_running = 0;
}

int main(void) {
    signal(SIGINT, signal_handler);

    printf("=== Chunjiin Korean Input Keyboard ===\n");
    printf("Initializing LVGL...\n");

    /* Initialize LVGL */
    lv_init();

    /* Create SDL window */
    printf("Creating SDL window (400x640)...\n");
    lv_display_t* disp = lv_sdl_window_create(400, 640);
    if (disp == NULL) {
        printf("ERROR: Failed to create SDL window\n");
        return -1;
    }

    /* Set window title */
    lv_sdl_window_set_title(disp, "Chunjiin Keyboard - Korean/English/Numbers");

    /* Create input devices */
    lv_indev_t* mouse = lv_sdl_mouse_create();
    lv_indev_t* mousewheel = lv_sdl_mousewheel_create();
    lv_indev_t* keyboard = lv_sdl_keyboard_create();

    /* Initialize FreeType for Korean fonts */
    printf("Initializing FreeType...\n");
    if (!lv_freetype_init(256)) {
        printf("WARNING: FreeType initialization failed\n");
        printf("Korean fonts may not display correctly\n");
    } else {
        printf("FreeType initialized successfully\n");
    }

    /* Initialize keyboard UI */
    printf("Creating keyboard UI...\n");
    keyboard_ui_init();

    printf("\n=== Keyboard Ready ===\n");
    printf("Features:\n");
    printf("  - Korean input using Chunjiin method\n");
    printf("  - English input using T9 method\n");
    printf("  - Number input\n");
    printf("  - Click 'Mode' button to switch between modes\n");
    printf("\nPress Ctrl+C to exit\n\n");

    /* Main loop */
    while (keep_running) {
        lv_timer_handler();
        usleep(5000);  /* 5ms */
    }

    printf("\nShutting down...\n");
    return 0;
}
