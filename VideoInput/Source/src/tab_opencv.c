#include "tab_opencv.h"
#include <stdio.h>
#include <string.h>
#include <opencv4/opencv2/opencv.hpp>

// Global variables for OpenCV tab
static lv_obj_t * g_opencv_status_label = NULL;
static lv_obj_t * g_opencv_result_label = NULL;

// OpenCV callback functions
static void opencv_image_processing_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("OpenCV Tab: Image processing demo started\n");
        opencv_image_processing_demo();
        if (g_opencv_status_label != NULL) {
            lv_label_set_text(g_opencv_status_label, "Status: Image processing completed");
        }
    }
}

static void opencv_camera_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("OpenCV Tab: Camera demo started\n");
        opencv_camera_demo();
        if (g_opencv_status_label != NULL) {
            lv_label_set_text(g_opencv_status_label, "Status: Camera demo completed");
        }
    }
}

static void opencv_video_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("OpenCV Tab: Video demo started\n");
        opencv_video_demo();
        if (g_opencv_status_label != NULL) {
            lv_label_set_text(g_opencv_status_label, "Status: Video demo completed");
        }
    }
}

// Create OpenCV tab
void create_opencv_tab(lv_obj_t * parent) {
    // Create title label
    lv_obj_t * title = lv_label_create(parent);
    lv_label_set_text(title, "OpenCV Functions");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    
    // Create status label
    g_opencv_status_label = lv_label_create(parent);
    lv_label_set_text(g_opencv_status_label, "Status: Ready");
    lv_obj_align(g_opencv_status_label, LV_ALIGN_TOP_MID, 0, 40);
    
    // Create result label
    g_opencv_result_label = lv_label_create(parent);
    lv_label_set_text(g_opencv_result_label, "OpenCV 4.8.0 - Local Build");
    lv_obj_set_style_text_color(g_opencv_result_label, lv_color_hex(0x00FF00), 0); // Green color
    lv_obj_align(g_opencv_result_label, LV_ALIGN_TOP_MID, 0, 70);
    
    // Create button container
    lv_obj_t * btn_container = lv_obj_create(parent);
    lv_obj_set_size(btn_container, 280, 160);
    lv_obj_align(btn_container, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_pad_all(btn_container, 10, 0);
    lv_obj_set_style_bg_color(btn_container, lv_color_hex(0xF0F0F0), 0);
    lv_obj_set_style_border_width(btn_container, 1, 0);
    lv_obj_set_style_border_color(btn_container, lv_color_hex(0xCCCCCC), 0);
    
    // Create Image Processing button
    lv_obj_t * btn_image = lv_btn_create(btn_container);
    lv_obj_set_size(btn_image, 120, 50);
    lv_obj_align(btn_image, LV_ALIGN_TOP_LEFT, 0, 0);
    lv_obj_add_event_cb(btn_image, opencv_image_processing_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * btn_image_label = lv_label_create(btn_image);
    lv_label_set_text(btn_image_label, "Image Proc");
    lv_obj_center(btn_image_label);
    
    // Create Camera button
    lv_obj_t * btn_camera = lv_btn_create(btn_container);
    lv_obj_set_size(btn_camera, 120, 50);
    lv_obj_align(btn_camera, LV_ALIGN_TOP_RIGHT, 0, 0);
    lv_obj_add_event_cb(btn_camera, opencv_camera_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * btn_camera_label = lv_label_create(btn_camera);
    lv_label_set_text(btn_camera_label, "Camera");
    lv_obj_center(btn_camera_label);
    
    // Create Video button
    lv_obj_t * btn_video = lv_btn_create(btn_container);
    lv_obj_set_size(btn_video, 120, 50);
    lv_obj_align(btn_video, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    lv_obj_add_event_cb(btn_video, opencv_video_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t * btn_video_label = lv_label_create(btn_video);
    lv_label_set_text(btn_video_label, "Video");
    lv_obj_center(btn_video_label);
    
    // Create Info button
    lv_obj_t * btn_info = lv_btn_create(btn_container);
    lv_obj_set_size(btn_info, 120, 50);
    lv_obj_align(btn_info, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
    
    lv_obj_t * btn_info_label = lv_label_create(btn_info);
    lv_label_set_text(btn_info_label, "Info");
    lv_obj_center(btn_info_label);
    
    printf("OpenCV Tab: Created successfully\n");
}

// OpenCV image processing demo
void opencv_image_processing_demo(void) {
    printf("OpenCV Demo: Starting image processing demo\n");
    
    // Create a test image
    cv::Mat test_image = cv::Mat::zeros(200, 200, CV_8UC3);
    
    // Draw some shapes
    cv::circle(test_image, cv::Point(100, 100), 50, cv::Scalar(0, 255, 0), -1);
    cv::rectangle(test_image, cv::Point(50, 50), cv::Point(150, 150), cv::Scalar(255, 0, 0), 2);
    cv::line(test_image, cv::Point(0, 0), cv::Point(200, 200), cv::Scalar(0, 0, 255), 3);
    
    // Apply some image processing
    cv::Mat gray_image;
    cv::cvtColor(test_image, gray_image, cv::COLOR_BGR2GRAY);
    
    cv::Mat blurred_image;
    cv::GaussianBlur(gray_image, blurred_image, cv::Size(15, 15), 0);
    
    cv::Mat edges;
    cv::Canny(blurred_image, edges, 50, 150);
    
    printf("OpenCV Demo: Image processing completed\n");
    printf("OpenCV Demo: Original image size: %dx%d\n", test_image.cols, test_image.rows);
    printf("OpenCV Demo: Gray image size: %dx%d\n", gray_image.cols, gray_image.rows);
    printf("OpenCV Demo: Edges detected: %d pixels\n", cv::countNonZero(edges));
    
    if (g_opencv_result_label != NULL) {
        lv_label_set_text(g_opencv_result_label, "Image processing: OK");
    }
}

// OpenCV camera demo
void opencv_camera_demo(void) {
    printf("OpenCV Demo: Starting camera demo\n");
    
    // Try to open camera
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        printf("OpenCV Demo: Could not open camera\n");
        if (g_opencv_result_label != NULL) {
            lv_label_set_text(g_opencv_result_label, "Camera: Not available");
        }
        return;
    }
    
    // Get camera properties
    double fps = cap.get(cv::CAP_PROP_FPS);
    int width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    
    printf("OpenCV Demo: Camera opened successfully\n");
    printf("OpenCV Demo: Camera FPS: %.2f\n", fps);
    printf("OpenCV Demo: Camera resolution: %dx%d\n", width, height);
    
    // Capture a single frame
    cv::Mat frame;
    cap >> frame;
    
    if (frame.empty()) {
        printf("OpenCV Demo: Could not capture frame\n");
        if (g_opencv_result_label != NULL) {
            lv_label_set_text(g_opencv_result_label, "Camera: No frame");
        }
    } else {
        printf("OpenCV Demo: Frame captured successfully\n");
        printf("OpenCV Demo: Frame size: %dx%d\n", frame.cols, frame.rows);
        
        // Apply some processing to the frame
        cv::Mat gray_frame;
        cv::cvtColor(frame, gray_frame, cv::COLOR_BGR2GRAY);
        
        cv::Mat blurred_frame;
        cv::GaussianBlur(gray_frame, blurred_frame, cv::Size(5, 5), 0);
        
        printf("OpenCV Demo: Frame processing completed\n");
        
        if (g_opencv_result_label != NULL) {
            lv_label_set_text(g_opencv_result_label, "Camera: Frame captured");
        }
    }
    
    cap.release();
}

// OpenCV video demo
void opencv_video_demo(void) {
    printf("OpenCV Demo: Starting video demo\n");
    
    // Try to open a video file
    cv::VideoCapture cap("A:../assets/example.mp4");
    if (!cap.isOpened()) {
        printf("OpenCV Demo: Could not open video file\n");
        if (g_opencv_result_label != NULL) {
            lv_label_set_text(g_opencv_result_label, "Video: File not found");
        }
        return;
    }
    
    // Get video properties
    double fps = cap.get(cv::CAP_PROP_FPS);
    int frame_count = cap.get(cv::CAP_PROP_FRAME_COUNT);
    int width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    
    printf("OpenCV Demo: Video opened successfully\n");
    printf("OpenCV Demo: Video FPS: %.2f\n", fps);
    printf("OpenCV Demo: Video frames: %d\n", frame_count);
    printf("OpenCV Demo: Video resolution: %dx%d\n", width, height);
    
    // Read a few frames
    cv::Mat frame;
    int frames_read = 0;
    for (int i = 0; i < 5 && cap.read(frame); i++) {
        frames_read++;
        printf("OpenCV Demo: Read frame %d, size: %dx%d\n", i, frame.cols, frame.rows);
    }
    
    printf("OpenCV Demo: Read %d frames successfully\n", frames_read);
    
    if (g_opencv_result_label != NULL) {
        char result_text[64];
        snprintf(result_text, sizeof(result_text), "Video: %d frames read", frames_read);
        lv_label_set_text(g_opencv_result_label, result_text);
    }
    
    cap.release();
} 