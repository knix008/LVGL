#include "tab_opencv.h"
#include "opencv_test.h"
#include <stdio.h>
#include <string.h>

extern "C" {

// OpenCV callback functions
static void opencv_image_processing_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("OpenCV Tab: Image processing demo started\n");
        opencv_image_processing_demo();
    }
}

static void opencv_camera_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("OpenCV Tab: Camera demo started\n");
        opencv_camera_demo();
    }
}

static void opencv_video_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("OpenCV Tab: Video demo started\n");
        opencv_video_demo();
    }
}

// YOLOv8 callback functions
static void opencv_yolo_image_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("OpenCV Tab: YOLOv8 image detection started\n");
        opencv_yolo_image_demo();
    }
}

static void opencv_yolo_camera_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("OpenCV Tab: YOLOv8 camera detection started\n");
        opencv_yolo_camera_demo();
    }
}

static void opencv_yolo_video_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("OpenCV Tab: YOLOv8 video detection started\n");
        opencv_yolo_video_demo();
    }
}

// Create OpenCV tab
void create_opencv_tab(lv_obj_t * parent) {
    // Create title
    lv_obj_t * title = lv_label_create(parent);
    lv_label_set_text(title, "OpenCV Functions");
    lv_obj_set_style_text_font(title, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_align(title, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 5);
    
    // Create status label
    lv_obj_t * status_label = lv_label_create(parent);
    lv_label_set_text(status_label, "Status: Ready");
    lv_obj_align(status_label, LV_ALIGN_TOP_MID, 0, 20);
    
    // Create result label
    lv_obj_t * result_label = lv_label_create(parent);
    lv_label_set_text(result_label, "OpenCV 4.8.0 - Local Build");
    lv_obj_set_style_text_color(result_label, lv_color_hex(0x00FF00), 0); // Green color
    lv_obj_align(result_label, LV_ALIGN_TOP_MID, 0, 60);
    
    // Create image display container
    lv_obj_t * image_container = lv_obj_create(parent);
    lv_obj_set_size(image_container, 300, 200);
    lv_obj_align(image_container, LV_ALIGN_TOP_MID, 0, 90);
    lv_obj_set_style_pad_all(image_container, 10, 0);
    lv_obj_set_style_bg_color(image_container, lv_color_hex(0xF0F0F0), 0);
    lv_obj_set_style_border_width(image_container, 2, 0);
    lv_obj_set_style_border_color(image_container, lv_color_hex(0xCCCCCC), 0);
    
    // Create image title label
    lv_obj_t * image_title = lv_label_create(image_container);
    lv_label_set_text(image_title, "Processed Image");
    lv_obj_align(image_title, LV_ALIGN_TOP_MID, 0, 5);
    
    // Create image display widget
    lv_obj_t * image_display = lv_img_create(image_container);
    lv_obj_set_size(image_display, 200, 150);
    lv_obj_align(image_display, LV_ALIGN_CENTER, 0, -40);
    
    // Create button container
    lv_obj_t * btn_container = lv_obj_create(parent);
    lv_obj_set_size(btn_container, 280, 200);
    lv_obj_align(btn_container, LV_ALIGN_BOTTOM_MID, 0, -20);
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
    
    // Create YOLOv8 Image button
    lv_obj_t * btn_yolo_image = lv_btn_create(btn_container);
    lv_obj_set_size(btn_yolo_image, 120, 50);
    lv_obj_align(btn_yolo_image, LV_ALIGN_TOP_LEFT, 0, 60);
    lv_obj_add_event_cb(btn_yolo_image, opencv_yolo_image_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_bg_color(btn_yolo_image, lv_color_hex(0xFF6B35), 0); // Orange color
    
    lv_obj_t * btn_yolo_image_label = lv_label_create(btn_yolo_image);
    lv_label_set_text(btn_yolo_image_label, "YOLO Image");
    lv_obj_center(btn_yolo_image_label);
    
    // Create YOLOv8 Camera button
    lv_obj_t * btn_yolo_camera = lv_btn_create(btn_container);
    lv_obj_set_size(btn_yolo_camera, 120, 50);
    lv_obj_align(btn_yolo_camera, LV_ALIGN_TOP_RIGHT, 0, 60);
    lv_obj_add_event_cb(btn_yolo_camera, opencv_yolo_camera_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_bg_color(btn_yolo_camera, lv_color_hex(0xFF6B35), 0); // Orange color
    
    lv_obj_t * btn_yolo_camera_label = lv_label_create(btn_yolo_camera);
    lv_label_set_text(btn_yolo_camera_label, "YOLO Camera");
    lv_obj_center(btn_yolo_camera_label);
    
    // Create YOLOv8 Video button
    lv_obj_t * btn_yolo_video = lv_btn_create(btn_container);
    lv_obj_set_size(btn_yolo_video, 120, 50);
    lv_obj_align(btn_yolo_video, LV_ALIGN_BOTTOM_LEFT, 0, 60);
    lv_obj_add_event_cb(btn_yolo_video, opencv_yolo_video_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_bg_color(btn_yolo_video, lv_color_hex(0xFF6B35), 0); // Orange color
    
    lv_obj_t * btn_yolo_video_label = lv_label_create(btn_yolo_video);
    lv_label_set_text(btn_yolo_video_label, "YOLO Video");
    lv_obj_center(btn_yolo_video_label);
    
    // Set UI references for OpenCV functions
    opencv_set_ui_references(status_label, result_label, image_display, image_container);
    
    printf("OpenCV Tab: Created successfully\n");
}

} // extern "C" 