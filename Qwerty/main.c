#include "lvgl/lvgl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "qwerty.h"

// Application state
typedef struct {
    lv_obj_t *screen;
    lv_obj_t *text_area;
    lv_obj_t *status_label;
    lv_obj_t *shift_buttons[2];  // Left and right shift
    lv_obj_t *caps_button;
    lv_obj_t *lang_button;
    lv_obj_t *clear_button;
    lv_obj_t *enter_button;
    QwertyState qwerty;
    lv_font_t *korean_font_14;  // FreeType font for status
    lv_font_t *korean_font_20;  // FreeType font for text area
    lv_font_t *korean_font_16;  // FreeType font for buttons
    lv_font_t *korean_font_small_20;  // Smaller font for ASCII symbols with better visibility
} AppState;

static AppState app_state = {NULL, NULL, NULL, {NULL, NULL}, NULL, NULL, NULL, NULL, {LANG_ENGLISH, 0, 0, {0, 0, 0, 0}}, NULL, NULL, NULL, NULL};

// Global storage for key buttons to update labels
static lv_obj_t *key_buttons[50];
static KeyMap *key_button_maps[50];
static int num_key_buttons = 0;

// Forward declarations
static void update_status(void);
static void update_button_labels(void);

// Display buffer
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

// Update status label
static void update_status(void) {
    char status_text[128];
    snprintf(status_text, sizeof(status_text),
        "Mode: %s | Shift: %s | Caps: %s",
        app_state.qwerty.current_language == LANG_ENGLISH ? "ENG" : "한국어",
        app_state.qwerty.shift_pressed ? "ON" : "OFF",
        app_state.qwerty.caps_lock ? "ON" : "OFF"
    );
    lv_label_set_text(app_state.status_label, status_text);
}

// Delete last character (handles multi-byte UTF-8 properly)
static void delete_last_char(void) {
    const char *current_text = lv_textarea_get_text(app_state.text_area);
    size_t len = strlen(current_text);
    
    if (len == 0) return;
    
    // Find the start of the last UTF-8 character
    size_t i = len - 1;
    // Skip continuation bytes (10xxxxxx)
    while (i > 0 && (current_text[i] & 0xC0) == 0x80) {
        i--;
    }
    
    // Create a new string without the last character
    char *new_text = malloc(i + 1);
    if (new_text) {
        memcpy(new_text, current_text, i);
        new_text[i] = '\0';
        lv_textarea_set_text(app_state.text_area, new_text);
        free(new_text);
        
        // Move cursor to end
        lv_textarea_set_cursor_pos(app_state.text_area, LV_TEXTAREA_CURSOR_LAST);
    }
}

// Insert text at cursor position
static void insert_text(const char *text) {
    lv_textarea_add_text(app_state.text_area, text);
}

// Button click callback
static void on_key_clicked(lv_event_t *e) {
    KeyMap *key_map = (KeyMap *)lv_event_get_user_data(e);
    const char *text = qwerty_get_key_char(&app_state.qwerty, key_map);

    if (app_state.qwerty.current_language == LANG_KOREAN) {
        char output[21] = {0};  // Enough for 2 Korean syllables
        int delete_prev = 0;
        qwerty_process_korean_char(&app_state.qwerty, text, output, &delete_prev);

        if (delete_prev) {
            delete_last_char();
        }
        insert_text(output);
    } else {
        insert_text(text);
        qwerty_reset_composition(&app_state.qwerty);
    }
}

// Backspace callback
static void on_backspace_clicked(lv_event_t *e) {
    (void)e;
    delete_last_char();
    qwerty_reset_composition(&app_state.qwerty);
}

// Space callback
static void on_space_clicked(lv_event_t *e) {
    (void)e;
    insert_text(" ");
    qwerty_reset_composition(&app_state.qwerty);
}

// Enter callback - shows popup with text and clears
static void on_enter_clicked(lv_event_t *e) {
    (void)e;
    
    // Get current text
    const char *text = lv_textarea_get_text(app_state.text_area);
    
    // Create a message box to show the result
    lv_obj_t *msgbox = lv_msgbox_create(lv_screen_active());
    lv_msgbox_add_title(msgbox, "Input Result");
    
    // Add the text content
    if (text && strlen(text) > 0) {
        lv_msgbox_add_text(msgbox, text);
    } else {
        lv_msgbox_add_text(msgbox, "(Empty)");
    }
    
    // Apply Korean font to the message box text if available
    if (app_state.korean_font_20) {
        lv_obj_t *content = lv_msgbox_get_content(msgbox);
        if (content) {
            // Find the label in the content
            uint32_t child_count = lv_obj_get_child_count(content);
            for (uint32_t i = 0; i < child_count; i++) {
                lv_obj_t *child = lv_obj_get_child(content, i);
                if (lv_obj_check_type(child, &lv_label_class)) {
                    lv_obj_set_style_text_font(child, app_state.korean_font_20, 0);
                }
            }
        }
    }
    
    // Add close button
    lv_msgbox_add_close_button(msgbox);
    
    // Center the message box
    lv_obj_center(msgbox);
    
    // Clear the text area
    lv_textarea_set_text(app_state.text_area, "");
    qwerty_reset_composition(&app_state.qwerty);
}

// Tab callback
static void on_tab_clicked(lv_event_t *e) {
    (void)e;
    insert_text("    ");  // Insert 4 spaces instead of tab character
    qwerty_reset_composition(&app_state.qwerty);
}

// Shift toggle callback
static void on_shift_clicked(lv_event_t *e) {
    (void)e;
    app_state.qwerty.shift_pressed = !app_state.qwerty.shift_pressed;
    update_status();
    update_button_labels();
}

// Caps lock callback
static void on_caps_clicked(lv_event_t *e) {
    (void)e;
    app_state.qwerty.caps_lock = !app_state.qwerty.caps_lock;
    update_status();
    update_button_labels();
}

// Language switch callback
static void on_lang_clicked(lv_event_t *e) {
    (void)e;
    app_state.qwerty.current_language = (app_state.qwerty.current_language == LANG_ENGLISH)
                                  ? LANG_KOREAN : LANG_ENGLISH;
    qwerty_reset_composition(&app_state.qwerty);
    update_status();
    update_button_labels();
}

// Clear text callback
static void on_clear_clicked(lv_event_t *e) {
    (void)e;
    lv_textarea_set_text(app_state.text_area, "");
    qwerty_reset_composition(&app_state.qwerty);
}

// Create a keyboard button
static lv_obj_t* create_key_button(lv_obj_t *parent, const char *label, 
                                    lv_event_cb_t callback, void *user_data, int width) {
    lv_obj_t *btn = lv_button_create(parent);
    lv_obj_set_size(btn, width, 39);  // Reduced from 42 to 39
    
    lv_obj_t *label_obj = lv_label_create(btn);
    lv_label_set_text(label_obj, label);
    lv_obj_center(label_obj);
    
    // Check if needs larger font for backtick/tilde visibility
    int needs_larger = (strcmp(label, "`") == 0 || strcmp(label, "~") == 0);
    
    // Use font from assets - larger size for backtick/tilde
    if (needs_larger && app_state.korean_font_small_20) {
        lv_obj_set_style_text_font(label_obj, app_state.korean_font_small_20, 0);
    } else {
        lv_obj_set_style_text_font(label_obj, app_state.korean_font_16, 0);
    }
    
    // Ensure text is visible with explicit black color
    lv_obj_set_style_text_color(label_obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    
    // Force label to use single line and align center
    lv_label_set_long_mode(label_obj, LV_LABEL_LONG_CLIP);
    lv_obj_set_style_text_align(label_obj, LV_TEXT_ALIGN_CENTER, 0);
    
    // Force refresh
    lv_obj_invalidate(label_obj);
    
    if (callback) {
        lv_obj_add_event_cb(btn, callback, LV_EVENT_CLICKED, user_data);
    }
    
    return btn;
}

// Update all button labels based on current state
static void update_button_labels(void) {
    for (int i = 0; i < num_key_buttons; i++) {
        const char *label = qwerty_get_key_char(&app_state.qwerty, key_button_maps[i]);
        lv_obj_t *label_obj = lv_obj_get_child(key_buttons[i], 0);
        if (label_obj) {
            lv_label_set_text(label_obj, label);
            
            // Check if needs larger font for visibility
            int needs_larger = (strcmp(label, "`") == 0 || strcmp(label, "~") == 0);
            
            // Ensure style is reapplied after text change
            if (needs_larger && app_state.korean_font_small_20) {
                lv_obj_set_style_text_font(label_obj, app_state.korean_font_small_20, 0);
            } else {
                lv_obj_set_style_text_font(label_obj, app_state.korean_font_16, 0);
            }
            lv_obj_set_style_text_color(label_obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_invalidate(label_obj);
        }
    }

    // Update shift button appearance
    for (int i = 0; i < 2; i++) {
        if (app_state.shift_buttons[i]) {
            if (app_state.qwerty.shift_pressed) {
                lv_obj_set_style_bg_color(app_state.shift_buttons[i], 
                                          lv_palette_main(LV_PALETTE_GREEN), 0);
            } else {
                lv_obj_set_style_bg_color(app_state.shift_buttons[i], 
                                          lv_palette_main(LV_PALETTE_GREY), 0);
            }
        }
    }

    // Update caps button appearance
    if (app_state.caps_button) {
        if (app_state.qwerty.caps_lock) {
            lv_obj_set_style_bg_color(app_state.caps_button, 
                                      lv_palette_main(LV_PALETTE_GREEN), 0);
        } else {
            lv_obj_set_style_bg_color(app_state.caps_button, 
                                      lv_palette_main(LV_PALETTE_GREY), 0);
        }
    }

    // Update language button appearance and text
    if (app_state.lang_button) {
        // Set text based on current language (show opposite language)
        lv_obj_t *label_obj = lv_obj_get_child(app_state.lang_button, 0);
        if (label_obj) {
            if (app_state.qwerty.current_language == LANG_ENGLISH) {
                lv_label_set_text(label_obj, "한글");
            } else {
                lv_label_set_text(label_obj, "ENG");
            }
            lv_obj_set_style_text_font(label_obj, app_state.korean_font_16, 0);
            lv_obj_set_style_text_color(label_obj, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_invalidate(label_obj);
        }
        
        // Always set orange color
        lv_obj_set_style_bg_color(app_state.lang_button, 
                                  lv_color_hex(0xFF8C00), 0);  // Orange color
    }
}

// Create the GUI
static void create_gui(void) {
    // Initialize qwerty state
    qwerty_init(&app_state.qwerty);

    // Create main screen
    app_state.screen = lv_screen_active();
    lv_obj_set_style_bg_color(app_state.screen, lv_color_hex(0xF0F0F0), 0);

    // Main container
    lv_obj_t *main_cont = lv_obj_create(app_state.screen);
    lv_obj_set_size(main_cont, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20);
    lv_obj_center(main_cont);
    lv_obj_set_flex_flow(main_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(main_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(main_cont, 10, 0);
    lv_obj_set_style_pad_gap(main_cont, 5, 0);

    // Status label - use font from assets
    app_state.status_label = lv_label_create(main_cont);
    lv_obj_set_style_text_font(app_state.status_label, app_state.korean_font_14, 0);
    update_status();

    // Text area - use font from assets
    app_state.text_area = lv_textarea_create(main_cont);
    lv_obj_set_size(app_state.text_area, SCREEN_WIDTH - 40, 100);
    lv_textarea_set_placeholder_text(app_state.text_area, "Type here...");
    lv_obj_set_style_text_font(app_state.text_area, app_state.korean_font_20, 0);
    // Ensure text is visible
    lv_obj_set_style_text_color(app_state.text_area, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(app_state.text_area, lv_color_hex(0x000000), LV_PART_TEXTAREA_PLACEHOLDER);

    // Keyboard container
    lv_obj_t *keyboard_cont = lv_obj_create(main_cont);
    lv_obj_set_size(keyboard_cont, SCREEN_WIDTH - 40, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(keyboard_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(keyboard_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(keyboard_cont, 5, 0);
    lv_obj_set_style_pad_gap(keyboard_cont, 2, 0);

    // Row 0: Numbers and symbols
    lv_obj_t *row = lv_obj_create(keyboard_cont);
    lv_obj_set_size(row, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(row, 0, 0);
    lv_obj_set_style_pad_gap(row, 2, 0);
    lv_obj_set_style_border_width(row, 0, 0);

    for (int i = 0; i < 13; i++) {
        key_buttons[num_key_buttons] = create_key_button(
            row,
            qwerty_get_key_char(&app_state.qwerty, &key_maps[i]),
            on_key_clicked,
            &key_maps[i],
            35  // Reduced from 38
        );
        key_button_maps[num_key_buttons] = &key_maps[i];
        num_key_buttons++;
    }
    create_key_button(row, "←", on_backspace_clicked, NULL, 73);  // Reduced from 76

    // Row 1: QWERTY
    row = lv_obj_create(keyboard_cont);
    lv_obj_set_size(row, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(row, 0, 0);
    lv_obj_set_style_pad_gap(row, 2, 0);
    lv_obj_set_style_border_width(row, 0, 0);

    create_key_button(row, "Tab", on_tab_clicked, NULL, 55);  // Reduced from 58

    for (int i = 13; i < 26; i++) {
        key_buttons[num_key_buttons] = create_key_button(
            row,
            qwerty_get_key_char(&app_state.qwerty, &key_maps[i]),
            on_key_clicked,
            &key_maps[i],
            35  // Reduced from 38
        );
        key_button_maps[num_key_buttons] = &key_maps[i];
        num_key_buttons++;
    }

    // Row 2: ASDF
    row = lv_obj_create(keyboard_cont);
    lv_obj_set_size(row, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(row, 0, 0);
    lv_obj_set_style_pad_gap(row, 2, 0);
    lv_obj_set_style_border_width(row, 0, 0);

    app_state.caps_button = create_key_button(row, "Caps", on_caps_clicked, NULL, 67);  // Reduced from 70

    for (int i = 26; i < 37; i++) {
        key_buttons[num_key_buttons] = create_key_button(
            row,
            qwerty_get_key_char(&app_state.qwerty, &key_maps[i]),
            on_key_clicked,
            &key_maps[i],
            35  // Reduced from 38
        );
        key_button_maps[num_key_buttons] = &key_maps[i];
        num_key_buttons++;
    }

    // Create Enter button and set it to blue color
    app_state.enter_button = create_key_button(row, "Enter", on_enter_clicked, NULL, 61);  // Reduced from 64
    lv_obj_set_style_bg_color(app_state.enter_button, lv_color_hex(0x0000FF), 0);  // Blue color
    
    // Set Enter button label color to white for better contrast
    lv_obj_t *enter_label = lv_obj_get_child(app_state.enter_button, 0);
    if (enter_label) {
        lv_obj_set_style_text_color(enter_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    }

    // Row 3: ZXCV
    row = lv_obj_create(keyboard_cont);
    lv_obj_set_size(row, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(row, 0, 0);
    lv_obj_set_style_pad_gap(row, 2, 0);
    lv_obj_set_style_border_width(row, 0, 0);

    app_state.shift_buttons[0] = create_key_button(row, "Shift", on_shift_clicked, NULL, 81);  // Reduced from 84

    for (int i = 37; i < 47; i++) {
        key_buttons[num_key_buttons] = create_key_button(
            row,
            qwerty_get_key_char(&app_state.qwerty, &key_maps[i]),
            on_key_clicked,
            &key_maps[i],
            35  // Reduced from 38
        );
        key_button_maps[num_key_buttons] = &key_maps[i];
        num_key_buttons++;
    }

    app_state.shift_buttons[1] = create_key_button(row, "Shift", on_shift_clicked, NULL, 81);  // Reduced from 84

    // Row 4: Space bar and controls
    row = lv_obj_create(keyboard_cont);
    lv_obj_set_size(row, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(row, 0, 0);
    lv_obj_set_style_pad_gap(row, 2, 0);
    lv_obj_set_style_border_width(row, 0, 0);

    app_state.lang_button = create_key_button(row, "한글", on_lang_clicked, NULL, 58);  // Reduced from 61
    // Set language button to orange color
    lv_obj_set_style_bg_color(app_state.lang_button, lv_color_hex(0xFF8C00), 0);  // Orange color
    
    create_key_button(row, "Space", on_space_clicked, NULL, 343);  // Reduced from 346
    
    // Create Clear button and set it to orange color
    app_state.clear_button = create_key_button(row, "Clear", on_clear_clicked, NULL, 58);  // Reduced from 61
    lv_obj_set_style_bg_color(app_state.clear_button, lv_color_hex(0xFF8C00), 0);  // Orange color

    // Initial button state update
    update_button_labels();
}

// Initialize FreeType fonts
static int init_fonts(void) {
    // Initialize FreeType fonts with different sizes from assets directory
    // Using NanumGothicCoding.ttf as the default font which includes ASCII and Korean characters
    
    // Font for status label (12px)
    app_state.korean_font_14 = lv_freetype_font_create("assets/NanumGothicCoding.ttf", 
                                                        LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                                        12, 
                                                        LV_FREETYPE_FONT_STYLE_NORMAL);
    if (!app_state.korean_font_14) {
        fprintf(stderr, "Error: Failed to load NanumGothicCoding font from assets (12px)\n");
        return -1;
    }
    
    // Font for text area (16px) - use coding font for better readability
    app_state.korean_font_20 = lv_freetype_font_create("assets/NanumGothicCoding.ttf",
                                                        LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                                        16,
                                                        LV_FREETYPE_FONT_STYLE_NORMAL);
    if (!app_state.korean_font_20) {
        fprintf(stderr, "Error: Failed to load NanumGothicCoding font from assets (16px)\n");
        return -1;
    }
    
    // Font for keyboard buttons (16px) - coding font for consistent appearance
    app_state.korean_font_16 = lv_freetype_font_create("assets/NanumGothicCoding.ttf",
                                                        LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                                        16,
                                                        LV_FREETYPE_FONT_STYLE_NORMAL);
    if (!app_state.korean_font_16) {
        fprintf(stderr, "Error: Failed to load NanumGothicCoding font from assets (16px)\n");
        return -1;
    }
    
    // Larger coding font for special characters like backtick
    app_state.korean_font_small_20 = lv_freetype_font_create("assets/NanumGothicCoding.ttf",
                                                              LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                                              20,
                                                              LV_FREETYPE_FONT_STYLE_NORMAL);
    if (!app_state.korean_font_small_20) {
        fprintf(stderr, "Warning: Failed to load NanumGothicCoding 20px font for special characters\n");
    }
    
    return 0;
}

// Initialize LVGL with SDL
static int init_lvgl(void) {
    // Initialize LVGL
    lv_init();

    // Create SDL window and display with LVGL v9 built-in SDL support
    // This automatically initializes SDL, creates window, and sets up rendering
    lv_display_t *display = lv_sdl_window_create(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!display) {
        fprintf(stderr, "Error: Failed to create SDL window\n");
        return -1;
    }

    // Create mouse input device (LVGL v9 SDL handles this internally)
    lv_indev_t *mouse = lv_sdl_mouse_create();
    if (!mouse) {
        fprintf(stderr, "Warning: Failed to create mouse input device\n");
    }

    // Create keyboard input device (optional, for physical keyboard support)
    lv_indev_t *keyboard = lv_sdl_keyboard_create();
    if (!keyboard) {
        fprintf(stderr, "Warning: Failed to create keyboard input device\n");
    }
    
    // Initialize FreeType fonts
    if (init_fonts() != 0) {
        fprintf(stderr, "Warning: Font initialization had issues, but continuing...\n");
    }

    return 0;
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    
    setlocale(LC_ALL, "");

    // Initialize LVGL with SDL
    if (init_lvgl() != 0) {
        return 1;
    }

    // Create GUI
    create_gui();

    // Main loop - LVGL v9 with SDL handles the event loop automatically
    // Just need to keep calling lv_timer_handler periodically
    while (1) {
        uint32_t time_till_next = lv_timer_handler();
        lv_delay_ms(time_till_next);
    }

    return 0;
}
