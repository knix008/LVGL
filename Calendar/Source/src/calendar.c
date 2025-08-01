#include "calendar.h"
#include <string.h>
#include <stdio.h>
#include <time.h>

// Initialize calendar with current date
void calendar_init(calendar_date_t* date) {
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    
    date->year = tm_info->tm_year + 1900;
    date->month = tm_info->tm_mon + 1;
    date->day = tm_info->tm_mday;
}

// Set specific date
void calendar_set_date(calendar_date_t* date, int year, int month, int day) {
    date->year = year;
    date->month = month;
    date->day = day;
}

// Get current system date
void calendar_get_current_date(calendar_date_t* date) {
    calendar_init(date);
}

// Navigate to previous month
void calendar_prev_month(calendar_date_t* date) {
    date->month--;
    if (date->month < 1) {
        date->month = 12;
        date->year--;
    }
    
    // Adjust day if it exceeds the new month's days
    int days_in_month = calendar_get_days_in_month(date->year, date->month);
    if (date->day > days_in_month) {
        date->day = days_in_month;
    }
}

// Navigate to next month
void calendar_next_month(calendar_date_t* date) {
    date->month++;
    if (date->month > 12) {
        date->month = 1;
        date->year++;
    }
    
    // Adjust day if it exceeds the new month's days
    int days_in_month = calendar_get_days_in_month(date->year, date->month);
    if (date->day > days_in_month) {
        date->day = days_in_month;
    }
}

// Navigate to previous year
void calendar_prev_year(calendar_date_t* date) {
    date->year--;
    
    // Adjust day for leap year changes
    int days_in_month = calendar_get_days_in_month(date->year, date->month);
    if (date->day > days_in_month) {
        date->day = days_in_month;
    }
}

// Navigate to next year
void calendar_next_year(calendar_date_t* date) {
    date->year++;
    
    // Adjust day for leap year changes
    int days_in_month = calendar_get_days_in_month(date->year, date->month);
    if (date->day > days_in_month) {
        date->day = days_in_month;
    }
}

// Navigate to previous day
void calendar_prev_day(calendar_date_t* date) {
    date->day--;
    if (date->day < 1) {
        // Go to previous month
        calendar_prev_month(date);
        date->day = calendar_get_days_in_month(date->year, date->month);
    }
}

// Navigate to next day
void calendar_next_day(calendar_date_t* date) {
    int days_in_month = calendar_get_days_in_month(date->year, date->month);
    date->day++;
    if (date->day > days_in_month) {
        // Go to next month
        calendar_next_month(date);
        date->day = 1;
    }
}

// Navigate to previous week
void calendar_prev_week(calendar_date_t* date) {
    for (int i = 0; i < 7; i++) {
        calendar_prev_day(date);
    }
}

// Navigate to next week
void calendar_next_week(calendar_date_t* date) {
    for (int i = 0; i < 7; i++) {
        calendar_next_day(date);
    }
}

// Set specific month
void calendar_set_month(calendar_date_t* date, int month) {
    if (month >= 1 && month <= 12) {
        date->month = month;
        // Adjust day if it exceeds the new month's days
        int days_in_month = calendar_get_days_in_month(date->year, date->month);
        if (date->day > days_in_month) {
            date->day = days_in_month;
        }
    }
}

// Set specific year
void calendar_set_year(calendar_date_t* date, int year) {
    if (year >= 1900 && year <= 9999) {
        date->year = year;
        // Adjust day for leap year changes
        int days_in_month = calendar_get_days_in_month(date->year, date->month);
        if (date->day > days_in_month) {
            date->day = days_in_month;
        }
    }
}

// Set specific day
void calendar_set_day(calendar_date_t* date, int day) {
    int days_in_month = calendar_get_days_in_month(date->year, date->month);
    if (day >= 1 && day <= days_in_month) {
        date->day = day;
    }
}

// Set specific week (ISO 8601 week number)
void calendar_set_week(calendar_date_t* date, int week_number) {
    if (week_number < 1 || week_number > 53) {
        return; // Invalid week number
    }
    
    // Calculate the first week of the year
    // ISO 8601: Week 1 is the week containing January 4th
    int days_to_add = (week_number - 1) * 7;
    
    // Find January 4th
    calendar_date_t jan4 = {date->year, 1, 4};
    int jan4_dow = calendar_get_day_of_week(&jan4);
    
    // Calculate the Monday of week 1
    int monday_offset = jan4_dow - 1; // 0 = Monday, 1 = Tuesday, etc.
    if (monday_offset < 0) monday_offset += 7;
    
    // Calculate the target date
    int target_day = 4 - monday_offset + days_to_add;
    
    // Set the date
    if (target_day <= 0) {
        // Previous year
        calendar_set_date(date, date->year - 1, 12, 31 + target_day);
    } else if (target_day > calendar_get_days_in_month(date->year, 12)) {
        // Next year
        calendar_set_date(date, date->year + 1, 1, target_day - calendar_get_days_in_month(date->year, 12));
    } else {
        // Same year
        int month = 1;
        while (target_day > calendar_get_days_in_month(date->year, month)) {
            target_day -= calendar_get_days_in_month(date->year, month);
            month++;
        }
        calendar_set_date(date, date->year, month, target_day);
    }
}

// Get number of days in a month
int calendar_get_days_in_month(int year, int month) {
    const int days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    if (month == 2 && calendar_is_leap_year(year)) {
        return 29;
    }
    
    return days_in_month[month - 1];
}

// Check if year is leap year
int calendar_is_leap_year(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// Get day of week (0 = Sunday, 1 = Monday, ..., 6 = Saturday)
int calendar_get_day_of_week(const calendar_date_t* date) {
    // Use a simpler algorithm based on known anchor dates
    // January 1, 1900 was a Monday (day 1)
    
    // Calculate days since January 1, 1900
    int days = 0;
    
    // Add days from years
    for (int year = 1900; year < date->year; year++) {
        days += calendar_is_leap_year(year) ? 366 : 365;
    }
    
    // Add days from months in current year
    for (int month = 1; month < date->month; month++) {
        days += calendar_get_days_in_month(date->year, month);
    }
    
    // Add days in current month
    days += date->day - 1;
    
    // January 1, 1900 was Monday (day 1), so we need to adjust
    // Since we want 0 = Sunday, 1 = Monday, ..., 6 = Saturday
    return (days + 1) % 7;
}

// Get ISO 8601 week number
int calendar_get_week_number(const calendar_date_t* date) {
    // Find January 4th of the year
    calendar_date_t jan4 = {date->year, 1, 4};
    int jan4_dow = calendar_get_day_of_week(&jan4);
    
    // Calculate the Monday of week 1
    int monday_offset = jan4_dow - 1; // 0 = Monday, 1 = Tuesday, etc.
    if (monday_offset < 0) monday_offset += 7;
    
    // Find the Monday of week 1
    int week1_monday_day = 4 - monday_offset;
    
    // Calculate days since January 1st
    int days_since_jan1 = 0;
    for (int month = 1; month < date->month; month++) {
        days_since_jan1 += calendar_get_days_in_month(date->year, month);
    }
    days_since_jan1 += date->day - 1;
    
    // Calculate days since week 1 Monday
    int days_since_week1 = days_since_jan1 - (week1_monday_day - 1);
    
    // Calculate week number
    int week_number = (days_since_week1 / 7) + 1;
    
    // Handle edge cases for year boundaries
    if (date->month == 12 && date->day >= 29) {
        // Check if this date belongs to next year's week 1
        calendar_date_t next_jan4 = {date->year + 1, 1, 4};
        int next_jan4_dow = calendar_get_day_of_week(&next_jan4);
        int next_monday_offset = next_jan4_dow - 1;
        if (next_monday_offset < 0) next_monday_offset += 7;
        
        int next_week1_monday_day = 4 - next_monday_offset;
        
        // Calculate days to next year's week 1 Monday
        int days_in_current_year = 0;
        for (int month = 1; month <= 12; month++) {
            days_in_current_year += calendar_get_days_in_month(date->year, month);
        }
        
        int days_to_next_week1 = days_in_current_year - days_since_jan1 + next_week1_monday_day - 1;
        
        if (days_to_next_week1 < 4) {
            return 1; // Week 1 of next year
        }
    }
    
    return week_number;
}

// Get month name
const char* calendar_get_month_name(int month) {
    const char* months[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                           "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    
    if (month >= 1 && month <= 12) {
        return months[month - 1];
    }
    
    return "Unknown";
}

// Get day name
const char* calendar_get_day_name(int day_of_week) {
    const char* days[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    
    if (day_of_week >= 0 && day_of_week <= 6) {
        return days[day_of_week];
    }
    
    return "Unknown";
}

// Format date as string
void calendar_format_date_string(const calendar_date_t* date, char* buffer, size_t buffer_size) {
    const char* month_name = calendar_get_month_name(date->month);
    snprintf(buffer, buffer_size, "%s %d, %d", month_name, date->day, date->year);
}

// Format month and year as string
void calendar_format_month_year_string(const calendar_date_t* date, char* buffer, size_t buffer_size) {
    const char* month_name = calendar_get_month_name(date->month);
    snprintf(buffer, buffer_size, "%s %d", month_name, date->year);
}

// Format week information as string
void calendar_format_week_string(const calendar_date_t* date, char* buffer, size_t buffer_size) {
    int week_number = calendar_get_week_number(date);
    int day_of_week = calendar_get_day_of_week(date);
    const char* day_name = calendar_get_day_name(day_of_week);
    snprintf(buffer, buffer_size, "Week %d, %s", week_number, day_name);
} 