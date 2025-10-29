/*
 * Simple Korean Keypad MiniGUI Application - FIXED
 * 
 * A clean GUI application for Korean keyboard input with clickable buttons
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
#include "ime/libime/ime_korean.h"

#define IDC_EDIT_INPUT      100
#define IDC_BTN_CLEAR       101
#define IDC_STATIC_STATUS   102

static HWND hEditInput;
static HWND hStatusLabel;
static int shift_state = 0;
static char input_buffer[1024] = "";
static char stroke_buffer[256] = "";  // Buffer for Korean keystrokes (current composing)
static char committed_text[2048] = ""; // Buffer for committed text (completed syllables)
static PLOGFONT korean_font = NULL;

/* Korean character arrays - UTF-8 encoded */
static const char* korean_chars[] = {
    "ㅂ", "ㅈ", "ㄷ", "ㄱ", "ㅅ", "ㅛ", "ㅕ", "ㅑ", "ㅐ", "ㅔ",
    "ㅁ", "ㄴ", "ㅇ", "ㄹ", "ㅎ", "ㅗ", "ㅓ", "ㅏ", "ㅣ",
    "ㅋ", "ㅌ", "ㅊ", "ㅍ", "ㅠ", "ㅜ", "ㅡ"
};

static const char* korean_shift_chars[] = {
    "ㅃ", "ㅉ", "ㄸ", "ㄲ", "ㅆ", "ㅛ", "ㅕ", "ㅑ", "ㅒ", "ㅖ",
    "ㅁ", "ㄴ", "ㅇ", "ㄹ", "ㅎ", "ㅗ", "ㅓ", "ㅏ", "ㅣ",
    "ㅋ", "ㅌ", "ㅊ", "ㅍ", "ㅠ", "ㅜ", "ㅡ"
};

/* Korean QWERTY key mapping */
static const char korean_keys[] = "qwertyuiopasdfghjklzxcvbnm";

/* Button structure for click detection */
typedef struct {
    RECT rect;
    char key;
    int char_index;
} button_t;

static button_t buttons[30]; // Store all clickable buttons
static int button_count = 0;

/* Map visual key + shift_state to IME keystroke expected by ime_korean.c */
static char map_key_for_ime(char key, int shift_on)
{
    if (!shift_on) return key;
    /* Only uppercase keys that have shifted meanings in Dubeolsik */
    switch (key) {
        /* Double consonants */
        case 'q': return 'Q'; /* ㅂ -> ㅃ */
        case 'w': return 'W'; /* ㅈ -> ㅉ */
        case 'e': return 'E'; /* ㄷ -> ㄸ */
        case 'r': return 'R'; /* ㄱ -> ㄲ */
        case 't': return 'T'; /* ㅅ -> ㅆ */
        /* Alternative vowels */
        case 'o': return 'O'; /* ㅐ/ㅒ */
        case 'p': return 'P'; /* ㅔ/ㅖ */
        case 'k': return 'K'; /* ㅏ/ㅐ */
        default:
            return key; /* others unchanged when shifted */
    }
}

/* Initialize button positions */
static void InitButtons(int x, int y, int width, int height) {
    int key_width = width / 10;
    int key_height = height / 4;
    int key_spacing = 2;
    button_count = 0;
    
    /* First row: ㅂㅈㄷㄱㅅㅛㅕㅑㅐㅔ */
    for (int i = 0; i < 10; i++) {
        buttons[button_count].rect.left = x + i * (key_width + key_spacing);
        buttons[button_count].rect.top = y;
        buttons[button_count].rect.right = x + (i + 1) * key_width + i * key_spacing;
        buttons[button_count].rect.bottom = y + key_height;
        buttons[button_count].key = korean_keys[i];
        buttons[button_count].char_index = i;
        button_count++;
    }
    
    /* Second row: ㅁㄴㅇㄹㅎㅗㅓㅏㅣ */
    for (int i = 0; i < 9; i++) {
        buttons[button_count].rect.left = x + (i + 0.5) * (key_width + key_spacing);
        buttons[button_count].rect.top = y + key_height + key_spacing;
        buttons[button_count].rect.right = x + (i + 1.5) * key_width + (i + 0.5) * key_spacing;
        buttons[button_count].rect.bottom = y + 2 * key_height + key_spacing;
        buttons[button_count].key = korean_keys[10 + i];
        buttons[button_count].char_index = 10 + i;
        button_count++;
    }
    
    /* Third row: ㅋㅌㅊㅍㅠㅜㅡ */
    for (int i = 0; i < 7; i++) {
        buttons[button_count].rect.left = x + (i + 1) * (key_width + key_spacing);
        buttons[button_count].rect.top = y + 2 * (key_height + key_spacing);
        buttons[button_count].rect.right = x + (i + 2) * key_width + (i + 1) * key_spacing;
        buttons[button_count].rect.bottom = y + 3 * key_height + 2 * key_spacing;
        buttons[button_count].key = korean_keys[19 + i];
        buttons[button_count].char_index = 19 + i;
        button_count++;
    }
}

/* Check if point is inside rectangle */
static int PointInRect(POINT pt, RECT rect) {
    return (pt.x >= rect.left && pt.x <= rect.right && 
            pt.y >= rect.top && pt.y <= rect.bottom);
}

/* Find clicked button */
static button_t* FindClickedButton(int x, int y) {
    POINT pt = {x, y};
    for (int i = 0; i < button_count; i++) {
        if (PointInRect(pt, buttons[i].rect)) {
            return &buttons[i];
        }
    }
    return NULL;
}

/* Draw Korean keyboard layout */
static void DrawKoreanKeyboard(HDC hdc, int x, int y, int width, int height) {
    int key_width = width / 10;
    int key_height = height / 4;
    int key_spacing = 2;

    /* Set Korean font for keyboard rendering */
    if (korean_font) {
        SelectFont(hdc, korean_font);
    }
    
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

/* Handle Korean character input using IME logic */
static void HandleKoreanInput(char key) {
    char prev_output[256] = "";
    char new_output[256] = "";
    
    if (key == '\b') {
        // Handle backspace
        if (strlen(stroke_buffer) > 0) {
            stroke_buffer[strlen(stroke_buffer) - 1] = '\0';
        } else if (strlen(committed_text) > 0) {
            // Remove last UTF-8 codepoint from committed text
            size_t len = strlen(committed_text);
            if (len > 0) {
                // Move backwards to the start of previous UTF-8 character
                size_t i = len - 1;
                while (i > 0 && ((unsigned char)committed_text[i] & 0xC0) == 0x80) {
                    i--;
                }
                committed_text[i] = '\0';
            }
        }
    } else if (key == ' ') {
        // Commit current composition and add a space
        cb_hangul_match_keystrokes(stroke_buffer, prev_output, sizeof(prev_output), 0, 0);
        if (strlen(prev_output) + strlen(committed_text) + 1 < sizeof(committed_text)) {
            strcat(committed_text, prev_output);
            strcat(committed_text, " ");
        }
        stroke_buffer[0] = '\0';
    } else {
        // Compute previous output (before adding key)
        cb_hangul_match_keystrokes(stroke_buffer, prev_output, sizeof(prev_output), 0, 0);

        // Tentatively add key
        char temp_strokes[256];
        snprintf(temp_strokes, sizeof(temp_strokes), "%s%c", stroke_buffer, key);

        // Compute new output after adding key
        cb_hangul_match_keystrokes(temp_strokes, new_output, sizeof(new_output), 0, 0);

        // Heuristic: if prev_output is a complete syllable (3-byte UTF-8)
        // and new_output looks like starting a new syllable (single jamo),
        // commit prev_output and start new composition with just this key.
        int prev_is_syllable = (strlen(prev_output) == 3);
        int new_is_jamo = (strlen(new_output) == 3 ? 0 : strlen(new_output) > 0);

        if (prev_is_syllable && new_is_jamo) {
            if (strlen(committed_text) + strlen(prev_output) < sizeof(committed_text)) {
                strcat(committed_text, prev_output);
            }
            // Start new composition with current key only
            stroke_buffer[0] = key;
            stroke_buffer[1] = '\0';
            cb_hangul_match_keystrokes(stroke_buffer, new_output, sizeof(new_output), 0, 0);
        } else {
            // Keep composing within the same syllable/word
            size_t len = strlen(stroke_buffer);
            if (len + 1 < sizeof(stroke_buffer)) {
                stroke_buffer[len] = key;
                stroke_buffer[len + 1] = '\0';
            }
        }
    }
    // Get current composition for display
    cb_hangul_match_keystrokes(stroke_buffer, new_output, sizeof(new_output), 0, 0);

    // Build display text = committed_text + current composition
    char display_buffer[4096];
    display_buffer[0] = '\0';
    strncat(display_buffer, committed_text, sizeof(display_buffer) - 1);
    
    // Append the current composition (complete or incomplete)
    if (strlen(new_output) > 0) {
        strncat(display_buffer, new_output, sizeof(display_buffer) - 1 - strlen(display_buffer));
    }

    // Update the edit control with the full text
    SetWindowText(hEditInput, display_buffer);
    
    // Update status label to show stroke buffer
    char status_text[256];
    snprintf(status_text, sizeof(status_text), "Korean Mode: ON | Shift: %s | Strokes: [%s]", 
             shift_state ? "ON" : "OFF", stroke_buffer);
    SetWindowText(hStatusLabel, status_text);
    
    printf("Stroke buffer: [%s] -> Output: [%s] | Full text: [%s]\n", 
           stroke_buffer, new_output, display_buffer);
}

static LRESULT KoreanKeypadProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
        case MSG_CREATE:
        {
            /* Create Korean font for Unicode/UTF-8 display */
            korean_font = CreateLogFont("ttf", "NanumGothic-Regular", "UTF-8", 
                                        FONT_WEIGHT_NORMAL, FONT_SLANT_ROMAN, FONT_FLIP_NONE,
                                        FONT_OTHER_NONE, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                                        20, 0);
            if (korean_font == NULL) {
                // Try with Bold variant
                korean_font = CreateLogFont("ttf", "NanumGothic-Bold", "UTF-8", 
                                            FONT_WEIGHT_NORMAL, FONT_SLANT_ROMAN, FONT_FLIP_NONE,
                                            FONT_OTHER_NONE, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                                            20, 0);
            }
            if (korean_font == NULL) {
                // Try with ExtraBold variant
                korean_font = CreateLogFont("ttf", "NanumGothic-ExtraBold", "UTF-8", 
                                            FONT_WEIGHT_NORMAL, FONT_SLANT_ROMAN, FONT_FLIP_NONE,
                                            FONT_OTHER_NONE, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                                            20, 0);
            }

            if (korean_font == NULL) {
                // Final fallback to default font
                korean_font = NULL;
                printf("Using MiniGUI default font (Korean characters may not display correctly)\n");
            } else {
                printf("Loaded Korean font from assets directory for Korean character support\n");
                printf("Font family: %s, charset: %s, size: %d\n", 
                       korean_font->family, korean_font->charset, korean_font->size);
            }

            /* Input text field */
            hEditInput = CreateWindow(
                "EDIT",
                "",
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_MULTILINE,
                IDC_EDIT_INPUT,
                20, 20, 560, 100,
                hWnd, 0);

            /* Set Korean font for edit control */
            if (korean_font) {
                SetWindowFont(hEditInput, korean_font);
            }

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

            /* Set Korean font for status label */
            if (korean_font) {
                SetWindowFont(hStatusLabel, korean_font);
            }

            /* Initialize button positions */
            InitButtons(20, 220, 560, 200);

            printf("Korean keypad application created!\n");
            break;
        }

        case MSG_PAINT:
        {
            HDC hdc = BeginPaint(hWnd);
            RECT rect;
            GetClientRect(hWnd, &rect);

            /* Set Korean font for text rendering */
            if (korean_font) {
                SelectFont(hdc, korean_font);
            }

            /* Clear background */
            SetBrushColor(hdc, RGB2Pixel(hdc, 255, 255, 255));
            FillBox(hdc, 0, 0, rect.right, rect.bottom);

            /* Draw title */
            SetTextColor(hdc, RGB2Pixel(hdc, 0, 0, 128));
            TextOut(hdc, 20, 5, "Korean Keypad Application - \xED\x95\x9C\xEA\xB5\xAD\xEC\x96\xB4 \xED\x82\xA4\xED\x8C\xA8\xEB\x93\x9C");

            /* Draw Korean keyboard layout */
            DrawKoreanKeyboard(hdc, 20, 220, 560, 200);

            /* Draw instructions */
            SetTextColor(hdc, RGB2Pixel(hdc, 64, 64, 64));
            TextOut(hdc, 20, 430, "Instructions:");
            TextOut(hdc, 20, 450, "• Click Korean character buttons to input Korean characters");
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
                    stroke_buffer[0] = '\0';  // Clear stroke buffer too
                    committed_text[0] = '\0'; // Clear committed text
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
            /* Check if backspace was clicked */
            else if (x >= 20 + 8 * 60 && x <= 20 + 10 * 60 && y >= 220 + 2 * 50 && y <= 220 + 3 * 50) {
                HandleKoreanInput('\b');
            }
            /* Check if space was clicked */
            else if (x >= 20 + 2 * 60 && x <= 20 + 8 * 60 && y >= 220 + 3 * 50 && y <= 220 + 4 * 50) {
                HandleKoreanInput(' ');
            }
            /* Check if Korean character button was clicked */
            else {
                button_t* clicked_button = FindClickedButton(x, y);
                if (clicked_button) {
                    char ime_key = map_key_for_ime(clicked_button->key, shift_state);
                    HandleKoreanInput(ime_key);
                    printf("Clicked button: %c -> %s\n", clicked_button->key, 
                           shift_state ? korean_shift_chars[clicked_button->char_index] : korean_chars[clicked_button->char_index]);
                }
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
                /* Normalize to lowercase letters for mapping table, then apply shift mapping */
                if (key >= 'A' && key <= 'Z') {
                    key = (char)(key - 'A' + 'a');
                }
                char ime_key = map_key_for_ime(key, shift_state);
                HandleKoreanInput(ime_key);
            }
            break;
        }

        case MSG_CLOSE:
            /* Clean up Korean font */
            if (korean_font) {
                DestroyLogFont(korean_font);
                korean_font = NULL;
            }
            DestroyWindow(hWnd);
            PostQuitMessage(hWnd);
            break;
    }
    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

int MiniGUIMain(int argc, const char* argv[])
{
    (void)argc;  // Suppress unused parameter warning
    (void)argv;   // Suppress unused parameter warning
    
    MSG Msg;
    HWND hMainWnd;
    MAINWINCREATE CreateInfo;

    printf("Simple Korean Keypad MiniGUI Application - FIXED\n");
    printf("================================================\n");
    printf("Starting Korean keyboard GUI application with clickable buttons...\n");

#ifdef _MGRM_LITE
    SetMgEtcValue("enable_cursor", 1);
#endif

    CreateInfo.dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION;
    CreateInfo.dwExStyle = WS_EX_TOOLWINDOW;
    CreateInfo.spCaption = "Korean Keypad - \xED\x95\x9C\xEA\xB5\xAD\xEC\x96\xB4 \xED\x82\xA4\xED\x8C\xA8\xEB\x93\x9C (Clickable)";
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
    printf("- CLICKABLE Korean character buttons\n");
    printf("- Shift functionality for double consonants\n");
    printf("- Physical keyboard input support\n");
    printf("- Input text field\n");
    printf("- Visual feedback\n");
    printf("\nTry clicking the Korean character buttons!\n");
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
