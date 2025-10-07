/**
 * @file lv_conf.h
 * Configuration file for LVGL
 */

#ifndef LV_CONF_H
#define LV_CONF_H

#define LV_USE_LINUX            0
#define LV_USE_FREETYPE         1
#define LV_FREETYPE_CACHE_FT_GLYPH_CNT 2048

/* Memory settings */
#define LV_MEM_SIZE    (128U * 1024U)

/* Display settings */
#define LV_COLOR_DEPTH 32

/* Feature usage */
#define LV_USE_LOG              1

/* Driver usage */
#define LV_USE_SDL              1
#define LV_SDL_INCLUDE_PATH     <SDL2/SDL.h>

/* Font settings */
#define LV_FONT_MONTSERRAT_12   1
#define LV_FONT_MONTSERRAT_14   1
#define LV_FONT_MONTSERRAT_16   1
#define LV_FONT_MONTSERRAT_18   1
#define LV_FONT_MONTSERRAT_20   1
#define LV_FONT_MONTSERRAT_24   1
#define LV_FONT_DEFAULT         &lv_font_montserrat_14

/* Widget usage */
#define LV_USE_BTN              1
#define LV_USE_LABEL            1
#define LV_USE_TEXTAREA         1
#define LV_USE_KEYBOARD         1
#define LV_USE_BTNMATRIX        1
#define LV_USE_MSGBOX           1

#endif /* LV_CONF_H */
