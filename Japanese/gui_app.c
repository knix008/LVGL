/*
 * GUI Application Module Implementation
 * Handles the Japanese input GUI application interface and event handling
 */

#include "gui_app.h"
#include "lvgl/src/libs/freetype/lv_freetype.h"
#include <stdio.h>
#include <string.h>

// Global application widgets
static AppWidgets app_widgets;
static lv_obj_t *active_mbox = NULL; // Track active message box
static lv_obj_t *flick_window = NULL; // Track active flick window

// Font objects
static lv_font_t *japanese_font_12 = NULL;
static lv_font_t *japanese_font_14 = NULL;
static lv_font_t *japanese_font_16 = NULL;
static lv_font_t *japanese_font_20 = NULL;

// Initialize GUI application
int gui_app_init(void) {
    // Initialize Japanese input state
    japanese_input_init(&app_widgets.state);
    
    // Initialize widgets to NULL
    app_widgets.text_area = NULL;
    for (int i = 0; i < 12; i++) {
        app_widgets.buttons[i] = NULL;
    }
    app_widgets.mode_button = NULL;
    app_widgets.clear_button = NULL;
    app_widgets.shift_button = NULL;
    app_widgets.enter_button = NULL;
    app_widgets.backspace_button = NULL;
    app_widgets.space_button = NULL;
    
    // Initialize shift button state tracking
    app_widgets.previous_shift_state = false;
    app_widgets.shift_disabled = false;
    
    // Initialize global pointers
    active_mbox = NULL;
    flick_window = NULL;
    
    // Load Japanese fonts
    japanese_font_12 = lv_freetype_font_create("assets/NotoSansCJK.ttc", LV_FREETYPE_FONT_RENDER_MODE_BITMAP, 12, LV_FREETYPE_FONT_STYLE_NORMAL);
    japanese_font_14 = lv_freetype_font_create("assets/NotoSansCJK.ttc", LV_FREETYPE_FONT_RENDER_MODE_BITMAP, 14, LV_FREETYPE_FONT_STYLE_NORMAL);
    japanese_font_16 = lv_freetype_font_create("assets/NotoSansCJK.ttc", LV_FREETYPE_FONT_RENDER_MODE_BITMAP, 16, LV_FREETYPE_FONT_STYLE_NORMAL);
    japanese_font_20 = lv_freetype_font_create("assets/NotoSansCJK.ttc", LV_FREETYPE_FONT_RENDER_MODE_BITMAP, 20, LV_FREETYPE_FONT_STYLE_NORMAL);
    
    // Fallback to built-in fonts if TrueType fonts fail
    if (!japanese_font_12) japanese_font_12 = (lv_font_t*)&lv_font_montserrat_14;
    if (!japanese_font_14) japanese_font_14 = (lv_font_t*)&lv_font_montserrat_14;
    if (!japanese_font_16) japanese_font_16 = (lv_font_t*)&lv_font_montserrat_14;
    if (!japanese_font_20) japanese_font_20 = (lv_font_t*)&lv_font_montserrat_14;
    
    return 0;
}

// Cleanup GUI application
void gui_app_cleanup(void) {
    // Clean up fonts
    if (japanese_font_12 && japanese_font_12 != &lv_font_montserrat_14) {
        lv_freetype_font_delete(japanese_font_12);
    }
    if (japanese_font_14 && japanese_font_14 != &lv_font_montserrat_14) {
        lv_freetype_font_delete(japanese_font_14);
    }
    if (japanese_font_16 && japanese_font_16 != &lv_font_montserrat_14) {
        lv_freetype_font_delete(japanese_font_16);
    }
    if (japanese_font_20 && japanese_font_20 != &lv_font_montserrat_14) {
        lv_freetype_font_delete(japanese_font_20);
    }
    
    // Clean up global pointers
    active_mbox = NULL;
    flick_window = NULL;
}

// Font getter functions
const lv_font_t* gui_app_get_japanese_font_12(void) { return (const lv_font_t*)japanese_font_12; }
const lv_font_t* gui_app_get_japanese_font_14(void) { return (const lv_font_t*)japanese_font_14; }
const lv_font_t* gui_app_get_japanese_font_16(void) { return (const lv_font_t*)japanese_font_16; }
const lv_font_t* gui_app_get_japanese_font_20(void) { return (const lv_font_t*)japanese_font_20; }


// Helper function to set button label text and font
static void set_button_label_text(lv_obj_t* label, const wchar_t* wtext, const char* utf8_text) {
    (void)wtext; // Suppress unused parameter warning
    if (!label || !utf8_text) return;
    
    lv_label_set_text(label, utf8_text);
    // Always use Japanese font since we're not using emojis anymore
    lv_obj_set_style_text_font(label, japanese_font_14, 0);
}

// Get global application widgets
AppWidgets* gui_app_get_widgets(void) {
    return &app_widgets;
}

// Flick character selection handler
void gui_app_on_flick_char_clicked(lv_event_t *e) {
    int char_index = (int)(intptr_t)lv_event_get_user_data(e);
    int button_num = app_widgets.state.flick_button;
    
    // Select the character
    japanese_input_select_flick_char(&app_widgets.state, button_num, char_index);
    
    // Update text area
    char *utf8_text = wchar_to_utf8(app_widgets.state.text_buffer, MAX_TEXT_LEN);
    lv_textarea_set_text(app_widgets.text_area, utf8_text);
    
    // Close flick window
    if (flick_window && lv_obj_is_valid(flick_window)) {
        lv_obj_del(flick_window);
        flick_window = NULL;
    }
}

// Create flick input selection window
void gui_app_create_flick_window(int button_num) {
    // Close existing flick window
    if (flick_window && lv_obj_is_valid(flick_window)) {
        lv_obj_del(flick_window);
        flick_window = NULL;
    }
    
    // Get characters for this button (with shift mode consideration)
    const wchar_t* chars = get_button_flick_chars_with_shift(app_widgets.state.now_mode, button_num, app_widgets.state.shift_mode);
    int char_count = get_button_char_count(app_widgets.state.now_mode, button_num);
    
    if (char_count == 0) return;
    
    // Create flick window
    flick_window = lv_obj_create(lv_screen_active());
    lv_obj_set_size(flick_window, 280, 120);
    lv_obj_center(flick_window);
    lv_obj_set_style_bg_opa(flick_window, LV_OPA_90, 0);
    lv_obj_set_style_bg_color(flick_window, lv_color_hex(0x2C2C2C), 0);
    lv_obj_set_style_border_width(flick_window, 2, 0);
    lv_obj_set_style_border_color(flick_window, lv_color_hex(0x4A90E2), 0);
    lv_obj_set_style_radius(flick_window, 15, 0);
    lv_obj_set_style_pad_all(flick_window, 10, 0);
    lv_obj_set_style_shadow_width(flick_window, 20, 0);
    lv_obj_set_style_shadow_color(flick_window, lv_color_black(), 0);
    
    // Create title
    lv_obj_t *title_label = lv_label_create(flick_window);
    lv_label_set_text(title_label, "文字選択 - Character Selection");
    lv_obj_set_style_text_color(title_label, lv_color_hex(0x4A90E2), 0);
    lv_obj_set_style_text_font(title_label, japanese_font_16, 0);
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 5);
    
    // Create character buttons
    lv_obj_t *char_container = lv_obj_create(flick_window);
    lv_obj_set_size(char_container, 240, 70);
    lv_obj_align(char_container, LV_ALIGN_BOTTOM_MID, 0, -10);
    lv_obj_set_style_pad_all(char_container, 5, 0);
    lv_obj_set_style_pad_column(char_container, 5, 0);
    lv_obj_set_layout(char_container, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(char_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(char_container, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(char_container, LV_OPA_0, 0);
    lv_obj_set_style_border_width(char_container, 0, 0);
    
    // Create character buttons
    for (int i = 0; i < char_count && i < 5; i++) {
        lv_obj_t *char_btn = lv_button_create(char_container);
        lv_obj_set_size(char_btn, 40, 40);
        lv_obj_set_style_radius(char_btn, 8, 0);
        
        // Get character (use shift-aware function for all modes)
        wchar_t char_to_show = L'?';
        if (chars && wcslen(chars) > (size_t)i) {
            char_to_show = chars[i];
        }
        
        // Create label for character
        lv_obj_t *char_label = lv_label_create(char_btn);
        wchar_t char_str[2] = {char_to_show, L'\0'};
        char *utf8_char = wchar_to_utf8(char_str, 2);
        lv_label_set_text(char_label, utf8_char);
        lv_obj_set_style_text_font(char_label, japanese_font_16, 0);
        lv_obj_center(char_label);
        
        // Add click event
        lv_obj_add_event_cb(char_btn, gui_app_on_flick_char_clicked, LV_EVENT_CLICKED, (void*)(intptr_t)i);
    }
}

// Button click event handler
void gui_app_on_button_clicked(lv_event_t *e) {
    int button_num = (int)(intptr_t)lv_event_get_user_data(e);
    
    // Process input (now shows flick input)
    japanese_input_process_input(&app_widgets.state, button_num);
    
    // If flick input is now active, show the flick window
    if (app_widgets.state.flick_active) {
        gui_app_create_flick_window(button_num);
    }

    // Update text area
    char *utf8_text = wchar_to_utf8(app_widgets.state.text_buffer, MAX_TEXT_LEN);
    lv_textarea_set_text(app_widgets.text_area, utf8_text);
}

// Mode button click handler
void gui_app_on_mode_button_clicked(lv_event_t *e) {
    (void)e;
    
    // Store current shift state before changing mode (if not already disabled)
    if (!app_widgets.shift_disabled) {
        app_widgets.previous_shift_state = app_widgets.state.shift_mode;
    }
    
    change_input_mode(&app_widgets.state);

    // Update button labels
    for (int i = 0; i < 12; i++) {
        if (app_widgets.buttons[i] == NULL) continue;
        const wchar_t *wtext = get_button_text_with_shift(app_widgets.state.now_mode, i, app_widgets.state.shift_mode);
        char *utf8_text = wchar_to_utf8(wtext, 20);
        lv_obj_t *label = lv_obj_get_child(app_widgets.buttons[i], 0);
        if (label) {
            set_button_label_text(label, wtext, utf8_text);
        }
    }
    
    // Update mode button text to show next mode
    lv_obj_t *mode_label = lv_obj_get_child(app_widgets.mode_button, 0);
    if (mode_label) {
        InputMode next_mode = (app_widgets.state.now_mode + 1) % MODE_COUNT;
        lv_label_set_text(mode_label, get_mode_name(next_mode));
    }
    
    // Handle shift button state based on current mode
    lv_obj_t *shift_label = lv_obj_get_child(app_widgets.shift_button, 0);
    if (shift_label) {
        if (app_widgets.state.now_mode == MODE_NUMBER) {
            // Disable shift button in number mode (numbers don't have shift variants)
            app_widgets.shift_disabled = true;
            lv_obj_add_state(app_widgets.shift_button, LV_STATE_DISABLED);
            lv_obj_set_style_bg_color(app_widgets.shift_button, lv_color_hex(0x808080), 0); // Gray
            lv_obj_set_style_text_color(shift_label, lv_color_hex(0x404040), 0); // Dark gray text
        } else {
            // Enable shift button in other modes
            if (app_widgets.shift_disabled) {
                // Restore previous shift state when re-enabling
                app_widgets.state.shift_mode = app_widgets.previous_shift_state;
                app_widgets.shift_disabled = false;
            }
            lv_obj_clear_state(app_widgets.shift_button, LV_STATE_DISABLED);
            
            // Set color based on current shift state
            bool shift_active = app_widgets.state.shift_mode;
            if (shift_active) {
                // Orange when active/clicked
                lv_obj_set_style_bg_color(app_widgets.shift_button, lv_color_hex(0xFF8000), 0);
                lv_obj_set_style_text_color(shift_label, lv_color_white(), 0);
            } else {
                // Green when inactive/default
                lv_obj_set_style_bg_color(app_widgets.shift_button, lv_color_hex(0x00AA00), 0);
                lv_obj_set_style_text_color(shift_label, lv_color_white(), 0);
            }
        }
    }
}

// Clear button handler
void gui_app_on_clear_clicked(lv_event_t *e) {
    (void)e;
    
    // Defensive: check if text area is valid
    if (!app_widgets.text_area) {
        printf("Error: Text area not initialized\n");
        return;
    }
    
    // Save current mode
    InputMode current_mode = app_widgets.state.now_mode;

    // Clear text (preserve mode)
    japanese_input_init(&app_widgets.state);
    app_widgets.state.now_mode = current_mode;
    lv_textarea_set_text(app_widgets.text_area, "");
    
    // Defensive: reset message box pointer
    if (active_mbox) {
        active_mbox = NULL;
    }
    
    // Close flick window if open
    if (flick_window && lv_obj_is_valid(flick_window)) {
        lv_obj_del(flick_window);
        flick_window = NULL;
    }
}

// Shift button handler
void gui_app_on_shift_clicked(lv_event_t *e) {
    (void)e;
    
    // Don't allow shift button clicks when disabled (in number mode)
    if (app_widgets.shift_disabled) {
        return;
    }
    
    // Close any existing message box
    if (active_mbox && lv_obj_is_valid(active_mbox)) {
        lv_msgbox_close(active_mbox);
        active_mbox = NULL;
    }
    
    // Close flick window if open
    if (flick_window && lv_obj_is_valid(flick_window)) {
        lv_obj_del(flick_window);
        flick_window = NULL;
    }
    
    // Toggle shift mode for all modes (Japanese: Hiragana/Katakana, Alphabet: lowercase/uppercase)
    app_widgets.state.shift_mode = !app_widgets.state.shift_mode;
    
    // Update button labels to reflect mode change
    for (int i = 0; i < 12; i++) {
        if (app_widgets.buttons[i] == NULL) continue;
        const wchar_t *wtext = get_button_text_with_shift(app_widgets.state.now_mode, i, app_widgets.state.shift_mode);
        char *utf8_text = wchar_to_utf8(wtext, 20);
        lv_obj_t *label = lv_obj_get_child(app_widgets.buttons[i], 0);
        if (label) {
            set_button_label_text(label, wtext, utf8_text);
        }
    }
    
    // Update mode button text to show next mode (mode button should not change with shift)
    lv_obj_t *mode_label = lv_obj_get_child(app_widgets.mode_button, 0);
    if (mode_label) {
        InputMode next_mode = (app_widgets.state.now_mode + 1) % MODE_COUNT;
        lv_label_set_text(mode_label, get_mode_name(next_mode));
    }
    
    // Update shift button appearance based on current state
    lv_obj_t *shift_label = lv_obj_get_child(app_widgets.shift_button, 0);
    if (shift_label) {
        // Check if shift button should be active (orange) or inactive (dark green)
        bool shift_active = false;
        
        // In all modes, shift state is preserved and shows the current shift_mode state
        shift_active = app_widgets.state.shift_mode;
        
        if (shift_active) {
            // Orange when active/clicked
            lv_obj_set_style_bg_color(app_widgets.shift_button, lv_color_hex(0xFF8000), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(shift_label, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
        } else {
            // Green when inactive/default
            lv_obj_set_style_bg_color(app_widgets.shift_button, lv_color_hex(0x00AA00), LV_PART_MAIN | LV_STATE_DEFAULT);
            lv_obj_set_style_text_color(shift_label, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);
        }
    }
}

// Enter button handler
void gui_app_on_enter_clicked(lv_event_t *e) {
    (void)e;
    
    // Defensive: check if text area is valid
    if (!app_widgets.text_area) {
        printf("Error: Text area not initialized\n");
        return;
    }
    
    // Close any existing message box
    if (active_mbox && lv_obj_is_valid(active_mbox)) {
        lv_msgbox_close(active_mbox);
        active_mbox = NULL;
    }
    
    // Close flick window if open
    if (flick_window && lv_obj_is_valid(flick_window)) {
        lv_obj_del(flick_window);
        flick_window = NULL;
    }
    
    // Get current text and show result
    char *utf8_text = wchar_to_utf8(app_widgets.state.text_buffer, MAX_TEXT_LEN);
    
    // Clear the text area and input buffer
    lv_textarea_set_text(app_widgets.text_area, "");
    memset(app_widgets.state.text_buffer, 0, sizeof(app_widgets.state.text_buffer));
    app_widgets.state.cursor_pos = 0;
    
    // Create result message box
    active_mbox = lv_msgbox_create(lv_screen_active());
    lv_msgbox_add_title(active_mbox, "入力完了 - Input Complete");
    lv_msgbox_add_text(active_mbox, utf8_text);
    lv_msgbox_add_close_button(active_mbox);
    lv_obj_set_style_text_font(active_mbox, japanese_font_16, 0);
    lv_obj_center(active_mbox);
}

// Backspace button handler
void gui_app_on_backspace_clicked(lv_event_t *e) {
    (void)e;
    
    // Defensive: check if text area is valid
    if (!app_widgets.text_area) {
        printf("Error: Text area not initialized\n");
        return;
    }
    
    // Close flick window if open
    if (flick_window && lv_obj_is_valid(flick_window)) {
        lv_obj_del(flick_window);
        flick_window = NULL;
    }
    
    // Perform backspace operation
    if (app_widgets.state.cursor_pos > 0) {
        // Shift characters left
        for (int i = app_widgets.state.cursor_pos - 1; i < MAX_TEXT_LEN - 1; i++) {
            app_widgets.state.text_buffer[i] = app_widgets.state.text_buffer[i + 1];
        }
        app_widgets.state.cursor_pos--;
        
        // Update text area
        char *utf8_text = wchar_to_utf8(app_widgets.state.text_buffer, MAX_TEXT_LEN);
        lv_textarea_set_text(app_widgets.text_area, utf8_text);
    }
}

// Space button handler
void gui_app_on_space_clicked(lv_event_t *e) {
    (void)e;
    
    // Defensive: check if text area is valid
    if (!app_widgets.text_area) {
        printf("Error: Text area not initialized\n");
        return;
    }
    
    // Close flick window if open
    if (flick_window && lv_obj_is_valid(flick_window)) {
        lv_obj_del(flick_window);
        flick_window = NULL;
    }
    
    // Add space character
    if (app_widgets.state.cursor_pos < MAX_TEXT_LEN - 1) {
        // Shift existing characters to make room
        for (int i = MAX_TEXT_LEN - 1; i > app_widgets.state.cursor_pos; i--) {
            app_widgets.state.text_buffer[i] = app_widgets.state.text_buffer[i - 1];
        }
        
        // Insert space
        app_widgets.state.text_buffer[app_widgets.state.cursor_pos] = L' ';
        app_widgets.state.cursor_pos++;
        
        // Ensure null termination
        if (app_widgets.state.cursor_pos < MAX_TEXT_LEN) {
            app_widgets.state.text_buffer[app_widgets.state.cursor_pos] = L'\0';
        }
        
        // Update text area
        char *utf8_text = wchar_to_utf8(app_widgets.state.text_buffer, MAX_TEXT_LEN);
        lv_textarea_set_text(app_widgets.text_area, utf8_text);
    }
}

// Create the main UI
void gui_app_create_ui(void) {
    // Main container
    lv_obj_t *main_cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(main_cont, 320, 640);
    lv_obj_center(main_cont);
    lv_obj_set_flex_flow(main_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(main_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(main_cont, 10, 0);
    lv_obj_set_style_pad_row(main_cont, 10, 0);

    // Title label
    lv_obj_t *title_label = lv_label_create(main_cont);
    lv_label_set_text(title_label, "日本語入力 - モバイルキーパッド");
    lv_obj_set_style_text_font(title_label, japanese_font_16, 0);

    // Text area (scrollable)
    app_widgets.text_area = lv_textarea_create(main_cont);
    lv_obj_set_size(app_widgets.text_area, 300, 120);
    lv_textarea_set_text(app_widgets.text_area, "");
    lv_obj_set_style_text_font(app_widgets.text_area, japanese_font_16, 0);
    
    // Set font for the textarea's internal label
    lv_obj_t *textarea_label = lv_textarea_get_label(app_widgets.text_area);
    if (textarea_label) {
        lv_obj_set_style_text_font(textarea_label, japanese_font_16, 0);
    }

    // Button container - Mobile phone keypad layout using flex
    lv_obj_t *button_container = lv_obj_create(main_cont);
    lv_obj_set_size(button_container, 240, 420);
    lv_obj_set_style_pad_all(button_container, 5, 0);
    lv_obj_set_style_pad_row(button_container, 3, 0);
    lv_obj_set_style_pad_column(button_container, 3, 0);
    lv_obj_set_layout(button_container, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(button_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(button_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // Create rows for mobile phone keypad layout
    // Row 0: 1(あ), 2(か), 3(さ)
    lv_obj_t *row0 = lv_obj_create(button_container);
    lv_obj_set_size(row0, 220, 55);
    lv_obj_set_style_pad_all(row0, 2, 0);
    lv_obj_set_style_pad_column(row0, 3, 0);
    lv_obj_set_layout(row0, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(row0, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row0, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(row0, LV_OPA_0, 0);
    lv_obj_set_style_border_width(row0, 0, 0);

    for (int i = 0; i < 3; i++) {
        const wchar_t *wtext = get_button_text(app_widgets.state.now_mode, i);
        char *utf8_text = wchar_to_utf8(wtext, 20);
        
        app_widgets.buttons[i] = lv_button_create(row0);
        lv_obj_set_size(app_widgets.buttons[i], 70, 50);
        lv_obj_set_style_radius(app_widgets.buttons[i], 10, 0);

        lv_obj_t *label = lv_label_create(app_widgets.buttons[i]);
        set_button_label_text(label, wtext, utf8_text);
        lv_obj_center(label);

        lv_obj_add_event_cb(app_widgets.buttons[i], gui_app_on_button_clicked, LV_EVENT_CLICKED, (void*)(intptr_t)i);
    }

    // Row 1: 4(た), 5(な), 6(は)
    lv_obj_t *row1 = lv_obj_create(button_container);
    lv_obj_set_size(row1, 220, 55);
    lv_obj_set_style_pad_all(row1, 2, 0);
    lv_obj_set_style_pad_column(row1, 3, 0);
    lv_obj_set_layout(row1, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(row1, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row1, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(row1, LV_OPA_0, 0);
    lv_obj_set_style_border_width(row1, 0, 0);

    for (int i = 3; i < 6; i++) {
        const wchar_t *wtext = get_button_text(app_widgets.state.now_mode, i);
        char *utf8_text = wchar_to_utf8(wtext, 20);
        
        app_widgets.buttons[i] = lv_button_create(row1);
        lv_obj_set_size(app_widgets.buttons[i], 70, 50);
        lv_obj_set_style_radius(app_widgets.buttons[i], 10, 0);

        lv_obj_t *label = lv_label_create(app_widgets.buttons[i]);
        set_button_label_text(label, wtext, utf8_text);
        lv_obj_center(label);

        lv_obj_add_event_cb(app_widgets.buttons[i], gui_app_on_button_clicked, LV_EVENT_CLICKED, (void*)(intptr_t)i);
    }

    // Row 2: 7(ま), 8(や), 9(ら)
    lv_obj_t *row2 = lv_obj_create(button_container);
    lv_obj_set_size(row2, 220, 55);
    lv_obj_set_style_pad_all(row2, 2, 0);
    lv_obj_set_style_pad_column(row2, 3, 0);
    lv_obj_set_layout(row2, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(row2, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row2, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(row2, LV_OPA_0, 0);
    lv_obj_set_style_border_width(row2, 0, 0);

    for (int i = 6; i < 9; i++) {
        const wchar_t *wtext = get_button_text(app_widgets.state.now_mode, i);
        char *utf8_text = wchar_to_utf8(wtext, 20);
        
        app_widgets.buttons[i] = lv_button_create(row2);
        lv_obj_set_size(app_widgets.buttons[i], 70, 50);
        lv_obj_set_style_radius(app_widgets.buttons[i], 10, 0);

        lv_obj_t *label = lv_label_create(app_widgets.buttons[i]);
        set_button_label_text(label, wtext, utf8_text);
        lv_obj_center(label);

        lv_obj_add_event_cb(app_widgets.buttons[i], gui_app_on_button_clicked, LV_EVENT_CLICKED, (void*)(intptr_t)i);
    }

    // Row 3: *(゛), 0(わ), #(ー)
    lv_obj_t *row3 = lv_obj_create(button_container);
    lv_obj_set_size(row3, 220, 55);
    lv_obj_set_style_pad_all(row3, 2, 0);
    lv_obj_set_style_pad_column(row3, 3, 0);
    lv_obj_set_layout(row3, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(row3, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row3, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(row3, LV_OPA_0, 0);
    lv_obj_set_style_border_width(row3, 0, 0);

    for (int i = 9; i < 12; i++) {
        const wchar_t *wtext = get_button_text(app_widgets.state.now_mode, i);
        char *utf8_text = wchar_to_utf8(wtext, 20);
        
        app_widgets.buttons[i] = lv_button_create(row3);
        lv_obj_set_size(app_widgets.buttons[i], 70, 50);
        lv_obj_set_style_radius(app_widgets.buttons[i], 10, 0);

        lv_obj_t *label = lv_label_create(app_widgets.buttons[i]);
        set_button_label_text(label, wtext, utf8_text);
        lv_obj_center(label);

        lv_obj_add_event_cb(app_widgets.buttons[i], gui_app_on_button_clicked, LV_EVENT_CLICKED, (void*)(intptr_t)i);
    }

    // Row 4: Mode, Space, Backspace buttons
    lv_obj_t *row4 = lv_obj_create(button_container);
    lv_obj_set_size(row4, 220, 55);
    lv_obj_set_style_pad_all(row4, 2, 0);
    lv_obj_set_style_pad_column(row4, 3, 0);
    lv_obj_set_layout(row4, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(row4, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row4, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(row4, LV_OPA_0, 0);
    lv_obj_set_style_border_width(row4, 0, 0);

    app_widgets.shift_button = lv_button_create(row4);
    lv_obj_set_size(app_widgets.shift_button, 70, 50);
    lv_obj_set_style_radius(app_widgets.shift_button, 10, 0);
    lv_obj_t *shift_label = lv_label_create(app_widgets.shift_button);
    lv_label_set_text(shift_label, "Shift");
    lv_obj_set_style_text_font(shift_label, japanese_font_14, 0);
    lv_obj_center(shift_label);
    lv_obj_add_event_cb(app_widgets.shift_button, gui_app_on_shift_clicked, LV_EVENT_CLICKED, NULL);
    
    // Set initial shift button color (green - inactive by default)
    lv_obj_set_style_bg_color(app_widgets.shift_button, lv_color_hex(0x00AA00), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(shift_label, lv_color_white(), LV_PART_MAIN | LV_STATE_DEFAULT);

    app_widgets.mode_button = lv_button_create(row4);
    lv_obj_set_size(app_widgets.mode_button, 70, 50);
    lv_obj_set_style_radius(app_widgets.mode_button, 10, 0);
    lv_obj_t *mode_label = lv_label_create(app_widgets.mode_button);
    InputMode next_mode = (app_widgets.state.now_mode + 1) % MODE_COUNT;
    lv_label_set_text(mode_label, get_mode_name(next_mode));
    lv_obj_set_style_text_font(mode_label, japanese_font_14, 0);
    lv_obj_center(mode_label);
    lv_obj_add_event_cb(app_widgets.mode_button, gui_app_on_mode_button_clicked, LV_EVENT_CLICKED, NULL);

    app_widgets.backspace_button = lv_button_create(row4);
    lv_obj_set_size(app_widgets.backspace_button, 70, 50);
    lv_obj_set_style_radius(app_widgets.backspace_button, 10, 0);
    lv_obj_t *backspace_label = lv_label_create(app_widgets.backspace_button);
    lv_label_set_text(backspace_label, "←");
    lv_obj_set_style_text_font(backspace_label, japanese_font_14, 0);
    lv_obj_center(backspace_label);
    lv_obj_add_event_cb(app_widgets.backspace_button, gui_app_on_backspace_clicked, LV_EVENT_CLICKED, NULL);

    // Row 5: Clear and Enter buttons
    lv_obj_t *row5 = lv_obj_create(button_container);
    lv_obj_set_size(row5, 220, 55);
    lv_obj_set_style_pad_all(row5, 2, 0);
    lv_obj_set_style_pad_column(row5, 3, 0);
    lv_obj_set_layout(row5, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(row5, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row5, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(row5, LV_OPA_0, 0);
    lv_obj_set_style_border_width(row5, 0, 0);

    app_widgets.clear_button = lv_button_create(row5);
    lv_obj_set_size(app_widgets.clear_button, 70, 50);
    lv_obj_set_style_radius(app_widgets.clear_button, 10, 0);
    lv_obj_t *clear_label = lv_label_create(app_widgets.clear_button);
    lv_label_set_text(clear_label, "Clear");
    lv_obj_set_style_text_font(clear_label, japanese_font_14, 0);
    lv_obj_center(clear_label);
    lv_obj_add_event_cb(app_widgets.clear_button, gui_app_on_clear_clicked, LV_EVENT_CLICKED, NULL);

    app_widgets.space_button = lv_button_create(row5);
    lv_obj_set_size(app_widgets.space_button, 70, 50);
    lv_obj_set_style_radius(app_widgets.space_button, 10, 0);
    lv_obj_t *space_label = lv_label_create(app_widgets.space_button);
    lv_label_set_text(space_label, "Space");
    lv_obj_set_style_text_font(space_label, japanese_font_14, 0);
    lv_obj_center(space_label);
    lv_obj_add_event_cb(app_widgets.space_button, gui_app_on_space_clicked, LV_EVENT_CLICKED, NULL);

    app_widgets.enter_button = lv_button_create(row5);
    lv_obj_set_size(app_widgets.enter_button, 70, 50);
    lv_obj_set_style_radius(app_widgets.enter_button, 10, 0);
    lv_obj_t *enter_label = lv_label_create(app_widgets.enter_button);
    lv_label_set_text(enter_label, "Enter");
    lv_obj_set_style_text_font(enter_label, japanese_font_14, 0);
    lv_obj_center(enter_label);
    lv_obj_add_event_cb(app_widgets.enter_button, gui_app_on_enter_clicked, LV_EVENT_CLICKED, NULL);

    // Info label
    lv_obj_t *info_label = lv_label_create(main_cont);
    lv_label_set_text(info_label, "日本語/アルファベット/数字/記号入力");
    lv_obj_set_style_text_font(info_label, japanese_font_12, 0);
}
