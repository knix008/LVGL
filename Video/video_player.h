#ifndef VIDEO_PLAYER_H
#define VIDEO_PLAYER_H

#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"

// Video player state
typedef enum {
    PLAYER_STOPPED,
    PLAYER_PLAYING,
    PLAYER_PAUSED
} player_state_t;

// Video player API functions
void video_player_init(void);
void video_player_cleanup(void);
void video_player_load_file(const char *filename);
void video_player_play(void);
void video_player_pause(void);
void video_player_stop(void);
void video_player_seek(int position);
void video_player_set_volume(int volume);
void video_player_update(void);

// LVGL object management
void video_player_set_object(lv_obj_t *player);
lv_obj_t *video_player_get_object(void);

// Video rendering functions
bool video_player_get_frame(uint8_t **frame_data, int *width, int *height);
void video_player_release_frame(void);

// Getters
player_state_t video_player_get_state(void);
const char* video_player_get_current_file(void);
int video_player_get_position(void);
int video_player_get_duration(void);
int video_player_get_volume(void);

#endif // VIDEO_PLAYER_H
