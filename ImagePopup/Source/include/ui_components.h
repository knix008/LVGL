#ifndef UI_COMPONENTS_H
#define UI_COMPONENTS_H

#include "lvgl.h"

// UI initialization and setup

void lv_example_tab_menu(void);

// Font management
lv_font_t * get_korean_font(void);
lv_font_t * get_korean_font_small(void);

// Button event callbacks
void btn_event_cb(lv_event_t * e);
void btn2_event_cb(lv_event_t * e);

// Tab event callbacks
void tab_event_cb(lv_event_t * e);
void slider_event_cb(lv_event_t * e);
void switch_event_cb(lv_event_t * e);

// Number keypad callbacks
void keypad_number_cb(lv_event_t * e);
void keypad_clear_cb(lv_event_t * e);
void keypad_enter_cb(lv_event_t * e);
void keypad_backspace_cb(lv_event_t * e);

#endif // UI_COMPONENTS_H 