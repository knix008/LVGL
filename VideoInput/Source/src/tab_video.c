#include "tab_video.h"
#include "video.h"
#include "webcam.h"
#include "tab_webcam.h"
#include <stdio.h>
#include <string.h>

// GUI-only variables
static lv_obj_t * g_video_controls = NULL;

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

// Webcam callbacks are now handled in tab_webcam.c

// Create Video Input tab
void create_video_tab(lv_obj_t * parent) {
    // Initialize video system
    video_init();
    
    // Create title label
    lv_obj_t * title = lv_label_create(parent);
    lv_label_set_text(title, "Video Input");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    
    // Create video player
    lv_obj_t * video_player = lv_ffmpeg_player_create(parent);
    if (video_player != NULL) {
        // Set video player in video logic
        video_set_player(video_player);
        
        // Load initial video file
        lv_result_t result = video_load_file(video_get_current_path());
        if (result == LV_RESULT_OK) {
            printf("Video tab: Video loaded successfully: %s\n", video_get_current_path());
            
            // Enable auto restart
            lv_ffmpeg_player_set_auto_restart(video_player, true);
            
            // Position the video player
            lv_obj_set_size(video_player, 400, 300);
            lv_obj_align(video_player, LV_ALIGN_TOP_MID, 0, 50);
            
            // Create status label
            lv_obj_t * status_label = lv_label_create(parent);
            lv_label_set_text(status_label, "Status: Ready");
            lv_obj_align(status_label, LV_ALIGN_TOP_MID, 0, 370);
            video_set_status_label(status_label);
            
            // Create path label
            lv_obj_t * path_label = lv_label_create(parent);
            lv_label_set_text(path_label, video_get_current_path());
            lv_obj_set_style_text_color(path_label, lv_color_hex(0x808080), 0); // Gray color
            lv_obj_align(path_label, LV_ALIGN_TOP_MID, 0, 390);
            lv_obj_set_width(path_label, 400);
            lv_label_set_long_mode(path_label, LV_LABEL_LONG_SCROLL_CIRCULAR);
            video_set_path_label(path_label);
            
            // Create control buttons
            g_video_controls = lv_obj_create(parent);
            lv_obj_set_size(g_video_controls, 400, 120);
            lv_obj_align(g_video_controls, LV_ALIGN_TOP_MID, 0, 420);
            lv_obj_set_style_pad_all(g_video_controls, 5, 0);
            lv_obj_set_style_bg_opa(g_video_controls, LV_OPA_TRANSP, 0);
            lv_obj_set_style_border_opa(g_video_controls, LV_OPA_TRANSP, 0);
            
            // First row: Video controls
            // Play button
            lv_obj_t * play_btn = lv_btn_create(g_video_controls);
            lv_obj_set_size(play_btn, 60, 30);
            lv_obj_align(play_btn, LV_ALIGN_LEFT_MID, 10, -20);
            lv_obj_t * play_label = lv_label_create(play_btn);
            lv_label_set_text(play_label, "Play");
            lv_obj_center(play_label);
            lv_obj_add_event_cb(play_btn, video_play_cb, LV_EVENT_CLICKED, NULL);
            
            // Pause button
            lv_obj_t * pause_btn = lv_btn_create(g_video_controls);
            lv_obj_set_size(pause_btn, 60, 30);
            lv_obj_align(pause_btn, LV_ALIGN_LEFT_MID, 80, -20);
            lv_obj_t * pause_label = lv_label_create(pause_btn);
            lv_label_set_text(pause_label, "Pause");
            lv_obj_center(pause_label);
            lv_obj_add_event_cb(pause_btn, video_pause_cb, LV_EVENT_CLICKED, NULL);
            
            // Stop button
            lv_obj_t * stop_btn = lv_btn_create(g_video_controls);
            lv_obj_set_size(stop_btn, 60, 30);
            lv_obj_align(stop_btn, LV_ALIGN_LEFT_MID, 150, -20);
            lv_obj_t * stop_label = lv_label_create(stop_btn);
            lv_label_set_text(stop_label, "Stop");
            lv_obj_center(stop_label);
            lv_obj_add_event_cb(stop_btn, video_stop_cb, LV_EVENT_CLICKED, NULL);
            
            // Restart button
            lv_obj_t * restart_btn = lv_btn_create(g_video_controls);
            lv_obj_set_size(restart_btn, 60, 30);
            lv_obj_align(restart_btn, LV_ALIGN_LEFT_MID, 220, -20);
            lv_obj_t * restart_label = lv_label_create(restart_btn);
            lv_label_set_text(restart_label, "Restart");
            lv_obj_center(restart_label);
            lv_obj_add_event_cb(restart_btn, video_restart_cb, LV_EVENT_CLICKED, NULL);
            
            // Next Video button
            lv_obj_t * next_btn = lv_btn_create(g_video_controls);
            lv_obj_set_size(next_btn, 80, 30);
            lv_obj_align(next_btn, LV_ALIGN_LEFT_MID, 290, -20);
            lv_obj_t * next_label = lv_label_create(next_btn);
            lv_label_set_text(next_label, "Next");
            lv_obj_center(next_label);
            lv_obj_add_event_cb(next_btn, video_next_cb, LV_EVENT_CLICKED, NULL);
            
            // Create webcam controls using separated GUI module
            create_webcam_controls(parent, video_get_player(), video_get_status_label(), video_get_path_label());
            
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