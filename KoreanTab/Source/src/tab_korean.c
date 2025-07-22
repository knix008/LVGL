#include "tab_korean.h"
#include "lv_freetype.h"
#include <stdio.h>

// Simple Korean tab with basic FreeType font support
void create_korean_tab(lv_obj_t * parent) {
    printf("Creating Korean tab...\n");
    
    // Try to load a Korean font
    lv_font_t * korean_font = lv_freetype_font_create(
        "../assets/NanumGothic-Regular.ttf",
        LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
        16,
        LV_FREETYPE_FONT_STYLE_NORMAL
    );
    
    if (korean_font == NULL) {
        printf("Failed to load Korean font, using default font\n");
    } else {
        printf("Korean font loaded successfully\n");
    }
    
    // Simple header
    lv_obj_t * header = lv_label_create(parent);
    lv_label_set_text(header, "Korean Test");
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 10);
    
    // Korean text label
    lv_obj_t * korean_label = lv_label_create(parent);
    lv_label_set_text(korean_label, "안녕하세요");
    lv_obj_align(korean_label, LV_ALIGN_CENTER, 0, -30);
    lv_obj_set_width(korean_label, 200);
    
    if (korean_font != NULL) {
        lv_obj_set_style_text_font(korean_label, korean_font, 0);
    }
    
    // English text label
    lv_obj_t * english_label = lv_label_create(parent);
    lv_label_set_text(english_label, "Hello World");
    lv_obj_align(english_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_width(english_label, 200);
    
    // Mixed text label
    lv_obj_t * mixed_label = lv_label_create(parent);
    lv_label_set_text(mixed_label, "한국어 Korean");
    lv_obj_align(mixed_label, LV_ALIGN_CENTER, 0, 30);
    lv_obj_set_width(mixed_label, 200);
    
    if (korean_font != NULL) {
        lv_obj_set_style_text_font(mixed_label, korean_font, 0);
    }
    
    // More Korean text
    lv_obj_t * korean2_label = lv_label_create(parent);
    lv_label_set_text(korean2_label, "가나다라마바사");
    lv_obj_align(korean2_label, LV_ALIGN_CENTER, 0, 60);
    lv_obj_set_width(korean2_label, 200);
    
    if (korean_font != NULL) {
        lv_obj_set_style_text_font(korean2_label, korean_font, 0);
    }
    
    // Korean greeting
    lv_obj_t * greeting_label = lv_label_create(parent);
    lv_label_set_text(greeting_label, "반갑습니다");
    lv_obj_align(greeting_label, LV_ALIGN_CENTER, 0, 90);
    lv_obj_set_width(greeting_label, 200);
    
    if (korean_font != NULL) {
        lv_obj_set_style_text_font(greeting_label, korean_font, 0);
    }
    
    printf("Korean tab created\n");
} 