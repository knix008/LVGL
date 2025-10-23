/**
 * @file keypad.h
 * @brief Mobile-style keypad interface (3 columns x 5 rows)
 *
 * This module provides a mobile-style keypad with multiple input modes:
 * - Lowercase letters
 * - Uppercase letters
 * - Numbers
 * - Special characters
 */

#ifndef KEYPAD_H
#define KEYPAD_H

#include "lvgl/lvgl.h"
#include <stdbool.h>

/**
 * @brief Keypad input modes
 */
typedef enum {
    KEYPAD_MODE_LOWERCASE,      /**< Lowercase letters */
    KEYPAD_MODE_UPPERCASE,      /**< Uppercase letters */
    KEYPAD_MODE_NUMBERS,        /**< Numbers and basic symbols */
    KEYPAD_MODE_SPECIAL,        /**< Special characters */
    KEYPAD_MODE_COUNT           /**< Total number of modes */
} keypad_mode_t;

/**
 * @brief Keypad configuration
 */
typedef struct {
    int width;                  /**< Keypad width in pixels */
    int height;                 /**< Keypad height in pixels */
    lv_obj_t *parent;          /**< Parent object to attach keypad to */
    lv_obj_t *target_textarea; /**< Target textarea to send input to */
    lv_font_t *button_font;    /**< Font for button labels (optional) */
} keypad_config_t;

/**
 * @brief Creates a mobile-style keypad
 *
 * Creates a 3-column x 5-row keypad with mode switching buttons.
 *
 * @param config Keypad configuration
 * @return Pointer to created keypad object, NULL on failure
 */
lv_obj_t *keypad_create(const keypad_config_t *config);

/**
 * @brief Sets the target textarea for keypad input
 *
 * @param keypad Keypad object
 * @param textarea Target textarea object
 */
void keypad_set_target(lv_obj_t *keypad, lv_obj_t *textarea);

/**
 * @brief Sets the current keypad mode
 *
 * @param keypad Keypad object
 * @param mode New mode to set
 */
void keypad_set_mode(lv_obj_t *keypad, keypad_mode_t mode);

/**
 * @brief Gets the current keypad mode
 *
 * @param keypad Keypad object
 * @return Current mode
 */
keypad_mode_t keypad_get_mode(lv_obj_t *keypad);

/**
 * @brief Deletes the keypad and frees resources
 *
 * @param keypad Keypad object to delete
 */
void keypad_delete(lv_obj_t *keypad);

#endif /* KEYPAD_H */
