/*
 * japanese_gui.h
 * Japanese Input Method Editor - GUI Components
 */

#ifndef JAPANESE_GUI_H
#define JAPANESE_GUI_H

#include "lvgl/lvgl.h"
#include "japanese_qwerty.h"

// GUI State structure
typedef struct {
    IMEState *ime_state;
    
    // UI Components
    lv_obj_t *screen;
    lv_obj_t *text_area;
    lv_obj_t *mode_label;
    lv_obj_t *buffer_label;
    lv_obj_t *keyboard_container;
    
    // Fonts
    const lv_font_t *japanese_font;
    
    // Keyboard buttons
    lv_obj_t *key_buttons[50];
    lv_obj_t *shift_button;
    int num_buttons;
    
    // Input state
    bool shift_pressed;
    bool number_mode;
} GUIState;

// Function declarations
void gui_init(GUIState *state, IMEState *ime_state);
bool gui_load_fonts(GUIState *state);
void gui_create_ui(GUIState *state);
void gui_update_display(GUIState *state);
void gui_create_qwerty_keyboard(GUIState *state, lv_obj_t *parent);
void gui_update_keyboard_labels(GUIState *state);
void gui_button_event_cb(lv_event_t *e);
void gui_mode_button_event_cb(lv_event_t *e);
void gui_shift_button_event_cb(lv_event_t *e);
void gui_cycle_mode_button_event_cb(lv_event_t *e);
void gui_special_button_event_cb(lv_event_t *e);

#endif // JAPANESE_GUI_H

