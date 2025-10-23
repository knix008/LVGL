/**
 * @file main.c
 * @brief Main entry point for Password Input System
 *
 * This file contains only the main() function and coordinates
 * the password validation logic (password.c) and GUI components
 * (password_gui.c).
 *
 * Architecture:
 * - password.c:     LVGL-independent password validation logic
 * - password_gui.c: LVGL-dependent GUI implementation
 * - main.c:         Application entry point and coordination
 */

#include "password.h"
#include "password_gui.h"
#include <stdio.h>

/**
 * @brief Callback function called when user successfully logs in
 *
 * @param userid User ID string
 * @param password Password string (already validated)
 */
static void on_login_success(const char *userid, const char *password) {
    printf("\n=== Login Successful ===\n");
    printf("User ID: %s\n", userid);
    printf("Password: %s\n", password);
    printf("========================\n\n");
}

/**
 * @brief Main application entry point
 */
int main(void) {
    printf("\n========================================\n");
    printf("  Login System with Password Validation\n");
    printf("========================================\n");
    printf("Window Size: %dx%d\n", WINDOW_WIDTH, WINDOW_HEIGHT);
    printf("\nPassword Requirements:\n");
    printf("  - Minimum %d characters\n", MIN_PASSWORD_LENGTH);
    printf("  - At least one capital letter (A-Z)\n");
    printf("  - At least one number (0-9)\n");
    printf("  - At least one special character\n");
    printf("\nEnter your credentials in the window.\n");
    printf("========================================\n\n");

    // Initialize GUI system
    if (!gui_init()) {
        fprintf(stderr, "Failed to initialize GUI\n");
        return 1;
    }

    // Set login callback
    gui_set_login_callback(on_login_success);

    // Create the login UI
    gui_create_login_screen();

    // Run the main event loop
    gui_run();

    // Cleanup (unreachable in this example)
    gui_cleanup();

    return 0;
}
