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

// Global variables to store created logical fonts
static PLOGFONT noto_sans_cjk_regular_logfont = NULL;
static PLOGFONT noto_sans_cjk_bold_logfont = NULL;
static PLOGFONT noto_serif_cjk_regular_logfont = NULL;




// Font names for display - Korean TTF fonts
static const char* font_names[] = {
    "NanumGothic-Regular.ttf (Korean TTF)",
    "NanumGothic-Bold.ttf (Korean TTF Bold)",
    "NanumGothic-ExtraBold.ttf (Korean TTF ExtraBold)",
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
            
            // Use pre-created logical fonts from main
            PLOGFONT selected_font = NULL;
            
            if (current_font == 0) {
                // Use pre-created NotoSansCJK-Regular logical font
                if (noto_sans_cjk_regular_logfont) {
                    selected_font = noto_sans_cjk_regular_logfont;
                    printf("✓ Using pre-created NotoSansCJK-Regular logical font\n");
                } else {
                    printf("✗ Pre-created NotoSansCJK-Regular not available, using system font 5\n");
                    selected_font = GetSystemFont(5);
                }
            } else if (current_font == 1) {
                // Use pre-created NotoSansCJK-Bold logical font
                if (noto_sans_cjk_bold_logfont) {
                    selected_font = noto_sans_cjk_bold_logfont;
                    printf("✓ Using pre-created NotoSansCJK-Bold logical font\n");
                } else {
                    printf("✗ Pre-created NotoSansCJK-Bold not available, using system font 5\n");
                    selected_font = GetSystemFont(5);
                }
            } else {
                // Use pre-created NotoSerifCJK-Regular logical font
                if (noto_serif_cjk_regular_logfont) {
                    selected_font = noto_serif_cjk_regular_logfont;
                    printf("✓ Using pre-created NotoSerifCJK-Regular logical font\n");
                } else {
                    printf("✗ Pre-created NotoSerifCJK-Regular not available, using system font 5\n");
                    selected_font = GetSystemFont(5);
                }
            }
            
            // Apply the selected font
            if (selected_font) {
                SelectFont(hdc, selected_font);
                printf("✓ Font selected successfully\n");
            } else {
                // Ultimate fallback
                SelectFont(hdc, GetSystemFont(0));
                printf("Using ultimate fallback system font 0\n");
            }
            
            // Display current Korean text using direct UTF-8
            if (korean_texts[current_text]) {
                SetTextColor(hdc, PIXEL_black);
                
                // Debug: Print to console
                printf("Displaying Korean text: %s\n", korean_texts[current_text]);
                
                // First, draw some visible rectangles to ensure the window is working
                SetBkColor(hdc, PIXEL_red);
                FillBox(hdc, 10, 10, 100, 30);
                SetBkColor(hdc, PIXEL_green);
                FillBox(hdc, 120, 10, 100, 30);
                SetBkColor(hdc, PIXEL_blue);
                FillBox(hdc, 230, 10, 100, 30);
                
                // Try very simple text rendering with high contrast
                SetBkColor(hdc, PIXEL_black);
                SetTextColor(hdc, PIXEL_lightwhite);
                printf("Rendering text: TEST, Hello, World\n");
                TextOut(hdc, 20, 50, "TEST");
                TextOut(hdc, 20, 80, "Hello");
                TextOut(hdc, 20, 110, "World");
                
                // Try Korean text with high contrast
                SetBkColor(hdc, PIXEL_yellow);
                SetTextColor(hdc, PIXEL_black);
                TextOut(hdc, 20, 140, "안녕하세요");
                TextOut(hdc, 20, 170, "한국어");
                TextOut(hdc, 20, 200, "테스트");
                
                // Try with system font
                SelectFont(hdc, GetSystemFont(0));
                SetBkColor(hdc, PIXEL_lightgray);
                SetTextColor(hdc, PIXEL_black);
                TextOut(hdc, 20, 230, "System Font Test");
                TextOut(hdc, 20, 260, "안녕하세요");
                
                // Try with different font sizes and positions
                SetBkColor(hdc, PIXEL_lightwhite);
                SetTextColor(hdc, PIXEL_red);
                TextOut(hdc, 20, 290, korean_texts[current_text]);
                
                // Add more visible elements
                SetBkColor(hdc, PIXEL_lightgray);
                FillBox(hdc, 10, 250, 200, 20);
                SetTextColor(hdc, PIXEL_black);
                TextOut(hdc, 20, 255, "Status: Korean Font Test");
                
                // Display Korean text with different approaches
                SetTextColor(hdc, PIXEL_darkblue);
                TextOutLen(hdc, 50, 230, "안녕하세요", strlen("안녕하세요"));
                TextOutLen(hdc, 50, 260, "한국어", strlen("한국어"));
                TextOutLen(hdc, 50, 290, "테스트", strlen("테스트"));
                
                // Draw some rectangles to make sure the window is visible
                SetBkColor(hdc, PIXEL_red);
                FillBox(hdc, 10, 320, 50, 20);
                SetBkColor(hdc, PIXEL_green);
                FillBox(hdc, 70, 320, 50, 20);
                SetBkColor(hdc, PIXEL_blue);
                FillBox(hdc, 130, 320, 50, 20);
            }
            
            // Display font information
            SetTextColor(hdc, PIXEL_darkblue);
            rect.top = rect.bottom - 80;
            rect.bottom = rect.bottom - 50;
            if (font_names[current_font]) {
                char font_info[256];
                snprintf(font_info, sizeof(font_info), "Current Font: %s", font_names[current_font]);
                DrawText(hdc, font_info, -1, &rect, DT_CENTER | DT_SINGLELINE);
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
    
    // Create Korean fonts using system fonts and file paths
    printf("Creating Korean fonts...\n");
    
    // Get current working directory for font paths
    char cwd[512];
    getcwd(cwd, sizeof(cwd));
    
    // Try system font names first
    noto_sans_cjk_regular_logfont = CreateLogFont("ttf", "NanumGothic", "UTF-8", 
        FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_FLIP_NONE, 
        FONT_OTHER_AUTOSCALE, FONT_DECORATE_NONE, FONT_RENDER_GREY, 24, 0);
    if (noto_sans_cjk_regular_logfont) {
        printf("✓ NanumGothic system font created successfully\n");
    } else {
        printf("✗ Failed to create NanumGothic system font\n");
        
        // Try with absolute path as fallback
        char font_path_regular[512];
        snprintf(font_path_regular, sizeof(font_path_regular), "%s/assets/fonts/NanumGothic-Regular.ttf", cwd);
        noto_sans_cjk_regular_logfont = CreateLogFont("ttf", font_path_regular, "UTF-8", 
            FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_FLIP_NONE, 
            FONT_OTHER_AUTOSCALE, FONT_DECORATE_NONE, FONT_RENDER_GREY, 24, 0);
        if (noto_sans_cjk_regular_logfont) {
            printf("✓ NanumGothic-Regular logical font created successfully from %s\n", font_path_regular);
        } else {
            printf("✗ Failed to create NanumGothic-Regular logical font from %s\n", font_path_regular);
        }
    }
    
    // Try to create bold font
    noto_sans_cjk_bold_logfont = CreateLogFont("ttf", "NanumGothic", "UTF-8", 
        FONT_WEIGHT_BOLD, FONT_SLANT_ROMAN, FONT_FLIP_NONE, 
        FONT_OTHER_AUTOSCALE, FONT_DECORATE_NONE, FONT_RENDER_GREY, 24, 0);
    if (noto_sans_cjk_bold_logfont) {
        printf("✓ NanumGothic Bold system font created successfully\n");
    } else {
        printf("✗ Failed to create NanumGothic Bold system font\n");
    }
    
    // Try to create extra bold font
    noto_serif_cjk_regular_logfont = CreateLogFont("ttf", "NanumGothic", "UTF-8", 
        FONT_WEIGHT_BOLD, FONT_SLANT_ROMAN, FONT_FLIP_NONE, 
        FONT_OTHER_AUTOSCALE, FONT_DECORATE_NONE, FONT_RENDER_GREY, 24, 0);
    if (noto_serif_cjk_regular_logfont) {
        printf("✓ NanumGothic ExtraBold system font created successfully\n");
    } else {
        printf("✗ Failed to create NanumGothic ExtraBold system font\n");
    }
    
    printf("Korean font creation completed.\n");

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
