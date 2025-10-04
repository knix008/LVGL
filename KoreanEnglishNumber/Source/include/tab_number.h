#ifndef TAB_NUMBER_H
#define TAB_NUMBER_H

#include "lvgl.h"

// Number tab functions
lv_obj_t* create_number_tab(lv_obj_t * parent);

// Number tab callbacks
void keypad_number_cb(lv_event_t * e);
void keypad_clear_cb(lv_event_t * e);
void keypad_enter_cb(lv_event_t * e);
void keypad_backspace_cb(lv_event_t * e);
void keypad_close_dialog_cb(lv_event_t * e);

#endif // TAB_NUMBER_H 