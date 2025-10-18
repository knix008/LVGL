/*
 * Japanese Input Method - Header File
 * Supports Hiragana, Katakana, English, Numbers, and Special Characters
 */

#ifndef JAPANESE_INPUT_H
#define JAPANESE_INPUT_H

#include <stdint.h>
#include <stdbool.h>
#include <wchar.h>

// Maximum text buffer length
#define MAX_TEXT_LEN 1000

// Input modes
typedef enum {
    MODE_JAPANESE = 0,    // 日本語 (Japanese - Hiragana/Katakana controlled by shift)
    MODE_ALPHABET = 1,    // アルファベット (Alphabet)
    MODE_NUMBER = 2,      // 数字 (Numbers)
    MODE_SYMBOL = 3,      // 記号 (Symbols)
    MODE_COUNT = 4
} InputMode;

// Japanese input state
typedef struct {
    InputMode now_mode;           // Current input mode
    wchar_t text_buffer[MAX_TEXT_LEN]; // Text buffer (UTF-32)
    int cursor_pos;               // Cursor position
    bool shift_mode;              // Shift mode for alphabet
    wchar_t composing_buffer[10]; // Buffer for composing characters
    int composing_len;            // Length of composing buffer
    int flick_button;             // Currently showing flick options for this button (-1 if none)
    bool flick_active;            // Whether flick selection is active
} JapaneseInputState;

// Core input logic functions
void japanese_input_init(JapaneseInputState *state);
void japanese_input_process_input(JapaneseInputState *state, int button_num);
void japanese_input_show_flick(JapaneseInputState *state, int button_num);
void japanese_input_select_flick_char(JapaneseInputState *state, int button_num, int char_index);
void japanese_input_cancel_flick(JapaneseInputState *state);
void change_input_mode(JapaneseInputState *state);

// Character mapping functions
const wchar_t* get_button_flick_chars(InputMode mode, int button_num);
const wchar_t* get_button_flick_chars_with_shift(InputMode mode, int button_num, bool shift_mode);
int get_button_char_count(InputMode mode, int button_num);
const char* get_button_alphabet_chars(int button_num);
const char* get_button_number_chars(int button_num);
const char* get_mode_name(InputMode mode);

// GUI utility functions
const wchar_t* get_button_text(InputMode mode, int button_num);
const wchar_t* get_button_text_with_shift(InputMode mode, int button_num, bool shift_mode);

// Utility functions
char* wchar_to_utf8(const wchar_t* wstr, int max_len);

// Button mapping for mobile phone keypad
// 12-button layout:
// 1(あいう) 2(かきく) 3(さしす) 4(たちつ) 5(なにぬ) 6(はひふ) 7(まみむ) 8(やゆよ) 9(らりる) *() +() 0(わをん)

#endif // JAPANESE_INPUT_H
