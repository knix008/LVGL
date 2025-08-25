#include "video.h"
#include "audio.h"
#include <stdio.h>
#include <string.h>

// Global variables for video player
static lv_obj_t * g_video_player = NULL;
static lv_obj_t * g_video_status_label = NULL;
static lv_obj_t * g_video_path_label = NULL;

// Video file paths
static char g_video_file_paths[3][256] = {
    "A:../assets/example.mp4",
    "A:../assets/test_video.mp4", 
    "A:../assets/bulb.gif"
};

// Audio file paths
static char g_audio_file_paths[2][256] = {
    "A:../assets/test_audio.wav",
    "A:../assets/test_audio2.wav"
};

static int g_current_video_index = 0;
static int g_current_audio_index = 0;
static char g_video_file_path[256] = "A:../assets/example.mp4";
static char g_video_status[64] = "Ready";

// Initialize video system
void video_init(void) {
    printf("Video: Initializing video system\n");
    // Initialize audio system
    audio_init();
}

// Load video file
lv_result_t video_load_file(const char* file_path) {
    if (g_video_player == NULL) {
        printf("Video: Error - video player not created\n");
        return LV_RESULT_INVALID;
    }
    
        lv_result_t result = lv_ffmpeg_player_set_src(g_video_player, file_path);
    if (result == LV_RESULT_OK) {
        strcpy(g_video_file_path, file_path);
        printf("Video: Loaded file successfully: %s\n", file_path);
        strcpy(g_video_status, "Loaded");
    } else {
        printf("Video: Failed to load file: %s\n", file_path);
        strcpy(g_video_status, "Load Error");
    }
    
    return result;
}

// Play video
lv_result_t video_play(void) {
    if (g_video_player == NULL) {
        printf("Video: Error - video player not created\n");
        return LV_RESULT_INVALID;
    }
    
    lv_ffmpeg_player_set_cmd(g_video_player, LV_FFMPEG_PLAYER_CMD_START);
    strcpy(g_video_status, "Playing");
    printf("Video: Started playing\n");
    return LV_RESULT_OK;
}

// Pause video
lv_result_t video_pause(void) {
    if (g_video_player == NULL) {
        printf("Video: Error - video player not created\n");
        return LV_RESULT_INVALID;
    }
    
    lv_ffmpeg_player_set_cmd(g_video_player, LV_FFMPEG_PLAYER_CMD_PAUSE);
    strcpy(g_video_status, "Paused");
    printf("Video: Paused\n");
    return LV_RESULT_OK;
}

// Stop video
lv_result_t video_stop(void) {
    if (g_video_player == NULL) {
        printf("Video: Error - video player not created\n");
        return LV_RESULT_INVALID;
    }
    
    lv_ffmpeg_player_set_cmd(g_video_player, LV_FFMPEG_PLAYER_CMD_STOP);
    strcpy(g_video_status, "Stopped");
    printf("Video: Stopped\n");
    return LV_RESULT_OK;
}

// Restart video
lv_result_t video_restart(void) {
    if (g_video_player == NULL) {
        printf("Video: Error - video player not created\n");
        return LV_RESULT_INVALID;
    }
    
    // Stop first, then start
    lv_ffmpeg_player_set_cmd(g_video_player, LV_FFMPEG_PLAYER_CMD_STOP);
    lv_ffmpeg_player_set_cmd(g_video_player, LV_FFMPEG_PLAYER_CMD_START);
    strcpy(g_video_status, "Restarted");
    printf("Video: Restarted\n");
    return LV_RESULT_OK;
}

// Next video
lv_result_t video_next(void) {
    if (g_video_player == NULL) {
        printf("Video: Error - video player not created\n");
        return LV_RESULT_INVALID;
    }
    
    // Switch to next video
    g_current_video_index = (g_current_video_index + 1) % 3;
    strcpy(g_video_file_path, g_video_file_paths[g_current_video_index]);
    
    // Stop current video
    lv_ffmpeg_player_set_cmd(g_video_player, LV_FFMPEG_PLAYER_CMD_STOP);
    
    // Set new video source
    lv_result_t result = lv_ffmpeg_player_set_src(g_video_player, g_video_file_path);
    if (result == LV_RESULT_OK) {
        printf("Video: Switched to video: %s\n", g_video_file_path);
        lv_ffmpeg_player_set_cmd(g_video_player, LV_FFMPEG_PLAYER_CMD_START);
        strcpy(g_video_status, "Playing");
    } else {
        printf("Video: Failed to load video: %s\n", g_video_file_path);
        strcpy(g_video_status, "Error loading video");
    }
    
    return result;
}

// Get current video path
const char* video_get_current_path(void) {
    return g_video_file_path;
}

// Get status string
const char* video_get_status_string(void) {
    return g_video_status;
}

// Set video player object (called from GUI)
void video_set_player(lv_obj_t* player) {
    g_video_player = player;
}

// Set status label object (called from GUI)
void video_set_status_label(lv_obj_t* label) {
    g_video_status_label = label;
}

// Set path label object (called from GUI)
void video_set_path_label(lv_obj_t* label) {
    g_video_path_label = label;
}

// Get video player object
lv_obj_t* video_get_player(void) {
    return g_video_player;
}

// Get status label object
lv_obj_t* video_get_status_label(void) {
    return g_video_status_label;
}

// Get path label object
lv_obj_t* video_get_path_label(void) {
    return g_video_path_label;
}

// Audio control functions
void video_audio_play(void) {
    if (audio_is_initialized()) {
        // Load and play current audio file
        audio_play_file(g_audio_file_paths[g_current_audio_index]);
    }
}

void video_audio_pause(void) {
    if (audio_is_initialized()) {
        audio_pause();
    }
}

void video_audio_stop(void) {
    if (audio_is_initialized()) {
        audio_stop();
    }
}

void video_audio_set_volume(int volume) {
    if (audio_is_initialized()) {
        audio_set_volume(volume);
    }
}

int video_audio_get_volume(void) {
    if (audio_is_initialized()) {
        return audio_get_volume();
    }
    return 0;
}

void video_audio_set_mute(bool mute) {
    if (audio_is_initialized()) {
        audio_set_mute(mute);
    }
}

bool video_audio_is_muted(void) {
    if (audio_is_initialized()) {
        return audio_is_muted();
    }
    return false;
}

bool video_audio_is_playing(void) {
    if (audio_is_initialized()) {
        return audio_is_playing();
    }
    return false;
}

const char* video_audio_get_status(void) {
    if (audio_is_initialized()) {
        return audio_get_status_string();
    }
    return "Audio not initialized";
} 