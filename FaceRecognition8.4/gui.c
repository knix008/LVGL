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

// Forward declarations
static void event_handler_start_registration(lv_event_t *e);

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

    // Registration button (Korean: 얼굴 등록)
    capture_btn = lv_btn_create(screen);  // Reuse capture_btn for registration button
    lv_obj_set_size(capture_btn, 200, 60);
    lv_obj_align(capture_btn, LV_ALIGN_TOP_MID, 0, 320);
    lv_obj_add_event_cb(capture_btn, event_handler_start_registration, LV_EVENT_CLICKED, NULL);
    lv_obj_set_style_bg_color(capture_btn, lv_color_hex(0xFF9800), 0);
    printf("Registration button created\n");

    lv_obj_t *btn_label = lv_label_create(capture_btn);
    lv_label_set_text(btn_label, "얼굴 등록");
    lv_obj_set_style_text_font(btn_label, font_20, 0);
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

    // Only update LVGL objects if initialized
    if (lv_is_initialized()) {
        // Re-set the image source to notify LVGL of the update
        lv_image_set_src(camera_img, &img_dsc);

        // Force image refresh
        lv_obj_invalidate(camera_img);
    }
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
 * Draw a rectangle on the RGB image buffer
 */
static void draw_rectangle(uint8_t *buffer, int img_width, int img_height,
                          int x, int y, int width, int height,
                          uint8_t r, uint8_t g, uint8_t b, int thickness)
{
    // Clamp coordinates to image bounds
    if (x < 0) { width += x; x = 0; }
    if (y < 0) { height += y; y = 0; }
    if (x + width > img_width) width = img_width - x;
    if (y + height > img_height) height = img_height - y;

    if (width <= 0 || height <= 0) return;

    // Corner length (how long each corner line extends)
    int corner_length = (width < height ? width : height) / 4;  // 25% of smaller dimension
    if (corner_length < 10) corner_length = 10;  // Minimum length
    if (corner_length > 50) corner_length = 50;  // Maximum length

    // Draw top-left corner (horizontal and vertical lines)
    // Horizontal line (top-left)
    for (int t = 0; t < thickness; t++) {
        for (int i = 0; i < corner_length && (x + i) < img_width; i++) {
            int idx = ((y + t) * img_width + (x + i)) * 3;
            if (idx >= 0 && idx < img_width * img_height * 3 - 2) {
                buffer[idx] = r;
                buffer[idx + 1] = g;
                buffer[idx + 2] = b;
            }
        }
    }
    // Vertical line (top-left)
    for (int t = 0; t < thickness; t++) {
        for (int i = 0; i < corner_length && (y + i) < img_height; i++) {
            int idx = ((y + i) * img_width + (x + t)) * 3;
            if (idx >= 0 && idx < img_width * img_height * 3 - 2) {
                buffer[idx] = r;
                buffer[idx + 1] = g;
                buffer[idx + 2] = b;
            }
        }
    }

    // Draw top-right corner (horizontal and vertical lines)
    // Horizontal line (top-right)
    for (int t = 0; t < thickness; t++) {
        for (int i = 0; i < corner_length && (x + width - corner_length + i) < img_width; i++) {
            int idx = ((y + t) * img_width + (x + width - corner_length + i)) * 3;
            if (idx >= 0 && idx < img_width * img_height * 3 - 2) {
                buffer[idx] = r;
                buffer[idx + 1] = g;
                buffer[idx + 2] = b;
            }
        }
    }
    // Vertical line (top-right)
    for (int t = 0; t < thickness; t++) {
        for (int i = 0; i < corner_length && (y + i) < img_height; i++) {
            int idx = ((y + i) * img_width + (x + width - 1 - t)) * 3;
            if (idx >= 0 && idx < img_width * img_height * 3 - 2) {
                buffer[idx] = r;
                buffer[idx + 1] = g;
                buffer[idx + 2] = b;
            }
        }
    }

    // Draw bottom-left corner (horizontal and vertical lines)
    // Horizontal line (bottom-left)
    for (int t = 0; t < thickness; t++) {
        for (int i = 0; i < corner_length && (x + i) < img_width; i++) {
            int idx = ((y + height - 1 - t) * img_width + (x + i)) * 3;
            if (idx >= 0 && idx < img_width * img_height * 3 - 2) {
                buffer[idx] = r;
                buffer[idx + 1] = g;
                buffer[idx + 2] = b;
            }
        }
    }
    // Vertical line (bottom-left)
    for (int t = 0; t < thickness; t++) {
        for (int i = 0; i < corner_length && (y + height - corner_length + i) < img_height; i++) {
            int idx = ((y + height - corner_length + i) * img_width + (x + t)) * 3;
            if (idx >= 0 && idx < img_width * img_height * 3 - 2) {
                buffer[idx] = r;
                buffer[idx + 1] = g;
                buffer[idx + 2] = b;
            }
        }
    }

    // Draw bottom-right corner (horizontal and vertical lines)
    // Horizontal line (bottom-right)
    for (int t = 0; t < thickness; t++) {
        for (int i = 0; i < corner_length && (x + width - corner_length + i) < img_width; i++) {
            int idx = ((y + height - 1 - t) * img_width + (x + width - corner_length + i)) * 3;
            if (idx >= 0 && idx < img_width * img_height * 3 - 2) {
                buffer[idx] = r;
                buffer[idx + 1] = g;
                buffer[idx + 2] = b;
            }
        }
    }
    // Vertical line (bottom-right)
    for (int t = 0; t < thickness; t++) {
        for (int i = 0; i < corner_length && (y + height - corner_length + i) < img_height; i++) {
            int idx = ((y + height - corner_length + i) * img_width + (x + width - 1 - t)) * 3;
            if (idx >= 0 && idx < img_width * img_height * 3 - 2) {
                buffer[idx] = r;
                buffer[idx + 1] = g;
                buffer[idx + 2] = b;
            }
        }
    }
}

/**
 * Update camera preview with face detection overlay
 */
void gui_update_camera_preview_with_faces(uint8_t *frame_data, const FaceDetectionResult *faces)
{
    if (!camera_img || !img_buffer || !frame_data || !faces)
        return;

    // Copy frame data to image buffer (fastest operation)
    memcpy(img_buffer, frame_data, CAMERA_WIDTH * CAMERA_HEIGHT * 3);

    // Draw rectangles on the image buffer for each detected face
    for (int i = 0; i < faces->count; i++) {
        const FaceBox *box = &faces->boxes[i];

        // Convert normalized coordinates to pixel coordinates
        int x = (int)(box->x * CAMERA_WIDTH);
        int y = (int)(box->y * CAMERA_HEIGHT);
        int width = (int)(box->width * CAMERA_WIDTH);
        int height = (int)(box->height * CAMERA_HEIGHT);

        // Scale up the detection area by 20% (0.2 = 20% expansion)
        float scale_factor = 0.2f;
        int x_offset = (int)(width * scale_factor / 2);
        int y_offset = (int)(height * scale_factor / 2);

        x -= x_offset;
        y -= y_offset;
        width += (int)(width * scale_factor);
        height += (int)(height * scale_factor);

        // Draw green rectangle with thickness 2
        draw_rectangle(img_buffer, CAMERA_WIDTH, CAMERA_HEIGHT,
                      x, y, width, height,
                      0, 255, 0, 2);  // Green color (R=0, G=255, B=0)
    }

    // Only update LVGL objects if initialized
    if (lv_is_initialized()) {
        // Re-set the image source to notify LVGL of the update
        lv_image_set_src(camera_img, &img_dsc);

        // Force image refresh
        lv_obj_invalidate(camera_img);
    }

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
    // Only cleanup FreeType fonts if LVGL is still initialized
    if (lv_is_initialized()) {
        // Destroy FreeType fonts
        if (font_12) lv_freetype_font_delete(font_12);
        if (font_14) lv_freetype_font_delete(font_14);
        if (font_16) lv_freetype_font_delete(font_16);
        if (font_20) lv_freetype_font_delete(font_20);
        if (font_24) lv_freetype_font_delete(font_24);

        font_12 = font_14 = font_16 = font_20 = font_24 = NULL;
    }

    // Note: lv_freetype_uninit() is called automatically by lv_deinit()
    // in main.c, so we don't need to call it here

    // Free shutter sound and close SDL_mixer
    if (shutter_sound != NULL) {
        Mix_FreeChunk(shutter_sound);
        shutter_sound = NULL;
    }
    Mix_CloseAudio();

    // Free image buffer (safe to do even if LVGL is deinitialized)
    if (img_buffer) {
        free(img_buffer);
        img_buffer = NULL;
    }

    printf("GUI cleanup complete\n");
}

// Global variables for registration UI
static lv_obj_t *registration_screen = NULL;
static lv_obj_t *register_btn = NULL;
static lv_obj_t *recognize_btn = NULL;
static lv_obj_t *registration_name_input = NULL;
static lv_obj_t *registration_status_label = NULL;
static lv_obj_t *recognition_popup = NULL;
static lv_obj_t *registration_keyboard = NULL;

static void (*register_callback)(void *) = NULL;
static void *register_user_data = NULL;
static void (*recognize_callback)(void *) = NULL;
static void *recognize_user_data = NULL;
static void (*registration_shown_callback)(void *) = NULL;
static void *registration_shown_user_data = NULL;

/**
 * Event handler for starting registration (from main screen)
 */
static void event_handler_start_registration(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("Start Registration button clicked\n");
        gui_show_registration_screen("");  // Show with empty name

        // Prepare the name input field for immediate text entry
        if (registration_name_input) {
            lv_obj_clear_flag(registration_name_input, LV_OBJ_FLAG_HIDDEN);
            lv_textarea_set_text(registration_name_input, "");  // Clear any previous text
            printf("[DEBUG] Cleared and prepared registration name input\n");
        }

        // Call the registration shown callback
        if (registration_shown_callback) {
            registration_shown_callback(registration_shown_user_data);
        }
    }
}

/**
 * Event handler for register button
 */
static void event_handler_register(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("Register button clicked\n");
        if (register_callback) {
            register_callback(register_user_data);
        }
    }
}

/**
 * Event handler for recognize button
 */
static void event_handler_recognize(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("Recognize button clicked\n");
        if (recognize_callback) {
            recognize_callback(recognize_user_data);
        }
    }
}

/**
 * Show recognition result as popup
 */
void gui_show_recognition_popup(const char *person_name, float confidence)
{
    if (!lv_is_initialized()) return;

    // Remove old overlay if exists
    if (recognition_popup != NULL) {
        lv_obj_delete(recognition_popup);
    }

    // Check if this is an unregistered person
    bool is_unregistered = (strcmp(person_name, "미등록") == 0);

    // Create semi-transparent overlay container on top-left corner
    recognition_popup = lv_obj_create(lv_screen_active());
    lv_obj_set_size(recognition_popup, 280, 100);
    lv_obj_align(recognition_popup, LV_ALIGN_TOP_LEFT, 10, 10);

    // Dark semi-transparent background
    lv_obj_set_style_bg_color(recognition_popup, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_opa(recognition_popup, LV_OPA_80, 0);  // 80% opaque

    // Border color depends on registration status
    uint32_t border_color = is_unregistered ? 0xFF5252 : 0x4CAF50;  // Red for unregistered, green for registered
    lv_obj_set_style_border_color(recognition_popup, lv_color_hex(border_color), 0);
    lv_obj_set_style_border_width(recognition_popup, 2, 0);
    lv_obj_set_style_pad_all(recognition_popup, 10, 0);

    // Person name (larger and prominent)
    lv_obj_t *name_label = lv_label_create(recognition_popup);
    lv_label_set_text(name_label, person_name);
    lv_obj_set_style_text_font(name_label, font_20, 0);
    uint32_t text_color = is_unregistered ? 0xFF5252 : 0x4CAF50;  // Red for unregistered, green for registered
    lv_obj_set_style_text_color(name_label, lv_color_hex(text_color), 0);
    lv_obj_align(name_label, LV_ALIGN_TOP_LEFT, 0, 0);

    // Confidence level (only show for registered persons)
    if (!is_unregistered) {
        char confidence_str[64];
        snprintf(confidence_str, sizeof(confidence_str), "신뢰도: %.0f%%", confidence * 100);
        lv_obj_t *conf_label = lv_label_create(recognition_popup);
        lv_label_set_text(conf_label, confidence_str);
        lv_obj_set_style_text_font(conf_label, font_16, 0);
        lv_obj_set_style_text_color(conf_label, lv_color_hex(0xFFFFFF), 0);  // White
        lv_obj_align(conf_label, LV_ALIGN_BOTTOM_LEFT, 0, 0);
    }

    // Make overlay floating and always on top
    lv_obj_add_flag(recognition_popup, LV_OBJ_FLAG_FLOATING);
    lv_obj_move_foreground(recognition_popup);

    if (is_unregistered) {
        printf("[DEBUG] Unregistered person detected - overlay shown with red border\n");
    } else {
        printf("[DEBUG] Recognition overlay shown: name=%s, confidence=%.2f%%\n", person_name, confidence * 100);
    }
}

/**
 * Show registration screen
 */
void gui_show_registration_screen(const char *person_name)
{
    if (!lv_is_initialized()) return;

    // If already visible, just update the name
    if (registration_screen != NULL && !lv_obj_has_flag(registration_screen, LV_OBJ_FLAG_HIDDEN)) {
        if (registration_name_input) {
            lv_textarea_set_text(registration_name_input, person_name);
        }
        return;
    }

    // Create registration screen
    if (registration_screen == NULL) {
        registration_screen = lv_obj_create(lv_screen_active());
        lv_obj_set_size(registration_screen, WINDOW_WIDTH, WINDOW_HEIGHT);
        lv_obj_set_pos(registration_screen, 0, 0);
        lv_obj_set_style_bg_color(registration_screen, lv_color_hex(0x0D0D0D), 0);
        lv_obj_set_style_border_width(registration_screen, 0, 0);
        lv_obj_set_style_pad_all(registration_screen, 10, 0);

        // Title
        lv_obj_t *title = lv_label_create(registration_screen);
        lv_label_set_text(title, "얼굴 등록");  // Korean: "Face Registration"
        lv_obj_set_style_text_font(title, font_24, 0);
        lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), 0);
        lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

        // Instruction label
        lv_obj_t *instruction = lv_label_create(registration_screen);
        lv_label_set_text(instruction, "아래에 이름을 입력하세요");  // Korean: "Enter name below"
        lv_obj_set_style_text_font(instruction, font_14, 0);
        lv_obj_set_style_text_color(instruction, lv_color_hex(0xAAAAAA), 0);
        lv_obj_align(instruction, LV_ALIGN_TOP_MID, 0, 55);

        // Name input
        registration_name_input = lv_textarea_create(registration_screen);

        // Configure textarea for text input
        lv_textarea_set_text(registration_name_input, person_name);
        lv_textarea_set_placeholder_text(registration_name_input, "이름을 입력하세요");  // Korean: "Enter name"
        lv_textarea_set_one_line(registration_name_input, true);  // Single line input

        // Set maximum length to prevent overflow
        lv_textarea_set_max_length(registration_name_input, 127);

        // Allow all printable characters and Korean characters
        // Leave accepted_chars unset to allow all characters by default

        // Visual styling
        lv_obj_set_size(registration_name_input, WINDOW_WIDTH - 40, 50);
        lv_obj_align(registration_name_input, LV_ALIGN_TOP_MID, 0, 100);
        lv_obj_set_style_text_font(registration_name_input, font_16, 0);
        lv_obj_set_style_bg_color(registration_name_input, lv_color_hex(0x2A2A2A), 0);
        lv_obj_set_style_text_color(registration_name_input, lv_color_hex(0xFFFFFF), 0);
        lv_obj_set_style_border_color(registration_name_input, lv_color_hex(0x4CAF50), 0);
        lv_obj_set_style_border_width(registration_name_input, 2, 0);
        lv_obj_set_style_pad_all(registration_name_input, 5, 0);

        // Cursor styling to make it visible
        lv_obj_set_style_border_color(registration_name_input, lv_color_hex(0xFFFFFF), LV_PART_CURSOR);

        // Allow cursor and editing
        lv_textarea_set_cursor_click_pos(registration_name_input, true);

        // Make textarea editable by allowing key events
        lv_obj_clear_flag(registration_name_input, LV_OBJ_FLAG_HIDDEN);

        printf("[DEBUG] Name input textarea created and configured\n");

        // On-screen keyboard for text input
        registration_keyboard = lv_keyboard_create(registration_screen);
        lv_keyboard_set_textarea(registration_keyboard, registration_name_input);
        lv_obj_set_size(registration_keyboard, WINDOW_WIDTH - 20, 150);
        lv_obj_align(registration_keyboard, LV_ALIGN_TOP_MID, 0, 160);
        lv_keyboard_set_mode(registration_keyboard, LV_KEYBOARD_MODE_TEXT_LOWER);
        printf("[DEBUG] Keyboard widget created and configured\n");

        // Status label
        registration_status_label = lv_label_create(registration_screen);
        lv_label_set_text(registration_status_label, "사진 촬영 준비: 3장 필요");  // Korean: "Ready to capture 3 photos"
        lv_obj_set_style_text_font(registration_status_label, font_14, 0);
        lv_obj_set_style_text_color(registration_status_label, lv_color_hex(0x4CAF50), 0);
        lv_obj_align(registration_status_label, LV_ALIGN_TOP_MID, 0, 320);
        lv_obj_set_width(registration_status_label, WINDOW_WIDTH - 40);
        lv_obj_set_style_text_align(registration_status_label, LV_TEXT_ALIGN_CENTER, 0);

        // Register button
        register_btn = lv_btn_create(registration_screen);
        lv_obj_set_size(register_btn, 140, 50);
        lv_obj_align(register_btn, LV_ALIGN_BOTTOM_MID, -80, -20);
        lv_obj_set_style_bg_color(register_btn, lv_color_hex(0x4CAF50), 0);
        lv_obj_add_event_cb(register_btn, event_handler_register, LV_EVENT_CLICKED, NULL);

        lv_obj_t *reg_label = lv_label_create(register_btn);
        lv_label_set_text(reg_label, "촬영");  // Korean: "Capture"
        lv_obj_set_style_text_font(reg_label, font_16, 0);
        lv_obj_center(reg_label);

        // Recognize button
        recognize_btn = lv_btn_create(registration_screen);
        lv_obj_set_size(recognize_btn, 140, 50);
        lv_obj_align(recognize_btn, LV_ALIGN_BOTTOM_MID, 80, -20);
        lv_obj_set_style_bg_color(recognize_btn, lv_color_hex(0x2196F3), 0);
        lv_obj_add_event_cb(recognize_btn, event_handler_recognize, LV_EVENT_CLICKED, NULL);

        lv_obj_t *rec_label = lv_label_create(recognize_btn);
        lv_label_set_text(rec_label, "확인");  // Korean: "Confirm"
        lv_obj_set_style_text_font(rec_label, font_16, 0);
        lv_obj_center(rec_label);
    }

    // Update name input
    if (registration_name_input) {
        lv_textarea_set_text(registration_name_input, person_name);
    }

    // Show the screen
    lv_obj_clear_flag(registration_screen, LV_OBJ_FLAG_HIDDEN);
    lv_obj_move_foreground(registration_screen);
}

/**
 * Hide registration screen
 */
void gui_hide_registration_screen(void)
{
    if (registration_screen != NULL) {
        lv_obj_add_flag(registration_screen, LV_OBJ_FLAG_HIDDEN);
    }
}

/**
 * Check if registration screen is visible
 */
bool gui_is_registration_screen_visible(void)
{
    if (registration_screen == NULL) return false;
    return !lv_obj_has_flag(registration_screen, LV_OBJ_FLAG_HIDDEN);
}

/**
 * Set callback for register button
 */
void gui_set_register_callback(void (*callback)(void *), void *user_data)
{
    register_callback = callback;
    register_user_data = user_data;
}

/**
 * Set callback for recognize button
 */
void gui_set_recognize_callback(void (*callback)(void *), void *user_data)
{
    recognize_callback = callback;
    recognize_user_data = user_data;
}

/**
 * Get person name from registration screen
 */
const char *gui_get_registration_name(void)
{
    if (registration_name_input == NULL) {
        return "";
    }
    return lv_textarea_get_text(registration_name_input);
}

/**
 * Set callback for when registration screen is shown
 */
void gui_set_registration_shown_callback(void (*callback)(void *), void *user_data)
{
    registration_shown_callback = callback;
    registration_shown_user_data = user_data;
}
