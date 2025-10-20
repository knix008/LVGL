/*
 * LVGL Initialization Module Header
 * Handles LVGL system initialization, SDL setup, and display configuration
 */

#ifndef LVGL_INIT_H
#define LVGL_INIT_H

#include "lvgl/lvgl.h"

// LVGL initialization functions
int lvgl_init_system(void);
void lvgl_cleanup(void);

// Display and input device management
lv_display_t* lvgl_get_display(void);
void lvgl_set_window_title(const char* title);

// Main loop control
void lvgl_main_loop(void);

#endif // LVGL_INIT_H
