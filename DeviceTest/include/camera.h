#ifndef CAMERA_H
#define CAMERA_H

#include "common.h"

// Forward declarations for C++ types
#ifdef __cplusplus
extern "C" {
#endif

// Forward declare OpenCV types
typedef struct cv_VideoCapture cv_VideoCapture;
typedef struct cv_Mat cv_Mat;

// Resolution structure
typedef struct {
    int width;
    int height;
    int fps;
    bool supported;
    double score;
} resolution_info_t;

// Camera capability structure
typedef struct {
    int width;
    int height;
    int fps;
    int fourcc;
    const char* format_name;
    bool supported;
} camera_capability_t;

// Camera test structure
typedef struct {
    cv_VideoCapture* capture;
    bool is_running;
    int camera_index;
    int width;
    int height;
    int fps;
    resolution_info_t* resolutions;
    int resolution_count;
    int max_resolutions;
    camera_capability_t* capabilities;
    int capability_count;
    int max_capabilities;
} camera_test_t;

// Function declarations
int init_camera_test(camera_test_t* camera, int camera_index);
void cleanup_camera_test(camera_test_t* camera);
bool start_camera_test(camera_test_t* camera);
void stop_camera_test(camera_test_t* camera);
cv_Mat* capture_frame(camera_test_t* camera);
bool save_test_image(camera_test_t* camera, const char* filename);
void run_camera_test_console(camera_test_t* camera);

// Resolution testing functions
int discover_camera_resolutions(camera_test_t* camera);
bool test_resolution(camera_test_t* camera, int width, int height, int fps);
void print_supported_resolutions(camera_test_t* camera);
test_result_t test_all_resolutions(int camera_index);

// Camera capability testing functions
int discover_camera_capabilities(camera_test_t* camera);
bool test_capability(camera_test_t* camera, int width, int height, int fps, int fourcc);
void print_camera_capabilities(camera_test_t* camera);
test_result_t test_camera_capabilities(int camera_index);
test_result_t test_supported_camera_capabilities(int camera_index);

// Test functions
test_result_t test_camera_initialization(int camera_index);
test_result_t test_camera_capture(int camera_index);
test_result_t test_camera_resolution(int camera_index);
test_result_t test_camera_fps(int camera_index);

// Test suite function
test_summary_t run_all_camera_tests(int camera_index);

// Command handler function
int handle_camera_commands(const char* test_type, int camera_index, bool interactive_mode);

#ifdef __cplusplus
}
#endif

#endif // CAMERA_H
