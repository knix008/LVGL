#include "tab_clock.h"
#include "clock.h"
#include "font_config.h"
#include "ui_components.h"
#include <stdio.h>

// Constants for clock tab
#define CLOCK_TITLE_TEXT "현재 시간"
#define CLOCK_TIME_INITIAL_TEXT "00:00:00"
#define CLOCK_DATE_INITIAL_TEXT "2024년 01월 01일"
#define CLOCK_DAY_INITIAL_TEXT "월요일"
#define CLOCK_INFO_TEXT "실시간 시계\n매초 자동 업데이트"

// Color constants
#define CLOCK_TITLE_COLOR 0x2196F3    // Blue
#define CLOCK_TIME_COLOR 0x4CAF50      // Green
#define CLOCK_DATE_COLOR 0xFF9800      // Orange
#define CLOCK_DAY_COLOR 0x9C27B0       // Purple
#define CLOCK_INFO_COLOR 0x757575      // Gray
#define CLOCK_LINE_COLOR 0xE0E0E0      // Light gray

// Position constants
#define CLOCK_TITLE_Y 10
#define CLOCK_TIME_Y 50
#define CLOCK_DATE_Y -180
#define CLOCK_DAY_Y -140
#define CLOCK_LINE_Y -120
#define CLOCK_INFO_Y -20

// Size constants
#define CLOCK_LINE_WIDTH 400
#define CLOCK_LINE_HEIGHT 2
#define CLOCK_TIME_FONT_SIZE 72

// Global variables for clock display
static lv_obj_t *hours_label = NULL;
static lv_obj_t *colon_label = NULL;
static lv_obj_t *minutes_label = NULL;
static lv_obj_t *date_label = NULL;
static lv_obj_t *day_label = NULL;
static lv_timer_t *clock_timer = NULL;
static lv_timer_t *blink_timer = NULL;
static bool colon_visible = true;

// Font for large time display
static lv_font_t *time_large_font = NULL;

// Function to update clock display
static void update_clock_display(void) {
    clock_time_t current_time;
    clock_get_current_time(&current_time);
    
    // Format time strings for separate display
    char hours_str[8];
    char minutes_str[8];
    char date_str[64];
    
    snprintf(hours_str, sizeof(hours_str), "%02d", current_time.hour);
    snprintf(minutes_str, sizeof(minutes_str), "%02d", current_time.minute);
    clock_format_date_string(&current_time, date_str, sizeof(date_str));
    
    // Get day name
    const char* day_str = clock_get_day_name(current_time.day_of_week);
    
    // Update labels if they exist
    if (hours_label) {
        lv_label_set_text(hours_label, hours_str);
    }
    
    if (minutes_label) {
        lv_label_set_text(minutes_label, minutes_str);
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

// Blink timer callback function
static void blink_timer_cb(lv_timer_t * timer) {
    if (colon_label) {
        colon_visible = !colon_visible;
        if (colon_visible) {
            lv_obj_set_style_text_color(colon_label, lv_color_hex(CLOCK_TIME_COLOR), 0);
        } else {
            lv_obj_set_style_text_color(colon_label, lv_color_hex(0xFFFFFF), 0); // Transparent (same as background)
        }
    }
}

// Create Clock tab
void create_clock_tab(lv_obj_t * parent) {
    // Create large extra bold font for time display
    const char* font_path = FONT_PATH(KOREAN_FONT_EXTRA);
    time_large_font = lv_freetype_font_create(font_path, 
                                             KOREAN_FONT_RENDER_MODE, 
                                             CLOCK_TIME_FONT_SIZE,  // Large size for time display
                                             KOREAN_FONT_STYLE);
    if (!time_large_font) {
        time_large_font = get_korean_font(); // Fallback
        printf("Failed to create large font, using default font\n");
    } else {
        printf("Large font (%dpx) created successfully for time display\n", CLOCK_TIME_FONT_SIZE);
    }
    
    // Create title
    lv_obj_t * title_label = lv_label_create(parent);
    lv_label_set_text(title_label, CLOCK_TITLE_TEXT);
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, CLOCK_TITLE_Y);
    lv_obj_set_style_text_font(title_label, get_korean_font(), 0);
    lv_obj_set_style_text_color(title_label, lv_color_hex(CLOCK_TITLE_COLOR), 0);
    
    // Create hours display (left side)
    hours_label = lv_label_create(parent);
    lv_label_set_text(hours_label, "00");
    lv_obj_align(hours_label, LV_ALIGN_TOP_MID, -60, CLOCK_TIME_Y);
    lv_obj_set_style_text_font(hours_label, time_large_font, 0);
    lv_obj_set_style_text_color(hours_label, lv_color_hex(CLOCK_TIME_COLOR), 0);
    
    // Create blinking colon (center)
    colon_label = lv_label_create(parent);
    lv_label_set_text(colon_label, ":");
    lv_obj_align(colon_label, LV_ALIGN_TOP_MID, 0, CLOCK_TIME_Y);
    lv_obj_set_style_text_font(colon_label, time_large_font, 0);
    lv_obj_set_style_text_color(colon_label, lv_color_hex(CLOCK_TIME_COLOR), 0);
    
    // Create minutes display (right side)
    minutes_label = lv_label_create(parent);
    lv_label_set_text(minutes_label, "00");
    lv_obj_align(minutes_label, LV_ALIGN_TOP_MID, 60, CLOCK_TIME_Y);
    lv_obj_set_style_text_font(minutes_label, time_large_font, 0);
    lv_obj_set_style_text_color(minutes_label, lv_color_hex(CLOCK_TIME_COLOR), 0);
    
    // Create date display - moved to lower position
    date_label = lv_label_create(parent);
    lv_label_set_text(date_label, CLOCK_DATE_INITIAL_TEXT);
    lv_obj_align(date_label, LV_ALIGN_BOTTOM_MID, 0, CLOCK_DATE_Y);
    lv_obj_set_style_text_font(date_label, get_korean_font(), 0);
    lv_obj_set_style_text_color(date_label, lv_color_hex(CLOCK_DATE_COLOR), 0);
    
    // Create day of week display - moved to lower position
    day_label = lv_label_create(parent);
    lv_label_set_text(day_label, CLOCK_DAY_INITIAL_TEXT);
    lv_obj_align(day_label, LV_ALIGN_BOTTOM_MID, 0, CLOCK_DAY_Y);
    lv_obj_set_style_text_font(day_label, get_korean_font(), 0);
    lv_obj_set_style_text_color(day_label, lv_color_hex(CLOCK_DAY_COLOR), 0);
    
    // Create decorative line
    lv_obj_t * line = lv_obj_create(parent);
    lv_obj_set_size(line, CLOCK_LINE_WIDTH, CLOCK_LINE_HEIGHT);
    lv_obj_align(line, LV_ALIGN_BOTTOM_MID, 0, CLOCK_LINE_Y);
    lv_obj_set_style_bg_color(line, lv_color_hex(CLOCK_LINE_COLOR), 0);
    lv_obj_set_style_border_width(line, 0, 0);
    lv_obj_set_style_radius(line, 1, 0);
    
    // Create info text
    lv_obj_t * info_label = lv_label_create(parent);
    lv_label_set_text(info_label, CLOCK_INFO_TEXT);
    lv_obj_align(info_label, LV_ALIGN_BOTTOM_MID, 0, CLOCK_INFO_Y);
    lv_obj_set_style_text_font(info_label, get_korean_font_small(), 0);
    lv_obj_set_style_text_color(info_label, lv_color_hex(CLOCK_INFO_COLOR), 0);
    lv_obj_set_style_text_align(info_label, LV_TEXT_ALIGN_CENTER, 0);
    
    // Initialize clock display
    update_clock_display();
    
    // Create timer to update clock every second
    clock_timer = lv_timer_create(clock_timer_cb, 1000, NULL); // 1000ms = 1 second
    
    // Create timer for blinking colon every 500ms
    blink_timer = lv_timer_create(blink_timer_cb, 500, NULL); // 500ms = 0.5 second
    
    printf("Clock tab created successfully\n");
} 