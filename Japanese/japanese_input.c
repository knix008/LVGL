/*
 * Japanese Input Method - Implementation
 * Supports Hiragana, Katakana, English, Numbers, and Special Characters
 */

#include "japanese_input.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <locale.h>
#include <wchar.h>
#include <ctype.h>

// Hiragana character mappings for each button (Keitai input method)
// Based on gojūon table: each number corresponds to a column, presses determine row
static const wchar_t* hiragana_chars[12] = {
    L"あいうえお",    // Button 0: あ行 (a-column)
    L"かきくけこ",    // Button 1: か行 (ka-column)
    L"さしすせそ",    // Button 2: さ行 (sa-column)
    L"たちつてと",    // Button 3: た行 (ta-column)
    L"なにぬねの",    // Button 4: な行 (na-column)
    L"はひふへほ",    // Button 5: は行 (ha-column)
    L"まみむめも",    // Button 6: ま行 (ma-column)
    L"やゆよ",        // Button 7: や行 (ya-column) - only 3 characters
    L"らりるれろ",    // Button 8: ら行 (ra-column)
    L"わをん",        // Button 9: わ行 (wa-column) + ん
    L"゛゜",          // Button 10: * (dakuten/handakuten marks)
    L"ー"             // Button 11: # (long vowel mark)
};

// Small hiragana characters (for shift mode)
// If a button doesn't have small versions, we use NULL to indicate fallback to normal
static const wchar_t* hiragana_chars_small[12] = {
    L"ぁぃぅぇぉ",    // Button 0: small あ行
    NULL,             // Button 1: か行 - fallback to normal
    NULL,             // Button 2: さ行 - fallback to normal
    L"っ",            // Button 3: small つ (tsu)
    NULL,             // Button 4: な行 - fallback to normal
    NULL,             // Button 5: は行 - fallback to normal
    NULL,             // Button 6: ま行 - fallback to normal
    L"ゃゅょ",        // Button 7: small や行
    NULL,             // Button 8: ら行 - fallback to normal
    L"ゎ",            // Button 9: small わ
    L"゛゜",          // Button 10: * (dakuten/handakuten marks - same as normal)
    L"ー"             // Button 11: # (long vowel mark - same as normal)
};

// Katakana character mappings for each button (Keitai input method)
static const wchar_t* katakana_chars[12] = {
    L"アイウエオ",    // Button 0: ア行 (a-column)
    L"カキクケコ",    // Button 1: カ行 (ka-column)
    L"サシスセソ",    // Button 2: サ行 (sa-column)
    L"タチツテト",    // Button 3: タ行 (ta-column)
    L"ナニヌネノ",    // Button 4: ナ行 (na-column)
    L"ハヒフヘホ",    // Button 5: ハ行 (ha-column)
    L"マミムメモ",    // Button 6: マ行 (ma-column)
    L"ヤユヨ",        // Button 7: ヤ行 (ya-column) - only 3 characters
    L"ラリルレロ",    // Button 8: ラ行 (ra-column)
    L"ワヲン",        // Button 9: ワ行 (wa-column) + ン
    L"゛゜",          // Button 10: * (dakuten/handakuten marks)
    L"ー"             // Button 11: # (long vowel mark)
};

// Small katakana characters (for shift mode)
// If a button doesn't have small versions, we use NULL to indicate fallback to normal
static const wchar_t* katakana_chars_small[12] = {
    L"ァィゥェォ",    // Button 0: small ア行
    L"ヵヶ",          // Button 1: small カ (ka) and ケ (ke)
    NULL,             // Button 2: サ行 - fallback to normal
    L"ッ",            // Button 3: small ツ (tsu)
    NULL,             // Button 4: ナ行 - fallback to normal
    NULL,             // Button 5: ハ行 - fallback to normal
    NULL,             // Button 6: マ行 - fallback to normal
    L"ャュョ",        // Button 7: small ヤ行
    NULL,             // Button 8: ラ行 - fallback to normal
    L"ヮ",            // Button 9: small ワ
    L"゛゜",          // Button 10: * (dakuten/handakuten marks - same as normal)
    L"ー"             // Button 11: # (long vowel mark - same as normal)
};

// Alphabet mappings for each button
static const char* alphabet_chars[12] = {
    "abc",           // Button 0
    "def",           // Button 1
    "ghi",           // Button 2
    "jkl",           // Button 3
    "mno",           // Button 4
    "pqrs",          // Button 5
    "tuv",           // Button 6
    "wxyz",          // Button 7
    ".,!?",          // Button 8
    "@",             // Button 9 (first button in 4th row)
    "*",             // Button 10
    "#"              // Button 11
};

// Numbers for each button
static const char* number_chars[12] = {
    "1",             // Button 1
    "2",             // Button 2
    "3",             // Button 3
    "4",             // Button 4
    "5",             // Button 5
    "6",             // Button 6
    "7",             // Button 7
    "8",             // Button 8
    "9",             // Button 9
    "0",             // Button 0
    "*",             // Button *
    "#"              // Button #
};

// Special symbols
static const wchar_t* symbol_chars[12] = {
    L"！？",         // Button 0
    L"（）",         // Button 1
    L"「」",         // Button 2
    L"、。",         // Button 3
    L"・：",         // Button 4
    L"；〜",         // Button 5
    L"－＝",         // Button 6
    L"＋×",         // Button 7
    L"÷％",         // Button 8
    L"［］",         // Button 9 (first button in 4th row)
    L"<>/",          // Button 10 (angle brackets and forward slash)
    L"\\{}"          // Button 11 (backslash and curly braces)
};

// Shifted symbol characters (alternative symbols when shift is active)
static const wchar_t* symbol_chars_shifted[12] = {
    L"※◎",         // Button 0 (reference mark and bullseye)
    L"◆◇",         // Button 1 (filled and empty diamond)
    L"▲▼",         // Button 2 (up and down triangle)
    L"◀▶",         // Button 3 (left and right triangle)
    L"○●",         // Button 4 (empty and filled circle)
    L"□■",         // Button 5 (empty and filled square)
    L"△▽",         // Button 6 (triangle up and down)
    L"♠♣",         // Button 7 (spade and club)
    L"♥♦",         // Button 8 (heart and diamond)
    L"★☆",         // Button 9 (filled and empty star)
    L"♪♫",         // Button 10 (eighth note and beamed eighth notes)
    L"∞§"          // Button 11 (infinity and section sign)
};

// Mode names
static const char* mode_names[MODE_COUNT] = {
    "ひらがな",      // Hiragana
    "カタカナ",      // Katakana
    "ABC",           // Alphabet
    "123",           // Numbers
    "記号"           // Symbols
};

// Initialize Japanese input state
void japanese_input_init(JapaneseInputState *state) {
    state->now_mode = MODE_HIRAGANA;
    memset(state->text_buffer, 0, sizeof(state->text_buffer));
    state->cursor_pos = 0;
    state->shift_mode = false;
    memset(state->composing_buffer, 0, sizeof(state->composing_buffer));
    state->composing_len = 0;
    state->flick_button = -1;
    state->flick_active = false;
}

// Change input mode cyclically
void change_input_mode(JapaneseInputState *state) {
    state->now_mode = (state->now_mode + 1) % MODE_COUNT;
}

// Convert wide character string to UTF-8
char* wchar_to_utf8(const wchar_t* wstr, int max_len) {
    (void)max_len; // Suppress unused parameter warning
    if (!wstr) return NULL;
    
    static char utf8_buffer[MAX_TEXT_LEN * 4]; // UTF-8 can be up to 4 bytes per char
    memset(utf8_buffer, 0, sizeof(utf8_buffer));
    
    // Convert wide char to UTF-8
    size_t result = wcstombs(utf8_buffer, wstr, sizeof(utf8_buffer) - 1);
    if (result == (size_t)-1) {
        strcpy(utf8_buffer, "?");
    }
    
    return utf8_buffer;
}

// Show flick input options for a button
void japanese_input_show_flick(JapaneseInputState *state, int button_num) {
    state->flick_button = button_num;
    state->flick_active = true;
}

// Select a character from flick input
void japanese_input_select_flick_char(JapaneseInputState *state, int button_num, int char_index) {
    if (state->cursor_pos >= MAX_TEXT_LEN - 1) {
        return; // Buffer full
    }
    
    wchar_t char_to_add = L'\0';
    const wchar_t* chars = NULL;
    
    switch (state->now_mode) {
        case MODE_HIRAGANA:
            // Use shift_mode to determine normal vs small hiragana
            if (state->shift_mode) {
                chars = hiragana_chars_small[button_num];
                // Fallback to normal if no small version exists
                if (chars == NULL || wcslen(chars) == 0) {
                    chars = hiragana_chars[button_num];
                }
            } else {
                chars = hiragana_chars[button_num];
            }
            break;
        case MODE_KATAKANA:
            // Use shift_mode to determine normal vs small katakana
            if (state->shift_mode) {
                chars = katakana_chars_small[button_num];
                // Fallback to normal if no small version exists
                if (chars == NULL || wcslen(chars) == 0) {
                    chars = katakana_chars[button_num];
                }
            } else {
                chars = katakana_chars[button_num];
            }
            break;
        case MODE_ALPHABET:
            {
                const char* ascii_chars = alphabet_chars[button_num];
                if (ascii_chars && strlen(ascii_chars) > (size_t)char_index) {
                    char c = ascii_chars[char_index];
                    if (state->shift_mode) {
                        c = toupper(c);
                    }
                    char_to_add = (wchar_t)c;
                }
            }
            break;
        case MODE_NUMBER:
            {
                const char* ascii_chars = number_chars[button_num];
                if (ascii_chars && strlen(ascii_chars) > (size_t)char_index) {
                    char_to_add = (wchar_t)ascii_chars[char_index];
                }
            }
            break;
        case MODE_SYMBOL:
            // Use shift_mode to determine normal vs shifted symbols
            chars = state->shift_mode ? symbol_chars_shifted[button_num] : symbol_chars[button_num];
            break;
        case MODE_COUNT:
            // This case should never be reached, but included for completeness
            break;
    }
    
    // For wide character modes (Hiragana, Katakana, Symbol)
    if (chars && wcslen(chars) > (size_t)char_index) {
        char_to_add = chars[char_index];
    }
    
    // Add character to buffer
    if (char_to_add != L'\0') {
        // Shift existing characters to make room
        for (int i = MAX_TEXT_LEN - 1; i > state->cursor_pos; i--) {
            state->text_buffer[i] = state->text_buffer[i - 1];
        }
        
        // Insert new character
        state->text_buffer[state->cursor_pos] = char_to_add;
        state->cursor_pos++;
        
        // Ensure null termination
        if (state->cursor_pos < MAX_TEXT_LEN) {
            state->text_buffer[state->cursor_pos] = L'\0';
        }
    }
    
    // Close flick input
    japanese_input_cancel_flick(state);
}

// Cancel flick input
void japanese_input_cancel_flick(JapaneseInputState *state) {
    state->flick_button = -1;
    state->flick_active = false;
}

// Process input based on button number (now shows flick input)
void japanese_input_process_input(JapaneseInputState *state, int button_num) {
    // If flick is already active for this button, select the first character
    if (state->flick_active && state->flick_button == button_num) {
        japanese_input_select_flick_char(state, button_num, 0);
    } else {
        // Get the actual characters considering shift mode
        const wchar_t* chars = get_button_flick_chars_with_shift(state->now_mode, button_num, state->shift_mode);
        int char_count = (chars && wcslen(chars) > 0) ? wcslen(chars) : 0;

        if (char_count == 1) {
            // Only one character available, input it directly
            japanese_input_select_flick_char(state, button_num, 0);
        } else if (char_count > 1) {
            // Multiple characters available, show flick input
            japanese_input_show_flick(state, button_num);
        }
        // If char_count == 0, do nothing (button has no characters)
    }
}

// Get flick characters for a button with shift mode consideration
const wchar_t* get_button_flick_chars_with_shift(InputMode mode, int button_num, bool shift_mode) {
    if (button_num < 0 || button_num >= 12) {
        return L"";
    }
    
    switch (mode) {
        case MODE_HIRAGANA:
            // Use shift_mode to determine normal vs small hiragana
            if (shift_mode) {
                const wchar_t* small_chars = hiragana_chars_small[button_num];
                // Fallback to normal if no small version exists
                if (small_chars == NULL || wcslen(small_chars) == 0) {
                    return hiragana_chars[button_num];
                }
                return small_chars;
            }
            return hiragana_chars[button_num];
        case MODE_KATAKANA:
            // Use shift_mode to determine normal vs small katakana
            if (shift_mode) {
                const wchar_t* small_chars = katakana_chars_small[button_num];
                // Fallback to normal if no small version exists
                if (small_chars == NULL || wcslen(small_chars) == 0) {
                    return katakana_chars[button_num];
                }
                return small_chars;
            }
            return katakana_chars[button_num];
        case MODE_ALPHABET:
            {
                const char* ascii_chars = alphabet_chars[button_num];
                if (ascii_chars && strlen(ascii_chars) > 0) {
                    static wchar_t shifted_chars[20];
                    int len = strlen(ascii_chars);
                    for (int i = 0; i < len && i < 19; i++) {
                        char c = ascii_chars[i];
                        if (shift_mode) {
                            c = toupper(c);
                        }
                        shifted_chars[i] = (wchar_t)c;
                    }
                    shifted_chars[len] = L'\0';
                    return shifted_chars;
                }
                return L"";
            }
        case MODE_NUMBER:
            {
                const char* ascii_chars = number_chars[button_num];
                if (ascii_chars && strlen(ascii_chars) > 0) {
                    static wchar_t w_chars[20];
                    int len = strlen(ascii_chars);
                    for (int i = 0; i < len && i < 19; i++) {
                        w_chars[i] = (wchar_t)ascii_chars[i];
                    }
                    w_chars[len] = L'\0';
                    return w_chars;
                }
                return L"";
            }
        case MODE_SYMBOL:
            return shift_mode ? symbol_chars_shifted[button_num] : symbol_chars[button_num];
        default:
            return L"";
    }
}

// Get flick characters for a button
const wchar_t* get_button_flick_chars(InputMode mode, int button_num) {
    if (button_num < 0 || button_num >= 12) {
        return L"";
    }

    switch (mode) {
        case MODE_HIRAGANA:
            return hiragana_chars[button_num];
        case MODE_KATAKANA:
            return katakana_chars[button_num];
        case MODE_SYMBOL:
            return symbol_chars[button_num];
        default:
            return L"";
    }
}

// Get character count for a button
int get_button_char_count(InputMode mode, int button_num) {
    if (button_num < 0 || button_num >= 12) {
        return 0;
    }

    switch (mode) {
        case MODE_HIRAGANA:
        case MODE_KATAKANA:
        case MODE_SYMBOL:
            {
                const wchar_t* chars = get_button_flick_chars(mode, button_num);
                return chars ? wcslen(chars) : 0;
            }
        case MODE_ALPHABET:
            {
                const char* chars = alphabet_chars[button_num];
                return chars ? strlen(chars) : 0;
            }
        case MODE_NUMBER:
            {
                const char* chars = number_chars[button_num];
                return chars ? strlen(chars) : 0;
            }
        default:
            return 0;
    }
}

// Get alphabet characters for a button
const char* get_button_alphabet_chars(int button_num) {
    if (button_num < 0 || button_num >= 12) {
        return "";
    }
    return alphabet_chars[button_num];
}

// Get number characters for a button
const char* get_button_number_chars(int button_num) {
    if (button_num < 0 || button_num >= 12) {
        return "";
    }
    return number_chars[button_num];
}

// Get mode name
const char* get_mode_name(InputMode mode) {
    if (mode >= 0 && mode < MODE_COUNT) {
        return mode_names[mode];
    }
    return "Unknown";
}

// Get button text with shift mode consideration
const wchar_t* get_button_text_with_shift(InputMode mode, int button_num, bool shift_mode) {
    if (button_num < 0 || button_num >= 12) {
        return L"";
    }
    
    static wchar_t single_char[2];
    single_char[0] = L'\0';
    single_char[1] = L'\0';
    
    switch (mode) {
        case MODE_HIRAGANA:
            {
                // For button labels: only show small chars if they exist, otherwise keep normal
                const wchar_t* chars;
                if (shift_mode && hiragana_chars_small[button_num] != NULL && wcslen(hiragana_chars_small[button_num]) > 0) {
                    // Show small characters only if they exist
                    chars = hiragana_chars_small[button_num];
                } else {
                    // Keep showing normal characters
                    chars = hiragana_chars[button_num];
                }
                if (chars && wcslen(chars) > 0) {
                    // For button 10 (dakuten/handakuten), show both marks
                    if (button_num == 10 && wcslen(chars) == 2) {
                        return chars;  // Return the full "゛゜" string
                    }
                    single_char[0] = chars[0];
                }
            }
            break;
        case MODE_KATAKANA:
            {
                // For button labels: only show small chars if they exist, otherwise keep normal
                const wchar_t* chars;
                if (shift_mode && katakana_chars_small[button_num] != NULL && wcslen(katakana_chars_small[button_num]) > 0) {
                    // Show small characters only if they exist
                    chars = katakana_chars_small[button_num];
                } else {
                    // Keep showing normal characters
                    chars = katakana_chars[button_num];
                }
                if (chars && wcslen(chars) > 0) {
                    // For button 10 (dakuten/handakuten), show both marks
                    if (button_num == 10 && wcslen(chars) == 2) {
                        return chars;  // Return the full "゛゜" string
                    }
                    single_char[0] = chars[0];
                }
            }
            break;
        case MODE_ALPHABET:
                    {
                        const char* chars = get_button_alphabet_chars(button_num);
                        if (chars && strlen(chars) > 0) {
                            // For English mode, show all possible characters on the button
                            // Convert the entire string to wide characters
                            static wchar_t all_chars[20];
                            int len = strlen(chars);
                            for (int i = 0; i < len && i < 19; i++) {
                                char c = chars[i];
                                if (shift_mode) {
                                    c = toupper(c);
                                }
                                all_chars[i] = (wchar_t)c;
                            }
                            all_chars[len] = L'\0';
                            return all_chars;
                        }
                    }
                    break;
        case MODE_NUMBER:
            {
                const char* chars = get_button_number_chars(button_num);
                if (chars && strlen(chars) > 0) {
                    single_char[0] = (wchar_t)chars[0];
                }
            }
            break;
        case MODE_SYMBOL:
            {
                const wchar_t* chars = get_button_flick_chars_with_shift(mode, button_num, shift_mode);
                if (chars && wcslen(chars) > 0) {
                    // For symbol mode, show all possible characters on the button
                    static wchar_t all_chars[20];
                    int len = wcslen(chars);
                    for (int i = 0; i < len && i < 19; i++) {
                        all_chars[i] = chars[i];
                    }
                    all_chars[len] = L'\0';
                    return all_chars;
                }
            }
            break;
        default:
            break;
    }
    return single_char;
}

// Get button text for current mode (shows only first character)
const wchar_t* get_button_text(InputMode mode, int button_num) {
    if (button_num < 0 || button_num >= 12) {
        return L"";
    }
    
    static wchar_t single_char[2];
    single_char[0] = L'\0';
    single_char[1] = L'\0';
    
    switch (mode) {
        case MODE_HIRAGANA:
            {
                const wchar_t* chars = hiragana_chars[button_num];
                if (chars && wcslen(chars) > 0) {
                    // For button 10 (dakuten/handakuten), show both marks
                    if (button_num == 10 && wcslen(chars) == 2) {
                        return chars;  // Return the full "゛゜" string
                    }
                    single_char[0] = chars[0];
                }
            }
            break;
        case MODE_KATAKANA:
            {
                const wchar_t* chars = katakana_chars[button_num];
                if (chars && wcslen(chars) > 0) {
                    // For button 10 (dakuten/handakuten), show both marks
                    if (button_num == 10 && wcslen(chars) == 2) {
                        return chars;  // Return the full "゛゜" string
                    }
                    single_char[0] = chars[0];
                }
            }
            break;
        case MODE_ALPHABET:
            {
                const char* chars = get_button_alphabet_chars(button_num);
                if (chars && strlen(chars) > 0) {
                    // For English mode, show all possible characters on the button
                    static wchar_t all_chars[20];
                    int len = strlen(chars);
                    for (int i = 0; i < len && i < 19; i++) {
                        all_chars[i] = (wchar_t)chars[i];
                    }
                    all_chars[len] = L'\0';
                    return all_chars;
                }
            }
            break;
        case MODE_NUMBER:
            {
                const char* chars = get_button_number_chars(button_num);
                if (chars && strlen(chars) > 0) {
                    single_char[0] = (wchar_t)chars[0];
                }
            }
            break;
        case MODE_SYMBOL:
            {
                const wchar_t* chars = get_button_flick_chars(mode, button_num);
                if (chars && wcslen(chars) > 0) {
                    // For symbol mode, show all possible characters on the button
                    static wchar_t all_chars[20];
                    int len = wcslen(chars);
                    for (int i = 0; i < len && i < 19; i++) {
                        all_chars[i] = chars[i];
                    }
                    all_chars[len] = L'\0';
                    return all_chars;
                }
            }
            break;
        default:
            break;
    }
    
    return single_char;
}
