#ifndef FONT_CONFIG_H
#define FONT_CONFIG_H

// Font configuration settings
// You can modify these paths as needed for your deployment

// Font directory options (uncomment the one you want to use):
// #define FONT_DIR ""                           // Current directory (default)
#define FONT_DIR "../assets/"              // Relative to assets folder
// #define FONT_DIR "/usr/share/fonts/korean/" // System font directory
// #define FONT_DIR "./fonts/"                // Local fonts subdirectory
// #define FONT_DIR "/opt/korean-input/fonts/" // Custom installation directory

// Font file names
#define KOREAN_FONT_REGULAR "NanumGothic-Regular.ttf"
#define KOREAN_FONT_BOLD    "NanumGothic-Bold.ttf"
#define KOREAN_FONT_EXTRA   "NanumGothic-ExtraBold.ttf"

// Font settings
#define DEFAULT_FONT_SIZE 24
#define KOREAN_FONT_RENDER_MODE LV_FREETYPE_FONT_RENDER_MODE_BITMAP
#define KOREAN_FONT_STYLE LV_FREETYPE_FONT_STYLE_NORMAL

// Helper macro to construct full font path
#define FONT_PATH(filename) FONT_DIR filename

#endif // FONT_CONFIG_H
