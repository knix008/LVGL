#include "web_control.h"
#include "ui_components.h"
#include <stdio.h>
#include <string.h>

// Global variable to store the tabview reference
static lv_obj_t* g_tabview = NULL;
static tab_index_t g_current_tab = TAB_DB;

// Function to set the tabview reference (called from ui_components.c)
void set_tabview_reference(lv_obj_t* tabview) {
    g_tabview = tabview;
    printf("Tab control: Tabview reference set\n");
}

// Function to switch to a specific tab
void switch_to_tab(tab_index_t tab_index) {
    if (g_tabview == NULL) {
        printf("Tab control: Tabview not initialized\n");
        return;
    }
    
    // Get the total number of tabs
    uint32_t tab_count = lv_tabview_get_tab_count(g_tabview);
    
    // Check if the tab index is valid
    if (tab_index >= tab_count) {
        printf("Tab control: Invalid tab index %d (max: %lu)\n", tab_index, (unsigned long)tab_count);
        return;
    }
    
    // Switch to the specified tab
    lv_tabview_set_active(g_tabview, tab_index, LV_ANIM_ON);
    g_current_tab = tab_index;
    
    printf("Tab control: Switched to tab %d\n", tab_index);
}

// Function to get current active tab
tab_index_t get_current_tab(void) {
    if (g_tabview == NULL) {
        return TAB_DB; // Default tab
    }
    
    uint32_t active_tab = lv_tabview_get_tab_act(g_tabview);
    g_current_tab = (tab_index_t)active_tab;
    return g_current_tab;
}

// Function to switch tab by name
void switch_to_tab_by_name(const char* tab_name) {
    if (tab_name == NULL) {
        printf("Tab control: NULL tab name\n");
        return;
    }
    
    tab_index_t tab_index = TAB_DB; // Default
    
    // Map tab names to indices
    if (strcmp(tab_name, "db") == 0) {
        tab_index = TAB_DB;
    } else if (strcmp(tab_name, "settings") == 0) {
        tab_index = TAB_SETTINGS;
    } else if (strcmp(tab_name, "num") == 0) {
        tab_index = TAB_NUM;
    } else if (strcmp(tab_name, "kor") == 0 || strcmp(tab_name, "korean") == 0) {
        tab_index = TAB_KOR;
    } else if (strcmp(tab_name, "cji") == 0 || strcmp(tab_name, "chunjiin") == 0) {
        tab_index = TAB_CJI;
    } else if (strcmp(tab_name, "qwerty") == 0) {
        tab_index = TAB_QWERTY;
    } else if (strcmp(tab_name, "cal") == 0 || strcmp(tab_name, "calendar") == 0) {
        tab_index = TAB_CAL;
    } else if (strcmp(tab_name, "clock") == 0) {
        tab_index = TAB_CLOCK;
    } else if (strcmp(tab_name, "av") == 0 || strcmp(tab_name, "video") == 0) {
        tab_index = TAB_AV;
    } else if (strcmp(tab_name, "info") == 0) {
        tab_index = TAB_INFO;
    } else {
        printf("Tab control: Unknown tab name '%s'\n", tab_name);
        return;
    }
    
    printf("Tab control: Switching to tab '%s' (index %d)\n", tab_name, tab_index);
    switch_to_tab(tab_index);
}
