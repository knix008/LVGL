/**
 * @file gui.c
 * GUI components implementation with Korean language support using FreeType
 */

#include "gui.h"
#include "camera.h"
#include "lvgl/lvgl.h"
#include "lvgl/src/libs/freetype/lv_freetype.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Display configuration
#define WINDOW_WIDTH  340
#define WINDOW_HEIGHT 640

// Font paths
#define FONT_PATH_REGULAR "assets/NanumGothicCoding.ttf"
#define FONT_PATH_BOLD    "assets/NanumGothicCoding-Bold.ttf"

// GUI objects
static lv_obj_t *camera_img = NULL;
static lv_obj_t *capture_btn = NULL;
static lv_obj_t *status_label = NULL;
static lv_obj_t *photo_count_label = NULL;
static uint8_t *img_buffer = NULL;
static lv_image_dsc_t img_dsc;

// FreeType fonts
static lv_font_t *font_12 = NULL;
static lv_font_t *font_14 = NULL;
static lv_font_t *font_16 = NULL;
static lv_font_t *font_20 = NULL;
static lv_font_t *font_24 = NULL;

// Callback
static void (*capture_callback)(void *) = NULL;
static void *capture_user_data = NULL;

/**
 * Event handler for capture button
 */
static void event_handler_capture(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    printf("Button event received: code=%d\n", code);

    if (code == LV_EVENT_CLICKED) {
        printf("Button CLICKED event!\n");
        if (capture_callback) {
            capture_callback(capture_user_data);
        }
    } else if (code == LV_EVENT_PRESSED) {
        printf("Button PRESSED event!\n");
    } else if (code == LV_EVENT_RELEASED) {
        printf("Button RELEASED event!\n");
    }
}

/**
 * Initialize the GUI
 */
int gui_init(void)
{
    // Note: FreeType is automatically initialized by lv_init() in main.c
    // Do NOT call lv_freetype_init() again as it will fail

    // Create FreeType fonts using the new API
    font_12 = lv_freetype_font_create(FONT_PATH_REGULAR,
                                      LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                      12,
                                      LV_FREETYPE_FONT_STYLE_NORMAL);
    if (!font_12) {
        fprintf(stderr, "Failed to create font 12\n");
        return -1;
    }

    font_14 = lv_freetype_font_create(FONT_PATH_REGULAR,
                                      LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                      14,
                                      LV_FREETYPE_FONT_STYLE_NORMAL);
    if (!font_14) {
        fprintf(stderr, "Failed to create font 14\n");
        return -1;
    }

    font_16 = lv_freetype_font_create(FONT_PATH_REGULAR,
                                      LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                      16,
                                      LV_FREETYPE_FONT_STYLE_NORMAL);
    if (!font_16) {
        fprintf(stderr, "Failed to create font 16\n");
        return -1;
    }

    font_20 = lv_freetype_font_create(FONT_PATH_REGULAR,
                                      LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                      20,
                                      LV_FREETYPE_FONT_STYLE_NORMAL);
    if (!font_20) {
        fprintf(stderr, "Failed to create font 20\n");
        return -1;
    }

    font_24 = lv_freetype_font_create(FONT_PATH_BOLD,
                                      LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                      24,
                                      LV_FREETYPE_FONT_STYLE_BOLD);
    if (!font_24) {
        fprintf(stderr, "Failed to create font 24\n");
        return -1;
    }

    printf("FreeType fonts loaded successfully\n");

    lv_obj_t *screen = lv_screen_active();
    lv_obj_set_style_bg_color(screen, lv_color_hex(0x000000), 0);

    // Title label (Korean: 웹캠 캡처)
    lv_obj_t *title = lv_label_create(screen);
    lv_label_set_text(title, "웹캠 캡처");
    lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(title, font_20, 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);

    // Camera image
    camera_img = lv_image_create(screen);

    // Allocate buffer for camera preview
    img_buffer = malloc(CAMERA_WIDTH * CAMERA_HEIGHT * 3);
    if (!img_buffer) {
        fprintf(stderr, "Failed to allocate image buffer\n");
        return -1;
    }
    memset(img_buffer, 0x40, CAMERA_WIDTH * CAMERA_HEIGHT * 3);

    // Setup image descriptor
    img_dsc.header.cf = LV_COLOR_FORMAT_RGB888;
    img_dsc.header.w = CAMERA_WIDTH;
    img_dsc.header.h = CAMERA_HEIGHT;
    img_dsc.data = img_buffer;
    img_dsc.data_size = CAMERA_WIDTH * CAMERA_HEIGHT * 3;

    lv_image_set_src(camera_img, &img_dsc);
    lv_obj_align(camera_img, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_set_style_border_color(camera_img, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_border_width(camera_img, 2, 0);

    // Capture button (Korean: 촬영)
    capture_btn = lv_btn_create(screen);
    lv_obj_set_size(capture_btn, 200, 60);
    lv_obj_align(capture_btn, LV_ALIGN_TOP_MID, 0, 320);
    lv_obj_add_event_cb(capture_btn, event_handler_capture, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_bg_color(capture_btn, lv_color_hex(0x2196F3), 0);
    printf("Button created at position y=320, size 200x60\n");

    lv_obj_t *btn_label = lv_label_create(capture_btn);
    lv_label_set_text(btn_label, "촬영");
    lv_obj_set_style_text_font(btn_label, font_24, 0);
    lv_obj_center(btn_label);

    // Status label (Korean: 준비)
    status_label = lv_label_create(screen);
    lv_label_set_text(status_label, "준비");
    lv_obj_set_style_text_color(status_label, lv_color_hex(0x00FF00), 0);
    lv_obj_set_style_text_font(status_label, font_14, 0);
    lv_obj_align(status_label, LV_ALIGN_TOP_MID, 0, 400);
    lv_obj_set_width(status_label, WINDOW_WIDTH - 20);
    lv_obj_set_style_text_align(status_label, LV_TEXT_ALIGN_CENTER, 0);

    // Photo count label (Korean: 사진)
    photo_count_label = lv_label_create(screen);
    lv_label_set_text(photo_count_label, "사진: 0");
    lv_obj_set_style_text_color(photo_count_label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(photo_count_label, font_16, 0);
    lv_obj_align(photo_count_label, LV_ALIGN_TOP_MID, 0, 440);

    // Instructions (Korean)
    lv_obj_t *info = lv_label_create(screen);
    lv_label_set_text(info, "촬영 버튼을 클릭하여 사진 촬영\nJPEG 파일로 저장됩니다");
    lv_obj_set_style_text_color(info, lv_color_hex(0xAAAAAA), 0);
    lv_obj_set_style_text_font(info, font_12, 0);
    lv_obj_set_style_text_align(info, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(info, LV_ALIGN_BOTTOM_MID, 0, -20);

    printf("GUI initialized with Korean language support\n");
    return 0;
}

/**
 * Update camera preview with new frame
 */
void gui_update_camera_preview(uint8_t *frame_data)
{
    if (!camera_img || !img_buffer || !frame_data)
        return;

    // Copy frame data to image buffer
    memcpy(img_buffer, frame_data, CAMERA_WIDTH * CAMERA_HEIGHT * 3);

    // Re-set the image source to notify LVGL of the update
    lv_image_set_src(camera_img, &img_dsc);
}

/**
 * Update status message
 */
void gui_update_status(const char *message)
{
    if (status_label && message) {
        lv_label_set_text(status_label, message);
    }
}

/**
 * Update photo count
 */
void gui_update_photo_count(int count)
{
    if (photo_count_label) {
        char text[64];
        snprintf(text, sizeof(text), "사진: %d", count);
        lv_label_set_text(photo_count_label, text);
    }
}

/**
 * Set callback for capture button
 */
void gui_set_capture_callback(void (*callback)(void *), void *user_data)
{
    capture_callback = callback;
    capture_user_data = user_data;
}

/**
 * Get the camera image object
 */
lv_obj_t *gui_get_camera_canvas(void)
{
    return camera_img;
}

/**
 * Cleanup GUI resources
 */
void gui_cleanup(void)
{
    // Destroy FreeType fonts
    if (font_12) lv_freetype_font_delete(font_12);
    if (font_14) lv_freetype_font_delete(font_14);
    if (font_16) lv_freetype_font_delete(font_16);
    if (font_20) lv_freetype_font_delete(font_20);
    if (font_24) lv_freetype_font_delete(font_24);

    // Note: lv_freetype_uninit() is called automatically by lv_deinit()
    // in main.c, so we don't need to call it here

    if (img_buffer) {
        free(img_buffer);
        img_buffer = NULL;
    }

    printf("GUI cleanup complete\n");
}
