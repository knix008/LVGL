/**
 * @file image_button_app.h
 * @brief Image Button GUI Application Header
 * 
 * This file contains the declarations for the image button demonstration application.
 * It showcases LVGL image buttons with different image formats (PNG, JPG, GIF, BMP).
 */

#ifndef IMAGE_BUTTON_APP_H
#define IMAGE_BUTTON_APP_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the image button application
 * 
 * Creates and displays a GUI with image buttons demonstrating different image formats.
 * The application shows buttons with PNG, JPG, GIF, and BMP images.
 */
void image_button_app_init(void);

/**
 * @brief Handle button click events
 * 
 * @param e Event object containing information about the button click
 */
void button_click_handler(lv_event_t * e);

/**
 * @brief Create a button with an image and text
 *
 * @param parent Parent object for the button
 * @param image_path Path to the image file
 * @param text Text to display on the button
 * @param x X position of the button
 * @param y Y position of the button
 * @param width Width of the button
 * @param height Height of the button
 * @return Pointer to the created button object
 */
lv_obj_t * create_image_button(lv_obj_t * parent, const char * image_path,
                               const char * text,
                               lv_coord_t x, lv_coord_t y,
                               lv_coord_t width, lv_coord_t height);

/**
 * @brief Create a label to display button information
 * 
 * @param parent Parent object for the label
 * @param text Text to display
 * @param x X position of the label
 * @param y Y position of the label
 * @return Pointer to the created label object
 */
lv_obj_t * create_info_label(lv_obj_t * parent, const char * text, 
                            lv_coord_t x, lv_coord_t y);

#ifdef __cplusplus
}
#endif

#endif /* IMAGE_BUTTON_APP_H */
