#ifndef FONT_CONFIG_H
#define FONT_CONFIG_H

#define FONT_PATH(x) "../assets/" x
#define KOREAN_FONT_REGULAR "NanumGothic-Bold.ttf"
#define KOREAN_FONT_RENDER_MODE LV_FREETYPE_FONT_RENDER_MODE_BITMAP
#define DEFAULT_FONT_SIZE 20
#define KOREAN_FONT_STYLE LV_FREETYPE_FONT_STYLE_NORMAL

// Don't redefine LV_FREETYPE_CACHE_FT_GLYPH_CNT if already defined in lv_conf.h
// If you need to change it, modify Source/lvgl/lv_conf.h instead

#endif
