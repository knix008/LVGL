/**
 * @file audio_gui.h
 * @brief Audio player GUI interface
 */

#ifndef AUDIO_GUI_H
#define AUDIO_GUI_H

#include "lvgl/lvgl.h"

/**
 * @brief Create the audio player GUI
 * @return Pointer to the main screen object
 */
lv_obj_t *audio_gui_create(void);

/**
 * @brief Update the playback position display
 * @param position_ms Current position in milliseconds
 * @param duration_ms Total duration in milliseconds
 */
void audio_gui_update_position(uint32_t position_ms, uint32_t duration_ms);

/**
 * @brief Update the playback state display
 * @param is_playing true if playing, false if paused/stopped
 */
void audio_gui_update_state(bool is_playing);

/**
 * @brief Update the track information display
 * @param title Song title
 * @param artist Artist name
 * @param album Album name
 */
void audio_gui_update_track_info(const char *title, const char *artist, const char *album);

#endif // AUDIO_GUI_H

