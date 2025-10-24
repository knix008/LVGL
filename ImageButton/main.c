/**
 * @file main.c
 * @brief Main entry point for the LVGL Image Button Application
 *
 * This application demonstrates LVGL image buttons with different image formats.
 * It showcases PNG, JPG, GIF, and BMP image formats in interactive buttons.
 */

#include "lvgl.h"
#include "image_button_app.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <locale.h>

/**
 * @brief Main entry point
 */
int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    printf("=========================================\n");
    printf(" LVGL Image Button Demo Application\n");
    printf("=========================================\n");
    printf("Demonstrating image buttons with different formats:\n");
    printf("  • PNG (with transparency)\n");
    printf("  • JPG (with compression)\n");
    printf("  • GIF (with animation support)\n");
    printf("  • BMP (uncompressed bitmap)\n");
    printf("=========================================\n\n");

    // Set locale
    setlocale(LC_ALL, "");

    // Initialize LVGL
    lv_init();

    // Create SDL window and display
    lv_display_t *disp = lv_sdl_window_create(320, 640);
    if (disp == NULL) {
        printf("Failed to create SDL window!\n");
        return -1;
    }

    printf("LVGL initialized with SDL backend\n");
    printf("Window size: 320x640\n");

    // Set window title
    lv_sdl_window_set_title(disp, "LVGL Image Button Demo");

    // Create input devices
    lv_indev_t *mouse = lv_sdl_mouse_create();
    lv_indev_t *mousewheel = lv_sdl_mousewheel_create();
    lv_indev_t *keyboard = lv_sdl_keyboard_create();
    (void)mouse;
    (void)mousewheel;
    (void)keyboard;

    // Initialize the image button application UI
    image_button_app_init();

    // Main loop - call lv_timer_handler periodically (every 5ms)
    while(1) {
        lv_timer_handler();
        usleep(5000);  // 5ms
    }

    printf("Bye!!!\n");

    return 0;
}
