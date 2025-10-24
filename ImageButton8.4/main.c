#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <locale.h>
#include <SDL2/SDL.h>

#include "lvgl.h"
#include "lv_conf.h"
#include "lvgl/src/extra/libs/freetype/lv_freetype.h"

/*******************************************************************************
 * Display Configuration
 ******************************************************************************/

#define DISP_HOR_RES 480
#define DISP_VER_RES 320

// Double buffer for smoother rendering
#define BUF_SIZE (DISP_HOR_RES * DISP_VER_RES / 10)
static lv_disp_draw_buf_t disp_buf;
static lv_color_t buf1[BUF_SIZE];
static lv_color_t buf2[BUF_SIZE];

// SDL2 Window and Renderer
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *texture = NULL;

// Input device driver
static lv_indev_drv_t indev_drv;
static lv_indev_t *indev;

// Korean fonts
static lv_font_t *korean_font_16 = NULL;
static lv_font_t *korean_font_20 = NULL;

/*******************************************************************************
 * Button Click Handlers
 ******************************************************************************/

static void button_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED) {
        printf("Button clicked!\n");
        // Add visual feedback by changing color
        lv_obj_set_style_bg_color(obj, lv_color_hex(0x4CAF50), LV_PART_MAIN);
    }
    else if (code == LV_EVENT_LONG_PRESSED) {
        printf("Button long pressed!\n");
    }
}

static void toggle_button_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        if (lv_obj_has_state(obj, LV_STATE_CHECKED)) {
            printf("Toggle button is ON\n");
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x2196F3), LV_PART_MAIN);
        }
        else {
            printf("Toggle button is OFF\n");
            lv_obj_set_style_bg_color(obj, lv_color_hex(0x757575), LV_PART_MAIN);
        }
    }
}

/*******************************************************************************
 * SDL2 Display Driver
 ******************************************************************************/

static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
    if (renderer == NULL || texture == NULL) {
        lv_disp_flush_ready(disp_drv);
        return;
    }

    // Lock texture for direct pixel access
    void *pixels;
    int pitch;
    SDL_LockTexture(texture, NULL, &pixels, &pitch);

    uint32_t *pixel_data = (uint32_t *)pixels;
    (void)pitch;  // Pitch is fixed, pixel_data is sufficient

    // Copy the LVGL framebuffer to SDL2 texture
    for (int y = area->y1; y <= area->y2; y++) {
        for (int x = area->x1; x <= area->x2; x++) {
            int index = y * DISP_HOR_RES + x;
            uint32_t color = lv_color_to32(*color_p);
            pixel_data[index] = color;
            color_p++;
        }
    }

    SDL_UnlockTexture(texture);

    // Render to screen
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    lv_disp_flush_ready(disp_drv);
}

/*******************************************************************************
 * Input Device Driver (Mouse/Touch)
 ******************************************************************************/

static void indev_read(lv_indev_drv_t *drv, lv_indev_data_t *data)
{
    (void)drv;  // Unused parameter

    // Get current mouse state
    int x, y;
    uint32_t mouse_state = SDL_GetMouseState(&x, &y);

    data->point.x = x;
    data->point.y = y;
    data->state = (mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT)) ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}

/*******************************************************************************
 * Create Button UI
 ******************************************************************************/

static void create_buttons(void)
{
    lv_obj_t *scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0xF5F5F5), LV_PART_MAIN);

    // Title with Korean font
    lv_obj_t *title = lv_label_create(scr);
    lv_label_set_text(title, "LVGL 8.4 버튼 데모");
    lv_obj_set_pos(title, 10, 10);
    if (korean_font_20) {
        lv_obj_set_style_text_font(title, korean_font_20, LV_PART_MAIN);
    }
    lv_obj_set_style_text_color(title, lv_color_hex(0x000000), LV_PART_MAIN);

    // Button 1
    lv_obj_t *btn1 = lv_btn_create(scr);
    lv_obj_set_pos(btn1, 20, 50);
    lv_obj_set_size(btn1, 100, 40);
    lv_obj_set_style_bg_color(btn1, lv_color_hex(0xFF9800), LV_PART_MAIN);
    lv_obj_set_style_border_width(btn1, 2, LV_PART_MAIN);
    lv_obj_set_style_border_color(btn1, lv_color_hex(0x333333), LV_PART_MAIN);
    lv_obj_add_event_cb(btn1, button_event_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_t *label1 = lv_label_create(btn1);
    lv_label_set_text(label1, "버튼 1");
    lv_obj_center(label1);
    if (korean_font_16) {
        lv_obj_set_style_text_font(label1, korean_font_16, LV_PART_MAIN);
    }
    lv_obj_set_style_text_color(label1, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    // Button 2
    lv_obj_t *btn2 = lv_btn_create(scr);
    lv_obj_set_pos(btn2, 190, 50);
    lv_obj_set_size(btn2, 100, 40);
    lv_obj_set_style_bg_color(btn2, lv_color_hex(0xFF9800), LV_PART_MAIN);
    lv_obj_set_style_border_width(btn2, 2, LV_PART_MAIN);
    lv_obj_set_style_border_color(btn2, lv_color_hex(0x333333), LV_PART_MAIN);
    lv_obj_add_event_cb(btn2, button_event_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_t *label2 = lv_label_create(btn2);
    lv_label_set_text(label2, "버튼 2");
    lv_obj_center(label2);
    if (korean_font_16) {
        lv_obj_set_style_text_font(label2, korean_font_16, LV_PART_MAIN);
    }
    lv_obj_set_style_text_color(label2, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    // Button 3
    lv_obj_t *btn3 = lv_btn_create(scr);
    lv_obj_set_pos(btn3, 360, 50);
    lv_obj_set_size(btn3, 100, 40);
    lv_obj_set_style_bg_color(btn3, lv_color_hex(0xFF9800), LV_PART_MAIN);
    lv_obj_set_style_border_width(btn3, 2, LV_PART_MAIN);
    lv_obj_set_style_border_color(btn3, lv_color_hex(0x333333), LV_PART_MAIN);
    lv_obj_add_event_cb(btn3, button_event_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_t *label3 = lv_label_create(btn3);
    lv_label_set_text(label3, "버튼 3");
    lv_obj_center(label3);
    if (korean_font_16) {
        lv_obj_set_style_text_font(label3, korean_font_16, LV_PART_MAIN);
    }
    lv_obj_set_style_text_color(label3, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    // Toggle Button 1
    lv_obj_t *toggle_btn1 = lv_btn_create(scr);
    lv_obj_set_pos(toggle_btn1, 20, 120);
    lv_obj_set_size(toggle_btn1, 100, 40);
    lv_obj_set_style_bg_color(toggle_btn1, lv_color_hex(0x757575), LV_PART_MAIN);
    lv_obj_set_style_border_width(toggle_btn1, 2, LV_PART_MAIN);
    lv_obj_set_style_border_color(toggle_btn1, lv_color_hex(0x333333), LV_PART_MAIN);
    lv_obj_add_flag(toggle_btn1, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_add_event_cb(toggle_btn1, toggle_button_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t *toggle_label1 = lv_label_create(toggle_btn1);
    lv_label_set_text(toggle_label1, "토글 1");
    lv_obj_center(toggle_label1);
    if (korean_font_16) {
        lv_obj_set_style_text_font(toggle_label1, korean_font_16, LV_PART_MAIN);
    }
    lv_obj_set_style_text_color(toggle_label1, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    // Toggle Button 2
    lv_obj_t *toggle_btn2 = lv_btn_create(scr);
    lv_obj_set_pos(toggle_btn2, 190, 120);
    lv_obj_set_size(toggle_btn2, 100, 40);
    lv_obj_set_style_bg_color(toggle_btn2, lv_color_hex(0x757575), LV_PART_MAIN);
    lv_obj_set_style_border_width(toggle_btn2, 2, LV_PART_MAIN);
    lv_obj_set_style_border_color(toggle_btn2, lv_color_hex(0x333333), LV_PART_MAIN);
    lv_obj_add_flag(toggle_btn2, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_add_event_cb(toggle_btn2, toggle_button_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t *toggle_label2 = lv_label_create(toggle_btn2);
    lv_label_set_text(toggle_label2, "토글 2");
    lv_obj_center(toggle_label2);
    if (korean_font_16) {
        lv_obj_set_style_text_font(toggle_label2, korean_font_16, LV_PART_MAIN);
    }
    lv_obj_set_style_text_color(toggle_label2, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    // Disabled Button
    lv_obj_t *disabled_btn = lv_btn_create(scr);
    lv_obj_set_pos(disabled_btn, 360, 120);
    lv_obj_set_size(disabled_btn, 100, 40);
    lv_obj_set_style_bg_color(disabled_btn, lv_color_hex(0xBDBDBD), LV_PART_MAIN);
    lv_obj_set_style_border_width(disabled_btn, 2, LV_PART_MAIN);
    lv_obj_set_style_border_color(disabled_btn, lv_color_hex(0x999999), LV_PART_MAIN);
    lv_obj_add_state(disabled_btn, LV_STATE_DISABLED);

    lv_obj_t *disabled_label = lv_label_create(disabled_btn);
    lv_label_set_text(disabled_label, "비활성");
    lv_obj_center(disabled_label);
    if (korean_font_16) {
        lv_obj_set_style_text_font(disabled_label, korean_font_16, LV_PART_MAIN);
    }
    lv_obj_set_style_text_color(disabled_label, lv_color_hex(0x666666), LV_PART_MAIN);

    // Info Label with Korean text
    lv_obj_t *info = lv_label_create(scr);
    lv_label_set_text(info, "버튼을 클릭하여 시각적 피드백을 확인하세요!");
    lv_obj_set_pos(info, 10, 200);
    lv_obj_set_width(info, 460);
    lv_label_set_long_mode(info, LV_LABEL_LONG_WRAP);
    if (korean_font_16) {
        lv_obj_set_style_text_font(info, korean_font_16, LV_PART_MAIN);
    }
    lv_obj_set_style_text_color(info, lv_color_hex(0x333333), LV_PART_MAIN);
}

/*******************************************************************************
 * Font Initialization
 ******************************************************************************/

static void init_korean_fonts(void)
{
    // Set locale to support Korean characters
    setlocale(LC_ALL, "ko_KR.UTF-8");

    // Initialize FreeType library (0 for defaults)
    if (!lv_freetype_init(0, 0, 0)) {
        fprintf(stderr, "Failed to initialize FreeType library!\n");
        return;
    }

    // Create font info structure for size 20
    static lv_ft_info_t info_20 = {0};
    info_20.name = "assets/fonts/NanumGothicCoding.ttf";
    info_20.weight = 20;
    info_20.style = FT_FONT_STYLE_NORMAL;

    // Load NanumGothicCoding font at size 20 (for title)
    if (!lv_ft_font_init(&info_20)) {
        fprintf(stderr, "Failed to load Korean font at size 20!\n");
    } else {
        korean_font_20 = info_20.font;
        printf("Korean font (size 20) loaded successfully!\n");
    }

    // Create font info structure for size 16
    static lv_ft_info_t info_16 = {0};
    info_16.name = "assets/fonts/NanumGothicCoding.ttf";
    info_16.weight = 16;
    info_16.style = FT_FONT_STYLE_NORMAL;

    // Load NanumGothicCoding font at size 16 (for buttons and text)
    if (!lv_ft_font_init(&info_16)) {
        fprintf(stderr, "Failed to load Korean font at size 16!\n");
    } else {
        korean_font_16 = info_16.font;
        printf("Korean font (size 16) loaded successfully!\n");
    }
}

/*******************************************************************************
 * Main Application
 ******************************************************************************/

int main(void)
{
    // Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Failed to initialize SDL2: %s\n", SDL_GetError());
        return 1;
    }

    // Create window
    window = SDL_CreateWindow(
        "LVGL 8.4 Button Demo",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        DISP_HOR_RES,
        DISP_VER_RES,
        SDL_WINDOW_SHOWN
    );

    if (window == NULL) {
        fprintf(stderr, "Failed to create SDL window: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL) {
        fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Create texture for framebuffer
    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        DISP_HOR_RES,
        DISP_VER_RES
    );

    if (texture == NULL) {
        fprintf(stderr, "Failed to create texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Initialize LVGL
    lv_init();

    // Initialize display buffer (double-buffering)
    lv_disp_draw_buf_init(&disp_buf, buf1, buf2, BUF_SIZE);

    // Create and register display driver
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.draw_buf = &disp_buf;
    disp_drv.flush_cb = disp_flush;
    disp_drv.hor_res = DISP_HOR_RES;
    disp_drv.ver_res = DISP_VER_RES;
    lv_disp_drv_register(&disp_drv);

    // Create and register input device driver
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = indev_read;
    indev = lv_indev_drv_register(&indev_drv);

    // Initialize Korean fonts
    init_korean_fonts();

    // Create the UI
    create_buttons();

    // Main event loop
    int running = 1;
    uint32_t last_time = SDL_GetTicks();

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_ESCAPE) {
                    running = 0;
                }
            }
        }

        // Update LVGL timing
        uint32_t current_time = SDL_GetTicks();
        uint32_t elapsed = current_time - last_time;
        if (elapsed > 0) {
            lv_tick_inc(elapsed);
            last_time = current_time;
        }

        // Handle LVGL tasks
        lv_timer_handler();

        // Small delay to reduce CPU usage
        SDL_Delay(5);
    }

    // Cleanup
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
