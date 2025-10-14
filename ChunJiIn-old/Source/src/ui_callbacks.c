#include "ui_callbacks.h"
#include <stdio.h>

// Tab event callback
void tab_event_cb(lv_event_t * e) {
    lv_obj_t * tabview = lv_event_get_target(e);
    uint32_t active_tab = lv_tabview_get_tab_act(tabview);
    
    printf("Active tab changed to: %lu\n", (unsigned long)active_tab);
} 