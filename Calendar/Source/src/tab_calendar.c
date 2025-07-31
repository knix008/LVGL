#include "tab_calendar.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Global variables for calendar input
static char calendar_buffer[32] = "";
static lv_obj_t * calendar_display_label = NULL;
static int current_year = 2024;
static int current_month = 1;
static int current_day = 1;

// Navigation callback functions
void calendar_prev_month_cb(lv_event_t * e) {
    current_month--;
    if (current_month < 1) {
        current_month = 12;
        current_year--;
    }
    
    // Update display
    const char * months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    char display_text[64];
    snprintf(display_text, sizeof(display_text), "%s %d, %d", months[current_month-1], current_day, current_year);
    lv_label_set_text(calendar_display_label, display_text);
}

void calendar_next_month_cb(lv_event_t * e) {
    current_month++;
    if (current_month > 12) {
        current_month = 1;
        current_year++;
    }
    
    // Update display
    const char * months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    char display_text[64];
    snprintf(display_text, sizeof(display_text), "%s %d, %d", months[current_month-1], current_day, current_year);
    lv_label_set_text(calendar_display_label, display_text);
}

// Popup dialog close callback
void calendar_close_dialog_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        // Close the dialog
        lv_obj_t * btn = lv_event_get_target(e);
        lv_obj_t * dialog = lv_obj_get_parent(btn);
        lv_obj_del(dialog);
    }
}

void calendar_enter_cb(lv_event_t * e) {
    // Get current display text
    const char* current_text = lv_label_get_text(calendar_display_label);
    
    // Create popup dialog to show the result
    lv_obj_t * parent = lv_obj_get_parent(lv_event_get_target(e));
    lv_obj_t * dialog = lv_obj_create(parent);
    lv_obj_set_size(dialog, 250, 150);
    lv_obj_align(dialog, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_bg_color(dialog, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(dialog, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(dialog, 2, 0);
    lv_obj_set_style_border_color(dialog, lv_color_hex(0x333333), 0);
    lv_obj_set_style_radius(dialog, 10, 0);
    
    // Title label
    lv_obj_t * title_label = lv_label_create(dialog);
    lv_label_set_text(title_label, "Calendar Selection Result");
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_14, 0);
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 0);
    
    // Result text label
    lv_obj_t * popup_result_label = lv_label_create(dialog);
    lv_obj_set_style_text_font(popup_result_label, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(popup_result_label, lv_color_hex(0x00AA00), 0); // Green text
    lv_obj_align(popup_result_label, LV_ALIGN_CENTER, 0, -5);
    
    char popup_text[300];
    snprintf(popup_text, sizeof(popup_text), "Selected Date:\n%s", current_text);
    lv_label_set_text(popup_result_label, popup_text);
    
    // Close button
    lv_obj_t * close_btn = lv_btn_create(dialog);
    lv_obj_set_size(close_btn, 80, 40);
    lv_obj_align(close_btn, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_t * close_label = lv_label_create(close_btn);
    lv_label_set_text(close_label, "Close");
    lv_obj_center(close_label);
    
    // Close button callback
    lv_obj_add_event_cb(close_btn, calendar_close_dialog_cb, LV_EVENT_CLICKED, NULL);
    
    printf("Calendar: Enter 버튼 클릭 - Selected date: %s\n", current_text);
}

// Create Calendar tab
void create_calendar_tab(lv_obj_t * parent) {
    // Tab: Calendar Input
    lv_obj_t * calendar_label = lv_label_create(parent);
    lv_label_set_text(calendar_label, "Calendar Input");
    lv_obj_align(calendar_label, LV_ALIGN_TOP_MID, 0, 10);

    // Display area for selected date
    lv_obj_t * calendar_display = lv_label_create(parent);
    lv_label_set_text(calendar_display, "Jan 1, 2024");
    lv_obj_set_style_bg_color(calendar_display, lv_color_hex(0xE0E0E0), 0);
    lv_obj_set_style_bg_opa(calendar_display, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(calendar_display, 10, 0);
    lv_obj_set_size(calendar_display, 300, 40);
    lv_obj_align(calendar_display, LV_ALIGN_TOP_MID, 0, 80);
    calendar_display_label = calendar_display;

    // Month navigation buttons
    lv_obj_t * prev_month_btn = lv_btn_create(parent);
    lv_obj_set_size(prev_month_btn, 50, 40);
    lv_obj_align(prev_month_btn, LV_ALIGN_TOP_MID, -180, 80);
    lv_obj_t * prev_month_label = lv_label_create(prev_month_btn);
    lv_label_set_text(prev_month_label, "<");
    lv_obj_center(prev_month_label);
    lv_obj_add_event_cb(prev_month_btn, calendar_prev_month_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * next_month_btn = lv_btn_create(parent);
    lv_obj_set_size(next_month_btn, 50, 40);
    lv_obj_align(next_month_btn, LV_ALIGN_TOP_MID, 180, 80);
    lv_obj_t * next_month_label = lv_label_create(next_month_btn);
    lv_label_set_text(next_month_label, ">");
    lv_obj_center(next_month_label);
    lv_obj_add_event_cb(next_month_btn, calendar_next_month_cb, LV_EVENT_CLICKED, NULL);

    // Function buttons
    lv_obj_t * enter_btn = lv_btn_create(parent);
    lv_obj_set_size(enter_btn, 80, 40);
    lv_obj_align(enter_btn, LV_ALIGN_TOP_MID, 0, 140);
    
    lv_obj_t * enter_label = lv_label_create(enter_btn);
    lv_label_set_text(enter_label, "Enter");
    lv_obj_center(enter_label);
    lv_obj_add_event_cb(enter_btn, calendar_enter_cb, LV_EVENT_CLICKED, NULL);
} 