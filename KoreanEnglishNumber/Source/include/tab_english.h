#ifndef TAB_ENGLISH_H
#define TAB_ENGLISH_H

#include "lvgl.h"

// Create English T9-style input tab
lv_obj_t* create_english_tab(lv_obj_t* parent);

// Close dialog callback
void english_close_dialog_cb(lv_event_t * e);

#endif // TAB_ENGLISH_H
