/*
 * GUI Application Module Header
 * Handles the Japanese input GUI application interface and event handling
 */

#ifndef GUI_APP_H
#define GUI_APP_H

#include "lvgl/lvgl.h"
#include "japanese_input.h"

// Application widget structure
typedef struct {
    lv_obj_t *text_area;
    lv_obj_t *buttons[12];
    lv_obj_t *mode_button;
    lv_obj_t *clear_button;
    lv_obj_t *shift_button;
    lv_obj_t *enter_button;
    lv_obj_t *backspace_button;
    lv_obj_t *space_button;
    JapaneseInputState state;
    bool previous_shift_state;  // Store shift state before entering number mode
    bool shift_disabled;        // Track if shift button is currently disabled
} AppWidgets;

// GUI Application functions
int gui_app_init(void);
void gui_app_create_ui(void);
void gui_app_cleanup(void);

// Event handlers
void gui_app_on_button_clicked(lv_event_t *e);
void gui_app_on_mode_button_clicked(lv_event_t *e);
void gui_app_on_clear_clicked(lv_event_t *e);
void gui_app_on_shift_clicked(lv_event_t *e);
void gui_app_on_enter_clicked(lv_event_t *e);
void gui_app_on_backspace_clicked(lv_event_t *e);
void gui_app_on_space_clicked(lv_event_t *e);

// Flick input functions
void gui_app_create_flick_window(int button_num);
void gui_app_on_flick_char_clicked(lv_event_t *e);
void gui_app_on_flick_cancel_clicked(lv_event_t *e);

// Font management
const lv_font_t* gui_app_get_japanese_font_12(void);
const lv_font_t* gui_app_get_japanese_font_14(void);
const lv_font_t* gui_app_get_japanese_font_16(void);
const lv_font_t* gui_app_get_japanese_font_20(void);

// Global application widgets access
AppWidgets* gui_app_get_widgets(void);

#endif // GUI_APP_H
