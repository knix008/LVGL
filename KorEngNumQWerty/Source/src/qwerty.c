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

// QWERTY Korean input system - using qwerty_korean.c functions
static char g_qwerty_input_buffer[MAX_OUTPUT_LEN] = "";
static size_t g_qwerty_input_len = 0;
static wchar_t g_qwerty_output_buffer[MAX_OUTPUT_LEN] = L"";

// Current input mode
static InputMode g_current_mode = INPUT_MODE_KOREAN;
static bool g_shift_mode = false;

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

// Number and symbol layout
static const char* qwerty_keys_number[3][10] = {
    {"1", "2", "3", "4", "5", "6", "7", "8", "9", "0"},
    {"-", "/", ":", ";", "(", ")", "$", "&", "@", "\""},
    {".", ",", "?", "!", "'", "", "", "", "", ""}
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

// Update display with current text
static void update_qwerty_display(void) {
    if (g_qwerty_display_label) {
        if (g_current_mode == INPUT_MODE_KOREAN) {
            // Use wide character output for Korean
            char display_text[512] = "";
            wcstombs(display_text, g_qwerty_output_buffer, sizeof(display_text) - 1);
            lv_label_set_text(g_qwerty_display_label, display_text);
        } else {
            // Use regular input buffer for English/Numbers
            lv_label_set_text(g_qwerty_display_label, g_qwerty_input_buffer);
        }
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
            current_layout = qwerty_keys_number;
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
                mode_text = g_shift_mode ? "한글 (Shift)" : "한글";
                break;
            case INPUT_MODE_ENGLISH_LOWER:
                mode_text = "English (abc)";
                break;
            case INPUT_MODE_ENGLISH_UPPER:
                mode_text = "English (ABC)";
                break;
            case INPUT_MODE_NUMBER:
                mode_text = "123";
                break;
        }
        lv_label_set_text(g_mode_label, mode_text);
    }

    // Update shift button appearance
    if (g_shift_btn) {
        if (g_shift_mode && g_current_mode == INPUT_MODE_KOREAN) {
            lv_obj_set_style_bg_color(g_shift_btn, lv_color_make(100, 150, 255), 0);
        } else {
            lv_obj_set_style_bg_color(g_shift_btn, lv_color_make(200, 200, 200), 0);
        }
    }
}

// Key press callback
static void qwerty_key_cb(lv_event_t* e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;

    lv_obj_t* btn = lv_event_get_target(e);
    lv_obj_t* label = lv_obj_get_child(btn, 0);
    const char* key_text = lv_label_get_text(label);

    if (!key_text || strlen(key_text) == 0) return;

    if (g_current_mode == INPUT_MODE_KOREAN) {
        // Korean mode - use qwerty_korean system
        char input_char = map_korean_to_english(key_text);
        if (input_char != '\0') {
            qwerty_process_input(g_qwerty_input_buffer, &g_qwerty_input_len,
                               g_qwerty_output_buffer, input_char);
        }
    } else {
        // English/Number mode - direct append
        if (g_qwerty_input_len < MAX_OUTPUT_LEN - 1) {
            strncat(g_qwerty_input_buffer, key_text, 1);
            g_qwerty_input_len++;
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
        update_keyboard_layout();
    }
}

// Shift callback
static void shift_cb(lv_event_t* e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;

    if (g_current_mode == INPUT_MODE_KOREAN) {
        g_shift_mode = !g_shift_mode;
        update_keyboard_layout();
    } else if (g_current_mode == INPUT_MODE_ENGLISH_LOWER) {
        g_current_mode = INPUT_MODE_ENGLISH_UPPER;
        update_keyboard_layout();
    } else if (g_current_mode == INPUT_MODE_ENGLISH_UPPER) {
        g_current_mode = INPUT_MODE_ENGLISH_LOWER;
        update_keyboard_layout();
    }
}

// Space callback
static void space_cb(lv_event_t* e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;

    if (g_current_mode == INPUT_MODE_KOREAN) {
        qwerty_process_input(g_qwerty_input_buffer, &g_qwerty_input_len,
                           g_qwerty_output_buffer, ' ');
    } else {
        if (g_qwerty_input_len < MAX_OUTPUT_LEN - 1) {
            strcat(g_qwerty_input_buffer, " ");
            g_qwerty_input_len++;
        }
    }
    update_qwerty_display();
}

// Backspace callback
static void backspace_cb(lv_event_t* e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;

    if (g_current_mode == INPUT_MODE_KOREAN) {
        qwerty_process_input(g_qwerty_input_buffer, &g_qwerty_input_len,
                           g_qwerty_output_buffer, 0x7f);
    } else {
        if (g_qwerty_input_len > 0) {
            g_qwerty_input_buffer[--g_qwerty_input_len] = '\0';
        }
    }
    update_qwerty_display();
}

// Enter callback
static void enter_cb(lv_event_t* e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;

    if (g_current_mode == INPUT_MODE_KOREAN) {
        qwerty_process_input(g_qwerty_input_buffer, &g_qwerty_input_len,
                           g_qwerty_output_buffer, '\n');
    } else {
        if (g_qwerty_input_len < MAX_OUTPUT_LEN - 1) {
            strcat(g_qwerty_input_buffer, "\n");
            g_qwerty_input_len++;
        }
    }
    update_qwerty_display();
}

// Clear callback
static void clear_cb(lv_event_t* e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) return;

    memset(g_qwerty_input_buffer, 0, sizeof(g_qwerty_input_buffer));
    memset(g_qwerty_output_buffer, 0, sizeof(g_qwerty_output_buffer));
    g_qwerty_input_len = 0;
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
    g_qwerty_input_len = 0;
    g_current_mode = INPUT_MODE_KOREAN;
    g_shift_mode = false;

    // Get font - use larger font for better Korean character visibility
    lv_font_t* font = get_korean_font();
    if (font == NULL) font = get_korean_font_small();
    if (font == NULL) font = (lv_font_t*)&lv_font_montserrat_14;

    // Create main container
    lv_obj_t* tab = lv_obj_create(parent);
    lv_obj_set_size(tab, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_all(tab, 10, 0);

    // Top row: Mode buttons, current mode label, and clear button
    int top_row_y = 10;
    int mode_btn_width = 70;
    int mode_btn_height = 30;
    int mode_btn_spacing = 8;

    // Korean mode button (left)
    lv_obj_t* btn_korean = lv_btn_create(tab);
    lv_obj_set_size(btn_korean, mode_btn_width, mode_btn_height);
    lv_obj_align(btn_korean, LV_ALIGN_TOP_LEFT, 20, top_row_y);
    lv_obj_t* label_korean = lv_label_create(btn_korean);
    lv_label_set_text(label_korean, "한글");
    lv_obj_set_style_text_font(label_korean, font, 0);
    lv_obj_center(label_korean);
    lv_obj_add_event_cb(btn_korean, mode_switch_cb, LV_EVENT_CLICKED, &mode_korean);

    // English mode button
    lv_obj_t* btn_english = lv_btn_create(tab);
    lv_obj_set_size(btn_english, mode_btn_width, mode_btn_height);
    lv_obj_align(btn_english, LV_ALIGN_TOP_LEFT, 20 + mode_btn_width + mode_btn_spacing, top_row_y);
    lv_obj_t* label_english = lv_label_create(btn_english);
    lv_label_set_text(label_english, "Eng");
    lv_obj_set_style_text_font(label_english, font, 0);
    lv_obj_center(label_english);
    lv_obj_add_event_cb(btn_english, mode_switch_cb, LV_EVENT_CLICKED, &mode_eng_lower);

    // Number mode button
    lv_obj_t* btn_number = lv_btn_create(tab);
    lv_obj_set_size(btn_number, mode_btn_width, mode_btn_height);
    lv_obj_align(btn_number, LV_ALIGN_TOP_LEFT, 20 + 2 * (mode_btn_width + mode_btn_spacing), top_row_y);
    lv_obj_t* label_number = lv_label_create(btn_number);
    lv_label_set_text(label_number, "123");
    lv_obj_set_style_text_font(label_number, font, 0);
    lv_obj_center(label_number);
    lv_obj_add_event_cb(btn_number, mode_switch_cb, LV_EVENT_CLICKED, &mode_number);

    // Current mode label (center)
    g_mode_label = lv_label_create(tab);
    lv_obj_align(g_mode_label, LV_ALIGN_TOP_MID, 0, top_row_y + 5);
    lv_obj_set_style_text_font(g_mode_label, font, 0);
    lv_label_set_text(g_mode_label, "한글");

    // Clear button (right)
    lv_obj_t* top_clear_btn = lv_btn_create(tab);
    lv_obj_set_size(top_clear_btn, 80, mode_btn_height);
    lv_obj_align(top_clear_btn, LV_ALIGN_TOP_RIGHT, -20, top_row_y);
    lv_obj_t* top_clear_label = lv_label_create(top_clear_btn);
    lv_label_set_text(top_clear_label, "Clear");
    lv_obj_set_style_text_font(top_clear_label, font, 0);
    lv_obj_center(top_clear_label);
    lv_obj_add_event_cb(top_clear_btn, clear_cb, LV_EVENT_CLICKED, NULL);

    // Create display area - below the top buttons
    g_qwerty_display_label = lv_label_create(tab);
    lv_obj_set_size(g_qwerty_display_label, 700, 60);
    lv_obj_align(g_qwerty_display_label, LV_ALIGN_TOP_MID, 0, 50);
    lv_obj_set_style_bg_color(g_qwerty_display_label, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_bg_opa(g_qwerty_display_label, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(g_qwerty_display_label, lv_color_make(150, 150, 150), 0);
    lv_obj_set_style_border_width(g_qwerty_display_label, 2, 0);
    lv_obj_set_style_pad_all(g_qwerty_display_label, 12, 0);
    lv_obj_set_style_text_font(g_qwerty_display_label, font, 0);
    lv_obj_set_style_text_color(g_qwerty_display_label, lv_color_make(0, 0, 0), 0);
    lv_obj_set_style_radius(g_qwerty_display_label, 8, 0);
    lv_label_set_text(g_qwerty_display_label, "");

    // Keyboard layout - mobile-style proportions
    int btn_width = 68;
    int btn_height = 50;
    int btn_spacing = 4;
    int start_y = 125;

    // Create QWERTY keyboard buttons (3 rows)
    for (int row = 0; row < 3; row++) {
        int valid_keys = 0;
        for (int col = 0; col < 10; col++) {
            if (strlen(qwerty_keys_korean[row][col]) > 0) valid_keys++;
        }

        int total_width = valid_keys * btn_width + (valid_keys - 1) * btn_spacing;
        int center_offset = -total_width / 2;

        int key_index = 0;
        for (int col = 0; col < 10; col++) {
            if (strlen(qwerty_keys_korean[row][col]) == 0) continue;

            lv_obj_t* btn = lv_btn_create(tab);
            lv_obj_set_size(btn, btn_width, btn_height);
            lv_obj_align(btn, LV_ALIGN_TOP_MID,
                        center_offset + key_index * (btn_width + btn_spacing),
                        start_y + row * (btn_height + btn_spacing));
            lv_obj_set_style_radius(btn, 6, 0);
            lv_obj_set_style_shadow_width(btn, 2, 0);
            lv_obj_set_style_shadow_opa(btn, LV_OPA_30, 0);

            lv_obj_t* label = lv_label_create(btn);
            lv_label_set_text(label, qwerty_keys_korean[row][col]);
            lv_obj_set_style_text_font(label, font, 0);
            lv_obj_center(label);

            lv_obj_add_event_cb(btn, qwerty_key_cb, LV_EVENT_CLICKED, NULL);
            g_qwerty_buttons[row][col] = btn;

            key_index++;
        }
    }

    // Bottom special keys row - only Shift, Space, Backspace, Enter (Clear moved to top)
    int special_y = start_y + 3 * (btn_height + btn_spacing) + 8;
    int shift_width = 100;
    int space_width = 280;
    int back_width = 100;
    int enter_width = 100;

    int total_special = shift_width + space_width + back_width + enter_width + 3 * btn_spacing;
    int special_offset = -total_special / 2;

    // Shift button
    g_shift_btn = lv_btn_create(tab);
    lv_obj_set_size(g_shift_btn, shift_width, btn_height);
    lv_obj_align(g_shift_btn, LV_ALIGN_TOP_MID, special_offset, special_y);
    lv_obj_set_style_radius(g_shift_btn, 6, 0);
    lv_obj_t* shift_label = lv_label_create(g_shift_btn);
    lv_label_set_text(shift_label, "Shift");
    lv_obj_set_style_text_font(shift_label, font, 0);
    lv_obj_center(shift_label);
    lv_obj_add_event_cb(g_shift_btn, shift_cb, LV_EVENT_CLICKED, NULL);

    // Space button
    lv_obj_t* space_btn = lv_btn_create(tab);
    lv_obj_set_size(space_btn, space_width, btn_height);
    lv_obj_align(space_btn, LV_ALIGN_TOP_MID,
                special_offset + shift_width + btn_spacing, special_y);
    lv_obj_set_style_radius(space_btn, 6, 0);
    lv_obj_t* space_label = lv_label_create(space_btn);
    lv_label_set_text(space_label, "Space");
    lv_obj_set_style_text_font(space_label, font, 0);
    lv_obj_center(space_label);
    lv_obj_add_event_cb(space_btn, space_cb, LV_EVENT_CLICKED, NULL);

    // Backspace button
    lv_obj_t* back_btn = lv_btn_create(tab);
    lv_obj_set_size(back_btn, back_width, btn_height);
    lv_obj_align(back_btn, LV_ALIGN_TOP_MID,
                special_offset + shift_width + space_width + 2 * btn_spacing, special_y);
    lv_obj_set_style_radius(back_btn, 6, 0);
    lv_obj_t* back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "←");
    lv_obj_set_style_text_font(back_label, font, 0);
    lv_obj_center(back_label);
    lv_obj_add_event_cb(back_btn, backspace_cb, LV_EVENT_CLICKED, NULL);

    // Enter button
    lv_obj_t* enter_btn = lv_btn_create(tab);
    lv_obj_set_size(enter_btn, enter_width, btn_height);
    lv_obj_align(enter_btn, LV_ALIGN_TOP_MID,
                special_offset + shift_width + space_width + back_width + 3 * btn_spacing, special_y);
    lv_obj_set_style_radius(enter_btn, 6, 0);
    lv_obj_t* enter_label = lv_label_create(enter_btn);
    lv_label_set_text(enter_label, "Enter");
    lv_obj_set_style_text_font(enter_label, font, 0);
    lv_obj_center(enter_label);
    lv_obj_add_event_cb(enter_btn, enter_cb, LV_EVENT_CLICKED, NULL);

    return tab;
}
