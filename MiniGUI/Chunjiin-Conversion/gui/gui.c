/*
 * GUI Module for Chunjiin Korean Input Method
 * Handles all window management, controls, and UI rendering
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

#include "gui.h"
#include "file_dialog.h"
#include "../input/input.h"
#include "../input/file_io.h"

#define IDC_TEXT_AREA      100
#define IDC_MODE_BUTTON    101
#define IDC_PUNCT_BUTTON   102
#define IDC_ENTER_BUTTON   103
#define IDC_SAVE_BUTTON    104
#define IDC_LOAD_BUTTON    105
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

// Global GUI state
static HWND hTextArea;
static HWND hButtons[12];
static HWND hModeButton, hPunctButton, hEnterButton, hSaveButton, hLoadButton;
static PLOGFONT korean_font = NULL;
static PLOGFONT korean_font_bold = NULL;

// Punctuation cycling state
static const wchar_t *punctuation_marks[] = {L".", L",", L"?"};
static int current_punct_index = 0;
static int punct_position = -1;

// Reference to global state (set by main)
static ChunjiinState *g_state = NULL;

// Helper: Convert wchar_t to UTF-8
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

int gui_init_fonts(void) {
    korean_font = CreateLogFont(FONT_TYPE_NAME_SCALE_TTF, "SansSerif",
                               "UTF-8", FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN,
                               FONT_FLIP_NIL, FONT_OTHER_NIL, FONT_UNDERLINE_NONE,
                               FONT_STRUCKOUT_NONE, 16, 0);

    if (!korean_font) {
        korean_font = GetSystemFont(SYSLOGFONT_DEFAULT);
        return 0;
    }

    korean_font_bold = CreateLogFont(FONT_TYPE_NAME_SCALE_TTF, "SansSerif",
                               "UTF-8", FONT_WEIGHT_BOLD, FONT_SLANT_ROMAN,
                               FONT_FLIP_NIL, FONT_OTHER_NIL, FONT_UNDERLINE_NONE,
                               FONT_STRUCKOUT_NONE, 16, 0);

    if (!korean_font_bold) {
        korean_font_bold = korean_font;
    }

    return 1;
}

void gui_cleanup_fonts(void) {
    if (korean_font && korean_font != GetSystemFont(SYSLOGFONT_DEFAULT)) {
        DestroyLogFont(korean_font);
    }
    if (korean_font_bold && korean_font_bold != korean_font) {
        DestroyLogFont(korean_font_bold);
    }
}

void gui_update_display(ChunjiinState *state) {
    if (!hTextArea) return;

    char utf8_text[MAX_TEXT_LEN * 4];
    wchar_to_utf8(state->text_buffer, utf8_text, sizeof(utf8_text));
    SetWindowText(hTextArea, utf8_text);
}

void gui_update_button_labels(ChunjiinState *state) {
    for (int i = 0; i < 12; i++) {
        if (hButtons[i] != HWND_INVALID) {
            const wchar_t *wtext = input_get_button_text(state->now_mode, i);
            char utf8_text[256];
            wchar_to_utf8(wtext, utf8_text, sizeof(utf8_text));
            SetWindowText(hButtons[i], utf8_text);
            if (korean_font_bold) {
                SetWindowFont(hButtons[i], korean_font_bold);
            }
        }
    }
}

void gui_handle_key_press(ChunjiinState *state, int key) {
    input_process(state, key);
    gui_update_display(state);
}

void gui_handle_mode_change(ChunjiinState *state) {
    input_change_mode(state);
    gui_update_display(state);
    gui_update_button_labels(state);
}

void gui_handle_punctuation(ChunjiinState *state) {
    size_t current_len = wcslen(state->text_buffer);

    if (punct_position == -1) {
        // First click - add first punctuation mark at cursor position
        if (current_len < MAX_TEXT_LEN - 1) {
            int cursor = state->cursor_pos;
            punct_position = cursor;
            current_punct_index = 0;

            state->text_buffer[cursor] = punctuation_marks[current_punct_index][0];
            state->text_buffer[cursor + 1] = L'\0';
            state->cursor_pos = cursor + 1;
            gui_update_display(state);
        }
    } else if (punct_position == (int)current_len - 1) {
        // Subsequent clicks on same position - cycle to next punctuation
        current_punct_index = (current_punct_index + 1) % 3;
        state->text_buffer[punct_position] = punctuation_marks[current_punct_index][0];
        gui_update_display(state);
    } else {
        // Clicked after other input - add new punctuation at cursor
        if (current_len < MAX_TEXT_LEN - 1) {
            int cursor = state->cursor_pos;
            punct_position = cursor;
            current_punct_index = 0;

            state->text_buffer[cursor] = punctuation_marks[current_punct_index][0];
            state->text_buffer[cursor + 1] = L'\0';
            state->cursor_pos = cursor + 1;
            gui_update_display(state);
        }
    }
}

void gui_handle_enter(ChunjiinState *state, HWND hWnd) {
    char utf8_text[MAX_TEXT_LEN * 4];
    wchar_to_utf8(state->text_buffer, utf8_text, sizeof(utf8_text));

    if (strlen(utf8_text) > 0) {
        MessageBox(hWnd, utf8_text, "입력 결과 (Input Result)", MB_OK | MB_ICONINFORMATION);
    } else {
        MessageBox(hWnd, "입력된 텍스트가 없습니다 (No input text)", "입력 결과 (Input Result)", MB_OK | MB_ICONINFORMATION);
    }

    input_clear_preserve_mode(state);
    gui_update_display(state);
}

void gui_handle_save(ChunjiinState *state, HWND hWnd) {
    if (file_save_input(state, "/tmp/chunjiin_input.txt")) {
        MessageBox(hWnd, "/tmp/chunjiin_input.txt에 저장되었습니다 (Saved to /tmp/chunjiin_input.txt)",
                  "저장 완료 (Save Complete)", MB_OK | MB_ICONINFORMATION);
    } else {
        MessageBox(hWnd, "저장에 실패했습니다 (Save failed)",
                  "오류 (Error)", MB_OK | MB_ICONSTOP);
    }
}

void gui_handle_load(ChunjiinState *state, HWND hWnd) {
    if (file_load_input(state, "/tmp/chunjiin_input.txt")) {
        gui_update_display(state);
        MessageBox(hWnd, "/tmp/chunjiin_input.txt에서 로드되었습니다 (Loaded from /tmp/chunjiin_input.txt)",
                  "로드 완료 (Load Complete)", MB_OK | MB_ICONINFORMATION);
    } else {
        MessageBox(hWnd, "로드에 실패했습니다 (Load failed)",
                  "오류 (Error)", MB_OK | MB_ICONSTOP);
    }
}

void gui_handle_save_dialog(ChunjiinState *state, HWND hWnd) {
    if (gui_dialog_save(state, hWnd)) {
        gui_update_display(state);
    }
}

void gui_handle_load_dialog(ChunjiinState *state, HWND hWnd) {
    if (gui_dialog_load(state, hWnd)) {
        gui_update_display(state);
    }
}

// Button click handler
static LRESULT ButtonClickProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    if (!g_state) return DefaultMainWinProc(hWnd, message, wParam, lParam);

    switch (message) {
        case MSG_COMMAND:
            switch (LOWORD(wParam)) {
                case IDC_KEY_0: gui_handle_key_press(g_state, 0); break;
                case IDC_KEY_1: gui_handle_key_press(g_state, 1); break;
                case IDC_KEY_2: gui_handle_key_press(g_state, 2); break;
                case IDC_KEY_3: gui_handle_key_press(g_state, 3); break;
                case IDC_KEY_4: gui_handle_key_press(g_state, 4); break;
                case IDC_KEY_5: gui_handle_key_press(g_state, 5); break;
                case IDC_KEY_6: gui_handle_key_press(g_state, 6); break;
                case IDC_KEY_7: gui_handle_key_press(g_state, 7); break;
                case IDC_KEY_8: gui_handle_key_press(g_state, 8); break;
                case IDC_KEY_9: gui_handle_key_press(g_state, 9); break;
                case IDC_KEY_10: gui_handle_key_press(g_state, 10); break;
                case IDC_KEY_11: gui_handle_key_press(g_state, 11); break;

                case IDC_MODE_BUTTON:
                    gui_handle_mode_change(g_state);
                    break;

                case IDC_PUNCT_BUTTON:
                    gui_handle_punctuation(g_state);
                    break;

                case IDC_ENTER_BUTTON:
                    gui_handle_enter(g_state, hWnd);
                    break;

                case IDC_SAVE_BUTTON:
                    gui_handle_save_dialog(g_state, hWnd);
                    break;

                case IDC_LOAD_BUTTON:
                    gui_handle_load_dialog(g_state, hWnd);
                    break;
            }
            break;
    }
    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

// Main window procedure
static LRESULT ChunjiinWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case MSG_CREATE: {
            // Create text area using STATIC control for multi-line display (MLEDIT crashes on ARM64)
            hTextArea = CreateWindowEx2(CTRL_STATIC, "",
                                   WS_VISIBLE | WS_BORDER | SS_LEFT,
                                   0,
                                   IDC_TEXT_AREA,
                                   10, 10, 260, 50, hWnd, NULL, NULL, 0);

            if (korean_font_bold) {
                SetWindowFont(hTextArea, korean_font_bold);
            }

            // Initialize button handles
            for (int i = 0; i < 12; i++) {
                hButtons[i] = HWND_INVALID;
            }

            // Button positions mapping
            int positions[12][2] = {
                {1, 3}, // 0: Row 3, Col 1
                {0, 0}, {1, 0}, {2, 0}, // 1-3: Row 0
                {0, 1}, {1, 1}, {2, 1}, // 4-6: Row 1
                {0, 2}, {1, 2}, {2, 2}, // 7-9: Row 2
                {0, 3}, {2, 3}  // 10-11: Row 3
            };

            int key_ids[] = {
                IDC_KEY_0, IDC_KEY_1, IDC_KEY_2, IDC_KEY_3, IDC_KEY_4, IDC_KEY_5,
                IDC_KEY_6, IDC_KEY_7, IDC_KEY_8, IDC_KEY_9, IDC_KEY_10, IDC_KEY_11
            };

            int button_start_x = 18;
            int button_spacing_x = 88;
            int button_start_y = 70;
            int button_spacing_y = 50;

            // Create keypad buttons
            for (int i = 0; i < 12; i++) {
                const wchar_t *wtext = input_get_button_text(g_state->now_mode, i);
                char button_text[256];
                wchar_to_utf8(wtext, button_text, sizeof(button_text));

                hButtons[i] = CreateWindowEx2(CTRL_BUTTON, button_text,
                           WS_VISIBLE | BS_PUSHBUTTON,
                           0,
                           key_ids[i],
                           button_start_x + positions[i][0] * button_spacing_x,
                           button_start_y + positions[i][1] * button_spacing_y,
                           70, 40, hWnd, NULL, NULL, 0);

                if (korean_font_bold && hButtons[i] != HWND_INVALID) {
                    SetWindowFont(hButtons[i], korean_font_bold);
                }
            }

            // Create control buttons
            hModeButton = CreateWindowEx2(CTRL_BUTTON, "Mode",
                       WS_VISIBLE | BS_PUSHBUTTON,
                       0,
                       IDC_MODE_BUTTON,
                       button_start_x + 0 * button_spacing_x,
                       button_start_y + 4 * button_spacing_y,
                       70, 40, hWnd, NULL, NULL, 0);
            if (korean_font_bold && hModeButton != HWND_INVALID) {
                SetWindowFont(hModeButton, korean_font_bold);
            }

            hPunctButton = CreateWindowEx2(CTRL_BUTTON, ".,?",
                       WS_VISIBLE | BS_PUSHBUTTON,
                       0,
                       IDC_PUNCT_BUTTON,
                       button_start_x + 1 * button_spacing_x,
                       button_start_y + 4 * button_spacing_y,
                       70, 40, hWnd, NULL, NULL, 0);
            if (korean_font_bold && hPunctButton != HWND_INVALID) {
                SetWindowFont(hPunctButton, korean_font_bold);
            }

            hEnterButton = CreateWindowEx2(CTRL_BUTTON, "Enter",
                       WS_VISIBLE | BS_PUSHBUTTON,
                       0,
                       IDC_ENTER_BUTTON,
                       button_start_x + 2 * button_spacing_x,
                       button_start_y + 4 * button_spacing_y,
                       70, 40, hWnd, NULL, NULL, 0);
            if (korean_font_bold && hEnterButton != HWND_INVALID) {
                SetWindowFont(hEnterButton, korean_font_bold);
            }

            hSaveButton = CreateWindowEx2(CTRL_BUTTON, "Save",
                       WS_VISIBLE | BS_PUSHBUTTON,
                       0,
                       IDC_SAVE_BUTTON,
                       button_start_x + 0 * button_spacing_x,
                       button_start_y + 5 * button_spacing_y,
                       70, 40, hWnd, NULL, NULL, 0);
            if (korean_font_bold && hSaveButton != HWND_INVALID) {
                SetWindowFont(hSaveButton, korean_font_bold);
            }

            hLoadButton = CreateWindowEx2(CTRL_BUTTON, "Load",
                       WS_VISIBLE | BS_PUSHBUTTON,
                       0,
                       IDC_LOAD_BUTTON,
                       button_start_x + 1 * button_spacing_x,
                       button_start_y + 5 * button_spacing_y,
                       70, 40, hWnd, NULL, NULL, 0);
            if (korean_font_bold && hLoadButton != HWND_INVALID) {
                SetWindowFont(hLoadButton, korean_font_bold);
            }

            if (g_state) {
                gui_update_display(g_state);
            }
            break;
        }

        case MSG_COMMAND:
            return ButtonClickProc(hWnd, message, wParam, lParam);

        case MSG_CLOSE:
            DestroyMainWindow(hWnd);
            PostQuitMessage(hWnd);
            return 0;

        case MSG_DESTROY:
            gui_cleanup_fonts();
            return 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

HWND gui_create_main_window(ChunjiinState *state) {
    g_state = state;

    HWND hMainWnd;
    MAINWINCREATE CreateInfo;

    setlocale(LC_ALL, "ko_KR.UTF-8");
    gui_init_fonts();

    int screen_width = GetGDCapability(HDC_SCREEN, GDCAP_HPIXEL);
    int screen_height = GetGDCapability(HDC_SCREEN, GDCAP_VPIXEL);
    int window_width = 280;
    int window_height = 420;
    int window_x = (screen_width - window_width) / 2;
    int window_y = (screen_height - window_height) / 2;

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
        return HWND_INVALID;
    }

    printf("Main window created successfully\n");
    ShowWindow(hMainWnd, SW_SHOWNORMAL);
    printf("Window shown\n");

    return hMainWnd;
}
