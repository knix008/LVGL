#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "webcam.h"

// Webcam device paths to try
static const char* webcam_devices[] = {
    "/dev/video0",
    "/dev/video1", 
    "/dev/video2",
    "/dev/video3",
    "/dev/video4",
    NULL
};

// Webcam context
static webcam_context_t g_webcam_ctx = {0};

// Function to find available webcam device
static const char* find_webcam_device(void) {
    for (int i = 0; webcam_devices[i] != NULL; i++) {
        if (access(webcam_devices[i], F_OK) == 0) {
            printf("Webcam: Found device: %s\n", webcam_devices[i]);
            return webcam_devices[i];
        }
    }
    printf("Webcam: No webcam devices found\n");
    return NULL;
}

// Initialize webcam system
void webcam_init(void) {
    memset(&g_webcam_ctx, 0, sizeof(webcam_context_t));
    printf("Webcam: System initialized\n");
}

// Check if webcam is available
bool webcam_is_available(void) {
    return find_webcam_device() != NULL;
}

// Get current webcam device path
const char* webcam_get_device_path(void) {
    return g_webcam_ctx.device_path;
}

// Get webcam capture status
bool webcam_is_capturing(void) {
    return g_webcam_ctx.is_capturing;
}

// Get webcam configuration
void webcam_get_config(int* width, int* height, int* fps) {
    if (width) *width = g_webcam_ctx.width;
    if (height) *height = g_webcam_ctx.height;
    if (fps) *fps = g_webcam_ctx.fps;
}

// Set webcam configuration
void webcam_set_config(int width, int height, int fps) {
    g_webcam_ctx.width = width;
    g_webcam_ctx.height = height;
    g_webcam_ctx.fps = fps;
    printf("Webcam: Configuration set to %dx%d@%dfps\n", width, height, fps);
}

// Create FFmpeg input URL for webcam
char* webcam_create_input_url(const char* device, int width, int height, int fps) {
    static char input_url[512];
    snprintf(input_url, sizeof(input_url), 
             "v4l2://%s?video_size=%dx%d&framerate=%d/1", 
             device, width, height, fps);
    return input_url;
}

// Start webcam capture (logic only)
bool webcam_start_capture_logic(const char* device, int width, int height, int fps) {
    if (device == NULL) {
        printf("Webcam: No webcam device available\n");
        return false;
    }
    
    // Store webcam context
    strcpy(g_webcam_ctx.device_path, device);
    g_webcam_ctx.width = width;
    g_webcam_ctx.height = height;
    g_webcam_ctx.fps = fps;
    
    // Create FFmpeg input URL for webcam
    char* input_url = webcam_create_input_url(device, width, height, fps);
    printf("Webcam: Starting capture with URL: %s\n", input_url);
    
    // Store the input URL for external use
    strcpy(g_webcam_ctx.input_url, input_url);
    g_webcam_ctx.is_capturing = true;
    
    printf("Webcam: Capture logic prepared successfully\n");
    return true;
}

// Stop webcam capture (logic only)
void webcam_stop_capture_logic(void) {
    g_webcam_ctx.is_capturing = false;
    printf("Webcam: Capture logic stopped\n");
}

// Get the prepared input URL
const char* webcam_get_input_url(void) {
    return g_webcam_ctx.input_url;
}

// Get webcam status string
const char* webcam_get_status_string(void) {
    if (!webcam_is_available()) {
        return "Webcam: Not Available";
    } else if (webcam_is_capturing()) {
        return "Webcam: Active";
    } else {
        return "Webcam: Ready";
    }
} 