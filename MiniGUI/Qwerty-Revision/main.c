#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <unistd.h>
#include <signal.h>
#include <locale.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include "qwerty.h"

// Control IDs
#define IDC_TEXTBOX    101

// Row 0: ` 1 2 3 4 5 6 7 8 9 0 - = Backspace
#define IDC_KEY_GRAVE      200
#define IDC_KEY_1          201
#define IDC_KEY_2          202
#define IDC_KEY_3          203
#define IDC_KEY_4          204
#define IDC_KEY_5          205
#define IDC_KEY_6          206
#define IDC_KEY_7          207
#define IDC_KEY_8          208
#define IDC_KEY_9          209
#define IDC_KEY_0          210
#define IDC_KEY_MINUS      211
#define IDC_KEY_EQUAL      212
#define IDC_KEY_BACKSPACE  213

// Row 1: Tab Q-P [ ] backslash
#define IDC_KEY_TAB        220
#define IDC_KEY_Q          221
#define IDC_KEY_W          222
#define IDC_KEY_E          223
#define IDC_KEY_R          224
#define IDC_KEY_T          225
#define IDC_KEY_Y          226
#define IDC_KEY_U          227
#define IDC_KEY_I          228
#define IDC_KEY_O          229
#define IDC_KEY_P          230
#define IDC_KEY_LBRACKET   231
#define IDC_KEY_RBRACKET   232
#define IDC_KEY_BACKSLASH  233

// Row 2: A-L ; '
#define IDC_KEY_A          240
#define IDC_KEY_S          241
#define IDC_KEY_D          242
#define IDC_KEY_F          243
#define IDC_KEY_G          244
#define IDC_KEY_H          245
#define IDC_KEY_J          246
#define IDC_KEY_K          247
#define IDC_KEY_L          248
#define IDC_KEY_SEMICOLON  249
#define IDC_KEY_QUOTE      250

// Row 3: Z-M , . /
#define IDC_KEY_Z          260
#define IDC_KEY_X          261
#define IDC_KEY_C          262
#define IDC_KEY_V          263
#define IDC_KEY_B          264
#define IDC_KEY_N          265
#define IDC_KEY_M          266
#define IDC_KEY_COMMA      267
#define IDC_KEY_PERIOD     268
#define IDC_KEY_SLASH      269

// Row 4: Special keys
#define IDC_KEY_LEFTSHIFT  280
#define IDC_KEY_MODE       281  // Korean/English toggle
#define IDC_KEY_SPACE      282
#define IDC_KEY_RIGHTSHIFT 283
#define IDC_KEY_BACK       284
#define IDC_KEY_ENTER      285
#define IDC_KEY_STORE      286
#define IDC_KEY_LOAD       287

#define MAX_OUTPUT_LEN 1024
#define SAVE_DIR "/tmp/korean_input"
#define MAX_FILENAME 256

// UI Layout Constants
#define TEXTBOX_X 20
#define TEXTBOX_Y 20
#define TEXTBOX_WIDTH 650
#define TEXTBOX_HEIGHT 70

#define KEYBOARD_START_Y 110
#define KEY_WIDTH 40
#define KEY_HEIGHT 40
#define KEY_SPACING 42
#define WINDOW_WIDTH 700

// Dialog Constants - Save Dialog
#define SAVE_DLG_WIDTH 300
#define SAVE_DLG_HEIGHT 140
#define SAVE_DLG_X 100
#define SAVE_DLG_Y 100
#define SAVE_LABEL_X 10
#define SAVE_LABEL_Y 15
#define SAVE_LABEL_WIDTH 280
#define SAVE_LABEL_HEIGHT 20
#define SAVE_INPUT_X 10
#define SAVE_INPUT_Y 38
#define SAVE_INPUT_WIDTH 280
#define SAVE_INPUT_HEIGHT 25
#define SAVE_BTN_Y 80
#define SAVE_OK_BTN_X 80
#define SAVE_CANCEL_BTN_X 160
#define SAVE_BTN_WIDTH 60
#define SAVE_BTN_HEIGHT 25

// Dialog Constants - Load Dialog
#define LOAD_DLG_WIDTH 300
#define LOAD_DLG_HEIGHT 240
#define LOAD_DLG_X 100
#define LOAD_DLG_Y 100
#define LOAD_LABEL_X 10
#define LOAD_LABEL_Y 15
#define LOAD_LABEL_WIDTH 280
#define LOAD_LABEL_HEIGHT 20
#define LOAD_LIST_X 10
#define LOAD_LIST_Y 38
#define LOAD_LIST_WIDTH 280
#define LOAD_LIST_HEIGHT 120
#define LOAD_BTN_Y 170
#define LOAD_OK_BTN_X 80
#define LOAD_CANCEL_BTN_X 160
#define LOAD_BTN_WIDTH 60
#define LOAD_BTN_HEIGHT 25

// Dialog Control IDs
#define SAVE_LABEL_ID 1000
#define SAVE_INPUT_ID 1001
#define LOAD_LABEL_ID 1000
#define LOAD_LIST_ID 1002

// Global variables
static HWND hMainWnd = HWND_INVALID;
static HWND hTextBox = HWND_INVALID;
static HWND hKeyButtons[80] = {HWND_INVALID}; // Initialize all to invalid
static PLOGFONT korean_font = NULL;
static BOOL korean_font_needs_destroy = FALSE; // Track if we need to destroy the font
static QwertyState qwerty_state;
static wchar_t display_buffer[MAX_OUTPUT_LEN] = {0};
static size_t display_len = 0;

// Forward declaration
static void update_textbox(void);

// Custom implementation of wcstombs() - converts wide character string to multibyte string
static size_t custom_wcstombs(char *dest, const wchar_t *src, size_t n) {
    size_t written = 0;
    size_t i = 0;
    
    if (!src) {
        return (size_t)-1;
    }
    
    // If dest is NULL, just count the bytes needed
    if (!dest) {
        while (src[i] != L'\0') {
            wchar_t wc = src[i++];
            
            if (wc < 0x80) {
                written += 1;
            } else if (wc < 0x800) {
                written += 2;
            } else if (wc < 0x10000) {
                written += 3;
            } else if (wc < 0x110000) {
                written += 4;
            } else {
                return (size_t)-1; // Invalid character
            }
        }
        return written;
    }
    
    // Convert and write to dest
    while (src[i] != L'\0' && written < n) {
        wchar_t wc = src[i++];
        
        if (wc < 0x80) {
            // 1-byte UTF-8 (ASCII)
            if (written + 1 > n) break;
            dest[written++] = (char)wc;
        } else if (wc < 0x800) {
            // 2-byte UTF-8
            if (written + 2 > n) break;
            dest[written++] = (char)(0xC0 | (wc >> 6));
            dest[written++] = (char)(0x80 | (wc & 0x3F));
        } else if (wc < 0x10000) {
            // 3-byte UTF-8
            if (written + 3 > n) break;
            dest[written++] = (char)(0xE0 | (wc >> 12));
            dest[written++] = (char)(0x80 | ((wc >> 6) & 0x3F));
            dest[written++] = (char)(0x80 | (wc & 0x3F));
        } else if (wc < 0x110000) {
            // 4-byte UTF-8
            if (written + 4 > n) break;
            dest[written++] = (char)(0xF0 | (wc >> 18));
            dest[written++] = (char)(0x80 | ((wc >> 12) & 0x3F));
            dest[written++] = (char)(0x80 | ((wc >> 6) & 0x3F));
            dest[written++] = (char)(0x80 | (wc & 0x3F));
        } else {
            // Invalid character
            return (size_t)-1;
        }
    }
    
    return written;
}

// Custom implementation of mbstowcs() - converts multibyte string (UTF-8) to wide character string
static size_t custom_mbstowcs(wchar_t *dest, const char *src, size_t n) {
    size_t written = 0;
    size_t i = 0;

    if (!src) {
        return (size_t)-1;
    }

    // If dest is NULL, just count the wide characters needed
    if (!dest) {
        while (src[i] != '\0') {
            unsigned char c = (unsigned char)src[i];

            if (c < 0x80) {
                // 1-byte UTF-8 (ASCII)
                i += 1;
            } else if ((c & 0xE0) == 0xC0) {
                // 2-byte UTF-8
                if (src[i+1] == '\0' || (src[i+1] & 0xC0) != 0x80) {
                    return (size_t)-1; // Invalid UTF-8
                }
                i += 2;
            } else if ((c & 0xF0) == 0xE0) {
                // 3-byte UTF-8
                if (src[i+1] == '\0' || src[i+2] == '\0' ||
                    (src[i+1] & 0xC0) != 0x80 || (src[i+2] & 0xC0) != 0x80) {
                    return (size_t)-1; // Invalid UTF-8
                }
                i += 3;
            } else if ((c & 0xF8) == 0xF0) {
                // 4-byte UTF-8
                if (src[i+1] == '\0' || src[i+2] == '\0' || src[i+3] == '\0' ||
                    (src[i+1] & 0xC0) != 0x80 || (src[i+2] & 0xC0) != 0x80 || (src[i+3] & 0xC0) != 0x80) {
                    return (size_t)-1; // Invalid UTF-8
                }
                i += 4;
            } else {
                return (size_t)-1; // Invalid UTF-8
            }
            written++;
        }
        return written;
    }

    // Convert and write to dest
    while (src[i] != '\0' && written < n) {
        unsigned char c = (unsigned char)src[i];
        wchar_t wc;

        if (c < 0x80) {
            // 1-byte UTF-8 (ASCII)
            wc = c;
            i += 1;
        } else if ((c & 0xE0) == 0xC0) {
            // 2-byte UTF-8
            if (src[i+1] == '\0' || (src[i+1] & 0xC0) != 0x80) {
                return (size_t)-1; // Invalid UTF-8
            }
            wc = ((c & 0x1F) << 6) | (src[i+1] & 0x3F);
            i += 2;
        } else if ((c & 0xF0) == 0xE0) {
            // 3-byte UTF-8
            if (src[i+1] == '\0' || src[i+2] == '\0' ||
                (src[i+1] & 0xC0) != 0x80 || (src[i+2] & 0xC0) != 0x80) {
                return (size_t)-1; // Invalid UTF-8
            }
            wc = ((c & 0x0F) << 12) | ((src[i+1] & 0x3F) << 6) | (src[i+2] & 0x3F);
            i += 3;
        } else if ((c & 0xF8) == 0xF0) {
            // 4-byte UTF-8
            if (src[i+1] == '\0' || src[i+2] == '\0' || src[i+3] == '\0' ||
                (src[i+1] & 0xC0) != 0x80 || (src[i+2] & 0xC0) != 0x80 || (src[i+3] & 0xC0) != 0x80) {
                return (size_t)-1; // Invalid UTF-8
            }
            wc = ((c & 0x07) << 18) | ((src[i+1] & 0x3F) << 12) |
                 ((src[i+2] & 0x3F) << 6) | (src[i+3] & 0x3F);
            i += 4;
        } else {
            // Invalid UTF-8
            return (size_t)-1;
        }

        dest[written++] = wc;
    }

    return written;
}

// Helper function to convert wide string to UTF-8
static void wchar_to_utf8(const wchar_t *wstr, char *utf8, size_t utf8_size) {
    size_t len = custom_wcstombs(utf8, wstr, utf8_size - 1);
    if (len == (size_t)-1) {
        utf8[0] = '\0';
    } else {
        utf8[len] = '\0';
    }
}

// Helper function to ensure save directory exists
static void ensure_save_dir(void) {
    struct stat st = {0};
    if (stat(SAVE_DIR, &st) == -1) {
        mkdir(SAVE_DIR, 0755);
    }
}

// Helper function to list all saved files
static char** get_saved_files(int *count) {
    DIR *dir;
    struct dirent *entry;
    char **files = NULL;
    int file_count = 0;

    ensure_save_dir();

    dir = opendir(SAVE_DIR);
    if (!dir) {
        *count = 0;
        return NULL;
    }

    // Count files first
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG && strstr(entry->d_name, ".txt") != NULL) {
            file_count++;
        }
    }

    if (file_count == 0) {
        closedir(dir);
        *count = 0;
        return NULL;
    }

    // Allocate and fill array
    files = (char**)malloc(file_count * sizeof(char*));
    rewinddir(dir);

    file_count = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG && strstr(entry->d_name, ".txt") != NULL) {
            files[file_count] = (char*)malloc(strlen(entry->d_name) + 1);
            strcpy(files[file_count], entry->d_name);
            file_count++;
        }
    }

    closedir(dir);
    *count = file_count;
    return files;
}

// Helper function to free file list
static void free_file_list(char **files, int count) {
    if (files) {
        for (int i = 0; i < count; i++) {
            free(files[i]);
        }
        free(files);
    }
}

// Function to save current input to file
static void save_korean_input(const char *filename) {
    char filepath[512];
    FILE *file;
    char utf8_output[MAX_OUTPUT_LEN * 4] = {0};

    ensure_save_dir();
    snprintf(filepath, sizeof(filepath), "%s/%s", SAVE_DIR, filename);

    file = fopen(filepath, "w");
    if (!file) {
        printf("Error: Cannot create file %s\n", filepath);
        MessageBox(hMainWnd, "Failed to save file", "Error", MB_OK | MB_ICONEXCLAMATION);
        return;
    }

    wchar_to_utf8(display_buffer, utf8_output, sizeof(utf8_output));
    fprintf(file, "%s", utf8_output);
    fclose(file);

    printf("Saved Korean input to %s\n", filepath);
    MessageBox(hMainWnd, filepath, "File Saved", MB_OK | MB_ICONINFORMATION);
}

// Function to load Korean input from file
static void load_korean_input(const char *filename) {
    char filepath[512];
    FILE *file;
    char utf8_buffer[MAX_OUTPUT_LEN * 4] = {0};

    snprintf(filepath, sizeof(filepath), "%s/%s", SAVE_DIR, filename);

    file = fopen(filepath, "r");
    if (!file) {
        printf("Error: Cannot open file %s\n", filepath);
        MessageBox(hMainWnd, "Failed to open file", "Error", MB_OK | MB_ICONEXCLAMATION);
        return;
    }

    // Read file content
    size_t read = fread(utf8_buffer, 1, sizeof(utf8_buffer) - 1, file);
    utf8_buffer[read] = '\0';
    fclose(file);

    // Convert UTF-8 to wchar_t using custom function
    display_len = custom_mbstowcs(display_buffer, utf8_buffer, MAX_OUTPUT_LEN - 1);
    if (display_len == (size_t)-1) {
        display_len = 0;
        display_buffer[0] = L'\0';
        printf("Error: Invalid UTF-8 in file\n");
        MessageBox(hMainWnd, "Failed to load file (invalid UTF-8)", "Error", MB_OK | MB_ICONEXCLAMATION);
        return;
    }

    display_buffer[display_len] = L'\0';
    qwerty_reset_composition(&qwerty_state);

    printf("Loaded Korean input from %s\n", filepath);
    update_textbox();
    MessageBox(hMainWnd, filepath, "File Loaded", MB_OK | MB_ICONINFORMATION);
}

// Comprehensive cleanup function
static void cleanup_resources(void) {
    printf("Cleaning up all resources...\n");

    // Clean up font first
    if (korean_font && korean_font_needs_destroy) {
        DestroyLogFont(korean_font);
        korean_font = NULL;
        korean_font_needs_destroy = FALSE;
    }

    // Clean up text box
    if (hTextBox != HWND_INVALID) {
        DestroyWindow(hTextBox);
        hTextBox = HWND_INVALID;
    }

    // Clean up all key buttons
    for (int i = 0; i < 80; i++) {
        if (hKeyButtons[i] != HWND_INVALID) {
            DestroyWindow(hKeyButtons[i]);
            hKeyButtons[i] = HWND_INVALID;
        }
    }

    // Allow some time for window cleanup to propagate
    usleep(50000); // 50ms delay

    // Clean up main window
    if (hMainWnd != HWND_INVALID) {
        DestroyMainWindow(hMainWnd);
        hMainWnd = HWND_INVALID;
    }

    printf("Resource cleanup completed.\n");
}

// Signal handler for cleanup
void cleanup_handler(int sig) {
    printf("Received signal %d, cleaning up...\n", sig);
    cleanup_resources();
    exit(0);
}

// Function to update text box
static void update_textbox() {
    char utf8_output[MAX_OUTPUT_LEN * 4] = {0};
    wchar_to_utf8(display_buffer, utf8_output, sizeof(utf8_output));

    SetWindowText(hTextBox, utf8_output);
    InvalidateRect(hTextBox, NULL, TRUE);
    UpdateWindow(hTextBox, TRUE);
    InvalidateRect(hMainWnd, NULL, TRUE);
    UpdateWindow(hMainWnd, TRUE);
}

// Function to update button labels based on Korean/English mode and shift state
static void update_button_labels() {
    typedef struct {
        int btn_idx;
        int key_id;
        int key_map_idx;
    } KeyLabelInfo;

    KeyLabelInfo labels[] = {
        // Row 0: Numbers and special chars
        {0, IDC_KEY_GRAVE, 0},
        {1, IDC_KEY_1, 1},
        {2, IDC_KEY_2, 2},
        {3, IDC_KEY_3, 3},
        {4, IDC_KEY_4, 4},
        {5, IDC_KEY_5, 5},
        {6, IDC_KEY_6, 6},
        {7, IDC_KEY_7, 7},
        {8, IDC_KEY_8, 8},
        {9, IDC_KEY_9, 9},
        {10, IDC_KEY_0, 10},
        {11, IDC_KEY_MINUS, 11},
        {12, IDC_KEY_EQUAL, 12},

        // Row 1: QWERTYUIOP
        {14, IDC_KEY_Q, 13},
        {15, IDC_KEY_W, 14},
        {16, IDC_KEY_E, 15},
        {17, IDC_KEY_R, 16},
        {18, IDC_KEY_T, 17},
        {19, IDC_KEY_Y, 18},
        {20, IDC_KEY_U, 19},
        {21, IDC_KEY_I, 20},
        {22, IDC_KEY_O, 21},
        {23, IDC_KEY_P, 22},
        {24, IDC_KEY_LBRACKET, 23},
        {25, IDC_KEY_RBRACKET, 24},
        {26, IDC_KEY_BACKSLASH, 25},

        // Row 2: ASDFGHJKL
        {27, IDC_KEY_A, 26},
        {28, IDC_KEY_S, 27},
        {29, IDC_KEY_D, 28},
        {30, IDC_KEY_F, 29},
        {31, IDC_KEY_G, 30},
        {32, IDC_KEY_H, 31},
        {33, IDC_KEY_J, 32},
        {34, IDC_KEY_K, 33},
        {35, IDC_KEY_L, 34},
        {36, IDC_KEY_SEMICOLON, 35},
        {37, IDC_KEY_QUOTE, 36},

        // Row 3: ZXCVBNM
        {39, IDC_KEY_Z, 37},
        {40, IDC_KEY_X, 38},
        {41, IDC_KEY_C, 39},
        {42, IDC_KEY_V, 40},
        {43, IDC_KEY_B, 41},
        {44, IDC_KEY_N, 42},
        {45, IDC_KEY_M, 43},
        {46, IDC_KEY_COMMA, 44},
        {47, IDC_KEY_PERIOD, 45},
        {48, IDC_KEY_SLASH, 46},
    };

    int num_labels = sizeof(labels) / sizeof(KeyLabelInfo);

    // Update all key labels
    for (int i = 0; i < num_labels; i++) {
        HWND btn = hKeyButtons[labels[i].btn_idx];
        if (btn && labels[i].key_map_idx < 47) {
            const char* label = qwerty_get_key_char(&qwerty_state, &key_maps[labels[i].key_map_idx]);
            SetWindowText(btn, label);
            InvalidateRect(btn, NULL, TRUE);
            UpdateWindow(btn, TRUE);
        }
    }

    // Update mode button
    for (int i = 0; i < 80; i++) {
        if (hKeyButtons[i] && GetDlgCtrlID(hKeyButtons[i]) == IDC_KEY_MODE) {
            const char* mode_label = (qwerty_state.current_language == LANG_KOREAN) ? "English" : "한글";
            SetWindowText(hKeyButtons[i], mode_label);
            SetWindowBkColor(hKeyButtons[i], (qwerty_state.current_language == LANG_KOREAN) ? PIXEL_lightgray : PIXEL_lightwhite);
            InvalidateRect(hKeyButtons[i], NULL, TRUE);
            UpdateWindow(hKeyButtons[i], TRUE);
            break;
        }
    }

    // Update shift button
    for (int i = 0; i < 80; i++) {
        if (hKeyButtons[i]) {
            int ctrl_id = GetDlgCtrlID(hKeyButtons[i]);
            if (ctrl_id == IDC_KEY_LEFTSHIFT || ctrl_id == IDC_KEY_RIGHTSHIFT) {
                SetWindowText(hKeyButtons[i], "Shift");
                SetWindowBkColor(hKeyButtons[i], qwerty_state.shift_pressed ? PIXEL_darkgray : PIXEL_lightwhite);
                InvalidateRect(hKeyButtons[i], NULL, TRUE);
                UpdateWindow(hKeyButtons[i], TRUE);
            }
        }
    }
}

// Map key ID to key_maps array index
static int get_key_map_index(int key_id) {
    switch (key_id) {
        // Row 0
        case IDC_KEY_GRAVE: return 0;
        case IDC_KEY_1: return 1;
        case IDC_KEY_2: return 2;
        case IDC_KEY_3: return 3;
        case IDC_KEY_4: return 4;
        case IDC_KEY_5: return 5;
        case IDC_KEY_6: return 6;
        case IDC_KEY_7: return 7;
        case IDC_KEY_8: return 8;
        case IDC_KEY_9: return 9;
        case IDC_KEY_0: return 10;
        case IDC_KEY_MINUS: return 11;
        case IDC_KEY_EQUAL: return 12;
        // Row 1
        case IDC_KEY_Q: return 13;
        case IDC_KEY_W: return 14;
        case IDC_KEY_E: return 15;
        case IDC_KEY_R: return 16;
        case IDC_KEY_T: return 17;
        case IDC_KEY_Y: return 18;
        case IDC_KEY_U: return 19;
        case IDC_KEY_I: return 20;
        case IDC_KEY_O: return 21;
        case IDC_KEY_P: return 22;
        case IDC_KEY_LBRACKET: return 23;
        case IDC_KEY_RBRACKET: return 24;
        case IDC_KEY_BACKSLASH: return 25;
        // Row 2
        case IDC_KEY_A: return 26;
        case IDC_KEY_S: return 27;
        case IDC_KEY_D: return 28;
        case IDC_KEY_F: return 29;
        case IDC_KEY_G: return 30;
        case IDC_KEY_H: return 31;
        case IDC_KEY_J: return 32;
        case IDC_KEY_K: return 33;
        case IDC_KEY_L: return 34;
        case IDC_KEY_SEMICOLON: return 35;
        case IDC_KEY_QUOTE: return 36;
        // Row 3
        case IDC_KEY_Z: return 37;
        case IDC_KEY_X: return 38;
        case IDC_KEY_C: return 39;
        case IDC_KEY_V: return 40;
        case IDC_KEY_B: return 41;
        case IDC_KEY_N: return 42;
        case IDC_KEY_M: return 43;
        case IDC_KEY_COMMA: return 44;
        case IDC_KEY_PERIOD: return 45;
        case IDC_KEY_SLASH: return 46;
        default: return -1;
    }
}

// Dialog procedure for saving
static LRESULT SaveDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hInput = HWND_INVALID;

    switch (message) {
        case MSG_INITDIALOG: {
            hInput = GetDlgItem(hDlg, 1001);
            SetFocus(hInput);
            return TRUE;
        }
        case MSG_COMMAND:
            if (wParam == IDOK) {
                char filename[MAX_FILENAME];
                GetWindowText(hInput, filename, sizeof(filename));

                if (strlen(filename) == 0) {
                    MessageBox(hDlg, "Please enter a filename", "Error", MB_OK | MB_ICONEXCLAMATION);
                    return TRUE;
                }

                // Ensure .txt extension
                if (!strstr(filename, ".txt")) {
                    strcat(filename, ".txt");
                }

                save_korean_input(filename);
                EndDialog(hDlg, IDOK);
            } else if (wParam == IDCANCEL) {
                EndDialog(hDlg, IDCANCEL);
            }
            return TRUE;
    }
    return DefaultDialogProc(hDlg, message, wParam, lParam);
}

// Dialog procedure for loading
static LRESULT LoadDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hListBox = HWND_INVALID;
    static char **files = NULL;
    static int file_count = 0;

    switch (message) {
        case MSG_INITDIALOG: {
            hListBox = GetDlgItem(hDlg, 1002);

            // Get list of files
            files = get_saved_files(&file_count);

            if (file_count == 0) {
                SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)"No saved files");
                EnableWindow(GetDlgItem(hDlg, IDOK), FALSE);
                return TRUE;
            }

            // Add files to listbox
            for (int i = 0; i < file_count; i++) {
                SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)files[i]);
            }

            // Select first item
            SendMessage(hListBox, LB_SETCURSEL, 0, 0);
            SetFocus(hListBox);
            return TRUE;
        }
        case MSG_COMMAND:
            if (wParam == IDOK) {
                int selected = SendMessage(hListBox, LB_GETCURSEL, 0, 0);
                if (selected >= 0 && selected < file_count) {
                    load_korean_input(files[selected]);
                }
                free_file_list(files, file_count);
                EndDialog(hDlg, IDOK);
            } else if (wParam == IDCANCEL) {
                free_file_list(files, file_count);
                EndDialog(hDlg, IDCANCEL);
            }
            return TRUE;
    }
    return DefaultDialogProc(hDlg, message, wParam, lParam);
}

// Function to handle key button clicks
static void handle_key_click(int key_id) {
    // Handle store button
    if (key_id == IDC_KEY_STORE) {
        // Create a simple filename input dialog
        DLGTEMPLATE dlg_template = {
            WS_BORDER | WS_CAPTION,
            WS_EX_NONE,
            SAVE_DLG_X, SAVE_DLG_Y, SAVE_DLG_WIDTH, SAVE_DLG_HEIGHT,
            "Save Korean Input",
            0, 0,
            4,
            NULL,
            0
        };

        CTRLDATA CtrlData[] = {
            {
                CTRL_STATIC,
                WS_VISIBLE | SS_LEFT,
                SAVE_LABEL_X, SAVE_LABEL_Y, SAVE_LABEL_WIDTH, SAVE_LABEL_HEIGHT,
                SAVE_LABEL_ID,
                "Enter filename (without .txt):",
                0,
                WS_EX_NONE
            },
            {
                CTRL_EDIT,
                WS_VISIBLE | WS_TABSTOP | WS_BORDER,
                SAVE_INPUT_X, SAVE_INPUT_Y, SAVE_INPUT_WIDTH, SAVE_INPUT_HEIGHT,
                SAVE_INPUT_ID,
                "",
                0,
                WS_EX_NONE
            },
            {
                CTRL_BUTTON,
                WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
                SAVE_OK_BTN_X, SAVE_BTN_Y, SAVE_BTN_WIDTH, SAVE_BTN_HEIGHT,
                IDOK,
                "OK",
                0,
                WS_EX_NONE
            },
            {
                CTRL_BUTTON,
                WS_VISIBLE | WS_TABSTOP,
                SAVE_CANCEL_BTN_X, SAVE_BTN_Y, SAVE_BTN_WIDTH, SAVE_BTN_HEIGHT,
                IDCANCEL,
                "Cancel",
                0,
                WS_EX_NONE
            }
        };

        dlg_template.controlnr = 4;
        dlg_template.controls = CtrlData;

        DialogBoxIndirectParam(&dlg_template, hMainWnd, SaveDialogProc, 0L);
        return;
    }

    // Handle load button
    if (key_id == IDC_KEY_LOAD) {
        DLGTEMPLATE dlg_template = {
            WS_BORDER | WS_CAPTION,
            WS_EX_NONE,
            LOAD_DLG_X, LOAD_DLG_Y, LOAD_DLG_WIDTH, LOAD_DLG_HEIGHT,
            "Load Korean Input",
            0, 0,
            4,
            NULL,
            0
        };

        CTRLDATA CtrlData[] = {
            {
                CTRL_STATIC,
                WS_VISIBLE | SS_LEFT,
                LOAD_LABEL_X, LOAD_LABEL_Y, LOAD_LABEL_WIDTH, LOAD_LABEL_HEIGHT,
                LOAD_LABEL_ID,
                "Select a file to load:",
                0,
                WS_EX_NONE
            },
            {
                CTRL_LISTBOX,
                WS_VISIBLE | WS_TABSTOP | WS_BORDER,
                LOAD_LIST_X, LOAD_LIST_Y, LOAD_LIST_WIDTH, LOAD_LIST_HEIGHT,
                LOAD_LIST_ID,
                "",
                0,
                WS_EX_NONE
            },
            {
                CTRL_BUTTON,
                WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
                LOAD_OK_BTN_X, LOAD_BTN_Y, LOAD_BTN_WIDTH, LOAD_BTN_HEIGHT,
                IDOK,
                "OK",
                0,
                WS_EX_NONE
            },
            {
                CTRL_BUTTON,
                WS_VISIBLE | WS_TABSTOP,
                LOAD_CANCEL_BTN_X, LOAD_BTN_Y, LOAD_BTN_WIDTH, LOAD_BTN_HEIGHT,
                IDCANCEL,
                "Cancel",
                0,
                WS_EX_NONE
            }
        };

        dlg_template.controlnr = 4;
        dlg_template.controls = CtrlData;

        DialogBoxIndirectParam(&dlg_template, hMainWnd, LoadDialogProc, 0L);
        return;
    }

    // Handle mode toggle
    if (key_id == IDC_KEY_MODE) {
        qwerty_state.current_language = (qwerty_state.current_language == LANG_KOREAN) ? LANG_ENGLISH : LANG_KOREAN;
        qwerty_reset_composition(&qwerty_state);
        printf("Mode switched to: %s\n", (qwerty_state.current_language == LANG_KOREAN) ? "Korean" : "English");
        update_button_labels();
        return;
    }

    // Handle shift toggle (both left and right shift)
    if (key_id == IDC_KEY_LEFTSHIFT || key_id == IDC_KEY_RIGHTSHIFT) {
        qwerty_state.shift_pressed = !qwerty_state.shift_pressed;
        printf("Shift %s\n", qwerty_state.shift_pressed ? "ON" : "OFF");
        update_button_labels();
        return;
    }

    // Handle backspace
    if (key_id == IDC_KEY_BACKSPACE || key_id == IDC_KEY_BACK) {
        if (display_len > 0) {
            display_len--;
            display_buffer[display_len] = L'\0';
            qwerty_reset_composition(&qwerty_state);
        }
        update_textbox();
        return;
    }

    // Handle enter - show popup with result and clear
    if (key_id == IDC_KEY_ENTER) {
        if (display_len > 0) {
            // Convert display buffer to UTF-8 for popup
            char utf8_result[MAX_OUTPUT_LEN * 4] = {0};
            wchar_to_utf8(display_buffer, utf8_result, sizeof(utf8_result));

            // Show message box with the input result
            MessageBox(hMainWnd, utf8_result, "Input Result", MB_OK | MB_ICONINFORMATION);

            // Clear the display buffer
            display_len = 0;
            display_buffer[0] = L'\0';
            qwerty_reset_composition(&qwerty_state);
        }
        update_textbox();
        return;
    }

    // Handle space
    if (key_id == IDC_KEY_SPACE) {
        if (display_len < MAX_OUTPUT_LEN - 1) {
            display_buffer[display_len++] = L' ';
            display_buffer[display_len] = L'\0';
            qwerty_reset_composition(&qwerty_state);
        }
        update_textbox();
        return;
    }

    // Handle regular keys
    int key_map_idx = get_key_map_index(key_id);
    if (key_map_idx >= 0 && key_map_idx < 47) {
        const char* key_str = qwerty_get_key_char(&qwerty_state, &key_maps[key_map_idx]);

        if (qwerty_state.current_language == LANG_KOREAN) {
            // Korean mode: use composition
            char output[16] = {0};
            int delete_previous = 0;

            qwerty_process_korean_char(&qwerty_state, key_str, output, &delete_previous);

            if (delete_previous && display_len > 0) {
                display_len--;
            }

            // Convert UTF-8 output to wchar_t using custom function
            wchar_t wch[8] = {0};
            custom_mbstowcs(wch, output, sizeof(wch) / sizeof(wchar_t) - 1);

            // Append to display buffer
            for (int i = 0; wch[i] != L'\0' && display_len < MAX_OUTPUT_LEN - 1; i++) {
                display_buffer[display_len++] = wch[i];
            }
            display_buffer[display_len] = L'\0';
        } else {
            // English mode: direct input
            wchar_t wch[8] = {0};
            custom_mbstowcs(wch, key_str, sizeof(wch) / sizeof(wchar_t) - 1);

            if (display_len < MAX_OUTPUT_LEN - 1 && wch[0] != L'\0') {
                display_buffer[display_len++] = wch[0];
                display_buffer[display_len] = L'\0';
            }
        }

        update_textbox();
    }
}

static LRESULT KoreanInputWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;

    switch (message) {
        case MSG_CREATE:
            // Set locale for wide character support
            setlocale(LC_ALL, "ko_KR.UTF-8");

            // Load Korean font - try Bold first since Regular has zero-width backtick
            // Load Korean font - try Bold first since Regular has zero-width backtick
            // Using smaller font size to reduce FreeType2 cache issues
            korean_font = CreateLogFont("ttf", "NanumGothic-Bold", "UTF-8",
                                        FONT_WEIGHT_BOLD, FONT_SLANT_ROMAN, FONT_FLIP_NONE,
                                        FONT_OTHER_NONE, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                                        14, 0);
            if (korean_font != NULL) {
                korean_font_needs_destroy = TRUE;
            } else {
                korean_font = CreateLogFont("ttf", "NanumGothic-Regular", "UTF-8",
                                            FONT_WEIGHT_NORMAL, FONT_SLANT_ROMAN, FONT_FLIP_NONE,
                                            FONT_OTHER_NONE, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                                            14, 0);
                if (korean_font != NULL) {
                    korean_font_needs_destroy = TRUE;
                }
            }
            if (korean_font == NULL) {
                // Try system font as fallback
                korean_font = CreateLogFont(NULL, "fixed", "UTF-8",
                                            FONT_WEIGHT_NORMAL, FONT_SLANT_ROMAN, FONT_FLIP_NONE,
                                            FONT_OTHER_NONE, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
                                            12, 0);
                if (korean_font != NULL) {
                    korean_font_needs_destroy = TRUE;
                }
            }

            if (korean_font) {
                printf("Loaded Korean font for character support (size 14pt, optimized for cache)\n");
            } else {
                printf("Using default font (Korean characters may not display correctly)\n");
                // Use system default font as last resort - don't need to free system fonts
                korean_font = GetSystemFont(SYSLOGFONT_DEFAULT);
                korean_font_needs_destroy = FALSE; // System fonts don't need to be destroyed
            }

            // Create text box - use STATIC control instead of MLEDIT (MLEDIT crashes on ARM64)
            printf("Creating text box...\n");
            hTextBox = CreateWindowEx2(CTRL_STATIC, "",
                WS_VISIBLE | WS_BORDER | SS_LEFT,
                0,
                IDC_TEXTBOX,
                TEXTBOX_X, TEXTBOX_Y, TEXTBOX_WIDTH, TEXTBOX_HEIGHT,
                hWnd, NULL, NULL, 0);
            printf("Text box created, handle: %p\n", hTextBox);
            if (hTextBox == HWND_INVALID) {
                printf("ERROR: Failed to create text box\n");
                return -1;
            }
            printf("Setting font for text box...\n");
            if (korean_font) {
                SetWindowFont(hTextBox, korean_font);
                InvalidateRect(hTextBox, NULL, TRUE);
                UpdateWindow(hTextBox, TRUE);
            }
            printf("Text box setup complete.\n");

            // Full QWERTY keyboard layout
            int btn_idx = 0;

            // Calculate keyboard width and center it
            // Row 0 is the widest: 13 keys (40px each) + 1 backspace (65px) + spacing
            int keyboard_width = (KEY_SPACING * 13) + 65;  // Total width of row 0
            int start_x = (WINDOW_WIDTH - keyboard_width) / 2;  // Center position
            int start_y = KEYBOARD_START_Y;

            // Row 0: ` 1 2 3 4 5 6 7 8 9 0 - = Backspace
            hKeyButtons[btn_idx++] = CreateWindow("button", "'", WS_VISIBLE | WS_CHILD, IDC_KEY_GRAVE,
                start_x, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "1", WS_VISIBLE | WS_CHILD, IDC_KEY_1,
                start_x + KEY_SPACING, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "2", WS_VISIBLE | WS_CHILD, IDC_KEY_2,
                start_x + KEY_SPACING * 2, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "3", WS_VISIBLE | WS_CHILD, IDC_KEY_3,
                start_x + KEY_SPACING * 3, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "4", WS_VISIBLE | WS_CHILD, IDC_KEY_4,
                start_x + KEY_SPACING * 4, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "5", WS_VISIBLE | WS_CHILD, IDC_KEY_5,
                start_x + KEY_SPACING * 5, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "6", WS_VISIBLE | WS_CHILD, IDC_KEY_6,
                start_x + KEY_SPACING * 6, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "7", WS_VISIBLE | WS_CHILD, IDC_KEY_7,
                start_x + KEY_SPACING * 7, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "8", WS_VISIBLE | WS_CHILD, IDC_KEY_8,
                start_x + KEY_SPACING * 8, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "9", WS_VISIBLE | WS_CHILD, IDC_KEY_9,
                start_x + KEY_SPACING * 9, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "0", WS_VISIBLE | WS_CHILD, IDC_KEY_0,
                start_x + KEY_SPACING * 10, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "-", WS_VISIBLE | WS_CHILD, IDC_KEY_MINUS,
                start_x + KEY_SPACING * 11, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "=", WS_VISIBLE | WS_CHILD, IDC_KEY_EQUAL,
                start_x + KEY_SPACING * 12, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "←", WS_VISIBLE | WS_CHILD, IDC_KEY_BACKSPACE,
                start_x + KEY_SPACING * 13, start_y, 65, KEY_HEIGHT, hWnd, 0);

            // Row 1: QWERTYUIOP + [ ] backslash
            start_y += KEY_SPACING + 5;
            int row1_x = start_x + 20;
            hKeyButtons[btn_idx++] = CreateWindow("button", "q", WS_VISIBLE | WS_CHILD, IDC_KEY_Q,
                row1_x, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "w", WS_VISIBLE | WS_CHILD, IDC_KEY_W,
                row1_x + KEY_SPACING, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "e", WS_VISIBLE | WS_CHILD, IDC_KEY_E,
                row1_x + KEY_SPACING * 2, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "r", WS_VISIBLE | WS_CHILD, IDC_KEY_R,
                row1_x + KEY_SPACING * 3, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "t", WS_VISIBLE | WS_CHILD, IDC_KEY_T,
                row1_x + KEY_SPACING * 4, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "y", WS_VISIBLE | WS_CHILD, IDC_KEY_Y,
                row1_x + KEY_SPACING * 5, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "u", WS_VISIBLE | WS_CHILD, IDC_KEY_U,
                row1_x + KEY_SPACING * 6, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "i", WS_VISIBLE | WS_CHILD, IDC_KEY_I,
                row1_x + KEY_SPACING * 7, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "o", WS_VISIBLE | WS_CHILD, IDC_KEY_O,
                row1_x + KEY_SPACING * 8, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "p", WS_VISIBLE | WS_CHILD, IDC_KEY_P,
                row1_x + KEY_SPACING * 9, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "[", WS_VISIBLE | WS_CHILD, IDC_KEY_LBRACKET,
                row1_x + KEY_SPACING * 10, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "]", WS_VISIBLE | WS_CHILD, IDC_KEY_RBRACKET,
                row1_x + KEY_SPACING * 11, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "\\", WS_VISIBLE | WS_CHILD, IDC_KEY_BACKSLASH,
                row1_x + KEY_SPACING * 12, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);

            // Row 2: ASDFGHJKL + ; ' - centered
            start_y += KEY_SPACING;
            // Calculate Row 2 width: 11 keys (a-l, ;, ') = 11 × 42px = 462px
            int row2_width = KEY_SPACING * 11;
            int row2_x = (WINDOW_WIDTH - row2_width) / 2;
            hKeyButtons[btn_idx++] = CreateWindow("button", "a", WS_VISIBLE | WS_CHILD, IDC_KEY_A,
                row2_x, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "s", WS_VISIBLE | WS_CHILD, IDC_KEY_S,
                row2_x + KEY_SPACING, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "d", WS_VISIBLE | WS_CHILD, IDC_KEY_D,
                row2_x + KEY_SPACING * 2, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "f", WS_VISIBLE | WS_CHILD, IDC_KEY_F,
                row2_x + KEY_SPACING * 3, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "g", WS_VISIBLE | WS_CHILD, IDC_KEY_G,
                row2_x + KEY_SPACING * 4, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "h", WS_VISIBLE | WS_CHILD, IDC_KEY_H,
                row2_x + KEY_SPACING * 5, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "j", WS_VISIBLE | WS_CHILD, IDC_KEY_J,
                row2_x + KEY_SPACING * 6, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "k", WS_VISIBLE | WS_CHILD, IDC_KEY_K,
                row2_x + KEY_SPACING * 7, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "l", WS_VISIBLE | WS_CHILD, IDC_KEY_L,
                row2_x + KEY_SPACING * 8, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", ";", WS_VISIBLE | WS_CHILD, IDC_KEY_SEMICOLON,
                row2_x + KEY_SPACING * 9, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "'", WS_VISIBLE | WS_CHILD, IDC_KEY_QUOTE,
                row2_x + KEY_SPACING * 10, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            // Enter button moved to bottom row

            // Row 3: Shift + ZXCVBNM + , . / + Shift - centered
            start_y += KEY_SPACING;
            // Calculate Row 3 width: LShift(60) + 10 keys + RShift(60) + proper spacing = 60 + 10*42 + 60 = 540px
            int row3_width = 60 + KEY_SPACING * 10 + 60;
            int row3_x = (WINDOW_WIDTH - row3_width) / 2;
            hKeyButtons[btn_idx++] = CreateWindow("button", "Shift", WS_VISIBLE | WS_CHILD, IDC_KEY_LEFTSHIFT,
                row3_x, start_y, 60, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "z", WS_VISIBLE | WS_CHILD, IDC_KEY_Z,
                row3_x + 60, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "x", WS_VISIBLE | WS_CHILD, IDC_KEY_X,
                row3_x + 60 + KEY_SPACING, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "c", WS_VISIBLE | WS_CHILD, IDC_KEY_C,
                row3_x + 60 + KEY_SPACING * 2, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "v", WS_VISIBLE | WS_CHILD, IDC_KEY_V,
                row3_x + 60 + KEY_SPACING * 3, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "b", WS_VISIBLE | WS_CHILD, IDC_KEY_B,
                row3_x + 60 + KEY_SPACING * 4, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "n", WS_VISIBLE | WS_CHILD, IDC_KEY_N,
                row3_x + 60 + KEY_SPACING * 5, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "m", WS_VISIBLE | WS_CHILD, IDC_KEY_M,
                row3_x + 60 + KEY_SPACING * 6, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", ",", WS_VISIBLE | WS_CHILD, IDC_KEY_COMMA,
                row3_x + 60 + KEY_SPACING * 7, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", ".", WS_VISIBLE | WS_CHILD, IDC_KEY_PERIOD,
                row3_x + 60 + KEY_SPACING * 8, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "/", WS_VISIBLE | WS_CHILD, IDC_KEY_SLASH,
                row3_x + 60 + KEY_SPACING * 9, start_y, KEY_WIDTH, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "Shift", WS_VISIBLE | WS_CHILD, IDC_KEY_RIGHTSHIFT,
                row3_x + 60 + KEY_SPACING * 10, start_y, 60, KEY_HEIGHT, hWnd, 0);

            // Row 4: Special keys with Enter button at the end - centered (no Shift)
            start_y += KEY_SPACING + 5;
            // Calculate Row 4 width: Eng(65) + gap(10) + Space(250) + gap(10) + Enter(90) = 425
            int row4_width = 65 + 10 + 250 + 10 + 90;
            int row4_x = (WINDOW_WIDTH - row4_width) / 2; // Centered
            hKeyButtons[btn_idx++] = CreateWindow("button", "English", WS_VISIBLE | WS_CHILD, IDC_KEY_MODE,
                row4_x, start_y, 65, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "Space", WS_VISIBLE | WS_CHILD, IDC_KEY_SPACE,
                row4_x + 65 + 10, start_y, 250, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "Enter", WS_VISIBLE | WS_CHILD, IDC_KEY_ENTER,
                row4_x + 65 + 10 + 250 + 10, start_y, 90, KEY_HEIGHT, hWnd, 0);

            // Row 5: Store and Load buttons
            start_y += KEY_SPACING + 5;
            int row5_width = 60 + 10 + 60;
            int row5_x = (WINDOW_WIDTH - row5_width) / 2;
            hKeyButtons[btn_idx++] = CreateWindow("button", "Store", WS_VISIBLE | WS_CHILD, IDC_KEY_STORE,
                row5_x, start_y, 60, KEY_HEIGHT, hWnd, 0);
            hKeyButtons[btn_idx++] = CreateWindow("button", "Load", WS_VISIBLE | WS_CHILD, IDC_KEY_LOAD,
                row5_x + 60 + 10, start_y, 60, KEY_HEIGHT, hWnd, 0);

            // Apply Korean font to all buttons
            if (korean_font) {
                for (int i = 0; i < 80; i++) {
                    if (hKeyButtons[i]) {
                        SetWindowFont(hKeyButtons[i], korean_font);
                        InvalidateRect(hKeyButtons[i], NULL, TRUE);
                        UpdateWindow(hKeyButtons[i], TRUE);
                    }
                }
            }

            // Initialize qwerty state
            qwerty_init(&qwerty_state);

            // Initialize button labels
            update_button_labels();

            return 0;

        case MSG_COMMAND:
            handle_key_click(LOWORD(wParam));
            return 0;

        case MSG_KEYDOWN:
            // Handle physical keyboard input
            switch (wParam) {
                // Numbers
                case SCANCODE_1: handle_key_click(IDC_KEY_1); break;
                case SCANCODE_2: handle_key_click(IDC_KEY_2); break;
                case SCANCODE_3: handle_key_click(IDC_KEY_3); break;
                case SCANCODE_4: handle_key_click(IDC_KEY_4); break;
                case SCANCODE_5: handle_key_click(IDC_KEY_5); break;
                case SCANCODE_6: handle_key_click(IDC_KEY_6); break;
                case SCANCODE_7: handle_key_click(IDC_KEY_7); break;
                case SCANCODE_8: handle_key_click(IDC_KEY_8); break;
                case SCANCODE_9: handle_key_click(IDC_KEY_9); break;
                case SCANCODE_0: handle_key_click(IDC_KEY_0); break;

                // Special characters
                case SCANCODE_GRAVE: handle_key_click(IDC_KEY_GRAVE); break;
                case SCANCODE_MINUS: handle_key_click(IDC_KEY_MINUS); break;
                case SCANCODE_EQUAL: handle_key_click(IDC_KEY_EQUAL); break;
                case SCANCODE_LEFTBRACE: handle_key_click(IDC_KEY_LBRACKET); break;
                case SCANCODE_RIGHTBRACE: handle_key_click(IDC_KEY_RBRACKET); break;
                case SCANCODE_BACKSLASH: handle_key_click(IDC_KEY_BACKSLASH); break;
                case SCANCODE_SEMICOLON: handle_key_click(IDC_KEY_SEMICOLON); break;
                case SCANCODE_APOSTROPHE: handle_key_click(IDC_KEY_QUOTE); break;
                case SCANCODE_COMMA: handle_key_click(IDC_KEY_COMMA); break;
                case SCANCODE_PERIOD: handle_key_click(IDC_KEY_PERIOD); break;
                case SCANCODE_SLASH: handle_key_click(IDC_KEY_SLASH); break;

                // Letters
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

                // Special function keys
                case SCANCODE_SPACE: handle_key_click(IDC_KEY_SPACE); break;
                case SCANCODE_BACKSPACE: handle_key_click(IDC_KEY_BACKSPACE); break;
                case SCANCODE_ENTER: handle_key_click(IDC_KEY_ENTER); break;
                case SCANCODE_LEFTSHIFT:
                    handle_key_click(IDC_KEY_LEFTSHIFT);
                    break;
                case SCANCODE_ESCAPE:
                    PostMessage(hWnd, MSG_CLOSE, 0, 0);
                    break;
            }
            return 0;

        case MSG_PAINT:
            hdc = BeginPaint(hWnd);
            EndPaint(hWnd, hdc);
            return 0;

        case MSG_CLOSE:
            cleanup_resources();
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
    pCreateInfo->spCaption = "Korean QWERTY Input System - Full Keyboard";
    pCreateInfo->hMenu = 0;
    pCreateInfo->hCursor = GetSystemCursor(0);
    pCreateInfo->hIcon = 0;
    pCreateInfo->MainWindowProc = KoreanInputWinProc;
    pCreateInfo->lx = 50;
    pCreateInfo->ty = 50;
    pCreateInfo->rx = 750;  // Reduced back to original width
    pCreateInfo->by = 480;
    pCreateInfo->iBkColor = COLOR_lightwhite;
    pCreateInfo->dwAddData = 0;
    pCreateInfo->hHosting = HWND_DESKTOP;
}

int MiniGUIMain(int argc, const char* argv[])
{
    MAINWINCREATE CreateInfo;
    MSG msg;

    // Register signal handlers
    signal(SIGINT, cleanup_handler);
    signal(SIGTERM, cleanup_handler);
    signal(SIGSEGV, cleanup_handler);

    InitCreateInfo(&CreateInfo);
    hMainWnd = CreateMainWindow(&CreateInfo);

    if (hMainWnd == HWND_INVALID)
        return -1;

    ShowWindow(hMainWnd, SW_SHOWNORMAL);

    while (GetMessage(&msg, hMainWnd)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Final cleanup
    cleanup_resources();

    printf("Cleaning up MiniGUI resources...\n");
    usleep(200000); // Give MiniGUI more time to clean up internally

    return 0;
}

#ifndef _MGRM_PROCESSES
#include <minigui/dti.c>
#endif
