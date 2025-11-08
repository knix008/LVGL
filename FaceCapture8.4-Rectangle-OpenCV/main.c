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
#include <pthread.h>
#include <SDL2/SDL.h>

// Display configuration
#define WINDOW_WIDTH  340
#define WINDOW_HEIGHT 640

// Global variables
static int photo_count = 0;
static volatile int cleanup_started = 0;

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
    lv_display_t *display = lv_sdl_window_create(WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!display) {
        fprintf(stderr, "Failed to create SDL window\n");
        return -1;
    }

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
 * Watchdog thread that force-kills the process if cleanup hangs
 */
static void *watchdog_thread(void *arg)
{
    (void)arg;

    // Wait for cleanup to start
    while (!cleanup_started) {
        usleep(10000);  // 10ms
    }

    // Now wait 1 second for cleanup to finish
    printf("Watchdog: Cleanup started, waiting 1 second...\n");
    sleep(1);

    // If we're still here after 1 second, force exit
    fprintf(stderr, "\n!!! WATCHDOG TIMEOUT !!!\n");
    fprintf(stderr, "Cleanup took more than 1 second, FORCE KILLING PROCESS NOW!\n");
    fflush(stderr);
    fflush(stdout);
    _exit(1);  // Nuclear option - kill everything NOW

    return NULL;
}

/**
 * Cleanup resources
 */
static void cleanup(void)
{
    // Signal that cleanup has started
    cleanup_started = 1;

    printf("Starting cleanup...\n");

    // Cleanup face detection
    printf("Cleaning up face detection...\n");
    face_detection_cleanup();
    printf("Face detection cleaned up\n");

    // Stop camera
    printf("Stopping camera...\n");
    camera_stop();
    printf("Cleaning up camera...\n");
    camera_cleanup();
    printf("Camera cleaned up\n");

    // Cleanup GUI only if LVGL is still initialized
    // If LVGL already called lv_deinit(), GUI resources are already freed
    if (lv_is_initialized()) {
        printf("Cleaning up GUI...\n");
        gui_cleanup();
        printf("GUI cleaned up\n");
    } else {
        printf("LVGL already deinitialized, skipping GUI cleanup\n");
    }

    printf("Cleanup complete\n");
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

    while (1) {
        // IMPORTANT: Peek at SDL events BEFORE LVGL processes them
        // This ensures we see SDL_QUIT even if LVGL timers are running
        SDL_Event event;
        if (SDL_PeepEvents(&event, 1, SDL_PEEKEVENT, SDL_QUIT, SDL_QUIT) > 0) {
            fprintf(stderr, "\n=== SDL QUIT EVENT DETECTED (PEEK) ===\n");
            fprintf(stderr, "Stopping camera to release /dev/video0...\n");
            fflush(stderr);

            // CRITICAL: Stop camera FIRST to release the V4L2 device
            camera_stop();

            fprintf(stderr, "Camera stopped, exiting now!\n");
            fflush(stderr);
            _exit(0);
        }

        // Also check for window close events
        if (SDL_PeepEvents(&event, 1, SDL_PEEKEVENT, SDL_WINDOWEVENT, SDL_WINDOWEVENT) > 0) {
            if (event.window.event == SDL_WINDOWEVENT_CLOSE) {
                fprintf(stderr, "\n=== SDL WINDOW CLOSE EVENT DETECTED ===\n");
                fprintf(stderr, "Stopping camera to release /dev/video0...\n");
                fflush(stderr);

                // CRITICAL: Stop camera FIRST to release the V4L2 device
                camera_stop();

                fprintf(stderr, "Camera stopped, exiting now!\n");
                fflush(stderr);
                _exit(0);
            }
        }

        // Also check LVGL initialization
        if (!lv_is_initialized()) {
            fprintf(stderr, "\n=== LVGL DEINITIALIZED ===\n");
            fflush(stderr);
            _exit(0);
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
                }
            }
        }

        // Handle LVGL tasks - returns time until next task
        // LVGL will consume the SDL events here
        uint32_t time_till_next = lv_timer_handler();

        lv_delay_ms(time_till_next < 5 ? time_till_next : 5);  // Max 5ms delay
    }

    // Should never reach here
    printf("Main loop exited unexpectedly\n");
    _exit(0);
}
