/*
 * Chunjiin Korean Input Method - MiniGUI Version
 * Main application file
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "chunjiin.h"

#define IDC_TEXT_AREA      100
#define IDC_MODE_BUTTON    101
#define IDC_PUNCT_BUTTON   102
#define IDC_ENTER_BUTTON   103
#define IDC_KEY_0          110
#define IDC_KEY_1          111
#define IDC_KEY_2          112
#define IDC_KEY_3          113
#define IDC_KEY_4          114
#define IDC_KEY_5          115
#define IDC_KEY_6          116
#define IDC_KEY_7          117
#define IDC_KEY_8          118
#define IDC_KEY_9          119
#define IDC_KEY_10         120
#define IDC_KEY_11         121

static ChunjiinState chunjiin_state;
static HWND hTextArea;
static HWND hButtons[12];  // Store button handles for updating labels
static HWND hModeButton, hPunctButton, hEnterButton;
static PLOGFONT korean_font = NULL;
static PLOGFONT korean_font_bold = NULL;

// Punctuation cycling
static const wchar_t *punctuation_marks[] = {L".", L",", L"?"};
static int current_punct_index = 0;
static int punct_position = -1;  // Position in text buffer where punctuation is being edited (-1 = none)

// Korean font initialization
static int init_korean_font(void) {
    korean_font = CreateLogFont(FONT_TYPE_NAME_SCALE_TTF, "SansSerif", 
                               "UTF-8", FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN,
                               FONT_FLIP_NIL, FONT_OTHER_NIL, FONT_UNDERLINE_NONE,
                               FONT_STRUCKOUT_NONE, 16, 0);
    
    if (!korean_font) {
        // Fallback to system font
        korean_font = GetSystemFont(SYSLOGFONT_DEFAULT);
        return 0;
    }
    
    // Create bold font for buttons
    korean_font_bold = CreateLogFont(FONT_TYPE_NAME_SCALE_TTF, "SansSerif", 
                               "UTF-8", FONT_WEIGHT_BOLD, FONT_SLANT_ROMAN,
                               FONT_FLIP_NIL, FONT_OTHER_NIL, FONT_UNDERLINE_NONE,
                               FONT_STRUCKOUT_NONE, 16, 0);
    
    if (!korean_font_bold) {
        // Fallback to regular font if bold not available
        korean_font_bold = korean_font;
    }
    
    return 1;
}

// Convert wchar_t to UTF-8
static void wchar_to_utf8(const wchar_t *wstr, char *utf8_str, size_t max_len) {
    if (wstr == NULL || utf8_str == NULL) return;
    
    size_t i = 0, j = 0;
    while (wstr[i] != 0 && j < max_len - 4) {
        uint32_t uc = (uint32_t)wstr[i];
        
        if (uc < 0x80) {
            utf8_str[j++] = (char)uc;
        } else if (uc < 0x800) {
            utf8_str[j++] = (char)(0xC0 | (uc >> 6));
            utf8_str[j++] = (char)(0x80 | (uc & 0x3F));
        } else if (uc < 0x10000) {
            utf8_str[j++] = (char)(0xE0 | (uc >> 12));
            utf8_str[j++] = (char)(0x80 | ((uc >> 6) & 0x3F));
            utf8_str[j++] = (char)(0x80 | (uc & 0x3F));
        } else {
            utf8_str[j++] = (char)(0xF0 | (uc >> 18));
            utf8_str[j++] = (char)(0x80 | ((uc >> 12) & 0x3F));
            utf8_str[j++] = (char)(0x80 | ((uc >> 6) & 0x3F));
            utf8_str[j++] = (char)(0x80 | (uc & 0x3F));
        }
        i++;
    }
    utf8_str[j] = '\0';
}

// Update display text
static void update_display(void) {
    char utf8_text[MAX_TEXT_LEN * 4];
    wchar_to_utf8(chunjiin_state.text_buffer, utf8_text, sizeof(utf8_text));
    SetWindowText(hTextArea, utf8_text);
}

// Update button labels based on current mode
static void update_button_labels(void) {
    for (int i = 0; i < 12; i++) {
        if (hButtons[i] != HWND_INVALID) {
            const wchar_t *wtext = get_button_text(chunjiin_state.now_mode, i);
            char utf8_text[256];
            wchar_to_utf8(wtext, utf8_text, sizeof(utf8_text));
            SetWindowText(hButtons[i], utf8_text);
            // Apply bold Korean font to buttons
            if (korean_font_bold) {
                SetWindowFont(hButtons[i], korean_font_bold);
            }
        }
    }
}

// Key button click handler
static void handle_key_press(int key) {
    chunjiin_process_input(&chunjiin_state, key);
    // Don't reset punct_position - let it naturally move with text length
    update_display();
}

// Button click handler
static LRESULT ButtonClickProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case MSG_COMMAND:
            switch (LOWORD(wParam)) {
                case IDC_KEY_0: handle_key_press(0); break;
                case IDC_KEY_1: handle_key_press(1); break;
                case IDC_KEY_2: handle_key_press(2); break;
                case IDC_KEY_3: handle_key_press(3); break;
                case IDC_KEY_4: handle_key_press(4); break;
                case IDC_KEY_5: handle_key_press(5); break;
                case IDC_KEY_6: handle_key_press(6); break;
                case IDC_KEY_7: handle_key_press(7); break;
                case IDC_KEY_8: handle_key_press(8); break;
                case IDC_KEY_9: handle_key_press(9); break;
                case IDC_KEY_10: handle_key_press(10); break;
                case IDC_KEY_11: handle_key_press(11); break;
                
                case IDC_MODE_BUTTON:
                    change_mode(&chunjiin_state);
                    // Don't reset punct_position - continue from current position
                    update_display();
                    update_button_labels();  // Update button labels after mode change
                    break;

                case IDC_PUNCT_BUTTON: {
                    size_t current_len = wcslen(chunjiin_state.text_buffer);

                    if (punct_position == -1) {
                        // First click - add first punctuation mark at cursor position
                        if (current_len < MAX_TEXT_LEN - 1) {
                            int cursor = chunjiin_state.cursor_pos;
                            punct_position = cursor;
                            current_punct_index = 0;

                            // Insert at cursor position
                            chunjiin_state.text_buffer[cursor] = punctuation_marks[current_punct_index][0];
                            chunjiin_state.text_buffer[cursor + 1] = L'\0';
                            chunjiin_state.cursor_pos = cursor + 1;  // Move cursor after punctuation
                            update_display();
                        }
                    } else if (punct_position == (int)current_len - 1) {
                        // Subsequent clicks on same position - cycle to next punctuation
                        current_punct_index = (current_punct_index + 1) % 3;
                        chunjiin_state.text_buffer[punct_position] = punctuation_marks[current_punct_index][0];
                        update_display();
                    } else {
                        // Clicked after other input - add new punctuation at cursor
                        if (current_len < MAX_TEXT_LEN - 1) {
                            int cursor = chunjiin_state.cursor_pos;
                            punct_position = cursor;
                            current_punct_index = 0;

                            // Insert at cursor position
                            chunjiin_state.text_buffer[cursor] = punctuation_marks[current_punct_index][0];
                            chunjiin_state.text_buffer[cursor + 1] = L'\0';
                            chunjiin_state.cursor_pos = cursor + 1;  // Move cursor after punctuation
                            update_display();
                        }
                    }
                    break;
                }

                case IDC_ENTER_BUTTON: {
                    // Show input result in popup window, then clear
                    char utf8_text[MAX_TEXT_LEN * 4];
                    wchar_to_utf8(chunjiin_state.text_buffer, utf8_text, sizeof(utf8_text));

                    // Show popup with the current text
                    if (strlen(utf8_text) > 0) {
                        MessageBox(hWnd, utf8_text, "입력 결과 (Input Result)", MB_OK | MB_ICONINFORMATION);
                    } else {
                        MessageBox(hWnd, "입력된 텍스트가 없습니다 (No input text)", "입력 결과 (Input Result)", MB_OK | MB_ICONINFORMATION);
                    }

                    // Clear the text after showing popup
                    chunjiin_clear_preserve_mode(&chunjiin_state);
                    update_display();
                    break;
                }
            }
            break;
    }
    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

// Main window procedure
static LRESULT ChunjiinWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case MSG_CREATE: {
            // Create text area to fit window width with auto-wrap
            // Window is 280px wide, use ~260px width with margins
            // Use CTRL_MLEDIT (Multi-Line Edit) for proper multi-line support with scrollbars
            // ES_AUTOWRAP: automatically wraps text to next line when reaching edge
            // ES_AUTOVSCROLL: shows vertical scrollbar when content exceeds height
            // ES_AUTOHSCROLL: shows horizontal scrollbar when needed
            hTextArea = CreateWindow(CTRL_MLEDIT, "",
                                   WS_VISIBLE | WS_BORDER | WS_VSCROLL | WS_HSCROLL |
                                   ES_READONLY | ES_AUTOWRAP,
                                   IDC_TEXT_AREA,
                                   10, 10, 260, 100, hWnd, 0);
            
            // Apply bold Korean font to text area
            if (korean_font_bold) {
                SetWindowFont(hTextArea, korean_font_bold);
            }
            
            // Initialize button handles
            for (int i = 0; i < 12; i++) {
                hButtons[i] = HWND_INVALID;
            }
            
            // Button positions mapping: [button_id][col, row]
            // Row 0: 천(1), 지(2), 인(3)
            // Row 1: ㄱ(4), ㄴ(5), ㄷ(6)
            // Row 2: ㅂ(7), ㅅ(8), ㅈ(9)
            // Row 3: 공백(10), ㅇㅁ(0), 삭제(11)
            // Row 4: 모드, 지우기, 엔터
            int positions[12][2] = {
                {1, 3}, // 0: Row 3, Col 1 (ㅇㅁ)
                {0, 0}, {1, 0}, {2, 0}, // 1-3: Row 0 (천, 지, 인)
                {0, 1}, {1, 1}, {2, 1}, // 4-6: Row 1 (ㄱ, ㄴ, ㄷ)
                {0, 2}, {1, 2}, {2, 2}, // 7-9: Row 2 (ㅂ, ㅅ, ㅈ)
                {0, 3}, {2, 3}  // 10-11: Row 3 (공백, 삭제)
            };
            
            int key_ids[] = {
                IDC_KEY_0, IDC_KEY_1, IDC_KEY_2, IDC_KEY_3, IDC_KEY_4, IDC_KEY_5,
                IDC_KEY_6, IDC_KEY_7, IDC_KEY_8, IDC_KEY_9, IDC_KEY_10, IDC_KEY_11
            };
            
            // Center-aligned button layout
            // Window width: 280px, Button width: 70px, 3 columns
            // Total buttons width: 3 * 70 = 210px
            // Spacing between buttons: (280 - 210) / 4 = 17.5px ≈ 18px
            // Starting X: 18px, spacing: 88px (70 + 18)
            int button_start_x = 18;
            int button_spacing_x = 88;
            int button_start_y = 130;
            int button_spacing_y = 50;
            
            // Create keypad buttons (0-11) with center alignment
            for (int i = 0; i < 12; i++) {
                const wchar_t *wtext = get_button_text(chunjiin_state.now_mode, i);
                char button_text[256];
                wchar_to_utf8(wtext, button_text, sizeof(button_text));
                
                hButtons[i] = CreateWindow(CTRL_BUTTON, button_text,
                           WS_VISIBLE | BS_PUSHBUTTON,
                           key_ids[i],
                           button_start_x + positions[i][0] * button_spacing_x, 
                           button_start_y + positions[i][1] * button_spacing_y, 
                           70, 40, hWnd, 0);
                
                // Apply bold Korean font to button
                if (korean_font_bold && hButtons[i] != HWND_INVALID) {
                    SetWindowFont(hButtons[i], korean_font_bold);
                }
            }
            
            // Create control buttons in row 4 (center-aligned)
            hModeButton = CreateWindow(CTRL_BUTTON, "Mode",
                       WS_VISIBLE | BS_PUSHBUTTON,
                       IDC_MODE_BUTTON,
                       button_start_x + 0 * button_spacing_x,
                       button_start_y + 4 * button_spacing_y,
                       70, 40, hWnd, 0);
            if (korean_font_bold && hModeButton != HWND_INVALID) {
                SetWindowFont(hModeButton, korean_font_bold);
            }

            hPunctButton = CreateWindow(CTRL_BUTTON, ".,?",
                       WS_VISIBLE | BS_PUSHBUTTON,
                       IDC_PUNCT_BUTTON,
                       button_start_x + 1 * button_spacing_x,
                       button_start_y + 4 * button_spacing_y,
                       70, 40, hWnd, 0);
            if (korean_font_bold && hPunctButton != HWND_INVALID) {
                SetWindowFont(hPunctButton, korean_font_bold);
            }
            
            hEnterButton = CreateWindow(CTRL_BUTTON, "Enter",
                       WS_VISIBLE | BS_PUSHBUTTON,
                       IDC_ENTER_BUTTON,
                       button_start_x + 2 * button_spacing_x,
                       button_start_y + 4 * button_spacing_y,
                       70, 40, hWnd, 0);
            if (korean_font_bold && hEnterButton != HWND_INVALID) {
                SetWindowFont(hEnterButton, korean_font_bold);
            }
            
            // Initialize chunjiin state
            chunjiin_init(&chunjiin_state);
            update_display();
            break;
        }
        
        case MSG_COMMAND:
            return ButtonClickProc(hWnd, message, wParam, lParam);
        
        case MSG_CLOSE:
            DestroyMainWindow(hWnd);
            PostQuitMessage(hWnd);
            return 0;
        
        case MSG_DESTROY:
            if (korean_font && korean_font != GetSystemFont(SYSLOGFONT_DEFAULT)) {
                DestroyLogFont(korean_font);
            }
            if (korean_font_bold && korean_font_bold != korean_font) {
                DestroyLogFont(korean_font_bold);
            }
            return 0;
    }
    
    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}
// Application entry point
int MiniGUIMain(int argc, const char* argv[]) {
    (void)argc;
    (void)argv;
    
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;
    
    // Set locale for Korean support
    setlocale(LC_ALL, "ko_KR.UTF-8");
    
    // Initialize Korean font
    init_korean_font();

    // Calculate centered window position
    int screen_width = GetGDCapability(HDC_SCREEN, GDCAP_HPIXEL);
    int screen_height = GetGDCapability(HDC_SCREEN, GDCAP_VPIXEL);
    int window_width = 280;
    int window_height = 420;
    int window_x = (screen_width - window_width) / 2;
    int window_y = (screen_height - window_height) / 2;

    // Create main window (non-resizable, non-minimizable, center-aligned)
    CreateInfo.dwStyle = WS_VISIBLE | WS_CAPTION | WS_SYSMENU;
    CreateInfo.dwExStyle = WS_EX_NONE;
    CreateInfo.spCaption = "천지인 한글 입력기 (Chunjiin Korean Input)";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(0);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = ChunjiinWinProc;
    CreateInfo.lx = window_x;
    CreateInfo.ty = window_y;
    CreateInfo.rx = window_x + window_width;
    CreateInfo.by = window_y + window_height;
    CreateInfo.iBkColor = COLOR_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;
    
    hMainWnd = CreateMainWindow(&CreateInfo);
    
    if (hMainWnd == HWND_INVALID) {
        printf("Failed to create main window!\n");
        return -1;
    }
    
    printf("Main window created successfully\n");
    ShowWindow(hMainWnd, SW_SHOWNORMAL);
    printf("Window shown\n");
    
    // Main message loop
    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }
    
    MainWindowThreadCleanup(hMainWnd);
    return 0;
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif
