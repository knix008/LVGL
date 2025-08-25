#ifndef VIDEO_H
#define VIDEO_H

#include "lvgl.h"

// Video logic functions
void video_init(void);
lv_result_t video_load_file(const char* file_path);
lv_result_t video_play(void);
lv_result_t video_pause(void);
lv_result_t video_stop(void);
lv_result_t video_restart(void);
lv_result_t video_next(void);
const char* video_get_current_path(void);
const char* video_get_status_string(void);

// Video player object getter
lv_obj_t* video_get_player(void);

// Video status label getter
lv_obj_t* video_get_status_label(void);

// Video path label getter
lv_obj_t* video_get_path_label(void);

// Setter functions for GUI objects
void video_set_player(lv_obj_t* player);
void video_set_status_label(lv_obj_t* label);
void video_set_path_label(lv_obj_t* label);

// Audio control functions
void video_audio_play(void);
void video_audio_pause(void);
void video_audio_stop(void);
void video_audio_set_volume(int volume);
int video_audio_get_volume(void);
void video_audio_set_mute(bool mute);
bool video_audio_is_muted(void);
bool video_audio_is_playing(void);
const char* video_audio_get_status(void);

#endif // VIDEO_H 