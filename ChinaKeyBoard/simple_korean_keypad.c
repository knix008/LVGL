/*
 * Simple Korean Keypad MiniGUI Application
 * 
 * A clean GUI application for Korean keyboard input without animation dependencies
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#define IDC_EDIT_INPUT      100
#define IDC_BTN_CLEAR       101
#define IDC_STATIC_STATUS   102

static HWND hEditInput;
static HWND hStatusLabel;
static int shift_state = 0;
static char input_buffer[1024] = "";

/* Korean character arrays - UTF-8 encoded */
static const char* korean_chars[] = {
    "\xE3\x85\x82", /* ㅂ */
    "\xE3\x85\x88", /* ㅈ */
    "\xE3\x84\xB7", /* ㄷ */
    "\xE3\x84\xB1", /* ㄱ */
    "\xE3\x85\x85", /* ㅅ */
    "\xE3\x85\x9B", /* ㅛ */
    "\xE3\x85\x95", /* ㅕ */
    "\xE3\x85\x91", /* ㅑ */
    "\xE3\x85\x90", /* ㅐ */
    "\xE3\x85\x94", /* ㅔ */
    "\xE3\x85\x81", /* ㅁ */
    "\xE3\x84\xB2", /* ㄴ */
    "\xE3\x85\x87", /* ㅇ */
    "\xE3\x84\xB9", /* ㄹ */
    "\xE3\x85\x8E", /* ㅎ */
    "\xE3\x85\x97", /* ㅗ */
    "\xE3\x85\x93", /* ㅓ */
    "\xE3\x85\x8F", /* ㅏ */
    "\xE3\x85\xA3", /* ㅣ */
    "\xE3\x85\x8B", /* ㅋ */
    "\xE3\x85\x8C", /* ㅌ */
    "\xE3\x85\x8A", /* ㅊ */
    "\xE3\x85\x8D", /* ㅍ */
    "\xE3\x85\xA0", /* ㅠ */
    "\xE3\x85\x9C", /* ㅜ */
    "\xE3\x85\xA1"  /* ㅡ */
};

static const char* korean_shift_chars[] = {
    "\xE3\x85\x83", /* ㅃ */
    "\xE3\x85\x89", /* ㅉ */
    "\xE3\x84\xB8", /* ㄸ */
    "\xE3\x84\xB2", /* ㄲ */
    "\xE3\x85\x86", /* ㅆ */
    "\xE3\x85\x9B", /* ㅛ */
    "\xE3\x85\x95", /* ㅕ */
    "\xE3\x85\x91", /* ㅑ */
    "\xE3\x85\x92", /* ㅒ */
    "\xE3\x85\x96", /* ㅖ */
    "\xE3\x85\x81", /* ㅁ */
    "\xE3\x84\xB2", /* ㄴ */
    "\xE3\x85\x87", /* ㅇ */
    "\xE3\x84\xB9", /* ㄹ */
    "\xE3\x85\x8E", /* ㅎ */
    "\xE3\x85\x97", /* ㅗ */
    "\xE3\x85\x93", /* ㅓ */
    "\xE3\x85\x8F", /* ㅏ */
    "\xE3\x85\xA3", /* ㅣ */
    "\xE3\x85\x8B", /* ㅋ */
    "\xE3\x85\x8C", /* ㅌ */
    "\xE3\x85\x8A", /* ㅊ */
    "\xE3\x85\x8D", /* ㅍ */
    "\xE3\x85\xA0", /* ㅠ */
    "\xE3\x85\x9C", /* ㅜ */
    "\xE3\x85\xA1"  /* ㅡ */
};

/* Korean QWERTY key mapping */
static const char korean_keys[] = "qwertyuiopasdfghjklzxcvbnm";

/* Draw Korean keyboard layout */
static void DrawKoreanKeyboard(HDC hdc, int x, int y, int width, int height)
{
    int key_width = width / 10;
    int key_height = height / 4;
    int key_spacing = 2;
    
    /* First row: ㅂㅈㄷㄱㅅㅛㅕㅑㅐㅔ */
    for (int i = 0; i < 10; i++) {
        RECT key_rect = {
            x + i * (key_width + key_spacing),
            y,
            x + (i + 1) * key_width + i * key_spacing,
            y + key_height
        };
        
        /* Draw key background */
        SetBrushColor(hdc, RGB2Pixel(hdc, 240, 240, 240));
        FillBox(hdc, key_rect.left, key_rect.top, 
                key_rect.right - key_rect.left, 
                key_rect.bottom - key_rect.top);
        
        /* Draw key border */
        SetPenColor(hdc, RGB2Pixel(hdc, 180, 180, 180));
        Rectangle(hdc, key_rect.left, key_rect.top, key_rect.right, key_rect.bottom);
        
        /* Draw Korean character */
        SetTextColor(hdc, RGB2Pixel(hdc, 0, 0, 0));
        SetBkMode(hdc, BM_TRANSPARENT);
        RECT text_rect = key_rect;
        const char* char_to_show = shift_state ? korean_shift_chars[i] : korean_chars[i];
        DrawText(hdc, char_to_show, -1, &text_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
    
    /* Second row: ㅁㄴㅇㄹㅎㅗㅓㅏㅣ */
    for (int i = 0; i < 9; i++) {
        RECT key_rect = {
            x + (i + 0.5) * (key_width + key_spacing),
            y + key_height + key_spacing,
            x + (i + 1.5) * key_width + (i + 0.5) * key_spacing,
            y + 2 * key_height + key_spacing
        };
        
        SetBrushColor(hdc, RGB2Pixel(hdc, 240, 240, 240));
        FillBox(hdc, key_rect.left, key_rect.top, 
                key_rect.right - key_rect.left, 
                key_rect.bottom - key_rect.top);
        
        SetPenColor(hdc, RGB2Pixel(hdc, 180, 180, 180));
        Rectangle(hdc, key_rect.left, key_rect.top, key_rect.right, key_rect.bottom);
        
        SetTextColor(hdc, RGB2Pixel(hdc, 0, 0, 0));
        SetBkMode(hdc, BM_TRANSPARENT);
        RECT text_rect = key_rect;
        const char* char_to_show = shift_state ? korean_shift_chars[10 + i] : korean_chars[10 + i];
        DrawText(hdc, char_to_show, -1, &text_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
    
    /* Third row: Shift + ㅋㅌㅊㅍㅠㅜㅡ + Backspace */
    /* Shift key */
    RECT shift_rect = {x, y + 2 * (key_height + key_spacing), 
                       x + key_width, y + 3 * key_height + 2 * key_spacing};
    SetBrushColor(hdc, shift_state ? RGB2Pixel(hdc, 200, 200, 255) : RGB2Pixel(hdc, 220, 220, 220));
    FillBox(hdc, shift_rect.left, shift_rect.top, 
            shift_rect.right - shift_rect.left, 
            shift_rect.bottom - shift_rect.top);
    SetPenColor(hdc, RGB2Pixel(hdc, 180, 180, 180));
    Rectangle(hdc, shift_rect.left, shift_rect.top, shift_rect.right, shift_rect.bottom);
    SetTextColor(hdc, RGB2Pixel(hdc, 0, 0, 0));
    SetBkMode(hdc, BM_TRANSPARENT);
    RECT shift_text_rect = shift_rect;
    DrawText(hdc, "Shift", -1, &shift_text_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    
    /* Korean characters */
    for (int i = 0; i < 7; i++) {
        RECT key_rect = {
            x + (i + 1) * (key_width + key_spacing),
            y + 2 * (key_height + key_spacing),
            x + (i + 2) * key_width + (i + 1) * key_spacing,
            y + 3 * key_height + 2 * key_spacing
        };
        
        SetBrushColor(hdc, RGB2Pixel(hdc, 240, 240, 240));
        FillBox(hdc, key_rect.left, key_rect.top, 
                key_rect.right - key_rect.left, 
                key_rect.bottom - key_rect.top);
        
        SetPenColor(hdc, RGB2Pixel(hdc, 180, 180, 180));
        Rectangle(hdc, key_rect.left, key_rect.top, key_rect.right, key_rect.bottom);
        
        SetTextColor(hdc, RGB2Pixel(hdc, 0, 0, 0));
        SetBkMode(hdc, BM_TRANSPARENT);
        RECT text_rect = key_rect;
        const char* char_to_show = shift_state ? korean_shift_chars[19 + i] : korean_chars[19 + i];
        DrawText(hdc, char_to_show, -1, &text_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
    
    /* Backspace key */
    RECT backspace_rect = {x + 8 * (key_width + key_spacing), y + 2 * (key_height + key_spacing),
                           x + 10 * key_width + 8 * key_spacing, y + 3 * key_height + 2 * key_spacing};
    SetBrushColor(hdc, RGB2Pixel(hdc, 255, 200, 200));
    FillBox(hdc, backspace_rect.left, backspace_rect.top, 
            backspace_rect.right - backspace_rect.left, 
            backspace_rect.bottom - backspace_rect.top);
    SetPenColor(hdc, RGB2Pixel(hdc, 180, 180, 180));
    Rectangle(hdc, backspace_rect.left, backspace_rect.top, backspace_rect.right, backspace_rect.bottom);
    SetTextColor(hdc, RGB2Pixel(hdc, 0, 0, 0));
    SetBkMode(hdc, BM_TRANSPARENT);
    RECT backspace_text_rect = backspace_rect;
    DrawText(hdc, "⌫", -1, &backspace_text_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    
    /* Fourth row: Space bar */
    RECT space_rect = {x + 2 * (key_width + key_spacing), y + 3 * (key_height + key_spacing),
                       x + 8 * key_width + 6 * key_spacing, y + 4 * key_height + 3 * key_spacing};
    SetBrushColor(hdc, RGB2Pixel(hdc, 240, 240, 240));
    FillBox(hdc, space_rect.left, space_rect.top, 
            space_rect.right - space_rect.left, 
            space_rect.bottom - space_rect.top);
    SetPenColor(hdc, RGB2Pixel(hdc, 180, 180, 180));
    Rectangle(hdc, space_rect.left, space_rect.top, space_rect.right, space_rect.bottom);
    SetTextColor(hdc, RGB2Pixel(hdc, 0, 0, 0));
    SetBkMode(hdc, BM_TRANSPARENT);
    RECT space_text_rect = space_rect;
    DrawText(hdc, "Space", -1, &space_text_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

/* Handle Korean character input */
static void HandleKoreanInput(char key)
{
    char buffer[256];
    GetWindowText(hEditInput, buffer, sizeof(buffer));
    
    /* Find the Korean character for this key */
    const char* pos = strchr(korean_keys, key);
    if (pos) {
        int index = pos - korean_keys;
        const char* korean_char = shift_state ? korean_shift_chars[index] : korean_chars[index];
        strcat(buffer, korean_char);
        SetWindowText(hEditInput, buffer);
        printf("Korean input: %s (key: %c)\n", korean_char, key);
    } else if (key == ' ') {
        strcat(buffer, " ");
        SetWindowText(hEditInput, buffer);
    } else if (key == '\b') {
        if (strlen(buffer) > 0) {
            buffer[strlen(buffer) - 1] = '\0';
            SetWindowText(hEditInput, buffer);
        }
    }
}

static LRESULT KoreanKeypadProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case MSG_CREATE:
        {
            /* Input text field */
            hEditInput = CreateWindow(
                "EDIT",
                "",
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_MULTILINE,
                IDC_EDIT_INPUT,
                20, 20, 560, 100,
                hWnd, 0);
            
            /* Control buttons */
            CreateWindow("BUTTON", "Clear", 
                        WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                        IDC_BTN_CLEAR,
                        20, 140, 80, 30, hWnd, 0);
            
            /* Status label */
            hStatusLabel = CreateWindow("STATIC", "Korean Mode: ON | Shift: OFF", 
                                      WS_CHILD | WS_VISIBLE,
                                      IDC_STATIC_STATUS,
                                      20, 180, 300, 20, hWnd, 0);
            
            printf("Korean keypad application created!\n");
            break;
        }

        case MSG_PAINT:
        {
            HDC hdc = BeginPaint(hWnd);
            RECT rect;
            GetClientRect(hWnd, &rect);
            
            /* Clear background */
            SetBrushColor(hdc, RGB2Pixel(hdc, 255, 255, 255));
            FillBox(hdc, 0, 0, rect.right, rect.bottom);
            
            /* Draw title */
            SetTextColor(hdc, RGB2Pixel(hdc, 0, 0, 128));
            TextOut(hdc, 20, 5, "Korean Keypad Application - 한국어 키패드");
            
            /* Draw Korean keyboard layout */
            DrawKoreanKeyboard(hdc, 20, 220, 560, 200);
            
            /* Draw instructions */
            SetTextColor(hdc, RGB2Pixel(hdc, 64, 64, 64));
            TextOut(hdc, 20, 430, "Instructions:");
            TextOut(hdc, 20, 450, "• Type on physical keyboard to input Korean characters");
            TextOut(hdc, 20, 470, "• Shift key toggles double consonants (ㅂ→ㅃ, ㅈ→ㅉ, etc.)");
            TextOut(hdc, 20, 490, "• Korean characters are displayed on visual keyboard");
            
            EndPaint(hWnd, hdc);
            return 0;
        }

        case MSG_COMMAND:
        {
            switch (LOWORD(wParam)) {
                case IDC_BTN_CLEAR:
                    SetWindowText(hEditInput, "");
                    input_buffer[0] = '\0';
                    break;
            }
            break;
        }

        case MSG_LBUTTONDOWN:
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            
            /* Check if shift key was clicked */
            if (x >= 20 && x <= 20 + 60 && y >= 220 + 2 * 50 && y <= 220 + 3 * 50) {
                shift_state = !shift_state;
                char status_text[64];
                snprintf(status_text, sizeof(status_text), 
                        "Korean Mode: ON | Shift: %s", 
                        shift_state ? "ON" : "OFF");
                SetWindowText(hStatusLabel, status_text);
                InvalidateRect(hWnd, NULL, TRUE);
                printf("Shift state: %s\n", shift_state ? "ON" : "OFF");
            }
            break;
        }

        case MSG_KEYDOWN:
        {
            /* Handle physical keyboard input */
            char key = (char)wParam;
            
            if (key == 16) { /* Shift key */
                shift_state = !shift_state;
                char status_text[64];
                snprintf(status_text, sizeof(status_text), 
                        "Korean Mode: ON | Shift: %s", 
                        shift_state ? "ON" : "OFF");
                SetWindowText(hStatusLabel, status_text);
                InvalidateRect(hWnd, NULL, TRUE);
                printf("Shift state: %s\n", shift_state ? "ON" : "OFF");
            } else {
                HandleKoreanInput(key);
            }
            break;
        }

        case MSG_CLOSE:
            DestroyWindow(hWnd);
            PostQuitMessage(hWnd);
            break;
    }
    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

int MiniGUIMain(int argc, const char* argv[])
{
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;

    printf("Simple Korean Keypad MiniGUI Application\n");
    printf("========================================\n");
    printf("Starting Korean keyboard GUI application...\n");

#ifdef _MGRM_LITE
    SetMgEtcValue("enable_cursor", 1);
#endif

    CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION;
    CreateInfo.dwExStyle = WS_EX_TOOLWINDOW;
    CreateInfo.spCaption = "Korean Keypad - 한국어 키패드";
    CreateInfo.hMenu = 0;
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = KoreanKeypadProc;
    CreateInfo.lx = 100;
    CreateInfo.ty = 100;
    CreateInfo.rx = 700;
    CreateInfo.by = 600;
    CreateInfo.iBkColor = COLOR_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;

    hMainWnd = CreateMainWindow(&CreateInfo);
    if (hMainWnd == HWND_INVALID) {
        printf("Failed to create main window!\n");
        return -1;
    }

    ShowWindow(hMainWnd, SW_SHOWNORMAL);
    
    printf("Korean keypad application started!\n");
    printf("Features:\n");
    printf("- Korean character display on visual keyboard\n");
    printf("- Shift functionality for double consonants\n");
    printf("- Physical keyboard input support\n");
    printf("- Input text field\n");
    printf("- Visual feedback\n");
    printf("\nTry typing: qwertyuiopasdfghjklzxcvbnm\n");
    printf("Use Shift key to toggle double consonants\n");

    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    printf("Korean keypad application closed.\n");
    return 0;
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif
