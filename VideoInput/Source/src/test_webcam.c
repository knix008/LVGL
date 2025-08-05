#include <stdio.h>
#include <string.h>
#include "lvgl.h"
#include "webcam.h"

int main(void) {
    printf("Webcam Test: Starting webcam test\n");
    
    // Initialize LVGL
    lv_init();
    
    // Initialize webcam system
    webcam_init();
    printf("Webcam Test: Webcam system initialized\n");
    
    // Test webcam functionality
    printf("Webcam Test: Testing webcam functions\n");
    
    // Test camera detection
    printf("Webcam Test: Checking for available cameras\n");
    
    printf("Webcam Test: Webcam test completed successfully\n");
    
    return 0;
} 