#ifndef UI_KOREAN_H
#define UI_KOREAN_H

#include "lvgl.h"
#include "korean.h"

// Korean input display buffer size
#define KOREAN_DISPLAY_MAX 256

// Function prototypes
void create_korean_input_tab(lv_obj_t *parent);
void korean_key_event_cb(lv_event_t *e);
void korean_clear_cb(lv_event_t *e);
void korean_backspace_cb(lv_event_t *e);
void korean_space_cb(lv_event_t *e);
void korean_enter_cb(lv_event_t *e);

// Global variables for Korean input
extern lv_obj_t *korean_display_label;
extern lv_obj_t *korean_input_label;
extern char korean_input_buffer[KOREAN_DISPLAY_MAX];
extern wchar_t korean_output_buffer[KOREAN_DISPLAY_MAX];
extern size_t korean_input_len;

#endif // UI_KOREAN_H
