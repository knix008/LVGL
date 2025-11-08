/**
 * @file camera.h
 * Camera capture logic using OpenCV
 */

#ifndef CAMERA_H
#define CAMERA_H

#include <stdint.h>
#include <stdbool.h>

// Camera configuration
#define VIDEO_DEVICE "/dev/video0"
#define CAMERA_WIDTH  320
#define CAMERA_HEIGHT 240
#define BUFFER_COUNT 4

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialize the camera device
 * @return 0 on success, -1 on failure
 */
int camera_init(void);

/**
 * Cleanup camera resources
 */
void camera_cleanup(void);

/**
 * Start camera capture thread
 * @return 0 on success, -1 on failure
 */
int camera_start(void);

/**
 * Stop camera capture thread
 */
void camera_stop(void);

/**
 * Get pointer to the latest camera frame data
 * @return Pointer to RGB24 frame data (CAMERA_WIDTH * CAMERA_HEIGHT * 3 bytes)
 */
uint8_t *camera_get_frame(void);

/**
 * Check if camera is running
 * @return true if camera is capturing, false otherwise
 */
bool camera_is_running(void);

/**
 * Save current frame to file
 * @param filename Output filename (will be .ppm format)
 * @return 0 on success, -1 on failure
 */
int camera_save_photo(const char *filename);

/**
 * Get camera frame dimensions
 * @param width Output pointer for frame width
 * @param height Output pointer for frame height
 */
void camera_get_dimensions(int *width, int *height);

#ifdef __cplusplus
}
#endif

#endif // CAMERA_H
