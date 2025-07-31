#include "tab_calendar.h"
#include <string.h>
#include <stdio.h>
#include <time.h>

// Global variables for calendar input
static char calendar_buffer[32] = "";
static lv_obj_t * calendar_display_label = NULL;
static int current_year = 2024;
static int current_month = 1;
static int current_day = 1;

// Calendar input callback functions
void calendar_date_cb(lv_event_t * e) {
    lv_obj_t * btn = lv_event_get_target(e);
    lv_obj_t * label = lv_obj_get_child(btn, 0);
    const char * number = lv_label_get_text(label);
    
    // Add number to buffer if there's space
    if (strlen(calendar_buffer) < sizeof(calendar_buffer) - 1) {
        strcat(calendar_buffer, number);
        lv_label_set_text(calendar_display_label, calendar_buffer);
    }
}

void calendar_month_cb(lv_event_t * e) {
    lv_obj_t * btn = lv_event_get_target(e);
    lv_obj_t * label = lv_obj_get_child(btn, 0);
    const char * month_text = lv_label_get_text(label);
    
    // Convert month name to number
    const char * months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                            "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    int month_num = 0;
    for (int i = 0; i < 12; i++) {
        if (strcmp(month_text, months[i]) == 0) {
            month_num = i + 1;
            break;
        }
    }
    
    if (month_num > 0) {
        current_month = month_num;
        // Update display with selected month
        char display_text[64];
        snprintf(display_text, sizeof(display_text), "%s %d, %d", months[month_num-1], current_day, current_year);
        lv_label_set_text(calendar_display_label, display_text);
    }
}

void calendar_year_cb(lv_event_t * e) {
    lv_obj_t * btn = lv_event_get_target(e);
    lv_obj_t * label = lv_obj_get_child(btn, 0);
    const char * year_text = lv_label_get_text(label);
    
    // Convert year text to number
    int year = atoi(year_text);
    if (year >= 1900 && year <= 2100) {
        current_year = year;
        // Update display with selected year
        char display_text[64];
        const char * months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
        snprintf(display_text, sizeof(display_text), "%s %d, %d", months[current_month-1], current_day, current_year);
        lv_label_set_text(calendar_display_label, display_text);
    }
}

void calendar_clear_cb(lv_event_t * e) {
    calendar_buffer[0] = '\0';
    current_year = 2024;
    current_month = 1;
    current_day = 1;
    lv_label_set_text(calendar_display_label, "Jan 1, 2024");
}

void calendar_enter_cb(lv_event_t * e) {
    if (strlen(calendar_buffer) > 0) {
        // Parse the entered date
        int day = atoi(calendar_buffer);
        if (day >= 1 && day <= 31) {
            current_day = day;
            // Update display with complete date
            char display_text[64];
            const char * months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
            snprintf(display_text, sizeof(display_text), "%s %d, %d", months[current_month-1], current_day, current_year);
            lv_label_set_text(calendar_display_label, display_text);
            printf("Selected date: %s %d, %d\n", months[current_month-1], current_day, current_year);
        }
        calendar_buffer[0] = '\0';
    }
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
    lv_obj_align(calendar_display, LV_ALIGN_TOP_MID, 0, 40);
    calendar_display_label = calendar_display;

    // Month selection buttons (3x4 grid)
    const char * month_labels[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    int btn_width = 80;
    int btn_height = 40;
    int btn_spacing = 5;
    int grid_width = 3 * btn_width + 2 * btn_spacing;
    int grid_height = 4 * btn_height + 3 * btn_spacing;
    
    // Calculate center offsets
    int center_x_offset = -(grid_width / 3);
    int center_y_offset = -80;

    // Create month buttons
    for (int i = 0; i < 12; i++) {
        lv_obj_t * btn = lv_btn_create(parent);
        lv_obj_set_size(btn, btn_width, btn_height);
        
        int row = i / 3;
        int col = i % 3;
        
        int rel_x = center_x_offset + col * (btn_width + btn_spacing);
        int rel_y = center_y_offset + row * (btn_height + btn_spacing);
        
        lv_obj_align(btn, LV_ALIGN_CENTER, rel_x, rel_y);

        lv_obj_t * btn_label = lv_label_create(btn);
        lv_label_set_text(btn_label, month_labels[i]);
        lv_obj_center(btn_label);

        lv_obj_add_event_cb(btn, calendar_month_cb, LV_EVENT_CLICKED, NULL);
    }

    // Year selection buttons (recent years)
    lv_obj_t * year_label = lv_label_create(parent);
    lv_label_set_text(year_label, "Year:");
    lv_obj_align(year_label, LV_ALIGN_CENTER, -200, 80);

    const char * year_labels[] = {"2020", "2021", "2022", "2023", "2024", "2025"};
    int year_btn_width = 60;
    int year_btn_height = 30;
    int year_btn_spacing = 5;
    
    for (int i = 0; i < 6; i++) {
        lv_obj_t * btn = lv_btn_create(parent);
        lv_obj_set_size(btn, year_btn_width, year_btn_height);
        
        int rel_x = -200 + i * (year_btn_width + year_btn_spacing);
        lv_obj_align(btn, LV_ALIGN_CENTER, rel_x, 110);

        lv_obj_t * btn_label = lv_label_create(btn);
        lv_label_set_text(btn_label, year_labels[i]);
        lv_obj_center(btn_label);

        lv_obj_add_event_cb(btn, calendar_year_cb, LV_EVENT_CLICKED, NULL);
    }

    // Day input area
    lv_obj_t * day_label = lv_label_create(parent);
    lv_label_set_text(day_label, "Day:");
    lv_obj_align(day_label, LV_ALIGN_CENTER, 100, 80);

    // Day input display
    lv_obj_t * day_display = lv_label_create(parent);
    lv_label_set_text(day_display, "1");
    lv_obj_set_style_bg_color(day_display, lv_color_hex(0xF0F0F0), 0);
    lv_obj_set_style_bg_opa(day_display, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(day_display, 5, 0);
    lv_obj_set_size(day_display, 60, 30);
    lv_obj_align(day_display, LV_ALIGN_CENTER, 100, 110);

    // Day number buttons (1-31)
    const char * day_numbers[] = {"1", "2", "3", "4", "5", "6", "7", "8", "9", "10",
                                 "11", "12", "13", "14", "15", "16", "17", "18", "19", "20",
                                 "21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31"};
    
    int day_btn_width = 40;
    int day_btn_height = 25;
    int day_btn_spacing = 2;
    
    for (int i = 0; i < 31; i++) {
        lv_obj_t * btn = lv_btn_create(parent);
        lv_obj_set_size(btn, day_btn_width, day_btn_height);
        
        int row = i / 7;  // 7 buttons per row
        int col = i % 7;
        
        int rel_x = 100 + col * (day_btn_width + day_btn_spacing);
        int rel_y = 150 + row * (day_btn_height + day_btn_spacing);
        
        lv_obj_align(btn, LV_ALIGN_CENTER, rel_x, rel_y);

        lv_obj_t * btn_label = lv_label_create(btn);
        lv_label_set_text(btn_label, day_numbers[i]);
        lv_obj_center(btn_label);

        lv_obj_add_event_cb(btn, calendar_date_cb, LV_EVENT_CLICKED, NULL);
    }

    // Function buttons
    lv_obj_t * clear_btn = lv_btn_create(parent);
    lv_obj_set_size(clear_btn, 80, 40);
    lv_obj_align(clear_btn, LV_ALIGN_CENTER, -100, 200);
    
    lv_obj_t * clear_label = lv_label_create(clear_btn);
    lv_label_set_text(clear_label, "Clear");
    lv_obj_center(clear_label);
    lv_obj_add_event_cb(clear_btn, calendar_clear_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t * enter_btn = lv_btn_create(parent);
    lv_obj_set_size(enter_btn, 80, 40);
    lv_obj_align(enter_btn, LV_ALIGN_CENTER, 100, 200);
    
    lv_obj_t * enter_label = lv_label_create(enter_btn);
    lv_label_set_text(enter_label, "Enter");
    lv_obj_center(enter_label);
    lv_obj_add_event_cb(enter_btn, calendar_enter_cb, LV_EVENT_CLICKED, NULL);
} 