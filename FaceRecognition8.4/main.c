/**
 * @file main.c
 * Webcam Photo Capture Application - Main Entry Point
 * Window Size: 340x640
 */

#include "lvgl/lvgl.h"
#include "camera.h"
#include "gui.h"
#include "face_detection.h"
#include "face_recognition.h"
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <pthread.h>
#include <SDL2/SDL.h>

// Display configuration
#define WINDOW_WIDTH  340
#define WINDOW_HEIGHT 640

// Global exit control variables
static volatile bool should_exit = false;
static volatile bool cleanup_done = false;

// Global variables
static int photo_count = 0;
static int registration_photo_count = 0;
static char registration_person_name[128] = "";
static bool registration_mode = false;

// Forward declarations
static void exit_handler(void);
static void* force_exit_thread(void* arg);
static void signal_handler(int signum);

/**
 * Force exit thread - ensures process terminates if cleanup takes too long
 */
static void* force_exit_thread(void* arg)
{
    (void)arg;

    printf("Force exit thread started - will terminate process in 3 seconds if not exited naturally.\n");
    sleep(3);

    printf("Force exit timeout reached - terminating process with extreme prejudice.\n");
    fflush(stdout);

    // Try different termination methods in order
    _Exit(1);  // Force process termination

    return NULL;  // Should never reach here
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

    // Cleanup face recognition
    face_recognition_cleanup();

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

    printf("Exit handler cleanup complete\n");
}

/**
 * Registration screen shown callback - sets registration mode and reads name from GUI
 */
static void registration_screen_shown_callback(void *user_data)
{
    (void)user_data;

    printf("Registration screen shown - setting registration_mode to true\n");
    registration_mode = true;
    registration_photo_count = 0;

    // Get person name from GUI (will be filled when user enters it)
    // For now, will be set by register_face_callback
}

/**
 * Register face callback - captures face for registration
 */
static void register_face_callback(void *user_data)
{
    (void)user_data;

    if (!registration_mode) {
        printf("Registration mode not active\n");
        return;
    }

    // Get person name from GUI
    const char *name_from_gui = gui_get_registration_name();
    if (name_from_gui && strlen(name_from_gui) > 0) {
        strncpy(registration_person_name, name_from_gui, sizeof(registration_person_name) - 1);
        registration_person_name[sizeof(registration_person_name) - 1] = '\0';
    } else {
        printf("Error: Person name is empty\n");
        gui_update_status("이름을 입력하세요!");  // Korean: "Please enter name"
        return;
    }

    printf("[DEBUG] Got person name from GUI: %s\n", registration_person_name);

    uint8_t *frame = camera_get_frame();
    if (!frame) {
        printf("Failed to get camera frame\n");
        return;
    }

    int width, height;
    camera_get_dimensions(&width, &height);

    // Register this face sample
    if (face_recognition_register_face(registration_person_name, frame, width, height)) {
        registration_photo_count++;
        printf("Registered face sample %d for %s\n", registration_photo_count, registration_person_name);

        // Also save the image to images/ directory
        printf("[DEBUG] Calling face_recognition_save_registration_image with: person_name=%s, sample=%d, frame=%p\n",
               registration_person_name, registration_photo_count, (void *)frame);
        bool save_result = face_recognition_save_registration_image(registration_person_name, registration_photo_count, frame, width, height);
        printf("[DEBUG] face_recognition_save_registration_image returned: %s\n", save_result ? "true" : "false");

        char status[256];
        snprintf(status, sizeof(status), "촬영됨: %d/3", registration_photo_count);
        gui_update_status(status);

        // If we have 3 samples, finalize registration
        if (registration_photo_count >= 3) {
            if (face_recognition_finalize_registration(registration_person_name)) {
                printf("Registration completed for %s\n", registration_person_name);
                gui_update_status("등록 완료!");  // Korean: "Registration complete!"
                gui_hide_registration_screen();
                registration_mode = false;
                registration_photo_count = 0;
                memset(registration_person_name, 0, sizeof(registration_person_name));
            }
        }
    } else {
        printf("Failed to register face\n");
        gui_update_status("얼굴 등록 실패!");  // Korean: "Face registration failed!"
    }
}

/**
 * Recognize face callback - finalizes registration
 */
static void recognize_face_callback(void *user_data)
{
    (void)user_data;

    if (!registration_mode) {
        return;
    }

    // If less than 3 samples, don't allow finalization
    if (registration_photo_count < 1) {
        gui_update_status("최소 1장 필요");  // Korean: "Need at least 1 photo"
        return;
    }

    // Finalize registration
    if (face_recognition_finalize_registration(registration_person_name)) {
        printf("Registration completed for %s\n", registration_person_name);
        gui_update_status("등록 완료!");  // Korean: "Registration complete!"
        gui_hide_registration_screen();
        registration_mode = false;
        registration_photo_count = 0;
        memset(registration_person_name, 0, sizeof(registration_person_name));
    } else {
        gui_update_status("등록 실패!");  // Korean: "Registration failed!"
    }
}

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
        exit_handler();
        return 1;
    }

    // Set capture button callback
    gui_set_capture_callback(capture_photo_callback, NULL);

    // Set registration and recognition callbacks
    gui_set_register_callback(register_face_callback, NULL);
    gui_set_recognize_callback(recognize_face_callback, NULL);
    gui_set_registration_shown_callback(registration_screen_shown_callback, NULL);

    // Initialize face recognition
    if (!face_recognition_init()) {
        fprintf(stderr, "Warning: Failed to initialize face recognition\n");
        gui_update_status("얼굴 인식 초기화 실패!");  // Korean: "Face recognition init failed!"
    }

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
    int recognition_skip = 0;
    FaceDetectionResult face_result;
    time_t last_recognition_display_time = 0;

    while (!should_exit) {
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

                            // Run face recognition if not in registration mode (every 10 frames)
                            if (!registration_mode && face_recognition_is_initialized()) {
                                recognition_skip++;
                                if (recognition_skip >= 10) {
                                    recognition_skip = 0;

                                    // If faces detected, try to recognize them
                                    if (face_result.count > 0) {
                                        RecognitionResult recognition_result;
                                        if (face_recognition_recognize(frame, width, height, &recognition_result)) {
                                            time_t now = time(NULL);
                                            // Show popup if enough time has passed since last update
                                            if (now - last_recognition_display_time > 2) {  // Update at most every 2 seconds
                                                if (recognition_result.is_recognized) {
                                                    // Recognized person - show name and confidence
                                                    gui_show_recognition_popup(recognition_result.person_name,
                                                                              recognition_result.confidence);
                                                    printf("Recognized: %s (confidence: %.2f)\n",
                                                          recognition_result.person_name,
                                                          recognition_result.confidence);
                                                } else {
                                                    // Unknown person - show "미등록" (Not registered) message
                                                    gui_show_recognition_popup("미등록", 0.0f);
                                                    printf("Unknown person detected (not registered)\n");
                                                }
                                                last_recognition_display_time = now;
                                            }
                                        }
                                    }
                                }
                            }
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

        // Check if LVGL has been deinitialized (window closed) AFTER timer handler
        // When LV_SDL_DIRECT_EXIT=0, LVGL calls lv_deinit() on window close
        if (!lv_is_initialized()) {
            printf("Window closed, exiting main loop...\n");
            break;
        }

        lv_delay_ms(time_till_next < 5 ? time_till_next : 5);  // Max 5ms delay
    }

    // Call exit handler (will be called again by atexit, but that's ok due to cleanup_done flag)
    exit_handler();

    printf("Application closed\n");
    return 0;
}
