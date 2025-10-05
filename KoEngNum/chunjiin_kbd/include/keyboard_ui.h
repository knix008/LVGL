/**
 * @file keyboard_ui.h
 * @brief UI for Chunjiin keyboard with Korean font support
 */

#ifndef KEYBOARD_UI_H
#define KEYBOARD_UI_H

#include "lvgl.h"
#include "chunjiin_input.h"

/**
 * @brief Input modes
 */
typedef enum {
    INPUT_MODE_KOREAN,
    INPUT_MODE_ENGLISH,
    INPUT_MODE_NUMBER
} input_mode_t;

/**
 * @brief Initialize keyboard UI
 */
void keyboard_ui_init(void);

/**
 * @brief Get current input mode
 */
input_mode_t keyboard_get_mode(void);

/**
 * @brief Set input mode
 */
void keyboard_set_mode(input_mode_t mode);

/**
 * @brief Get text output
 */
const char* keyboard_get_text(void);

#endif /* KEYBOARD_UI_H */
