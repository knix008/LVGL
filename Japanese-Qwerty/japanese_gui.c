/*
 * japanese_gui.c
 * Japanese Input Method Editor - GUI Implementation
 */

#include "japanese_gui.h"
#include "lvgl/src/libs/freetype/lv_freetype.h"
#include <stdio.h>
#include <string.h>

// QWERTY keyboard layout (for English mode only) - Custom layout
static const char *english_row1 = "qwertyuiop[]";  // QWERTYUIOP[] (12 keys)
static const char *english_row2 = "asdfghjkl;'";   // ASDFGHJKL;' (11 keys)
static const char *english_row3 = "zxcvbnm,./";    // ZXCVBNM,./ (10 keys)

// Japanese character hints for QWERTY keys (Hiragana)
// Custom layout based on user requirements
static const char *hiragana_hints[] = {
    // Row 0 (number row): ` 1 2 3 4 5 6 7 8 9 0 - = (13 keys)
    "ろ", "ぬ", "ふ", "あ", "う", "え", "お", "や", "ゆ", "よ", "わ", "ほ", "へ",
    // Row 1: Q W E R T Y U I O P [ ] (12 keys) - Extended with を and む
    "た", "て", "い", "す", "か", "ん", "な", "に", "ら", "せ", "を", "む",
    // Row 2: A S D F G H J K L ; ' (11 keys)
    "ち", "と", "し", "は", "き", "く", "ま", "の", "り", "れ", "け",
    // Row 3: Z X C V B N M , . / (10 keys)
    "つ", "さ", "そ", "ひ", "こ", "み", "も", "ね", "る", "め"
};

// Small Hiragana characters for shift mode (matches positions with normal mode)
static const char *hiragana_small[] = {
    // Row 0: ` 1 2 3 4 5 6 7 8 9 0 - = (13 keys with small variants)
    "ろ", "ぬ", "ふ", "ぁ", "ぅ", "ぇ", "ぉ", "ゃ", "ゅ", "ょ", "ゎ", "ほ", "へ",
    // Row 1: Q W E R T Y U I O P [ ] (12 keys with small variants)
    "た", "て", "ぃ", "す", "か", "ん", "な", "に", "ら", "せ", "を", "む",
    // Row 2: A S D F G H J K L ; ' (11 keys)
    "ち", "と", "し", "は", "き", "く", "ま", "の", "り", "れ", "け",
    // Row 3: Z X C V B N M , . / (10 keys with small tsu)
    "っ", "さ", "そ", "ひ", "こ", "み", "も", "ね", "る", "め"
};

// Japanese character hints for QWERTY keys (Katakana)
// Custom layout matching Hiragana
static const char *katakana_hints[] = {
    // Row 0: ` 1 2 3 4 5 6 7 8 9 0 - = (13 keys)
    "ロ", "ヌ", "フ", "ア", "ウ", "エ", "オ", "ヤ", "ユ", "ヨ", "ワ", "ホ", "ヘ",
    // Row 1: Q W E R T Y U I O P [ ] (12 keys)
    "タ", "テ", "イ", "ス", "カ", "ン", "ナ", "ニ", "ラ", "se", "ヲ", "ム",
    // Row 2: A S D F G H J K L ; ' (11 keys)
    "チ", "ト", "シ", "ハ", "キ", "ク", "マ", "ノ", "リ", "レ", "ケ",
    // Row 3: Z X C V B N M , . / (10 keys)
    "ツ", "サ", "ソ", "ヒ", "コ", "ミ", "モ", "ネ", "ル", "メ"
};

// Small Katakana characters for shift mode (matches positions with normal mode)
static const char *katakana_small[] = {
    // Row 0: ` 1 2 3 4 5 6 7 8 9 0 - = (13 keys with small variants)
    "ロ", "ヌ", "フ", "ァ", "ゥ", "ェ", "ォ", "ャ", "ュ", "ョ", "ヮ", "ホ", "ヘ",
    // Row 1: Q W E R T Y U I O P [ ] (12 keys with small variants)
    "タ", "テ", "ィ", "ス", "カ", "ン", "ナ", "ニ", "ラ", "se", "ヲ", "ム",
    // Row 2: A S D F G H J K L ; ' (11 keys)
    "チ", "ト", "シ", "ハ", "キ", "ク", "マ", "ノ", "リ", "レ", "ケ",
    // Row 3: Z X C V B N M , . / (10 keys with small tsu)
    "ッ", "サ", "ソ", "ヒ", "コ", "ミ", "モ", "ネ", "ル", "メ"
};

// Dakuten conversion table (゛)
typedef struct {
    const char *base;
    const char *dakuten;
} DakutenMap;

static const DakutenMap hiragana_dakuten[] = {
    {"か", "が"}, {"き", "ぎ"}, {"く", "ぐ"}, {"け", "げ"}, {"こ", "ご"},
    {"さ", "ざ"}, {"し", "じ"}, {"す", "ず"}, {"せ", "ぜ"}, {"そ", "ぞ"},
    {"た", "だ"}, {"ち", "ぢ"}, {"つ", "づ"}, {"て", "で"}, {"と", "ど"},
    {"は", "ば"}, {"ひ", "び"}, {"ふ", "ぶ"}, {"へ", "べ"}, {"ほ", "ぼ"},
    {NULL, NULL}
};

static const DakutenMap katakana_dakuten[] = {
    {"カ", "ガ"}, {"キ", "ギ"}, {"ク", "グ"}, {"ケ", "ゲ"}, {"コ", "ゴ"},
    {"サ", "ザ"}, {"シ", "ジ"}, {"ス", "ズ"}, {"セ", "ゼ"}, {"ソ", "ゾ"},
    {"タ", "ダ"}, {"チ", "ヂ"}, {"ツ", "ヅ"}, {"テ", "デ"}, {"ト", "ド"},
    {"ハ", "バ"}, {"ヒ", "ビ"}, {"フ", "ブ"}, {"ヘ", "ベ"}, {"ホ", "ボ"},
    {NULL, NULL}
};

// Handakuten conversion table (゜)
static const DakutenMap hiragana_handakuten[] = {
    {"は", "ぱ"}, {"ひ", "ぴ"}, {"ふ", "ぷ"}, {"へ", "ぺ"}, {"ほ", "ぽ"},
    {NULL, NULL}
};

static const DakutenMap katakana_handakuten[] = {
    {"ハ", "パ"}, {"ヒ", "ピ"}, {"フ", "プ"}, {"ヘ", "ペ"}, {"ホ", "ポ"},
    {NULL, NULL}
};

static GUIState *global_gui_state = NULL;

void gui_init(GUIState *state, IMEState *ime_state) {
    memset(state, 0, sizeof(GUIState));
    state->ime_state = ime_state;
    state->japanese_font = NULL;  // Will be loaded later
    state->shift_pressed = false;
    state->number_mode = false;
    state->previous_letter_mode = MODE_HIRAGANA;  // Default to Hiragana
    global_gui_state = state;
}

bool gui_load_fonts(GUIState *state) {
    // Initialize FreeType library (max 256 glyphs in cache)
    lv_freetype_init(256);
    
    // Load Japanese font from assets directory using FreeType (14px)
    lv_font_t *japanese_font = lv_freetype_font_create(
        "assets/NotoSansCJK.ttc",
        LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
        14,
        LV_FREETYPE_FONT_STYLE_NORMAL
    );
    
    if (!japanese_font) {
        printf("Error: Failed to load font from assets/NotoSansCJK.ttc\n");
        printf("Falling back to default font\n");
        state->japanese_font = (lv_font_t*)&lv_font_montserrat_14;
        return false;
    }
    
    state->japanese_font = japanese_font;
    printf("Successfully loaded Japanese font (14px) from assets/NotoSansCJK.ttc\n");
    return true;
}

void gui_update_display(GUIState *state) {
    if (!state || !state->text_area || !state->mode_label) {
        return;
    }
    
    // Update text area
    const char *display_text = ime_get_display_text(state->ime_state);
    lv_textarea_set_text(state->text_area, display_text);
    
    // Update mode label
    const char *mode_str = ime_get_mode_string(state->ime_state);
    char mode_text[64];
    snprintf(mode_text, sizeof(mode_text), "Mode: %s", mode_str);
    lv_label_set_text(state->mode_label, mode_text);
}

void gui_button_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED && global_gui_state) {
        lv_obj_t *btn = (lv_obj_t *)lv_event_get_target(e);
        const char *label = lv_label_get_text(lv_obj_get_child(btn, 0));
        
        if (label && strlen(label) > 0) {
            // Check if we're in Japanese mode (Hiragana or Katakana)
            if (global_gui_state->ime_state->mode == MODE_HIRAGANA || 
                global_gui_state->ime_state->mode == MODE_KATAKANA) {
                // Directly append Japanese character to display
                if (global_gui_state->ime_state->display_pos + strlen(label) < (int)sizeof(global_gui_state->ime_state->display) - 1) {
                    strcat(global_gui_state->ime_state->display, label);
                    global_gui_state->ime_state->display_pos += strlen(label);
                }
            } else if (global_gui_state->ime_state->mode == MODE_ENGLISH) {
                // For English mode, directly append the character shown on button (supports uppercase)
                if (global_gui_state->ime_state->display_pos + strlen(label) < (int)sizeof(global_gui_state->ime_state->display) - 1) {
                    strcat(global_gui_state->ime_state->display, label);
                    global_gui_state->ime_state->display_pos += strlen(label);
                }
            } else {
                // For Number mode, process character normally
                for (size_t i = 0; i < strlen(label); i++) {
                    char c = label[i];
                    if (global_gui_state->ime_state->display_pos < (int)sizeof(global_gui_state->ime_state->display) - 1) {
                        global_gui_state->ime_state->display[global_gui_state->ime_state->display_pos++] = c;
                        global_gui_state->ime_state->display[global_gui_state->ime_state->display_pos] = '\0';
                    }
                }
            }
            gui_update_display(global_gui_state);
        }
    }
}

void gui_shift_button_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED && global_gui_state) {
        // Toggle shift state (shows small characters in Japanese, uppercase in English)
        global_gui_state->shift_pressed = !global_gui_state->shift_pressed;
        
        // Shift NO LONGER changes mode - it only shows small/uppercase characters
        // Update keyboard labels to reflect shift state
        gui_update_keyboard_labels(global_gui_state);
        gui_update_display(global_gui_state);
        
        // Update shift button appearance
        if (global_gui_state->shift_pressed) {
            lv_obj_set_style_bg_color(global_gui_state->shift_button, lv_color_hex(0xFF8000), 0);  // Orange when active
        } else {
            lv_obj_set_style_bg_color(global_gui_state->shift_button, lv_color_hex(0x00AA00), 0);  // Green when inactive
        }
    }
}

void gui_mode_button_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED && global_gui_state) {
        lv_obj_t *mode_label = lv_obj_get_child(lv_event_get_target(e), 0);
        
        if (global_gui_state->number_mode) {
            // Currently in number mode, switch back to previous letter mode
            global_gui_state->number_mode = false;
            ime_set_mode(global_gui_state->ime_state, global_gui_state->previous_letter_mode);
            
            // Update button to show "123"
            if (mode_label) {
                lv_label_set_text(mode_label, "123");
            }
        } else {
            // Currently in letter mode, switch to number mode
            // Save current mode as previous
            global_gui_state->previous_letter_mode = global_gui_state->ime_state->mode;
            global_gui_state->number_mode = true;
            
            // Update button to show previous mode name
            if (mode_label) {
                const char *mode_name;
                if (global_gui_state->previous_letter_mode == MODE_HIRAGANA) {
                    mode_name = "ひらがな";  // Hiragana mode indicator
                } else if (global_gui_state->previous_letter_mode == MODE_KATAKANA) {
                    mode_name = "カタカナ";  // Katakana mode indicator
                } else {
                    mode_name = "ABC";  // English mode indicator
                }
                lv_label_set_text(mode_label, mode_name);
            }
        }
        
        // Don't reset shift when entering/exiting number mode
        // Update keyboard labels to reflect number mode AND current shift state
        gui_update_keyboard_labels(global_gui_state);
        gui_update_display(global_gui_state);
    }
}

// Apply dakuten mark to last character
void apply_dakuten_mark(IMEState *state, bool is_handakuten) {
    if (state->display_pos == 0) return;
    
    // Find start of last UTF-8 character
    int pos = state->display_pos - 1;
    while (pos > 0 && (state->display[pos] & 0xC0) == 0x80) {
        pos--;
    }
    
    // Extract last character
    char last_char[16] = {0};
    int char_len = state->display_pos - pos;
    strncpy(last_char, state->display + pos, char_len);
    last_char[char_len] = '\0';
    
    // Find replacement in conversion table
    const DakutenMap *map = NULL;
    const char *replacement = NULL;
    
    if (state->mode == MODE_HIRAGANA) {
        map = is_handakuten ? hiragana_handakuten : hiragana_dakuten;
    } else if (state->mode == MODE_KATAKANA) {
        map = is_handakuten ? katakana_handakuten : katakana_dakuten;
    }
    
    if (map) {
        for (int i = 0; map[i].base != NULL; i++) {
            if (strcmp(last_char, map[i].base) == 0) {
                replacement = map[i].dakuten;
                break;
            }
        }
    }
    
    // Replace last character with dakuten version
    if (replacement) {
        state->display[pos] = '\0';
        state->display_pos = pos;
        strcat(state->display, replacement);
        state->display_pos += strlen(replacement);
    }
}

void gui_special_button_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED && global_gui_state) {
        lv_obj_t *btn = (lv_obj_t *)lv_event_get_target(e);
        const char *label = lv_label_get_text(lv_obj_get_child(btn, 0));
        
        if (strcmp(label, "Space") == 0) {
            ime_process_space(global_gui_state->ime_state);
        } else if (strcmp(label, "Enter") == 0) {
            // Get current text
            const char *current_text = ime_get_display_text(global_gui_state->ime_state);
            
            // Only show popup if there's text to display
            if (current_text && strlen(current_text) > 0) {
                // Create result message box (like gui_app.c)
                lv_obj_t *mbox = lv_msgbox_create(lv_screen_active());
                lv_msgbox_add_title(mbox, "入力完了 - Input Complete");
                lv_msgbox_add_text(mbox, current_text);
                lv_msgbox_add_close_button(mbox);
                lv_obj_set_style_text_font(mbox, global_gui_state->japanese_font, 0);
                lv_obj_center(mbox);
                // Clear the input after showing popup
                ime_clear(global_gui_state->ime_state);
                gui_update_display(global_gui_state);
            }
        } else if (strcmp(label, "←") == 0) {
            ime_process_backspace(global_gui_state->ime_state);
        } else if (strcmp(label, "Clear") == 0) {
            ime_clear(global_gui_state->ime_state);
        } else if (strcmp(label, "゛") == 0 || strcmp(label, "濁点") == 0) {
            // Dakuten mark
            apply_dakuten_mark(global_gui_state->ime_state, false);
        } else if (strcmp(label, "゜") == 0 || strcmp(label, "半濁") == 0) {
            // Handakuten mark
            apply_dakuten_mark(global_gui_state->ime_state, true);
        } else if (strcmp(label, "-") == 0 || strcmp(label, "ー") == 0) {
            // Prolonged sound mark
            int remaining = (int)sizeof(global_gui_state->ime_state->display) - global_gui_state->ime_state->display_pos - 1;
            if (remaining > (int)strlen("ー")) {
                strcat(global_gui_state->ime_state->display, "ー");
                global_gui_state->ime_state->display_pos += strlen("ー");
            }
        }
        
        gui_update_display(global_gui_state);
    }
}

// Toggle between Japanese (Hiragana/Katakana) and English modes
void gui_cycle_mode_button_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED && global_gui_state) {
        // Cycle through THREE modes: Hiragana → Katakana → English → Hiragana
        InputMode current_mode = global_gui_state->ime_state->mode;
        InputMode next_mode;
        
        if (current_mode == MODE_HIRAGANA) {
            next_mode = MODE_KATAKANA;
        } else if (current_mode == MODE_KATAKANA) {
            next_mode = MODE_ENGLISH;
        } else {
            next_mode = MODE_HIRAGANA;
        }
        
        ime_set_mode(global_gui_state->ime_state, next_mode);
        global_gui_state->previous_letter_mode = next_mode;  // Update previous mode
        // Don't reset shift - keep current shift state when changing modes
        
        // Update mode toggle button to show NEXT mode indicator
        if (global_gui_state->mode_toggle_button) {
            lv_obj_t *toggle_label = lv_obj_get_child(global_gui_state->mode_toggle_button, 0);
            if (toggle_label) {
                const char *next_mode_label;
                if (next_mode == MODE_HIRAGANA) {
                    next_mode_label = "カタカナ";  // Show next (Katakana)
                } else if (next_mode == MODE_KATAKANA) {
                    next_mode_label = "ABC";  // Show next (English)
                } else {
                    next_mode_label = "ひらがな";  // Show next (Hiragana)
                }
                lv_label_set_text(toggle_label, next_mode_label);
            }
        }
        
        // Update keyboard labels to reflect new mode AND current shift state
        gui_update_keyboard_labels(global_gui_state);
        gui_update_display(global_gui_state);
    }
}

// Update keyboard labels based on current mode and shift state
void gui_update_keyboard_labels(GUIState *state) {
    if (!state) return;
    
    int idx = 0;
    int hint_idx = 0;
    
    if (state->number_mode) {
        // Number mode: Show numbers and special characters only (no English letters)
        const char *row0_normal = "`1234567890-=";
        const char *row0_shift[] = {"~", "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "_", "+"};

        // Row 0: Numbers or special characters (with shift) - 13 keys
        for (int i = 0; i < 13 && idx < state->num_buttons; i++, idx++) {
            lv_obj_t *label = lv_obj_get_child(state->key_buttons[idx], 0);
            if (label) {
                if (state->shift_pressed) {
                    lv_label_set_text(label, row0_shift[i]);
                } else {
                    char str[2] = {row0_normal[i], '\0'};
                    lv_label_set_text(label, str);
                }
            }
        }

        // Skip backspace button (now at end of Row 0)
        idx++;

        // Row 1: Special characters and symbols - 12 keys (no duplicates)
        const char *row1_normal = "[]{}\\|<>()+-";
        const char *row1_shift[] = {"«", "»", "←", "→", "↑", "↓", "☆", "★", "▲", "▼", "♪", "♫"};
        for (int i = 0; i < 12 && idx < state->num_buttons; i++, idx++) {
            lv_obj_t *label = lv_obj_get_child(state->key_buttons[idx], 0);
            if (label) {
                if (state->shift_pressed) {
                    lv_label_set_text(label, row1_shift[i]);
                } else {
                    char str[2] = {row1_normal[i], '\0'};
                    lv_label_set_text(label, str);
                }
            }
        }

        // Row 2: More symbols - 11 keys
        const char *row2_normal[] = {"@", ";", ":", ".", "/", "^", "`", "€", "¥", "'", "\""};
        const char *row2_shift[] = {"©", "®", "™", "¿", "¡", "√", "º", "£", "¤", "\"", "'"};
        for (int i = 0; i < 11 && idx < state->num_buttons; i++, idx++) {
            lv_obj_t *label = lv_obj_get_child(state->key_buttons[idx], 0);
            if (label) {
                const char *symbol = state->shift_pressed ? row2_shift[i] : row2_normal[i];
                lv_label_set_text(label, symbol);
            }
        }

        // Row 3: Additional symbols - 10 keys (replaced Mac symbols with common ones)
        const char *row3_normal[] = {"&", "#", "$", "%", "¦", "µ", "°", "±", "×", "÷"};
        const char *row3_shift[] = {"?", "¢", "§", "¶", "†", "‡", "✓", "∑", "π", "Ω"};
        for (int i = 0; i < 10 && idx < state->num_buttons; i++, idx++) {
            lv_obj_t *label = lv_obj_get_child(state->key_buttons[idx], 0);
            if (label) {
                const char *symbol = state->shift_pressed ? row3_shift[i] : row3_normal[i];
                lv_label_set_text(label, symbol);
            }
        }
    } else {
        // Letter mode: Show Japanese characters or English letters
        const char **japanese_chars = NULL;
        
        // Select Japanese character set based on mode and shift state
        if (state->ime_state->mode == MODE_HIRAGANA) {
            if (state->shift_pressed) {
                japanese_chars = hiragana_small;
            } else {
                japanese_chars = hiragana_hints;
            }
        } else if (state->ime_state->mode == MODE_KATAKANA) {
            if (state->shift_pressed) {
                japanese_chars = katakana_small;
            } else {
                japanese_chars = katakana_hints;
            }
        }
        
        // Row 0: Number row (` 1-9 0 - =) - 13 keys
        for (int i = 0; i < 13 && idx < state->num_buttons; i++, idx++, hint_idx++) {
            lv_obj_t *label = lv_obj_get_child(state->key_buttons[idx], 0);
            if (label) {
                if (japanese_chars) {
                    // Show Japanese character
                    lv_label_set_text(label, japanese_chars[hint_idx]);
                } else {
                    // Show number/symbol for English mode
                    const char *num_keys = "`1234567890-=";
                    char str[2] = {num_keys[i], '\0'};
                    lv_label_set_text(label, str);
                }
            }
        }

        // Skip backspace button (now at end of Row 0)
        idx++;

        // Row 1: Q-P [] (12 keys)
        for (int i = 0; i < 12 && idx < state->num_buttons; i++, idx++, hint_idx++) {
            lv_obj_t *label = lv_obj_get_child(state->key_buttons[idx], 0);
            if (label) {
                if (japanese_chars) {
                    // Show Japanese character
                    lv_label_set_text(label, japanese_chars[hint_idx]);
                } else {
                    // Show English letter
                    char c = english_row1[i];
                    if (state->shift_pressed && c >= 'a' && c <= 'z') {
                        c = c - 32;  // Convert to uppercase
                    }
                    char str[2] = {c, '\0'};
                    lv_label_set_text(label, str);
                }
            }
        }

        // Row 2: A S D F G H J K L ; ' (11 keys)
        for (int i = 0; i < 11 && idx < state->num_buttons; i++, idx++, hint_idx++) {
            lv_obj_t *label = lv_obj_get_child(state->key_buttons[idx], 0);
            if (label) {
                if (japanese_chars) {
                    // Show Japanese character
                    lv_label_set_text(label, japanese_chars[hint_idx]);
                } else {
                    // Show English letter
                    char c = english_row2[i];
                    if (state->shift_pressed && c >= 'a' && c <= 'z') {
                        c = c - 32;  // Convert to uppercase
                    }
                    char str[2] = {c, '\0'};
                    lv_label_set_text(label, str);
                }
            }
        }

        // Row 3: Z X C V B N M , . / (10 keys)
        for (int i = 0; i < 10 && idx < state->num_buttons; i++, idx++, hint_idx++) {
            lv_obj_t *label = lv_obj_get_child(state->key_buttons[idx], 0);
            if (label) {
                if (japanese_chars) {
                    // Show Japanese character
                    lv_label_set_text(label, japanese_chars[hint_idx]);
                } else {
                    // Show English letter or punctuation
                    char c = english_row3[i];
                    if (state->shift_pressed && c >= 'a' && c <= 'z') {
                        c = c - 32;  // Convert to uppercase
                    }
                    char str[2] = {c, '\0'};
                    lv_label_set_text(label, str);
                }
            }
        }
    }
}

void gui_create_qwerty_keyboard(GUIState *state, lv_obj_t *parent) {
    int btn_width = 50;  // Character button width
    int btn_wide = btn_width * 2;   // Shift/Enter button width is twice character button
    int btn_height = 40; // Increased button height for larger keys
    int btn_gap = 4;
    int start_y = 5;
    int button_index = 0;
    
    // Row 0: Number row (` 1 2 3 4 5 6 7 8 9 0 - =) - 13 buttons + Backspace
    int row0_total_width = 13 * btn_width + 50 + 13 * btn_gap;
    int row0_start_x = (780 - row0_total_width) / 2 - 5;

    const char *number_keys = "`1234567890-=";
    for (int i = 0; i < 13; i++) {
        lv_obj_t *btn = lv_button_create(parent);
        lv_obj_set_size(btn, btn_width, btn_height);
        lv_obj_set_pos(btn, row0_start_x + i * (btn_width + btn_gap) - 5, start_y);

        lv_obj_t *label = lv_label_create(btn);
        char str[2] = {number_keys[i], '\0'};
        lv_label_set_text(label, str);
        lv_obj_set_style_text_font(label, state->japanese_font, 0);
        lv_obj_center(label);

        lv_obj_add_event_cb(btn, gui_button_event_cb, LV_EVENT_CLICKED, NULL);
        state->key_buttons[button_index++] = btn;
    }

    // Backspace button at end of row 0
    lv_obj_t *bksp_btn = lv_button_create(parent);
    lv_obj_set_size(bksp_btn, 50, btn_height);
    lv_obj_set_pos(bksp_btn, row0_start_x + 13 * (btn_width + btn_gap) - 5, start_y);
    lv_obj_t *bksp_label = lv_label_create(bksp_btn);
    lv_label_set_text(bksp_label, "\u2190");
    lv_obj_set_style_text_font(bksp_label, state->japanese_font, 0);
    lv_obj_center(bksp_label);
    lv_obj_add_event_cb(bksp_btn, gui_special_button_event_cb, LV_EVENT_CLICKED, NULL);
    button_index++;  // Don't store in key_buttons array

    // Row 1: Q W E R T Y U I O P [ ] (12 keys)
    start_y += btn_height + btn_gap;
    int row1_total_width = 12 * btn_width + 11 * btn_gap;
    int row1_start_x = (780 - row1_total_width) / 2 - 5;

    for (int i = 0; i < 12; i++) {
        lv_obj_t *btn = lv_button_create(parent);
        lv_obj_set_size(btn, btn_width, btn_height);
        lv_obj_set_pos(btn, row1_start_x + i * (btn_width + btn_gap) - 5, start_y);

        lv_obj_t *label = lv_label_create(btn);
        char str[2] = {english_row1[i], '\0'};
        lv_label_set_text(label, str);
        lv_obj_set_style_text_font(label, state->japanese_font, 0);
        lv_obj_center(label);

        lv_obj_add_event_cb(btn, gui_button_event_cb, LV_EVENT_CLICKED, NULL);
        state->key_buttons[button_index++] = btn;
    }
    
    // Row 2: 11 buttons (42px) + 10 gaps (4px) = 502px total
    int row2_total_width = 11 * btn_width + 10 * btn_gap;
    int row2_start_x = (780 - row2_total_width) / 2 - 5;

    // Row 2: A S D F G H J K L ; ' (11 keys)
    start_y += btn_height + btn_gap;
    for (int i = 0; i < 11; i++) {
        lv_obj_t *btn = lv_button_create(parent);
        lv_obj_set_size(btn, btn_width, btn_height);
    lv_obj_set_pos(btn, row2_start_x + i * (btn_width + btn_gap) - 5, start_y);

        lv_obj_t *label = lv_label_create(btn);
        char str[2] = {english_row2[i], '\0'};
        lv_label_set_text(label, str);
        lv_obj_set_style_text_font(label, state->japanese_font, 0);
        lv_obj_center(label);

        lv_obj_add_event_cb(btn, gui_button_event_cb, LV_EVENT_CLICKED, NULL);
        state->key_buttons[button_index++] = btn;
    }
    
    // Row 3: Shift + Z X C V B N M , . / + Enter (12 buttons)
    int row3_total_width = btn_wide + 10 * btn_width + btn_wide + 11 * btn_gap;
    int row3_start_x = (780 - row3_total_width) / 2 - 5;

    start_y += btn_height + btn_gap;
    // Shift button at start of Row 3
    int x = row3_start_x;
    state->shift_button = lv_button_create(parent);
    lv_obj_set_size(state->shift_button, btn_wide, btn_height); // Shift button is twice as wide
    lv_obj_set_pos(state->shift_button, x - 5, start_y);
    x += btn_wide + btn_gap;
    lv_obj_t *shift_label = lv_label_create(state->shift_button);
    lv_label_set_text(shift_label, "Shift");
    lv_obj_set_style_text_font(shift_label, state->japanese_font, 0);
    lv_obj_center(shift_label);
    lv_obj_set_style_bg_color(state->shift_button, lv_color_hex(0x00AA00), 0);
    lv_obj_add_event_cb(state->shift_button, gui_shift_button_event_cb, LV_EVENT_CLICKED, NULL);

    // Z X C V B N M , . /
    for (int i = 0; i < 10; i++) {
        lv_obj_t *btn = lv_button_create(parent);
        lv_obj_set_size(btn, btn_width, btn_height);
    lv_obj_set_pos(btn, x - 5, start_y);
        lv_obj_t *label = lv_label_create(btn);
        char str[2] = {english_row3[i], '\0'};
        lv_label_set_text(label, str);
        lv_obj_set_style_text_font(label, state->japanese_font, 0);
        lv_obj_center(label);
        lv_obj_add_event_cb(btn, gui_button_event_cb, LV_EVENT_CLICKED, NULL);
        state->key_buttons[button_index++] = btn;
        x += btn_width + btn_gap;
    }

    // Enter button at end of Row 3
    lv_obj_t *enter_btn = lv_button_create(parent);
    lv_obj_set_size(enter_btn, btn_wide, btn_height); // Enter button is twice as wide
    lv_obj_set_pos(enter_btn, x - 5, start_y);
    lv_obj_t *enter_label = lv_label_create(enter_btn);
    lv_label_set_text(enter_label, "Enter");
    lv_obj_set_style_text_font(enter_label, state->japanese_font, 0);
    lv_obj_center(enter_label);
    lv_obj_set_style_bg_color(enter_btn, lv_color_hex(0x222222), 0); // Darker color
    lv_obj_add_event_cb(enter_btn, gui_special_button_event_cb, LV_EVENT_CLICKED, NULL);
    
    // Row 4: Shift (50) + Space (160) + 123 (80) + Clear (50) + ゛(32) + ゜(32) + ー(32) + Enter (50) + 7 gaps (4px)
    int row4_total_width = 50 + 160 + 80 + 50 + 32 + 32 + 32 + 50 + 7 * btn_gap;
    int row4_start_x = (780 - row4_total_width) / 2 - 5;

    // Row 4: Shift, Space, 123/ABC, Clear, dakuten buttons, Enter
    start_y += btn_height + btn_gap;
    int current_x = row4_start_x;

    // Space button
    lv_obj_t *space_btn = lv_button_create(parent);
    lv_obj_set_size(space_btn, 160, btn_height);
    lv_obj_set_pos(space_btn, current_x - 5, start_y);
    lv_obj_t *space_label = lv_label_create(space_btn);
    lv_label_set_text(space_label, "Space");
    lv_obj_set_style_text_font(space_label, state->japanese_font, 0);
    lv_obj_center(space_label);
    lv_obj_add_event_cb(space_btn, gui_special_button_event_cb, LV_EVENT_CLICKED, NULL);
    current_x += 160 + btn_gap;

    // 123/ABC mode toggle button
    lv_obj_t *num_mode_btn = lv_button_create(parent);
    lv_obj_set_size(num_mode_btn, 80, btn_height);
    lv_obj_set_pos(num_mode_btn, current_x - 5, start_y);
    lv_obj_t *mode_label = lv_label_create(num_mode_btn);
    lv_label_set_text(mode_label, "123");
    lv_obj_set_style_text_font(mode_label, state->japanese_font, 0);
    lv_obj_center(mode_label);
    lv_obj_add_event_cb(num_mode_btn, gui_mode_button_event_cb, LV_EVENT_CLICKED, NULL);
    current_x += 80 + btn_gap;

    // Dakuten button (゛)
    int jp_btn_gap = btn_gap * 2;
    lv_obj_t *dakuten_btn = lv_button_create(parent);
    lv_obj_set_size(dakuten_btn, btn_width, btn_height);
    lv_obj_set_pos(dakuten_btn, current_x - 5, start_y);
    lv_obj_t *dakuten_label = lv_label_create(dakuten_btn);
    lv_label_set_text(dakuten_label, "゛");
    lv_obj_set_style_text_font(dakuten_label, state->japanese_font, 0);
    lv_obj_center(dakuten_label);
    lv_obj_add_event_cb(dakuten_btn, gui_special_button_event_cb, LV_EVENT_CLICKED, NULL);
    current_x += btn_width + jp_btn_gap;

    // Handakuten button (゜)
    lv_obj_t *handakuten_btn = lv_button_create(parent);
    lv_obj_set_size(handakuten_btn, btn_width, btn_height);
    lv_obj_set_pos(handakuten_btn, current_x - 5, start_y);
    lv_obj_t *handakuten_label = lv_label_create(handakuten_btn);
    lv_label_set_text(handakuten_label, "゜");
    lv_obj_set_style_text_font(handakuten_label, state->japanese_font, 0);
    lv_obj_center(handakuten_label);
    lv_obj_add_event_cb(handakuten_btn, gui_special_button_event_cb, LV_EVENT_CLICKED, NULL);
    current_x += btn_width + jp_btn_gap;

    // Prolonged sound mark button (ー)
    lv_obj_t *chouon_btn = lv_button_create(parent);
    lv_obj_set_size(chouon_btn, btn_width, btn_height);
    lv_obj_set_pos(chouon_btn, current_x - 5, start_y);
    lv_obj_t *chouon_label = lv_label_create(chouon_btn);
    lv_label_set_text(chouon_label, "ー");
    lv_obj_set_style_text_font(chouon_label, state->japanese_font, 0);
    lv_obj_center(chouon_label);
    lv_obj_add_event_cb(chouon_btn, gui_special_button_event_cb, LV_EVENT_CLICKED, NULL);
    current_x += btn_width + jp_btn_gap;

    // Clear button at end
    lv_obj_t *clear_btn = lv_button_create(parent);
    lv_obj_set_size(clear_btn, btn_width * 2, btn_height);
    lv_obj_set_pos(clear_btn, current_x - 5, start_y);
    lv_obj_t *clear_label = lv_label_create(clear_btn);
    lv_label_set_text(clear_label, "Clear");
    lv_obj_set_style_text_font(clear_label, state->japanese_font, 0);
    lv_obj_center(clear_label);
    lv_obj_add_event_cb(clear_btn, gui_special_button_event_cb, LV_EVENT_CLICKED, NULL);

    // ...existing code...
    
    state->num_buttons = button_index;
}

void gui_create_ui(GUIState *state) {
    // Create main screen
    state->screen = lv_screen_active();
    lv_obj_set_style_bg_color(state->screen, lv_color_hex(0xF5F5F5), 0);
    
    // Create mode label at top
    state->mode_label = lv_label_create(state->screen);
    lv_label_set_text(state->mode_label, "Mode:ひらがな");
    lv_obj_set_pos(state->mode_label, 10, 5);
    lv_obj_set_style_text_font(state->mode_label, state->japanese_font, 0);
    
    // Create mode switch button (Hiragana → Katakana → English)
    lv_obj_t *mode_btn = lv_button_create(state->screen);
    lv_obj_set_size(mode_btn, 100, 30);
    // Align mode button to the right side (window width 780, button width 100, margin 10)
    lv_obj_set_pos(mode_btn, 780 - 100 - 10, 5);
    state->mode_toggle_button = mode_btn;  // Save for later updates
    lv_obj_t *mode_btn_label = lv_label_create(mode_btn);
    lv_label_set_text(mode_btn_label, "カタカナ");  // Start in Hiragana, show Katakana as next
    lv_obj_set_style_text_font(mode_btn_label, state->japanese_font, 0);
    lv_obj_center(mode_btn_label);
    lv_obj_add_event_cb(mode_btn, gui_cycle_mode_button_event_cb, LV_EVENT_CLICKED, NULL);
    
    // Create text area for displaying Japanese text
    state->text_area = lv_textarea_create(state->screen);
    lv_obj_set_size(state->text_area, 780, 160);
    lv_obj_set_pos(state->text_area, 10, 40);
    lv_textarea_set_text(state->text_area, "");
    lv_obj_set_style_text_font(state->text_area, state->japanese_font, 0);
    lv_textarea_set_placeholder_text(state->text_area, "Type here...");
    
    // Create keyboard container (taller for 5 rows)
    state->keyboard_container = lv_obj_create(state->screen);
    lv_obj_set_size(state->keyboard_container, 780, 250);
    lv_obj_set_pos(state->keyboard_container, 10, 220);
    lv_obj_set_style_bg_color(state->keyboard_container, lv_color_hex(0xE0E0E0), 0);
    lv_obj_set_style_border_width(state->keyboard_container, 2, 0);
    lv_obj_set_style_border_color(state->keyboard_container, lv_color_hex(0x808080), 0);
    lv_obj_set_style_pad_all(state->keyboard_container, 10, 0);
    
    // Create QWERTY keyboard
    gui_create_qwerty_keyboard(state, state->keyboard_container);
    
    // Update keyboard labels based on initial mode
    gui_update_keyboard_labels(state);
    
    // Initial display update
    gui_update_display(state);
}

