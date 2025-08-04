#include "tab_clock.h"
#include "calendar.h"
#include "font_config.h"
#include "ui_components.h"
#include <stdio.h>
#include <time.h>
#include <string.h>

// Global variables for clock display
static lv_obj_t *time_label = NULL;
static lv_obj_t *date_label = NULL;
static lv_obj_t *day_label = NULL;
static lv_timer_t *clock_timer = NULL;

// Function to update clock display
static void update_clock_display(void) {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    
    // Format time string (HH:MM:SS)
    char time_str[32];
    snprintf(time_str, sizeof(time_str), "%02d:%02d:%02d", 
             tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
    
    // Format date string
    char date_str[64];
    snprintf(date_str, sizeof(date_str), "%04d년 %02d월 %02d일", 
             tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday);
    
    // Get day of week
    const char* day_names[] = {"일요일", "월요일", "화요일", "수요일", 
                               "목요일", "금요일", "토요일"};
    const char* day_str = day_names[tm_info->tm_wday];
    
    // Update labels if they exist
    if (time_label) {
        lv_label_set_text(time_label, time_str);
    }
    
    if (date_label) {
        lv_label_set_text(date_label, date_str);
    }
    
    if (day_label) {
        lv_label_set_text(day_label, day_str);
    }
}

// Timer callback function
static void clock_timer_cb(lv_timer_t * timer) {
    update_clock_display();
}

// Create Clock tab
void create_clock_tab(lv_obj_t * parent) {
    // Create title
    lv_obj_t * title_label = lv_label_create(parent);
    lv_label_set_text(title_label, "현재 시간");
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 20);
    lv_obj_set_style_text_font(title_label, get_korean_font(), 0);
    lv_obj_set_style_text_color(title_label, lv_color_hex(0x2196F3), 0); // Blue color
    
    // Create time display (large font)
    time_label = lv_label_create(parent);
    lv_label_set_text(time_label, "00:00:00");
    lv_obj_align(time_label, LV_ALIGN_CENTER, 0, -30);
    lv_obj_set_style_text_font(time_label, get_korean_font(), 0);
    lv_obj_set_style_text_color(time_label, lv_color_hex(0x4CAF50), 0); // Green color
    
    // Create date display
    date_label = lv_label_create(parent);
    lv_label_set_text(date_label, "2024년 01월 01일");
    lv_obj_align(date_label, LV_ALIGN_CENTER, 0, 20);
    lv_obj_set_style_text_font(date_label, get_korean_font(), 0);
    lv_obj_set_style_text_color(date_label, lv_color_hex(0xFF9800), 0); // Orange color
    
    // Create day of week display
    day_label = lv_label_create(parent);
    lv_label_set_text(day_label, "월요일");
    lv_obj_align(day_label, LV_ALIGN_CENTER, 0, 60);
    lv_obj_set_style_text_font(day_label, get_korean_font(), 0);
    lv_obj_set_style_text_color(day_label, lv_color_hex(0x9C27B0), 0); // Purple color
    
    // Create decorative line
    lv_obj_t * line = lv_obj_create(parent);
    lv_obj_set_size(line, 400, 2);
    lv_obj_align(line, LV_ALIGN_CENTER, 0, 100);
    lv_obj_set_style_bg_color(line, lv_color_hex(0xE0E0E0), 0); // Light gray
    lv_obj_set_style_border_width(line, 0, 0);
    lv_obj_set_style_radius(line, 1, 0);
    
    // Create info text
    lv_obj_t * info_label = lv_label_create(parent);
    lv_label_set_text(info_label, "실시간 시계\n매초 자동 업데이트");
    lv_obj_align(info_label, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_set_style_text_font(info_label, get_korean_font_small(), 0);
    lv_obj_set_style_text_color(info_label, lv_color_hex(0x757575), 0); // Gray color
    lv_obj_set_style_text_align(info_label, LV_TEXT_ALIGN_CENTER, 0);
    
    // Initialize clock display
    update_clock_display();
    
    // Create timer to update clock every second
    clock_timer = lv_timer_create(clock_timer_cb, 1000, NULL); // 1000ms = 1 second
    
    printf("Clock tab created successfully\n");
} 