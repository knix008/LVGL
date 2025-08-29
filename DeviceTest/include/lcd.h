#ifndef LCD_H
#define LCD_H

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

// LCD interface types
typedef enum {
    LCD_INTERFACE_SPI,
    LCD_INTERFACE_I2C,
    LCD_INTERFACE_PARALLEL,
    LCD_INTERFACE_MIPI_DSI,
    LCD_INTERFACE_HDMI,
    LCD_INTERFACE_VGA,
    LCD_INTERFACE_DVI,
    LCD_INTERFACE_DISPLAYPORT
} lcd_interface_t;

// LCD test structure
typedef struct {
    lcd_interface_t interface;
    char device_path[64];
    int width;
    int height;
    int bits_per_pixel;
    bool is_connected;
    bool is_initialized;
} lcd_test_t;

// Function declarations
int init_lcd_test(lcd_test_t* lcd, lcd_interface_t interface, const char* device_path);
void cleanup_lcd_test(lcd_test_t* lcd);

// Test functions
test_result_t test_lcd_connection(lcd_test_t* lcd);
test_result_t test_lcd_initialization(lcd_test_t* lcd);
test_result_t test_lcd_resolution(lcd_test_t* lcd);
test_result_t test_lcd_color_depth(lcd_test_t* lcd);
test_result_t test_lcd_refresh_rate(lcd_test_t* lcd);
test_result_t test_lcd_brightness(lcd_test_t* lcd);
test_result_t test_lcd_contrast(lcd_test_t* lcd);
test_result_t test_lcd_color_patterns(lcd_test_t* lcd);
test_result_t test_lcd_text_rendering(lcd_test_t* lcd);
test_result_t test_lcd_image_display(lcd_test_t* lcd);
test_result_t test_lcd_touch_input(lcd_test_t* lcd);
test_result_t test_lcd_power_management(lcd_test_t* lcd);
test_result_t test_lcd_all_capabilities(lcd_test_t* lcd);

// Main test runner
test_summary_t run_all_lcd_tests(lcd_interface_t interface, const char* device_path);

// Command handler
int handle_lcd_commands(const char* test_type, lcd_interface_t interface, const char* device_path, bool interactive_mode);

#ifdef __cplusplus
}
#endif

#endif // LCD_H
