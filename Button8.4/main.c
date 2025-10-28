#include "lvgl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <SDL2/SDL.h>
#include "lvgl/src/extra/libs/freetype/lv_freetype.h"

// Display Configuration
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

// Global variables for the number input system
static lv_obj_t *display_label;
static lv_obj_t *keypad;
static char input_buffer[32] = {0};
static int buffer_index = 0;

// Function prototypes
static void button_matrix_cb(lv_event_t *e);
static void create_number_input_ui(void);
static void init_korean_fonts(void);
static void disp_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);
static void indev_read(lv_indev_drv_t *drv, lv_indev_data_t *data);

/**
 * Close popup dialog callback
 */
static void close_popup_cb(lv_event_t *e)
{
    lv_obj_t *btn = lv_event_get_target(e);
    lv_obj_t *popup = lv_obj_get_parent(btn);
    lv_obj_del(popup);
}

/**
 * Button matrix callback function - handles all button presses
 */
static void button_matrix_cb(lv_event_t *e)
{
    lv_obj_t *btnm = lv_event_get_target(e);
    uint16_t btn_id = lv_btnmatrix_get_selected_btn(btnm);
    const char *text = lv_btnmatrix_get_btn_text(btnm, btn_id);
    
    // Handle different button types based on text
    if (strcmp(text, "C") == 0 || strcmp(text, "지우기") == 0) {
        // Clear button
        buffer_index = 0;
        input_buffer[0] = '\0';
        lv_label_set_text(display_label, "0");
        printf("Clear button pressed\n"); // Debug output
    }
    else if (strcmp(text, "←") == 0) {
        // Backspace button
        if (buffer_index > 0) {
            buffer_index--;
            input_buffer[buffer_index] = '\0';
            if (buffer_index == 0) {
                lv_label_set_text(display_label, "0");
            } else {
                lv_label_set_text(display_label, input_buffer);
            }
            printf("Backspace pressed, buffer: %s\n", input_buffer); // Debug output
        }
    }
    else if (strcmp(text, "Enter") == 0 || strcmp(text, "입력") == 0) {
        // Enter button
        if (buffer_index > 0) {
            //printf("Entered number: %s\n", input_buffer);
            
            // Create custom popup dialog with Korean font support
            lv_obj_t *mbox = lv_obj_create(lv_scr_act());
            lv_obj_set_size(mbox, 250, 150);
            lv_obj_center(mbox);
            lv_obj_set_style_bg_color(mbox, lv_color_hex(0xFFFFFF), 0);
            lv_obj_set_style_border_width(mbox, 2, 0);
            lv_obj_set_style_border_color(mbox, lv_color_hex(0x000000), 0);
            lv_obj_set_style_radius(mbox, 10, 0);
            
            // Create title label
            lv_obj_t *title_label = lv_label_create(mbox);
            lv_label_set_text(title_label, "숫자 입력");
            lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 10);
            if (korean_font_16) {
                lv_obj_set_style_text_font(title_label, korean_font_16, LV_PART_MAIN);
            }
            
            // Create content label
            lv_obj_t *content_label = lv_label_create(mbox);
            lv_label_set_text(content_label, input_buffer);
            lv_obj_align(content_label, LV_ALIGN_CENTER, 0, 0);
            if (korean_font_16) {
                lv_obj_set_style_text_font(content_label, korean_font_16, LV_PART_MAIN);
            }
            
            // Create close button
            lv_obj_t *close_btn = lv_btn_create(mbox);
            lv_obj_set_size(close_btn, 80, 30);
            lv_obj_align(close_btn, LV_ALIGN_BOTTOM_MID, 0, -10);
            lv_obj_t *close_label = lv_label_create(close_btn);
            lv_label_set_text(close_label, "닫기");
            lv_obj_align(close_label, LV_ALIGN_CENTER, 0, 0);
            if (korean_font_16) {
                lv_obj_set_style_text_font(close_label, korean_font_16, LV_PART_MAIN);
            }
            
            // Add close button event
            lv_obj_add_event_cb(close_btn, close_popup_cb, LV_EVENT_CLICKED, NULL);
            
            // Clear the input buffer and display
            buffer_index = 0;
            input_buffer[0] = '\0';
            lv_label_set_text(display_label, "0");
        }
    }
    else {
        // Number buttons (0-9)
        if (buffer_index < (int)sizeof(input_buffer) - 1) {
            input_buffer[buffer_index] = text[0];
            buffer_index++;
            input_buffer[buffer_index] = '\0';
            lv_label_set_text(display_label, input_buffer);
            //printf("Number input: %s\n", input_buffer); // Debug output
        }
    }
}

/**
 * Initialize Korean fonts
 */
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
    info_20.name = "fonts/NanumGothicCoding-Bold.ttf";
    info_20.weight = 20;
    info_20.style = FT_FONT_STYLE_NORMAL;

    // Load NanumGothicCoding-Bold font at size 20 (for title)
    if (!lv_ft_font_init(&info_20)) {
        fprintf(stderr, "Failed to load Korean font at size 20!\n");
    } else {
        korean_font_20 = info_20.font;
        printf("Korean font (size 20) loaded successfully!\n");
    }

    // Create font info structure for size 16
    static lv_ft_info_t info_16 = {0};
    info_16.name = "fonts/NanumGothicCoding-Bold.ttf";
    info_16.weight = 16;
    info_16.style = FT_FONT_STYLE_NORMAL;

    // Load NanumGothicCoding-Bold font at size 16 (for buttons and text)
    if (!lv_ft_font_init(&info_16)) {
        fprintf(stderr, "Failed to load Korean font at size 16!\n");
    } else {
        korean_font_16 = info_16.font;
        printf("Korean font (size 16) loaded successfully!\n");
    }
}

/**
 * Create the number input UI
 */
static void create_number_input_ui(void)
{
    // Create main container
    lv_obj_t *main_container = lv_obj_create(lv_scr_act());
    lv_obj_set_size(main_container, 320, 640);
    lv_obj_set_style_pad_all(main_container, 10, 0);

    // Create display area
    lv_obj_t *display_container = lv_obj_create(main_container);
    lv_obj_set_size(display_container, 300, 80);
    lv_obj_align(display_container, LV_ALIGN_TOP_MID, 0, 20);

    // Create display label
    display_label = lv_label_create(display_container);
    lv_label_set_text(display_label, "0");
    lv_obj_set_style_text_align(display_label, LV_TEXT_ALIGN_LEFT, 0);
    lv_obj_align(display_label, LV_ALIGN_TOP_LEFT, 0, 0);

    // Create keypad
    keypad = lv_btnmatrix_create(main_container);
    lv_obj_set_size(keypad, 300, 400);
    lv_obj_align(keypad, LV_ALIGN_BOTTOM_MID, 0, -20);

    // Define keypad map
    static const char *btnm_map[] = {
        "1", "2", "3", "\n",
        "4", "5", "6", "\n", 
        "7", "8", "9", "\n",
        "지우기", "0", "←", "\n",
        "입력", ""
    };
    
    lv_btnmatrix_set_map(keypad, btnm_map);

    // Apply Korean font to button matrix
    if (korean_font_16) {
        lv_obj_set_style_text_font(keypad, korean_font_16, LV_PART_ITEMS);
    }

    // Set custom button colors
    lv_obj_set_style_bg_color(keypad, lv_color_hex(0x1E90FF), LV_PART_ITEMS); // Dodger blue default
    lv_obj_set_style_bg_color(keypad, lv_color_hex(0xFFB366), LV_PART_ITEMS | LV_STATE_PRESSED); // Light orange when pressed
    lv_obj_set_style_bg_color(keypad, lv_color_hex(0x1E90FF), LV_PART_ITEMS | LV_STATE_FOCUSED); // Dodger blue when focused
    lv_obj_set_style_bg_color(keypad, lv_color_hex(0xFFB366), LV_PART_ITEMS | LV_STATE_FOCUSED | LV_STATE_PRESSED); // Light orange when focused and pressed
    
    // Set button text color to white
    lv_obj_set_style_text_color(keypad, lv_color_hex(0xFFFFFF), LV_PART_ITEMS);

    // Add event callback
    lv_obj_add_event_cb(keypad, button_matrix_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // Create title label
    lv_obj_t *title_label = lv_label_create(main_container);
    lv_label_set_text(title_label, "숫자 입력 시스템");
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, -5);
    if (korean_font_20) {
        lv_obj_set_style_text_font(title_label, korean_font_20, LV_PART_MAIN);
    }
}

/**
 * Display flush callback
 */
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

/**
 * Input device callback
 */
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

/**
 * Main function
 */
int main(void)
{
    // Initialize SDL2
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "Failed to initialize SDL2: %s\n", SDL_GetError());
        return 1;
    }

    // Create window
    window = SDL_CreateWindow(
        "Number Input System",
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

    // Create the number input UI
    create_number_input_ui();

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
