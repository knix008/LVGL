#ifndef UI_KOREAN_KEYPAD_H
#define UI_KOREAN_KEYPAD_H

#include "lvgl.h"
#include "korean_hangul.h"

// Korean keypad display label (extern for callbacks)
extern lv_obj_t * korean_keypad_display_label;

// Create Korean keypad UI components
void create_korean_keypad_ui(lv_obj_t * parent);

// Korean keypad callback functions
void korean_keypad_char_cb(lv_event_t * e);
void korean_keypad_mode_cb(lv_event_t * e);
void korean_keypad_clear_cb(lv_event_t * e);
void korean_keypad_backspace_cb(lv_event_t * e);
void korean_keypad_enter_cb(lv_event_t * e);
void korean_keypad_space_cb(lv_event_t * e);

#endif // UI_KOREAN_KEYPAD_H 