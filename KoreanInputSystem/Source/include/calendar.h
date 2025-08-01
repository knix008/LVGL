#ifndef CALENDAR_H
#define CALENDAR_H

#include <time.h>

// Calendar state structure
typedef struct {
    int year;
    int month;
    int day;
} calendar_date_t;

// Calendar logic functions
void calendar_init(calendar_date_t* date);
void calendar_set_date(calendar_date_t* date, int year, int month, int day);
void calendar_get_current_date(calendar_date_t* date);
void calendar_prev_month(calendar_date_t* date);
void calendar_next_month(calendar_date_t* date);
void calendar_prev_year(calendar_date_t* date);
void calendar_next_year(calendar_date_t* date);
void calendar_prev_day(calendar_date_t* date);
void calendar_next_day(calendar_date_t* date);
void calendar_prev_week(calendar_date_t* date);
void calendar_next_week(calendar_date_t* date);
void calendar_set_month(calendar_date_t* date, int month);
void calendar_set_year(calendar_date_t* date, int year);
void calendar_set_day(calendar_date_t* date, int day);
void calendar_set_week(calendar_date_t* date, int week_number);
int calendar_get_days_in_month(int year, int month);
int calendar_is_leap_year(int year);
int calendar_get_week_number(const calendar_date_t* date);
int calendar_get_day_of_week(const calendar_date_t* date);
const char* calendar_get_month_name(int month);
const char* calendar_get_day_name(int day_of_week);
void calendar_format_date_string(const calendar_date_t* date, char* buffer, size_t buffer_size);
void calendar_format_month_year_string(const calendar_date_t* date, char* buffer, size_t buffer_size);
void calendar_format_week_string(const calendar_date_t* date, char* buffer, size_t buffer_size);

#endif // CALENDAR_H 