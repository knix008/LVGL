/**
 * @file main.c
 * Webcam Photo Capture Application - Main Entry Point
 * Window Size: 340x640
 */

#include "lvgl/lvgl.h"
#include "camera.h"
#include "gui.h"
#include "face_detection.h"
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <pthread.h>
#include <sys/types.h>

// Display configuration
#define WINDOW_WIDTH  340
#define WINDOW_HEIGHT 640

// Global variables
static int photo_count = 0;
static volatile bool should_exit = false;
static volatile bool cleanup_done = false;
static volatile bool display_event_handling = false;  // Prevent recursive display events
static lv_display_t *main_display = NULL;

// Forward declarations
static void exit_handler(void);
static void display_event_cb(lv_event_t *e);
static void* force_exit_thread(void* arg);

/**
 * Capture button callback
 */
static void capture_photo_callback(void *user_data)
{
    (void)user_data;

    printf("Capture button clicked!\n");

    // Play shutter sound
    gui_play_shutter_sound();

    // Show white flash for 200ms to provide lighting
    gui_show_flash(200);

    // Wait a bit for the flash to be visible and camera to adjust
    lv_delay_ms(100);

    // Create filename with timestamp
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char filename[256];
    snprintf(filename, sizeof(filename), "photo_%04d%02d%02d_%02d%02d%02d.jpg",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec);

    printf("Attempting to save photo: %s\n", filename);

    // Save photo using camera module
    int result = camera_save_photo(filename);
    printf("Save result: %d\n", result);

    if (result == 0) {
        photo_count++;
        gui_update_photo_count(photo_count);
        gui_update_status(filename);
        printf("Photo saved successfully, count: %d\n", photo_count);
    } else {
        gui_update_status("저장 실패!");  // Korean: "Save failed!"
        printf("Photo save failed!\n");
    }
}

/**
 * Initialize LVGL with SDL
 */
static int init_lvgl(void)
{
    // Initialize LVGL
    lv_init();

    // Create SDL window and display using LVGL's built-in SDL driver
    main_display = lv_sdl_window_create(WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!main_display) {
        fprintf(stderr, "Failed to create SDL window\n");
        return -1;
    }

    // Add event callback to detect display deletion (window close)
    lv_display_add_event_cb(main_display, display_event_cb, LV_EVENT_DELETE, NULL);
    printf("Display event callback registered\n");

    // Create mouse input device
    lv_indev_t *mouse = lv_sdl_mouse_create();
    if (!mouse) {
        fprintf(stderr, "Warning: Failed to create mouse input device\n");
    }

    // Create keyboard input device (optional)
    lv_indev_t *keyboard = lv_sdl_keyboard_create();
    if (!keyboard) {
        fprintf(stderr, "Warning: Failed to create keyboard input device\n");
    }

    printf("LVGL with SDL initialized\n");
    return 0;
}

/**
 * Force exit thread - ensures process terminates within 3 seconds
 */
static void* force_exit_thread(void* arg)
{
    (void)arg;
    
    printf("Force exit thread started - will terminate process in 3 seconds if not exited naturally.\n");
    sleep(3);
    
    printf("Force exit timeout reached - terminating process with extreme prejudice.\n");
    fflush(stdout);
    
    // Try different termination methods in order
    //kill(getpid(), SIGKILL);  // Send SIGKILL to ourselves
    //abort();                  // If SIGKILL somehow fails, abort
    _Exit(1);                // If abort fails, use _Exit
    
    return NULL;  // Should never reach here
}

/**
 * Display event callback - handles display close events
 */
static void display_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    // Prevent recursive display event handling during cleanup
    if (display_event_handling) {
        printf("Display event already being handled, ignoring recursive call\n");
        return;
    }

    if (code == LV_EVENT_DELETE) {
        display_event_handling = true;
        printf("Display delete event received, performing immediate cleanup and exit...\n");
        fflush(stdout);
        // Signal GUI to stop accepting updates
        gui_signal_exit();
        // Don't call exit_handler here as it will call lv_deinit() which triggers
        // more display events. Instead, do minimal cleanup and exit.
        printf("Skipping full cleanup from event callback to avoid recursion\n");
        fflush(stdout);
        _exit(0);  // Force exit immediately without further cleanup
    }
}

/**
 * Signal handler for graceful shutdown
 */
static void signal_handler(int signum)
{
    printf("\nReceived signal %d, initiating graceful shutdown...\n", signum);
    should_exit = true;
    
    // For immediate response, also perform cleanup and exit
    if (signum == SIGTERM || signum == SIGINT) {
        printf("Performing immediate cleanup and exit...\n");
        fflush(stdout);  // Ensure output is written
        exit_handler();
        fflush(stdout);  // Ensure cleanup messages are written
        _exit(0);  // Use _exit() to avoid calling atexit handlers again
    }
}

/**
 * Exit handler to ensure cleanup when application exits
 */
static void exit_handler(void)
{
    // Prevent duplicate cleanup
    if (cleanup_done) {
        printf("Cleanup already performed, skipping...\n");
        return;
    }
    cleanup_done = true;
    
    printf("Exit handler called, performing cleanup...\n");
    
    // Start force exit thread to ensure we don't hang forever
    pthread_t force_thread;
    pthread_create(&force_thread, NULL, force_exit_thread, NULL);
    pthread_detach(force_thread);
    
    // Cleanup face detection
    face_detection_cleanup();

    // Stop camera
    camera_stop();
    camera_cleanup();

    // Cleanup GUI only if LVGL is still initialized
    // If LVGL already called lv_deinit(), GUI resources are already freed
    if (lv_is_initialized()) {
        gui_cleanup();
        
        // Deinitialize LVGL if still initialized
        printf("Deinitializing LVGL...\n");
        lv_deinit();
    } else {
        printf("LVGL already deinitialized, skipping GUI cleanup\n");
    }

    // Clean up SDL subsystems
    printf("Cleaning up SDL...\n");
    SDL_Quit();

    printf("Cleanup complete\n");
    
    // Force immediate termination using multiple methods
    printf("Forcing process termination...\n");
    fflush(stdout);
    fflush(stderr);
    
    // Try exit methods in order of preference
    printf("Attempting _exit(0)...\n");
    fflush(stdout);
    _exit(0);  // Should terminate immediately
}

/**
 * Cleanup resources
 */
static void cleanup(void)
{
    exit_handler();
}

/**
 * Main function
 */
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    printf("========================================\n");
    printf("  웹캠 캡처 애플리케이션\n");  // Korean: Webcam Capture Application
    printf("========================================\n");

    // Register exit handler to ensure cleanup
    atexit(exit_handler);
    
    // Register signal handlers for graceful shutdown
    signal(SIGINT, signal_handler);   // Ctrl+C
    signal(SIGTERM, signal_handler);  // Termination request
    
    // Initialize LVGL with SDL
    if (init_lvgl() != 0) {
        return 1;
    }

    // Initialize GUI
    if (gui_init() != 0) {
        cleanup();
        return 1;
    }

    // Set capture button callback
    gui_set_capture_callback(capture_photo_callback, NULL);

    // Initialize face detection
    if (!face_detection_init("models/yolov8n-face.onnx")) {
        fprintf(stderr, "Warning: Failed to initialize face detection\n");
        gui_update_status("얼굴 감지 초기화 실패!");  // Korean: "Face detection init failed!"
    }

    // Initialize camera
    if (camera_init() != 0) {
        fprintf(stderr, "Failed to initialize camera\n");
        gui_update_status("카메라 초기화 실패!");  // Korean: "Camera init failed!"
    } else {
        // Start camera capture
        if (camera_start() != 0) {
            fprintf(stderr, "Failed to start camera\n");
            gui_update_status("카메라 시작 실패!");  // Korean: "Camera start failed!"
        } else {
            gui_update_status("준비");  // Korean: "Ready"
        }
    }

    // Main loop - use LVGL timer handler
    printf("Entering main loop...\n");
    //int frame_count = 0;
    int update_skip = 0;
    FaceDetectionResult face_result;

    while (!should_exit) {
        // LVGL handles SDL events internally, including window close events
        // Our display_event_cb will be called when the window is closed

        // Check for exit signal first
        if (should_exit) {
            printf("Exit signal received, breaking main loop...\n");
            break;
        }

        // Update camera preview only every 3rd frame (~15 FPS instead of 200 FPS)
        if (camera_is_running()) {
            update_skip++;
            if (update_skip >= 3) {
                update_skip = 0;
                uint8_t *frame = camera_get_frame();
                if (frame) {
                    // Run face detection on the frame
                    if (face_detection_is_initialized()) {
                        int width, height;
                        camera_get_dimensions(&width, &height);
                        if (face_detection_detect(frame, width, height, &face_result)) {
                            // Update GUI with face detection results
                            gui_update_camera_preview_with_faces(frame, &face_result);
                        } else {
                            gui_update_camera_preview(frame);
                        }
                    } else {
                        gui_update_camera_preview(frame);
                    }
                    //frame_count++;
                    //if (frame_count % 30 == 0) {
                    //    printf("Camera frames updated: %d\n", frame_count);
                    //}
                }
            }
        }

        // Handle LVGL tasks - returns time until next task
        uint32_t time_till_next = lv_timer_handler();

        // Check if LVGL has been deinitialized (window closed)
        // With LV_SDL_DIRECT_EXIT=0, LVGL calls lv_deinit() on window close
        if (!lv_is_initialized()) {
            printf("LVGL deinitialized (window closed), performing immediate cleanup and exit...\n");
            fflush(stdout);
            exit_handler();
            fflush(stdout);
            _exit(0);  // Force exit immediately
        }

        // Use a shorter delay and check exit condition more frequently
        uint32_t delay_time = time_till_next < 5 ? time_till_next : 5;
        if (delay_time > 1) delay_time = 1;  // Max 1ms delay for more responsive exit
        lv_delay_ms(delay_time);
    }

    // Cleanup
    cleanup();

    printf("Application closed\n");
    return 0;
}
