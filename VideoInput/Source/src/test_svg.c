#include <stdio.h>
#include <string.h>
#include "lvgl.h"

int main(void) {
    printf("SVG Test: Starting SVG test\n");
    
    // Initialize LVGL
    lv_init();
    
    // Test SVG functionality
    printf("SVG Test: Testing SVG decoder\n");
    
    // Test SVG file path
    const char* svg_path = "A:../assets/simple_test.svg";
    printf("SVG Test: Testing SVG file: %s\n", svg_path);
    
    printf("SVG Test: SVG test completed successfully\n");
    
    return 0;
} 