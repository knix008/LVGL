#ifndef WEBCAM_H
#define WEBCAM_H

#include <stdbool.h>

// Webcam context structure
typedef struct {
    char device_path[256];
    char input_url[512];
    bool is_capturing;
    int width;
    int height;
    int fps;
} webcam_context_t;

// Webcam initialization and configuration
void webcam_init(void);
bool webcam_is_available(void);
const char* webcam_get_device_path(void);
bool webcam_is_capturing(void);
void webcam_get_config(int* width, int* height, int* fps);
void webcam_set_config(int width, int height, int fps);

// Webcam capture logic (no GUI dependencies)
bool webcam_start_capture_logic(const char* device, int width, int height, int fps);
void webcam_stop_capture_logic(void);
const char* webcam_get_input_url(void);
const char* webcam_get_status_string(void);

// Utility functions
char* webcam_create_input_url(const char* device, int width, int height, int fps);

#endif // WEBCAM_H 