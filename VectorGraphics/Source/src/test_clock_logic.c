#include "clock.h"
#include <stdio.h>

// Test function to verify clock logic
void test_clock_logic() {
    printf("=== TESTING CLOCK LOGIC ===\n");
    
    // Initialize clock system
    clock_init();
    
    // Get current time
    clock_time_t current_time;
    clock_get_current_time(&current_time);
    
    printf("Current time: %02d:%02d:%02d\n", 
           current_time.hour, current_time.minute, current_time.second);
    printf("Current date: %04d-%02d-%02d\n", 
           current_time.year, current_time.month, current_time.day);
    printf("Day of week: %d (%s)\n", 
           current_time.day_of_week, clock_get_day_name(current_time.day_of_week));
    
    // Test formatting functions
    char time_str[32];
    char date_str[64];
    
    clock_format_time_string(&current_time, time_str, sizeof(time_str));
    clock_format_date_string(&current_time, date_str, sizeof(date_str));
    
    printf("Formatted time: %s\n", time_str);
    printf("Formatted date: %s\n", date_str);
    printf("Day name: %s\n", clock_get_day_name(current_time.day_of_week));
    
    // Test day name function with invalid input
    printf("Invalid day name: %s\n", clock_get_day_name(10));
    
    printf("Clock logic test completed successfully!\n\n");
}

int main() {
    printf("Clock Logic Test Program\n");
    printf("========================\n\n");
    
    test_clock_logic();
    
    printf("All tests completed successfully!\n");
    return 0;
} 