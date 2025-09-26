/**
 * @file chunjiin_app.c
 * @brief ChunJiIn Korean Input System - Complete Application
 * @version 1.0.0
 * @date 2024
 * 
 * This file implements the complete ChunJiIn Korean input system with MiniGUI.
 * It includes the ChunJiIn input system, keyboard layout, and main application
 * all in one file for simplicity.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <signal.h>
#include <unistd.h>
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include "chunjiin_input.h"

// ============================================================================
// APPLICATION CONSTANTS
// ============================================================================

#define CHUNJIIN_APP_WIDTH     600
#define CHUNJIIN_APP_HEIGHT    500
#define CHUNJIIN_APP_TITLE     "ChunJiIn Korean Input System"

// Keyboard layout constants
#define CHUNJIIN_KEY_SIZE          40
#define CHUNJIIN_KEY_SPACING       5
#define CHUNJIIN_ROW_SPACING       10

// Control IDs
#define IDC_CHUNJIIN_TEXT         1001
#define IDC_CHUNJIIN_DOT          2001    // ㆍ (천 - sky/heaven)
#define IDC_CHUNJIIN_EU           2002    // ㅡ (지 - earth)
#define IDC_CHUNJIIN_I            2003    // ㅣ (인 - human)
#define IDC_CHUNJIIN_G            2010    // ㄱ, ㅋ, ㄲ
#define IDC_CHUNJIIN_N            2011    // ㄴ, ㄹ
#define IDC_CHUNJIIN_D            2012    // ㄷ, ㅌ, ㄸ
#define IDC_CHUNJIIN_B            2013    // ㅂ, ㅍ, ㅃ
#define IDC_CHUNJIIN_S            2014    // ㅅ, ㅎ, ㅆ
#define IDC_CHUNJIIN_J            2015    // ㅈ, ㅊ, ㅉ
#define IDC_CHUNJIIN_M            2016    // ㅇ, ㅁ
#define IDC_CHUNJIIN_SPACE        2020    // Space
#define IDC_CHUNJIIN_BACKSPACE    2021    // Backspace
#define IDC_CHUNJIIN_ENTER        2022    // Enter
#define IDC_CHUNJIIN_CLEAR        2023    // Clear all

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

static HWND hMainWnd = HWND_INVALID;
static HWND hTextBox = HWND_INVALID;
static HWND hKeyButtons[14] = {0};
static PLOGFONT korean_font = NULL;
static wchar_t display_buffer[1024] = {0};
static BOOL app_running = FALSE;

// ============================================================================
// SIGNAL HANDLERS
// ============================================================================

void chunjiin_signal_handler(int sig) {
    printf("Received signal %d, cleaning up ChunJiIn application...\n", sig);
    
    app_running = FALSE;
    
    // Clean up Korean font
    if (korean_font) {
        DestroyLogFont(korean_font);
        korean_font = NULL;
    }
    
    // Clean up buttons
    for (int i = 0; i < 14; i++) {
        if (hKeyButtons[i]) {
            DestroyWindow(hKeyButtons[i]);
            hKeyButtons[i] = HWND_INVALID;
        }
    }
    
    // Clean up text box
    if (hTextBox) {
        DestroyWindow(hTextBox);
        hTextBox = HWND_INVALID;
    }
    
    // Post quit message to exit main loop
    if (hMainWnd != HWND_INVALID) {
        PostMessage(hMainWnd, MSG_CLOSE, 0, 0);
    }
}

// ============================================================================
// KEYBOARD FUNCTIONS
// ============================================================================

void chunjiin_handle_dot(void) {
    //printf("ChunJiIn Dot (ㆍ) input\n");
    process_input('a'); // 'a' maps to dot in ChunJiIn system
}

void chunjiin_handle_eu(void) {
    //printf("ChunJiIn Eu (ㅡ) input\n");
    process_input('e'); // 'e' maps to eu in ChunJiIn system
}

void chunjiin_handle_i(void) {
    //printf("ChunJiIn I (ㅣ) input\n");
    process_input('i'); // 'i' maps to i in ChunJiIn system
}

void chunjiin_handle_consonant_group(int group_id) {
    if (group_id < 0 || group_id >= 7) return;
    
    // Map group ID to ChunJiIn input keys
    char input_key = 0;
    switch (group_id) {
        case 0: input_key = 'g'; break; // ㄱ, ㅋ, ㄲ
        case 1: input_key = 'n'; break; // ㄴ, ㄹ
        case 2: input_key = 'd'; break; // ㄷ, ㅌ, ㄸ
        case 3: input_key = 'b'; break; // ㅂ, ㅍ, ㅃ
        case 4: input_key = 's'; break; // ㅅ, ㅎ, ㅆ
        case 5: input_key = 'j'; break; // ㅈ, ㅊ, ㅉ
        case 6: input_key = 'm'; break; // ㅇ, ㅁ
    }
    
    if (input_key != 0) {
        //printf("ChunJiIn consonant group %d input (%c)\n", group_id, input_key);
        process_input(input_key);
    }
}

void chunjiin_handle_space(void) {
    //printf("ChunJiIn Space input\n");
    process_input(' ');
}

void chunjiin_handle_backspace(void) {
    //printf("ChunJiIn Backspace input\n");
    process_input('<');
}

void chunjiin_handle_enter(void) {
    //printf("ChunJiIn Enter input\n");
    chunjiin_enter_key_handler();
}

void chunjiin_handle_clear(void) {
    //printf("ChunJiIn Clear input\n");
    process_input('.');
}

// Forward declaration
void chunjiin_keyboard_update_display(void);

void chunjiin_keyboard_handle_key(int key_id) {
    switch (key_id) {
        // ChunJiIn fundamental elements
        case IDC_CHUNJIIN_DOT:
            chunjiin_handle_dot();
            break;
        case IDC_CHUNJIIN_EU:
            chunjiin_handle_eu();
            break;
        case IDC_CHUNJIIN_I:
            chunjiin_handle_i();
            break;
            
        // Consonant groups
        case IDC_CHUNJIIN_G:
            chunjiin_handle_consonant_group(0);
            break;
        case IDC_CHUNJIIN_N:
            chunjiin_handle_consonant_group(1);
            break;
        case IDC_CHUNJIIN_D:
            chunjiin_handle_consonant_group(2);
            break;
        case IDC_CHUNJIIN_B:
            chunjiin_handle_consonant_group(3);
            break;
        case IDC_CHUNJIIN_S:
            chunjiin_handle_consonant_group(4);
            break;
        case IDC_CHUNJIIN_J:
            chunjiin_handle_consonant_group(5);
            break;
        case IDC_CHUNJIIN_M:
            chunjiin_handle_consonant_group(6);
            break;
            
        // Special keys
        case IDC_CHUNJIIN_SPACE:
            chunjiin_handle_space();
            break;
        case IDC_CHUNJIIN_BACKSPACE:
            chunjiin_handle_backspace();
            break;
        case IDC_CHUNJIIN_ENTER:
            chunjiin_handle_enter();
            break;
        case IDC_CHUNJIIN_CLEAR:
            chunjiin_handle_clear();
            break;
    }
    
    // Update display after handling key
    chunjiin_keyboard_update_display();
}

void chunjiin_keyboard_update_display(void) {
    if (!hTextBox) return;
    
    // Get current text from ChunJiIn input system
    chunjiin_get_current_text(display_buffer);
    
    // Convert to UTF-8 for display
    char utf8_buffer[4096];
    size_t len = wcslen(display_buffer);
    size_t utf8_len = 0;
    
    for (size_t i = 0; i < len && utf8_len < sizeof(utf8_buffer) - 1; i++) {
        char utf8_char[8];
        int bytes = wchar_to_utf8(display_buffer[i], utf8_char, sizeof(utf8_char));
        
        if (bytes > 0 && utf8_len + bytes < sizeof(utf8_buffer)) {
            memcpy(utf8_buffer + utf8_len, utf8_char, bytes);
            utf8_len += bytes;
        }
    }
    
    utf8_buffer[utf8_len] = '\0';
    printf("utf8_buffer: %s\n", utf8_buffer);
    
    if (utf8_len > 0) {
        SetWindowText(hTextBox, utf8_buffer);
        InvalidateRect(hTextBox, NULL, TRUE);
        UpdateWindow(hTextBox, TRUE);
    }
}

PLOGFONT chunjiin_load_korean_font(void) {
    PLOGFONT font = NULL;
    
    // First try using CreateLogFontByName with system font names (as shown in your example)
    const char* font_names[] = {
        "NanumGothic",
        "Malgun Gothic", 
        "Dotum",
        "Gulim",
        "Batang",
        NULL
    };
    
    for (int i = 0; font_names[i] != NULL; i++) {
        font = CreateLogFontByName(font_names[i]);
        if (font != NULL) {
            printf("Loaded Korean font: %s\n", font_names[i]);
            break;
        }
    }
    
    // If system fonts fail, try loading from assets directory
    if (font == NULL) {
        font = CreateLogFont("ttf", "NanumGothic-Regular", "UTF-8", 
                          FONT_WEIGHT_NORMAL, FONT_SLANT_ROMAN, FONT_FLIP_NONE,
                          FONT_OTHER_NONE, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                          16, 0);
        
        if (font == NULL) {
            // Try Bold variant
            font = CreateLogFont("ttf", "NanumGothic-Bold", "UTF-8", 
                                FONT_WEIGHT_NORMAL, FONT_SLANT_ROMAN, FONT_FLIP_NONE,
                                FONT_OTHER_NONE, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                                16, 0);
        }
        
        if (font == NULL) {
            // Try ExtraBold variant
            font = CreateLogFont("ttf", "NanumGothic-ExtraBold", "UTF-8", 
                                FONT_WEIGHT_NORMAL, FONT_SLANT_ROMAN, FONT_FLIP_NONE,
                                FONT_OTHER_NONE, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                                16, 0);
        }
        
        if (font) {
            printf("Loaded Korean font from assets directory\n");
        }
    }
    
    if (font == NULL) {
        printf("Warning: Could not load Korean font, Korean characters may not display correctly\n");
    }
    
    return font;
}

// ============================================================================
// MAIN WINDOW PROCEDURE
// ============================================================================

static LRESULT ChunJiInAppWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    HDC hdc;
    
    switch (message) {
        case MSG_CREATE:
            printf("Creating ChunJiIn application window...\n");
            
            // Initialize ChunJiIn input system
            initialize();
            
            // Load Korean font
            korean_font = chunjiin_load_korean_font();
            
            // Create text display area
            hTextBox = CreateWindow("static", "ChunJiIn Korean Input:",
                WS_VISIBLE | WS_CHILD | WS_BORDER,
                IDC_CHUNJIIN_TEXT,
                20, 20, CHUNJIIN_APP_WIDTH - 40, 80,
                hWnd, 0);
            
            // Apply Korean font to text box
            if (korean_font) {
                SetWindowFont(hTextBox, korean_font);
            }
            
            // Calculate center positions for each row
            int window_width = 400;  // Approximate window width
            int button_width = CHUNJIIN_KEY_SIZE * 2;
            int button_spacing = 10;
            
            // Row 1: ChunJiIn fundamental elements (인천지) - 3 buttons
            int y_pos = 120;
            int x_offset = 100;
            int row1_start = (window_width - (3 * button_width + 2 * button_spacing)) / 2 + x_offset;
            hKeyButtons[0] = CreateWindow("button", "ㅣ", 
                WS_VISIBLE | WS_CHILD, IDC_CHUNJIIN_I, row1_start, y_pos, button_width, CHUNJIIN_KEY_SIZE, hWnd, 0);
            hKeyButtons[1] = CreateWindow("button", "ㆍ", 
                WS_VISIBLE | WS_CHILD, IDC_CHUNJIIN_DOT, row1_start + button_width + button_spacing, y_pos, button_width, CHUNJIIN_KEY_SIZE, hWnd, 0);
            hKeyButtons[2] = CreateWindow("button", "ㅡ", 
                WS_VISIBLE | WS_CHILD, IDC_CHUNJIIN_EU, row1_start + 2 * (button_width + button_spacing), y_pos, button_width, CHUNJIIN_KEY_SIZE, hWnd, 0);
            
            // Row 2: Basic consonants with cycling - 3 buttons
            y_pos += CHUNJIIN_KEY_SIZE + CHUNJIIN_ROW_SPACING;
            int row2_start = (window_width - (3 * button_width + 2 * button_spacing)) / 2 + x_offset;
            hKeyButtons[3] = CreateWindow("button", "ㄱ,ㅋ", 
                WS_VISIBLE | WS_CHILD, IDC_CHUNJIIN_G, row2_start, y_pos, button_width, CHUNJIIN_KEY_SIZE, hWnd, 0);
            hKeyButtons[4] = CreateWindow("button", "ㄴ,ㄹ", 
                WS_VISIBLE | WS_CHILD, IDC_CHUNJIIN_N, row2_start + button_width + button_spacing, y_pos, button_width, CHUNJIIN_KEY_SIZE, hWnd, 0);
            hKeyButtons[5] = CreateWindow("button", "ㄷ,ㅌ", 
                WS_VISIBLE | WS_CHILD, IDC_CHUNJIIN_D, row2_start + 2 * (button_width + button_spacing), y_pos, button_width, CHUNJIIN_KEY_SIZE, hWnd, 0);
            
            // Row 3: Additional consonants with cycling - 3 buttons
            y_pos += CHUNJIIN_KEY_SIZE + CHUNJIIN_ROW_SPACING;
            int row3_start = (window_width - (3 * button_width + 2 * button_spacing)) / 2 + x_offset;
            hKeyButtons[6] = CreateWindow("button", "ㅂ,ㅍ", 
                WS_VISIBLE | WS_CHILD, IDC_CHUNJIIN_B, row3_start, y_pos, button_width, CHUNJIIN_KEY_SIZE, hWnd, 0);
            hKeyButtons[7] = CreateWindow("button", "ㅅ,ㅎ", 
                WS_VISIBLE | WS_CHILD, IDC_CHUNJIIN_S, row3_start + button_width + button_spacing, y_pos, button_width, CHUNJIIN_KEY_SIZE, hWnd, 0);
            hKeyButtons[8] = CreateWindow("button", "ㅈ,ㅊ", 
                WS_VISIBLE | WS_CHILD, IDC_CHUNJIIN_J, row3_start + 2 * (button_width + button_spacing), y_pos, button_width, CHUNJIIN_KEY_SIZE, hWnd, 0);
            
            // Row 4: Special keys with cycling - 3 buttons
            y_pos += CHUNJIIN_KEY_SIZE + CHUNJIIN_ROW_SPACING;
            int row4_start = (window_width - (3 * button_width + 2 * button_spacing)) / 2 + x_offset;
            hKeyButtons[9] = CreateWindow("button", "Space", 
                WS_VISIBLE | WS_CHILD, IDC_CHUNJIIN_SPACE, row4_start, y_pos, button_width, CHUNJIIN_KEY_SIZE, hWnd, 0);
            hKeyButtons[10] = CreateWindow("button", "ㅇ,ㅁ", 
                WS_VISIBLE | WS_CHILD, IDC_CHUNJIIN_M, row4_start + button_width + button_spacing, y_pos, button_width, CHUNJIIN_KEY_SIZE, hWnd, 0);
            hKeyButtons[11] = CreateWindow("button", "Back", 
                WS_VISIBLE | WS_CHILD, IDC_CHUNJIIN_BACKSPACE, row4_start + 2 * (button_width + button_spacing), y_pos, button_width, CHUNJIIN_KEY_SIZE, hWnd, 0);
            
            // Row 5: Enter and Clear buttons - 2 buttons
            y_pos += CHUNJIIN_KEY_SIZE + CHUNJIIN_ROW_SPACING;
            int row5_start = (window_width - (2 * button_width + 1 * button_spacing)) / 2 + x_offset;
            hKeyButtons[12] = CreateWindow("button", "Enter", 
                WS_VISIBLE | WS_CHILD, IDC_CHUNJIIN_ENTER, row5_start, y_pos, button_width, CHUNJIIN_KEY_SIZE, hWnd, 0);
            hKeyButtons[13] = CreateWindow("button", "Clear", 
                WS_VISIBLE | WS_CHILD, IDC_CHUNJIIN_CLEAR, row5_start + button_width + button_spacing, y_pos, button_width, CHUNJIIN_KEY_SIZE, hWnd, 0);
            
            // Apply Korean font to all buttons
            if (korean_font) {
                for (int i = 0; i < 14; i++) {
                    if (hKeyButtons[i]) {
                        SetWindowFont(hKeyButtons[i], korean_font);
                    }
                }
            }
            
            printf("ChunJiIn application created successfully\n");
            return 0;
            
        case MSG_COMMAND:
            chunjiin_keyboard_handle_key(LOWORD(wParam));
            return 0;
            
        case MSG_PAINT:
            hdc = BeginPaint(hWnd);
            SetBkMode(hdc, BM_TRANSPARENT);
            
            // Display title
            TextOut(hdc, 20, 5, "ChunJiIn Korean Input System");
            
            // Display current text
            if (display_buffer[0] != L'\0') {
                char utf8_output[1024];
                size_t len = wcslen(display_buffer);
                size_t utf8_len = 0;
                
                for (size_t i = 0; i < len && utf8_len < sizeof(utf8_output) - 1; i++) {
                    char utf8_char[8];
                    int bytes = wchar_to_utf8(display_buffer[i], utf8_char, sizeof(utf8_char));
                    
                    if (bytes > 0 && utf8_len + bytes < sizeof(utf8_output)) {
                        memcpy(utf8_output + utf8_len, utf8_char, bytes);
                        utf8_len += bytes;
                    }
                }
                
                utf8_output[utf8_len] = '\0';
                
                if (korean_font) {
                    PLOGFONT old_font = SelectFont(hdc, korean_font);
                    TextOut(hdc, 20, 100, utf8_output);
                    if (old_font) SelectFont(hdc, old_font);
                } else {
                    TextOut(hdc, 20, 100, utf8_output);
                }
            }
            
            EndPaint(hWnd, hdc);
            return 0;
            
        case MSG_KEYDOWN:
            // Handle keyboard shortcuts
            switch (wParam) {
                case SCANCODE_ESCAPE:
                case SCANCODE_Q:
                    PostMessage(hWnd, MSG_CLOSE, 0, 0);
                    break;
                case SCANCODE_F1:
                    // Show help
                    MessageBox(hWnd, 
                        "ChunJiIn Korean Input System\n\n"
                        "Fundamental Elements:\n"
                        "ㆍ (천) - Sky/Heaven\n"
                        "ㅡ (지) - Earth\n"
                        "ㅣ (인) - Human\n\n"
                        "Use these three elements to create all Korean characters.\n"
                        "Press Escape or Q to quit.",
                        "ChunJiIn Help", 
                        MB_OK | MB_ICONINFORMATION);
                    break;
            }
            return 0;
            
        case MSG_CLOSE:
            printf("Closing ChunJiIn application...\n");
            
            // Clean up Korean font
            if (korean_font) {
                DestroyLogFont(korean_font);
                korean_font = NULL;
            }
            
            // Clean up buttons
            for (int i = 0; i < 14; i++) {
                if (hKeyButtons[i]) {
                    DestroyWindow(hKeyButtons[i]);
                    hKeyButtons[i] = HWND_INVALID;
                }
            }
            
            // Clean up text box
            if (hTextBox) {
                DestroyWindow(hTextBox);
                hTextBox = HWND_INVALID;
            }
            
            // Clean up main window
            DestroyMainWindow(hWnd);
            PostQuitMessage(hWnd);
            return 0;
            
        case MSG_DESTROY:
            return 0;
    }
    
    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

// ============================================================================
// APPLICATION INITIALIZATION
// ============================================================================

static void InitCreateInfo(MAINWINCREATE* pCreateInfo) {
    pCreateInfo->dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION;
    pCreateInfo->dwExStyle = WS_EX_NONE;
    pCreateInfo->spCaption = CHUNJIIN_APP_TITLE;
    pCreateInfo->hMenu = 0;
    pCreateInfo->hCursor = GetSystemCursor(0);
    pCreateInfo->hIcon = 0;
    pCreateInfo->MainWindowProc = ChunJiInAppWinProc;
    pCreateInfo->lx = 100;
    pCreateInfo->ty = 100;
    pCreateInfo->rx = pCreateInfo->lx + CHUNJIIN_APP_WIDTH;
    pCreateInfo->by = pCreateInfo->ty + CHUNJIIN_APP_HEIGHT;
    pCreateInfo->iBkColor = COLOR_lightwhite;
    pCreateInfo->dwAddData = 0;
    pCreateInfo->hHosting = HWND_DESKTOP;
}

// ============================================================================
// MAIN APPLICATION ENTRY POINT
// ============================================================================

int ChunJiInMain(int argc, const char* argv[]) {
    MAINWINCREATE CreateInfo;
    MSG msg;
    
    printf("Starting ChunJiIn Korean Input System...\n");
    
    // Set up locale for Korean support
    setlocale(LC_ALL, "ko_KR.UTF-8");
    
    // Register signal handlers for cleanup
    signal(SIGINT, chunjiin_signal_handler);
    signal(SIGTERM, chunjiin_signal_handler);
    signal(SIGSEGV, chunjiin_signal_handler);
    
    // Initialize application state
    app_running = TRUE;
    
    // Create main window
    InitCreateInfo(&CreateInfo);
    hMainWnd = CreateMainWindow(&CreateInfo);
    
    if (hMainWnd == HWND_INVALID) {
        printf("Failed to create ChunJiIn main window\n");
        return -1;
    }
    
    // Load and set Korean font for main window (following your example)
    korean_font = chunjiin_load_korean_font();
    if (korean_font) {
        SetWindowFont(hMainWnd, korean_font);
    }
    
    // Show main window
    ShowWindow(hMainWnd, SW_SHOWNORMAL);
    
    printf("ChunJiIn application started successfully\n");
    printf("Press F1 for help, Escape or Q to quit\n");
    
    // Main message loop
    while (app_running && GetMessage(&msg, hMainWnd)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    printf("ChunJiIn application exiting...\n");
    
    return 0;
}

// ============================================================================
// MINIGUI ENTRY POINT
// ============================================================================

int MiniGUIMain(int argc, const char* argv[]) {
    return ChunJiInMain(argc, argv);
}

#ifndef _MGRM_PROCESSES
#include <minigui/dti.c>
#endif