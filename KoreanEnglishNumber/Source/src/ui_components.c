#include "ui_components.h"
#include "tab_chunjiin.h"
#include "tab_english.h"
#include "tab_number.h"
#include "font_config.h"
#include "lv_freetype.h"
#include <stdio.h>

// Global Korean font variable
lv_font_t * korean_font = NULL;
lv_font_t * korean_font_small = NULL;

// Global variables for input mode switching
typedef enum {
    INPUT_MODE_CHUNJIIN,
    INPUT_MODE_ENGLISH,
    INPUT_MODE_NUMBER
} input_mode_t;

static input_mode_t current_input_mode = INPUT_MODE_CHUNJIIN;
static lv_obj_t * input_container = NULL;
static lv_obj_t * mode_button = NULL;

// Function to initialize FreeType and load Korean font
static void init_freetype_and_fonts(void) {
    if (korean_font == NULL) {
        printf("Loading Korean TrueType font with enhanced memory settings...\n");
        printf("FreeType cache size: %d glyphs, LVGL memory: %d KB, Cache: %d KB\n", 
               LV_FREETYPE_CACHE_FT_GLYPH_CNT, 
               LV_MEM_SIZE / 1024, 
               LV_CACHE_DEF_SIZE / 1024);
        
        const char* font_path = FONT_PATH(KOREAN_FONT_REGULAR);
        printf("Font path: %s\n", font_path);
        
        korean_font = lv_freetype_font_create(font_path, 
                                             KOREAN_FONT_RENDER_MODE, 
                                             DEFAULT_FONT_SIZE, 
                                             KOREAN_FONT_STYLE);
        
        if (korean_font != NULL) {
            printf("Korean TrueType font loaded successfully with enhanced memory allocation\n");
            printf("- Glyph cache: %d entries\n", LV_FREETYPE_CACHE_FT_GLYPH_CNT);
            printf("- Font size: 24px\n");
            printf("- Render mode: Bitmap\n");
        } else {
            printf("Failed to load Korean font, falling back to built-in font\n");
            korean_font = (lv_font_t*)&lv_font_source_han_sans_sc_16_cjk;
        }
        
        // Create smaller Korean font for compact UI elements
        korean_font_small = lv_freetype_font_create(font_path, 
                                                   KOREAN_FONT_RENDER_MODE, 
                                                   16,  // Smaller size
                                                   KOREAN_FONT_STYLE);
        if (korean_font_small != NULL) {
            printf("Small Korean font (16px) loaded successfully\n");
        } else {
            printf("Failed to load small Korean font, using regular font\n");
            korean_font_small = korean_font;
        }
    }
}

// Function to get the Korean font
lv_font_t * get_korean_font(void) {
    return korean_font;
}

// Function to get the small Korean font
lv_font_t * get_korean_font_small(void) {
    return korean_font_small;
}

// Function to switch input mode
static void switch_input_mode(void) {
    if (input_container == NULL) return;

    // Clear current input container
    lv_obj_clean(input_container);

    // Create appropriate input based on mode
    switch (current_input_mode) {
        case INPUT_MODE_CHUNJIIN:
            create_chunjiin_tab(input_container);
            lv_label_set_text(lv_obj_get_child(mode_button, 0), "KOR");
            printf("Switched to ChunJiIn mode\n");
            break;
        case INPUT_MODE_ENGLISH:
            create_english_tab(input_container);
            lv_label_set_text(lv_obj_get_child(mode_button, 0), "ENG");
            printf("Switched to English T9 mode\n");
            break;
        case INPUT_MODE_NUMBER:
            create_number_tab(input_container);
            lv_label_set_text(lv_obj_get_child(mode_button, 0), "123");
            printf("Switched to Number mode\n");
            break;
    }
}

// Mode switch button callback
static void mode_switch_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        // Cycle through modes: ChunJiIn -> English -> Number -> ChunJiIn
        current_input_mode = (current_input_mode + 1) % 3;
        switch_input_mode();
    }
}

// UI initialization with mode switching
void lv_example_tab_menu(void) {
    // Initialize FreeType and fonts first
    init_freetype_and_fonts();

    // Create a screen
    lv_obj_t * scr = lv_scr_act();

    // Create mode switch button at top
    mode_button = lv_btn_create(scr);
    lv_obj_set_size(mode_button, 80, 40);
    lv_obj_align(mode_button, LV_ALIGN_TOP_RIGHT, -5, 5);
    lv_obj_t * mode_label = lv_label_create(mode_button);
    lv_label_set_text(mode_label, "KOR");
    lv_obj_set_style_text_font(mode_label, &lv_font_montserrat_14, 0);  // Use built-in font for button
    lv_obj_center(mode_label);
    lv_obj_add_event_cb(mode_button, mode_switch_cb, LV_EVENT_CLICKED, NULL);

    // Create input container below the mode button
    input_container = lv_obj_create(scr);
    lv_obj_set_size(input_container, 310, 585);  // Adjusted for mode button
    lv_obj_align(input_container, LV_ALIGN_TOP_MID, 0, 50);  // Below mode button
    lv_obj_set_style_pad_all(input_container, 0, 0);
    lv_obj_set_style_border_width(input_container, 0, 0);

    // Initialize with ChunJiIn mode
    switch_input_mode();
}