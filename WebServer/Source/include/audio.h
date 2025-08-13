#ifndef AUDIO_H
#define AUDIO_H

#include "lvgl.h"
#include <stdint.h>
#include <stdbool.h>

// Audio system functions
void audio_init(void);
void audio_deinit(void);
bool audio_is_initialized(void);

// Audio playback functions
bool audio_play_file(const char* file_path);
void audio_stop(void);
void audio_pause(void);
bool audio_resume(void);
bool audio_is_playing(void);
bool audio_is_paused(void);

// Volume control
void audio_set_volume(int volume); // 0-100
int audio_get_volume(void);
void audio_set_mute(bool mute);
bool audio_is_muted(void);

// Audio file management
bool audio_load_file(const char* file_path);
const char* audio_get_current_file(void);

// Audio status
const char* audio_get_status_string(void);

#endif // AUDIO_H
