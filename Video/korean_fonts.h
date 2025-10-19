#ifndef KOREAN_FONTS_H
#define KOREAN_FONTS_H

#include "lvgl.h"

// Korean font declarations
extern lv_font_t *nanum_gothic_16;
extern lv_font_t *nanum_gothic_bold_16;

// Font loading functions
void load_korean_fonts(void);
void cleanup_korean_fonts(void);

#endif // KOREAN_FONTS_H
