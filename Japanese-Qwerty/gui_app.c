/*
 * Japanese Input GUI Application Implementation
 * Provides a QWERTY keyboard with romaji to hiragana conversion
 */

#include "gui_app.h"
#include "lvgl_init.h"
#include <string.h>
#include <stdio.h>

// UI Components
static lv_obj_t *text_area = NULL;
static lv_obj_t *keyboard = NULL;
static lv_obj_t *mode_label = NULL;
static lv_obj_t *buffer_label = NULL;

// Input state
static char input_buffer[64] = "";  // Buffer for romaji input
static bool japanese_mode = true;   // Toggle between Japanese and direct input

// Romaji to Hiragana conversion table
typedef struct {
    const char *romaji;
    const char *hiragana;
} RomajiMap;

static const RomajiMap romaji_table[] = {
    // Vowels
    {"a", "あ"}, {"i", "い"}, {"u", "う"}, {"e", "え"}, {"o", "お"},

    // K column
    {"ka", "か"}, {"ki", "き"}, {"ku", "く"}, {"ke", "け"}, {"ko", "こ"},
    {"kya", "きゃ"}, {"kyu", "きゅ"}, {"kyo", "きょ"},

    // S column
    {"sa", "さ"}, {"shi", "し"}, {"su", "す"}, {"se", "せ"}, {"so", "そ"},
    {"sha", "しゃ"}, {"shu", "しゅ"}, {"sho", "しょ"},

    // T column
    {"ta", "た"}, {"chi", "ち"}, {"tsu", "つ"}, {"te", "て"}, {"to", "と"},
    {"cha", "ちゃ"}, {"chu", "ちゅ"}, {"cho", "ちょ"},

    // N column
    {"na", "な"}, {"ni", "に"}, {"nu", "ぬ"}, {"ne", "ね"}, {"no", "の"},
    {"nya", "にゃ"}, {"nyu", "にゅ"}, {"nyo", "にょ"},
    {"nn", "ん"}, {"n", "ん"},

    // H column
    {"ha", "は"}, {"hi", "ひ"}, {"fu", "ふ"}, {"he", "へ"}, {"ho", "ほ"},
    {"hya", "ひゃ"}, {"hyu", "ひゅ"}, {"hyo", "ひょ"},

    // M column
    {"ma", "ま"}, {"mi", "み"}, {"mu", "む"}, {"me", "め"}, {"mo", "も"},
    {"mya", "みゃ"}, {"myu", "みゅ"}, {"myo", "みょ"},

    // Y column
    {"ya", "や"}, {"yu", "ゆ"}, {"yo", "よ"},

    // R column
    {"ra", "ら"}, {"ri", "り"}, {"ru", "る"}, {"re", "れ"}, {"ro", "ろ"},
    {"rya", "りゃ"}, {"ryu", "りゅ"}, {"ryo", "りょ"},

    // W column
    {"wa", "わ"}, {"wo", "を"},

    // G column
    {"ga", "が"}, {"gi", "ぎ"}, {"gu", "ぐ"}, {"ge", "げ"}, {"go", "ご"},
    {"gya", "ぎゃ"}, {"gyu", "ぎゅ"}, {"gyo", "ぎょ"},

    // Z column
    {"za", "ざ"}, {"ji", "じ"}, {"zu", "ず"}, {"ze", "ぜ"}, {"zo", "ぞ"},
    {"ja", "じゃ"}, {"ju", "じゅ"}, {"jo", "じょ"},

    // D column
    {"da", "だ"}, {"di", "ぢ"}, {"du", "づ"}, {"de", "で"}, {"do", "ど"},

    // B column
    {"ba", "ば"}, {"bi", "び"}, {"bu", "ぶ"}, {"be", "べ"}, {"bo", "ぼ"},
    {"bya", "びゃ"}, {"byu", "びゅ"}, {"byo", "びょ"},

    // P column
    {"pa", "ぱ"}, {"pi", "ぴ"}, {"pu", "ぷ"}, {"pe", "ぺ"}, {"po", "ぽ"},
    {"pya", "ぴゃ"}, {"pyu", "ぴゅ"}, {"pyo", "ぴょ"},

    {NULL, NULL} // End marker
};

// Function to convert romaji to hiragana
static const char* romaji_to_hiragana(const char *romaji) {
    if (!romaji || strlen(romaji) == 0) {
        return NULL;
    }

    // Check for exact matches (longest first)
    for (int i = 0; romaji_table[i].romaji != NULL; i++) {
        if (strcmp(romaji, romaji_table[i].romaji) == 0) {
            return romaji_table[i].hiragana;
        }
    }

    return NULL;
}

// Try to convert the longest possible romaji sequence
static bool try_convert_buffer(void) {
    size_t len = strlen(input_buffer);

    // Try from longest to shortest
    for (size_t i = len; i > 0; i--) {
        char temp[64];
        strncpy(temp, input_buffer, i);
        temp[i] = '\0';

        const char *hiragana = romaji_to_hiragana(temp);
        if (hiragana) {
            // Add the hiragana to text area
            const char *current_text = lv_textarea_get_text(text_area);
            char new_text[1024];
            snprintf(new_text, sizeof(new_text), "%s%s", current_text, hiragana);
            lv_textarea_set_text(text_area, new_text);

            // Remove converted part from buffer
            memmove(input_buffer, input_buffer + i, len - i + 1);
            return true;
        }
    }

    return false;
}

// Update the buffer display
static void update_buffer_display(void) {
    if (buffer_label) {
        char display_text[128];
        snprintf(display_text, sizeof(display_text), "Buffer: %s", input_buffer);
        lv_label_set_text(buffer_label, display_text);
    }
}

// Keyboard event handler
static void keyboard_event_cb(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *kb = lv_event_get_target(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        const char *key_text = lv_keyboard_get_btn_text(kb, lv_keyboard_get_selected_btn(kb));

        if (!key_text) return;

        // Handle special keys
        if (strcmp(key_text, LV_SYMBOL_BACKSPACE) == 0) {
            // First try to delete from buffer
            size_t buf_len = strlen(input_buffer);
            if (buf_len > 0) {
                input_buffer[buf_len - 1] = '\0';
                update_buffer_display();
            } else {
                // If buffer is empty, delete from text area
                lv_textarea_delete_char(text_area);
            }
        }
        else if (strcmp(key_text, LV_SYMBOL_NEW_LINE) == 0) {
            // Convert any remaining buffer and add newline
            if (strlen(input_buffer) > 0) {
                // Add remaining buffer as-is if no conversion possible
                const char *current_text = lv_textarea_get_text(text_area);
                char new_text[1024];
                snprintf(new_text, sizeof(new_text), "%s%s", current_text, input_buffer);
                lv_textarea_set_text(text_area, new_text);
                input_buffer[0] = '\0';
            }
            lv_textarea_add_char(text_area, '\n');
            update_buffer_display();
        }
        else if (strcmp(key_text, "ABC") == 0 || strcmp(key_text, "あア") == 0) {
            // Toggle mode
            japanese_mode = !japanese_mode;

            // Clear buffer when switching modes
            input_buffer[0] = '\0';
            update_buffer_display();

            if (mode_label) {
                lv_label_set_text(mode_label, japanese_mode ? "Mode: Japanese (Hiragana)" : "Mode: Direct Input");
            }
        }
        else if (strcmp(key_text, LV_SYMBOL_LEFT) == 0) {
            lv_textarea_cursor_left(text_area);
        }
        else if (strcmp(key_text, LV_SYMBOL_RIGHT) == 0) {
            lv_textarea_cursor_right(text_area);
        }
        else {
            // Regular character input
            if (japanese_mode) {
                // Add to buffer
                size_t buf_len = strlen(input_buffer);
                if (buf_len < sizeof(input_buffer) - 2) {
                    // Convert to lowercase for romaji matching
                    char lowercase_key[8];
                    size_t key_len = strlen(key_text);
                    for (size_t i = 0; i < key_len && i < sizeof(lowercase_key) - 1; i++) {
                        lowercase_key[i] = (key_text[i] >= 'A' && key_text[i] <= 'Z')
                            ? key_text[i] + 32 : key_text[i];
                    }
                    lowercase_key[key_len] = '\0';

                    strncat(input_buffer, lowercase_key, sizeof(input_buffer) - buf_len - 1);

                    // Try to convert
                    if (!try_convert_buffer()) {
                        // Check if buffer is getting too long without conversion
                        if (strlen(input_buffer) > 4) {
                            // Add first character as-is and keep rest
                            char first_char[2] = {input_buffer[0], '\0'};
                            const char *current_text = lv_textarea_get_text(text_area);
                            char new_text[1024];
                            snprintf(new_text, sizeof(new_text), "%s%s", current_text, first_char);
                            lv_textarea_set_text(text_area, new_text);

                            // Remove first character from buffer
                            memmove(input_buffer, input_buffer + 1, strlen(input_buffer));
                        }
                    }

                    update_buffer_display();
                }
            } else {
                // Direct input mode - just add the character
                lv_textarea_add_text(text_area, key_text);
            }
        }
    }
}

// Initialize GUI application
int gui_app_init(void) {
    input_buffer[0] = '\0';
    japanese_mode = true;

    printf("GUI application initialized\n");
    return 0;
}

// Create the user interface
void gui_app_create_ui(void) {
    lv_obj_t *screen = lv_scr_act();

    // Create a container for the entire UI
    lv_obj_t *main_cont = lv_obj_create(screen);
    lv_obj_set_size(main_cont, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_all(main_cont, 5, 0);

    // Mode label at the top
    mode_label = lv_label_create(main_cont);
    lv_label_set_text(mode_label, "Mode: Japanese (Hiragana)");
    lv_obj_set_style_text_font(mode_label, &lv_font_montserrat_14, 0);
    lv_obj_align(mode_label, LV_ALIGN_TOP_MID, 0, 5);

    // Buffer display label
    buffer_label = lv_label_create(main_cont);
    lv_label_set_text(buffer_label, "Buffer: ");
    lv_obj_set_style_text_color(buffer_label, lv_color_hex(0x0080FF), 0);
    lv_obj_align(buffer_label, LV_ALIGN_TOP_MID, 0, 30);

    // Text area for displaying input
    text_area = lv_textarea_create(main_cont);
    lv_obj_set_size(text_area, LV_PCT(95), 180);
    lv_obj_align(text_area, LV_ALIGN_TOP_MID, 0, 55);
    lv_textarea_set_placeholder_text(text_area, "Type in romaji to get hiragana...");

    // Create QWERTY keyboard
    keyboard = lv_keyboard_create(main_cont);
    lv_obj_set_size(keyboard, LV_PCT(95), 350);
    lv_obj_align(keyboard, LV_ALIGN_BOTTOM_MID, 0, -5);
    lv_keyboard_set_textarea(keyboard, text_area);
    lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_TEXT_LOWER);

    // Add event handler for keyboard
    lv_obj_add_event_cb(keyboard, keyboard_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    printf("UI created successfully\n");
}

// Cleanup resources
void gui_app_cleanup(void) {
    // Objects are automatically cleaned up by LVGL
    text_area = NULL;
    keyboard = NULL;
    mode_label = NULL;
    buffer_label = NULL;

    printf("GUI application cleaned up\n");
}
