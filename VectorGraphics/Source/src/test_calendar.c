#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "calendar.h"

// Test result tracking
static int total_tests = 0;
static int passed_tests = 0;
static int failed_tests = 0;

// Test assertion macro
#define TEST_ASSERT(condition, message, ...) do { \
    total_tests++; \
    if (condition) { \
        passed_tests++; \
        printf("✓ " message "\n", ##__VA_ARGS__); \
    } else { \
        failed_tests++; \
        printf("✗ " message "\n", ##__VA_ARGS__); \
    } \
} while(0)

// Test calendar initialization
void test_calendar_init() {
    printf("=== TESTING CALENDAR INITIALIZATION ===\n");
    
    calendar_date_t date;
    calendar_init(&date);
    
    // Test that date is initialized with current system date
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    int expected_year = tm_info->tm_year + 1900;
    int expected_month = tm_info->tm_mon + 1;
    int expected_day = tm_info->tm_mday;
    
    TEST_ASSERT(date.year == expected_year, "Year initialized correctly");
    TEST_ASSERT(date.month == expected_month, "Month initialized correctly");
    TEST_ASSERT(date.day == expected_day, "Day initialized correctly");
    
    printf("\n");
}

// Test calendar date setting
void test_calendar_set_date() {
    printf("=== TESTING CALENDAR DATE SETTING ===\n");
    
    calendar_date_t date;
    
    // Test setting valid date
    calendar_set_date(&date, 2024, 6, 15);
    TEST_ASSERT(date.year == 2024, "Year set correctly");
    TEST_ASSERT(date.month == 6, "Month set correctly");
    TEST_ASSERT(date.day == 15, "Day set correctly");
    
    // Test setting edge cases
    calendar_set_date(&date, 1900, 1, 1);
    TEST_ASSERT(date.year == 1900 && date.month == 1 && date.day == 1, "Edge case date set correctly");
    
    calendar_set_date(&date, 9999, 12, 31);
    TEST_ASSERT(date.year == 9999 && date.month == 12 && date.day == 31, "Max date set correctly");
    
    printf("\n");
}

// Test month navigation
void test_calendar_month_navigation() {
    printf("=== TESTING CALENDAR MONTH NAVIGATION ===\n");
    
    calendar_date_t date;
    
    // Test next month
    calendar_set_date(&date, 2024, 1, 15);
    calendar_navigate(&date, 1, 1);
    TEST_ASSERT(date.year == 2024 && date.month == 2 && date.day == 15, "Next month from January");
    
    // Test next month across year boundary
    calendar_set_date(&date, 2024, 12, 15);
    calendar_navigate(&date, 1, 1);
    TEST_ASSERT(date.year == 2025 && date.month == 1 && date.day == 15, "Next month across year");
    
    // Test previous month
    calendar_set_date(&date, 2024, 3, 15);
    calendar_navigate(&date, -1, 1);
    TEST_ASSERT(date.year == 2024 && date.month == 2 && date.day == 15, "Previous month to February");
    
    // Test previous month across year boundary
    calendar_set_date(&date, 2024, 1, 15);
    calendar_navigate(&date, -1, 1);
    TEST_ASSERT(date.year == 2023 && date.month == 12 && date.day == 15, "Previous month across year");
    
    // Test day adjustment for different month lengths
    calendar_set_date(&date, 2024, 1, 31);
    calendar_navigate(&date, 1, 1);
    TEST_ASSERT(date.year == 2024 && date.month == 2 && date.day == 29, "Day adjusted for February leap year");
    
    calendar_set_date(&date, 2023, 1, 31);
    calendar_navigate(&date, 1, 1);
    TEST_ASSERT(date.year == 2023 && date.month == 2 && date.day == 28, "Day adjusted for February non-leap year");
    
    printf("\n");
}

// Test year navigation
void test_calendar_year_navigation() {
    printf("=== TESTING CALENDAR YEAR NAVIGATION ===\n");
    
    calendar_date_t date;
    
    // Test next year
    calendar_set_date(&date, 2024, 6, 15);
    calendar_navigate(&date, 1, 2);
    TEST_ASSERT(date.year == 2025 && date.month == 6 && date.day == 15, "Next year");
    
    // Test previous year
    calendar_set_date(&date, 2024, 6, 15);
    calendar_navigate(&date, -1, 2);
    TEST_ASSERT(date.year == 2023 && date.month == 6 && date.day == 15, "Previous year");
    
    // Test leap year day adjustment
    calendar_set_date(&date, 2024, 2, 29);
    calendar_navigate(&date, 1, 2);
    TEST_ASSERT(date.year == 2025 && date.month == 2 && date.day == 28, "Leap year day adjustment");
    
    printf("\n");
}

// Test day navigation
void test_calendar_day_navigation() {
    printf("=== TESTING CALENDAR DAY NAVIGATION ===\n");
    
    calendar_date_t date;
    
    // Test next day within month
    calendar_set_date(&date, 2024, 6, 15);
    calendar_navigate(&date, 1, 0);
    TEST_ASSERT(date.year == 2024 && date.month == 6 && date.day == 16, "Next day within month");
    
    // Test next day across month boundary
    calendar_set_date(&date, 2024, 6, 30);
    calendar_navigate(&date, 1, 0);
    TEST_ASSERT(date.year == 2024 && date.month == 7 && date.day == 1, "Next day across month");
    
    // Test previous day within month
    calendar_set_date(&date, 2024, 6, 15);
    calendar_navigate(&date, -1, 0);
    TEST_ASSERT(date.year == 2024 && date.month == 6 && date.day == 14, "Previous day within month");
    
    // Test previous day across month boundary
    calendar_set_date(&date, 2024, 6, 1);
    calendar_navigate(&date, -1, 0);
    TEST_ASSERT(date.year == 2024 && date.month == 5 && date.day == 31, "Previous day across month");
    
    // Test day navigation across year boundary
    calendar_set_date(&date, 2024, 1, 1);
    calendar_navigate(&date, -1, 0);
    TEST_ASSERT(date.year == 2023 && date.month == 12 && date.day == 31, "Previous day across year");
    
    calendar_set_date(&date, 2024, 12, 31);
    calendar_navigate(&date, 1, 0);
    TEST_ASSERT(date.year == 2025 && date.month == 1 && date.day == 1, "Next day across year");
    
    printf("\n");
}

// Test week navigation - DISABLED (functions not in public API)
/*
void test_calendar_week_navigation() {
    printf("=== TESTING CALENDAR WEEK NAVIGATION ===\n");
    
    calendar_date_t date;
    
    // Test next week
    calendar_set_date(&date, 2024, 6, 15);
    calendar_next_week(&date);
    TEST_ASSERT(date.year == 2024 && date.month == 6 && date.day == 22, "Next week within month");
    
    // Test next week across month boundary
    calendar_set_date(&date, 2024, 6, 30);
    calendar_next_week(&date);
    TEST_ASSERT(date.year == 2024 && date.month == 7 && date.day == 7, "Next week across month");
    
    // Test previous week
    calendar_set_date(&date, 2024, 6, 15);
    calendar_prev_week(&date);
    TEST_ASSERT(date.year == 2024 && date.month == 6 && date.day == 8, "Previous week within month");
    
    // Test previous week across month boundary
    calendar_set_date(&date, 2024, 6, 1);
    calendar_prev_week(&date);
    TEST_ASSERT(date.year == 2024 && date.month == 5 && date.day == 25, "Previous week across month");
    
    // Test week navigation across year boundary
    calendar_set_date(&date, 2024, 1, 1);
    calendar_prev_week(&date);
    TEST_ASSERT(date.year == 2023 && date.month == 12 && date.day == 25, "Previous week across year");
    
    calendar_set_date(&date, 2024, 12, 31);
    calendar_next_week(&date);
    TEST_ASSERT(date.year == 2025 && date.month == 1 && date.day == 7, "Next week across year");
    
    printf("\n");
}
*/

// Test individual component setting - DISABLED (functions not in public API)
/*
void test_calendar_component_setting() {
    printf("=== TESTING CALENDAR COMPONENT SETTING ===\n");
    
    calendar_date_t date;
    calendar_set_date(&date, 2024, 6, 15);
    
    // Test setting month
    calendar_set_month(&date, 12);
    TEST_ASSERT(date.year == 2024 && date.month == 12 && date.day == 15, "Month set correctly");
    
    // Test setting month with day adjustment
    calendar_set_date(&date, 2024, 1, 31);
    calendar_set_month(&date, 2);
    TEST_ASSERT(date.year == 2024 && date.month == 2 && date.day == 29, "Month set with day adjustment for leap year");
    
    calendar_set_date(&date, 2023, 1, 31);
    calendar_set_month(&date, 2);
    TEST_ASSERT(date.year == 2023 && date.month == 2 && date.day == 28, "Month set with day adjustment for non-leap year");
    
    // Test setting year
    calendar_set_date(&date, 2024, 2, 29);
    calendar_set_year(&date, 2023);
    TEST_ASSERT(date.year == 2023 && date.month == 2 && date.day == 28, "Year set with leap year day adjustment");
    
    // Test setting day
    calendar_set_date(&date, 2024, 2, 15);
    calendar_set_day(&date, 29);
    TEST_ASSERT(date.year == 2024 && date.month == 2 && date.day == 29, "Day set correctly in leap year");
    
    calendar_set_date(&date, 2023, 2, 15);
    calendar_set_day(&date, 28);
    TEST_ASSERT(date.year == 2023 && date.month == 2 && date.day == 28, "Day set correctly in non-leap year");
    
    printf("\n");
}
*/

// Test day of week calculation
void test_calendar_day_of_week() {
    printf("=== TESTING CALENDAR DAY OF WEEK ===\n");
    
    calendar_date_t date;
    
    // Test known dates and their day of week
    calendar_set_date(&date, 2024, 1, 1);
    TEST_ASSERT(calendar_get_day_of_week(&date) == 1, "January 1, 2024 is Monday");
    
    calendar_set_date(&date, 2024, 1, 2);
    TEST_ASSERT(calendar_get_day_of_week(&date) == 2, "January 2, 2024 is Tuesday");
    
    calendar_set_date(&date, 2024, 1, 3);
    TEST_ASSERT(calendar_get_day_of_week(&date) == 3, "January 3, 2024 is Wednesday");
    
    calendar_set_date(&date, 2024, 1, 4);
    TEST_ASSERT(calendar_get_day_of_week(&date) == 4, "January 4, 2024 is Thursday");
    
    calendar_set_date(&date, 2024, 1, 5);
    TEST_ASSERT(calendar_get_day_of_week(&date) == 5, "January 5, 2024 is Friday");
    
    calendar_set_date(&date, 2024, 1, 6);
    TEST_ASSERT(calendar_get_day_of_week(&date) == 6, "January 6, 2024 is Saturday");
    
    calendar_set_date(&date, 2024, 1, 7);
    TEST_ASSERT(calendar_get_day_of_week(&date) == 0, "January 7, 2024 is Sunday");
    
    // Test leap year February 29
    calendar_set_date(&date, 2024, 2, 29);
    TEST_ASSERT(calendar_get_day_of_week(&date) == 4, "February 29, 2024 is Thursday");
    
    // Test specific date: August 1, 2025 is Friday
    calendar_set_date(&date, 2025, 8, 1);
    TEST_ASSERT(calendar_get_day_of_week(&date) == 5, "August 1, 2025 is Friday");
    
    // Test additional dates around August 1, 2025
    calendar_set_date(&date, 2025, 7, 31);
    TEST_ASSERT(calendar_get_day_of_week(&date) == 4, "July 31, 2025 is Thursday");
    
    calendar_set_date(&date, 2025, 8, 2);
    TEST_ASSERT(calendar_get_day_of_week(&date) == 6, "August 2, 2025 is Saturday");
    
    calendar_set_date(&date, 2025, 8, 3);
    TEST_ASSERT(calendar_get_day_of_week(&date) == 0, "August 3, 2025 is Sunday");
    
    calendar_set_date(&date, 2025, 8, 4);
    TEST_ASSERT(calendar_get_day_of_week(&date) == 1, "August 4, 2025 is Monday");
    
    calendar_set_date(&date, 2025, 8, 5);
    TEST_ASSERT(calendar_get_day_of_week(&date) == 2, "August 5, 2025 is Tuesday");
    
    printf("\n");
}

// Test week number calculation - DISABLED (function not in public API)
/*
void test_calendar_week_number() {
    printf("=== TESTING CALENDAR WEEK NUMBER ===\n");
    
    calendar_date_t date;
    
    // Test week 1 of 2024 (January 1-7)
    calendar_set_date(&date, 2024, 1, 1);
    TEST_ASSERT(calendar_get_week_number(&date) == 1, "January 1, 2024 is week 1");
    
    calendar_set_date(&date, 2024, 1, 7);
    TEST_ASSERT(calendar_get_week_number(&date) == 1, "January 7, 2024 is week 1");
    
    // Test week 2 of 2024 (January 8-14)
    calendar_set_date(&date, 2024, 1, 8);
    TEST_ASSERT(calendar_get_week_number(&date) == 2, "January 8, 2024 is week 2");
    
    calendar_set_date(&date, 2024, 1, 14);
    TEST_ASSERT(calendar_get_week_number(&date) == 2, "January 14, 2024 is week 2");
    
    // Test mid-year week
    calendar_set_date(&date, 2024, 6, 15);
    TEST_ASSERT(calendar_get_week_number(&date) == 24, "June 15, 2024 is week 24");
    
    // Test end of year week
    calendar_set_date(&date, 2024, 12, 31);
    TEST_ASSERT(calendar_get_week_number(&date) == 1, "December 31, 2024 is week 1 of 2025");
    
    // Test year boundary cases
    calendar_set_date(&date, 2023, 12, 31);
    TEST_ASSERT(calendar_get_week_number(&date) == 1, "December 31, 2023 is week 1 of 2024");
    
    printf("\n");
}
*/

// Test leap year detection - DISABLED (function not in public API)
/*
void test_calendar_leap_year() {
    printf("=== TESTING CALENDAR LEAP YEAR DETECTION ===\n");
    
    // Test leap years
    TEST_ASSERT(calendar_is_leap_year(2000), "Year 2000 is leap year");
    TEST_ASSERT(calendar_is_leap_year(2004), "Year 2004 is leap year");
    TEST_ASSERT(calendar_is_leap_year(2008), "Year 2008 is leap year");
    TEST_ASSERT(calendar_is_leap_year(2012), "Year 2012 is leap year");
    TEST_ASSERT(calendar_is_leap_year(2016), "Year 2016 is leap year");
    TEST_ASSERT(calendar_is_leap_year(2020), "Year 2020 is leap year");
    TEST_ASSERT(calendar_is_leap_year(2024), "Year 2024 is leap year");
    
    // Test non-leap years
    TEST_ASSERT(!calendar_is_leap_year(2001), "Year 2001 is not leap year");
    TEST_ASSERT(!calendar_is_leap_year(2002), "Year 2002 is not leap year");
    TEST_ASSERT(!calendar_is_leap_year(2003), "Year 2003 is not leap year");
    TEST_ASSERT(!calendar_is_leap_year(2023), "Year 2023 is not leap year");
    TEST_ASSERT(!calendar_is_leap_year(2025), "Year 2025 is not leap year");
    
    // Test century rules
    TEST_ASSERT(!calendar_is_leap_year(1900), "Year 1900 is not leap year (century rule)");
    TEST_ASSERT(!calendar_is_leap_year(2100), "Year 2100 is not leap year (century rule)");
    TEST_ASSERT(calendar_is_leap_year(2000), "Year 2000 is leap year (400-year rule)");
    TEST_ASSERT(calendar_is_leap_year(2400), "Year 2400 is leap year (400-year rule)");
    
    printf("\n");
}
*/

// Test days in month calculation - DISABLED (function not in public API)
/*
void test_calendar_days_in_month() {
    printf("=== TESTING CALENDAR DAYS IN MONTH ===\n");
    
    // Test regular months
    TEST_ASSERT(calendar_get_days_in_month(2024, 1) == 31, "January has 31 days");
    TEST_ASSERT(calendar_get_days_in_month(2024, 3) == 31, "March has 31 days");
    TEST_ASSERT(calendar_get_days_in_month(2024, 4) == 30, "April has 30 days");
    TEST_ASSERT(calendar_get_days_in_month(2024, 5) == 31, "May has 31 days");
    TEST_ASSERT(calendar_get_days_in_month(2024, 6) == 30, "June has 30 days");
    TEST_ASSERT(calendar_get_days_in_month(2024, 7) == 31, "July has 31 days");
    TEST_ASSERT(calendar_get_days_in_month(2024, 8) == 31, "August has 31 days");
    TEST_ASSERT(calendar_get_days_in_month(2024, 9) == 30, "September has 30 days");
    TEST_ASSERT(calendar_get_days_in_month(2024, 10) == 31, "October has 31 days");
    TEST_ASSERT(calendar_get_days_in_month(2024, 11) == 30, "November has 30 days");
    TEST_ASSERT(calendar_get_days_in_month(2024, 12) == 31, "December has 31 days");
    
    // Test February in leap years
    TEST_ASSERT(calendar_get_days_in_month(2024, 2) == 29, "February 2024 has 29 days (leap year)");
    TEST_ASSERT(calendar_get_days_in_month(2000, 2) == 29, "February 2000 has 29 days (leap year)");
    
    // Test February in non-leap years
    TEST_ASSERT(calendar_get_days_in_month(2023, 2) == 28, "February 2023 has 28 days (non-leap year)");
    TEST_ASSERT(calendar_get_days_in_month(1900, 2) == 28, "February 1900 has 28 days (non-leap year)");
    
    printf("\n");
}
*/

// Test month name retrieval
void test_calendar_month_names() {
    printf("=== TESTING CALENDAR MONTH NAMES ===\n");
    
    const char* expected_names[] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
                                   "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
    
    for (int i = 1; i <= 12; i++) {
        const char* name = calendar_get_month_name(i);
        TEST_ASSERT(strcmp(name, expected_names[i-1]) == 0, 
                   "Month name for month %d is correct", i);
    }
    
    // Test invalid month
    const char* invalid_name = calendar_get_month_name(0);
    TEST_ASSERT(strcmp(invalid_name, "Unknown") == 0, "Invalid month returns 'Unknown'");
    
    invalid_name = calendar_get_month_name(13);
    TEST_ASSERT(strcmp(invalid_name, "Unknown") == 0, "Invalid month returns 'Unknown'");
    
    printf("\n");
}

// Test day name retrieval
void test_calendar_day_names() {
    printf("=== TESTING CALENDAR DAY NAMES ===\n");
    
    const char* expected_names[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    
    for (int i = 0; i <= 6; i++) {
        const char* name = calendar_get_day_name(i);
        TEST_ASSERT(strcmp(name, expected_names[i]) == 0, 
                   "Day name for day %d is correct", i);
    }
    
    // Test invalid day
    const char* invalid_name = calendar_get_day_name(-1);
    TEST_ASSERT(strcmp(invalid_name, "Unknown") == 0, "Invalid day returns 'Unknown'");
    
    invalid_name = calendar_get_day_name(7);
    TEST_ASSERT(strcmp(invalid_name, "Unknown") == 0, "Invalid day returns 'Unknown'");
    
    printf("\n");
}

// Test date formatting
void test_calendar_date_formatting() {
    printf("=== TESTING CALENDAR DATE FORMATTING ===\n");
    
    calendar_date_t date;
    char buffer[64];
    
    // Test full date formatting
    calendar_set_date(&date, 2024, 6, 15);
    calendar_format_date_string(&date, buffer, sizeof(buffer));
    TEST_ASSERT(strcmp(buffer, "Jun 15, 2024") == 0, "Date formatting for Jun 15, 2024");
    
    calendar_set_date(&date, 2023, 12, 31);
    calendar_format_date_string(&date, buffer, sizeof(buffer));
    TEST_ASSERT(strcmp(buffer, "Dec 31, 2023") == 0, "Date formatting for Dec 31, 2023");
    
    // Test month/year formatting - DISABLED (function not in public API)
    // calendar_set_date(&date, 2024, 6, 15);
    // calendar_format_month_year_string(&date, buffer, sizeof(buffer));
    // TEST_ASSERT(strcmp(buffer, "Jun 2024") == 0, "Month/Year formatting for Jun 2024");
    
    // calendar_set_date(&date, 2023, 12, 31);
    // calendar_format_month_year_string(&date, buffer, sizeof(buffer));
    // TEST_ASSERT(strcmp(buffer, "Dec 2023") == 0, "Month/Year formatting for Dec 2023");
    
    // Test week formatting - DISABLED (function not in public API)
    // calendar_set_date(&date, 2024, 1, 1);
    // calendar_format_week_string(&date, buffer, sizeof(buffer));
    // TEST_ASSERT(strcmp(buffer, "Week 1, Mon") == 0, "Week formatting for Jan 1, 2024");
    
    // calendar_set_date(&date, 2024, 6, 15);
    // calendar_format_week_string(&date, buffer, sizeof(buffer));
    // TEST_ASSERT(strcmp(buffer, "Week 24, Sat") == 0, "Week formatting for Jun 15, 2024");
    
    printf("\n");
}

// Test edge cases and boundary conditions
void test_calendar_edge_cases() {
    printf("=== TESTING CALENDAR EDGE CASES ===\n");
    
    calendar_date_t date;
    
    // Test year boundaries
    calendar_set_date(&date, 1900, 1, 1);
    calendar_navigate(&date, -1, 2);
    TEST_ASSERT(date.year == 1899, "Year boundary test - previous year from 1900");
    
    calendar_set_date(&date, 9999, 12, 31);
    calendar_navigate(&date, 1, 2);
    TEST_ASSERT(date.year == 10000, "Year boundary test - next year from 9999");
    
    // Test month boundaries with different day counts
    calendar_set_date(&date, 2024, 1, 31);
    calendar_navigate(&date, 1, 1);
    TEST_ASSERT(date.month == 2 && date.day == 29, "Month boundary with day adjustment (leap year)");
    
    calendar_set_date(&date, 2023, 1, 31);
    calendar_navigate(&date, 1, 1);
    TEST_ASSERT(date.month == 2 && date.day == 28, "Month boundary with day adjustment (non-leap year)");
    
    // Test day boundaries
    calendar_set_date(&date, 2024, 2, 29);
    calendar_navigate(&date, 1, 0);
    TEST_ASSERT(date.month == 3 && date.day == 1, "Day boundary across leap year February");
    
    calendar_set_date(&date, 2023, 2, 28);
    calendar_navigate(&date, 1, 0);
    TEST_ASSERT(date.month == 3 && date.day == 1, "Day boundary across non-leap year February");
    
    // Test week boundaries - DISABLED (function not in public API)
    // calendar_set_date(&date, 2024, 12, 31);
    // calendar_next_week(&date);
    // TEST_ASSERT(date.year == 2025 && date.month == 1 && date.day == 7, "Week boundary across year");
    
    printf("\n");
}

int main() {
    printf("========================\n");
    printf("=== CALENDAR TEST SUITE ===\n");
    printf("Testing calendar.c functionality\n\n");
    
    test_calendar_init();
    test_calendar_set_date();
    test_calendar_month_navigation();
    test_calendar_year_navigation();
    test_calendar_day_navigation();
    // test_calendar_week_navigation(); // DISABLED - functions not in public API
    // test_calendar_component_setting(); // DISABLED - functions not in public API
    test_calendar_day_of_week();
    // test_calendar_week_number(); // DISABLED - function not in public API
    // test_calendar_leap_year(); // DISABLED - function not in public API
    // test_calendar_days_in_month(); // DISABLED - function not in public API
    test_calendar_month_names();
    test_calendar_day_names();
    test_calendar_date_formatting();
    test_calendar_edge_cases();
    
    printf("=== TEST SUMMARY ===\n");
    printf("Total tests: %d\n", total_tests);
    printf("Passed: %d\n", passed_tests);
    printf("Failed: %d\n", failed_tests);
    printf("Success rate: %.1f%%\n", (float)passed_tests / total_tests * 100.0);
    
    if (failed_tests == 0) {
        printf("✓ ALL TESTS PASSED!\n");
    } else {
        printf("✗ %d TESTS FAILED!\n", failed_tests);
    }
    
    printf("========================\n");
    printf("Calendar test completed!\n");
    
    return (failed_tests == 0) ? 0 : 1;
} 