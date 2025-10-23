/**
 * @file password_gui.c
 * @brief Password login GUI implementation (LVGL-dependent)
 *
 * This module implements the graphical user interface for the
 * password login system using LVGL.
 */

#include "password_gui.h"
#include "password.h"
#include "keypad.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

// UI object references
static lv_obj_t *userid_textarea;
static lv_obj_t *password_textarea;
static lv_obj_t *login_btn;
static lv_obj_t *keypad_obj;
static lv_obj_t *active_textarea = NULL;

// Font references (loaded at runtime)
static lv_font_t *font_normal = NULL;
static lv_font_t *font_bold = NULL;

// Login callback
static login_callback_t login_callback = NULL;

// Display reference
static lv_display_t *display = NULL;

/**
 * @brief Updates User ID validation (no-op, login button always enabled)
 */
static void update_userid_validation_message(const char *userid) {
    // Login button is always enabled, no validation needed for UI state
    (void)userid;
}

/**
 * @brief Updates password validation (no-op, login button always enabled)
 */
static void update_validation_message(const char *password) {
    // Login button is always enabled, no validation needed for UI state
    (void)password;
}

/**
 * @brief Event handler for User ID textarea changes
 */
static void userid_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *ta = (lv_obj_t *)lv_event_get_target(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        const char *txt = lv_textarea_get_text(ta);
        update_userid_validation_message(txt);
    } else if (code == LV_EVENT_FOCUSED) {
        // Set this textarea as active target for keypad
        active_textarea = ta;
        if (keypad_obj) {
            keypad_set_target(keypad_obj, active_textarea);
        }
    }
}

/**
 * @brief Event handler for password textarea changes
 */
static void password_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t *ta = (lv_obj_t *)lv_event_get_target(e);

    if (code == LV_EVENT_VALUE_CHANGED) {
        const char *txt = lv_textarea_get_text(ta);
        update_validation_message(txt);
    } else if (code == LV_EVENT_FOCUSED) {
        // Set this textarea as active target for keypad
        active_textarea = ta;
        if (keypad_obj) {
            keypad_set_target(keypad_obj, active_textarea);
        }
    }
}

/**
 * @brief Shows a popup message box
 */
static void show_popup_message(const char *title, const char *message, bool is_error) {
    lv_obj_t *msgbox = lv_msgbox_create(NULL);
    
    // Set Korean font first before adding content
    if (font_normal) {
        lv_obj_set_style_text_font(msgbox, font_normal, 0);
    }
    
    lv_msgbox_add_title(msgbox, title);
    lv_msgbox_add_text(msgbox, message);
    lv_msgbox_add_close_button(msgbox);
    
    // Set popup colors based on success/error
    lv_obj_t *header = lv_msgbox_get_header(msgbox);
    if (is_error) {
        lv_obj_set_style_bg_color(header, lv_color_hex(0xFF0000), 0);
    } else {
        lv_obj_set_style_bg_color(header, lv_color_hex(0x4CAF50), 0);
    }
}

/**
 * @brief Event handler for login button
 */
static void login_btn_event_handler(lv_event_t *e) {
    lv_event_code_t code = lv_event_get_code(e);

    if (code == LV_EVENT_CLICKED) {
        const char *userid = lv_textarea_get_text(userid_textarea);
        const char *password = lv_textarea_get_text(password_textarea);

        // Validate User ID
        if (strlen(userid) == 0) {
            show_popup_message("Error", "Please enter User ID", true);
            return;
        }

        if (!userid_validate(userid)) {
            show_popup_message("Error", "Invalid User ID format", true);
            return;
        }

        // Validate Password
        if (!password_validate(password)) {
            show_popup_message("Error", "Password does not meet requirements", true);
            return;
        }

        // Both valid - proceed with login
        char msg[256];
        snprintf(msg, sizeof(msg), "Login successful!\nUser ID: %s", userid);
        show_popup_message("Success", msg, false);

        // Call the login callback if set
        if (login_callback) {
            login_callback(userid, password);
        }
    }
}

/**
 * @brief Loads TrueType fonts at runtime using FreeType
 */
static bool load_fonts(void) {
    // Load normal font
    font_normal = lv_freetype_font_create("assets/NanumGothicCoding.ttf",
                                          LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                          16,
                                          LV_FREETYPE_FONT_STYLE_NORMAL);
    if (!font_normal) {
        printf("Warning: Failed to load normal font. Using default font.\n");
    }

    // Load bold font
    font_bold = lv_freetype_font_create("assets/NanumGothicCoding-Bold.ttf",
                                        LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                        16,
                                        LV_FREETYPE_FONT_STYLE_NORMAL);
    if (!font_bold) {
        printf("Warning: Failed to load bold font. Using default font.\n");
    }

    return (font_normal != NULL || font_bold != NULL);
}

/**
 * @brief Initializes the GUI system
 */
bool gui_init(void) {
    // Initialize LVGL
    lv_init();

    // Create display with SDL driver
    display = lv_sdl_window_create(WINDOW_WIDTH, WINDOW_HEIGHT);
    if (!display) {
        fprintf(stderr, "Failed to create SDL window\n");
        return false;
    }

    // Create input device (mouse)
    lv_indev_t *mouse = lv_sdl_mouse_create();
    if (!mouse) {
        fprintf(stderr, "Failed to create mouse input device\n");
        return false;
    }

    // Create keyboard input device
    lv_indev_t *keyboard = lv_sdl_keyboard_create();
    if (!keyboard) {
        fprintf(stderr, "Failed to create keyboard input device\n");
        return false;
    }

    // Load fonts
    load_fonts();

    return true;
}

/**
 * @brief Creates the login user interface with mobile-style keypad
 */
void gui_create_login_screen(void) {
    lv_obj_t *screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_hex(0xF0F0F0), 0);

    // Create top section for input fields
    lv_obj_t *top_cont = lv_obj_create(screen);
    lv_obj_set_size(top_cont, WINDOW_WIDTH, 260);
    lv_obj_set_pos(top_cont, 0, 0);
    lv_obj_set_style_pad_all(top_cont, 10, 0);
    lv_obj_set_style_pad_row(top_cont, 5, 0);
    lv_obj_clear_flag(top_cont, LV_OBJ_FLAG_SCROLLABLE);

    // Title
    lv_obj_t *title = lv_label_create(top_cont);
    lv_label_set_text(title, "로그인");  // "Login" in Korean
    if (font_bold) {
        lv_obj_set_style_text_font(title, font_bold, 0);
    }
    lv_obj_set_style_text_color(title, lv_color_hex(0x333333), 0);
    lv_obj_set_pos(title, 10, 5);

    // User ID Input
    lv_obj_t *userid_label = lv_label_create(top_cont);
    lv_label_set_text(userid_label, "User ID:");
    lv_obj_set_pos(userid_label, 10, 35);
    if (font_normal) {
        lv_obj_set_style_text_font(userid_label, font_normal, 0);
    }

    userid_textarea = lv_textarea_create(top_cont);
    lv_obj_set_size(userid_textarea, WINDOW_WIDTH - 40, 32);
    lv_obj_set_pos(userid_textarea, 10, 55);
    lv_textarea_set_one_line(userid_textarea, true);
    lv_textarea_set_max_length(userid_textarea, 32);  // Max 32 characters
    lv_textarea_set_placeholder_text(userid_textarea, "Enter you user iD here.");
    if (font_normal) {
        lv_obj_set_style_text_font(userid_textarea, font_normal, 0);
    }
    lv_obj_add_event_cb(userid_textarea, userid_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(userid_textarea, userid_event_handler, LV_EVENT_FOCUSED, NULL);

    // Password Input
    lv_obj_t *password_label = lv_label_create(top_cont);
    lv_label_set_text(password_label, "Password:");
    lv_obj_set_pos(password_label, 10, 95);
    if (font_normal) {
        lv_obj_set_style_text_font(password_label, font_normal, 0);
    }

    password_textarea = lv_textarea_create(top_cont);
    lv_obj_set_size(password_textarea, WINDOW_WIDTH - 40, 32);
    lv_obj_set_pos(password_textarea, 10, 115);
    lv_textarea_set_one_line(password_textarea, true);
    lv_textarea_set_password_mode(password_textarea, true);
    lv_textarea_set_max_length(password_textarea, 32);  // Max 32 characters
    lv_textarea_set_placeholder_text(password_textarea, "Enter your password here(more than 11 chars).");
    if (font_normal) {
        lv_obj_set_style_text_font(password_textarea, font_normal, 0);
    }
    lv_obj_add_event_cb(password_textarea, password_event_handler, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(password_textarea, password_event_handler, LV_EVENT_FOCUSED, NULL);

    // Login button (always enabled)
    login_btn = lv_button_create(top_cont);
    lv_obj_set_size(login_btn, WINDOW_WIDTH - 40, 35);
    lv_obj_set_pos(login_btn, 10, 160);
    lv_obj_add_event_cb(login_btn, login_btn_event_handler, LV_EVENT_CLICKED, NULL);

    lv_obj_t *btn_label = lv_label_create(login_btn);
    lv_label_set_text(btn_label, "로그인");
    if (font_normal) {
        lv_obj_set_style_text_font(btn_label, font_normal, 0);
    }
    lv_obj_center(btn_label);

    // Create keypad (3x5 grid with all functions integrated)
    // All necessary buttons (Shift, Backspace, Space, Mode) are in the keypad grid
    // Use bold font, size 20 for keypad buttons
    keypad_config_t keypad_cfg = {
        .width = WINDOW_WIDTH - 10,
        .height = 290,  // Reduced height to leave bottom margin
        .parent = screen,
        .target_textarea = userid_textarea,
        .button_font = font_bold  // Use bold Korean font for keypad buttons
    };

    keypad_obj = keypad_create(&keypad_cfg);
    lv_obj_set_pos(keypad_obj, 5, 275);  // Moved down slightly for top margin

    // Set initial active textarea
    active_textarea = userid_textarea;
}

/**
 * @brief Sets the login callback function
 */
void gui_set_login_callback(login_callback_t callback) {
    login_callback = callback;
}

/**
 * @brief Runs the main GUI event loop
 */
void gui_run(void) {
    while (1) {
        uint32_t time_till_next = lv_timer_handler();
        usleep(time_till_next * 1000);
    }
}

/**
 * @brief Cleans up GUI resources
 */
void gui_cleanup(void) {
    if (font_normal) {
        lv_freetype_font_delete(font_normal);
        font_normal = NULL;
    }
    if (font_bold) {
        lv_freetype_font_delete(font_bold);
        font_bold = NULL;
    }
}

/**
 * @brief Gets the User ID input field object
 */
lv_obj_t *gui_get_userid_input(void) {
    return userid_textarea;
}

/**
 * @brief Gets the Password input field object
 */
lv_obj_t *gui_get_password_input(void) {
    return password_textarea;
}

/**
 * @brief Displays a status message (no-op since status label removed)
 */
void gui_show_status(const char *message, bool is_error) {
    // Status label has been removed from UI
    // Function kept for API compatibility but does nothing
    (void)message;
    (void)is_error;
}

/**
 * @brief Gets the normal Korean font
 */
lv_font_t *gui_get_normal_font(void) {
    return font_normal;
}

/**
 * @brief Gets the bold Korean font
 */
lv_font_t *gui_get_bold_font(void) {
    return font_bold;
}
