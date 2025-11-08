/**
 * @file main.c
 * @brief Round Button Example Application using LVGL
 *
 * This application demonstrates round buttons with different styles
 * Window size: 320x640
 */

#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include <SDL2/SDL.h>

#define WINDOW_WIDTH  320
#define WINDOW_HEIGHT 640

static lv_display_t *display;
static lv_indev_t *indev;

/* Forward declarations */
static void create_round_buttons(void);
static void button_event_handler(lv_event_t *e);

/* Korean font */
static lv_font_t *korean_font = NULL;

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
}

/**
 * Event handler for button clicks
 */
static void button_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED) {
        lv_obj_t *label = lv_obj_get_child(btn, 0);
        const char *btn_text = lv_label_get_text(label);
        printf("Button clicked: %s\n", btn_text);
    }
}

/**
 * Create round buttons with different styles
 */
static void create_round_buttons(void)
{
    lv_obj_t *screen = lv_scr_act();

    /* Set a gradient background for the screen */
    static lv_style_t style_screen;
    lv_style_init(&style_screen);
    lv_style_set_bg_color(&style_screen, lv_color_hex(0x003a57));
    lv_style_set_bg_grad_color(&style_screen, lv_color_hex(0x001f3f));
    lv_style_set_bg_grad_dir(&style_screen, LV_GRAD_DIR_VER);
    lv_obj_add_style(screen, &style_screen, 0);

    /* Title label */
    lv_obj_t *title = lv_label_create(screen);
    lv_label_set_text(title, "원형 버튼 데모");
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

    /* Button 1: Primary round button */
    lv_obj_t *btn1 = lv_button_create(cont);
    lv_obj_set_size(btn1, 120, 120);
    lv_obj_set_style_radius(btn1, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(btn1, lv_color_hex(0x2196F3), 0);
    lv_obj_set_style_bg_grad_color(btn1, lv_color_hex(0x1976D2), 0);
    lv_obj_set_style_bg_grad_dir(btn1, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_shadow_width(btn1, 15, 0);
    lv_obj_set_style_shadow_color(btn1, lv_color_hex(0x1976D2), 0);
    lv_obj_set_style_shadow_offset_y(btn1, 5, 0);
    /* Disable transform scale on press to keep button same size */
    lv_obj_set_style_transform_scale(btn1, 256, LV_STATE_PRESSED);
    lv_obj_add_event_cb(btn1, button_event_handler, LV_EVENT_ALL, NULL);

    lv_obj_t *label1 = lv_label_create(btn1);
    lv_label_set_text(label1, "재생");
    if (korean_font) {
        lv_obj_set_style_text_font(label1, korean_font, 0);
    } else {
        lv_obj_set_style_text_font(label1, &lv_font_montserrat_20, 0);
    }
    lv_obj_set_style_text_color(label1, lv_color_white(), 0);
    lv_obj_center(label1);

    /* Button 2: Success round button */
    lv_obj_t *btn2 = lv_button_create(cont);
    lv_obj_set_size(btn2, 120, 120);
    lv_obj_set_style_radius(btn2, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(btn2, lv_color_hex(0x4CAF50), 0);
    lv_obj_set_style_bg_grad_color(btn2, lv_color_hex(0x388E3C), 0);
    lv_obj_set_style_bg_grad_dir(btn2, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_shadow_width(btn2, 15, 0);
    lv_obj_set_style_shadow_color(btn2, lv_color_hex(0x388E3C), 0);
    lv_obj_set_style_shadow_offset_y(btn2, 5, 0);
    /* Disable transform scale on press to keep button same size */
    lv_obj_set_style_transform_scale(btn2, 256, LV_STATE_PRESSED);
    lv_obj_add_event_cb(btn2, button_event_handler, LV_EVENT_ALL, NULL);

    lv_obj_t *label2 = lv_label_create(btn2);
    lv_label_set_text(label2, "시작");
    if (korean_font) {
        lv_obj_set_style_text_font(label2, korean_font, 0);
    } else {
        lv_obj_set_style_text_font(label2, &lv_font_montserrat_20, 0);
    }
    lv_obj_set_style_text_color(label2, lv_color_white(), 0);
    lv_obj_center(label2);

    /* Button 3: Warning round button */
    lv_obj_t *btn3 = lv_button_create(cont);
    lv_obj_set_size(btn3, 120, 120);
    lv_obj_set_style_radius(btn3, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(btn3, lv_color_hex(0xFF9800), 0);
    lv_obj_set_style_bg_grad_color(btn3, lv_color_hex(0xF57C00), 0);
    lv_obj_set_style_bg_grad_dir(btn3, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_shadow_width(btn3, 15, 0);
    lv_obj_set_style_shadow_color(btn3, lv_color_hex(0xF57C00), 0);
    lv_obj_set_style_shadow_offset_y(btn3, 5, 0);
    /* Disable transform scale on press to keep button same size */
    lv_obj_set_style_transform_scale(btn3, 256, LV_STATE_PRESSED);
    lv_obj_add_event_cb(btn3, button_event_handler, LV_EVENT_ALL, NULL);

    lv_obj_t *label3 = lv_label_create(btn3);
    lv_label_set_text(label3, "일시정지");
    if (korean_font) {
        lv_obj_set_style_text_font(label3, korean_font, 0);
    } else {
        lv_obj_set_style_text_font(label3, &lv_font_montserrat_20, 0);
    }
    lv_obj_set_style_text_color(label3, lv_color_white(), 0);
    lv_obj_center(label3);

    /* Button 4: Danger round button */
    lv_obj_t *btn4 = lv_button_create(cont);
    lv_obj_set_size(btn4, 120, 120);
    lv_obj_set_style_radius(btn4, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(btn4, lv_color_hex(0xF44336), 0);
    lv_obj_set_style_bg_grad_color(btn4, lv_color_hex(0xD32F2F), 0);
    lv_obj_set_style_bg_grad_dir(btn4, LV_GRAD_DIR_VER, 0);
    lv_obj_set_style_shadow_width(btn4, 15, 0);
    lv_obj_set_style_shadow_color(btn4, lv_color_hex(0xD32F2F), 0);
    lv_obj_set_style_shadow_offset_y(btn4, 5, 0);
    /* Disable transform scale on press to keep button same size */
    lv_obj_set_style_transform_scale(btn4, 256, LV_STATE_PRESSED);
    lv_obj_add_event_cb(btn4, button_event_handler, LV_EVENT_ALL, NULL);

    lv_obj_t *label4 = lv_label_create(btn4);
    lv_label_set_text(label4, "정지");
    if (korean_font) {
        lv_obj_set_style_text_font(label4, korean_font, 0);
    } else {
        lv_obj_set_style_text_font(label4, &lv_font_montserrat_20, 0);
    }
    lv_obj_set_style_text_color(label4, lv_color_white(), 0);
    lv_obj_center(label4);
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

    printf("Round Button Demo Running...\n");
    printf("Window size: %dx%d\n", WINDOW_WIDTH, WINDOW_HEIGHT);
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
