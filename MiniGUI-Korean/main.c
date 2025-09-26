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

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

// Korean text samples
static const char* korean_texts[] = {
    "안녕하세요! Hello in Korean",
    "한국어 텍스트 표시 프로그램", 
    "MiniGUI로 한국어를 표시합니다",
    "가나다라마바사아자차카타파하",
    "한글 입력 시스템 테스트",
    "다운로드된 한국어 폰트 테스트",
    "NanumGothic 폰트로 표시",
    "NotoSansKR 폰트로 표시",
    NULL
};

static int current_text = 0;
static int current_font = 0;

// Function to ensure UTF-8 encoding for Korean text
static const char* ensure_utf8_korean(const char* text) {
    // Check if the text is already in UTF-8 format
    // Korean characters in UTF-8 should start with 0xEC, 0xED, 0xEA, etc.
    if (text && strlen(text) > 0) {
        // For now, just return the text as-is since it's already in UTF-8
        // In a real application, you might want to validate UTF-8 encoding
        return text;
    }
    return text;
}

// Function to draw Korean text with proper UTF-8 handling
static void draw_korean_text(HDC hdc, int x, int y, const char* utf8_text) {
    if (!utf8_text) return;
    
    // Convert UTF-8 to wide character string using standard C library
    wchar_t wstr[256];
    size_t wlen = mbstowcs(wstr, utf8_text, 256);
    
    if (wlen > 0 && wlen < 256) {
        printf("Drawing Korean text: %s (UTF-8 length: %zu, Wide length: %zu)\n", 
               utf8_text, strlen(utf8_text), wlen);
        
        // Method 1: Use TextOut with UTF-8 text
        TextOut(hdc, x, y, utf8_text);
        
        // Method 2: Use TextOutLen with UTF-8 length
        TextOutLen(hdc, x, y + 20, utf8_text, strlen(utf8_text));
        
        // Method 3: Try with different background
        SetBkColor(hdc, PIXEL_lightgray);
        TextOut(hdc, x, y + 40, utf8_text);
        SetBkColor(hdc, PIXEL_lightwhite);
        
        // Method 4: Try with different text color
        SetTextColor(hdc, PIXEL_blue);
        TextOut(hdc, x, y + 60, utf8_text);
        SetTextColor(hdc, PIXEL_black);
    } else {
        // Fallback to regular TextOut if conversion fails
        printf("Wide char conversion failed, using regular TextOut\n");
        TextOut(hdc, x, y, utf8_text);
    }
}

// Font names for display
static const char* font_names[] = {
    "NanumGothic.ttf (Local Korean)",
    "NanumMyeongjo.ttf (Local Korean)",
    "NotoSansKR-Bold.otf (Local Korean)",
    "System Font 1 (Courier)",
    "System Font 2 (SansSerif)",
    NULL
};

static LRESULT KoreanWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    RECT rect;

    switch (message) {
        case MSG_PAINT:
            hdc = BeginPaint(hWnd);
            GetClientRect(hWnd, &rect);
            
            // Set background color
            SetBkColor(hdc, PIXEL_lightwhite);
            FillBox(hdc, 0, 0, RECTW(rect), RECTH(rect));
            
            // Use device fonts loaded at startup
            if (current_font == 0) {
                // Use NanumGothic device font
                PLOGFONT nanum_gothic_logfont = CreateLogFont("ttf", "NanumGothic", "UTF-8", 
                    FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_FLIP_NONE, 
                    FONT_OTHER_AUTOSCALE, FONT_DECORATE_NONE, FONT_RENDER_GREY, 24, 0);
                if (nanum_gothic_logfont) {
                    SelectFont(hdc, nanum_gothic_logfont);
                    printf("Using NanumGothic device font (loaded at startup)\n");
                } else {
                    SelectFont(hdc, GetSystemFont(5));
                    printf("NanumGothic device font failed, using system font 5\n");
                }
            } else if (current_font == 1) {
                // Use NanumMyeongjo device font
                PLOGFONT nanum_myeongjo_logfont = CreateLogFont("ttf", "NanumMyeongjo", "UTF-8", 
                    FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, FONT_FLIP_NONE, 
                    FONT_OTHER_AUTOSCALE, FONT_DECORATE_NONE, FONT_RENDER_GREY, 24, 0);
                if (nanum_myeongjo_logfont) {
                    SelectFont(hdc, nanum_myeongjo_logfont);
                    printf("Using NanumMyeongjo device font (loaded at startup)\n");
                } else {
                    SelectFont(hdc, GetSystemFont(5));
                    printf("NanumMyeongjo device font failed, using system font 5\n");
                }
            } else if (current_font == 2) {
                // Use NotoSansKR-Bold device font
                PLOGFONT noto_sans_kr_logfont = CreateLogFont("ttf", "NotoSansKR-Bold", "UTF-8", 
                    FONT_WEIGHT_BOLD, FONT_SLANT_ROMAN, FONT_FLIP_NONE, 
                    FONT_OTHER_AUTOSCALE, FONT_DECORATE_NONE, FONT_RENDER_GREY, 24, 0);
                if (noto_sans_kr_logfont) {
                    SelectFont(hdc, noto_sans_kr_logfont);
                    printf("Using NotoSansKR-Bold device font (loaded at startup)\n");
                } else {
                    SelectFont(hdc, GetSystemFont(0));
                    printf("NotoSansKR-Bold device font failed, using system font 0\n");
                }
            } else if (current_font == 3) {
                // Use system font 1 (Courier)
                SelectFont(hdc, GetSystemFont(1));
                printf("Using system font 1 (Courier)\n");
            } else {
                // Use system font 2 (SansSerif)
                SelectFont(hdc, GetSystemFont(2));
                printf("Using system font 2 (SansSerif)\n");
            }
            
            // Display current Korean text
            if (korean_texts[current_text]) {
                SetTextColor(hdc, PIXEL_black);
                
                // Ensure UTF-8 encoding for Korean text
                const char* utf8_korean_text = ensure_utf8_korean(korean_texts[current_text]);
                
                // Debug: Print to console
                printf("Displaying Korean text: %s\n", utf8_korean_text);
                
                // First, draw a background rectangle to make sure we can see the text area
                SetBkColor(hdc, PIXEL_lightgray);
                FillBox(hdc, 10, 10, 580, 50);
                
                // Display Korean text in center using DrawText
                DrawText(hdc, utf8_korean_text, -1, &rect, 
                        DT_CENTER | DT_VCENTER | DT_SINGLELINE);
                
                // Try TextOut with explicit length
                SetBkColor(hdc, PIXEL_lightwhite);
                TextOutLen(hdc, 50, 100, utf8_korean_text, strlen(utf8_korean_text));
                TextOutLen(hdc, 50, 130, "안녕하세요", strlen("안녕하세요"));
                TextOutLen(hdc, 50, 160, "한국어 테스트", strlen("한국어 테스트"));
                
                // Try with different font sizes and approaches using the new function
                SetTextColor(hdc, PIXEL_darkgreen);
                draw_korean_text(hdc, 50, 200, "가나다라마바사");
                draw_korean_text(hdc, 50, 280, "아자차카타파하");
                
                // Add English text to verify display is working
                SetTextColor(hdc, PIXEL_red);
                TextOut(hdc, 50, 190, "English Test: Hello World");
                TextOut(hdc, 50, 220, "Font Test: ABCDEFG");
                TextOut(hdc, 50, 250, "Korean Test: Should show Korean below");
                
                // Try Korean text with different approaches using the new function
                SetTextColor(hdc, PIXEL_blue);
                draw_korean_text(hdc, 50, 360, "안녕하세요");
                draw_korean_text(hdc, 50, 420, "한국어");
                draw_korean_text(hdc, 50, 480, "테스트");
                
                // Draw some rectangles to make sure the window is visible
                SetBkColor(hdc, PIXEL_red);
                FillBox(hdc, 10, 400, 50, 20);
                SetBkColor(hdc, PIXEL_green);
                FillBox(hdc, 70, 400, 50, 20);
                SetBkColor(hdc, PIXEL_blue);
                FillBox(hdc, 130, 400, 50, 20);
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
    
    // Verify UTF-8 encoding
    printf("Locale set to: %s\n", setlocale(LC_ALL, NULL));
    
    // Test UTF-8 encoding of Korean text
    const char* test_korean = "안녕하세요";
    printf("Korean text UTF-8 bytes: ");
    for (int i = 0; test_korean[i] != '\0'; i++) {
        printf("%02X ", (unsigned char)test_korean[i]);
    }
    printf("\n");
    
    // Load Korean fonts at startup using proper device font mechanism
    printf("Loading Korean fonts at startup using device font mechanism...\n");
    
    // Load NanumGothic font using LoadDevFontFromFile
    DEVFONT* nanum_gothic_devfont = LoadDevFontFromFile("NanumGothic", "./assets/fonts/NanumGothic.ttf");
    if (nanum_gothic_devfont) {
        printf("✓ NanumGothic device font loaded successfully\n");
    } else {
        printf("✗ Failed to load NanumGothic device font\n");
    }
    
    // Load NanumMyeongjo font using LoadDevFontFromFile
    DEVFONT* nanum_myeongjo_devfont = LoadDevFontFromFile("NanumMyeongjo", "./assets/fonts/NanumMyeongjo.ttf");
    if (nanum_myeongjo_devfont) {
        printf("✓ NanumMyeongjo device font loaded successfully\n");
    } else {
        printf("✗ Failed to load NanumMyeongjo device font\n");
    }
    
    // Load NotoSansKR-Bold font using LoadDevFontFromFile
    DEVFONT* noto_sans_kr_devfont = LoadDevFontFromFile("NotoSansKR-Bold", "./assets/fonts/NotoSansKR-Bold.otf");
    if (noto_sans_kr_devfont) {
        printf("✓ NotoSansKR-Bold device font loaded successfully\n");
    } else {
        printf("✗ Failed to load NotoSansKR-Bold device font\n");
    }
    
    printf("Korean device font loading completed.\n");

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
        return -1;
    }

    ShowWindow(hMainWnd, SW_SHOWNORMAL);

    while (GetMessage(&Msg, hMainWnd)) {
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup(hMainWnd);
    return 0;
}
