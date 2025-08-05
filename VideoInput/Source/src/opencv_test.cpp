#include <stdio.h>
#include <string.h>
#include <opencv4/opencv2/opencv.hpp>
#include "lvgl.h"
#include "yolo_detection.h"

extern "C" {

// Global variables for OpenCV functions
static lv_obj_t * g_opencv_status_label = NULL;
static lv_obj_t * g_opencv_result_label = NULL;
static lv_obj_t * g_opencv_image_display = NULL;
static lv_obj_t * g_opencv_image_container = NULL;

// Function to set OpenCV UI references
void opencv_set_ui_references(lv_obj_t* status_label, lv_obj_t* result_label, 
                              lv_obj_t* image_display, lv_obj_t* image_container) {
    g_opencv_status_label = status_label;
    g_opencv_result_label = result_label;
    g_opencv_image_display = image_display;
    g_opencv_image_container = image_container;
}

// Function to save and display processed image
static void save_and_display_image(const cv::Mat& image, const char* title, const char* filename) {
    if (g_opencv_image_container == NULL) {
        printf("OpenCV: Error - Image container not initialized\n");
        return;
    }
    
    // Update title
    lv_obj_t* title_label = lv_obj_get_child(g_opencv_image_container, 0);
    if (title_label != NULL) {
        lv_label_set_text(title_label, title);
    }
    
    // Save the processed image as JPEG
    std::string output_path = "../assets/";
    output_path += filename;
    
    std::vector<int> compression_params;
    compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
    compression_params.push_back(95); // High quality
    
    bool save_success = cv::imwrite(output_path, image, compression_params);
    
    if (!save_success) {
        printf("OpenCV: Error - Failed to save image to %s\n", output_path.c_str());
        return;
    }
    
    printf("OpenCV: Saved processed image to %s\n", output_path.c_str());
    
    // Create LVGL file path
    std::string lvgl_path = "A:";
    lvgl_path += output_path;
    
    // Display the saved image using LVGL
    if (g_opencv_image_display != NULL) {
        lv_img_set_src(g_opencv_image_display, lvgl_path.c_str());
        printf("OpenCV: Displayed image from %s\n", lvgl_path.c_str());
    } else {
        printf("OpenCV: Error - Image display widget not initialized\n");
    }
}

// Function to display OpenCV processing results with saved image
static void display_opencv_results_with_image(const cv::Mat& image, const char* title, const char* info, const char* filename) {
    if (g_opencv_image_container == NULL) {
        printf("OpenCV: Error - Image container not initialized\n");
        return;
    }
    
    // Save and display the image
    save_and_display_image(image, title, filename);
    
    // Show info text below the image
    lv_obj_t* info_label = lv_label_create(g_opencv_image_container);
    lv_label_set_text(info_label, info);
    lv_obj_align(info_label, LV_ALIGN_BOTTOM_MID, 0, -10);
    
    printf("OpenCV: Displayed image results: %s - %s\n", title, info);
}

// Basic OpenCV test function
void opencv_test_function(void) {
    printf("OpenCV Test: OpenCV version %s\n", CV_VERSION);
    
    // Create a simple test image
    cv::Mat test_image = cv::Mat::zeros(100, 100, CV_8UC3);
    cv::circle(test_image, cv::Point(50, 50), 30, cv::Scalar(0, 255, 0), -1);
    
    printf("OpenCV Test: Created test image successfully\n");
    printf("OpenCV Test: Image size: %dx%d\n", test_image.cols, test_image.rows);
    printf("OpenCV Test: Image channels: %d\n", test_image.channels());
    
    // Test basic OpenCV functions
    cv::Mat gray_image;
    cv::cvtColor(test_image, gray_image, cv::COLOR_BGR2GRAY);
    printf("OpenCV Test: Converted to grayscale successfully\n");
    printf("OpenCV Test: All OpenCV functions working correctly\n");
}

// OpenCV image processing demo with Lenna.png
void opencv_image_processing_demo(void) {
    printf("OpenCV Demo: Starting image processing demo with Lenna.png\n");
    
    if (g_opencv_status_label != NULL) {
        lv_label_set_text(g_opencv_status_label, "Status: Loading Lenna.png...");
    }
    
    // Load Lenna.png from assets directory
    std::string image_path = "../assets/Lenna.png";
    cv::Mat original_image = cv::imread(image_path, cv::IMREAD_COLOR);
    
    if (original_image.empty()) {
        printf("OpenCV Demo: Error - Could not load Lenna.png from %s\n", image_path.c_str());
        if (g_opencv_status_label != NULL) {
            lv_label_set_text(g_opencv_status_label, "Status: Error loading image");
        }
        if (g_opencv_result_label != NULL) {
            lv_label_set_text(g_opencv_result_label, "Failed to load Lenna.png");
        }
        return;
    }
    
    printf("OpenCV Demo: Successfully loaded Lenna.png (%dx%d)\n", original_image.cols, original_image.rows);
    
    if (g_opencv_status_label != NULL) {
        lv_label_set_text(g_opencv_status_label, "Status: Processing image...");
    }
    
    // Convert to grayscale
    cv::Mat gray_image;
    cv::cvtColor(original_image, gray_image, cv::COLOR_BGR2GRAY);
    
    // Apply edge detection
    cv::Mat edges;
    cv::Canny(gray_image, edges, 50, 150);
    
    // Apply Gaussian blur
    cv::Mat blurred;
    cv::GaussianBlur(gray_image, blurred, cv::Size(5, 5), 0);
    
    // Apply threshold
    cv::Mat threshold_img;
    cv::threshold(gray_image, threshold_img, 128, 255, cv::THRESH_BINARY);
    
    printf("OpenCV Demo: Image processing completed\n");
    printf("OpenCV Demo: Original image size: %dx%d\n", original_image.cols, original_image.rows);
    printf("OpenCV Demo: Gray image size: %dx%d\n", gray_image.cols, gray_image.rows);
    printf("OpenCV Demo: Edges detected: %d pixels\n", cv::countNonZero(edges));
    printf("OpenCV Demo: Threshold pixels: %d\n", cv::countNonZero(threshold_img));
    
    char info_text[512];
    snprintf(info_text, sizeof(info_text),
             "Lenna.png Processing Results:\n"
             "Original: %dx%d pixels\n"
             "Grayscale: %dx%d pixels\n"
             "Edges detected: %d pixels\n"
             "Threshold pixels: %d\n"
             "File: %s",
             original_image.cols, original_image.rows,
             gray_image.cols, gray_image.rows,
             cv::countNonZero(edges),
             cv::countNonZero(threshold_img),
             image_path.c_str());
    
    // Display the original image
    display_opencv_results_with_image(original_image, "Lenna.png - Original", info_text, "lenna_original.jpg");
    
    // Wait a moment, then show grayscale
    lv_timer_handler();
    lv_timer_handler();
    
    // Display grayscale image
    char gray_info[256];
    snprintf(gray_info, sizeof(gray_info), "Grayscale conversion of Lenna.png");
    display_opencv_results_with_image(gray_image, "Lenna.png - Grayscale", gray_info, "lenna_grayscale.jpg");
    
    // Wait a moment, then show edges
    lv_timer_handler();
    lv_timer_handler();
    
    // Display edge detection result
    char edge_info[256];
    snprintf(edge_info, sizeof(edge_info), "Edge detection: %d pixels", cv::countNonZero(edges));
    display_opencv_results_with_image(edges, "Lenna.png - Edge Detection", edge_info, "lenna_edges.jpg");
    
    if (g_opencv_result_label != NULL) {
        lv_label_set_text(g_opencv_result_label, "Lenna.png processed successfully");
    }
    
    if (g_opencv_status_label != NULL) {
        lv_label_set_text(g_opencv_status_label, "Status: Ready");
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
        
        cv::Mat edges_frame;
        cv::Canny(blurred_frame, edges_frame, 50, 150);
        
        char camera_info[256];
        snprintf(camera_info, sizeof(camera_info),
                 "Camera Capture Results:\n"
                 "Frame: %dx%d pixels\n"
                 "FPS: %.2f\n"
                 "Resolution: %dx%d",
                 frame.cols, frame.rows, fps, width, height);
        
        display_opencv_results_with_image(frame, "Camera Capture", camera_info, "camera_capture.jpg");
        
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
    std::string video_path = "../assets/example.mp4";
    cv::VideoCapture cap(video_path);
    
    if (!cap.isOpened()) {
        printf("OpenCV Demo: Could not open video file %s\n", video_path.c_str());
        if (g_opencv_result_label != NULL) {
            lv_label_set_text(g_opencv_result_label, "Video: File not found");
        }
        return;
    }
    
    // Get video properties
    double fps = cap.get(cv::CAP_PROP_FPS);
    int width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    int frame_count = cap.get(cv::CAP_PROP_FRAME_COUNT);
    
    printf("OpenCV Demo: Video opened successfully\n");
    printf("OpenCV Demo: Video FPS: %.2f\n", fps);
    printf("OpenCV Demo: Video resolution: %dx%d\n", width, height);
    printf("OpenCV Demo: Total frames: %d\n", frame_count);
    
    // Read a few frames
    int frames_read = 0;
    cv::Mat frame;
    
    for (int i = 0; i < 10 && frames_read < 5; i++) {
        cap >> frame;
        if (!frame.empty()) {
            frames_read++;
        }
    }
    
    if (frame.empty()) {
        printf("OpenCV Demo: Could not read video frames\n");
        if (g_opencv_result_label != NULL) {
            lv_label_set_text(g_opencv_result_label, "Video: No frames");
        }
    } else {
        printf("OpenCV Demo: Video frame read successfully\n");
        printf("OpenCV Demo: Frame size: %dx%d\n", frame.cols, frame.rows);
        
        char video_info[256];
        snprintf(video_info, sizeof(video_info),
                 "Video Processing Results:\n"
                 "Frames read: %d\n"
                 "Total frames: %d\n"
                 "FPS: %.2f\n"
                 "Resolution: %dx%d",
                 frames_read, frame_count, fps, width, height);
        
        display_opencv_results_with_image(frame, "Video Processing", video_info, "video_frame.jpg");
        
        if (g_opencv_result_label != NULL) {
            lv_label_set_text(g_opencv_result_label, "Video: Frame processed");
        }
    }
    
    cap.release();
}

// YOLOv8 demo functions integrated with OpenCV
void opencv_yolo_image_demo(void) {
    printf("OpenCV YOLO: Starting YOLOv8 image detection demo\n");
    
    if (g_opencv_status_label != NULL) {
        lv_label_set_text(g_opencv_status_label, "Status: Loading YOLOv8 model...");
    }
    
    // Initialize YOLOv8 model
    if (!yolo_init("../models/yolov8n.onnx")) {
        printf("OpenCV YOLO: Failed to initialize YOLOv8 model\n");
        if (g_opencv_status_label != NULL) {
            lv_label_set_text(g_opencv_status_label, "Status: YOLOv8 model failed to load");
        }
        return;
    }
    
    if (g_opencv_status_label != NULL) {
        lv_label_set_text(g_opencv_status_label, "Status: Running YOLOv8 detection...");
    }
    
    // Perform YOLOv8 detection on Lenna.png
    yolo_results_t* results = yolo_detect("../assets/Lenna.png");
    if (results) {
        printf("OpenCV YOLO: Found %d detections\n", results->num_detections);
        
        // Draw and save results
        yolo_draw_detections("../assets/Lenna.png", "../assets/lenna_yolo_detections.jpg", results);
        
        // Display results
        char info_text[512];
        snprintf(info_text, sizeof(info_text),
                 "YOLOv8 Detection Results:\n"
                 "Detections: %d\n"
                 "Model: YOLOv8n\n"
                 "Input: Lenna.png\n"
                 "Output: lenna_yolo_detections.jpg",
                 results->num_detections);
        
        display_opencv_results_with_image(cv::imread("../assets/lenna_yolo_detections.jpg"), 
                                        "YOLOv8 Detection", info_text, "lenna_yolo_detections.jpg");
        
        // Free results
        yolo_free_results(results);
        
        if (g_opencv_result_label != NULL) {
            lv_label_set_text(g_opencv_result_label, "YOLOv8 detection completed successfully");
        }
    }
    
    if (g_opencv_status_label != NULL) {
        lv_label_set_text(g_opencv_status_label, "Status: Ready");
    }
}

void opencv_yolo_camera_demo(void) {
    printf("OpenCV YOLO: Starting YOLOv8 camera detection demo\n");
    
    if (g_opencv_status_label != NULL) {
        lv_label_set_text(g_opencv_status_label, "Status: Loading YOLOv8 model...");
    }
    
    // Initialize YOLOv8 model
    if (!yolo_init("../models/yolov8n.onnx")) {
        printf("OpenCV YOLO: Failed to initialize YOLOv8 model\n");
        if (g_opencv_status_label != NULL) {
            lv_label_set_text(g_opencv_status_label, "Status: YOLOv8 model failed to load");
        }
        return;
    }
    
    if (g_opencv_status_label != NULL) {
        lv_label_set_text(g_opencv_status_label, "Status: Running YOLOv8 camera detection...");
    }
    
    // Try to open camera
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        printf("OpenCV YOLO: Could not open camera\n");
        if (g_opencv_result_label != NULL) {
            lv_label_set_text(g_opencv_result_label, "Camera: Not available");
        }
        return;
    }
    
    // Capture a frame
    cv::Mat frame;
    cap >> frame;
    
    if (!frame.empty()) {
        // Perform YOLOv8 detection
        yolo_results_t* results = yolo_detect_mat(&frame);
        if (results) {
            printf("OpenCV YOLO: Camera detection completed, found %d objects\n", results->num_detections);
            
            // Draw detections on frame
            for (int i = 0; i < results->num_detections; i++) {
                yolo_detection_t* det = &results->detections[i];
                cv::Point pt1(det->x1, det->y1);
                cv::Point pt2(det->x2, det->y2);
                cv::rectangle(frame, pt1, pt2, cv::Scalar(0, 255, 0), 2);
                
                char label[128];
                snprintf(label, sizeof(label), "%s %.2f", det->class_name, det->confidence);
                cv::putText(frame, label, cv::Point(det->x1, det->y1 - 10),
                           cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);
            }
            
            // Save and display result
            cv::imwrite("../assets/camera_yolo_detections.jpg", frame);
            
            char camera_info[256];
            snprintf(camera_info, sizeof(camera_info),
                     "YOLOv8 Camera Detection:\n"
                     "Detections: %d\n"
                     "Frame: %dx%d\n"
                     "Model: YOLOv8n",
                     results->num_detections, frame.cols, frame.rows);
            
            display_opencv_results_with_image(frame, "YOLOv8 Camera Detection", camera_info, "camera_yolo_detections.jpg");
            
            yolo_free_results(results);
            
            if (g_opencv_result_label != NULL) {
                lv_label_set_text(g_opencv_result_label, "YOLOv8 camera detection completed");
            }
        }
    }
    
    cap.release();
    
    if (g_opencv_status_label != NULL) {
        lv_label_set_text(g_opencv_status_label, "Status: Ready");
    }
}

void opencv_yolo_video_demo(void) {
    printf("OpenCV YOLO: Starting YOLOv8 video detection demo\n");
    
    if (g_opencv_status_label != NULL) {
        lv_label_set_text(g_opencv_status_label, "Status: Loading YOLOv8 model...");
    }
    
    // Initialize YOLOv8 model
    if (!yolo_init("../models/yolov8n.onnx")) {
        printf("OpenCV YOLO: Failed to initialize YOLOv8 model\n");
        if (g_opencv_status_label != NULL) {
            lv_label_set_text(g_opencv_status_label, "Status: YOLOv8 model failed to load");
        }
        return;
    }
    
    if (g_opencv_status_label != NULL) {
        lv_label_set_text(g_opencv_status_label, "Status: Running YOLOv8 video detection...");
    }
    
    // Try to open video file
    cv::VideoCapture cap("../assets/example.mp4");
    if (!cap.isOpened()) {
        printf("OpenCV YOLO: Could not open video file\n");
        if (g_opencv_result_label != NULL) {
            lv_label_set_text(g_opencv_result_label, "Video: File not found");
        }
        return;
    }
    
    // Read a frame
    cv::Mat frame;
    cap >> frame;
    
    if (!frame.empty()) {
        // Perform YOLOv8 detection
        yolo_results_t* results = yolo_detect_mat(&frame);
        if (results) {
            printf("OpenCV YOLO: Video detection completed, found %d objects\n", results->num_detections);
            
            // Draw detections on frame
            for (int i = 0; i < results->num_detections; i++) {
                yolo_detection_t* det = &results->detections[i];
                cv::Point pt1(det->x1, det->y1);
                cv::Point pt2(det->x2, det->y2);
                cv::rectangle(frame, pt1, pt2, cv::Scalar(0, 255, 0), 2);
                
                char label[128];
                snprintf(label, sizeof(label), "%s %.2f", det->class_name, det->confidence);
                cv::putText(frame, label, cv::Point(det->x1, det->y1 - 10),
                           cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);
            }
            
            // Save and display result
            cv::imwrite("../assets/video_yolo_detections.jpg", frame);
            
            char video_info[256];
            snprintf(video_info, sizeof(video_info),
                     "YOLOv8 Video Detection:\n"
                     "Detections: %d\n"
                     "Frame: %dx%d\n"
                     "Model: YOLOv8n",
                     results->num_detections, frame.cols, frame.rows);
            
            display_opencv_results_with_image(frame, "YOLOv8 Video Detection", video_info, "video_yolo_detections.jpg");
            
            yolo_free_results(results);
            
            if (g_opencv_result_label != NULL) {
                lv_label_set_text(g_opencv_result_label, "YOLOv8 video detection completed");
            }
        }
    }
    
    cap.release();
    
    if (g_opencv_status_label != NULL) {
        lv_label_set_text(g_opencv_status_label, "Status: Ready");
    }
}

} // extern "C" 