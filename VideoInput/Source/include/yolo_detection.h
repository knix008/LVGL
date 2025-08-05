#ifndef YOLO_DETECTION_H
#define YOLO_DETECTION_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

// YOLOv8 detection result structure
typedef struct {
    int class_id;
    float confidence;
    float x1, y1, x2, y2;  // Bounding box coordinates
    char class_name[64];
} yolo_detection_t;

// YOLOv8 detection results container
typedef struct {
    yolo_detection_t* detections;
    int num_detections;
    int max_detections;
} yolo_results_t;

// Initialize YOLOv8 model
int yolo_init(const char* model_path);

// Perform object detection on image
yolo_results_t* yolo_detect(const char* image_path);

// Perform object detection on OpenCV Mat
yolo_results_t* yolo_detect_mat(void* mat_ptr);

// Free detection results
void yolo_free_results(yolo_results_t* results);

// Get class name by ID
const char* yolo_get_class_name(int class_id);

// Draw detections on image and save
int yolo_draw_detections(const char* input_path, const char* output_path, yolo_results_t* results);

// YOLOv8 demo functions
void yolo_image_demo(void);
void yolo_camera_demo(void);
void yolo_video_demo(void);

#ifdef __cplusplus
}
#endif

#endif // YOLO_DETECTION_H 