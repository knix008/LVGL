#include "lvgl/lvgl.h"
#include "mac_input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Display buffer
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 640

// Application state (GUI components + MAC address)
typedef struct {
    lv_obj_t *screen;
    lv_obj_t *text_area;
    lv_obj_t *status_label;
    MacAddress mac;  // MAC address data
    lv_font_t *font_10;  // FreeType font 10pt
    lv_font_t *font_14;  // FreeType font 14pt
    lv_font_t *font_16;  // FreeType font 16pt
    lv_font_t *font_20;  // FreeType font 20pt
} AppState;

static AppState app_state = {NULL, NULL, NULL, {{0}, 0}, NULL, NULL, NULL, NULL};

// Static storage for hex digit characters
static char hex_digits[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

// Forward declarations
static void update_display(void);

// Update the display with current MAC address
static void update_display(void) {
    char formatted[18];
    mac_get_formatted(&app_state.mac, formatted, sizeof(formatted));
    lv_textarea_set_text(app_state.text_area, formatted);
    
    // Update status
    char status[32];
    snprintf(status, sizeof(status), "Digits: %d/12", mac_get_digit_count(&app_state.mac));
    lv_label_set_text(app_state.status_label, status);
}

// Hex button callback
static void on_hex_clicked(lv_event_t *e) {
    char *digit = (char *)lv_event_get_user_data(e);
    mac_add_digit(&app_state.mac, *digit);
    update_display();
}

// Backspace callback
static void on_backspace_clicked(lv_event_t *e) {
    (void)e;
    mac_delete_last(&app_state.mac);
    update_display();
}

// Clear callback
static void on_clear_clicked(lv_event_t *e) {
    (void)e;
    mac_clear(&app_state.mac);
    update_display();
}

// Enter callback - validates and shows popup with result
static void on_enter_clicked(lv_event_t *e) {
    (void)e;
    
    char error_msg[256];
    
    // Validate MAC address
    if (!mac_validate(&app_state.mac, error_msg, sizeof(error_msg))) {
        // Invalid: Show error message with error icon
        lv_obj_t *msgbox = lv_msgbox_create(lv_screen_active());
        
        // Add error icon and title
        char title_with_icon[64];
        snprintf(title_with_icon, sizeof(title_with_icon), "%s  Invalid MAC Address", LV_SYMBOL_CLOSE);
        lv_msgbox_add_title(msgbox, title_with_icon);
        
        // Set title color to red for error
        lv_obj_t *title = lv_msgbox_get_title(msgbox);
        if (title) {
            lv_obj_set_style_text_color(title, lv_palette_main(LV_PALETTE_RED), 0);
        }
        
        lv_msgbox_add_text(msgbox, error_msg);
        lv_msgbox_add_close_button(msgbox);
        lv_obj_center(msgbox);
        return;  // Don't clear - let user continue entering
    }
    
    // Valid MAC address: Show info message with info icon
    char formatted[18];
    mac_get_formatted(&app_state.mac, formatted, sizeof(formatted));
    
    lv_obj_t *msgbox = lv_msgbox_create(lv_screen_active());
    
    // Add info icon and title
    char title_with_icon[64];
    snprintf(title_with_icon, sizeof(title_with_icon), "%s  MAC Address", LV_SYMBOL_OK);
    lv_msgbox_add_title(msgbox, title_with_icon);
    
    // Set title color to green for success
    lv_obj_t *title = lv_msgbox_get_title(msgbox);
    if (title) {
        lv_obj_set_style_text_color(title, lv_palette_main(LV_PALETTE_GREEN), 0);
    }
    
    // Show the MAC address
    lv_msgbox_add_text(msgbox, formatted);
    
    lv_msgbox_add_close_button(msgbox);
    lv_obj_center(msgbox);
    
    // Clear the input after successful validation
    mac_clear(&app_state.mac);
    update_display();
}

// Create a hex keypad button
static lv_obj_t* create_hex_button(lv_obj_t *parent, const char *label, 
                                    lv_event_cb_t callback, void *user_data, 
                                    int width, int height) {
    lv_obj_t *btn = lv_button_create(parent);
    lv_obj_set_size(btn, width, height);
    
    lv_obj_t *label_obj = lv_label_create(btn);
    lv_label_set_text(label_obj, label);
    lv_obj_center(label_obj);
    
    // Set font size - use FreeType font from assets
    if (app_state.font_16) {
        lv_obj_set_style_text_font(label_obj, app_state.font_16, 0);
    }
    
    if (callback) {
        lv_obj_add_event_cb(btn, callback, LV_EVENT_CLICKED, user_data);
    }
    
    return btn;
}

// Create the GUI
static void create_gui(void) {
    // Initialize MAC address
    mac_init(&app_state.mac);

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
    lv_obj_set_style_pad_gap(main_cont, 10, 0);

    // Title label
    lv_obj_t *title = lv_label_create(main_cont);
    lv_label_set_text(title, "MAC Address Input");
    if (app_state.font_20) {
        lv_obj_set_style_text_font(title, app_state.font_20, 0);
    }

    // Status label
    app_state.status_label = lv_label_create(main_cont);
    lv_label_set_text(app_state.status_label, "Digits: 0/12");
    if (app_state.font_10) {
        lv_obj_set_style_text_font(app_state.status_label, app_state.font_10, 0);
    }

    // Text area for MAC address display
    app_state.text_area = lv_textarea_create(main_cont);
    lv_obj_set_size(app_state.text_area, SCREEN_WIDTH - 40, 45);
    lv_textarea_set_placeholder_text(app_state.text_area, "XX:XX:XX:XX:XX:XX");
    lv_textarea_set_one_line(app_state.text_area, true);
    lv_textarea_set_max_length(app_state.text_area, 17);
    if (app_state.font_16) {
        lv_obj_set_style_text_font(app_state.text_area, app_state.font_16, 0);
    }
    lv_obj_set_style_text_align(app_state.text_area, LV_TEXT_ALIGN_CENTER, 0);

    // Hex keypad container
    lv_obj_t *keypad_cont = lv_obj_create(main_cont);
    lv_obj_set_size(keypad_cont, SCREEN_WIDTH - 40, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(keypad_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(keypad_cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(keypad_cont, 5, 0);
    lv_obj_set_style_pad_gap(keypad_cont, 5, 0);

    const int btn_size = 55;
    const int gap = 5;

    // Row 1: 1 2 3 4
    lv_obj_t *row = lv_obj_create(keypad_cont);
    lv_obj_set_size(row, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(row, 0, 0);
    lv_obj_set_style_pad_gap(row, gap, 0);
    lv_obj_set_style_border_width(row, 0, 0);
    lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);

    for (int i = 1; i <= 4; i++) {
        char label[2] = {hex_digits[i], 0};
        create_hex_button(row, label, on_hex_clicked, &hex_digits[i], btn_size, btn_size);
    }

    // Row 2: 5 6 7 8
    row = lv_obj_create(keypad_cont);
    lv_obj_set_size(row, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(row, 0, 0);
    lv_obj_set_style_pad_gap(row, gap, 0);
    lv_obj_set_style_border_width(row, 0, 0);
    lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);

    for (int i = 5; i <= 8; i++) {
        char label[2] = {hex_digits[i], 0};
        create_hex_button(row, label, on_hex_clicked, &hex_digits[i], btn_size, btn_size);
    }

    // Row 3: 9 A B C
    row = lv_obj_create(keypad_cont);
    lv_obj_set_size(row, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(row, 0, 0);
    lv_obj_set_style_pad_gap(row, gap, 0);
    lv_obj_set_style_border_width(row, 0, 0);
    lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);

    create_hex_button(row, "9", on_hex_clicked, &hex_digits[9], btn_size, btn_size);
    for (int i = 10; i <= 12; i++) {
        char label[2] = {hex_digits[i], 0};
        create_hex_button(row, label, on_hex_clicked, &hex_digits[i], btn_size, btn_size);
    }

    // Row 4: 0 D E F
    row = lv_obj_create(keypad_cont);
    lv_obj_set_size(row, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(row, 0, 0);
    lv_obj_set_style_pad_gap(row, gap, 0);
    lv_obj_set_style_border_width(row, 0, 0);
    lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);

    create_hex_button(row, "0", on_hex_clicked, &hex_digits[0], btn_size, btn_size);
    for (int i = 13; i <= 15; i++) {
        char label[2] = {hex_digits[i], 0};
        create_hex_button(row, label, on_hex_clicked, &hex_digits[i], btn_size, btn_size);
    }

    // Control buttons row
    row = lv_obj_create(keypad_cont);
    lv_obj_set_size(row, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_pad_all(row, 0, 0);
    lv_obj_set_style_pad_gap(row, gap, 0);
    lv_obj_set_style_border_width(row, 0, 0);
    lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);

    // Backspace button
    lv_obj_t *backspace_btn = create_hex_button(row, "←", on_backspace_clicked, NULL, btn_size, btn_size);
    lv_obj_set_style_bg_color(backspace_btn, lv_palette_main(LV_PALETTE_ORANGE), 0);

    // Clear button
    lv_obj_t *clear_btn = create_hex_button(row, "CLR", on_clear_clicked, NULL, btn_size, btn_size);
    lv_obj_set_style_bg_color(clear_btn, lv_palette_main(LV_PALETTE_RED), 0);

    // Enter button
    lv_obj_t *enter_btn = create_hex_button(row, "ENTER", on_enter_clicked, NULL, btn_size * 2 + gap, btn_size);
    lv_obj_set_style_bg_color(enter_btn, lv_palette_main(LV_PALETTE_GREEN), 0);

    // Initialize display
    update_display();
}

// Initialize FreeType fonts from assets directory
static int init_fonts(void) {
    // Load font 10pt for status label
    app_state.font_10 = lv_freetype_font_create("assets/NanumGothicCoding.ttf",
                                                 LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                                 10,
                                                 LV_FREETYPE_FONT_STYLE_NORMAL);
    if (!app_state.font_10) {
        fprintf(stderr, "Error: Failed to load font 10pt\n");
        return -1;
    }

    // Load font 14pt (reserved for future use)
    app_state.font_14 = lv_freetype_font_create("assets/NanumGothicCoding.ttf",
                                                 LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                                 14,
                                                 LV_FREETYPE_FONT_STYLE_NORMAL);
    if (!app_state.font_14) {
        fprintf(stderr, "Error: Failed to load font 14pt\n");
        return -1;
    }

    // Load font 16pt for text area and buttons
    app_state.font_16 = lv_freetype_font_create("assets/NanumGothicCoding.ttf",
                                                 LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                                 16,
                                                 LV_FREETYPE_FONT_STYLE_NORMAL);
    if (!app_state.font_16) {
        fprintf(stderr, "Error: Failed to load font 16pt\n");
        return -1;
    }

    // Load font 20pt for title
    app_state.font_20 = lv_freetype_font_create("assets/NanumGothicCoding.ttf",
                                                 LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                                 20,
                                                 LV_FREETYPE_FONT_STYLE_NORMAL);
    if (!app_state.font_20) {
        fprintf(stderr, "Error: Failed to load font 20pt\n");
        return -1;
    }

    return 0;
}

// Initialize LVGL with SDL
static int init_lvgl(void) {
    // Initialize LVGL
    lv_init();

    // Create SDL window and display
    lv_display_t *display = lv_sdl_window_create(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!display) {
        fprintf(stderr, "Error: Failed to create SDL window\n");
        return -1;
    }

    // Create mouse input device
    lv_indev_t *mouse = lv_sdl_mouse_create();
    if (!mouse) {
        fprintf(stderr, "Warning: Failed to create mouse input device\n");
    }

    // Create keyboard input device (optional)
    lv_indev_t *keyboard = lv_sdl_keyboard_create();
    if (!keyboard) {
        fprintf(stderr, "Warning: Failed to create keyboard input device\n");
    }

    // Initialize FreeType fonts from assets directory
    if (init_fonts() != 0) {
        fprintf(stderr, "Error: Font initialization failed\n");
        return -1;
    }

    return 0;
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    // Initialize LVGL with SDL
    if (init_lvgl() != 0) {
        return 1;
    }

    // Create GUI
    create_gui();

    // Main loop
    while (1) {
        uint32_t time_till_next = lv_timer_handler();
        lv_delay_ms(time_till_next);
    }

    return 0;
}
