#include "tab_video.h"
#include "video.h"
#include <stdio.h>
#include <string.h>

// GUI-only variables
static lv_obj_t * g_video_controls = NULL;
static lv_obj_t * g_audio_controls = NULL;
static lv_obj_t * g_volume_slider = NULL;
static lv_obj_t * g_volume_label = NULL;
static lv_obj_t * g_audio_status_label = NULL;

// Callback functions for video controls (GUI layer)
static void video_play_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        lv_result_t result = video_play();
        if (result == LV_RESULT_OK) {
            lv_label_set_text(video_get_status_label(), "Status: Playing");
        }
    }
}

static void video_pause_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        lv_result_t result = video_pause();
        if (result == LV_RESULT_OK) {
            lv_label_set_text(video_get_status_label(), "Status: Paused");
        }
    }
}

static void video_stop_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        lv_result_t result = video_stop();
        if (result == LV_RESULT_OK) {
            lv_label_set_text(video_get_status_label(), "Status: Stopped");
        }
    }
}

static void video_restart_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        lv_result_t result = video_restart();
        if (result == LV_RESULT_OK) {
            lv_label_set_text(video_get_status_label(), "Status: Restarted");
        }
    }
}

static void video_next_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        lv_result_t result = video_next();
        if (result == LV_RESULT_OK) {
            lv_label_set_text(video_get_path_label(), video_get_current_path());
            lv_label_set_text(video_get_status_label(), "Status: Playing");
        } else {
            lv_label_set_text(video_get_status_label(), "Status: Error loading video");
        }
    }
}

// Callback functions for audio controls
static void audio_play_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        video_audio_play();
        lv_label_set_text(g_audio_status_label, video_audio_get_status());
    }
}

static void audio_pause_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        video_audio_pause();
        lv_label_set_text(g_audio_status_label, video_audio_get_status());
    }
}

static void audio_stop_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        video_audio_stop();
        lv_label_set_text(g_audio_status_label, video_audio_get_status());
    }
}

static void audio_mute_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        lv_obj_t * btn = lv_event_get_target(e);
        bool is_muted = video_audio_is_muted();
        video_audio_set_mute(!is_muted);
        
        // Update button text
        lv_obj_t * label = lv_obj_get_child(btn, 0);
        if (label) {
            lv_label_set_text(label, video_audio_is_muted() ? "Unmute" : "Mute");
        }
        
        lv_label_set_text(g_audio_status_label, video_audio_get_status());
    }
}

static void volume_slider_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_VALUE_CHANGED) {
        lv_obj_t * slider = lv_event_get_target(e);
        int32_t value = lv_slider_get_value(slider);
        video_audio_set_volume(value);
        
        // Update volume label
        char vol_text[32];
        snprintf(vol_text, sizeof(vol_text), "Volume: %d%%", value);
        lv_label_set_text(g_volume_label, vol_text);
        
        lv_label_set_text(g_audio_status_label, video_audio_get_status());
    }
}

// Create Video Input tab
void create_video_tab(lv_obj_t * parent) {
    // Initialize video system
    video_init();
    
    // Create title label
    lv_obj_t * title = lv_label_create(parent);
    lv_label_set_text(title, "Video & Audio Player");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    
    // Create main container for video and audio side by side
    lv_obj_t * main_container = lv_obj_create(parent);
    lv_obj_set_size(main_container, 750, 400);
    lv_obj_align(main_container, LV_ALIGN_TOP_MID, 0, -30);
    lv_obj_set_style_pad_all(main_container, 10, 0);
    lv_obj_set_style_bg_opa(main_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(main_container, LV_OPA_TRANSP, 0);
    
    // Create video player (left side)
    lv_obj_t * video_player = lv_ffmpeg_player_create(main_container);
    if (video_player != NULL) {
        // Set video player in video logic
        video_set_player(video_player);
        
        // Load initial video file
        lv_result_t result = video_load_file(video_get_current_path());
        if (result == LV_RESULT_OK) {
            printf("Video tab: Video loaded successfully: %s\n", video_get_current_path());
            
            // Enable auto restart
            lv_ffmpeg_player_set_auto_restart(video_player, true);
            
            // Position the video player on the left side
            lv_obj_set_size(video_player, 300, 200);
            lv_obj_align(video_player, LV_ALIGN_LEFT_MID, 0, 0);
            
            // Create audio controls (right side)
            g_audio_controls = lv_obj_create(main_container);
            lv_obj_set_size(g_audio_controls, 320, 300);
            lv_obj_align(g_audio_controls, LV_ALIGN_RIGHT_MID, 0, 0);
            lv_obj_set_style_pad_all(g_audio_controls, 10, 0);
            lv_obj_set_style_bg_opa(g_audio_controls, LV_OPA_TRANSP, 0);
            lv_obj_set_style_border_opa(g_audio_controls, LV_OPA_TRANSP, 0);
            
            // Audio title
            lv_obj_t * audio_title = lv_label_create(g_audio_controls);
            lv_label_set_text(audio_title, "Audio Controls");
            lv_obj_set_style_text_font(audio_title, &lv_font_montserrat_14, 0);
            lv_obj_align(audio_title, LV_ALIGN_TOP_MID, 0, 0);
            
            // Audio controls moved to unified control row below
            
            // Volume slider (second row)
            lv_obj_t * volume_label_title = lv_label_create(g_audio_controls);
            lv_label_set_text(volume_label_title, "Volume Control");
            lv_obj_align(volume_label_title, LV_ALIGN_LEFT_MID, 10, 80);
            
            g_volume_slider = lv_slider_create(g_audio_controls);
            lv_obj_set_size(g_volume_slider, 280, 20);
            lv_obj_align(g_volume_slider, LV_ALIGN_LEFT_MID, 10, 110);
            lv_slider_set_range(g_volume_slider, 0, 100);
            lv_slider_set_value(g_volume_slider, 100, LV_ANIM_OFF);
            lv_obj_add_event_cb(g_volume_slider, volume_slider_cb, LV_EVENT_VALUE_CHANGED, NULL);
            
            // Volume label
            g_volume_label = lv_label_create(g_audio_controls);
            lv_label_set_text(g_volume_label, "Volume: 100%");
            lv_obj_align(g_volume_label, LV_ALIGN_LEFT_MID, 10, 140);
            
            // Audio status label
            g_audio_status_label = lv_label_create(g_audio_controls);
            lv_label_set_text(g_audio_status_label, video_audio_get_status());
            lv_obj_align(g_audio_status_label, LV_ALIGN_LEFT_MID, 10, 170);
            lv_obj_set_style_text_color(g_audio_status_label, lv_color_hex(0x808080), 0);
            
            // Create status and path labels below the main container
            lv_obj_t * status_label = lv_label_create(parent);
            lv_label_set_text(status_label, "Status: Ready");
            lv_obj_align(status_label, LV_ALIGN_TOP_MID, 0, 300);
            video_set_status_label(status_label);
            
            // Create path label
            lv_obj_t * path_label = lv_label_create(parent);
            lv_label_set_text(path_label, video_get_current_path());
            lv_obj_set_style_text_color(path_label, lv_color_hex(0x808080), 0); // Gray color
            lv_obj_align(path_label, LV_ALIGN_TOP_MID, 0, 320);
            lv_obj_set_width(path_label, 750);
            lv_label_set_long_mode(path_label, LV_LABEL_LONG_SCROLL_CIRCULAR);
            video_set_path_label(path_label);
            
            // Create unified control buttons row below the main container
            g_video_controls = lv_obj_create(parent);
            lv_obj_set_size(g_video_controls, 750, 80);
            lv_obj_align(g_video_controls, LV_ALIGN_TOP_MID, 0, 350);
            lv_obj_set_style_pad_all(g_video_controls, 5, 0);
            lv_obj_set_style_bg_opa(g_video_controls, LV_OPA_TRANSP, 0);
            lv_obj_set_style_border_opa(g_video_controls, LV_OPA_TRANSP, 0);
            
            // Video controls (left side)
            // Play button
            lv_obj_t * play_btn = lv_btn_create(g_video_controls);
            lv_obj_set_size(play_btn, 60, 30);
            lv_obj_align(play_btn, LV_ALIGN_LEFT_MID, 15, 0);
            lv_obj_t * play_label = lv_label_create(play_btn);
            lv_label_set_text(play_label, "Play");
            lv_obj_set_style_text_font(play_label, &lv_font_montserrat_14, 0);
            lv_obj_center(play_label);
            lv_obj_add_event_cb(play_btn, video_play_cb, LV_EVENT_CLICKED, NULL);
            
            // Pause button
            lv_obj_t * pause_btn = lv_btn_create(g_video_controls);
            lv_obj_set_size(pause_btn, 60, 30);
            lv_obj_align(pause_btn, LV_ALIGN_LEFT_MID, 85, 0);
            lv_obj_t * pause_label = lv_label_create(pause_btn);
            lv_label_set_text(pause_label, "Pause");
            lv_obj_set_style_text_font(pause_label, &lv_font_montserrat_14, 0);
            lv_obj_center(pause_label);
            lv_obj_add_event_cb(pause_btn, video_pause_cb, LV_EVENT_CLICKED, NULL);
            
            // Stop button
            lv_obj_t * stop_btn = lv_btn_create(g_video_controls);
            lv_obj_set_size(stop_btn, 60, 30);
            lv_obj_align(stop_btn, LV_ALIGN_LEFT_MID, 155, 0);
            lv_obj_t * stop_label = lv_label_create(stop_btn);
            lv_label_set_text(stop_label, "Stop");
            lv_obj_set_style_text_font(stop_label, &lv_font_montserrat_14, 0);
            lv_obj_center(stop_label);
            lv_obj_add_event_cb(stop_btn, video_stop_cb, LV_EVENT_CLICKED, NULL);
            
            // Restart button
            lv_obj_t * restart_btn = lv_btn_create(g_video_controls);
            lv_obj_set_size(restart_btn, 60, 30);
            lv_obj_align(restart_btn, LV_ALIGN_LEFT_MID, 225, 0);
            lv_obj_t * restart_label = lv_label_create(restart_btn);
            lv_label_set_text(restart_label, "Restart");
            lv_obj_set_style_text_font(restart_label, &lv_font_montserrat_14, 0);
            lv_obj_center(restart_label);
            lv_obj_add_event_cb(restart_btn, video_restart_cb, LV_EVENT_CLICKED, NULL);
            
            // Next Video button
            lv_obj_t * next_btn = lv_btn_create(g_video_controls);
            lv_obj_set_size(next_btn, 75, 30);
            lv_obj_align(next_btn, LV_ALIGN_LEFT_MID, 295, 0);
            lv_obj_t * next_label = lv_label_create(next_btn);
            lv_label_set_text(next_label, "Next");
            lv_obj_set_style_text_font(next_label, &lv_font_montserrat_14, 0);
            lv_obj_center(next_label);
            lv_obj_add_event_cb(next_btn, video_next_cb, LV_EVENT_CLICKED, NULL);
            
            // Audio controls (right side)
            // Audio Play button
            lv_obj_t * audio_play_btn = lv_btn_create(g_video_controls);
            lv_obj_set_size(audio_play_btn, 60, 30);
            lv_obj_align(audio_play_btn, LV_ALIGN_RIGHT_MID, -240, 0);
            lv_obj_t * audio_play_label = lv_label_create(audio_play_btn);
            lv_label_set_text(audio_play_label, "Play");
            lv_obj_set_style_text_font(audio_play_label, &lv_font_montserrat_14, 0);
            lv_obj_center(audio_play_label);
            lv_obj_add_event_cb(audio_play_btn, audio_play_cb, LV_EVENT_CLICKED, NULL);
            
            // Audio Pause button
            lv_obj_t * audio_pause_btn = lv_btn_create(g_video_controls);
            lv_obj_set_size(audio_pause_btn, 60, 30);
            lv_obj_align(audio_pause_btn, LV_ALIGN_RIGHT_MID, -170, 0);
            lv_obj_t * audio_pause_label = lv_label_create(audio_pause_btn);
            lv_label_set_text(audio_pause_label, "Pause");
            lv_obj_set_style_text_font(audio_pause_label, &lv_font_montserrat_14, 0);
            lv_obj_center(audio_pause_label);
            lv_obj_add_event_cb(audio_pause_btn, audio_pause_cb, LV_EVENT_CLICKED, NULL);
            
            // Audio Stop button
            lv_obj_t * audio_stop_btn = lv_btn_create(g_video_controls);
            lv_obj_set_size(audio_stop_btn, 60, 30);
            lv_obj_align(audio_stop_btn, LV_ALIGN_RIGHT_MID, -100, 0);
            lv_obj_t * audio_stop_label = lv_label_create(audio_stop_btn);
            lv_label_set_text(audio_stop_label, "Stop");
            lv_obj_set_style_text_font(audio_stop_label, &lv_font_montserrat_14, 0);
            lv_obj_center(audio_stop_label);
            lv_obj_add_event_cb(audio_stop_btn, audio_stop_cb, LV_EVENT_CLICKED, NULL);
            
            // Mute button
            lv_obj_t * mute_btn = lv_btn_create(g_video_controls);
            lv_obj_set_size(mute_btn, 60, 30);
            lv_obj_align(mute_btn, LV_ALIGN_RIGHT_MID, -30, 0);
            lv_obj_t * mute_label = lv_label_create(mute_btn);
            lv_label_set_text(mute_label, "Mute");
            lv_obj_set_style_text_font(mute_label, &lv_font_montserrat_14, 0);
            lv_obj_center(mute_label);
            lv_obj_add_event_cb(mute_btn, audio_mute_cb, LV_EVENT_CLICKED, NULL);
            
            // Start playing automatically
            video_play();
            lv_label_set_text(video_get_status_label(), "Status: Playing");
            
        } else {
            printf("Video tab: Failed to load video: %s\n", video_get_current_path());
            
            // Create error message
            lv_obj_t * error_label = lv_label_create(parent);
            lv_label_set_text(error_label, "Error: Could not load video file");
            lv_obj_set_style_text_color(error_label, lv_color_hex(0xFF0000), 0); // Red color
            lv_obj_align(error_label, LV_ALIGN_TOP_MID, 0, 200);
            
            // Create path label for debugging
            lv_obj_t * path_label = lv_label_create(parent);
            lv_label_set_text(path_label, video_get_current_path());
            lv_obj_set_style_text_color(path_label, lv_color_hex(0x808080), 0);
            lv_obj_align(path_label, LV_ALIGN_TOP_MID, 0, 230);
            video_set_path_label(path_label);
        }
    } else {
        printf("Video tab: Failed to create video player\n");
        
        // Create error message
        lv_obj_t * error_label = lv_label_create(parent);
        lv_label_set_text(error_label, "Error: FFmpeg player not available");
        lv_obj_set_style_text_color(error_label, lv_color_hex(0xFF0000), 0);
        lv_obj_align(error_label, LV_ALIGN_TOP_MID, 0, 200);
    }
} 