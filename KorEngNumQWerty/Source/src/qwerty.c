#include "qwerty.h"
#include "ui_components.h"
#include "font_config.h"
#include "qwerty_korean.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Input mode enumeration
typedef enum {
    INPUT_MODE_KOREAN,
    INPUT_MODE_ENGLISH_LOWER,
    INPUT_MODE_ENGLISH_UPPER,
    INPUT_MODE_NUMBER
} InputMode;

// Unified output buffer - all modes use wide char for Korean support
static char g_qwerty_input_buffer[MAX_OUTPUT_LEN] = "";
static size_t g_qwerty_input_len = 0;
static wchar_t g_qwerty_output_buffer[MAX_OUTPUT_LEN] = {L'\0'};
static size_t g_qwerty_output_len = 0;

// Korean composition tracking
static wchar_t g_korean_temp_buffer[MAX_OUTPUT_LEN] = {L'\0'};
static size_t g_korean_start_pos = 0;  // Position where Korean input started

// Current input mode
static InputMode g_current_mode = INPUT_MODE_KOREAN;
static bool g_shift_mode = false;
static bool g_number_shift_active = false;  // Track shift state in number mode

// Korean QWERTY keyboard layout
static const char* qwerty_keys_korean[3][10] = {
    {"ㅂ", "ㅈ", "ㄷ", "ㄱ", "ㅅ", "ㅛ", "ㅕ", "ㅑ", "ㅐ", "ㅔ"},
    {"ㅁ", "ㄴ", "ㅇ", "ㄹ", "ㅎ", "ㅗ", "ㅓ", "ㅏ", "ㅣ", ""},
    {"ㅋ", "ㅌ", "ㅊ", "ㅍ", "ㅠ", "ㅜ", "ㅡ", "", "", ""}
};

// Shifted Korean QWERTY keyboard layout (double consonants and vowels)
static const char* qwerty_keys_korean_shifted[3][10] = {
    {"ㅃ", "ㅉ", "ㄸ", "ㄲ", "ㅆ", "ㅛ", "ㅕ", "ㅑ", "ㅒ", "ㅖ"},
    {"ㅁ", "ㄴ", "ㅇ", "ㄹ", "ㅎ", "ㅗ", "ㅓ", "ㅏ", "ㅣ", ""},
    {"ㅋ", "ㅌ", "ㅊ", "ㅍ", "ㅠ", "ㅜ", "ㅡ", "", "", ""}
};

// English lowercase QWERTY layout
static const char* qwerty_keys_english_lower[3][10] = {
    {"q", "w", "e", "r", "t", "y", "u", "i", "o", "p"},
    {"a", "s", "d", "f", "g", "h", "j", "k", "l", ""},
    {"z", "x", "c", "v", "b", "n", "m", "", "", ""}
};

// English uppercase QWERTY layout
static const char* qwerty_keys_english_upper[3][10] = {
    {"Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P"},
    {"A", "S", "D", "F", "G", "H", "J", "K", "L", ""},
    {"Z", "X", "C", "V", "B", "N", "M", "", "", ""}
};

// Number and symbol layout (normal)
static const char* qwerty_keys_number[3][10] = {
    {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"},
    {"!", "@", "#", "$", "%", "^", "&", "*", "(", ")"},
    {"+", "=", "[", "]", "{", "}", "|", "\\", ";", ":"}
};

// Number and symbol layout (shifted - numbers stay same, special chars change)
static const char* qwerty_keys_number_shifted[3][10] = {
    {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"},  // Numbers stay the same
    {"~", "`", "<", ">", "/", "?", "(", ")", "°", "•"},  // Different special chars
    {"-", "_", "\"", "'", ",", ".", "/", "]", "{", "}"}  // Different special chars (/ instead of [)
};

// Global UI elements
static lv_obj_t* g_qwerty_display_label = NULL;
static lv_obj_t* g_qwerty_buttons[3][10] = {NULL};
static lv_obj_t* g_mode_label = NULL;
static lv_obj_t* g_shift_btn = NULL;

// Function prototypes
static void update_qwerty_display(void);
static void update_keyboard_layout(void);
static char map_korean_to_english(const char* korean_char);

// Map Korean character to English key for qwerty_korean.c processing
static char map_korean_to_english(const char* korean_char) {
    if (strcmp(korean_char, "ㅂ") == 0) return 'q';
    if (strcmp(korean_char, "ㅈ") == 0) return 'w';
    if (strcmp(korean_char, "ㄷ") == 0) return 'e';
    if (strcmp(korean_char, "ㄱ") == 0) return 'r';
    if (strcmp(korean_char, "ㅅ") == 0) return 't';
    if (strcmp(korean_char, "ㅛ") == 0) return 'y';
    if (strcmp(korean_char, "ㅕ") == 0) return 'u';
    if (strcmp(korean_char, "ㅑ") == 0) return 'i';
    if (strcmp(korean_char, "ㅐ") == 0) return 'o';
    if (strcmp(korean_char, "ㅔ") == 0) return 'p';
    if (strcmp(korean_char, "ㅁ") == 0) return 'a';
    if (strcmp(korean_char, "ㄴ") == 0) return 's';
    if (strcmp(korean_char, "ㅇ") == 0) return 'd';
    if (strcmp(korean_char, "ㄹ") == 0) return 'f';
    if (strcmp(korean_char, "ㅎ") == 0) return 'g';
    if (strcmp(korean_char, "ㅗ") == 0) return 'h';
    if (strcmp(korean_char, "ㅓ") == 0) return 'j';
    if (strcmp(korean_char, "ㅏ") == 0) return 'k';
    if (strcmp(korean_char, "ㅣ") == 0) return 'l';
    if (strcmp(korean_char, "ㅋ") == 0) return 'z';
    if (strcmp(korean_char, "ㅌ") == 0) return 'x';
    if (strcmp(korean_char, "ㅊ") == 0) return 'c';
    if (strcmp(korean_char, "ㅍ") == 0) return 'v';
    if (strcmp(korean_char, "ㅠ") == 0) return 'b';
    if (strcmp(korean_char, "ㅜ") == 0) return 'n';
    if (strcmp(korean_char, "ㅡ") == 0) return 'm';
    // Shifted characters
    if (strcmp(korean_char, "ㅃ") == 0) return 'Q';
    if (strcmp(korean_char, "ㅉ") == 0) return 'W';
    if (strcmp(korean_char, "ㄸ") == 0) return 'E';
    if (strcmp(korean_char, "ㄲ") == 0) return 'R';
    if (strcmp(korean_char, "ㅆ") == 0) return 'T';
    if (strcmp(korean_char, "ㅒ") == 0) return 'O';
    if (strcmp(korean_char, "ㅖ") == 0) return 'P';

    return '\0';
}

// Update display with current text - unified output from wide char buffer
static void update_qwerty_display(void) {
    if (g_qwerty_display_label) {
        char display_text[512] = "";
        wcstombs(display_text, g_qwerty_output_buffer, sizeof(display_text) - 1);
        lv_label_set_text(g_qwerty_display_label, display_text);
    }
}

// Update keyboard layout based on current mode
static void update_keyboard_layout(void) {
    const char* (*current_layout)[10] = NULL;

    // Select appropriate layout
    switch (g_current_mode) {
        case INPUT_MODE_KOREAN:
            current_layout = g_shift_mode ? qwerty_keys_korean_shifted : qwerty_keys_korean;
            break;
        case INPUT_MODE_ENGLISH_LOWER:
            current_layout = qwerty_keys_english_lower;
            break;
        case INPUT_MODE_ENGLISH_UPPER:
            current_layout = qwerty_keys_english_upper;
            break;
        case INPUT_MODE_NUMBER:
            current_layout = g_number_shift_active ? qwerty_keys_number_shifted : qwerty_keys_number;
            break;
    }

    // Update button labels
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 10; col++) {
            if (g_qwerty_buttons[row][col] != NULL) {
                lv_obj_t* label = lv_obj_get_child(g_qwerty_buttons[row][col], 0);
                if (label != NULL && current_layout[row][col][0] != '\0') {
                    lv_label_set_text(label, current_layout[row][col]);
                }
            }
        }
    }

    // Update mode label
    if (g_mode_label) {
        const char* mode_text = "";
        switch (g_current_mode) {
            case INPUT_MODE_KOREAN:
                mode_text = "한글";
                break;
            case INPUT_MODE_ENGLISH_LOWER:
                mode_text = "English";
                break;
            case INPUT_MODE_ENGLISH_UPPER:
                mode_text = "English";
                break;
            case INPUT_MODE_NUMBER:
                mode_text = "123";
                break;
        }
        lv_label_set_text(g_mode_label, mode_text);
    }

    // Update shift button appearance
    if (g_shift_btn) {
        lv_obj_t* shift_label = lv_obj_get_child(g_shift_btn, 0);

        if ((g_shift_mode && g_current_mode == INPUT_MODE_KOREAN) ||
            (g_current_mode == INPUT_MODE_ENGLISH_UPPER) ||
            (g_number_shift_active && g_current_mode == INPUT_MODE_NUMBER)) {
            // Orange color when shift is active in any mode
            lv_obj_set_style_bg_color(g_shift_btn, lv_color_make(255, 140, 0), 0);
            lv_label_set_text(shift_label, "⇧Shift");
        } else if (g_current_mode == INPUT_MODE_NUMBER) {
            // Set dark gray button color for inactive shift in number mode
            lv_obj_set_style_bg_color(g_shift_btn, lv_color_make(192, 192, 192), 0);
            lv_label_set_text(shift_label, "⇧Shift");
        } else {
            // Set dark gray button color for inactive shift
            lv_obj_set_style_bg_color(g_shift_btn, lv_color_make(192, 192, 192), 0);
            lv_label_set_text(shift_label, "⇧Shift");
        }
    }
}

// Key press callback - unified to append to wide char buffer
static void qwerty_key_cb(lv_event_t* e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;

    lv_obj_t* btn = lv_event_get_target(e);
    lv_obj_t* label = lv_obj_get_child(btn, 0);
    const char* key_text = lv_label_get_text(label);

    if (!key_text || strlen(key_text) == 0) return;

    if (g_current_mode == INPUT_MODE_KOREAN) {
        // Korean mode - compose character and append to unified buffer
        char input_char = map_korean_to_english(key_text);
        if (input_char != '\0') {
            // Add to input buffer for composition
            if (g_qwerty_input_len < MAX_OUTPUT_LEN - 1) {
                g_qwerty_input_buffer[g_qwerty_input_len++] = input_char;
                g_qwerty_input_buffer[g_qwerty_input_len] = '\0';
            }

            // Compose the Korean character from input buffer
            qwerty_compose_korean_characters(g_qwerty_input_buffer, g_qwerty_input_len, g_korean_temp_buffer);

            // Replace the Korean composition section (from start pos to end)
            size_t korean_len = wcslen(g_korean_temp_buffer);
            if (g_korean_start_pos + korean_len < MAX_OUTPUT_LEN) {
                wcscpy(g_qwerty_output_buffer + g_korean_start_pos, g_korean_temp_buffer);
                g_qwerty_output_len = g_korean_start_pos + korean_len;
                g_qwerty_output_buffer[g_qwerty_output_len] = L'\0';
            }
        }
    } else {
        // English/Number mode - convert to wide char and append to unified buffer
        if (g_qwerty_output_len < MAX_OUTPUT_LEN - 1) {
            wchar_t wc;
            mbtowc(&wc, key_text, MB_CUR_MAX);
            g_qwerty_output_buffer[g_qwerty_output_len++] = wc;
            g_qwerty_output_buffer[g_qwerty_output_len] = L'\0';
        }
    }

    update_qwerty_display();
}

// Mode switching callback
static void mode_switch_cb(lv_event_t* e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;

    InputMode* target_mode = (InputMode*)lv_event_get_user_data(e);
    if (target_mode) {
        g_current_mode = *target_mode;
        g_shift_mode = false;  // Reset shift when changing modes
        
        // If switching to English mode, start with lowercase
        if (g_current_mode == INPUT_MODE_ENGLISH_UPPER) {
            g_current_mode = INPUT_MODE_ENGLISH_LOWER;
        }
        
        update_keyboard_layout();
    }
}

// Cycle mode callback (Korean -> English -> Number -> Korean)
static void cycle_mode_cb(lv_event_t* e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;

    // When leaving Korean mode, finalize any pending composition
    if (g_current_mode == INPUT_MODE_KOREAN && g_qwerty_input_len > 0) {
        // Korean composition is already in the output buffer
        // Just clear the input buffers
        memset(g_qwerty_input_buffer, 0, sizeof(g_qwerty_input_buffer));
        memset(g_korean_temp_buffer, 0, sizeof(g_korean_temp_buffer));
        g_qwerty_input_len = 0;
    }

    switch (g_current_mode) {
        case INPUT_MODE_KOREAN:
            g_current_mode = INPUT_MODE_ENGLISH_LOWER;
            break;
        case INPUT_MODE_ENGLISH_LOWER:
        case INPUT_MODE_ENGLISH_UPPER:
            g_current_mode = INPUT_MODE_NUMBER;
            break;
        case INPUT_MODE_NUMBER:
            g_current_mode = INPUT_MODE_KOREAN;
            // Set Korean start position to current output length (append mode)
            g_korean_start_pos = g_qwerty_output_len;
            // Reset Korean composition buffers
            memset(g_qwerty_input_buffer, 0, sizeof(g_qwerty_input_buffer));
            memset(g_korean_temp_buffer, 0, sizeof(g_korean_temp_buffer));
            g_qwerty_input_len = 0;
            break;
    }
    g_shift_mode = false;
    g_number_shift_active = false;  // Reset number shift when changing modes
    update_keyboard_layout();
    update_qwerty_display();
}

// Shift callback
static void shift_cb(lv_event_t* e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;

    if (g_current_mode == INPUT_MODE_KOREAN) {
        // Korean mode: toggle shift mode for double consonants/vowels
        g_shift_mode = !g_shift_mode;
        update_keyboard_layout();
    } else if (g_current_mode == INPUT_MODE_ENGLISH_LOWER) {
        // English lowercase -> uppercase
        g_current_mode = INPUT_MODE_ENGLISH_UPPER;
        update_keyboard_layout();
    } else if (g_current_mode == INPUT_MODE_ENGLISH_UPPER) {
        // English uppercase -> lowercase
        g_current_mode = INPUT_MODE_ENGLISH_LOWER;
        update_keyboard_layout();
    } else if (g_current_mode == INPUT_MODE_NUMBER) {
        // Number mode: toggle shift to show different special characters
        g_number_shift_active = !g_number_shift_active;
        update_keyboard_layout();
    }
}

// Space callback - unified buffer
static void space_cb(lv_event_t* e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;

    if (g_current_mode == INPUT_MODE_KOREAN) {
        // Finalize any pending Korean composition
        if (g_qwerty_input_len > 0) {
            // Korean composition is already in the output buffer
            // Just clear the input buffer and update the start position
            memset(g_qwerty_input_buffer, 0, sizeof(g_qwerty_input_buffer));
            memset(g_korean_temp_buffer, 0, sizeof(g_korean_temp_buffer));
            g_qwerty_input_len = 0;
        }
    }

    // Add space to unified buffer (for all modes)
    if (g_qwerty_output_len < MAX_OUTPUT_LEN - 1) {
        g_qwerty_output_buffer[g_qwerty_output_len++] = L' ';
        g_qwerty_output_buffer[g_qwerty_output_len] = L'\0';
    }

    // Update Korean start position to after the space
    if (g_current_mode == INPUT_MODE_KOREAN) {
        g_korean_start_pos = g_qwerty_output_len;
    }

    update_qwerty_display();
}

// Backspace callback - unified buffer (removes one character at a time)
static void backspace_cb(lv_event_t* e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;

    // Simply remove the last character from the unified output buffer
    if (g_qwerty_output_len > 0) {
        g_qwerty_output_buffer[--g_qwerty_output_len] = L'\0';

        // If we're in Korean mode, also clear the Korean input buffer
        // since the display no longer matches the composition state
        if (g_current_mode == INPUT_MODE_KOREAN) {
            memset(g_qwerty_input_buffer, 0, sizeof(g_qwerty_input_buffer));
            memset(g_korean_temp_buffer, 0, sizeof(g_korean_temp_buffer));
            g_qwerty_input_len = 0;
            g_korean_start_pos = g_qwerty_output_len;
        }
    }
    update_qwerty_display();
}

// Popup close callback
static void popup_close_cb(lv_event_t* e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;

    printf("[DEBUG] Popup close button clicked\n");
    lv_obj_t* popup = (lv_obj_t*)lv_event_get_user_data(e);
    if (popup) {
        printf("[DEBUG] Deleting popup object\n");
        lv_obj_del(popup);
        printf("[DEBUG] Popup deleted successfully\n");
    } else {
        printf("[ERROR] Popup object is NULL!\n");
    }
}

// Enter callback - show popup and clear textbox
static void enter_cb(lv_event_t* e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;
    
    printf("[DEBUG] Enter key pressed - buffer length: %zu\n", g_qwerty_output_len);

    // Finalize any pending Korean composition
    if (g_current_mode == INPUT_MODE_KOREAN) {
        if (g_qwerty_input_len > 0) {
            printf("[DEBUG] Finalizing Korean composition\n");
            // Korean composition is already in the output buffer
            // Just clear the input buffer
            memset(g_qwerty_input_buffer, 0, sizeof(g_qwerty_input_buffer));
            memset(g_korean_temp_buffer, 0, sizeof(g_korean_temp_buffer));
            g_qwerty_input_len = 0;
        }
    }

    // Only show popup if there's text entered
    if (g_qwerty_output_len > 0) {
        // Convert wide char buffer to UTF-8 for display with proper bounds checking
        char display_text[512] = "";
        size_t converted = wcstombs(display_text, g_qwerty_output_buffer, sizeof(display_text) - 1);
        if (converted == (size_t)-1) {
            // Conversion failed, use a safe fallback
            snprintf(display_text, sizeof(display_text), "Text conversion error");
        } else {
            display_text[converted] = '\0';  // Ensure null termination
        }

        // Get Korean font for the popup text
        lv_font_t* display_font = get_korean_font();
        if (display_font == NULL) {
            display_font = (lv_font_t*)lv_font_get_default();  // Cast to remove const qualifier
        }

        // Create popup message box using manual approach instead of lv_msgbox
        printf("[DEBUG] Creating popup with text: %s\n", display_text);
        
        // Create a modal background
        lv_obj_t* popup = lv_obj_create(lv_scr_act());
        if (popup == NULL) {
            printf("[ERROR] Failed to create popup!\n");
            return;
        }
        
        lv_obj_set_size(popup, 400, 200);
        lv_obj_center(popup);
        lv_obj_add_flag(popup, LV_OBJ_FLAG_CLICK_FOCUSABLE);
        
        // Add title
        lv_obj_t* title = lv_label_create(popup);
        lv_label_set_text(title, "Entered Text");
        lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
        
        // Add text content
        lv_obj_t* text_obj = lv_label_create(popup);
        lv_label_set_text(text_obj, display_text);
        lv_obj_align(text_obj, LV_ALIGN_CENTER, 0, -10);
        lv_label_set_long_mode(text_obj, LV_LABEL_LONG_WRAP);
        lv_obj_set_width(text_obj, 350);
        
        // Add close button
        lv_obj_t* close_btn = lv_btn_create(popup);
        lv_obj_set_size(close_btn, 80, 40);
        lv_obj_align(close_btn, LV_ALIGN_BOTTOM_MID, 0, -10);
        
        lv_obj_t* close_label = lv_label_create(close_btn);
        lv_label_set_text(close_label, "Close");
        lv_obj_center(close_label);
        
        printf("[DEBUG] Popup created successfully\n");

        // Apply Korean font to the text content
        if (text_obj) {
            lv_obj_set_style_text_font(text_obj, display_font, 0);
        }

        lv_obj_add_event_cb(close_btn, popup_close_cb, LV_EVENT_CLICKED, popup);
        lv_obj_center(popup);

        // Clear all buffers after showing popup
        printf("[DEBUG] Clearing buffers after popup creation\n");
        memset(g_qwerty_input_buffer, 0, sizeof(g_qwerty_input_buffer));
        memset(g_qwerty_output_buffer, 0, sizeof(g_qwerty_output_buffer));
        memset(g_korean_temp_buffer, 0, sizeof(g_korean_temp_buffer));
        g_qwerty_input_len = 0;
        g_qwerty_output_len = 0;
        g_korean_start_pos = 0;
        
        printf("[DEBUG] Updating display after buffer clear\n");
        update_qwerty_display();
        printf("[DEBUG] Enter callback completed successfully\n");
    } else {
        printf("[DEBUG] No text to display - buffer is empty\n");
    }
}

// Clear callback - unified buffer
static void clear_cb(lv_event_t* e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;

    memset(g_qwerty_input_buffer, 0, sizeof(g_qwerty_input_buffer));
    memset(g_qwerty_output_buffer, 0, sizeof(g_qwerty_output_buffer));
    g_qwerty_input_len = 0;
    g_qwerty_output_len = 0;
    update_qwerty_display();
}

// Mode selector storage (needs to persist)
static InputMode mode_korean = INPUT_MODE_KOREAN;
static InputMode mode_eng_lower = INPUT_MODE_ENGLISH_LOWER;
static InputMode mode_number = INPUT_MODE_NUMBER;

// Create QWERTY tab
lv_obj_t* create_qwerty_tab(lv_obj_t* parent) {
    // Initialize Korean input system
    qwerty_korean_init();

    // Reset buffers
    memset(g_qwerty_input_buffer, 0, sizeof(g_qwerty_input_buffer));
    memset(g_qwerty_output_buffer, 0, sizeof(g_qwerty_output_buffer));
    memset(g_korean_temp_buffer, 0, sizeof(g_korean_temp_buffer));
    g_qwerty_input_len = 0;
    g_qwerty_output_len = 0;
    g_korean_start_pos = 0;
    g_current_mode = INPUT_MODE_KOREAN;
    g_shift_mode = false;
    g_number_shift_active = false;

    // Get fonts - use 16px for buttons
    lv_font_t* font = get_korean_font_small();  // 16px font for buttons
    if (font == NULL) font = (lv_font_t*)&lv_font_montserrat_14;

    // Larger font for display area
    lv_font_t* display_font = get_korean_font();
    if (display_font == NULL) display_font = font;

    // Create main container
    lv_obj_t* tab = lv_obj_create(parent);
    lv_obj_set_size(tab, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_all(tab, 10, 0);

    // Layout parameters
    int top_row_y = 5;
    int mode_btn_width = 70;
    int mode_btn_height = 30;
    int side_margin = 65;  // Margin from edges
    int display_height = 60;  // Increased height from 50 to 60

    // Mode cycle button (left)
    lv_obj_t* btn_mode = lv_btn_create(tab);
    lv_obj_set_size(btn_mode, mode_btn_width, mode_btn_height);
    lv_obj_align(btn_mode, LV_ALIGN_TOP_LEFT, side_margin, top_row_y);
    lv_obj_set_style_radius(btn_mode, 4, 0);  // Rectangular shape
    lv_obj_t* label_mode = lv_label_create(btn_mode);
    lv_label_set_text(label_mode, "Mode");
    lv_obj_set_style_text_font(label_mode, font, 0);
    lv_obj_center(label_mode);
    lv_obj_add_event_cb(btn_mode, cycle_mode_cb, LV_EVENT_CLICKED, NULL);

    // Current mode label (center top)
    g_mode_label = lv_label_create(tab);
    lv_obj_align(g_mode_label, LV_ALIGN_TOP_MID, 0, top_row_y + 5);
    lv_obj_set_style_text_font(g_mode_label, font, 0);
    lv_label_set_text(g_mode_label, "한글");

    // Clear button (right)
    lv_obj_t* top_clear_btn = lv_btn_create(tab);
    lv_obj_set_size(top_clear_btn, mode_btn_width, mode_btn_height);
    lv_obj_align(top_clear_btn, LV_ALIGN_TOP_RIGHT, -side_margin, top_row_y);
    lv_obj_set_style_radius(top_clear_btn, 4, 0);  // Rectangular shape
    lv_obj_t* top_clear_label = lv_label_create(top_clear_btn);
    lv_label_set_text(top_clear_label, "Clear");
    lv_obj_set_style_text_font(top_clear_label, font, 0);
    lv_obj_center(top_clear_label);
    lv_obj_add_event_cb(top_clear_btn, clear_cb, LV_EVENT_CLICKED, NULL);

    // Create display area - stretches between Mode and Clear buttons with percentage width
    int display_y = top_row_y + mode_btn_height + 5;
    g_qwerty_display_label = lv_label_create(tab);
    lv_obj_set_height(g_qwerty_display_label, display_height);
    lv_obj_set_width(g_qwerty_display_label, LV_PCT(82));  // 82% width for responsive sizing
    lv_obj_align(g_qwerty_display_label, LV_ALIGN_TOP_MID, 0, display_y);
    lv_obj_set_style_bg_color(g_qwerty_display_label, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_bg_opa(g_qwerty_display_label, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(g_qwerty_display_label, lv_color_make(150, 150, 150), 0);
    lv_obj_set_style_border_width(g_qwerty_display_label, 2, 0);
    lv_obj_set_style_pad_all(g_qwerty_display_label, 8, 0);
    lv_obj_set_style_text_font(g_qwerty_display_label, display_font, 0);
    lv_obj_set_style_text_color(g_qwerty_display_label, lv_color_make(0, 0, 0), 0);
    lv_obj_set_style_radius(g_qwerty_display_label, 4, 0);
    lv_label_set_text(g_qwerty_display_label, "");

    // Keyboard layout - larger buttons for bigger window
    int btn_width = 55;
    int btn_height = 42;
    int btn_spacing = 4;
    int start_y = display_y + display_height + 10;

    // Align keyboard with textbox left edge (textbox is at side_margin) + 10px right shift
    int keyboard_left = side_margin + 10;

    // Create QWERTY keyboard buttons (3 rows)
    for (int row = 0; row < 3; row++) {
        // For row 2 (third row), we'll add Shift at start and Backspace at end
        int shift_width = (row == 2) ? 85 : 0;
        int back_width = (row == 2) ? 85 : 0;

        int valid_keys = 0;
        for (int col = 0; col < 10; col++) {
            if (strlen(qwerty_keys_korean[row][col]) > 0) valid_keys++;
        }

        // Row 1 (second row) is centered but shifted right, others aligned left
        bool is_centered = (row == 1);
        int current_x;

        if (is_centered) {
            int total_width = valid_keys * btn_width + (valid_keys - 1) * btn_spacing;
            current_x = -total_width / 2 + 25; // Shift the second row 35px to the right (15px original + 10px additional)
        } else {
            current_x = keyboard_left;
        }

        // Add Shift button at start of row 2
        if (row == 2) {
            g_shift_btn = lv_btn_create(tab);
            lv_obj_set_size(g_shift_btn, shift_width, btn_height);
            lv_obj_align(g_shift_btn, is_centered ? LV_ALIGN_TOP_MID : LV_ALIGN_TOP_LEFT,
                        current_x, start_y + row * (btn_height + btn_spacing));
            lv_obj_set_style_radius(g_shift_btn, 6, 0);
            lv_obj_t* shift_label = lv_label_create(g_shift_btn);
            lv_label_set_text(shift_label, "Shift");
            lv_obj_set_style_text_font(shift_label, font, 0);
            lv_obj_center(shift_label);
            lv_obj_add_event_cb(g_shift_btn, shift_cb, LV_EVENT_CLICKED, NULL);
            current_x += shift_width + btn_spacing;
        }

        int key_index = 0;
        for (int col = 0; col < 10; col++) {
            if (strlen(qwerty_keys_korean[row][col]) == 0) continue;

            lv_obj_t* btn = lv_btn_create(tab);
            lv_obj_set_size(btn, btn_width, btn_height);
            lv_obj_align(btn, is_centered ? LV_ALIGN_TOP_MID : LV_ALIGN_TOP_LEFT,
                        current_x, start_y + row * (btn_height + btn_spacing));
            lv_obj_set_style_radius(btn, 6, 0);
            lv_obj_set_style_shadow_width(btn, 2, 0);
            lv_obj_set_style_shadow_opa(btn, LV_OPA_30, 0);

            lv_obj_t* label = lv_label_create(btn);
            lv_label_set_text(label, qwerty_keys_korean[row][col]);
            lv_obj_set_style_text_font(label, font, 0);
            lv_obj_center(label);

            lv_obj_add_event_cb(btn, qwerty_key_cb, LV_EVENT_CLICKED, NULL);
            g_qwerty_buttons[row][col] = btn;

            current_x += btn_width + btn_spacing;
            key_index++;
        }

        // Add Backspace button at end of row 2
        if (row == 2) {
            lv_obj_t* back_btn = lv_btn_create(tab);
            lv_obj_set_size(back_btn, back_width, btn_height);
            lv_obj_align(back_btn, is_centered ? LV_ALIGN_TOP_MID : LV_ALIGN_TOP_LEFT,
                        current_x, start_y + row * (btn_height + btn_spacing));
            lv_obj_set_style_radius(back_btn, 6, 0);
            lv_obj_t* back_label = lv_label_create(back_btn);
            lv_label_set_text(back_label, "←");
            lv_obj_set_style_text_font(back_label, font, 0);
            lv_obj_center(back_label);
            lv_obj_add_event_cb(back_btn, backspace_cb, LV_EVENT_CLICKED, NULL);
        }
    }

    // Bottom special keys row - Space and Enter, centered horizontally
    int special_y = start_y + 3 * (btn_height + btn_spacing) + 4;
    int space_width = 5 * btn_width + 4 * btn_spacing;  // 5 button widths + 4 gaps = 291px
    int enter_width = 2 * btn_width + btn_spacing;      // 2 button widths + 1 gap = 114px
    
    // Calculate total width and center position for the 4th row
    int total_4th_row_width = space_width + btn_spacing + enter_width;
    int center_start_x = (800 - total_4th_row_width) / 2 - 32;  // Center in 800px window

    // Space button
    lv_obj_t* space_btn = lv_btn_create(tab);
    lv_obj_set_size(space_btn, space_width, btn_height);
    lv_obj_align(space_btn, LV_ALIGN_TOP_LEFT, center_start_x, special_y);
    lv_obj_set_style_radius(space_btn, 6, 0);
    lv_obj_t* space_label = lv_label_create(space_btn);
    lv_label_set_text(space_label, "Space");
    lv_obj_set_style_text_font(space_label, font, 0);
    lv_obj_center(space_label);
    lv_obj_add_event_cb(space_btn, space_cb, LV_EVENT_CLICKED, NULL);

    // Enter button
    lv_obj_t* enter_btn = lv_btn_create(tab);
    lv_obj_set_size(enter_btn, enter_width, btn_height);
    lv_obj_align(enter_btn, LV_ALIGN_TOP_LEFT,
                center_start_x + space_width + btn_spacing, special_y);
    lv_obj_set_style_radius(enter_btn, 6, 0);
    lv_obj_t* enter_label = lv_label_create(enter_btn);
    lv_label_set_text(enter_label, "Enter");
    lv_obj_set_style_text_font(enter_label, font, 0);
    lv_obj_center(enter_label);
    lv_obj_add_event_cb(enter_btn, enter_cb, LV_EVENT_CLICKED, NULL);

    // Initialize keyboard layout and shift button colors
    update_keyboard_layout();

    return tab;
}
