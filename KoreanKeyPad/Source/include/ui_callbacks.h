#ifndef UI_CALLBACKS_H
#define UI_CALLBACKS_H

#include "lvgl.h"

// Keypad callbacks
void keypad_number_cb(lv_event_t * e);
void keypad_clear_cb(lv_event_t * e);
void keypad_enter_cb(lv_event_t * e);
void keypad_backspace_cb(lv_event_t * e);

// Korean keypad callbacks
void korean_keypad_char_cb(lv_event_t * e);
void korean_keypad_mode_cb(lv_event_t * e);
void korean_keypad_clear_cb(lv_event_t * e);
void korean_keypad_backspace_cb(lv_event_t * e);
void korean_keypad_enter_cb(lv_event_t * e);
void korean_keypad_space_cb(lv_event_t * e);

// Database operation callbacks
void btn_event_cb(lv_event_t * e);
void btn2_event_cb(lv_event_t * e);

// UI control callbacks
void tab_event_cb(lv_event_t * e);
void slider_event_cb(lv_event_t * e);
void switch_event_cb(lv_event_t * e);

#endif // UI_CALLBACKS_H 