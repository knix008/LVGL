/**
 * @file password_gui.h
 * @brief Password login GUI interface (LVGL-dependent)
 *
 * This module provides the graphical user interface for the
 * password login system using LVGL.
 */

#ifndef PASSWORD_GUI_H
#define PASSWORD_GUI_H

#include "lvgl/lvgl.h"
#include <stdbool.h>

// Display configuration
#define WINDOW_WIDTH  320
#define WINDOW_HEIGHT 640

/**
 * @brief Login callback function type
 *
 * Called when user successfully logs in with valid credentials.
 *
 * @param userid User ID string
 * @param password Password string (already validated)
 */
typedef void (*login_callback_t)(const char *userid, const char *password);

/**
 * @brief Initializes the GUI system
 *
 * Sets up LVGL display, input devices, and fonts.
 *
 * @return true on success, false on failure
 */
bool gui_init(void);

/**
 * @brief Creates the login user interface
 *
 * Builds all UI components for the login screen.
 */
void gui_create_login_screen(void);

/**
 * @brief Sets the login callback function
 *
 * @param callback Function to call on successful login
 */
void gui_set_login_callback(login_callback_t callback);

/**
 * @brief Runs the main GUI event loop
 *
 * This function blocks and processes LVGL events.
 * Call this after initializing and creating the UI.
 */
void gui_run(void);

/**
 * @brief Cleans up GUI resources
 *
 * Frees fonts and other allocated resources.
 */
void gui_cleanup(void);

/**
 * @brief Gets the User ID input field object
 *
 * @return Pointer to user ID textarea object
 */
lv_obj_t *gui_get_userid_input(void);

/**
 * @brief Gets the Password input field object
 *
 * @return Pointer to password textarea object
 */
lv_obj_t *gui_get_password_input(void);

/**
 * @brief Displays a status message
 *
 * @param message Message to display
 * @param is_error true for error (red), false for success (green)
 */
void gui_show_status(const char *message, bool is_error);

/**
 * @brief Gets the normal Korean font
 *
 * @return Pointer to normal font, or NULL if not loaded
 */
lv_font_t *gui_get_normal_font(void);

/**
 * @brief Gets the bold Korean font
 *
 * @return Pointer to bold font, or NULL if not loaded
 */
lv_font_t *gui_get_bold_font(void);

#endif /* PASSWORD_GUI_H */
