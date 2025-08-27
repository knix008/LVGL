#ifndef TAB_WEBCAM_H
#define TAB_WEBCAM_H

#include "lvgl.h"
// No external OpenCV GUI/event dependencies needed

// Initialize webcam tab
void tab_webcam_init(lv_obj_t* parent);

// Clean up webcam tab
void tab_webcam_cleanup(void);

// Get webcam tab object
lv_obj_t* tab_webcam_get_tab(void);

// (Removed) Detection update API; tab shows raw messages only now

// Update webcam tab status
void tab_webcam_update_status(const char* status_message);

// Start webcam processing
void tab_webcam_start_webcam(void);

// Stop webcam processing
void tab_webcam_stop_processing(void);

// Set webcam processing mode (no-op since we only display messages)
void tab_webcam_set_mode(int mode);

#endif // TAB_WEBCAM_H
