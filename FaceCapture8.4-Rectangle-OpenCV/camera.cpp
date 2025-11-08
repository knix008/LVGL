/**
 * @file camera.c
 * Camera capture logic implementation using OpenCV
 */

#include "camera.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <errno.h>
#include <jpeglib.h>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>

// Use C++ namespaces
using namespace cv;

// Static variables
static VideoCapture *camera_cap = NULL;
static pthread_t camera_thread;
static bool camera_running = false;
static uint8_t *camera_frame_data = NULL;
static uint8_t *camera_frame_full = NULL;  // Full resolution frame buffer
static int full_width = 0;
static int full_height = 0;
static pthread_mutex_t frame_mutex = PTHREAD_MUTEX_INITIALIZER;
static Mat *current_frame = NULL;
static Mat *preview_frame = NULL;

// Forward declarations
static void *camera_thread_func(void *arg);

/**
 * Initialize camera device using OpenCV
 */
int camera_init(void)
{
    // Allocate frame buffer for RGB data
    camera_frame_data = (uint8_t*)malloc(CAMERA_WIDTH * CAMERA_HEIGHT * 3);
    if (!camera_frame_data) {
        fprintf(stderr, "Failed to allocate frame buffer\n");
        return -1;
    }

    // Initialize with test pattern
    for (int y = 0; y < CAMERA_HEIGHT; y++) {
        for (int x = 0; x < CAMERA_WIDTH; x++) {
            int idx = (y * CAMERA_WIDTH + x) * 3;
            camera_frame_data[idx + 0] = (x * 255) / CAMERA_WIDTH;     // R
            camera_frame_data[idx + 1] = (y * 255) / CAMERA_HEIGHT;    // G
            camera_frame_data[idx + 2] = 128;                          // B
        }
    }

    // Open video device using OpenCV
    camera_cap = new VideoCapture();

    // Try to open camera with V4L2 backend
    if (!camera_cap->open(0, CAP_V4L2)) {
        fprintf(stderr, "Cannot open video device %s\n", VIDEO_DEVICE);
        fprintf(stderr, "WARNING: Running in test pattern mode (no camera access)\n");
        fprintf(stderr, "To fix: Add your user to 'video' group with: sudo usermod -a -G video $USER\n");
        delete camera_cap;
        camera_cap = NULL;
        // Don't return error - continue in test pattern mode
        return 0;
    }

    // Set camera properties for MJPEG format and high resolution
    camera_cap->set(CAP_PROP_FOURCC, VideoWriter::fourcc('M', 'J', 'P', 'G'));
    camera_cap->set(CAP_PROP_FPS, 30);

    // Set buffer size to 1 for lower latency and better responsiveness on close
    camera_cap->set(CAP_PROP_BUFFERSIZE, 1);

    // Let camera auto-select its maximum resolution
    // Query what resolution we got
    full_width = (int)camera_cap->get(CAP_PROP_FRAME_WIDTH);
    full_height = (int)camera_cap->get(CAP_PROP_FRAME_HEIGHT);

    if (full_width == 0 || full_height == 0) {
        fprintf(stderr, "Failed to get camera resolution\n");
        delete camera_cap;
        camera_cap = NULL;
        return -1;
    }

    // Allocate Mat objects for frames
    current_frame = new Mat();
    preview_frame = new Mat();

    // Allocate buffer for full resolution frame data
    camera_frame_full = (uint8_t*)malloc(full_width * full_height * 3);
    if (!camera_frame_full) {
        fprintf(stderr, "Failed to allocate full frame buffer\n");
        delete current_frame;
        delete preview_frame;
        delete camera_cap;
        camera_cap = NULL;
        return -1;
    }

    printf("Camera initialized successfully with OpenCV\n");
    printf("  Backend: V4L2\n");
    printf("  Native Resolution: %dx%d (auto-detected maximum)\n", full_width, full_height);
    printf("  Preview: %dx%d BGR (for LVGL RGB888 display)\n", CAMERA_WIDTH, CAMERA_HEIGHT);
    printf("  Capture: %dx%d RGB (for JPEG full resolution)\n", full_width, full_height);

    return 0;
}

/**
 * Cleanup camera resources
 */
void camera_cleanup(void)
{
    printf("Starting camera cleanup...\n");

    // Step 1: First, make sure camera thread is stopped
    if (camera_running) {
        printf("Camera still running, stopping it...\n");
        camera_stop();
    }

    // Step 2: Give system a moment to stabilize
    printf("Stabilizing device...\n");
    usleep(200000);  // 200ms delay

    // Step 3: Free Mat objects
    if (current_frame) {
        printf("Freeing current frame...\n");
        delete current_frame;
        current_frame = NULL;
    }

    if (preview_frame) {
        printf("Freeing preview frame...\n");
        delete preview_frame;
        preview_frame = NULL;
    }

    // Step 4: Release camera capture
    if (camera_cap) {
        printf("Releasing camera capture...\n");
        camera_cap->release();
        delete camera_cap;
        camera_cap = NULL;

        printf("Device released, waiting for driver to stabilize...\n");
        usleep(500000);  // 500ms delay for device reset
        printf("Device released successfully\n");
    }

    // Step 5: Free camera frame data buffers
    if (camera_frame_full) {
        printf("Freeing full resolution frame buffer...\n");
        free(camera_frame_full);
        camera_frame_full = NULL;
    }

    if (camera_frame_data) {
        printf("Freeing preview frame buffer...\n");
        free(camera_frame_data);
        camera_frame_data = NULL;
    }

    // Step 6: Reset state variables
    camera_running = false;

    printf("Camera cleanup complete\n");
}

/**
 * Camera thread - continuously captures frames using OpenCV
 */
static void *camera_thread_func(void *arg)
{
    (void)arg;

    // Enable thread cancellation
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    Mat frame_bgr;  // OpenCV captures in BGR format by default
    Mat frame_rgb;  // RGB format for JPEG saving
    Mat frame_bgr_preview;  // Downscaled preview in BGR for LVGL RGB888

    while (camera_running) {
        // Add cancellation point
        pthread_testcancel();

        // Capture frame from camera with grab/retrieve for interruptibility
        // This allows us to check camera_running between grab and retrieve
        bool grabbed = camera_cap->grab();

        // Check if we should exit before retrieving
        if (!camera_running) {
            break;
        }

        if (!grabbed) {
            fprintf(stderr, "Error grabbing frame from camera\n");
            pthread_testcancel();
            usleep(10000);  // Sleep 10ms before retry
            continue;
        }

        // Retrieve the grabbed frame
        if (!camera_cap->retrieve(frame_bgr)) {
            fprintf(stderr, "Error retrieving frame from camera\n");
            pthread_testcancel();
            usleep(10000);  // Sleep 10ms before retry
            continue;
        }

        // Check if frame is empty
        if (frame_bgr.empty()) {
            fprintf(stderr, "Empty frame received\n");
            pthread_testcancel();
            continue;
        }

        // Convert BGR to RGB for JPEG saving (full resolution)
        cvtColor(frame_bgr, frame_rgb, COLOR_BGR2RGB);

        // Create downscaled preview version (keep as BGR for LVGL RGB888)
        resize(frame_bgr, frame_bgr_preview, Size(CAMERA_WIDTH, CAMERA_HEIGHT), 0, 0, INTER_LINEAR);

        // Copy frame data to buffers
        pthread_mutex_lock(&frame_mutex);

        // Copy preview frame (downscaled to CAMERA_WIDTH x CAMERA_HEIGHT)
        // Keep as BGR for LVGL's RGB888 format (which expects BGR byte order)
        if (frame_bgr_preview.isContinuous()) {
            memcpy(camera_frame_data, frame_bgr_preview.data, CAMERA_WIDTH * CAMERA_HEIGHT * 3);
        } else {
            // If not continuous, copy row by row
            for (int y = 0; y < CAMERA_HEIGHT; y++) {
                memcpy(camera_frame_data + y * CAMERA_WIDTH * 3,
                       frame_bgr_preview.ptr(y),
                       CAMERA_WIDTH * 3);
            }
        }

        // Copy full resolution frame (RGB for JPEG)
        if (frame_rgb.isContinuous()) {
            memcpy(camera_frame_full, frame_rgb.data, full_width * full_height * 3);
        } else {
            // If not continuous, copy row by row
            for (int y = 0; y < full_height; y++) {
                memcpy(camera_frame_full + y * full_width * 3,
                       frame_rgb.ptr(y),
                       full_width * 3);
            }
        }

        pthread_mutex_unlock(&frame_mutex);

        // Add cancellation point at end of loop iteration
        pthread_testcancel();
    }

    printf("Camera thread exiting cleanly\n");
    return NULL;
}

/**
 * Start camera capture thread
 */
int camera_start(void)
{
    if (!camera_cap) {
        fprintf(stderr, "Camera not available - using test pattern mode\n");
        camera_running = true;  // Enable running flag for test pattern mode
        printf("Test pattern mode started\n");
        return 0;
    }

    camera_running = true;
    if (pthread_create(&camera_thread, NULL, camera_thread_func, NULL) != 0) {
        perror("pthread_create");
        camera_running = false;
        return -1;
    }

    printf("Camera capture started with OpenCV\n");
    return 0;
}

/**
 * Stop camera capture thread
 */
void camera_stop(void)
{
    if (camera_running) {
        printf("Stopping camera capture thread...\n");
        camera_running = false;

        // Wait for thread to finish with a manual timeout mechanism
        if (camera_cap) {
            printf("Waiting for camera thread to join...\n");

            // Try to join with a shorter timeout since we're using grab/retrieve
            int timeout_ms = 500;  // 500ms timeout (one or two frame intervals)
            int elapsed_ms = 0;
            int join_result = pthread_tryjoin_np(camera_thread, NULL);

            while (join_result == EBUSY && elapsed_ms < timeout_ms) {
                usleep(50000);  // Sleep 50ms
                elapsed_ms += 50;
                join_result = pthread_tryjoin_np(camera_thread, NULL);
            }

            if (join_result == EBUSY) {
                fprintf(stderr, "Warning: Camera thread join timed out after %dms, canceling thread...\n", timeout_ms);
                pthread_cancel(camera_thread);
                pthread_join(camera_thread, NULL);  // Wait for cancellation to complete
            } else if (join_result != 0 && join_result != EBUSY) {
                fprintf(stderr, "Warning: pthread_tryjoin_np failed with code %d\n", join_result);
            } else {
                printf("Camera thread joined successfully\n");
            }
        }
        printf("Camera capture stopped\n");
    }
}

/**
 * Get pointer to the latest camera frame
 */
uint8_t *camera_get_frame(void)
{
    return camera_frame_data;
}

/**
 * Check if camera is running
 */
bool camera_is_running(void)
{
    return camera_running;
}

/**
 * Save current frame to file in JPEG format at full camera resolution
 */
int camera_save_photo(const char *filename)
{
    if (!camera_frame_full) {
        fprintf(stderr, "No camera data available\n");
        return -1;
    }

    // JPEG compression structure
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE *fp;
    JSAMPROW row_pointer[1];
    int row_stride;

    // Open output file
    fp = fopen(filename, "wb");
    if (!fp) {
        perror("Failed to create file");
        return -1;
    }

    // Initialize JPEG compression
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, fp);

    // Set image parameters (use full resolution)
    cinfo.image_width = full_width;
    cinfo.image_height = full_height;
    cinfo.input_components = 3;           // RGB
    cinfo.in_color_space = JCS_RGB;

    // Set default compression parameters
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 95, TRUE);   // Higher quality for full-res

    // Start compression
    jpeg_start_compress(&cinfo, TRUE);

    // Write scanlines
    row_stride = full_width * 3;
    pthread_mutex_lock(&frame_mutex);

    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = &camera_frame_full[cinfo.next_scanline * row_stride];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    pthread_mutex_unlock(&frame_mutex);

    // Finish compression
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(fp);

    printf("Photo saved: %s (%dx%d)\n", filename, full_width, full_height);
    return 0;
}

/**
 * Get camera frame dimensions
 */
void camera_get_dimensions(int *width, int *height)
{
    if (width) *width = CAMERA_WIDTH;
    if (height) *height = CAMERA_HEIGHT;
}
