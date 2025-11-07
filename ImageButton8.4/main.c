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
#include "lvgl/src/extra/libs/png/lv_png.h"
#include "lvgl/src/extra/libs/bmp/lv_bmp.h"
#include "lvgl/src/extra/libs/sjpg/lv_sjpg.h"

/*******************************************************************************
 * Display Configuration
 ******************************************************************************/

#define DISP_HOR_RES 320
#define DISP_VER_RES 640

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

// Timer callback to remove image border
static void remove_border_timer_cb(lv_timer_t *timer)
{
    lv_obj_t *img = (lv_obj_t *)timer->user_data;
    lv_obj_set_style_border_width(img, 0, 0);
    lv_timer_del(timer);
}

// Animation callback for button color change
static void btn_color_anim_cb(void *var, int32_t value)
{
    lv_obj_t *btn = (lv_obj_t *)var;
    lv_color_t color;

    // Check if this is a toggle button by looking for the checkable flag
    if (lv_obj_has_flag(btn, LV_OBJ_FLAG_CHECKABLE)) {
        // Toggle button color animation (blue to gray)
        if (value < 500) {
            // Gray color (OFF state)
            color = lv_color_hex(0x757575);
        } else {
            // Blue color (ON state)
            color = lv_color_hex(0x2196F3);
        }
    } else {
        // Regular button color animation (orange to green)
        if (value < 500) {
            // Green color
            color = lv_color_hex(0x4CAF50);
        } else {
            // Orange color
            color = lv_color_hex(0xFF9800);
        }
    }

    lv_obj_set_style_bg_color(btn, color, LV_PART_MAIN);
}

// Animation callback for toggle button 2 color change (green colors)
static void toggle_btn2_color_anim_cb(void *var, int32_t value)
{
    lv_obj_t *btn = (lv_obj_t *)var;
    lv_color_t color;

    // Toggle button 2 color animation (green to dark gray)
    if (value < 500) {
        // Dark gray color (OFF state)
        color = lv_color_hex(0x424242);
    } else {
        // Green color (ON state)
        color = lv_color_hex(0x4CAF50);
    }

    lv_obj_set_style_bg_color(btn, color, LV_PART_MAIN);
}

static void button_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (code == LV_EVENT_CLICKED) {
        printf("Button clicked!\n");
        
        // Find the image object within the button
        lv_obj_t *img = NULL;
        uint32_t child_cnt = lv_obj_get_child_cnt(obj);
        for (uint32_t i = 0; i < child_cnt; i++) {
            lv_obj_t *child = lv_obj_get_child(obj, i);
            if (lv_obj_check_type(child, &lv_img_class)) {
                img = child;
                break;
            }
        }
        
        if (img != NULL) {
            // Simple and safe visual effect - add a temporary border to the image
            lv_obj_set_style_border_width(img, 3, 0);
            lv_obj_set_style_border_color(img, lv_color_hex(0x00FF00), 0);
            
            // Create a timer to remove the border after 200ms
            lv_timer_t *border_timer = lv_timer_create(remove_border_timer_cb, 200, img);
            lv_timer_set_repeat_count(border_timer, 1);
        }
        
        // Button color animation
        lv_anim_t btn_anim;
        lv_anim_init(&btn_anim);
        lv_anim_set_var(&btn_anim, obj);
        lv_anim_set_values(&btn_anim, 1000, 500);
        lv_anim_set_time(&btn_anim, 300);
        lv_anim_set_exec_cb(&btn_anim, btn_color_anim_cb);
        lv_anim_set_path_cb(&btn_anim, lv_anim_path_ease_in_out);
        lv_anim_set_playback_time(&btn_anim, 300);
        lv_anim_set_playback_delay(&btn_anim, 0);
        lv_anim_start(&btn_anim);
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

            // Animated color change to blue
            lv_anim_t color_anim;
            lv_anim_init(&color_anim);
            lv_anim_set_var(&color_anim, obj);
            lv_anim_set_values(&color_anim, 0, 1000);
            lv_anim_set_time(&color_anim, 300);
            lv_anim_set_exec_cb(&color_anim, btn_color_anim_cb);
            lv_anim_set_path_cb(&color_anim, lv_anim_path_ease_in_out);
            lv_anim_start(&color_anim);
        }
        else {
            printf("Toggle button is OFF\n");

            // Animated color change to gray
            lv_anim_t color_anim;
            lv_anim_init(&color_anim);
            lv_anim_set_var(&color_anim, obj);
            lv_anim_set_values(&color_anim, 1000, 0);
            lv_anim_set_time(&color_anim, 300);
            lv_anim_set_exec_cb(&color_anim, btn_color_anim_cb);
            lv_anim_set_path_cb(&color_anim, lv_anim_path_ease_in_out);
            lv_anim_start(&color_anim);
        }
    }
}

static void toggle_button_2_event_handler(lv_event_t *e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *obj = lv_event_get_target(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        if (lv_obj_has_state(obj, LV_STATE_CHECKED)) {
            printf("Toggle button 2 is ON\n");

            // Animated color change to green
            lv_anim_t color_anim;
            lv_anim_init(&color_anim);
            lv_anim_set_var(&color_anim, obj);
            lv_anim_set_values(&color_anim, 0, 1000);
            lv_anim_set_time(&color_anim, 300);
            lv_anim_set_exec_cb(&color_anim, toggle_btn2_color_anim_cb);
            lv_anim_set_path_cb(&color_anim, lv_anim_path_ease_in_out);
            lv_anim_start(&color_anim);
        }
        else {
            printf("Toggle button 2 is OFF\n");

            // Animated color change to dark gray
            lv_anim_t color_anim;
            lv_anim_init(&color_anim);
            lv_anim_set_var(&color_anim, obj);
            lv_anim_set_values(&color_anim, 1000, 0);
            lv_anim_set_time(&color_anim, 300);
            lv_anim_set_exec_cb(&color_anim, toggle_btn2_color_anim_cb);
            lv_anim_set_path_cb(&color_anim, lv_anim_path_ease_in_out);
            lv_anim_start(&color_anim);
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

    // Title with Korean font (centered)
    lv_obj_t *title = lv_label_create(scr);
    lv_label_set_text(title, "LVGL 8.4 버튼 데모");
    lv_obj_set_pos(title, 10, 15);
    if (korean_font_20) {
        lv_obj_set_style_text_font(title, korean_font_20, LV_PART_MAIN);
    }
    lv_obj_set_style_text_color(title, lv_color_hex(0x000000), LV_PART_MAIN);

    // Button 1 (full width with image)
    lv_obj_t *btn1 = lv_btn_create(scr);
    lv_obj_set_pos(btn1, 20, 40);
    lv_obj_set_size(btn1, 280, 70);
    lv_obj_set_style_bg_color(btn1, lv_color_hex(0xFF9800), LV_PART_MAIN);
    lv_obj_set_style_border_width(btn1, 2, LV_PART_MAIN);
    lv_obj_set_style_border_color(btn1, lv_color_hex(0x333333), LV_PART_MAIN);
    lv_obj_add_event_cb(btn1, button_event_handler, LV_EVENT_CLICKED, NULL);

    // Set flex layout on button for image and label
    lv_obj_set_layout(btn1, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(btn1, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn1, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(btn1, 8, 0);
    lv_obj_set_style_pad_left(btn1, 8, 0);
    lv_obj_set_style_pad_right(btn1, 8, 0);

    // Image
    lv_obj_t *img1 = lv_img_create(btn1);
    lv_obj_set_flex_grow(img1, 0);
    // Load the image - it will size itself to the image's natural dimensions
    lv_img_set_src(img1, "A:assets/images/button_png.png");

    // Label
    lv_obj_t *label1 = lv_label_create(btn1);
    lv_label_set_text(label1, "PNG 버튼");
    if (korean_font_16) {
        lv_obj_set_style_text_font(label1, korean_font_16, LV_PART_MAIN);
    }
    lv_obj_set_style_text_color(label1, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_flex_grow(label1, 1);

    // Button 2 (full width with image)
    lv_obj_t *btn2 = lv_btn_create(scr);
    lv_obj_set_pos(btn2, 20, 125);
    lv_obj_set_size(btn2, 280, 70);
    lv_obj_set_style_bg_color(btn2, lv_color_hex(0xFF9800), LV_PART_MAIN);
    lv_obj_set_style_border_width(btn2, 2, LV_PART_MAIN);
    lv_obj_set_style_border_color(btn2, lv_color_hex(0x333333), LV_PART_MAIN);
    lv_obj_add_event_cb(btn2, button_event_handler, LV_EVENT_CLICKED, NULL);

    // Set flex layout on button for image and label
    lv_obj_set_layout(btn2, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(btn2, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn2, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(btn2, 8, 0);
    lv_obj_set_style_pad_left(btn2, 8, 0);
    lv_obj_set_style_pad_right(btn2, 8, 0);

    // Image
    lv_obj_t *img2 = lv_img_create(btn2);
    lv_obj_set_flex_grow(img2, 0);
    // Load the image - it will size itself to the image's natural dimensions
    lv_img_set_src(img2, "A:assets/images/button_bmp.bmp");

    // Label
    lv_obj_t *label2 = lv_label_create(btn2);
    lv_label_set_text(label2, "BMP 버튼");
    if (korean_font_16) {
        lv_obj_set_style_text_font(label2, korean_font_16, LV_PART_MAIN);
    }
    lv_obj_set_style_text_color(label2, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_flex_grow(label2, 1);

    // Button 3 (full width with image)
    lv_obj_t *btn3 = lv_btn_create(scr);
    lv_obj_set_pos(btn3, 20, 210);
    lv_obj_set_size(btn3, 280, 70);
    lv_obj_set_style_bg_color(btn3, lv_color_hex(0xFF9800), LV_PART_MAIN);
    lv_obj_set_style_border_width(btn3, 2, LV_PART_MAIN);
    lv_obj_set_style_border_color(btn3, lv_color_hex(0x333333), LV_PART_MAIN);
    lv_obj_add_event_cb(btn3, button_event_handler, LV_EVENT_CLICKED, NULL);

    // Set flex layout on button for image and label
    lv_obj_set_layout(btn3, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(btn3, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn3, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_row(btn3, 8, 0);
    lv_obj_set_style_pad_left(btn3, 8, 0);
    lv_obj_set_style_pad_right(btn3, 8, 0);

    // Image
    lv_obj_t *img3 = lv_img_create(btn3);
    lv_obj_set_flex_grow(img3, 0);
    // Load the image - it will size itself to the image's natural dimensions
    lv_img_set_src(img3, "A:assets/images/button_jpg.jpg");

    // Label
    lv_obj_t *label3 = lv_label_create(btn3);
    lv_label_set_text(label3, "JPG 버튼");
    if (korean_font_16) {
        lv_obj_set_style_text_font(label3, korean_font_16, LV_PART_MAIN);
    }
    lv_obj_set_style_text_color(label3, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_flex_grow(label3, 1);

    // Toggle Button 1 (full width)
    lv_obj_t *toggle_btn1 = lv_btn_create(scr);
    lv_obj_set_pos(toggle_btn1, 20, 295);
    lv_obj_set_size(toggle_btn1, 280, 70);
    lv_obj_set_style_bg_color(toggle_btn1, lv_color_hex(0x757575), LV_PART_MAIN);
    lv_obj_set_style_border_width(toggle_btn1, 2, LV_PART_MAIN);
    lv_obj_set_style_border_color(toggle_btn1, lv_color_hex(0x333333), LV_PART_MAIN);
    lv_obj_add_flag(toggle_btn1, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_add_event_cb(toggle_btn1, toggle_button_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t *toggle_label1 = lv_label_create(toggle_btn1);
    lv_label_set_text(toggle_label1, "토글 버튼 #1");
    lv_obj_center(toggle_label1);
    if (korean_font_16) {
        lv_obj_set_style_text_font(toggle_label1, korean_font_16, LV_PART_MAIN);
    }
    lv_obj_set_style_text_color(toggle_label1, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    // Toggle Button 2 (full width)
    lv_obj_t *toggle_btn2 = lv_btn_create(scr);
    lv_obj_set_pos(toggle_btn2, 20, 380);
    lv_obj_set_size(toggle_btn2, 280, 70);
    lv_obj_set_style_bg_color(toggle_btn2, lv_color_hex(0x424242), LV_PART_MAIN);
    lv_obj_set_style_border_width(toggle_btn2, 2, LV_PART_MAIN);
    lv_obj_set_style_border_color(toggle_btn2, lv_color_hex(0x333333), LV_PART_MAIN);
    lv_obj_add_flag(toggle_btn2, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_add_event_cb(toggle_btn2, toggle_button_2_event_handler, LV_EVENT_VALUE_CHANGED, NULL);

    lv_obj_t *toggle_label2 = lv_label_create(toggle_btn2);
    lv_label_set_text(toggle_label2, "토글 버튼 #2");
    lv_obj_center(toggle_label2);
    if (korean_font_16) {
        lv_obj_set_style_text_font(toggle_label2, korean_font_16, LV_PART_MAIN);
    }
    lv_obj_set_style_text_color(toggle_label2, lv_color_hex(0xFFFFFF), LV_PART_MAIN);

    // Disabled Button (full width)
    lv_obj_t *disabled_btn = lv_btn_create(scr);
    lv_obj_set_pos(disabled_btn, 20, 465);
    lv_obj_set_size(disabled_btn, 280, 70);
    lv_obj_set_style_bg_color(disabled_btn, lv_color_hex(0xBDBDBD), LV_PART_MAIN);
    lv_obj_set_style_border_width(disabled_btn, 2, LV_PART_MAIN);
    lv_obj_set_style_border_color(disabled_btn, lv_color_hex(0x999999), LV_PART_MAIN);
    lv_obj_add_state(disabled_btn, LV_STATE_DISABLED);

    lv_obj_t *disabled_label = lv_label_create(disabled_btn);
    lv_label_set_text(disabled_label, "비활성 버튼");
    lv_obj_center(disabled_label);
    if (korean_font_16) {
        lv_obj_set_style_text_font(disabled_label, korean_font_16, LV_PART_MAIN);
    }
    lv_obj_set_style_text_color(disabled_label, lv_color_hex(0x666666), LV_PART_MAIN);

    // Info Label with Korean text
    lv_obj_t *info = lv_label_create(scr);
    lv_label_set_text(info, "버튼을 클릭하여 시각적 피드백을 확인하세요!");
    lv_obj_set_pos(info, 10, 550);
    lv_obj_set_width(info, 300);
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

    // Initialize image decoders
    lv_png_init();
    lv_split_jpeg_init();

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
