/*
 * Korean Text Display Program for MiniGUI
 * This program displays Korean characters using UTF-8 encoding
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <wchar.h>
#include <iconv.h>
#include <unistd.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

// Korean text samples for CJK font testing
static const char* korean_texts[] = {
    "안녕하세요! Hello in Korean",
    "한국어 텍스트 표시 프로그램", 
    "MiniGUI로 한국어를 표시합니다",
    "가나다라마바사아자차카타파하",
    "한글 입력 시스템 테스트",
    "CJK 폰트로 한국어 표시 테스트",
    "NotoSansCJK 폰트로 표시",
    "NotoSerifCJK 폰트로 표시",
    "한글 자모음 테스트: ㄱㄴㄷㄹㅁㅂㅅㅇㅈㅊㅋㅌㅍㅎ",
    "한글 복합자음 테스트: ㄲㄸㅃㅆㅉ",
    "한글 복합모음 테스트: ㅐㅒㅔㅖㅘㅙㅚㅝㅞㅟㅢ",
    "한글 숫자 테스트: 0123456789",
    "한글 특수문자 테스트: !@#$%^&*()",
    "한글 문장 테스트: 이것은 한국어 문장입니다.",
    "한글 시험 문장: 가나다라마바사아자차카타파하",
    NULL
};

static int current_text = 0;
static int current_font = 0;

// Global variables to store three Korean fonts
static PLOGFONT korean_font_regular = NULL;
static PLOGFONT korean_font_bold = NULL;
static PLOGFONT korean_font_extrabold = NULL;

// Font names for display
static const char* font_names[] = {
    "NanumGothic Regular",
    "NanumGothic Bold", 
    "NanumGothic ExtraBold",
    NULL
};

static LRESULT KoreanWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    RECT rect;

    switch (message) {
        case MSG_PAINT:
            printf("MSG_PAINT received - starting paint\n");
            hdc = BeginPaint(hWnd);
            GetClientRect(hWnd, &rect);
            printf("Window size: %dx%d\n", RECTW(rect), RECTH(rect));
            
            // Set background color
            SetBkColor(hdc, PIXEL_lightwhite);
            FillBox(hdc, 0, 0, RECTW(rect), RECTH(rect));
            printf("Background filled\n");
            
            // Apply the appropriate Korean font based on current_font
            PLOGFONT selected_font = NULL;
            if (current_font == 0 && korean_font_regular) {
                selected_font = korean_font_regular;
                printf("✓ Using NanumGothic Regular font\n");
            } else if (current_font == 1 && korean_font_bold) {
                selected_font = korean_font_bold;
                printf("✓ Using NanumGothic Bold font\n");
            } else if (current_font == 2 && korean_font_extrabold) {
                selected_font = korean_font_extrabold;
                printf("✓ Using NanumGothic ExtraBold font\n");
            } else {
                // Fallback to any available font
                selected_font = korean_font_regular ? korean_font_regular : 
                               (korean_font_bold ? korean_font_bold : korean_font_extrabold);
                printf("✓ Using fallback Korean font\n");
            }
            
            if (selected_font) {
                SelectFont(hdc, selected_font);
                printf("✓ Korean font selected successfully\n");
            } else {
                printf("We cannot using korean fonts now!!!\n");
                break;
            }
            
            // Display current Korean text using direct UTF-8
            if (korean_texts[current_text]) {
                // Debug: Print to console
                printf("Displaying Korean text: %s\n", korean_texts[current_text]);
                
                // Clear background
                SetBkColor(hdc, PIXEL_lightwhite);
                SetTextColor(hdc, PIXEL_black);
                
                // Display the main Korean text prominently in the center
                RECT text_rect;
                text_rect.left = 50;
                text_rect.top = 80;
                text_rect.right = RECTW(rect) - 50;
                text_rect.bottom = 150;
                
                // Draw a subtle border around the text area
                SetBkColor(hdc, PIXEL_lightgray);
                FillBox(hdc, text_rect.left - 10, text_rect.top - 10, 
                       text_rect.right - text_rect.left + 20, 
                       text_rect.bottom - text_rect.top + 20);
                
                // Display the current Korean text from korean_texts array
                SetBkColor(hdc, PIXEL_lightwhite);
                SetTextColor(hdc, PIXEL_black);
                DrawText(hdc, korean_texts[current_text], -1, &text_rect, 
                        DT_CENTER | DT_VCENTER | DT_WORDBREAK);
                
                // Display text index information
                char index_info[100];
                snprintf(index_info, sizeof(index_info), "Text %d of %d", 
                        current_text + 1, 
                        (int)(sizeof(korean_texts)/sizeof(korean_texts[0]) - 1));
                        
                RECT index_rect;
                index_rect.left = 50;
                index_rect.top = 160;
                index_rect.right = RECTW(rect) - 50;
                index_rect.bottom = 180;
                
                SetTextColor(hdc, PIXEL_darkblue);
                DrawText(hdc, index_info, -1, &index_rect, DT_CENTER | DT_SINGLELINE);
            }
            
            // Display font information more prominently
            SetTextColor(hdc, PIXEL_darkblue);
            rect.top = rect.bottom - 80;
            rect.bottom = rect.bottom - 50;
            if (font_names[current_font]) {
                char font_info[256];
                snprintf(font_info, sizeof(font_info), "Font: %s (Index: %d)", 
                        font_names[current_font], current_font);
                DrawText(hdc, font_info, -1, &rect, DT_CENTER | DT_SINGLELINE);
            } else {
                DrawText(hdc, "Current Font: System Default", -1, &rect, DT_CENTER | DT_SINGLELINE);
            }
            
            // Display instruction text at bottom
            SetTextColor(hdc, PIXEL_blue);
            rect.top = rect.bottom - 30;
            rect.bottom = rect.bottom;
            DrawText(hdc, "SPACE: next text | F: next font | ESC/Q: quit", -1, &rect, 
                    DT_CENTER | DT_SINGLELINE);
            
            EndPaint(hWnd, hdc);
            break;

        case MSG_KEYDOWN:
            switch (wParam) {
                case SCANCODE_SPACE:
                    // Next Korean text
                    current_text++;
                    if (korean_texts[current_text] == NULL) {
                        current_text = 0;
                    }
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
                    
                case SCANCODE_F:
                    // Next font
                    current_font++;
                    if (font_names[current_font] == NULL) {
                        current_font = 0;
                    }
                    printf("Font switched to: %s (Index: %d)\n", font_names[current_font], current_font);
                    InvalidateRect(hWnd, NULL, TRUE);
                    break;
                    
                case SCANCODE_ESCAPE:
                case SCANCODE_Q:
                    // Quit application
                    PostMessage(hWnd, MSG_CLOSE, 0, 0);
                    break;
            }
            break;

        case MSG_CLOSE:
            // Clean up all Korean fonts before closing
            if (korean_font_regular) {
                DestroyLogFont(korean_font_regular);
                korean_font_regular = NULL;
            }
            if (korean_font_bold) {
                DestroyLogFont(korean_font_bold);
                korean_font_bold = NULL;
            }
            if (korean_font_extrabold) {
                DestroyLogFont(korean_font_extrabold);
                korean_font_extrabold = NULL;
            }
            DestroyMainWindow(hWnd);
            PostQuitMessage(hWnd);
            return 0;
    }

    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

int MiniGUIMain(int args, const char* arg[])
{
    MSG Msg;
    MAINWINCREATE CreateInfo;
    HWND hMainWnd;

    // Set locale for UTF-8 support
    setlocale(LC_ALL, "ko_KR.UTF-8");
    printf("Locale set to: %s\n", setlocale(LC_ALL, NULL));
    
    // Create all three Korean fonts from install/share/fonts
    printf("Creating Korean fonts using fonts from install/share/fonts...\n");
    
    // Load NanumGothic-Regular
    korean_font_regular = CreateLogFont("ttf", "NanumGothic-Regular", "UTF-8", 
                                        FONT_WEIGHT_NORMAL, FONT_SLANT_ROMAN, FONT_FLIP_NONE,
                                        FONT_OTHER_NONE, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                                        16, 0);
    if (korean_font_regular) {
        printf("✓ Loaded NanumGothic-Regular successfully\n");
    } else {
        printf("✗ Failed to load NanumGothic-Regular\n");
    }
    
    // Load NanumGothic-Bold
    korean_font_bold = CreateLogFont("ttf", "NanumGothic-Bold", "UTF-8", 
                                     FONT_WEIGHT_NORMAL, FONT_SLANT_ROMAN, FONT_FLIP_NONE,
                                     FONT_OTHER_NONE, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                                     16, 0);
    if (korean_font_bold) {
        printf("✓ Loaded NanumGothic-Bold successfully\n");
    } else {
        printf("✗ Failed to load NanumGothic-Bold\n");
    }
    
    // Load NanumGothic-ExtraBold
    korean_font_extrabold = CreateLogFont("ttf", "NanumGothic-ExtraBold", "UTF-8", 
                                          FONT_WEIGHT_NORMAL, FONT_SLANT_ROMAN, FONT_FLIP_NONE,
                                          FONT_OTHER_NONE, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                                          16, 0);
    if (korean_font_extrabold) {
        printf("✓ Loaded NanumGothic-ExtraBold successfully\n");
    } else {
        printf("✗ Failed to load NanumGothic-ExtraBold\n");
    }
    
    // Check if at least one font loaded successfully
    if (!korean_font_regular && !korean_font_bold && !korean_font_extrabold) {
        printf("✗ Failed to load any Korean fonts!\n");
    } else {
        printf("✓ Korean font loading completed. Available fonts: Regular=%s, Bold=%s, ExtraBold=%s\n",
               korean_font_regular ? "Yes" : "No",
               korean_font_bold ? "Yes" : "No", 
               korean_font_extrabold ? "Yes" : "No");
    }

#ifdef _MGRM_PROCESSES
    JoinLayer(NAME_DEF_LAYER, arg[0], 0, 0);
#endif

    // Create window
    CreateInfo.dwStyle = WS_CAPTION | WS_VISIBLE | WS_BORDER;
    CreateInfo.dwExStyle = 0;
    CreateInfo.spCaption = "Korean Text Display";
    CreateInfo.hMenu = 0;
    CreateInfo.hCursor = GetSystemCursor(IDC_ARROW);
    CreateInfo.hIcon = 0;
    CreateInfo.MainWindowProc = KoreanWinProc;
    CreateInfo.lx = 100;
    CreateInfo.ty = 100;
    CreateInfo.rx = 600;
    CreateInfo.by = 400;
    CreateInfo.iBkColor = PIXEL_lightwhite;
    CreateInfo.dwAddData = 0;
    CreateInfo.hHosting = HWND_DESKTOP;

    hMainWnd = CreateMainWindow(&CreateInfo);
    if (hMainWnd == HWND_INVALID) {
        printf("✗ Failed to create main window\n");
        return -1;
    }
    printf("✓ Main window created successfully\n");

    ShowWindow(hMainWnd, SW_SHOWNORMAL);
    printf("✓ Main window shown\n");

    while (GetMessage(&Msg, hMainWnd)) {
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup(hMainWnd);
    return 0;
}
