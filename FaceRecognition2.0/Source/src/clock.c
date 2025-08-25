#include "clock.h"
#include <stdio.h>
#include <string.h>

// Day names in Korean
static const char* day_names[] = {
    "일요일", "월요일", "화요일", "수요일", 
    "목요일", "금요일", "토요일"
};

// Initialize clock system
void clock_init(void) {
    // No specific initialization needed for basic time functions
    printf("Clock system initialized\n");
}

// Get current time and populate clock_time_t structure
void clock_get_current_time(clock_time_t* clock_time) {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    
    clock_time->hour = tm_info->tm_hour;
    clock_time->minute = tm_info->tm_min;
    clock_time->second = tm_info->tm_sec;
    clock_time->year = tm_info->tm_year + 1900;
    clock_time->month = tm_info->tm_mon + 1;
    clock_time->day = tm_info->tm_mday;
    clock_time->day_of_week = tm_info->tm_wday;
}

// Format time string (HH:MM:SS)
void clock_format_time_string(const clock_time_t* time, char* buffer, size_t buffer_size) {
    snprintf(buffer, buffer_size, "%02d:%02d:%02d", 
             time->hour, time->minute, time->second);
}

// Format date string (YYYY년 MM월 DD일)
void clock_format_date_string(const clock_time_t* time, char* buffer, size_t buffer_size) {
    snprintf(buffer, buffer_size, "%04d년 %02d월 %02d일", 
             time->year, time->month, time->day);
}

// Get day name in Korean
const char* clock_get_day_name(int day_of_week) {
    if (day_of_week >= 0 && day_of_week < 7) {
        return day_names[day_of_week];
    }
    return "알 수 없음"; // Unknown
} 