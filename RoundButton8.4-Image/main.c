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
#include <SDL2/SDL.h>

/* Include image decoder headers for PNG, JPEG, BMP, and GIF support */
#include "lvgl/src/extra/libs/png/lv_png.h"
#include "lvgl/src/extra/libs/sjpg/lv_sjpg.h"
#include "lvgl/src/extra/libs/bmp/lv_bmp.h"
#include "lvgl/src/extra/libs/gif/lv_gif.h"

#define WINDOW_WIDTH  320
#define WINDOW_HEIGHT 640

static lv_disp_t *display;

/* Forward declarations */
static void create_round_buttons(void);
static void button_event_handler(lv_event_t *e);
static void init_button_styles(void);
static void button_visual_effect_handler(lv_event_t *e);

/* Korean font */
static lv_font_t *korean_font = NULL;

/* Icon font for buttons (kept for fallback) */
static lv_font_t *icon_font = NULL;

/* Image formats for each button
 * Each button uses a different format to demonstrate all supported types:
 * - Button 1 (Play): PNG - Best quality with transparency
 * - Button 2 (OK): JPG - Smaller file size
 * - Button 3 (Pause): BMP - Uncompressed format
 * - Button 4 (Stop): GIF - Animation support
 */
#define IMG_FORMAT_BTN1 "png"
#define IMG_FORMAT_BTN2 "jpg"
#define IMG_FORMAT_BTN3 "bmp"
#define IMG_FORMAT_BTN4 "gif"

/* Reusable button styles */
static lv_style_t style_circle_btn_blue;
static lv_style_t style_circle_btn_green;
static lv_style_t style_circle_btn_orange;
static lv_style_t style_circle_btn_red;

/* Pressed state styles for buttons */
static lv_style_t style_btn_pressed;

/* Global SDL objects */
static SDL_Window *sdl_window = NULL;
static SDL_Renderer *sdl_renderer = NULL;
static SDL_Texture *sdl_texture = NULL;

/**
 * Flush callback for LVGL 8.4 - renders to SDL texture
 */
static void flush_cb(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p)
{
    if (sdl_renderer == NULL || sdl_texture == NULL) {
        lv_disp_flush_ready(drv);
        return;
    }

    /* Lock texture for direct pixel access */
    void *pixels;
    int pitch;
    SDL_LockTexture(sdl_texture, NULL, &pixels, &pitch);

    uint32_t *pixel_data = (uint32_t *)pixels;

    /* Copy the LVGL framebuffer to SDL2 texture */
    for (int y = area->y1; y <= area->y2; y++) {
        for (int x = area->x1; x <= area->x2; x++) {
            if (x >= 0 && x < WINDOW_WIDTH && y >= 0 && y < WINDOW_HEIGHT) {
                int index = y * WINDOW_WIDTH + x;
                lv_color_t lv_col = *color_p;
                uint32_t color = lv_color_to32(lv_col);
                pixel_data[index] = color;
            }
            color_p++;
        }
    }

    SDL_UnlockTexture(sdl_texture);

    /* Render to screen */
    SDL_RenderClear(sdl_renderer);
    SDL_RenderCopy(sdl_renderer, sdl_texture, NULL, NULL);
    SDL_RenderPresent(sdl_renderer);

    lv_disp_flush_ready(drv);
}

/**
 * Input device callback - handle mouse/touch input from SDL
 */
static void indev_read_cb(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    (void)drv;  /* Unused parameter */

    /* Get current mouse state */
    int x, y;
    uint32_t mouse_state = SDL_GetMouseState(&x, &y);

    data->point.x = x;
    data->point.y = y;
    data->state = (mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT)) ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}

/**
 * Initialize SDL and LVGL display
 */
static void hal_init(void)
{
    /* Initialize SDL */
    printf("Initializing SDL2...\n");
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Error: SDL_Init failed: %s\n", SDL_GetError());
        exit(1);
    }

    /* Create SDL window */
    printf("Creating SDL2 window (%dx%d)...\n", WINDOW_WIDTH, WINDOW_HEIGHT);
    sdl_window = SDL_CreateWindow(
        "Round Button Demo",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (sdl_window == NULL) {
        printf("Error: Failed to create SDL window: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    /* Create SDL renderer with V-sync */
    sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (sdl_renderer == NULL) {
        printf("Error: Failed to create SDL renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(sdl_window);
        SDL_Quit();
        exit(1);
    }

    /* Create SDL texture for rendering */
    sdl_texture = SDL_CreateTexture(
        sdl_renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        WINDOW_WIDTH,
        WINDOW_HEIGHT
    );

    if (sdl_texture == NULL) {
        printf("Error: Failed to create SDL texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(sdl_renderer);
        SDL_DestroyWindow(sdl_window);
        SDL_Quit();
        exit(1);
    }

    printf("SDL2 initialized successfully\n");

    /* Create a display buffer for rendering */
    static lv_disp_draw_buf_t draw_buf;
    static lv_color_t buf_1[WINDOW_WIDTH * 64];  /* First buffer (20% of screen) */
    static lv_color_t buf_2[WINDOW_WIDTH * 64];  /* Second buffer for double-buffering */

    lv_disp_draw_buf_init(&draw_buf, buf_1, buf_2, WINDOW_WIDTH * 64);

    /* Configure display driver */
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf = &draw_buf;
    disp_drv.hor_res = WINDOW_WIDTH;
    disp_drv.ver_res = WINDOW_HEIGHT;
    disp_drv.flush_cb = flush_cb;  /* Use our flush callback */

    display = lv_disp_drv_register(&disp_drv);

    if (display == NULL) {
        printf("Error: Failed to register display driver\n");
        SDL_DestroyTexture(sdl_texture);
        SDL_DestroyRenderer(sdl_renderer);
        SDL_DestroyWindow(sdl_window);
        SDL_Quit();
        exit(1);
    }

    /* Initialize and register input device (mouse/touch) */
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = indev_read_cb;
    lv_indev_drv_register(&indev_drv);

    /* Load Korean font using FreeType */
    #if LV_USE_FREETYPE
    lv_ft_info_t ft_info;
    ft_info.name = "assets/NanumGothicCoding.ttf";
    ft_info.weight = 20;
    ft_info.style = FT_FONT_STYLE_NORMAL;
    ft_info.mem = NULL;
    if (!lv_ft_font_init(&ft_info)) {
        printf("Warning: Failed to load Korean font, using default font\n");
        korean_font = NULL;
    } else {
        korean_font = ft_info.font;
    }
    #endif

    /* Load icon font using FreeType (larger size for button icons) */
    #if LV_USE_FREETYPE
    lv_ft_info_t ft_info_icon;
    ft_info_icon.name = "assets/NanumGothicCoding-Bold.ttf";
    ft_info_icon.weight = 48;
    ft_info_icon.style = FT_FONT_STYLE_NORMAL;
    ft_info_icon.mem = NULL;
    if (!lv_ft_font_init(&ft_info_icon)) {
        printf("Warning: Failed to load icon font, using default symbols\n");
        icon_font = NULL;
    } else {
        icon_font = ft_info_icon.font;
    }
    #endif
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
 * Changes opacity of the image for better feedback
 */
static void button_visual_effect_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *btn = lv_event_get_target(e);
    lv_obj_t *img = lv_obj_get_child(btn, 0);

    if (code == LV_EVENT_PRESSED) {
        /* Reduce opacity for press effect */
        lv_obj_set_style_img_opa(img, LV_OPA_70, 0);  /* 70% opacity */
    }
    else if (code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) {
        /* Return to full opacity */
        lv_obj_set_style_img_opa(img, LV_OPA_COVER, 0);  /* 100% opacity */
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
        /* Get the image path from button's user data */
        const char *img_path = (const char*)lv_obj_get_user_data(btn);

        if (img_path) {
            printf("Button clicked: %s\n", img_path);
        }
    }
}

/**
 * Initialize reusable button styles
 */
static void init_button_styles(void)
{
    /* Blue circle button style - RGB(33, 150, 243) to match icon background */
    lv_style_init(&style_circle_btn_blue);
    lv_style_set_radius(&style_circle_btn_blue, LV_RADIUS_CIRCLE);
    lv_style_set_bg_color(&style_circle_btn_blue, lv_color_make(33, 150, 243));
    lv_style_set_bg_grad_color(&style_circle_btn_blue, lv_color_make(33, 150, 243));
    lv_style_set_bg_grad_dir(&style_circle_btn_blue, LV_GRAD_DIR_VER);
    lv_style_set_border_width(&style_circle_btn_blue, 0);
    lv_style_set_shadow_width(&style_circle_btn_blue, 15);
    lv_style_set_shadow_color(&style_circle_btn_blue, lv_color_make(33, 150, 243));
    lv_style_set_shadow_ofs_y(&style_circle_btn_blue, 5);
    lv_style_set_text_color(&style_circle_btn_blue, lv_color_white());

    /* Green circle button style - RGB(76, 175, 80) to match icon background */
    lv_style_init(&style_circle_btn_green);
    lv_style_set_radius(&style_circle_btn_green, LV_RADIUS_CIRCLE);
    lv_style_set_bg_color(&style_circle_btn_green, lv_color_make(76, 175, 80));
    lv_style_set_bg_grad_color(&style_circle_btn_green, lv_color_make(76, 175, 80));
    lv_style_set_bg_grad_dir(&style_circle_btn_green, LV_GRAD_DIR_VER);
    lv_style_set_border_width(&style_circle_btn_green, 0);
    lv_style_set_shadow_width(&style_circle_btn_green, 15);
    lv_style_set_shadow_color(&style_circle_btn_green, lv_color_make(76, 175, 80));
    lv_style_set_shadow_ofs_y(&style_circle_btn_green, 5);
    lv_style_set_text_color(&style_circle_btn_green, lv_color_white());

    /* Orange circle button style - RGB(255, 152, 0) to match icon background */
    lv_style_init(&style_circle_btn_orange);
    lv_style_set_radius(&style_circle_btn_orange, LV_RADIUS_CIRCLE);
    lv_style_set_bg_color(&style_circle_btn_orange, lv_color_make(255, 152, 0));
    lv_style_set_bg_grad_color(&style_circle_btn_orange, lv_color_make(255, 152, 0));
    lv_style_set_bg_grad_dir(&style_circle_btn_orange, LV_GRAD_DIR_VER);
    lv_style_set_border_width(&style_circle_btn_orange, 0);
    lv_style_set_shadow_width(&style_circle_btn_orange, 15);
    lv_style_set_shadow_color(&style_circle_btn_orange, lv_color_make(255, 152, 0));
    lv_style_set_shadow_ofs_y(&style_circle_btn_orange, 5);
    lv_style_set_text_color(&style_circle_btn_orange, lv_color_white());

    /* Red circle button style - RGB(244, 67, 54) to match icon background */
    lv_style_init(&style_circle_btn_red);
    lv_style_set_radius(&style_circle_btn_red, LV_RADIUS_CIRCLE);
    lv_style_set_bg_color(&style_circle_btn_red, lv_color_make(244, 67, 54));
    lv_style_set_bg_grad_color(&style_circle_btn_red, lv_color_make(244, 67, 54));
    lv_style_set_bg_grad_dir(&style_circle_btn_red, LV_GRAD_DIR_VER);
    lv_style_set_border_width(&style_circle_btn_red, 0);
    lv_style_set_shadow_width(&style_circle_btn_red, 15);
    lv_style_set_shadow_color(&style_circle_btn_red, lv_color_make(244, 67, 54));
    lv_style_set_shadow_ofs_y(&style_circle_btn_red, 5);
    lv_style_set_text_color(&style_circle_btn_red, lv_color_white());

    /* Pressed state style - darker background and reduced shadow */
    lv_style_init(&style_btn_pressed);
    lv_style_set_shadow_width(&style_btn_pressed, 5);  /* Reduce shadow */
    lv_style_set_shadow_ofs_y(&style_btn_pressed, 2);  /* Move shadow up */
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
    lv_obj_t *btn1 = lv_btn_create(cont);
    lv_obj_set_size(btn1, 120, 120);
    lv_obj_add_flag(btn1, LV_OBJ_FLAG_ADV_HITTEST);  /* Enable advanced hit testing */
    lv_obj_add_style(btn1, &style_circle_btn_blue, 0);
    lv_obj_add_style(btn1, &style_btn_pressed, LV_STATE_PRESSED);
    lv_obj_add_event_cb(btn1, circle_hit_test_event_cb, LV_EVENT_HIT_TEST, NULL);
    lv_obj_add_event_cb(btn1, button_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn1, button_visual_effect_handler, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(btn1, button_visual_effect_handler, LV_EVENT_RELEASED, NULL);
    lv_obj_add_event_cb(btn1, button_visual_effect_handler, LV_EVENT_PRESS_LOST, NULL);

    lv_obj_set_user_data(btn1, (void*)"A:assets/icons/play." IMG_FORMAT_BTN1);  /* Store path */

    lv_obj_t *img1 = lv_img_create(btn1);
    lv_img_set_src(img1, "A:assets/icons/play." IMG_FORMAT_BTN1);  /* PNG format */
    lv_obj_add_flag(img1, LV_OBJ_FLAG_EVENT_BUBBLE);  /* Allow events to bubble to button */
    lv_obj_align(img1, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_transform_pivot_x(img1, lv_pct(50), 0);  /* Set pivot to center (50%) */
    lv_obj_set_style_transform_pivot_y(img1, lv_pct(50), 0);

    /* Button 2: Green round button - "시작" (Start) */
    lv_obj_t *btn2 = lv_btn_create(cont);
    lv_obj_set_size(btn2, 120, 120);
    lv_obj_add_flag(btn2, LV_OBJ_FLAG_ADV_HITTEST);  /* Enable advanced hit testing */
    lv_obj_add_style(btn2, &style_circle_btn_green, 0);
    lv_obj_add_style(btn2, &style_btn_pressed, LV_STATE_PRESSED);
    lv_obj_add_event_cb(btn2, circle_hit_test_event_cb, LV_EVENT_HIT_TEST, NULL);
    lv_obj_add_event_cb(btn2, button_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn2, button_visual_effect_handler, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(btn2, button_visual_effect_handler, LV_EVENT_RELEASED, NULL);
    lv_obj_add_event_cb(btn2, button_visual_effect_handler, LV_EVENT_PRESS_LOST, NULL);

    lv_obj_set_user_data(btn2, (void*)"A:assets/icons/ok." IMG_FORMAT_BTN2);  /* Store path */

    lv_obj_t *img2 = lv_img_create(btn2);
    lv_img_set_src(img2, "A:assets/icons/ok." IMG_FORMAT_BTN2);  /* JPG format */
    lv_obj_add_flag(img2, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_align(img2, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_transform_pivot_x(img2, lv_pct(50), 0);
    lv_obj_set_style_transform_pivot_y(img2, lv_pct(50), 0);

    /* Button 3: Orange round button - "일시정지" (Pause) */
    lv_obj_t *btn3 = lv_btn_create(cont);
    lv_obj_set_size(btn3, 120, 120);
    lv_obj_add_flag(btn3, LV_OBJ_FLAG_ADV_HITTEST);  /* Enable advanced hit testing */
    lv_obj_add_style(btn3, &style_circle_btn_orange, 0);
    lv_obj_add_style(btn3, &style_btn_pressed, LV_STATE_PRESSED);
    lv_obj_add_event_cb(btn3, circle_hit_test_event_cb, LV_EVENT_HIT_TEST, NULL);
    lv_obj_add_event_cb(btn3, button_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn3, button_visual_effect_handler, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(btn3, button_visual_effect_handler, LV_EVENT_RELEASED, NULL);
    lv_obj_add_event_cb(btn3, button_visual_effect_handler, LV_EVENT_PRESS_LOST, NULL);

    lv_obj_set_user_data(btn3, (void*)"A:assets/icons/pause." IMG_FORMAT_BTN3);  /* Store path */

    lv_obj_t *img3 = lv_img_create(btn3);
    lv_img_set_src(img3, "A:assets/icons/pause." IMG_FORMAT_BTN3);  /* BMP format */
    lv_obj_add_flag(img3, LV_OBJ_FLAG_EVENT_BUBBLE);
    lv_obj_align(img3, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_transform_pivot_x(img3, lv_pct(50), 0);
    lv_obj_set_style_transform_pivot_y(img3, lv_pct(50), 0);

    /* Button 4: Red round button - "정지" (Stop) */
    lv_obj_t *btn4 = lv_btn_create(cont);
    lv_obj_set_size(btn4, 120, 120);
    lv_obj_add_flag(btn4, LV_OBJ_FLAG_ADV_HITTEST);  /* Enable advanced hit testing */
    lv_obj_add_style(btn4, &style_circle_btn_red, 0);
    lv_obj_add_style(btn4, &style_btn_pressed, LV_STATE_PRESSED);
    lv_obj_add_event_cb(btn4, circle_hit_test_event_cb, LV_EVENT_HIT_TEST, NULL);
    lv_obj_add_event_cb(btn4, button_event_handler, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(btn4, button_visual_effect_handler, LV_EVENT_PRESSED, NULL);
    lv_obj_add_event_cb(btn4, button_visual_effect_handler, LV_EVENT_RELEASED, NULL);
    lv_obj_add_event_cb(btn4, button_visual_effect_handler, LV_EVENT_PRESS_LOST, NULL);

    lv_obj_set_user_data(btn4, (void*)"A:assets/icons/stop." IMG_FORMAT_BTN4);  /* Store path */

    lv_obj_t *img4 = lv_gif_create(btn4);  /* GIF requires lv_gif widget */
    lv_gif_set_src(img4, "A:assets/icons/stop." IMG_FORMAT_BTN4);  /* GIF format */
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

    /* Initialize image decoders - MUST be called before displaying images */
    lv_png_init();
    lv_split_jpeg_init();

    /* Initialize hardware abstraction layer */
    hal_init();

    /* Create the UI */
    create_round_buttons();

    printf("Round Image Button Demo Running...\n");
    printf("Window size: %dx%d\n", WINDOW_WIDTH, WINDOW_HEIGHT);
    printf("Image formats per button:\n");
    printf("  Button 1 (Play): %s\n", IMG_FORMAT_BTN1);
    printf("  Button 2 (OK):   %s\n", IMG_FORMAT_BTN2);
    printf("  Button 3 (Pause): %s\n", IMG_FORMAT_BTN3);
    printf("  Button 4 (Stop): %s\n", IMG_FORMAT_BTN4);
    printf("Click the buttons to see the effect!\n");

    /* Main loop */
    int running = 1;
    uint32_t last_time = SDL_GetTicks();
    SDL_Event event;

    while (running) {
        /* Handle SDL events */
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    printf("Window close button clicked\n");
                    running = 0;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        printf("ESC key pressed\n");
                        running = 0;
                    }
                    break;
            }
        }

        if (!running) break;

        /* Update LVGL timing */
        uint32_t current_time = SDL_GetTicks();
        uint32_t elapsed = current_time - last_time;
        if (elapsed > 0) {
            lv_tick_inc(elapsed);
            last_time = current_time;
        }

        /* Handle LVGL tasks */
        lv_timer_handler();

        /* Small delay to reduce CPU usage */
        SDL_Delay(5);
    }

    /* Cleanup SDL resources */
    printf("Cleaning up...\n");
    if (sdl_texture != NULL) {
        SDL_DestroyTexture(sdl_texture);
        sdl_texture = NULL;
    }
    if (sdl_renderer != NULL) {
        SDL_DestroyRenderer(sdl_renderer);
        sdl_renderer = NULL;
    }
    if (sdl_window != NULL) {
        SDL_DestroyWindow(sdl_window);
        sdl_window = NULL;
    }
    SDL_Quit();

    printf("Exiting...\n");

    return 0;
}
