/**
 * @file main.c
 * @brief Round Image Button Example Application using LVGL
 *
 * This application demonstrates round buttons with image icons
 * Supports multiple image formats: PNG, JPG, BMP, GIF
 * Window size: 320x640
 */

#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include "lvgl/src/core/lv_obj_event_private.h"
#include <SDL2/SDL.h>

#define WINDOW_WIDTH  320
#define WINDOW_HEIGHT 640

static lv_display_t *display;
static lv_indev_t *indev;

/* Forward declarations */
static void create_round_buttons(void);
static void button_event_handler(lv_event_t *e);
static void init_button_styles(void);
static void button_visual_effect_handler(lv_event_t *e);

/* Korean font */
static lv_font_t *korean_font = NULL;

/* Icon font for buttons (kept for fallback) */
static lv_font_t *icon_font = NULL;

/* Image format to use: "png", "jpg", "bmp", or "gif"
 * Change this to test different image formats:
 * - PNG: Best quality with transparency support (recommended)
 * - JPG: Good for photos, smaller file size, no transparency
 * - BMP: Uncompressed, larger file size
 * - GIF: Supports transparency and animation
 */
#define IMAGE_FORMAT "png"

/* Reusable button styles */
static lv_style_t style_circle_btn_blue;
static lv_style_t style_circle_btn_green;
static lv_style_t style_circle_btn_orange;
static lv_style_t style_circle_btn_red;

/* Pressed state styles for buttons */
static lv_style_t style_btn_pressed;

/**
 * Initialize SDL and LVGL display
 */
static void hal_init(void)
{
    /* Initialize SDL */
    SDL_Init(SDL_INIT_VIDEO);

    /* Create LVGL display with SDL */
    display = lv_sdl_window_create(WINDOW_WIDTH, WINDOW_HEIGHT);

    /* Create input device (mouse/touch) */
    indev = lv_sdl_mouse_create();

    /* Load Korean font using FreeType */
    korean_font = lv_freetype_font_create("assets/NanumGothicCoding.ttf",
                                          LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                          20,
                                          LV_FREETYPE_FONT_STYLE_NORMAL);
    if (!korean_font) {
        printf("Warning: Failed to load Korean font, using default font\n");
    }

    /* Load icon font using FreeType (larger size for button icons) */
    icon_font = lv_freetype_font_create("assets/NanumGothicCoding-Bold.ttf",
                                        LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                        48,
                                        LV_FREETYPE_FONT_STYLE_NORMAL);
    if (!icon_font) {
        printf("Warning: Failed to load icon font, using default symbols\n");
    }
}

/**
 * Circular hit test event handler
 * Uses LV_EVENT_HIT_TEST to define circular clickable area
 */
static void circle_hit_test_event_cb(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_HIT_TEST) {
        lv_obj_t *obj = lv_event_get_target(e);
        lv_hit_test_info_t *info = lv_event_get_param(e);

        if (!info) return;

        /* Get button coordinates */
        lv_area_t coords;
        lv_obj_get_coords(obj, &coords);

        /* Calculate center and radius */
        int32_t center_x = (coords.x1 + coords.x2) / 2;
        int32_t center_y = (coords.y1 + coords.y2) / 2;
        int32_t radius = (coords.x2 - coords.x1) / 2;

        /* Calculate distance from center to the test point */
        int32_t dx = info->point->x - center_x;
        int32_t dy = info->point->y - center_y;
        int32_t dist_sq = dx * dx + dy * dy;
        int32_t radius_sq = radius * radius;

        /* Set hit test result: true if inside circle, false if outside */
        info->res = (dist_sq <= radius_sq);
    }
}

/**
 * Visual effect handler for button press/release
 * Scales and changes opacity of the image for better feedback
 */
static void button_visual_effect_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    lv_obj_t *img = lv_obj_get_child(btn, 0);

    if (code == LV_EVENT_PRESSED) {
        /* Scale down the image to 85% and reduce opacity */
        lv_obj_set_style_transform_scale(img, 217, 0);  /* 217 = 85% of 256 */
        lv_obj_set_style_image_opa(img, LV_OPA_70, 0);  /* 70% opacity */
        /* Re-center the image after scaling */
        lv_obj_center(img);
    }
    else if (code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) {
        /* Return to normal size and full opacity */
        lv_obj_set_style_transform_scale(img, 256, 0);  /* 256 = 100% */
        lv_obj_set_style_image_opa(img, LV_OPA_COVER, 0);  /* 100% opacity */
        /* Re-center the image after scaling */
        lv_obj_center(img);
    }
}

/**
 * Event handler for button clicks
 */
static void button_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED) {
        lv_obj_t *img = lv_obj_get_child(btn, 0);
        const void *img_src = lv_image_get_src(img);
        printf("Button clicked: %s\n", (const char*)img_src);
    }
}

/**
 * Initialize reusable button styles
 */
static void init_button_styles(void)
{
    /* Blue circle button style */
    lv_style_init(&style_circle_btn_blue);
    lv_style_set_radius(&style_circle_btn_blue, LV_RADIUS_CIRCLE);
    lv_style_set_bg_color(&style_circle_btn_blue, lv_color_hex(0x2196F3));
    lv_style_set_bg_grad_color(&style_circle_btn_blue, lv_color_hex(0x1976D2));
    lv_style_set_bg_grad_dir(&style_circle_btn_blue, LV_GRAD_DIR_VER);
    lv_style_set_border_width(&style_circle_btn_blue, 0);
    lv_style_set_shadow_width(&style_circle_btn_blue, 15);
    lv_style_set_shadow_color(&style_circle_btn_blue, lv_color_hex(0x1976D2));
    lv_style_set_shadow_offset_y(&style_circle_btn_blue, 5);
    lv_style_set_text_color(&style_circle_btn_blue, lv_color_white());
    /* Disable scale on press */
    lv_style_set_transform_scale(&style_circle_btn_blue, 256);

    /* Green circle button style */
    lv_style_init(&style_circle_btn_green);
    lv_style_set_radius(&style_circle_btn_green, LV_RADIUS_CIRCLE);
    lv_style_set_bg_color(&style_circle_btn_green, lv_color_hex(0x4CAF50));
    lv_style_set_bg_grad_color(&style_circle_btn_green, lv_color_hex(0x388E3C));
    lv_style_set_bg_grad_dir(&style_circle_btn_green, LV_GRAD_DIR_VER);
    lv_style_set_border_width(&style_circle_btn_green, 0);
    lv_style_set_shadow_width(&style_circle_btn_green, 15);
    lv_style_set_shadow_color(&style_circle_btn_green, lv_color_hex(0x388E3C));
    lv_style_set_shadow_offset_y(&style_circle_btn_green, 5);
    lv_style_set_text_color(&style_circle_btn_green, lv_color_white());
    lv_style_set_transform_scale(&style_circle_btn_green, 256);

    /* Orange circle button style */
    lv_style_init(&style_circle_btn_orange);
    lv_style_set_radius(&style_circle_btn_orange, LV_RADIUS_CIRCLE);
    lv_style_set_bg_color(&style_circle_btn_orange, lv_color_hex(0xFF9800));
    lv_style_set_bg_grad_color(&style_circle_btn_orange, lv_color_hex(0xF57C00));
    lv_style_set_bg_grad_dir(&style_circle_btn_orange, LV_GRAD_DIR_VER);
    lv_style_set_border_width(&style_circle_btn_orange, 0);
    lv_style_set_shadow_width(&style_circle_btn_orange, 15);
    lv_style_set_shadow_color(&style_circle_btn_orange, lv_color_hex(0xF57C00));
    lv_style_set_shadow_offset_y(&style_circle_btn_orange, 5);
    lv_style_set_text_color(&style_circle_btn_orange, lv_color_white());
    lv_style_set_transform_scale(&style_circle_btn_orange, 256);

    /* Red circle button style */
    lv_style_init(&style_circle_btn_red);
    lv_style_set_radius(&style_circle_btn_red, LV_RADIUS_CIRCLE);
    lv_style_set_bg_color(&style_circle_btn_red, lv_color_hex(0xF44336));
    lv_style_set_bg_grad_color(&style_circle_btn_red, lv_color_hex(0xD32F2F));
    lv_style_set_bg_grad_dir(&style_circle_btn_red, LV_GRAD_DIR_VER);
    lv_style_set_border_width(&style_circle_btn_red, 0);
    lv_style_set_shadow_width(&style_circle_btn_red, 15);
    lv_style_set_shadow_color(&style_circle_btn_red, lv_color_hex(0xD32F2F));
    lv_style_set_shadow_offset_y(&style_circle_btn_red, 5);
    lv_style_set_text_color(&style_circle_btn_red, lv_color_white());
    lv_style_set_transform_scale(&style_circle_btn_red, 256);

    /* Pressed state style - darker background and reduced shadow */
    lv_style_init(&style_btn_pressed);
    lv_style_set_shadow_width(&style_btn_pressed, 5);  /* Reduce shadow */
    lv_style_set_shadow_offset_y(&style_btn_pressed, 2);  /* Move shadow up */
    lv_style_set_transform_scale(&style_btn_pressed, 243);  /* Scale to 95% */
}

/**
 * Create round buttons with different styles
 */
static void create_round_buttons(void)
{
    lv_obj_t *screen = lv_scr_act();

    /* Initialize button styles */
    init_button_styles();

    /* Set a gradient background for the screen */
    static lv_style_t style_screen;
    lv_style_init(&style_screen);
    lv_style_set_bg_color(&style_screen, lv_color_hex(0x003a57));
    lv_style_set_bg_grad_color(&style_screen, lv_color_hex(0x001f3f));
    lv_style_set_bg_grad_dir(&style_screen, LV_GRAD_DIR_VER);
    lv_obj_add_style(screen, &style_screen, 0);

    /* Title label */
    lv_obj_t *title = lv_label_create(screen);
    lv_label_set_text(title, "이미지 버튼 데모");  /* Image Button Demo */
    if (korean_font) {
        lv_obj_set_style_text_font(title, korean_font, 0);
    } else {
        lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);
    }
    lv_obj_set_style_text_color(title, lv_color_white(), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 20);

    /* Container for buttons */
    lv_obj_t *cont = lv_obj_create(screen);
    lv_obj_set_size(cont, 280, 540);
    lv_obj_align(cont, LV_ALIGN_CENTER, 0, 20);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_color(cont, lv_color_hex(0xf0f0f0), 0);
    lv_obj_set_style_border_width(cont, 0, 0);
    lv_obj_set_style_radius(cont, 20, 0);
    lv_obj_set_style_pad_all(cont, 15, 0);

    /* Button 1: Blue round button - "재생" (Play) */
    lv_obj_t *btn1 = lv_button_create(cont);
    lv_obj_set_size(btn1, 120, 120);
    lv_obj_add_flag(btn1, LV_OBJ_FLAG_ADV_HITTEST);  /* Enable advanced hit testing */
    lv_obj_add_style(btn1, &style_circle_btn_blue, 0);
    lv_obj_add_style(btn1, &style_btn_pressed, LV_STATE_PRESSED);
    lv_obj_add_event_cb(btn1, circle_hit_test_event_cb, LV_EVENT_HIT_TEST, NULL);
    lv_obj_add_event_cb(btn1, button_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn1, button_visual_effect_handler, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(btn1, button_visual_effect_handler, LV_EVENT_RELEASED, NULL);
    lv_obj_add_event_cb(btn1, button_visual_effect_handler, LV_EVENT_PRESS_LOST, NULL);

    lv_obj_t *img1 = lv_image_create(btn1);
    lv_image_set_src(img1, "A:assets/icons/play." IMAGE_FORMAT);
    lv_obj_add_flag(img1, LV_OBJ_FLAG_EVENT_BUBBLE);  /* Allow events to bubble to button */
    lv_obj_align(img1, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_transform_pivot_x(img1, lv_pct(50), 0);  /* Set pivot to center (50%) */
    lv_obj_set_style_transform_pivot_y(img1, lv_pct(50), 0);

    /* Button 2: Green round button - "시작" (Start) */
    lv_obj_t *btn2 = lv_button_create(cont);
    lv_obj_set_size(btn2, 120, 120);
    lv_obj_add_flag(btn2, LV_OBJ_FLAG_ADV_HITTEST);  /* Enable advanced hit testing */
    lv_obj_add_style(btn2, &style_circle_btn_green, 0);
    lv_obj_add_style(btn2, &style_btn_pressed, LV_STATE_PRESSED);
    lv_obj_add_event_cb(btn2, circle_hit_test_event_cb, LV_EVENT_HIT_TEST, NULL);
    lv_obj_add_event_cb(btn2, button_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn2, button_visual_effect_handler, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(btn2, button_visual_effect_handler, LV_EVENT_RELEASED, NULL);
    lv_obj_add_event_cb(btn2, button_visual_effect_handler, LV_EVENT_PRESS_LOST, NULL);

    lv_obj_t *img2 = lv_image_create(btn2);
    lv_image_set_src(img2, "A:assets/icons/ok." IMAGE_FORMAT);
    lv_obj_add_flag(img2, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_align(img2, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_transform_pivot_x(img2, lv_pct(50), 0);
    lv_obj_set_style_transform_pivot_y(img2, lv_pct(50), 0);

    /* Button 3: Orange round button - "일시정지" (Pause) */
    lv_obj_t *btn3 = lv_button_create(cont);
    lv_obj_set_size(btn3, 120, 120);
    lv_obj_add_flag(btn3, LV_OBJ_FLAG_ADV_HITTEST);  /* Enable advanced hit testing */
    lv_obj_add_style(btn3, &style_circle_btn_orange, 0);
    lv_obj_add_style(btn3, &style_btn_pressed, LV_STATE_PRESSED);
    lv_obj_add_event_cb(btn3, circle_hit_test_event_cb, LV_EVENT_HIT_TEST, NULL);
    lv_obj_add_event_cb(btn3, button_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn3, button_visual_effect_handler, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(btn3, button_visual_effect_handler, LV_EVENT_RELEASED, NULL);
    lv_obj_add_event_cb(btn3, button_visual_effect_handler, LV_EVENT_PRESS_LOST, NULL);

    lv_obj_t *img3 = lv_image_create(btn3);
    lv_image_set_src(img3, "A:assets/icons/pause." IMAGE_FORMAT);
    lv_obj_add_flag(img3, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_align(img3, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_transform_pivot_x(img3, lv_pct(50), 0);
    lv_obj_set_style_transform_pivot_y(img3, lv_pct(50), 0);

    /* Button 4: Red round button - "정지" (Stop) */
    lv_obj_t *btn4 = lv_button_create(cont);
    lv_obj_set_size(btn4, 120, 120);
    lv_obj_add_flag(btn4, LV_OBJ_FLAG_ADV_HITTEST);  /* Enable advanced hit testing */
    lv_obj_add_style(btn4, &style_circle_btn_red, 0);
    lv_obj_add_style(btn4, &style_btn_pressed, LV_STATE_PRESSED);
    lv_obj_add_event_cb(btn4, circle_hit_test_event_cb, LV_EVENT_HIT_TEST, NULL);
    lv_obj_add_event_cb(btn4, button_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn4, button_visual_effect_handler, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(btn4, button_visual_effect_handler, LV_EVENT_RELEASED, NULL);
    lv_obj_add_event_cb(btn4, button_visual_effect_handler, LV_EVENT_PRESS_LOST, NULL);

    lv_obj_t *img4 = lv_image_create(btn4);
    lv_image_set_src(img4, "A:assets/icons/stop." IMAGE_FORMAT);
    lv_obj_add_flag(img4, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_align(img4, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_transform_pivot_x(img4, lv_pct(50), 0);
    lv_obj_set_style_transform_pivot_y(img4, lv_pct(50), 0);
}

/**
 * Main entry point
 */
int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;

    /* Initialize LVGL */
    lv_init();

    /* Initialize hardware abstraction layer */
    hal_init();

    /* Create the UI */
    create_round_buttons();

    printf("Round Image Button Demo Running...\n");
    printf("Window size: %dx%d\n", WINDOW_WIDTH, WINDOW_HEIGHT);
    printf("Image format: %s\n", IMAGE_FORMAT);
    printf("Click the buttons to see the effect!\n");

    /* Main loop */
    while (1) {
        /* Handle LVGL tasks */
        uint32_t delay = lv_timer_handler();

        /* Check if display is still valid (window closed) */
        if (display == NULL || lv_display_get_screen_active(display) == NULL) {
            break;
        }

        /* Small delay */
        lv_delay_ms(delay < 5 ? 5 : delay);
    }

    printf("Exiting...\n");

    return 0;
}
