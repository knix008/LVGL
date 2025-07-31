#ifndef TAB_CALENDAR_H
#define TAB_CALENDAR_H

#include "lvgl.h"

// Calendar tab functions
void create_calendar_tab(lv_obj_t * parent);

// Calendar tab callbacks
void calendar_prev_month_cb(lv_event_t * e);
void calendar_next_month_cb(lv_event_t * e);
void calendar_prev_year_cb(lv_event_t * e);
void calendar_next_year_cb(lv_event_t * e);
void calendar_close_dialog_cb(lv_event_t * e);
void calendar_enter_cb(lv_event_t * e);

#endif // TAB_CALENDAR_H 