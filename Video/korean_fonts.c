#include "korean_fonts.h"
#include "lvgl/src/libs/freetype/lv_freetype.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Font objects
lv_font_t *nanum_gothic_16;
lv_font_t *nanum_gothic_bold_16;

// Font loading function using FreeType
void load_korean_fonts(void) {
    printf("Loading Korean fonts from TTF files...\n");
    
    // Initialize FreeType
    lv_freetype_init(LV_FREETYPE_CACHE_FT_GLYPH_CNT);
    
    // Load NanumGothicCoding Regular 16px
    nanum_gothic_16 = lv_freetype_font_create("font/NanumGothicCoding.ttf",
                                             LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                             16,
                                             LV_FREETYPE_FONT_STYLE_NORMAL);
    
    // Load NanumGothicCoding Bold 16px
    nanum_gothic_bold_16 = lv_freetype_font_create("font/NanumGothicCoding-Bold.ttf",
                                                   LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                                   16,
                                                   LV_FREETYPE_FONT_STYLE_NORMAL);
    
    // Check if fonts loaded successfully
    if (!nanum_gothic_16) {
        printf("Warning: Failed to load NanumGothicCoding.ttf, using fallback font\n");
        nanum_gothic_16 = (lv_font_t *)&lv_font_montserrat_14;
    }
    
    if (!nanum_gothic_bold_16) {
        printf("Warning: Failed to load NanumGothicCoding-Bold.ttf, using fallback font\n");
        nanum_gothic_bold_16 = (lv_font_t *)&lv_font_montserrat_14;
    }
    
    if (nanum_gothic_16 && nanum_gothic_bold_16) {
        printf("Korean fonts loaded successfully!\n");
    } else {
        printf("Some Korean fonts failed to load, using fallback fonts\n");
    }
}

void cleanup_korean_fonts(void) {
    printf("Cleaning up Korean fonts...\n");
    // FreeType cleanup is handled automatically by LVGL
}
