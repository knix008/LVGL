#include "tab_webcam.h"
#include "webcam.h"
#include "lvgl.h"
#include "lv_ffmpeg.h"
#include <stdio.h>
#include <string.h>

// ============================================================================
// GUI CONTROLS CONTEXT (for video tab integration)
// ============================================================================

// GUI-specific webcam context
typedef struct {
    lv_obj_t* video_player;
    lv_obj_t* status_label;
    lv_obj_t* device_label;
    lv_obj_t* start_button;
    lv_obj_t* stop_button;
} webcam_gui_context_t;

static webcam_gui_context_t g_webcam_gui = {0};

// ============================================================================
// DEDICATED WEBCAM TAB CONTEXT
// ============================================================================

// Webcam tab context
typedef struct {
    lv_obj_t* video_player;
    lv_obj_t* status_label;
    lv_obj_t* device_label;
    lv_obj_t* resolution_label;
    lv_obj_t* fps_label;
    lv_obj_t* controls_container;
    lv_obj_t* info_container;
} webcam_tab_context_t;

static webcam_tab_context_t g_webcam_tab = {0};

// ============================================================================
// SHARED WEBCAM FUNCTIONS
// ============================================================================

// Common webcam start function
static bool webcam_start_capture_common(lv_obj_t* video_player, lv_obj_t* status_label, lv_obj_t* device_label, const char* context_name) {
    if (video_player == NULL) return false;
    
    // Stop current video first
    lv_ffmpeg_player_set_cmd(video_player, LV_FFMPEG_PLAYER_CMD_STOP);
    
    // Get webcam device
    const char* device = webcam_get_device_path();
    if (device == NULL) {
        // Find available device
        if (webcam_is_available()) {
            device = webcam_get_device_path();
        }
    }
    
    if (device != NULL) {
        // Start webcam capture logic
        if (webcam_start_capture_logic(device, 640, 480, 30)) {
            // Get the prepared input URL
            const char* input_url = webcam_get_input_url();
            
            // Set video source to webcam
            lv_result_t result = lv_ffmpeg_player_set_src(video_player, input_url);
            if (result == LV_RESULT_OK) {
                // Start playing
                lv_ffmpeg_player_set_cmd(video_player, LV_FFMPEG_PLAYER_CMD_START);
                
                // Update GUI
                if (status_label) {
                    lv_label_set_text(status_label, "Status: Webcam Active");
                }
                if (device_label) {
                    lv_label_set_text(device_label, "Webcam: Live Capture");
                }
                printf("%s: Capture started successfully\n", context_name);
                return true;
            } else {
                if (status_label) {
                    lv_label_set_text(status_label, "Status: Failed to start webcam");
                }
                printf("%s: Failed to set video source\n", context_name);
                return false;
            }
        } else {
            if (status_label) {
                lv_label_set_text(status_label, "Status: Webcam not available");
            }
            printf("%s: Failed to start webcam capture\n", context_name);
            return false;
        }
    } else {
        if (status_label) {
            lv_label_set_text(status_label, "Status: No webcam device found");
        }
        printf("%s: No webcam device available\n", context_name);
        return false;
    }
}

// Common webcam stop function
static void webcam_stop_capture_common(lv_obj_t* video_player, lv_obj_t* status_label, const char* context_name) {
    if (webcam_is_capturing()) {
        // Stop webcam capture logic
        webcam_stop_capture_logic();
        
        // Stop video player
        if (video_player != NULL) {
            lv_ffmpeg_player_set_cmd(video_player, LV_FFMPEG_PLAYER_CMD_STOP);
        }
        
        // Update GUI
        if (status_label) {
            lv_label_set_text(status_label, "Status: Webcam Stopped");
        }
        printf("%s: Capture stopped\n", context_name);
    }
}

// ============================================================================
// GUI CONTROLS CALLBACKS (for video tab integration)
// ============================================================================

// Webcam start button callback (GUI controls)
static void webcam_start_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        webcam_start_capture_common(g_webcam_gui.video_player, g_webcam_gui.status_label, g_webcam_gui.device_label, "Webcam GUI");
    }
}

// Webcam stop button callback (GUI controls)
static void webcam_stop_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        webcam_stop_capture_common(g_webcam_gui.video_player, g_webcam_gui.status_label, "Webcam GUI");
    }
}

// ============================================================================
// DEDICATED WEBCAM TAB CALLBACKS
// ============================================================================

// Webcam start button callback (dedicated tab)
static void webcam_tab_start_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        webcam_start_capture_common(g_webcam_tab.video_player, g_webcam_tab.status_label, g_webcam_tab.device_label, "Webcam tab");
    }
}

// Webcam stop button callback (dedicated tab)
static void webcam_tab_stop_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        webcam_stop_capture_common(g_webcam_tab.video_player, g_webcam_tab.status_label, "Webcam tab");
    }
}

// Webcam pause button callback (dedicated tab)
static void webcam_tab_pause_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        if (g_webcam_tab.video_player != NULL && webcam_is_capturing()) {
            lv_ffmpeg_player_set_cmd(g_webcam_tab.video_player, LV_FFMPEG_PLAYER_CMD_PAUSE);
            if (g_webcam_tab.status_label) {
                lv_label_set_text(g_webcam_tab.status_label, "Status: Webcam Paused");
            }
            printf("Webcam tab: Capture paused\n");
        }
    }
}

// Webcam resume button callback (dedicated tab)
static void webcam_tab_resume_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        if (g_webcam_tab.video_player != NULL && webcam_is_capturing()) {
            lv_ffmpeg_player_set_cmd(g_webcam_tab.video_player, LV_FFMPEG_PLAYER_CMD_START);
            if (g_webcam_tab.status_label) {
                lv_label_set_text(g_webcam_tab.status_label, "Status: Webcam Active");
            }
            printf("Webcam tab: Capture resumed\n");
        }
    }
}

// ============================================================================
// GUI CONTROLS FUNCTIONS (for video tab integration)
// ============================================================================

// Create webcam GUI controls
void create_webcam_controls(lv_obj_t* parent, lv_obj_t* video_player, 
                           lv_obj_t* status_label, lv_obj_t* device_label) {
    // Store GUI context
    g_webcam_gui.video_player = video_player;
    g_webcam_gui.status_label = status_label;
    g_webcam_gui.device_label = device_label;
    
    // Create webcam control container
    lv_obj_t* webcam_controls = lv_obj_create(parent);
    lv_obj_set_size(webcam_controls, 200, 60);
    lv_obj_align(webcam_controls, LV_ALIGN_TOP_MID, 0, 540);
    lv_obj_set_style_pad_all(webcam_controls, 5, 0);
    lv_obj_set_style_bg_opa(webcam_controls, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_opa(webcam_controls, LV_OPA_TRANSP, 0);
    
    // Webcam Start button
    g_webcam_gui.start_button = lv_btn_create(webcam_controls);
    lv_obj_set_size(g_webcam_gui.start_button, 80, 30);
    lv_obj_align(g_webcam_gui.start_button, LV_ALIGN_LEFT_MID, 10, 0);
    lv_obj_set_style_bg_color(g_webcam_gui.start_button, lv_color_hex(0x00FF00), 0); // Green
    lv_obj_t * webcam_start_label = lv_label_create(g_webcam_gui.start_button);
    lv_label_set_text(webcam_start_label, "Webcam");
    lv_obj_center(webcam_start_label);
    lv_obj_add_event_cb(g_webcam_gui.start_button, webcam_start_cb, LV_EVENT_CLICKED, NULL);
    
    // Webcam Stop button
    g_webcam_gui.stop_button = lv_btn_create(webcam_controls);
    lv_obj_set_size(g_webcam_gui.stop_button, 80, 30);
    lv_obj_align(g_webcam_gui.stop_button, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_set_style_bg_color(g_webcam_gui.stop_button, lv_color_hex(0xFF0000), 0); // Red
    lv_obj_t * webcam_stop_label = lv_label_create(g_webcam_gui.stop_button);
    lv_label_set_text(webcam_stop_label, "Stop Cam");
    lv_obj_center(webcam_stop_label);
    lv_obj_add_event_cb(g_webcam_gui.stop_button, webcam_stop_cb, LV_EVENT_CLICKED, NULL);
    
    printf("Webcam GUI: Controls created successfully\n");
}

// Update webcam GUI status
void update_webcam_gui_status(void) {
    const char* status = webcam_get_status_string();
    if (g_webcam_gui.status_label) {
        lv_label_set_text(g_webcam_gui.status_label, status);
    }
}

// Initialize webcam GUI
void webcam_gui_init(void) {
    memset(&g_webcam_gui, 0, sizeof(webcam_gui_context_t));
    printf("Webcam GUI: Initialized\n");
}

// ============================================================================
// DEDICATED WEBCAM TAB FUNCTIONS
// ============================================================================

// Create dedicated webcam tab
void create_webcam_tab(lv_obj_t * parent) {
    printf("Creating dedicated webcam tab...\n");
    
    // Create title
    lv_obj_t * title = lv_label_create(parent);
    lv_label_set_text(title, "Webcam Control Center");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    
    // Create video player
    g_webcam_tab.video_player = lv_ffmpeg_player_create(parent);
    if (g_webcam_tab.video_player != NULL) {
        lv_obj_set_size(g_webcam_tab.video_player, 640, 480);
        lv_obj_align(g_webcam_tab.video_player, LV_ALIGN_TOP_MID, 0, 40);
        printf("Webcam tab: Video player created successfully\n");
    }
    
    // Create info container
    g_webcam_tab.info_container = lv_obj_create(parent);
    lv_obj_set_size(g_webcam_tab.info_container, 300, 120);
    lv_obj_align(g_webcam_tab.info_container, LV_ALIGN_TOP_LEFT, 10, 40);
    lv_obj_set_style_pad_all(g_webcam_tab.info_container, 10, 0);
    lv_obj_set_style_bg_opa(g_webcam_tab.info_container, LV_OPA_20, 0);
    lv_obj_set_style_border_width(g_webcam_tab.info_container, 1, 0);
    
    // Status label
    g_webcam_tab.status_label = lv_label_create(g_webcam_tab.info_container);
    lv_label_set_text(g_webcam_tab.status_label, "Status: Ready");
    lv_obj_align(g_webcam_tab.status_label, LV_ALIGN_TOP_LEFT, 0, 0);
    
    // Device label
    g_webcam_tab.device_label = lv_label_create(g_webcam_tab.info_container);
    lv_label_set_text(g_webcam_tab.device_label, "Device: Not Connected");
    lv_obj_align(g_webcam_tab.device_label, LV_ALIGN_TOP_LEFT, 0, 25);
    
    // Resolution label
    g_webcam_tab.resolution_label = lv_label_create(g_webcam_tab.info_container);
    lv_label_set_text(g_webcam_tab.resolution_label, "Resolution: 640x480");
    lv_obj_align(g_webcam_tab.resolution_label, LV_ALIGN_TOP_LEFT, 0, 50);
    
    // FPS label
    g_webcam_tab.fps_label = lv_label_create(g_webcam_tab.info_container);
    lv_label_set_text(g_webcam_tab.fps_label, "Frame Rate: 30 FPS");
    lv_obj_align(g_webcam_tab.fps_label, LV_ALIGN_TOP_LEFT, 0, 75);
    
    // Create controls container
    g_webcam_tab.controls_container = lv_obj_create(parent);
    lv_obj_set_size(g_webcam_tab.controls_container, 400, 80);
    lv_obj_align(g_webcam_tab.controls_container, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_set_style_pad_all(g_webcam_tab.controls_container, 10, 0);
    lv_obj_set_style_bg_opa(g_webcam_tab.controls_container, LV_OPA_20, 0);
    lv_obj_set_style_border_width(g_webcam_tab.controls_container, 1, 0);
    
    // Start button
    lv_obj_t * start_btn = lv_btn_create(g_webcam_tab.controls_container);
    lv_obj_set_size(start_btn, 80, 35);
    lv_obj_align(start_btn, LV_ALIGN_LEFT_MID, 10, 0);
    lv_obj_set_style_bg_color(start_btn, lv_color_hex(0x00FF00), 0); // Green
    lv_obj_t * start_label = lv_label_create(start_btn);
    lv_label_set_text(start_label, "Start");
    lv_obj_center(start_label);
    lv_obj_add_event_cb(start_btn, webcam_tab_start_cb, LV_EVENT_CLICKED, NULL);
    
    // Stop button
    lv_obj_t * stop_btn = lv_btn_create(g_webcam_tab.controls_container);
    lv_obj_set_size(stop_btn, 80, 35);
    lv_obj_align(stop_btn, LV_ALIGN_LEFT_MID, 100, 0);
    lv_obj_set_style_bg_color(stop_btn, lv_color_hex(0xFF0000), 0); // Red
    lv_obj_t * stop_label = lv_label_create(stop_btn);
    lv_label_set_text(stop_label, "Stop");
    lv_obj_center(stop_label);
    lv_obj_add_event_cb(stop_btn, webcam_tab_stop_cb, LV_EVENT_CLICKED, NULL);
    
    // Pause button
    lv_obj_t * pause_btn = lv_btn_create(g_webcam_tab.controls_container);
    lv_obj_set_size(pause_btn, 80, 35);
    lv_obj_align(pause_btn, LV_ALIGN_LEFT_MID, 190, 0);
    lv_obj_set_style_bg_color(pause_btn, lv_color_hex(0xFFFF00), 0); // Yellow
    lv_obj_t * pause_label = lv_label_create(pause_btn);
    lv_label_set_text(pause_label, "Pause");
    lv_obj_center(pause_label);
    lv_obj_add_event_cb(pause_btn, webcam_tab_pause_cb, LV_EVENT_CLICKED, NULL);
    
    // Resume button
    lv_obj_t * resume_btn = lv_btn_create(g_webcam_tab.controls_container);
    lv_obj_set_size(resume_btn, 80, 35);
    lv_obj_align(resume_btn, LV_ALIGN_LEFT_MID, 280, 0);
    lv_obj_set_style_bg_color(resume_btn, lv_color_hex(0x0080FF), 0); // Blue
    lv_obj_t * resume_label = lv_label_create(resume_btn);
    lv_label_set_text(resume_label, "Resume");
    lv_obj_center(resume_label);
    lv_obj_add_event_cb(resume_btn, webcam_tab_resume_cb, LV_EVENT_CLICKED, NULL);
    
    // Update initial status
    if (webcam_is_available()) {
        lv_label_set_text(g_webcam_tab.device_label, "Device: Available");
        lv_label_set_text(g_webcam_tab.status_label, "Status: Ready to Capture");
    } else {
        lv_label_set_text(g_webcam_tab.device_label, "Device: Not Available");
        lv_label_set_text(g_webcam_tab.status_label, "Status: No Webcam Found");
    }
    
    printf("Webcam tab: Dedicated webcam tab created successfully\n");
} 