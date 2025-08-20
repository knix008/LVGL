#ifndef TAB_OPENCV_H
#define TAB_OPENCV_H

#include "lvgl.h"
// No external OpenCV GUI/event dependencies needed

// Initialize OpenCV tab
void tab_opencv_init(lv_obj_t* parent);

// Clean up OpenCV tab
void tab_opencv_cleanup(void);

// Get OpenCV tab object
lv_obj_t* tab_opencv_get_tab(void);

// (Removed) Detection update API; tab shows raw messages only now

// Update OpenCV tab status
void tab_opencv_update_status(const char* status_message);

// Start OpenCV processing (webcam)
void tab_opencv_start_webcam(void);

// Stop OpenCV processing
void tab_opencv_stop_processing(void);

// Set OpenCV processing mode (no-op since we only display messages)
void tab_opencv_set_mode(int mode);

#endif // TAB_OPENCV_H
