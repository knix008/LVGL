#ifndef CLOCK_H
#define CLOCK_H

#include <time.h>

// Clock time structure
typedef struct {
    int hour;
    int minute;
    int second;
    int year;
    int month;
    int day;
    int day_of_week;
} clock_time_t;

// Clock functions
void clock_init(void);
void clock_get_current_time(clock_time_t* time);
void clock_format_time_string(const clock_time_t* time, char* buffer, size_t buffer_size);
void clock_format_date_string(const clock_time_t* time, char* buffer, size_t buffer_size);
const char* clock_get_day_name(int day_of_week);

#endif // CLOCK_H 