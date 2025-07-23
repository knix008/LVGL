#include "ui_components.h"
#include "tab_db.h"
#include "tab_settings.h"
#include "tab_info.h"
#include "tab_number.h"
#include "tab_korean.h"
#include "tab_chunjiin.h"
#include "ui_callbacks.h"
#include "font_config.h"
#include "lv_freetype.h"
#include <stdio.h>

// Global Korean font variable
lv_font_t * korean_font = NULL;
lv_font_t * korean_font_small = NULL;

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
    
    // Create tabview (크기 원래대로)
    lv_obj_t * tabview = lv_tabview_create(scr);
    lv_obj_set_size(tabview, 300, 400);  // 전체 탭뷰 크기 유지
    lv_obj_center(tabview);
    
    // 탭 바(탭 버튼 영역) 높이만 줄임
    lv_tabview_set_tab_bar_size(tabview, 32); // 예시: 32픽셀로 설정
    
    // Add event callback for tab changes
    lv_obj_add_event_cb(tabview, tab_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Create tabs
    lv_obj_t * tab1 = lv_tabview_add_tab(tabview, "DB");
    lv_obj_t * tab2 = lv_tabview_add_tab(tabview, "Settings");
    lv_obj_t * tab3 = lv_tabview_add_tab(tabview, "Info");
    lv_obj_t * tab4 = lv_tabview_add_tab(tabview, "NUM"); // Changed from Keypad to Number
    lv_obj_t * tab5 = lv_tabview_add_tab(tabview, "KOR"); // Added new tab
    lv_obj_t * tab6 = lv_tabview_add_tab(tabview, "ChungJiIn"); // Added ChunJiIn tab
    
    // Create tab contents using separate functions
    create_db_tab(tab1);
    create_settings_tab(tab2);
    create_info_tab(tab3);
    create_number_tab(tab4);
    create_korean_tab(tab5);
    create_chunjiin_tab(tab6);
}