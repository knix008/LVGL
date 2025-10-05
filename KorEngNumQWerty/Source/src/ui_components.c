#include "ui_components.h"
#include "qwerty.h"
#include "ui_callbacks.h"
#include "font_config.h"
#include "lv_freetype.h"
#include <stdio.h>

// Global Korean font variable
lv_font_t * korean_font = NULL;
lv_font_t * korean_font_small = NULL;

// Tab event callback implementation
void tab_event_cb(lv_event_t * e) {
    // Empty callback for now - can be extended later
    (void)e;  // Suppress unused parameter warning
}

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

// UI initialization and setup with tab menu
void lv_example_tab_menu(void) {
    // Initialize FreeType and fonts first
    init_freetype_and_fonts();
    
    // Create a screen
    lv_obj_t * scr = lv_scr_act();
    
    // Create tabview - larger for 1024x768 screen
    lv_obj_t * tabview = lv_tabview_create(scr);
    lv_obj_set_size(tabview, 1000, 730);  // Larger tabview for bigger screen
    lv_obj_align(tabview, LV_ALIGN_TOP_MID, 0, 10);

    // Tab bar size
    lv_tabview_set_tab_bar_size(tabview, 40);
    
    // Add event callback for tab changes
    lv_obj_add_event_cb(tabview, tab_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Create tabs
    lv_obj_t * tab = lv_tabview_add_tab(tabview, "QWERTY"); // Added QWERTY tab

    // Create tab contents using separate functions
    create_qwerty_tab(tab);
}