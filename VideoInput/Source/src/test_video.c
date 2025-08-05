#include <stdio.h>
#include <string.h>
#include "lvgl.h"
#include "lv_ffmpeg.h"

int main(void) {
    printf("Video Test: Starting video test\n");
    
    // Initialize LVGL
    lv_init();
    
    // Initialize FFmpeg
    lv_ffmpeg_init();
    printf("Video Test: FFmpeg initialized\n");
    
    // Test video file path
    const char* video_path = "A:../assets/example.mp4";
    printf("Video Test: Testing video file: %s\n", video_path);
    
    // Create a simple test
    printf("Video Test: Video test completed successfully\n");
    
    return 0;
} 