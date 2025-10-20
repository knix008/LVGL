/*
 * Japanese Input GUI Application Header
 * Defines interface for the Japanese QWERTY input system
 */

#ifndef GUI_APP_H
#define GUI_APP_H

#include "lvgl/lvgl.h"

// Initialize the GUI application
int gui_app_init(void);

// Create the user interface
void gui_app_create_ui(void);

// Cleanup resources
void gui_app_cleanup(void);

#endif // GUI_APP_H
