/**
 * @file gui.h
 * GUI components and interface
 */

#ifndef GUI_H
#define GUI_H

#include "lvgl/lvgl.h"
#include "face_detection.h"
#include <stdint.h>

/**
 * Initialize the GUI
 * @return 0 on success, -1 on failure
 */
int gui_init(void);

/**
 * Update camera preview with new frame
 * @param frame_data Pointer to RGB24 frame data
 */
void gui_update_camera_preview(uint8_t *frame_data);

/**
 * Update camera preview with new frame and face detection results
 * @param frame_data Pointer to RGB24 frame data
 * @param faces Pointer to face detection results
 */
void gui_update_camera_preview_with_faces(uint8_t *frame_data, const FaceDetectionResult *faces);

/**
 * Update status message
 * @param message Status message to display
 */
void gui_update_status(const char *message);

/**
 * Update photo count
 * @param count Number of photos taken
 */
void gui_update_photo_count(int count);

/**
 * Set callback for capture button
 * @param callback Function to call when capture button is clicked
 * @param user_data User data to pass to callback
 */
void gui_set_capture_callback(void (*callback)(void *), void *user_data);

/**
 * Get the canvas object for camera preview
 * @return Pointer to canvas object
 */
lv_obj_t *gui_get_camera_canvas(void);

/**
 * Show white flash overlay (for brighter photos)
 * @param duration_ms Duration in milliseconds to show the flash
 */
void gui_show_flash(uint32_t duration_ms);

/**
 * Play shutter sound effect
 */
void gui_play_shutter_sound(void);

/**
 * Signal that GUI is exiting - prevents GUI updates after cleanup starts
 */
void gui_signal_exit(void);

/**
 * Cleanup GUI resources
 */
void gui_cleanup(void);

#endif // GUI_H
