#ifndef UI_COMPONENTS_H
#define UI_COMPONENTS_H

#include "lvgl.h"

// UI initialization and setup
void lv_example_simple_gui(void);
void lv_example_tab_menu(void);

// Button event callbacks
void btn_event_cb(lv_event_t * e);
void btn2_event_cb(lv_event_t * e);

// Tab event callbacks
void tab_event_cb(lv_event_t * e);
void slider_event_cb(lv_event_t * e);
void switch_event_cb(lv_event_t * e);

#endif // UI_COMPONENTS_H 