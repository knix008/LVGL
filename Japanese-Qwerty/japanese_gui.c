/*
 * japanese_gui.c
 * Japanese Input Method Editor - GUI Implementation
 */

#include "japanese_gui.h"
#include "lvgl/src/libs/freetype/lv_freetype.h"
#include <stdio.h>
#include <string.h>

// QWERTY keyboard layout
static const char *qwerty_rows[] = {
    "qwertyuiop",
    "asdfghjkl",
    "zxcvbnm"
};

// Japanese character hints for QWERTY keys (Hiragana)
static const char *hiragana_hints[] = {
    // Row 1: q w e r t y u i o p
    "q", "わ", "え", "ら", "た", "や", "う", "い", "お", "ぱ",
    // Row 2: a s d f g h j k l
    "あ", "さ", "だ", "ふ", "が", "は", "じ", "か", "ら",
    // Row 3: z x c v b n m
    "ざ", "x", "ち", "ゔ", "ば", "ん", "ま"
};

// Japanese character hints for QWERTY keys (Katakana)
static const char *katakana_hints[] = {
    // Row 1: q w e r t y u i o p
    "q", "ワ", "エ", "ラ", "タ", "ヤ", "ウ", "イ", "オ", "パ",
    // Row 2: a s d f g h j k l
    "ア", "サ", "ダ", "フ", "ガ", "ハ", "ジ", "カ", "ラ",
    // Row 3: z x c v b n m
    "ザ", "x", "チ", "ヴ", "バ", "ン", "マ"
};

static GUIState *global_gui_state = NULL;

void gui_init(GUIState *state, IMEState *ime_state) {
    memset(state, 0, sizeof(GUIState));
    state->ime_state = ime_state;
    state->japanese_font = NULL;  // Will be loaded later
    state->shift_pressed = false;
    state->number_mode = false;
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
    if (!state || !state->text_area || !state->mode_label || !state->buffer_label) {
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
    
    // Update buffer label (showing current input)
    char buffer_text[1024];  // Increased size to handle long inputs
    if (state->ime_state->buffer_pos > 0 || state->ime_state->output_pos > 0) {
        int written = snprintf(buffer_text, sizeof(buffer_text), "Input: %s → %s", 
                 state->ime_state->buffer, 
                 state->ime_state->output);
        if (written >= (int)sizeof(buffer_text)) {
            buffer_text[sizeof(buffer_text) - 1] = '\0';  // Ensure null termination
        }
    } else {
        snprintf(buffer_text, sizeof(buffer_text), "Input: ");
    }
    lv_label_set_text(state->buffer_label, buffer_text);
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
            } else {
                // For English and Number modes, process character normally
                for (size_t i = 0; i < strlen(label); i++) {
                    ime_process_char(global_gui_state->ime_state, label[i]);
                }
            }
            gui_update_display(global_gui_state);
        }
    }
}

void gui_shift_button_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED && global_gui_state) {
        // Toggle shift state
        global_gui_state->shift_pressed = !global_gui_state->shift_pressed;
        
        // Cycle through modes based on shift state
        InputMode current_mode = global_gui_state->ime_state->mode;
        InputMode next_mode = current_mode;
        
        // For Japanese modes: toggle between hiragana and katakana
        if (current_mode == MODE_HIRAGANA) {
            next_mode = MODE_KATAKANA;
        } else if (current_mode == MODE_KATAKANA) {
            next_mode = MODE_HIRAGANA;
        }
        // For English mode: shift handles uppercase/lowercase in the key labels
        
        if (next_mode != current_mode) {
            ime_set_mode(global_gui_state->ime_state, next_mode);
        }
        
        // Update keyboard labels to reflect shift state
        gui_update_keyboard_labels(global_gui_state);
        gui_update_display(global_gui_state);
        
        // Update shift button appearance
        if (global_gui_state->shift_pressed) {
            lv_obj_set_style_bg_color(global_gui_state->shift_button, lv_color_hex(0x4A90E2), 0);
        } else {
            lv_obj_set_style_bg_color(global_gui_state->shift_button, lv_color_hex(0x808080), 0);
        }
    }
}

void gui_mode_button_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED && global_gui_state) {
        // Toggle between letter mode and number mode
        global_gui_state->number_mode = !global_gui_state->number_mode;
        global_gui_state->shift_pressed = false;  // Reset shift when changing modes
        
        // Update keyboard labels
        gui_update_keyboard_labels(global_gui_state);
        gui_update_display(global_gui_state);
        
        // Update mode button text
        lv_obj_t *mode_label = lv_obj_get_child(lv_event_get_target(e), 0);
        if (mode_label) {
            lv_label_set_text(mode_label, global_gui_state->number_mode ? "ABC" : "123");
        }
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
            ime_process_enter(global_gui_state->ime_state);
        } else if (strcmp(label, "←") == 0) {
            ime_process_backspace(global_gui_state->ime_state);
        } else if (strcmp(label, "Clear") == 0) {
            ime_clear(global_gui_state->ime_state);
        } else if (strcmp(label, "-") == 0) {
            ime_process_char(global_gui_state->ime_state, '-');
        }
        
        gui_update_display(global_gui_state);
    }
}

// Cycle through input modes: Hiragana -> Katakana -> English
void gui_cycle_mode_button_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED && global_gui_state) {
        // Cycle through modes
        InputMode current_mode = global_gui_state->ime_state->mode;
        InputMode next_mode;
        
        switch (current_mode) {
            case MODE_HIRAGANA:
                next_mode = MODE_KATAKANA;
                break;
            case MODE_KATAKANA:
                next_mode = MODE_ENGLISH;
                break;
            case MODE_ENGLISH:
                next_mode = MODE_HIRAGANA;
                break;
            default:
                next_mode = MODE_HIRAGANA;
                break;
        }
        
        ime_set_mode(global_gui_state->ime_state, next_mode);
        global_gui_state->shift_pressed = false;  // Reset shift when changing base mode
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
        // Number mode: Show numbers 1-0 and special characters
        const char *number_keys = "1234567890";
        const char *special_keys = "!@#$%^&*()";
        
        // Row 1: Numbers or special characters (with shift)
        for (int i = 0; i < 10 && idx < state->num_buttons; i++, idx++) {
            lv_obj_t *label = lv_obj_get_child(state->key_buttons[idx], 0);
            if (label) {
                char str[2] = {state->shift_pressed ? special_keys[i] : number_keys[i], '\0'};
                lv_label_set_text(label, str);
            }
        }
        
        // Skip backspace button (idx++)
        idx++;
        
        // Row 2: Additional special characters
        const char *row2_normal = "-=[]\\;',./";
        const char *row2_shift = "_+{}|:\"<>?";
        for (int i = 0; i < 9 && idx < state->num_buttons; i++, idx++) {
            lv_obj_t *label = lv_obj_get_child(state->key_buttons[idx], 0);
            if (label) {
                char str[2] = {state->shift_pressed ? row2_shift[i] : row2_normal[i], '\0'};
                lv_label_set_text(label, str);
            }
        }
        
        // Row 3: Fill with common symbols
        for (int i = 0; i < 7 && idx < state->num_buttons; i++, idx++) {
            lv_obj_t *label = lv_obj_get_child(state->key_buttons[idx], 0);
            if (label) {
                const char *symbols = state->shift_pressed ? "~`_+{}|" : "-=[]\\;'";
                char str[2] = {symbols[i], '\0'};
                lv_label_set_text(label, str);
            }
        }
    } else {
        // Letter mode: Show Japanese characters or English letters
        const char **japanese_chars = NULL;
        
        // Select Japanese character set based on mode
        if (state->ime_state->mode == MODE_HIRAGANA) {
            japanese_chars = hiragana_hints;
        } else if (state->ime_state->mode == MODE_KATAKANA) {
            japanese_chars = katakana_hints;
        }
        
        // Row 1: Q-P (10 keys)
        for (int i = 0; i < 10 && idx < state->num_buttons; i++, idx++, hint_idx++) {
            lv_obj_t *label = lv_obj_get_child(state->key_buttons[idx], 0);
            if (label) {
                if (japanese_chars) {
                    // Show Japanese character
                    lv_label_set_text(label, japanese_chars[hint_idx]);
                } else {
                    // Show English letter
                    char c = qwerty_rows[0][i];
                    if (state->shift_pressed) {
                        c = c - 32;  // Convert to uppercase
                    }
                    char str[2] = {c, '\0'};
                    lv_label_set_text(label, str);
                }
            }
        }
        
        // Skip backspace button (idx++)
        idx++;
        
        // Row 2: A-L (9 keys)
        for (int i = 0; i < 9 && idx < state->num_buttons; i++, idx++, hint_idx++) {
            lv_obj_t *label = lv_obj_get_child(state->key_buttons[idx], 0);
            if (label) {
                if (japanese_chars) {
                    // Show Japanese character
                    lv_label_set_text(label, japanese_chars[hint_idx]);
                } else {
                    // Show English letter
                    char c = qwerty_rows[1][i];
                    if (state->shift_pressed) {
                        c = c - 32;  // Convert to uppercase
                    }
                    char str[2] = {c, '\0'};
                    lv_label_set_text(label, str);
                }
            }
        }
        
        // Row 3: Z-M (7 keys)
        for (int i = 0; i < 7 && idx < state->num_buttons; i++, idx++, hint_idx++) {
            lv_obj_t *label = lv_obj_get_child(state->key_buttons[idx], 0);
            if (label) {
                if (japanese_chars) {
                    // Show Japanese character
                    lv_label_set_text(label, japanese_chars[hint_idx]);
                } else {
                    // Show English letter
                    char c = qwerty_rows[2][i];
                    if (state->shift_pressed) {
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
    int btn_width = 45;
    int btn_height = 32;
    int btn_gap = 4;
    int start_y = 5;
    int button_index = 0;
    
    // Calculate centered positions for each row
    // Row 1: 10 buttons (45px) + backspace (50px) + 10 gaps (4px) = 540px total
    int row1_total_width = 10 * btn_width + 50 + 10 * btn_gap;
    int row1_start_x = (620 - row1_total_width) / 2;
    
    // Row 1: Q W E R T Y U I O P [Backspace]
    for (int i = 0; i < 10; i++) {
        lv_obj_t *btn = lv_button_create(parent);
        lv_obj_set_size(btn, btn_width, btn_height);
        lv_obj_set_pos(btn, row1_start_x + i * (btn_width + btn_gap), start_y);
        
        lv_obj_t *label = lv_label_create(btn);
        char str[2] = {qwerty_rows[0][i], '\0'};
        lv_label_set_text(label, str);
        lv_obj_set_style_text_font(label, state->japanese_font, 0);
        lv_obj_center(label);
        
        lv_obj_add_event_cb(btn, gui_button_event_cb, LV_EVENT_CLICKED, NULL);
        state->key_buttons[button_index++] = btn;
    }
    
    // Backspace button at end of row 1
    lv_obj_t *bksp_btn = lv_button_create(parent);
    lv_obj_set_size(bksp_btn, 50, btn_height);
    lv_obj_set_pos(bksp_btn, row1_start_x + 10 * (btn_width + btn_gap), start_y);
    lv_obj_t *bksp_label = lv_label_create(bksp_btn);
    lv_label_set_text(bksp_label, "←");
    lv_obj_set_style_text_font(bksp_label, state->japanese_font, 0);
    lv_obj_center(bksp_label);
    lv_obj_add_event_cb(bksp_btn, gui_special_button_event_cb, LV_EVENT_CLICKED, NULL);
    button_index++;  // Don't store in key_buttons array
    
    // Row 2: 9 buttons (45px) + 8 gaps (4px) = 437px total
    int row2_total_width = 9 * btn_width + 8 * btn_gap;
    int row2_start_x = (620 - row2_total_width) / 2;
    
    // Row 2: A S D F G H J K L
    start_y += btn_height + btn_gap;
    for (int i = 0; i < 9; i++) {
        lv_obj_t *btn = lv_button_create(parent);
        lv_obj_set_size(btn, btn_width, btn_height);
        lv_obj_set_pos(btn, row2_start_x + i * (btn_width + btn_gap), start_y);
        
        lv_obj_t *label = lv_label_create(btn);
        char str[2] = {qwerty_rows[1][i], '\0'};
        lv_label_set_text(label, str);
        lv_obj_set_style_text_font(label, state->japanese_font, 0);
        lv_obj_center(label);
        
        lv_obj_add_event_cb(btn, gui_button_event_cb, LV_EVENT_CLICKED, NULL);
        state->key_buttons[button_index++] = btn;
    }
    
    // Row 3: 7 buttons (45px) + 6 gaps (4px) = 339px total
    int row3_total_width = 7 * btn_width + 6 * btn_gap;
    int row3_start_x = (620 - row3_total_width) / 2;
    
    // Row 3: Z X C V B N M
    start_y += btn_height + btn_gap;
    for (int i = 0; i < 7; i++) {
        lv_obj_t *btn = lv_button_create(parent);
        lv_obj_set_size(btn, btn_width, btn_height);
        lv_obj_set_pos(btn, row3_start_x + i * (btn_width + btn_gap), start_y);
        
        lv_obj_t *label = lv_label_create(btn);
        char str[2] = {qwerty_rows[2][i], '\0'};
        lv_label_set_text(label, str);
        lv_obj_set_style_text_font(label, state->japanese_font, 0);
        lv_obj_center(label);
        
        lv_obj_add_event_cb(btn, gui_button_event_cb, LV_EVENT_CLICKED, NULL);
        state->key_buttons[button_index++] = btn;
    }
    
    // Row 4: Shift (62) + Space (165) + Enter (62) + 123 (62) + Clear (62) + Hyphen (45) + 5 gaps (4px) = 478px total
    int row4_total_width = 62 + 165 + 62 + 62 + 62 + 45 + 5 * btn_gap;
    int row4_start_x = (620 - row4_total_width) / 2;
    
    // Row 4: Shift, Space, Enter, 123/ABC, Clear, Hyphen
    start_y += btn_height + btn_gap;
    int current_x = row4_start_x;
    
    // Shift button
    state->shift_button = lv_button_create(parent);
    lv_obj_set_size(state->shift_button, 62, btn_height);
    lv_obj_set_pos(state->shift_button, current_x, start_y);
    lv_obj_t *shift_label = lv_label_create(state->shift_button);
    lv_label_set_text(shift_label, "Shift");
    lv_obj_set_style_text_font(shift_label, state->japanese_font, 0);
    lv_obj_center(shift_label);
    lv_obj_set_style_bg_color(state->shift_button, lv_color_hex(0x808080), 0);
    lv_obj_add_event_cb(state->shift_button, gui_shift_button_event_cb, LV_EVENT_CLICKED, NULL);
    current_x += 62 + btn_gap;
    
    // Space button
    lv_obj_t *space_btn = lv_button_create(parent);
    lv_obj_set_size(space_btn, 165, btn_height);
    lv_obj_set_pos(space_btn, current_x, start_y);
    lv_obj_t *space_label = lv_label_create(space_btn);
    lv_label_set_text(space_label, "Space");
    lv_obj_set_style_text_font(space_label, state->japanese_font, 0);
    lv_obj_center(space_label);
    lv_obj_add_event_cb(space_btn, gui_special_button_event_cb, LV_EVENT_CLICKED, NULL);
    current_x += 165 + btn_gap;
    
    // Enter button
    lv_obj_t *enter_btn = lv_button_create(parent);
    lv_obj_set_size(enter_btn, 62, btn_height);
    lv_obj_set_pos(enter_btn, current_x, start_y);
    lv_obj_t *enter_label = lv_label_create(enter_btn);
    lv_label_set_text(enter_label, "Enter");
    lv_obj_set_style_text_font(enter_label, state->japanese_font, 0);
    lv_obj_center(enter_label);
    lv_obj_add_event_cb(enter_btn, gui_special_button_event_cb, LV_EVENT_CLICKED, NULL);
    current_x += 62 + btn_gap;
    
    // 123/ABC mode toggle button
    lv_obj_t *mode_btn = lv_button_create(parent);
    lv_obj_set_size(mode_btn, 62, btn_height);
    lv_obj_set_pos(mode_btn, current_x, start_y);
    lv_obj_t *mode_label = lv_label_create(mode_btn);
    lv_label_set_text(mode_label, "123");
    lv_obj_set_style_text_font(mode_label, state->japanese_font, 0);
    lv_obj_center(mode_label);
    lv_obj_add_event_cb(mode_btn, gui_mode_button_event_cb, LV_EVENT_CLICKED, NULL);
    current_x += 62 + btn_gap;
    
    // Clear button
    lv_obj_t *clear_btn = lv_button_create(parent);
    lv_obj_set_size(clear_btn, 62, btn_height);
    lv_obj_set_pos(clear_btn, current_x, start_y);
    lv_obj_t *clear_label = lv_label_create(clear_btn);
    lv_label_set_text(clear_label, "Clear");
    lv_obj_set_style_text_font(clear_label, state->japanese_font, 0);
    lv_obj_center(clear_label);
    lv_obj_add_event_cb(clear_btn, gui_special_button_event_cb, LV_EVENT_CLICKED, NULL);
    current_x += 62 + btn_gap;
    
    // Hyphen button (for prolonged sound mark)
    lv_obj_t *hyphen_btn = lv_button_create(parent);
    lv_obj_set_size(hyphen_btn, 45, btn_height);
    lv_obj_set_pos(hyphen_btn, current_x, start_y);
    lv_obj_t *hyphen_label = lv_label_create(hyphen_btn);
    lv_label_set_text(hyphen_label, "-");
    lv_obj_set_style_text_font(hyphen_label, state->japanese_font, 0);
    lv_obj_center(hyphen_label);
    lv_obj_add_event_cb(hyphen_btn, gui_special_button_event_cb, LV_EVENT_CLICKED, NULL);
    
    state->num_buttons = button_index;
}

void gui_create_ui(GUIState *state) {
    // Create main screen
    state->screen = lv_screen_active();
    lv_obj_set_style_bg_color(state->screen, lv_color_hex(0xF5F5F5), 0);
    
    // Create mode label at top
    state->mode_label = lv_label_create(state->screen);
    lv_label_set_text(state->mode_label, "Mode: ひらがな");
    lv_obj_set_pos(state->mode_label, 10, 5);
    lv_obj_set_style_text_font(state->mode_label, state->japanese_font, 0);
    
    // Create mode switch button (Hiragana/Katakana/English)
    lv_obj_t *mode_btn = lv_button_create(state->screen);
    lv_obj_set_size(mode_btn, 140, 30);
    lv_obj_set_pos(mode_btn, 490, 5);
    lv_obj_t *mode_btn_label = lv_label_create(mode_btn);
    lv_label_set_text(mode_btn_label, "Mode: あ→ア→A");
    lv_obj_set_style_text_font(mode_btn_label, state->japanese_font, 0);
    lv_obj_center(mode_btn_label);
    lv_obj_add_event_cb(mode_btn, gui_cycle_mode_button_event_cb, LV_EVENT_CLICKED, NULL);
    
    // Create text area for displaying Japanese text
    state->text_area = lv_textarea_create(state->screen);
    lv_obj_set_size(state->text_area, 620, 140);
    lv_obj_set_pos(state->text_area, 10, 40);
    lv_textarea_set_text(state->text_area, "");
    lv_obj_set_style_text_font(state->text_area, state->japanese_font, 0);
    lv_textarea_set_placeholder_text(state->text_area, "Type here...");
    
    // Create buffer label (shows current romaji input)
    state->buffer_label = lv_label_create(state->screen);
    lv_label_set_text(state->buffer_label, "Input: ");
    lv_obj_set_pos(state->buffer_label, 10, 185);
    lv_obj_set_style_text_font(state->buffer_label, state->japanese_font, 0);
    
    // Create keyboard container
    state->keyboard_container = lv_obj_create(state->screen);
    lv_obj_set_size(state->keyboard_container, 620, 250);
    lv_obj_set_pos(state->keyboard_container, 10, 210);
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

