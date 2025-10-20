/*
 * main.c
 * Main entry point for Japanese QWERTY Input Method Editor
 */

#include "lvgl/lvgl.h"
#include "japanese_qwerty.h"
#include "japanese_gui.h"
#include <stdio.h>

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    // Initialize LVGL
    lv_init();

    // Create display using LVGL's SDL driver (640x480)
    lv_display_t *disp = lv_sdl_window_create(SCREEN_WIDTH, SCREEN_HEIGHT);

    if (!disp) {
        printf("Error: Failed to create SDL window\n");
        return 1;
    }

    // Enable mouse input for clicking buttons
    lv_indev_t *mouse_indev = lv_sdl_mouse_create();
    if (!mouse_indev) {
        printf("Warning: Failed to create mouse input device\n");
    }

    // Initialize IME state
    IMEState ime_state;
    ime_init(&ime_state);

    // Initialize GUI state
    GUIState gui_state;
    gui_init(&gui_state, &ime_state);

    // Load Japanese fonts from assets directory
    if (!gui_load_fonts(&gui_state)) {
        printf("Warning: Using default font, Japanese characters may not display correctly\n");
    }

    // Create UI with QWERTY button keyboard
    gui_create_ui(&gui_state);

    printf("\n========================================\n");
    printf("Japanese QWERTY Input Method Started!\n");
    printf("========================================\n\n");
    printf("Window Size: %dx%d\n", SCREEN_WIDTH, SCREEN_HEIGHT);
    printf("\nControls:\n");
    printf("  Click buttons to type\n");
    printf("  Switch Mode - Toggle between Hiragana/Katakana/English\n");
    printf("  Space       - Commit text and add space\n");
    printf("  Enter       - Commit text and add newline\n");
    printf("  Bksp        - Backspace\n");
    printf("  -           - Prolonged sound mark (ー)\n");
    printf("  Clear       - Clear all text\n\n");
    printf("Examples:\n");
    printf("  konnichiha → こんにちは\n");
    printf("  arigatou   → ありがとう\n");
    printf("  nihon      → にほん\n");
    printf("  to-kyo-    → とーきょー\n\n");

    // Main loop
    while (1) {
        lv_timer_handler();
        lv_delay_ms(5);
    }

    return 0;
}
