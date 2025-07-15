#ifndef UI_KEYPAD_H
#define UI_KEYPAD_H

#include "lvgl.h"
#include "ui_callbacks.h"

// Keypad display label (extern for callbacks)
extern lv_obj_t * keypad_display_label;

// Keypad buffer (extern for callbacks)
extern char keypad_buffer[32];

// Create keypad UI components
void create_keypad_ui(lv_obj_t * parent);

// Initialize keypad buffer
void init_keypad_buffer(void);

#endif // UI_KEYPAD_H 