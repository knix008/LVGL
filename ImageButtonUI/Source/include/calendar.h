#ifndef CALENDAR_H
#define CALENDAR_H

#include <time.h>

// Calendar state structure
typedef struct {
    int year;
    int month;
    int day;
} calendar_date_t;

// Calendar logic functions - simplified API
void calendar_init(calendar_date_t* date);
void calendar_set_date(calendar_date_t* date, int year, int month, int day);

// Unified navigation function
// direction: -1 for previous, +1 for next
// unit: 0=day, 1=month, 2=year
void calendar_navigate(calendar_date_t* date, int direction, int unit);

// Utility functions
int calendar_get_day_of_week(const calendar_date_t* date);
const char* calendar_get_month_name(int month);
const char* calendar_get_day_name(int day_of_week);
void calendar_format_date_string(const calendar_date_t* date, char* buffer, size_t buffer_size);

#endif // CALENDAR_H 