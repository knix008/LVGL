/**
 * @file keyboard_ui.c
 * @brief Keyboard UI implementation with Korean font support
 */

#include "keyboard_ui.h"
#include "english_input.h"
#include "number_input.h"
#include "lv_freetype.h"
#include <stdio.h>
#include <string.h>

/* UI Components */
static lv_obj_t* text_area = NULL;
static lv_obj_t* mode_label = NULL;
static lv_obj_t* btn_matrix = NULL;
static lv_font_t* korean_font = NULL;
static lv_font_t* korean_font_btn = NULL;

/* State */
static input_mode_t current_mode = INPUT_MODE_KOREAN;
static char output_buffer[1024] = {0};
static int output_len = 0;
static char temp_char[8] = {0};

/* Button labels - 12 key layout matching number mode */
static const char* korean_btn_map[] = {
    "ㅣ", "ㆍ", "ㅡ", "\n",
    "ㄱㅋ", "ㄴㄹ", "ㄷㅌ", "\n",
    "ㅂㅍ", "ㅅㅎ", "ㅈㅊ", "\n",
    "Space", "ㅇㅁ", "←", "\n",
    "Enter", "", ""
};

static const char* english_btn_map[] = {
    ".,!?", "abc", "def", "\n",
    "ghi", "jkl", "mno", "\n",
    "pqrs", "tuv", "wxyz", "\n",
    "Space", "Shift", "←", "\n",
    "Enter", "", ""
};

static const char* english_btn_map_upper[] = {
    ".,!?", "ABC", "DEF", "\n",
    "GHI", "JKL", "MNO", "\n",
    "PQRS", "TUV", "WXYZ", "\n",
    "Space", "Shift", "←", "\n",
    "Enter", "", ""
};

static const char* number_btn_map[] = {
    "1", "2", "3", "\n",
    "4", "5", "6", "\n",
    "7", "8", "9", "\n",
    "Space", "0", "←", "\n",
    "Enter", "", ""
};

/* UTF-8 encoding helper */
static int utf8_encode(uint32_t codepoint, char* buffer) {
    if (codepoint < 0x80) {
        buffer[0] = codepoint;
        return 1;
    } else if (codepoint < 0x800) {
        buffer[0] = 0xC0 | (codepoint >> 6);
        buffer[1] = 0x80 | (codepoint & 0x3F);
        return 2;
    } else if (codepoint < 0x10000) {
        buffer[0] = 0xE0 | (codepoint >> 12);
        buffer[1] = 0x80 | ((codepoint >> 6) & 0x3F);
        buffer[2] = 0x80 | (codepoint & 0x3F);
        return 3;
    } else if (codepoint < 0x110000) {
        buffer[0] = 0xF0 | (codepoint >> 18);
        buffer[1] = 0x80 | ((codepoint >> 12) & 0x3F);
        buffer[2] = 0x80 | ((codepoint >> 6) & 0x3F);
        buffer[3] = 0x80 | (codepoint & 0x3F);
        return 4;
    }
    return 0;
}

/* Append character to output */
static void append_char(uint32_t codepoint) {
    char utf8_buf[8] = {0};
    int len = utf8_encode(codepoint, utf8_buf);

    if (output_len + len < sizeof(output_buffer) - 1) {
        memcpy(output_buffer + output_len, utf8_buf, len);
        output_len += len;
        output_buffer[output_len] = '\0';
        lv_textarea_set_text(text_area, output_buffer);
    }
}

/* Update display for Korean mode - shows accumulated + composing */
static void update_korean_display() {
    char display_text[4096];  /* Large enough for output_buffer + chunjiin_text */
    char chunjiin_text[2048];

    /* Get current text from chunjiin engine */
    chunjiin_get_current_text_utf8(chunjiin_text, sizeof(chunjiin_text));

    /* Combine output_buffer + chunjiin_text safely */
    size_t output_size = strlen(output_buffer);
    size_t chunjiin_size = strlen(chunjiin_text);

    if (output_size + chunjiin_size < sizeof(display_text)) {
        memcpy(display_text, output_buffer, output_size);
        memcpy(display_text + output_size, chunjiin_text, chunjiin_size);
        display_text[output_size + chunjiin_size] = '\0';
        lv_textarea_set_text(text_area, display_text);
    }
}

/* Button click handler */
static void btn_event_cb(lv_event_t* e) {
    lv_obj_t* obj = lv_event_get_target(e);
    uint32_t id = lv_btnmatrix_get_selected_btn(obj);
    const char* txt = lv_btnmatrix_get_btn_text(obj, id);

    printf("Button pressed: %s (mode: %d)\n", txt, current_mode);

    /* Mode switch */
    if (strcmp(txt, "Mode") == 0) {
        current_mode = (current_mode + 1) % 3;
        keyboard_set_mode(current_mode);
        chunjiin_reset();
        return;
    }

    /* Clear */
    if (strcmp(txt, "Clear") == 0) {
        output_buffer[0] = '\0';
        output_len = 0;
        lv_textarea_set_text(text_area, "");
        chunjiin_reset();
        return;
    }

    /* Backspace */
    if (strcmp(txt, "←") == 0) {
        if (current_mode == INPUT_MODE_KOREAN) {
            /* Try to delete from composing buffer first */
            char chunjiin_text[2048];
            chunjiin_get_current_text_utf8(chunjiin_text, sizeof(chunjiin_text));

            if (strlen(chunjiin_text) > 0) {
                /* Delete from composing buffer */
                chunjiin_process_input('<');
            } else if (output_len > 0) {
                /* Delete from output buffer */
                int i = output_len - 1;
                while (i > 0 && (output_buffer[i] & 0xC0) == 0x80) i--;
                output_buffer[i] = '\0';
                output_len = i;
            }
            update_korean_display();
        } else {
            if (output_len > 0) {
                int i = output_len - 1;
                while (i > 0 && (output_buffer[i] & 0xC0) == 0x80) i--;
                output_buffer[i] = '\0';
                output_len = i;
                lv_textarea_set_text(text_area, output_buffer);
            }
        }
        return;
    }

    /* Enter */
    if (strcmp(txt, "Enter") == 0) {
        /* Finalize Korean composing text if needed */
        if (current_mode == INPUT_MODE_KOREAN) {
            char chunjiin_text[2048];
            chunjiin_get_current_text_utf8(chunjiin_text, sizeof(chunjiin_text));

            /* Append to output buffer if there's composing text */
            int len = strlen(chunjiin_text);
            if (len > 0 && output_len + len < sizeof(output_buffer) - 1) {
                memcpy(output_buffer + output_len, chunjiin_text, len);
                output_len += len;
                output_buffer[output_len] = '\0';
            }
            chunjiin_reset();
        }

        /* Show popup with current text */
        if (output_len > 0) {
            lv_obj_t* mbox = lv_msgbox_create(NULL);
            lv_msgbox_add_title(mbox, "Input Result");
            lv_obj_t* text_label = lv_msgbox_add_text(mbox, output_buffer);
            lv_msgbox_add_close_button(mbox);

            /* Apply Korean font to message box text if available */
            if (korean_font != NULL && text_label != NULL) {
                lv_obj_set_style_text_font(text_label, korean_font, 0);
            }
        }

        /* Clear output buffer and display */
        output_buffer[0] = '\0';
        output_len = 0;

        if (current_mode == INPUT_MODE_KOREAN) {
            update_korean_display();
        } else {
            lv_textarea_set_text(text_area, "");
        }
        return;
    }

    /* Shift (English mode only) */
    if (strcmp(txt, "Shift") == 0) {
        /* Toggle shift mode */
        int current_shift = english_get_shift();
        english_set_shift(!current_shift);

        /* Update button color and labels - Shift button is at index 10 in English mode (row 4, col 2) */
        if (current_mode == INPUT_MODE_ENGLISH) {
            if (!current_shift) {
                /* Shift is now ON - show uppercase labels and highlight button */
                lv_btnmatrix_set_map(btn_matrix, english_btn_map_upper);
                lv_btnmatrix_set_btn_ctrl(btn_matrix, 10, LV_BTNMATRIX_CTRL_CHECKED);
            } else {
                /* Shift is now OFF - show lowercase labels and normal button */
                lv_btnmatrix_set_map(btn_matrix, english_btn_map);
                lv_btnmatrix_clear_btn_ctrl(btn_matrix, 10, LV_BTNMATRIX_CTRL_CHECKED);
            }
        }
        return;
    }

    /* Space */
    if (strcmp(txt, "Space") == 0) {
        if (current_mode == INPUT_MODE_KOREAN) {
            /* Finalize current syllable and add to output */
            chunjiin_process_input(' ');
            char chunjiin_text[2048];
            chunjiin_get_current_text_utf8(chunjiin_text, sizeof(chunjiin_text));

            /* Append finalized text to output_buffer */
            int len = strlen(chunjiin_text);
            if (output_len + len < sizeof(output_buffer) - 1) {
                memcpy(output_buffer + output_len, chunjiin_text, len);
                output_len += len;
                output_buffer[output_len] = '\0';
            }

            /* Reset chunjiin for next input */
            chunjiin_reset();

            /* Update display */
            update_korean_display();
        } else {
            append_char(' ');
        }
        return;
    }

    /* Input handling based on mode */
    if (current_mode == INPUT_MODE_KOREAN) {
        /* Map button text to letter key */
        char key = 0;
        if (strcmp(txt, "ㄱㅋ") == 0) key = 'g';      // ㄱㅋ
        else if (strcmp(txt, "ㄴㄹ") == 0) key = 'n'; // ㄴㄹ
        else if (strcmp(txt, "ㄷㅌ") == 0) key = 'd'; // ㄷㅌ
        else if (strcmp(txt, "ㅂㅍ") == 0) key = 'b'; // ㅂㅍ
        else if (strcmp(txt, "ㅅㅎ") == 0) key = 's'; // ㅅㅎ
        else if (strcmp(txt, "ㅈㅊ") == 0) key = 'j'; // ㅈㅊ
        else if (strcmp(txt, "ㅇㅁ") == 0) key = 'm'; // ㅇㅁ
        else if (strcmp(txt, "ㅣ") == 0) key = 'i';   // ㅣ
        else if (strcmp(txt, "ㆍ") == 0) key = 'a';   // ㆍ
        else if (strcmp(txt, "ㅡ") == 0) key = 'e';   // ㅡ

        if (key) {
            chunjiin_process_input(key);
            update_korean_display();
        }
    } else if (current_mode == INPUT_MODE_ENGLISH) {
        /* English T9 - map button text to number key (handle both lowercase and uppercase) */
        char key = '0';
        if (strcmp(txt, ".,!?") == 0) key = '1';
        else if (strcmp(txt, "abc") == 0 || strcmp(txt, "ABC") == 0) key = '2';
        else if (strcmp(txt, "def") == 0 || strcmp(txt, "DEF") == 0) key = '3';
        else if (strcmp(txt, "ghi") == 0 || strcmp(txt, "GHI") == 0) key = '4';
        else if (strcmp(txt, "jkl") == 0 || strcmp(txt, "JKL") == 0) key = '5';
        else if (strcmp(txt, "mno") == 0 || strcmp(txt, "MNO") == 0) key = '6';
        else if (strcmp(txt, "pqrs") == 0 || strcmp(txt, "PQRS") == 0) key = '7';
        else if (strcmp(txt, "tuv") == 0 || strcmp(txt, "TUV") == 0) key = '8';
        else if (strcmp(txt, "wxyz") == 0 || strcmp(txt, "WXYZ") == 0) key = '9';
        else if (strcmp(txt, "Spc") == 0) key = '0';
        else if (strcmp(txt, "+") == 0) append_char('+');

        if (key != '0' || strcmp(txt, "Spc") == 0) {
            int is_replacement = 0;
            char c = english_process_key(key, &is_replacement);
            if (c != 0) {
                if (is_replacement && output_len > 0) {
                    /* Replace last character - remove it first */
                    int i = output_len - 1;
                    while (i > 0 && (output_buffer[i] & 0xC0) == 0x80) i--;
                    output_buffer[i] = '\0';
                    output_len = i;
                }
                /* Add the new/replacement character */
                append_char(c);
            }
        }
    } else if (current_mode == INPUT_MODE_NUMBER) {
        /* Number input - only 0-9 digits */
        if (txt[0] >= '0' && txt[0] <= '9') {
            char c = number_process_key(txt[0]);
            if (c != 0) {
                append_char(c);
            }
        }
    }
}

/* Load Korean fonts */
static void load_fonts(void) {
    /* Try to load Korean font - use NanumGothic from fonts directory */
    korean_font = lv_freetype_font_create("../fonts/NanumGothic.ttf",
                                         LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                         24,
                                         LV_FREETYPE_FONT_STYLE_NORMAL);

    if (korean_font == NULL) {
        printf("Warning: Could not load Korean font for text area\n");
    } else {
        printf("Korean font loaded successfully (24px)\n");
    }

    korean_font_btn = lv_freetype_font_create("../fonts/NanumGothic.ttf",
                                              LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                              18,
                                              LV_FREETYPE_FONT_STYLE_NORMAL);

    if (korean_font_btn == NULL) {
        printf("Warning: Could not load Korean font for buttons\n");
    } else {
        printf("Korean button font loaded successfully (18px)\n");
    }
}

/* Mode switch button callback */
static void mode_btn_cb(lv_event_t* e) {
    /* Before switching, finalize any Korean composing text */
    if (current_mode == INPUT_MODE_KOREAN) {
        char chunjiin_text[2048];
        chunjiin_get_current_text_utf8(chunjiin_text, sizeof(chunjiin_text));

        /* Append to output buffer if there's composing text */
        int len = strlen(chunjiin_text);
        if (len > 0 && output_len + len < sizeof(output_buffer) - 1) {
            memcpy(output_buffer + output_len, chunjiin_text, len);
            output_len += len;
            output_buffer[output_len] = '\0';
        }

        /* Reset chunjiin for next time */
        chunjiin_reset();
    }

    /* Switch mode */
    current_mode = (current_mode + 1) % 3;
    keyboard_set_mode(current_mode);

    /* Clear checked state from all buttons when switching modes */
    /* This prevents the Shift button state in English mode from affecting other modes */
    for (int i = 0; i < 16; i++) {
        lv_btnmatrix_clear_btn_ctrl(btn_matrix, i, LV_BTNMATRIX_CTRL_CHECKED);
    }

    /* Update display to show accumulated output */
    if (current_mode == INPUT_MODE_KOREAN) {
        update_korean_display();
    } else {
        lv_textarea_set_text(text_area, output_buffer);
    }
}

/* Clear button callback */
static void clear_btn_cb(lv_event_t* e) {
    output_buffer[0] = '\0';
    output_len = 0;
    lv_textarea_set_text(text_area, "");
    chunjiin_reset();
}

void keyboard_ui_init(void) {
    chunjiin_init();
    load_fonts();

    lv_obj_t* scr = lv_scr_act();

    /* Create top button bar */
    lv_obj_t* top_bar = lv_obj_create(scr);
    lv_obj_set_size(top_bar, 380, 40);
    lv_obj_align(top_bar, LV_ALIGN_TOP_MID, 0, 5);
    lv_obj_set_style_pad_all(top_bar, 2, 0);

    /* Mode button */
    lv_obj_t* mode_btn = lv_btn_create(top_bar);
    lv_obj_set_size(mode_btn, 90, 35);
    lv_obj_align(mode_btn, LV_ALIGN_LEFT_MID, 5, 0);
    lv_obj_t* mode_btn_label = lv_label_create(mode_btn);
    lv_label_set_text(mode_btn_label, "MODE");
    lv_obj_center(mode_btn_label);
    lv_obj_add_event_cb(mode_btn, mode_btn_cb, LV_EVENT_CLICKED, NULL);

    /* Clear button */
    lv_obj_t* clear_btn = lv_btn_create(top_bar);
    lv_obj_set_size(clear_btn, 90, 35);
    lv_obj_align(clear_btn, LV_ALIGN_RIGHT_MID, -5, 0);
    lv_obj_t* clear_btn_label = lv_label_create(clear_btn);
    lv_label_set_text(clear_btn_label, "CLEAR");
    lv_obj_center(clear_btn_label);
    lv_obj_add_event_cb(clear_btn, clear_btn_cb, LV_EVENT_CLICKED, NULL);

    /* Mode indicator in center */
    mode_label = lv_label_create(top_bar);
    lv_label_set_text(mode_label, "한글");
    lv_obj_center(mode_label);

    /* Set Korean font for mode label if loaded */
    if (korean_font_btn != NULL) {
        lv_obj_set_style_text_font(mode_label, korean_font_btn, 0);
    }

    /* Create text display area */
    text_area = lv_textarea_create(scr);
    lv_obj_set_size(text_area, 380, 150);
    lv_obj_align(text_area, LV_ALIGN_TOP_MID, 0, 50);
    lv_textarea_set_text(text_area, "");
    lv_textarea_set_placeholder_text(text_area, "Type here...");

    /* Set Korean font if loaded */
    if (korean_font != NULL) {
        lv_obj_set_style_text_font(text_area, korean_font, 0);
    }

    /* Create button matrix */
    btn_matrix = lv_btnmatrix_create(scr);
    lv_obj_set_size(btn_matrix, 380, 430);
    lv_obj_align(btn_matrix, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_btnmatrix_set_map(btn_matrix, korean_btn_map);
    lv_obj_add_event_cb(btn_matrix, btn_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    /* Set Korean font for buttons if loaded */
    if (korean_font_btn != NULL) {
        lv_obj_set_style_text_font(btn_matrix, korean_font_btn, 0);
    }

    /* Button colors - normal state */
    lv_obj_set_style_bg_color(btn_matrix, lv_color_hex(0x4A90E2), LV_PART_ITEMS);  // Blue background
    lv_obj_set_style_bg_opa(btn_matrix, LV_OPA_COVER, LV_PART_ITEMS);
    lv_obj_set_style_text_color(btn_matrix, lv_color_hex(0xFFFFFF), LV_PART_ITEMS);  // White text

    /* Button colors - pressed state */
    lv_obj_set_style_bg_color(btn_matrix, lv_color_hex(0x2E5F8A), LV_PART_ITEMS | LV_STATE_PRESSED);  // Darker blue
    lv_obj_set_style_bg_opa(btn_matrix, LV_OPA_COVER, LV_PART_ITEMS | LV_STATE_PRESSED);

    /* Button colors - checked state (for Shift button) */
    lv_obj_set_style_bg_color(btn_matrix, lv_color_hex(0xFF8C00), LV_PART_ITEMS | LV_STATE_CHECKED);  // Orange when active
    lv_obj_set_style_bg_opa(btn_matrix, LV_OPA_COVER, LV_PART_ITEMS | LV_STATE_CHECKED);

    /* Button border */
    lv_obj_set_style_border_width(btn_matrix, 2, LV_PART_ITEMS);
    lv_obj_set_style_border_color(btn_matrix, lv_color_hex(0x2E5F8A), LV_PART_ITEMS);
    lv_obj_set_style_border_opa(btn_matrix, LV_OPA_50, LV_PART_ITEMS);
}

void keyboard_set_mode(input_mode_t mode) {
    current_mode = mode;

    switch (mode) {
        case INPUT_MODE_KOREAN:
            lv_btnmatrix_set_map(btn_matrix, korean_btn_map);
            lv_label_set_text(mode_label, "한글");
            break;
        case INPUT_MODE_ENGLISH:
            lv_btnmatrix_set_map(btn_matrix, english_btn_map);
            lv_label_set_text(mode_label, "ABC");
            break;
        case INPUT_MODE_NUMBER:
            lv_btnmatrix_set_map(btn_matrix, number_btn_map);
            lv_label_set_text(mode_label, "123");
            break;
    }
}

input_mode_t keyboard_get_mode(void) {
    return current_mode;
}

const char* keyboard_get_text(void) {
    return output_buffer;
}
