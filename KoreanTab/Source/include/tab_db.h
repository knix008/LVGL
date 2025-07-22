#ifndef TAB_DB_H
#define TAB_DB_H

#include "lvgl.h"

// Database tab functions
void create_db_tab(lv_obj_t * parent);

// Database tab callbacks
void btn_event_cb(lv_event_t * e);
void btn2_event_cb(lv_event_t * e);

#endif // TAB_DB_H 