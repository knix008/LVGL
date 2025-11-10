/**
 * @file gui.h
 * GUI components and interface
 */

#ifndef GUI_H
#define GUI_H

#include "lvgl/lvgl.h"
#include "face_detection.h"
#include <stdint.h>
#include <stdbool.h>

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
 * Show recognition result as popup
 * @param person_name Name of recognized person
 * @param confidence Recognition confidence (0-1)
 */
void gui_show_recognition_popup(const char *person_name, float confidence);

/**
 * Show registration screen
 * @param person_name Name of the person being registered
 */
void gui_show_registration_screen(const char *person_name);

/**
 * Hide registration screen
 */
void gui_hide_registration_screen(void);

/**
 * Get if registration screen is visible
 */
bool gui_is_registration_screen_visible(void);

/**
 * Set callback for register button
 * @param callback Function to call when register button is clicked
 * @param user_data User data to pass to callback
 */
void gui_set_register_callback(void (*callback)(void *), void *user_data);

/**
 * Set callback for recognize button
 * @param callback Function to call when recognize button is clicked
 * @param user_data User data to pass to callback
 */
void gui_set_recognize_callback(void (*callback)(void *), void *user_data);

/**
 * Get the person name from registration screen
 * @return Pointer to person name string (valid while registration screen is open)
 */
const char *gui_get_registration_name(void);

/**
 * Set callback for when registration screen is shown
 * @param callback Function to call when registration screen is displayed
 * @param user_data User data to pass to callback
 */
void gui_set_registration_shown_callback(void (*callback)(void *), void *user_data);

/**
 * Cleanup GUI resources
 */
void gui_cleanup(void);

#endif // GUI_H
