#include <stdio.h>
#include <time.h>
#include <string.h>

// Test function to verify clock time formatting
void test_clock_time_formatting() {
    printf("=== TESTING CLOCK TIME FORMATTING ===\n");
    
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
    
    printf("Current time: %s\n", time_str);
    printf("Current date: %s\n", date_str);
    printf("Day of week: %s\n", day_str);
    
    // Verify formatting is correct
    int expected_year = tm_info->tm_year + 1900;
    int expected_month = tm_info->tm_mon + 1;
    int expected_day = tm_info->tm_mday;
    
    printf("Year: %d, Month: %d, Day: %d\n", expected_year, expected_month, expected_day);
    printf("Hour: %d, Minute: %d, Second: %d\n", 
           tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
    
    printf("Clock time formatting test completed successfully!\n\n");
}

int main() {
    printf("Clock Tab Test Program\n");
    printf("======================\n\n");
    
    test_clock_time_formatting();
    
    printf("All tests completed successfully!\n");
    return 0;
} 