#ifndef TAB_OPENCV_H
#define TAB_OPENCV_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

// Create OpenCV tab
void create_opencv_tab(lv_obj_t * parent);

// OpenCV test functions
void opencv_image_processing_demo(void);
void opencv_camera_demo(void);
void opencv_video_demo(void);

#ifdef __cplusplus
}
#endif

#endif // TAB_OPENCV_H 