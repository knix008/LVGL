/*
 * Japanese Character Input Method - Main Entry Point
 * Orchestrates LVGL initialization and GUI application startup
 */

#include "lvgl_init.h"
#include "gui_app.h"
#include <stdio.h>

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    printf("Starting Japanese Input Application...\n");

    // Initialize LVGL system
    if (lvgl_init_system() != 0) {
        printf("Failed to initialize LVGL system!\n");
        return -1;
    }

    // Set window title
    lvgl_set_window_title("日本語入力 - Japanese Input Method");

    // Initialize GUI application
    if (gui_app_init() != 0) {
        printf("Failed to initialize GUI application!\n");
        lvgl_cleanup();
        return -1;
    }

    // Create the user interface
    gui_app_create_ui();

    printf("Application started successfully!\n");

    // Start the main loop
    lvgl_main_loop();

    // Cleanup (this will never be reached due to infinite loop, but included for completeness)
    gui_app_cleanup();
    lvgl_cleanup();

    return 0;
}
