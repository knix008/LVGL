#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// SCREEN CONFIGURATION
// ============================================================================

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 640
#define BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10)

#define TITLE_BAR_HEIGHT 60
#define STATUS_BAR_HEIGHT 50
#define BUTTON_WIDTH 60
#define BUTTON_HEIGHT 40

// ============================================================================
// COLOR CONFIGURATION
// ============================================================================

#define COLOR_BG_DARK 0x2A2A2A
#define COLOR_BG_TITLE 0x1A1A1A
#define COLOR_BUTTON_BG 0x1A1A1A
#define COLOR_BUTTON_BACK 0x444444
#define COLOR_BORDER 0x888888
#define COLOR_TEXT 0xFFFFFF
#define COLOR_TRANSPARENT 128

// ============================================================================
// APPLICATION CONFIGURATION
// ============================================================================

#define MAX_SCREENS 10
#define FONT_SIZE 16

// ============================================================================
// SCREEN IDS
// ============================================================================

enum {
    SCREEN_MAIN = 0,
    SCREEN_MENU = 1
};

#endif
