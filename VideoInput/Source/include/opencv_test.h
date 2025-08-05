#ifndef OPENCV_TEST_H
#define OPENCV_TEST_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

// Basic OpenCV test function
void opencv_test_function(void);

// Function to set OpenCV UI references
void opencv_set_ui_references(lv_obj_t* status_label, lv_obj_t* result_label, 
                              lv_obj_t* image_display, lv_obj_t* image_container);

// OpenCV demo functions
void opencv_image_processing_demo(void);
void opencv_camera_demo(void);
void opencv_video_demo(void);

// YOLOv8 demo functions
void opencv_yolo_image_demo(void);
void opencv_yolo_camera_demo(void);
void opencv_yolo_video_demo(void);

#ifdef __cplusplus
}
#endif

#endif // OPENCV_TEST_H 