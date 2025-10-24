/**
 * @file image_button_app.c
 * @brief Image Button GUI Application Implementation
 * 
 * This file implements the image button demonstration application.
 * It creates a GUI with image buttons showcasing different image formats.
 */

#include "image_button_app.h"
#include "lvgl/src/libs/freetype/lv_freetype.h"
#include <stdio.h>
#include <stdlib.h>

// TrueType font object
static lv_font_t * custom_font = NULL;

// Global variables for UI elements
static lv_obj_t * main_screen;
static lv_obj_t * info_label;
static lv_obj_t * status_label;

// Button information structure
typedef struct {
    const char * name;
    const char * image_path;
    const char * description;
} button_info_t;

// Button information array (use POSIX filesystem driver with A: prefix, matching ImageButtonUI pattern)
static const button_info_t buttons[] = {
    {"PNG Button", "A:assets/images/button_png.png", "PNG format with transparency support"},
    {"JPG Button", "A:assets/images/button_jpg.jpg", "JPEG format with compression"},
    {"BMP Button", "A:assets/images/button_bmp.bmp", "Bitmap format without compression"}
};

/**
 * @brief Handle button click events
 */
void button_click_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {
        // Get the button that was clicked
        lv_obj_t * btn = lv_event_get_target(e);

        // Find which button was clicked by checking user data
        int button_index = (int)(intptr_t)lv_obj_get_user_data(btn);

        if(button_index >= 0 && button_index < 3) {  // Now we have 3 buttons
            // Get image size information
            lv_image_header_t img_header;
            lv_result_t res = lv_image_decoder_get_info(buttons[button_index].image_path, &img_header);

            if (res == LV_RESULT_OK) {
                // Update info label with button information including image size
                lv_label_set_text_fmt(info_label,
                    "Clicked: %s\n"
                    "Format: %s\n"
                    "Image Size: %dx%d pixels\n"
                    "Color Format: %d\n"
                    "Description: %s",
                    buttons[button_index].name,
                    buttons[button_index].image_path,
                    img_header.w, img_header.h,
                    img_header.cf,
                    buttons[button_index].description);
            } else {
                // Fallback if image info cannot be retrieved
                lv_label_set_text_fmt(info_label,
                    "Clicked: %s\n"
                    "Format: %s\n"
                    "Description: %s",
                    buttons[button_index].name,
                    buttons[button_index].image_path,
                    buttons[button_index].description);
            }

            // Update status label
            lv_label_set_text_fmt(status_label,
                "Last clicked: %s", buttons[button_index].name);
        }
    }
}

/**
 * @brief Create a button with an image and text (following ImageButtonUI pattern)
 */
lv_obj_t * create_image_button(lv_obj_t * parent, const char * image_path,
                               const char * text,
                               lv_coord_t x, lv_coord_t y,
                               lv_coord_t width, lv_coord_t height)
{
    // Create a regular button (not imagebutton)
    lv_obj_t * btn = lv_btn_create(parent);
    lv_obj_set_pos(btn, x, y);
    lv_obj_set_size(btn, width, height);

    printf("Creating button with image: %s\n", image_path);

    // Create an image widget inside the button
    lv_obj_t * img = lv_image_create(btn);
    lv_image_set_src(img, image_path);

    // Set a smaller size for the image (reduce from original 64x64 to 48x48)
    lv_obj_set_size(img, 48, 48);

    // Position image at the top of the button
    lv_obj_align(img, LV_ALIGN_TOP_MID, 0, 8);

    // Try to load the image to verify it exists
    lv_image_header_t img_header;
    lv_result_t res = lv_image_decoder_get_info(image_path, &img_header);
    if (res == LV_RESULT_OK) {
        printf("  Image loaded successfully: %dx%d, format: %d\n",
               img_header.w, img_header.h, img_header.cf);
    } else {
        printf("  WARNING: Failed to load image: %s (error: %d)\n", image_path, res);
    }

    // Create a label below the image
    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, text);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, -5);

    printf("Button created at (%d, %d) with size %dx%d\n", (int)x, (int)y, (int)width, (int)height);

    // Add click event handler
    lv_obj_add_event_cb(btn, button_click_handler, LV_EVENT_CLICKED, NULL);

    return btn;
}

/**
 * @brief Create a label to display button information
 */
lv_obj_t * create_info_label(lv_obj_t * parent, const char * text, 
                            lv_coord_t x, lv_coord_t y)
{
    lv_obj_t * label = lv_label_create(parent);
    lv_obj_set_pos(label, x, y);
    lv_label_set_text(label, text);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(label, 300);  // Full width for 320x640 portrait screen
    return label;
}

/**
 * @brief Initialize TrueType font (matching ImageButtonUI pattern)
 */
static void init_custom_font(void)
{
    // Try to initialize FreeType library (use the constant from lv_conf.h)
    lv_freetype_init(LV_FREETYPE_CACHE_FT_GLYPH_CNT);
    printf("FreeType initialized\n");

    // Create a larger Korean font for the title (24px, matching ImageButtonUI)
    // FreeType may use direct filesystem paths instead of LVGL's "A:" prefix
    custom_font = lv_freetype_font_create("assets/fonts/NanumGothicCoding-Bold.ttf",
                                         LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                         16,
                                         LV_FREETYPE_FONT_STYLE_NORMAL);

    if (custom_font == NULL) {
        printf("WARNING: Custom font loading failed, trying absolute path...\n");
        // Try with absolute path as fallback
        custom_font = lv_freetype_font_create("/home/shkwon/Projects/LVGL/ImageButton/assets/fonts/NanumGothicCoding-Bold.ttf",
                                             LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                             16,
                                             LV_FREETYPE_FONT_STYLE_NORMAL);
        if (custom_font == NULL) {
            printf("WARNING: Font loading failed with both relative and absolute paths\n");
        } else {
            printf("Custom font loaded successfully with absolute path (24px)\n");
        }
    } else {
        printf("Custom font loaded successfully: NanumGothicCoding-Bold.ttf (24px)\n");
    }
}

/**
 * @brief Initialize the image button application
 */
void image_button_app_init(void)
{
    // Initialize custom font
    init_custom_font();
    
    // Create main screen
    main_screen = lv_obj_create(NULL);

    // Set white background explicitly
    lv_obj_set_style_bg_color(main_screen, lv_color_white(), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(main_screen, LV_OPA_COVER, LV_PART_MAIN);

    // Remove any padding or scrollbars to ensure clean white background
    lv_obj_set_scrollbar_mode(main_screen, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(main_screen, LV_OBJ_FLAG_SCROLLABLE);
    
    // Create title label (only one label at the top)
    lv_obj_t * title_label = lv_label_create(main_screen);
    lv_label_set_text(title_label, "LVGL 이미지 버튼 데모");
    lv_obj_set_style_text_font(title_label, custom_font ? custom_font : &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(title_label, lv_color_black(), 0);
    lv_obj_align(title_label, LV_ALIGN_TOP_LEFT, 10, 10);
    
    // Create 3 image buttons in a horizontal row with text on them
    // Images are 64x64, so make buttons 100x100 to fit image + text
    const int button_width = 100;
    const int button_height = 100;
    const int spacing = 10;
    const int num_buttons = 3;
    const int total_width = num_buttons * button_width + (num_buttons - 1) * spacing;
    const int start_x = (320 - total_width) / 2;  // Center horizontally
    const int start_y = 80;   // Top margin (below title)

    for(int i = 0; i < num_buttons; i++) {
        int x = start_x + i * (button_width + spacing);
        int y = start_y;

        // Create the image button with text on it
        lv_obj_t * btn = create_image_button(main_screen, buttons[i].image_path,
                                           buttons[i].name,
                                           x, y, button_width, button_height);

        // Set user data to identify which button this is
        lv_obj_set_user_data(btn, (void*)(intptr_t)i);
    }
    
    // Create info label for displaying button information (bottom of screen)
    info_label = create_info_label(main_screen,
        "Click any button above to see information about the image format.\n\n"
        "This demo showcases:\n"
        "• PNG: Transparency support\n"
        "• JPG: Compression\n"
        "• BMP: Uncompressed bitmap",
        10, 400);
    lv_obj_set_style_text_color(info_label, lv_color_hex(0x333333), 0);
    lv_obj_set_style_bg_color(info_label, lv_color_hex(0xF0F0F0), 0);
    lv_obj_set_style_bg_opa(info_label, LV_OPA_COVER, 0);
    lv_obj_set_style_pad_all(info_label, 8, 0);
    lv_obj_set_style_radius(info_label, 5, 0);
    lv_obj_set_style_text_font(info_label, custom_font ? custom_font : &lv_font_montserrat_14, 0);
    
    // Create status label
    status_label = lv_label_create(main_screen);
    lv_label_set_text(status_label, "Ready - Click a button to start");
    lv_obj_set_style_text_color(status_label, lv_color_hex(0x006600), 0);
    lv_obj_set_style_text_font(status_label, custom_font ? custom_font : &lv_font_montserrat_14, 0);
    lv_obj_align(status_label, LV_ALIGN_BOTTOM_MID, 0, -10);
    
    // Load the screen
    lv_scr_load(main_screen);
    
    printf("Image Button Application initialized successfully!\n");
    printf("Available image formats: PNG, JPG, GIF, BMP\n");
    printf("Main screen created and loaded\n");
    printf("Screen size: %dx%d\n", lv_obj_get_width(main_screen), lv_obj_get_height(main_screen));
}
