#ifndef TAB_CALENDAR_H
#define TAB_CALENDAR_H

#include "lvgl.h"

// Calendar tab functions
void create_calendar_tab(lv_obj_t * parent);

// Calendar tab callbacks
void calendar_month_cb(lv_event_t * e);
void calendar_clear_cb(lv_event_t * e);
void calendar_enter_cb(lv_event_t * e);

#endif // TAB_CALENDAR_H 