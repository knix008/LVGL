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
    printf("\nButton Controls:\n");
    printf("  Letter Buttons - Type Japanese characters directly (JIS layout)\n");
    printf("  あ/A Button - Toggle: Japanese (Hiragana) ↔ English\n");
    printf("  Shift       - Toggle Hiragana↔Katakana / Small chars / Uppercase\n");
    printf("  123/ABC     - Toggle numbers/symbols mode\n");
    printf("  Space       - Add space\n");
    printf("  Enter       - Show result popup and clear text\n");
    printf("  ← (Bksp)    - Delete last character\n");
    printf("  Clear       - Clear all text\n");
    printf("  ゛ (dakuten)   - Add dakuten mark (か→が)\n");
    printf("  ゜ (handakuten)- Add handakuten mark (は→ぱ)\n");
    printf("  ー          - Prolonged sound mark\n\n");
    printf("Examples:\n");
    printf("  か + ゛ → が (ga)\n");
    printf("  は + ゛ → ば (ba)\n");
    printf("  は + ゜ → ぱ (pa)\n");
    printf("  こんにちは: こ-ん-に-ち-は\n");
    printf("  ありがとう: Click buttons to type each character\n\n");

    // Main loop
    while (1) {
        lv_timer_handler();
        lv_delay_ms(5);
    }

    return 0;
}
