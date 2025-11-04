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
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

// Display configuration
#define WINDOW_WIDTH  340
#define WINDOW_HEIGHT 640

// Font paths
#define FONT_PATH_REGULAR "assets/NanumGothicCoding.ttf"
#define FONT_PATH_BOLD    "assets/NanumGothicCoding-Bold.ttf"

// Shutter sound path
#define SHUTTER_SOUND_PATH "assets/CameraShuffter.mp3"

// GUI objects
static lv_obj_t *camera_img = NULL;
static lv_obj_t *capture_btn = NULL;
static lv_obj_t *status_label = NULL;
static lv_obj_t *photo_count_label = NULL;
static lv_obj_t *flash_overlay = NULL;
static uint8_t *img_buffer = NULL;
static lv_image_dsc_t img_dsc;

// Face detection confidence labels (up to 10 faces)
#define MAX_CONF_LABELS 10
static lv_obj_t *conf_labels[MAX_CONF_LABELS] = {NULL};
static int conf_labels_initialized = 0;

// Audio variables
static Mix_Chunk *shutter_sound = NULL;

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
    img_dsc.header.stride = CAMERA_WIDTH * 3;  // Stride in bytes per row
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

    // Create flash overlay (initially hidden)
    flash_overlay = lv_obj_create(screen);
    lv_obj_set_size(flash_overlay, WINDOW_WIDTH, WINDOW_HEIGHT);
    lv_obj_set_style_bg_color(flash_overlay, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_bg_opa(flash_overlay, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(flash_overlay, 0, 0);
    lv_obj_add_flag(flash_overlay, LV_OBJ_FLAG_HIDDEN);  // Start hidden
    lv_obj_move_foreground(flash_overlay);  // Ensure it's on top

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

    // Force image refresh
    lv_obj_invalidate(camera_img);
}

/**
 * Initialize confidence labels for face detection
 */
static void init_confidence_labels(void)
{
    if (conf_labels_initialized) return;

    for (int i = 0; i < MAX_CONF_LABELS; i++) {
        conf_labels[i] = lv_label_create(lv_screen_active());
        lv_obj_set_style_text_font(conf_labels[i], font_20, 0);  // Use 20pt font
        lv_obj_set_style_text_color(conf_labels[i], lv_color_hex(0x00FF00), 0);  // Green color
        lv_obj_set_style_bg_opa(conf_labels[i], LV_OPA_TRANSP, 0);  // Transparent background
        lv_obj_set_style_pad_all(conf_labels[i], 0, 0);  // No padding
        lv_label_set_text(conf_labels[i], "");
        lv_obj_add_flag(conf_labels[i], LV_OBJ_FLAG_HIDDEN);  // Start hidden
        lv_obj_add_flag(conf_labels[i], LV_OBJ_FLAG_FLOATING);  // Don't affect layout
    }

    conf_labels_initialized = 1;
}

/**
 * Update camera preview with face detection overlay
 */
void gui_update_camera_preview_with_faces(uint8_t *frame_data, const FaceDetectionResult *faces)
{
    if (!camera_img || !img_buffer || !frame_data || !faces)
        return;

    // Copy frame data to image buffer
    memcpy(img_buffer, frame_data, CAMERA_WIDTH * CAMERA_HEIGHT * 3);

    // Draw face detection boxes on the frame
    for (int i = 0; i < faces->count; i++) {
        const FaceBox *box = &faces->boxes[i];

        // Convert normalized coordinates to pixel coordinates
        int x1 = (int)(box->x * CAMERA_WIDTH);
        int y1 = (int)(box->y * CAMERA_HEIGHT);
        int x2 = (int)((box->x + box->width) * CAMERA_WIDTH);
        int y2 = (int)((box->y + box->height) * CAMERA_HEIGHT);

        // Clamp to image bounds
        if (x1 < 0) x1 = 0;
        if (y1 < 0) y1 = 0;
        if (x2 >= CAMERA_WIDTH) x2 = CAMERA_WIDTH - 1;
        if (y2 >= CAMERA_HEIGHT) y2 = CAMERA_HEIGHT - 1;

        // Draw rectangle (green color: RGB 0, 255, 0)
        // Top horizontal line
        for (int x = x1; x <= x2; x++) {
            for (int t = 0; t < 2; t++) {  // 2 pixel thickness
                if (y1 + t < CAMERA_HEIGHT) {
                    int idx = ((y1 + t) * CAMERA_WIDTH + x) * 3;
                    if (idx >= 0 && idx + 2 < CAMERA_WIDTH * CAMERA_HEIGHT * 3) {
                        img_buffer[idx] = 0;      // R
                        img_buffer[idx + 1] = 255; // G
                        img_buffer[idx + 2] = 0;   // B
                    }
                }
            }
        }

        // Bottom horizontal line
        for (int x = x1; x <= x2; x++) {
            for (int t = 0; t < 2; t++) {
                if (y2 - t >= 0 && y2 - t < CAMERA_HEIGHT) {
                    int idx = ((y2 - t) * CAMERA_WIDTH + x) * 3;
                    if (idx >= 0 && idx + 2 < CAMERA_WIDTH * CAMERA_HEIGHT * 3) {
                        img_buffer[idx] = 0;
                        img_buffer[idx + 1] = 255;
                        img_buffer[idx + 2] = 0;
                    }
                }
            }
        }

        // Left vertical line
        for (int y = y1; y <= y2; y++) {
            for (int t = 0; t < 2; t++) {
                if (x1 + t < CAMERA_WIDTH) {
                    int idx = (y * CAMERA_WIDTH + (x1 + t)) * 3;
                    if (idx >= 0 && idx + 2 < CAMERA_WIDTH * CAMERA_HEIGHT * 3) {
                        img_buffer[idx] = 0;
                        img_buffer[idx + 1] = 255;
                        img_buffer[idx + 2] = 0;
                    }
                }
            }
        }

        // Right vertical line
        for (int y = y1; y <= y2; y++) {
            for (int t = 0; t < 2; t++) {
                if (x2 - t >= 0 && x2 - t < CAMERA_WIDTH) {
                    int idx = (y * CAMERA_WIDTH + (x2 - t)) * 3;
                    if (idx >= 0 && idx + 2 < CAMERA_WIDTH * CAMERA_HEIGHT * 3) {
                        img_buffer[idx] = 0;
                        img_buffer[idx + 1] = 255;
                        img_buffer[idx + 2] = 0;
                    }
                }
            }
        }

    }

    // Re-set the image source to notify LVGL of the update
    lv_image_set_src(camera_img, &img_dsc);

    // Force image refresh
    lv_obj_invalidate(camera_img);

    // Initialize confidence labels if not already done
    if (!conf_labels_initialized) {
        init_confidence_labels();
    }

    // Update confidence labels with LVGL text rendering
    // Get camera image position on screen
    lv_coord_t img_x = lv_obj_get_x(camera_img);
    lv_coord_t img_y = lv_obj_get_y(camera_img);

    // The camera preview is scaled up from 320x240 to fit the display area
    // Calculate the scaling factor
    lv_coord_t img_w = lv_obj_get_width(camera_img);
    lv_coord_t img_h = lv_obj_get_height(camera_img);
    float scale_x = (float)img_w / CAMERA_WIDTH;
    float scale_y = (float)img_h / CAMERA_HEIGHT;

    for (int i = 0; i < MAX_CONF_LABELS; i++) {
        if (i < faces->count) {
            const FaceBox *box = &faces->boxes[i];

            // Format confidence text
            char conf_text[16];
            snprintf(conf_text, sizeof(conf_text), "%.0f%%", box->confidence * 100.0f);
            lv_label_set_text(conf_labels[i], conf_text);

            // Calculate position in screen coordinates
            int x1 = (int)(box->x * CAMERA_WIDTH * scale_x);
            int y1 = (int)(box->y * CAMERA_HEIGHT * scale_y);

            // Position label above the bounding box
            lv_coord_t label_x = img_x + x1 + 2;
            lv_coord_t label_y = img_y + y1 - 25; // Above box

            // If too close to top, position below top edge
            if (label_y < img_y) {
                label_y = img_y + y1 + 3;
            }

            lv_obj_set_pos(conf_labels[i], label_x, label_y);
            lv_obj_clear_flag(conf_labels[i], LV_OBJ_FLAG_HIDDEN);
        } else {
            // Hide unused labels
            lv_obj_add_flag(conf_labels[i], LV_OBJ_FLAG_HIDDEN);
        }
    }
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
 * Timer callback to hide flash overlay
 */
static void flash_timer_cb(lv_timer_t *timer)
{
    lv_obj_add_flag(flash_overlay, LV_OBJ_FLAG_HIDDEN);
    lv_timer_del(timer);  // Delete the timer after use
}

/**
 * Show white flash overlay for brighter photos
 */
void gui_show_flash(uint32_t duration_ms)
{
    if (!flash_overlay) return;

    // Show the white overlay
    lv_obj_clear_flag(flash_overlay, LV_OBJ_FLAG_HIDDEN);
    lv_refr_now(NULL);  // Force immediate refresh

    // Create a one-shot timer to hide it after duration
    lv_timer_create(flash_timer_cb, duration_ms, NULL);
}

/**
 * Load and play shutter sound from MP3 file
 */
void gui_play_shutter_sound(void)
{
    // Initialize SDL_mixer if not already done
    if (shutter_sound == NULL) {
        // Initialize SDL audio subsystem
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
            fprintf(stderr, "Failed to initialize SDL audio: %s\n", SDL_GetError());
            return;
        }

        // Initialize SDL_mixer with MP3 support
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            fprintf(stderr, "Failed to initialize SDL_mixer: %s\n", Mix_GetError());
            return;
        }

        // Load the shutter sound MP3 file
        shutter_sound = Mix_LoadWAV(SHUTTER_SOUND_PATH);
        if (shutter_sound == NULL) {
            fprintf(stderr, "Failed to load shutter sound '%s': %s\n",
                    SHUTTER_SOUND_PATH, Mix_GetError());
            return;
        }

        printf("Shutter sound loaded successfully from %s\n", SHUTTER_SOUND_PATH);
    }

    // Play the shutter sound on first available channel
    if (Mix_PlayChannel(-1, shutter_sound, 0) == -1) {
        fprintf(stderr, "Failed to play shutter sound: %s\n", Mix_GetError());
    }
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

    // Free shutter sound and close SDL_mixer
    if (shutter_sound != NULL) {
        Mix_FreeChunk(shutter_sound);
        shutter_sound = NULL;
    }
    Mix_CloseAudio();

    if (img_buffer) {
        free(img_buffer);
        img_buffer = NULL;
    }

    printf("GUI cleanup complete\n");
}
