#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <unistd.h>
#include <signal.h>
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/ctrl/edit.h>
#include <minigui/ctrl/static.h>
#include "qwerty_korean.h"

// Control IDs
#define IDC_TEXTBOX    101
#define IDC_KEY_Q      201
#define IDC_KEY_W      202
#define IDC_KEY_E      203
#define IDC_KEY_R      204
#define IDC_KEY_T      205
#define IDC_KEY_Y      206
#define IDC_KEY_U      207
#define IDC_KEY_I      208
#define IDC_KEY_O      209
#define IDC_KEY_P      210
#define IDC_KEY_A      211
#define IDC_KEY_S      212
#define IDC_KEY_D      213
#define IDC_KEY_F      214
#define IDC_KEY_G      215
#define IDC_KEY_H      216
#define IDC_KEY_J      217
#define IDC_KEY_K      218
#define IDC_KEY_L      219
#define IDC_KEY_Z      220
#define IDC_KEY_X      221
#define IDC_KEY_C      222
#define IDC_KEY_V      223
#define IDC_KEY_B      224
#define IDC_KEY_N      225
#define IDC_KEY_M      226
#define IDC_KEY_SHIFT  227
#define IDC_KEY_SPACE  228
#define IDC_KEY_BACK   229
#define IDC_KEY_ENTER  230

// Global variables
static HWND hMainWnd;
static HWND hTextBox;
static HWND hKeyButtons[30];
static char input_buffer[MAX_OUTPUT_LEN] = {0};
static wchar_t output_buffer[MAX_OUTPUT_LEN] = {0};
static size_t input_len = 0;
static PLOGFONT korean_font = NULL;
static BOOL shift_pressed = FALSE;


// Signal handler for cleanup
void cleanup_handler(int sig) {
    printf("Received signal %d, cleaning up...\n", sig);
    
    // Clean up Korean input system
    qwerty_korean_cleanup();
    
    // Clean up Korean font if loaded
    if (korean_font) {
        DestroyLogFont(korean_font);
        korean_font = NULL;
    }
    
    exit(0);
}

// Function to update text box with Korean output
static void update_textbox() {
    // Display Korean output buffer contents
    char utf8_output[MAX_OUTPUT_LEN * 4] = {0};
    unicode_to_utf8(output_buffer, utf8_output, sizeof(utf8_output));

    // Manually wrap text by adding newlines when needed
    char wrapped_output[MAX_OUTPUT_LEN * 4] = {0};
    int line_len = 0;
    int max_line_chars = 40; // Approximate characters per line (adjust based on font size)
    int out_idx = 0;

    for (int i = 0; i < strlen(utf8_output); i++) {
        wrapped_output[out_idx++] = utf8_output[i];

        // Count characters (handle UTF-8 multi-byte characters)
        if ((utf8_output[i] & 0xC0) != 0x80) {  // Not a UTF-8 continuation byte
            line_len++;
        }

        // Add newline if line is too long
        if (line_len >= max_line_chars) {
            wrapped_output[out_idx++] = '\n';
            line_len = 0;
        }
    }
    wrapped_output[out_idx] = '\0';

    // Update static control text with wrapped Korean output
    SetWindowText(hTextBox, wrapped_output);

    // Force textbox to refresh
    InvalidateRect(hTextBox, NULL, TRUE);
    UpdateWindow(hTextBox, TRUE);

    // Force main window repaint to show Korean text on main window too
    InvalidateRect(hMainWnd, NULL, TRUE);
    UpdateWindow(hMainWnd, TRUE);
}

// Function to update button labels based on shift state
static void update_button_labels() {
    // Korean characters for buttons - normal state
    const char* korean_labels_normal[] = {"ㅂ", "ㅈ", "ㄷ", "ㄱ", "ㅅ", "ㅛ", "ㅕ", "ㅑ", "ㅐ", "ㅔ",
                                   "ㅁ", "ㄴ", "ㅇ", "ㄹ", "ㅎ", "ㅗ", "ㅓ", "ㅏ", "ㅣ",
                                   "ㅋ", "ㅌ", "ㅊ", "ㅍ", "ㅠ", "ㅜ", "ㅡ"};

    // Korean characters for buttons - shift state (strong consonants)
    const char* korean_labels_shifted[] = {"ㅃ", "ㅉ", "ㄸ", "ㄲ", "ㅆ", "ㅛ", "ㅕ", "ㅑ", "ㅒ", "ㅖ",
                                    "ㅁ", "ㄴ", "ㅇ", "ㄹ", "ㅎ", "ㅗ", "ㅓ", "ㅏ", "ㅣ",
                                    "ㅋ", "ㅌ", "ㅊ", "ㅍ", "ㅠ", "ㅜ", "ㅡ"};
    
    // Choose the appropriate labels based on shift state
    const char** korean_labels = shift_pressed ? korean_labels_shifted : korean_labels_normal;
    
    // Update Korean character buttons (buttons 0-25)
    for (int i = 0; i < 26; i++) {
        if (hKeyButtons[i]) {
            SetWindowText(hKeyButtons[i], korean_labels[i]);
            // Force button to redraw with new text
            InvalidateRect(hKeyButtons[i], NULL, TRUE);
            UpdateWindow(hKeyButtons[i], TRUE);
        }
    }
}

// Function to handle key button clicks
static void handle_key_click(int key_id) {
    char key_char = 0;

    // Handle shift key toggle
    if (key_id == IDC_KEY_SHIFT) {
        shift_pressed = !shift_pressed;
        printf("Shift %s\n", shift_pressed ? "pressed" : "released");
        update_button_labels();  // Update button labels to show current case
        
        // Change shift button color to show toggle state
        if (hKeyButtons[26]) {  // Shift button is at index 26
            if (shift_pressed) {
                // Set pressed color (darker background to show it's active)
                SetWindowBkColor(hKeyButtons[26], PIXEL_darkgray);
            } else {
                // Set normal color (default button color)
                SetWindowBkColor(hKeyButtons[26], PIXEL_lightwhite);
            }
            // Force button to redraw with new color
            InvalidateRect(hKeyButtons[26], NULL, TRUE);
            UpdateWindow(hKeyButtons[26], TRUE);
        }
        return;
    }
    
    // Only some characters are changed for ㅃ, ㅉ, ㄸ, ㄲ, ㅆ and ㅒ, ㅖ.
    switch (key_id) {
        case IDC_KEY_Q: key_char = shift_pressed ? 'Q' : 'q'; break; // Here is the changes.
        case IDC_KEY_W: key_char = shift_pressed ? 'W' : 'w'; break; // Here is the changes.
        case IDC_KEY_E: key_char = shift_pressed ? 'E' : 'e'; break; // Here is the changes.
        case IDC_KEY_R: key_char = shift_pressed ? 'R' : 'r'; break; // Here is the changes.
        case IDC_KEY_T: key_char = shift_pressed ? 'T' : 't'; break; // Here is the changes.
        case IDC_KEY_Y: key_char = shift_pressed ? 'Y' : 'y'; break; // Here is the changes.
        case IDC_KEY_U: key_char = shift_pressed ? 'u' : 'u'; break;
        case IDC_KEY_I: key_char = shift_pressed ? 'i' : 'i'; break;
        case IDC_KEY_O: key_char = shift_pressed ? 'O' : 'o'; break; // Here is the changes.
        case IDC_KEY_P: key_char = shift_pressed ? 'P' : 'p'; break; // Here is the changes.
        case IDC_KEY_A: key_char = shift_pressed ? 'a' : 'a'; break;
        case IDC_KEY_S: key_char = shift_pressed ? 's' : 's'; break;
        case IDC_KEY_D: key_char = shift_pressed ? 'd' : 'd'; break;
        case IDC_KEY_F: key_char = shift_pressed ? 'f' : 'f'; break;
        case IDC_KEY_G: key_char = shift_pressed ? 'g' : 'g'; break;
        case IDC_KEY_H: key_char = shift_pressed ? 'h' : 'h'; break;
        case IDC_KEY_J: key_char = shift_pressed ? 'j' : 'j'; break;
        case IDC_KEY_K: key_char = shift_pressed ? 'k' : 'k'; break;
        case IDC_KEY_L: key_char = shift_pressed ? 'l' : 'l'; break;
        case IDC_KEY_Z: key_char = shift_pressed ? 'z' : 'z'; break;
        case IDC_KEY_X: key_char = shift_pressed ? 'x' : 'x'; break;
        case IDC_KEY_C: key_char = shift_pressed ? 'c' : 'c'; break;
        case IDC_KEY_V: key_char = shift_pressed ? 'v' : 'v'; break;
        case IDC_KEY_B: key_char = shift_pressed ? 'b' : 'b'; break;
        case IDC_KEY_N: key_char = shift_pressed ? 'n' : 'n'; break;
        case IDC_KEY_M: key_char = shift_pressed ? 'm' : 'm'; break;
        case IDC_KEY_SPACE:
            printf("Space key pressed!\n");
            qwerty_process_input(input_buffer, &input_len, output_buffer, ' ');
            update_textbox();
            return;
        case IDC_KEY_BACK:
            qwerty_process_input(input_buffer, &input_len, output_buffer, 0x7f);
            update_textbox();
            return;
        case IDC_KEY_ENTER:
            qwerty_process_input(input_buffer, &input_len, output_buffer, '\n');
            update_textbox();
            return;
    }

    if (key_char != 0) {
        qwerty_process_input(input_buffer, &input_len, output_buffer, key_char);
        update_textbox();
    }
}

static LRESULT KoreanInputWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    
    switch (message) {
        case MSG_CREATE:
            // Load Korean font from MiniGUI font directory (copied from assets)
            korean_font = CreateLogFont("ttf", "NanumGothic-Regular", "UTF-8", 
                                        FONT_WEIGHT_NORMAL, FONT_SLANT_ROMAN, FONT_FLIP_NONE,
                                        FONT_OTHER_NONE, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                                        16, 0);
            if (korean_font == NULL) {
                // Try with Bold variant
                korean_font = CreateLogFont("ttf", "NanumGothic-Bold", "UTF-8", 
                                            FONT_WEIGHT_NORMAL, FONT_SLANT_ROMAN, FONT_FLIP_NONE,
                                            FONT_OTHER_NONE, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                                            16, 0);
            }
            if (korean_font == NULL) {
                // Try with ExtraBold variant
                korean_font = CreateLogFont("ttf", "NanumGothic-ExtraBold", "UTF-8", 
                                            FONT_WEIGHT_NORMAL, FONT_SLANT_ROMAN, FONT_FLIP_NONE,
                                            FONT_OTHER_NONE, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                                            16, 0);
            }

            if (korean_font == NULL) {
                // Final fallback to default font
                korean_font = NULL;
                printf("Using MiniGUI default font (Korean characters may not display correctly)\n");
            } else {
                printf("Loaded Korean font from MiniGUI font directory for Korean character support\n");
                printf("Font family: %s, charset: %s, size: %d\n", 
                       korean_font->family, korean_font->charset, korean_font->size);
            }
            
            // Create text box - use static control with left alignment for multiline text display
            hTextBox = CreateWindow("static", "",
                WS_VISIBLE | WS_CHILD | WS_BORDER | SS_NOTIFY | SS_LEFT,
                IDC_TEXTBOX,
                20, 20, 550, 120,
                hWnd, 0);

            // Set background color to white for better visibility
            SetWindowBkColor(hTextBox, PIXEL_lightwhite);

            // Apply Korean font to text box for Korean character display
            if (korean_font) {
                SetWindowFont(hTextBox, korean_font);
                // Force text box to redraw with new font
                InvalidateRect(hTextBox, NULL, TRUE);
                UpdateWindow(hTextBox, TRUE);
                printf("Applied Korean font to text box\n");
            }
            printf("Created text box for Korean output display\n");
            
            int first_row_start = 100;
            // First row: Q(ㅃ) W(ㅉ) E(ㄸ) R(ㄲ) T(ㅆ) Y(ㅛ) U(ㅕ) I(ㅑ) O(ㅐ) P(ㅖ)
            hKeyButtons[0] = CreateWindow("button", "ㅂ", WS_VISIBLE | WS_CHILD, IDC_KEY_Q, first_row_start, 140, 35, 35, hWnd, 0);
            hKeyButtons[1] = CreateWindow("button", "ㅈ", WS_VISIBLE | WS_CHILD, IDC_KEY_W, first_row_start + 40, 140, 35, 35, hWnd, 0);
            hKeyButtons[2] = CreateWindow("button", "ㄷ", WS_VISIBLE | WS_CHILD, IDC_KEY_E, first_row_start + 80, 140, 35, 35, hWnd, 0);
            hKeyButtons[3] = CreateWindow("button", "ㄱ", WS_VISIBLE | WS_CHILD, IDC_KEY_R, first_row_start + 120, 140, 35, 35, hWnd, 0);
            hKeyButtons[4] = CreateWindow("button", "ㅅ", WS_VISIBLE | WS_CHILD, IDC_KEY_T, first_row_start + 160, 140, 35, 35, hWnd, 0);
            hKeyButtons[5] = CreateWindow("button", "ㅛ", WS_VISIBLE | WS_CHILD, IDC_KEY_Y, first_row_start + 200, 140, 35, 35, hWnd, 0);
            hKeyButtons[6] = CreateWindow("button", "ㅕ", WS_VISIBLE | WS_CHILD, IDC_KEY_U, first_row_start + 240, 140, 35, 35, hWnd, 0);
            hKeyButtons[7] = CreateWindow("button", "ㅑ", WS_VISIBLE | WS_CHILD, IDC_KEY_I, first_row_start + 280, 140, 35, 35, hWnd, 0);
            hKeyButtons[8] = CreateWindow("button", "ㅐ", WS_VISIBLE | WS_CHILD, IDC_KEY_O, first_row_start + 320, 140, 35, 35, hWnd, 0);
            hKeyButtons[9] = CreateWindow("button", "ㅔ", WS_VISIBLE | WS_CHILD, IDC_KEY_P, first_row_start + 360, 140, 35, 35, hWnd, 0);
        
            // Second row: A(ㅁ) S(ㄴ) D(ㅇ) F(ㄹ) G(ㅎ) H(ㅗ) J(ㅓ) K(ㅏ) L(ㅣ)
            // Move to right side: start at 170 (slightly indented for QWERTY layout)
            int second_row_start = 120;
            // Second row: A(ㅁ) S(ㄴ) D(ㅇ) F(ㄹ) G(ㅎ) H(ㅗ) J(ㅓ) K(ㅏ) L(ㅣ)
            hKeyButtons[10] = CreateWindow("button", "ㅁ", WS_VISIBLE | WS_CHILD, IDC_KEY_A, second_row_start, 185, 35, 35, hWnd, 0);
            hKeyButtons[11] = CreateWindow("button", "ㄴ", WS_VISIBLE | WS_CHILD, IDC_KEY_S, second_row_start + 40, 185, 35, 35, hWnd, 0);
            hKeyButtons[12] = CreateWindow("button", "ㅇ", WS_VISIBLE | WS_CHILD, IDC_KEY_D, second_row_start + 80, 185, 35, 35, hWnd, 0);
            hKeyButtons[13] = CreateWindow("button", "ㄹ", WS_VISIBLE | WS_CHILD, IDC_KEY_F, second_row_start + 120, 185, 35, 35, hWnd, 0);
            hKeyButtons[14] = CreateWindow("button", "ㅎ", WS_VISIBLE | WS_CHILD, IDC_KEY_G, second_row_start + 160, 185, 35, 35, hWnd, 0);
            hKeyButtons[15] = CreateWindow("button", "ㅗ", WS_VISIBLE | WS_CHILD, IDC_KEY_H, second_row_start + 200, 185, 35, 35, hWnd, 0);
            hKeyButtons[16] = CreateWindow("button", "ㅓ", WS_VISIBLE | WS_CHILD, IDC_KEY_J, second_row_start + 240, 185, 35, 35, hWnd, 0);
            hKeyButtons[17] = CreateWindow("button", "ㅏ", WS_VISIBLE | WS_CHILD, IDC_KEY_K, second_row_start + 280, 185, 35, 35, hWnd, 0);
            hKeyButtons[18] = CreateWindow("button", "ㅣ", WS_VISIBLE | WS_CHILD, IDC_KEY_L, second_row_start + 320, 185, 35, 35, hWnd, 0);
            
            
            // Third row: Z(ㅋ) X(ㅌ) C(ㅊ) V(ㅍ) B(ㅠ) N(ㅜ) M(ㅡ)
            // Move third row further right: start at 220
            int third_row_start = 170;
            // Third row: Z(ㅋ) X(ㅌ) C(ㅊ) V(ㅍ) B(ㅠ) N(ㅜ) M(ㅡ)
            hKeyButtons[19] = CreateWindow("button", "ㅋ", WS_VISIBLE | WS_CHILD, IDC_KEY_Z, third_row_start, 230, 35, 35, hWnd, 0);
            hKeyButtons[20] = CreateWindow("button", "ㅌ", WS_VISIBLE | WS_CHILD, IDC_KEY_X, third_row_start + 40, 230, 35, 35, hWnd, 0);
            hKeyButtons[21] = CreateWindow("button", "ㅊ", WS_VISIBLE | WS_CHILD, IDC_KEY_C, third_row_start + 80, 230, 35, 35, hWnd, 0);
            hKeyButtons[22] = CreateWindow("button", "ㅍ", WS_VISIBLE | WS_CHILD, IDC_KEY_V, third_row_start + 120, 230, 35, 35, hWnd, 0);
            hKeyButtons[23] = CreateWindow("button", "ㅠ", WS_VISIBLE | WS_CHILD, IDC_KEY_B, third_row_start + 160, 230, 35, 35, hWnd, 0);
            hKeyButtons[24] = CreateWindow("button", "ㅜ", WS_VISIBLE | WS_CHILD, IDC_KEY_N, third_row_start + 200, 230, 35, 35, hWnd, 0);
            hKeyButtons[25] = CreateWindow("button", "ㅡ", WS_VISIBLE | WS_CHILD, IDC_KEY_M, third_row_start + 240, 230, 35, 35, hWnd, 0);
            
            int special_keys_start = 155;
            // Shift, Space, Backspace, Enter - moved further left to match button rows
            hKeyButtons[26] = CreateWindow("button", "Shift", WS_VISIBLE | WS_CHILD, IDC_KEY_SHIFT, special_keys_start, 290, 60, 40, hWnd, 0);
            hKeyButtons[27] = CreateWindow("button", "Space", WS_VISIBLE | WS_CHILD, IDC_KEY_SPACE, special_keys_start + 60, 290, 100, 40, hWnd, 0);
            hKeyButtons[28] = CreateWindow("button", "Back", WS_VISIBLE | WS_CHILD, IDC_KEY_BACK, special_keys_start + 160, 290, 60, 40, hWnd, 0);
            hKeyButtons[29] = CreateWindow("button", "Enter", WS_VISIBLE | WS_CHILD, IDC_KEY_ENTER, special_keys_start + 220, 290, 80, 40, hWnd, 0);
            
            // Apply Korean font to all buttons for Korean character display
            if (korean_font) {
                for (int i = 0; i < 30; i++) {
                    if (hKeyButtons[i]) {
                        SetWindowFont(hKeyButtons[i], korean_font);
                        // Force button to redraw with new font
                        InvalidateRect(hKeyButtons[i], NULL, TRUE);
                        UpdateWindow(hKeyButtons[i], TRUE);
                    }
                }
                printf("Applied Korean font to all buttons\n");
            } else {
                printf("Using default system font for buttons\n");
            }
            
            // Initialize Korean input system
            qwerty_korean_init();
            
            return 0;
            
        case MSG_COMMAND:
            handle_key_click(LOWORD(wParam));
            return 0;
            
            
        case MSG_KEYDOWN:
            // Handle keyboard input directly
            switch (wParam) {
                case SCANCODE_Q: handle_key_click(IDC_KEY_Q); break;
                case SCANCODE_W: handle_key_click(IDC_KEY_W); break;
                case SCANCODE_E: handle_key_click(IDC_KEY_E); break;
                case SCANCODE_R: handle_key_click(IDC_KEY_R); break;
                case SCANCODE_T: handle_key_click(IDC_KEY_T); break;
                case SCANCODE_Y: handle_key_click(IDC_KEY_Y); break;
                case SCANCODE_U: handle_key_click(IDC_KEY_U); break;
                case SCANCODE_I: handle_key_click(IDC_KEY_I); break;
                case SCANCODE_O: handle_key_click(IDC_KEY_O); break;
                case SCANCODE_P: handle_key_click(IDC_KEY_P); break;
                case SCANCODE_A: handle_key_click(IDC_KEY_A); break;
                case SCANCODE_S: handle_key_click(IDC_KEY_S); break;
                case SCANCODE_D: handle_key_click(IDC_KEY_D); break;
                case SCANCODE_F: handle_key_click(IDC_KEY_F); break;
                case SCANCODE_G: handle_key_click(IDC_KEY_G); break;
                case SCANCODE_H: handle_key_click(IDC_KEY_H); break;
                case SCANCODE_J: handle_key_click(IDC_KEY_J); break;
                case SCANCODE_K: handle_key_click(IDC_KEY_K); break;
                case SCANCODE_L: handle_key_click(IDC_KEY_L); break;
                case SCANCODE_Z: handle_key_click(IDC_KEY_Z); break;
                case SCANCODE_X: handle_key_click(IDC_KEY_X); break;
                case SCANCODE_C: handle_key_click(IDC_KEY_C); break;
                case SCANCODE_V: handle_key_click(IDC_KEY_V); break;
                case SCANCODE_B: handle_key_click(IDC_KEY_B); break;
                case SCANCODE_N: handle_key_click(IDC_KEY_N); break;
                case SCANCODE_M: handle_key_click(IDC_KEY_M); break;
                case SCANCODE_SPACE: handle_key_click(IDC_KEY_SPACE); break;
                case SCANCODE_BACKSPACE: handle_key_click(IDC_KEY_BACK); break;
                case SCANCODE_ENTER: handle_key_click(IDC_KEY_ENTER); break;
                case SCANCODE_ESCAPE:
                    PostMessage(hWnd, MSG_CLOSE, 0, 0);
                    break;
            }
            return 0;
            
        case MSG_PAINT:
            hdc = BeginPaint(hWnd);
            SetBkMode(hdc, BM_TRANSPARENT);
            TextOut(hdc, 20, 350, "Korean QWERTY Input System");

            // Display current Korean output directly on window using wide characters
            char utf8_output[MAX_OUTPUT_LEN * 4] = {0};
            unicode_to_utf8(output_buffer, utf8_output, sizeof(utf8_output));
            
            if (korean_font) {
                // Use Korean font for display
                PLOGFONT old_font = SelectFont(hdc, korean_font);
                SetTextColor(hdc, PIXEL_black);
                TextOut(hdc, 20, 380, utf8_output);
                if (old_font) SelectFont(hdc, old_font);
            } else {
                // Fallback to default font
                TextOut(hdc, 20, 380, utf8_output);
            }

            EndPaint(hWnd, hdc);
            return 0;
            
        case MSG_CLOSE:
            // Clean up Korean input system
            qwerty_korean_cleanup();
            
            // Clean up Korean font if loaded
            if (korean_font) {
                DestroyLogFont(korean_font);
                korean_font = NULL;
            }
            
            // Clean up child windows (buttons and text box)
            if (hTextBox) {
                DestroyWindow(hTextBox);
                hTextBox = HWND_INVALID;
            }
            
            for (int i = 0; i < 30; i++) {
                if (hKeyButtons[i]) {
                    DestroyWindow(hKeyButtons[i]);
                    hKeyButtons[i] = HWND_INVALID;
                }
            }
            
            // Test button is local variable, no need to clean up here
            
            DestroyMainWindow(hWnd);
            PostQuitMessage(hWnd);
            return 0;
            
        case MSG_DESTROY:
            return 0;
    }
    
    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

static void InitCreateInfo(MAINWINCREATE* pCreateInfo)
{
    pCreateInfo->dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION;
    pCreateInfo->dwExStyle = WS_EX_NONE;
    pCreateInfo->spCaption = "Korean QWERTY Input System";
    pCreateInfo->hMenu = 0;
    pCreateInfo->hCursor = GetSystemCursor(0);
    pCreateInfo->hIcon = 0;
    pCreateInfo->MainWindowProc = KoreanInputWinProc;
    pCreateInfo->lx = 100;
    pCreateInfo->ty = 100;
    pCreateInfo->rx = 700;
    pCreateInfo->by = 500;
    pCreateInfo->iBkColor = COLOR_lightwhite;
    pCreateInfo->dwAddData = 0;
    pCreateInfo->hHosting = HWND_DESKTOP;
}

int MiniGUIMain(int argc, const char* argv[])
{
    MAINWINCREATE CreateInfo;
    MSG msg;
    
    // Register signal handlers for cleanup
    signal(SIGINT, cleanup_handler);
    signal(SIGTERM, cleanup_handler);
    signal(SIGSEGV, cleanup_handler);
    
    // JoinLayer removed for standalone mode
    
    InitCreateInfo(&CreateInfo);
    hMainWnd = CreateMainWindow(&CreateInfo);
    
    if (hMainWnd == HWND_INVALID)
        return -1;
    
    ShowWindow(hMainWnd, SW_SHOWNORMAL);
    
    while (GetMessage(&msg, hMainWnd)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    // Clean up Korean font if loaded
    if (korean_font) {
        DestroyLogFont(korean_font);
        korean_font = NULL;
    }
    
    // Clean up Korean input system
    qwerty_korean_cleanup();
    
    // Try to force cleanup of MiniGUI resources
    printf("Cleaning up MiniGUI resources...\n");
    
    // Small delay to allow MiniGUI to clean up
    usleep(100000); // 100ms delay
    
    // MainWindowThreadCleanup removed for compatibility
    return 0;
}

#ifndef _MGRM_PROCESSES
#include <minigui/dti.c>
#endif

