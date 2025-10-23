/**
 * @file keypad.c
 * @brief Mobile-style keypad implementation (3 columns x 5 rows)
 *
 * This module implements a mobile-style keypad with multiple input modes.
 */

#include "keypad.h"
#include <string.h>

#define KEYPAD_COLS 3
#define KEYPAD_ROWS 5
#define KEYPAD_TOTAL_KEYS (KEYPAD_COLS * KEYPAD_ROWS)

/**
 * @brief Keypad data structure
 */
typedef struct {
    lv_obj_t *container;              /**< Main container */
    lv_obj_t *buttons[KEYPAD_TOTAL_KEYS]; /**< Button array */
    lv_obj_t *shift_btn;              /**< Shift button reference */
    lv_obj_t *target_textarea;        /**< Target textarea */
    keypad_mode_t current_mode;       /**< Current input mode */
    bool shift_active;                /**< Shift key state (uppercase when true) */
    int width;                         /**< Keypad width */
    int height;                        /**< Keypad height */
    lv_font_t *button_font;           /**< Font for button labels */
    const char *last_button_text;    /**< Last clicked button text */
    int cycle_index;                  /**< Current cycle index for multi-char buttons */
    uint32_t last_click_time;         /**< Time of last button click */
} keypad_data_t;

// Key layouts for different modes (3x5 = 15 keys)
// All buttons integrated into 3x5 grid
// Layout: [row][col]
// Row 0-2: Main input keys (max 3 chars per button for English)
// Row 3: Additional letters and symbols
// Row 4: Function keys (Shift, Mode, Backspace)
static const char *keypad_layouts[KEYPAD_MODE_COUNT][KEYPAD_TOTAL_KEYS] = {
    // KEYPAD_MODE_LOWERCASE
    {
        "abc", "def", "ghi",
        "jkl", "mno", "pqr",
        "stu", "vwx", "yz",
        "@", "_", ".",
        "SHIFT", "123", "←"  // Shift, Mode switch, Backspace
    },
    // KEYPAD_MODE_UPPERCASE
    {
        "ABC", "DEF", "GHI",
        "JKL", "MNO", "PQR",
        "STU", "VWX", "YZ",
        "@", "_", ".",
        "SHIFT", "123", "←"  // Shift, Mode switch, Backspace
    },
    // KEYPAD_MODE_NUMBERS
    {
        "1", "2", "3",
        "4", "5", "6",
        "7", "8", "9",
        "@", "0", ".",
        "SHIFT", "ABC", "←"  // Shift, Mode switch, Backspace
    },
    // KEYPAD_MODE_SPECIAL
    {
        "!", "@", "#",
        "$", "%", "^",
        "&", "*", "(",
        ")", "-", "?",
        "SHIFT", "ABC", "←"  // Shift, Mode switch, Backspace
    }
};


/**
 * @brief Forward declarations
 */
static void keypad_button_event_cb(lv_event_t *e);
static void keypad_update_layout(keypad_data_t *data);

/**
 * @brief Button click event handler
 */
static void keypad_button_event_cb(lv_event_t *e) {
    lv_obj_t *btn = lv_event_get_target(e);
    lv_event_code_t code = lv_event_get_code(e);

    if (code != LV_EVENT_CLICKED) {
        return;
    }

    // Get keypad data from parent container
    lv_obj_t *container = lv_obj_get_parent(btn);
    keypad_data_t *data = (keypad_data_t *)lv_obj_get_user_data(container);

    if (!data || !data->target_textarea) {
        return;
    }

    // Get button label
    lv_obj_t *label = lv_obj_get_child(btn, 0);
    const char *txt = lv_label_get_text(label);

    // Get current time for timing button clicks
    uint32_t current_time = lv_tick_get();
    uint32_t time_diff = current_time - data->last_click_time;

    // Handle mode switching keys (integrated in keypad)
    if (strcmp(txt, "SHIFT") == 0) {
        // Shift key - dual functionality:
        // In English mode: toggle between lowercase and uppercase
        // In Numbers/Special mode: toggle between numbers and special characters
        data->shift_active = !data->shift_active;

        if (data->current_mode == KEYPAD_MODE_LOWERCASE || data->current_mode == KEYPAD_MODE_UPPERCASE) {
            // English mode - toggle case
            if (data->shift_active) {
                data->current_mode = KEYPAD_MODE_UPPERCASE;
            } else {
                data->current_mode = KEYPAD_MODE_LOWERCASE;
            }
        } else if (data->current_mode == KEYPAD_MODE_NUMBERS || data->current_mode == KEYPAD_MODE_SPECIAL) {
            // Numbers/Special mode - toggle between numbers and special characters
            if (data->shift_active) {
                data->current_mode = KEYPAD_MODE_SPECIAL;
            } else {
                data->current_mode = KEYPAD_MODE_NUMBERS;
            }
        }

        keypad_update_layout(data);

        // Update shift button color
        if (data->shift_btn) {
            if (data->shift_active) {
                lv_obj_set_style_bg_color(data->shift_btn, lv_color_hex(0xFF8C00), 0);  // Orange when active
            } else {
                lv_obj_set_style_bg_color(data->shift_btn, lv_color_hex(0x4CAF50), 0);  // Green when inactive
            }
        }

        // Reset cycling state
        data->last_button_text = NULL;
        data->cycle_index = 0;
    } else if (strcmp(txt, "ABC") == 0) {
        // In English mode, treat 'ABC' as a character input button
        const char *abc_chars = data->shift_active ? "ABC" : "abc";
        int txt_len = strlen(abc_chars);
        if (txt_len > 1) {
            if (data->last_button_text && strcmp(data->last_button_text, txt) == 0 && time_diff < 1000) {
                lv_textarea_delete_char(data->target_textarea);
                data->cycle_index = (data->cycle_index + 1) % txt_len;
            } else {
                data->cycle_index = 0;
            }
            char ch[2] = {abc_chars[data->cycle_index], '\0'};
            lv_textarea_add_text(data->target_textarea, ch);
            data->last_button_text = txt;
        } else {
            const char *current_text = lv_textarea_get_text(data->target_textarea);
            if (strlen(current_text) < 32) {
                lv_textarea_add_text(data->target_textarea, abc_chars);
            }
            data->last_button_text = NULL;
            data->cycle_index = 0;
        }
    } else if (strcmp(txt, "123") == 0) {
        // Switch to Numbers/Special mode - respect current shift state
        if (data->shift_active) {
            keypad_set_mode(container, KEYPAD_MODE_SPECIAL);
        } else {
            keypad_set_mode(container, KEYPAD_MODE_NUMBERS);
        }
        data->last_button_text = NULL;
        data->cycle_index = 0;
    } else if (strcmp(txt, "⌫") == 0 || strcmp(txt, "←") == 0) {
        // Backspace - delete last character
        lv_textarea_delete_char(data->target_textarea);
        data->last_button_text = NULL;
        data->cycle_index = 0;
    } else if (strcmp(txt, " ") == 0) {
        // Space character - check length before adding
        const char *current_text = lv_textarea_get_text(data->target_textarea);
        if (strlen(current_text) < 32) {
            lv_textarea_add_text(data->target_textarea, " ");
        }
        data->last_button_text = NULL;
        data->cycle_index = 0;
    } else {
        // Regular character button - implement T9-style cycling
        int txt_len = strlen(txt);

        // Check if this is a multi-character button (more than 1 char)
        if (txt_len > 1) {
            // Check if same button clicked within 1 second
            if (data->last_button_text && strcmp(data->last_button_text, txt) == 0 && time_diff < 1000) {
                // Same button - cycle to next character
                // Delete the last character first
                lv_textarea_delete_char(data->target_textarea);

                // Advance to next character in cycle
                data->cycle_index = (data->cycle_index + 1) % txt_len;
            } else {
                // Different button or timeout - start new cycle
                data->cycle_index = 0;
            }

            // Add the current character from the cycle (already checked by textarea max_length)
            char ch[2] = {txt[data->cycle_index], '\0'};
            lv_textarea_add_text(data->target_textarea, ch);

            // Store current button info
            data->last_button_text = txt;
        } else {
            // Single character button - check length before adding
            const char *current_text = lv_textarea_get_text(data->target_textarea);
            if (strlen(current_text) < 32) {
                lv_textarea_add_text(data->target_textarea, txt);
            }
            data->last_button_text = NULL;
            data->cycle_index = 0;
        }
    }

    // Update last click time
    data->last_click_time = current_time;
}

/**
 * @brief Updates keypad layout based on current mode
 */
static void keypad_update_layout(keypad_data_t *data) {
    if (!data) {
        return;
    }

    // Update button labels
    for (int i = 0; i < KEYPAD_TOTAL_KEYS; i++) {
        lv_obj_t *label = lv_obj_get_child(data->buttons[i], 0);
        lv_label_set_text(label, keypad_layouts[data->current_mode][i]);
    }
}

/**
 * @brief Creates a mobile-style keypad
 */
lv_obj_t *keypad_create(const keypad_config_t *config) {
    if (!config || !config->parent) {
        return NULL;
    }

    // Allocate keypad data
    keypad_data_t *data = lv_malloc(sizeof(keypad_data_t));
    if (!data) {
        return NULL;
    }

    memset(data, 0, sizeof(keypad_data_t));
    data->target_textarea = config->target_textarea;
    data->current_mode = KEYPAD_MODE_LOWERCASE;
    data->shift_active = false;
    data->width = config->width;
    data->height = config->height;
    data->button_font = config->button_font;
    data->last_button_text = NULL;
    data->cycle_index = 0;
    data->last_click_time = 0;

    // Create main container
    data->container = lv_obj_create(config->parent);
    lv_obj_set_size(data->container, config->width, config->height);
    lv_obj_set_style_pad_top(data->container, 5, 0);
    lv_obj_set_style_pad_left(data->container, 5, 0);
    lv_obj_set_style_pad_right(data->container, 5, 0);
    lv_obj_set_style_pad_bottom(data->container, 20, 0);  // Extra bottom padding
    lv_obj_set_style_pad_gap(data->container, 5, 0);
    lv_obj_clear_flag(data->container, LV_OBJ_FLAG_SCROLLABLE);

    // Store data in container
    lv_obj_set_user_data(data->container, data);

    // Calculate button size (accounting for padding: 5+5+20 top/left/right/bottom, and 5px gaps)
    int btn_width = (config->width - 5 - 5 - 4 * 5) / KEYPAD_COLS;  // left pad + right pad + 4 gaps
    int btn_height = (config->height - 5 - 20 - 4 * 5) / KEYPAD_ROWS;  // top pad + bottom pad + 4 gaps

    // Create buttons in 3x5 grid
    for (int row = 0; row < KEYPAD_ROWS; row++) {
        for (int col = 0; col < KEYPAD_COLS; col++) {
            int idx = row * KEYPAD_COLS + col;

            // Create button
            data->buttons[idx] = lv_button_create(data->container);
            lv_obj_set_size(data->buttons[idx], btn_width, btn_height);

            // Position button
            int x = 5 + col * (btn_width + 5);
            int y = 5 + row * (btn_height + 5);
            lv_obj_set_pos(data->buttons[idx], x, y);

            // Create label for button
            lv_obj_t *label = lv_label_create(data->buttons[idx]);
            lv_label_set_text(label, keypad_layouts[data->current_mode][idx]);

            // Apply font if provided
            if (data->button_font) {
                lv_obj_set_style_text_font(label, data->button_font, 0);
            }

            lv_obj_center(label);

            // Store shift button reference (index 12 = row 4, col 0)
            if (idx == 12) {
                data->shift_btn = data->buttons[idx];
                // Set initial shift button color (inactive/green)
                lv_obj_set_style_bg_color(data->shift_btn, lv_color_hex(0x4CAF50), 0);
            }

            // Style mode switch button (index 13 = row 4, col 1)
            if (idx == 13) {
                lv_obj_set_style_bg_color(data->buttons[idx], lv_color_hex(0x9C27B0), 0);  // Purple color
            }

            // Style backspace button (index 14 = row 4, col 2)
            if (idx == 14) {
                lv_obj_set_style_bg_color(data->buttons[idx], lv_color_hex(0xF44336), 0);  // Red color
            }

            // Add event handler
            lv_obj_add_event_cb(data->buttons[idx], keypad_button_event_cb,
                              LV_EVENT_CLICKED, NULL);
        }
    }

    return data->container;
}

/**
 * @brief Sets the target textarea for keypad input
 */
void keypad_set_target(lv_obj_t *keypad, lv_obj_t *textarea) {
    if (!keypad) {
        return;
    }

    keypad_data_t *data = (keypad_data_t *)lv_obj_get_user_data(keypad);
    if (data) {
        data->target_textarea = textarea;
    }
}

/**
 * @brief Sets the current keypad mode
 */
void keypad_set_mode(lv_obj_t *keypad, keypad_mode_t mode) {
    if (!keypad || mode >= KEYPAD_MODE_COUNT) {
        return;
    }

    keypad_data_t *data = (keypad_data_t *)lv_obj_get_user_data(keypad);
    if (!data) {
        return;
    }

    data->current_mode = mode;

    // Update shift_active state based on the mode
    if (mode == KEYPAD_MODE_UPPERCASE || mode == KEYPAD_MODE_SPECIAL) {
        data->shift_active = true;
    } else if (mode == KEYPAD_MODE_LOWERCASE || mode == KEYPAD_MODE_NUMBERS) {
        data->shift_active = false;
    }

    keypad_update_layout(data);

    // Update shift button color based on shift state
    if (data->shift_btn) {
        if (data->shift_active) {
            // Orange color when shift is active
            lv_obj_set_style_bg_color(data->shift_btn, lv_color_hex(0xFF8C00), 0);
        } else {
            // Green color when shift is inactive
            lv_obj_set_style_bg_color(data->shift_btn, lv_color_hex(0x4CAF50), 0);
        }
    }
}

/**
 * @brief Gets the current keypad mode
 */
keypad_mode_t keypad_get_mode(lv_obj_t *keypad) {
    if (!keypad) {
        return KEYPAD_MODE_LOWERCASE;
    }

    keypad_data_t *data = (keypad_data_t *)lv_obj_get_user_data(keypad);
    if (!data) {
        return KEYPAD_MODE_LOWERCASE;
    }

    return data->current_mode;
}

/**
 * @brief Deletes the keypad and frees resources
 */
void keypad_delete(lv_obj_t *keypad) {
    if (!keypad) {
        return;
    }

    keypad_data_t *data = (keypad_data_t *)lv_obj_get_user_data(keypad);
    if (data) {
        lv_free(data);
    }

    lv_obj_delete(keypad);
}
