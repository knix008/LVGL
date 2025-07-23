#ifndef TAB_SETTINGS_H
#define TAB_SETTINGS_H

#include "lvgl.h"

// Settings tab functions
void create_settings_tab(lv_obj_t * parent);

// Settings tab callbacks
void slider_event_cb(lv_event_t * e);
void switch_event_cb(lv_event_t * e);

#endif // TAB_SETTINGS_H 