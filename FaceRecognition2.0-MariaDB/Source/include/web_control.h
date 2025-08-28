#ifndef WEB_CONTROL_H
#define WEB_CONTROL_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

// Tab indices (matching the order in ui_components.c)
typedef enum {
    TAB_DB = 0,
    TAB_SETTINGS = 1,
    TAB_NUM = 2,
    TAB_KOR = 3,
    TAB_CJI = 4,
    TAB_QWERTY = 5,
    TAB_CAL = 6,
    TAB_CLOCK = 7,
    TAB_AV = 8,
    TAB_OPENCV = 9,
    TAB_INFO = 10
} tab_index_t;

// Function to switch to a specific tab
void switch_to_tab(tab_index_t tab_index);

// Function to get current active tab
tab_index_t get_current_tab(void);

// Function to switch tab by name
void switch_to_tab_by_name(const char* tab_name);

// Function to set tabview reference (internal use)
void set_tabview_reference(lv_obj_t* tabview);

#ifdef __cplusplus
}
#endif

#endif // WEB_CONTROL_H
