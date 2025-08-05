#ifndef TAB_WEBCAM_H
#define TAB_WEBCAM_H

#include "lvgl.h"

// ============================================================================
// GUI CONTROLS FUNCTIONS (for video tab integration)
// ============================================================================

// Webcam GUI functions
void webcam_gui_init(void);
void create_webcam_controls(lv_obj_t* parent, lv_obj_t* video_player, 
                           lv_obj_t* status_label, lv_obj_t* device_label);
void update_webcam_gui_status(void);

// ============================================================================
// DEDICATED WEBCAM TAB FUNCTIONS
// ============================================================================

// Dedicated webcam tab functions
void create_webcam_tab(lv_obj_t * parent);

#endif // TAB_WEBCAM_H 