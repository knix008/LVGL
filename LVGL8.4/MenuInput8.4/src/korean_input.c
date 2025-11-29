#include "../include/korean_input.h"
#include "../include/config.h"
#include "../include/types.h"
#include "../include/style.h"
#include "../include/screen.h"
#include "../include/chunjiin.h"
#include <string.h>

// ============================================================================
// GLOBAL STATE
// ============================================================================

static ChunjiinState chunjiin_state;
static lv_obj_t *text_display;
static lv_obj_t *mode_label;
static lv_obj_t *keyboard_buttons[12];

// ============================================================================
// EVENT CALLBACKS
// ============================================================================

static void back_btn_callback(lv_event_t *e) {
    (void)e;
    if (screen_stack_top > 0) {
        screen_stack_top--;
        show_screen(screen_stack[screen_stack_top].screen_id);
    }
}

static void mode_switch_callback(lv_event_t *e) {
    (void)e;
    change_mode(&chunjiin_state);

    // Update mode label
    const char *mode_text = "";
    switch (chunjiin_state.now_mode) {
        case MODE_HANGUL:
            mode_text = "한글";
            break;
        case MODE_UPPER_ENGLISH:
            mode_text = "영문(대)";
            break;
        case MODE_ENGLISH:
            mode_text = "영문(소)";
            break;
        case MODE_NUMBER:
            mode_text = "숫자";
            break;
        case MODE_SPECIAL:
            mode_text = "특수문자";
            break;
    }
    lv_label_set_text(mode_label, mode_text);

    // Update button labels
    for (int i = 0; i < 12; i++) {
        const wchar_t *btn_text = get_button_text(chunjiin_state.now_mode, i);
        char *utf8_text = wchar_to_utf8(btn_text, 16);
        lv_obj_t *label = lv_obj_get_child(keyboard_buttons[i], 0);
        if (label) {
            lv_label_set_text(label, utf8_text);
        }
    }
}

static void keyboard_btn_callback(lv_event_t *e) {
    int btn_num = (int)(intptr_t)lv_event_get_user_data(e);

    chunjiin_process_input(&chunjiin_state, btn_num);

    // Update text display
    char *utf8_text = wchar_to_utf8(chunjiin_state.text_buffer, MAX_TEXT_LEN);
    lv_label_set_text(text_display, utf8_text);
}

static void clear_btn_callback(lv_event_t *e) {
    (void)e;
    chunjiin_init(&chunjiin_state);
    lv_label_set_text(text_display, "");
}

static void info_btn_callback(lv_event_t *e) {
    (void)e;
    if (screen_stack[screen_stack_top].screen_id != SCREEN_INFO) {
        // Navigate using absolute path: clear stack to MENU then go to INFO
        screen_stack_top = 0;  // Reset to MAIN
        show_screen(SCREEN_MENU);  // Go through MENU
        show_screen(SCREEN_INFO);  // Then to INFO
    }
}

static void admin_btn_callback(lv_event_t *e) {
    (void)e;
    if (screen_stack[screen_stack_top].screen_id != SCREEN_ADMIN) {
        // Navigate using absolute path: clear stack to MENU then go to ADMIN
        screen_stack_top = 0;  // Reset to MAIN
        show_screen(SCREEN_MENU);  // Go through MENU
        show_screen(SCREEN_ADMIN);  // Then to ADMIN
    }
}

static void network_btn_callback(lv_event_t *e) {
    (void)e;
    if (screen_stack[screen_stack_top].screen_id != SCREEN_NETWORK) {
        // Navigate using absolute path: clear stack to MENU then go to NETWORK
        screen_stack_top = 0;  // Reset to MAIN
        show_screen(SCREEN_MENU);  // Go through MENU
        show_screen(SCREEN_NETWORK);  // Then to NETWORK
    }
}

static void korean_input_btn_callback(lv_event_t *e) {
    (void)e;
    if (screen_stack[screen_stack_top].screen_id != SCREEN_KOREAN_INPUT) {
        // Navigate using absolute path: clear stack to MENU then go to KOREAN_INPUT
        screen_stack_top = 0;  // Reset to MAIN
        show_screen(SCREEN_MENU);  // Go through MENU
        show_screen(SCREEN_KOREAN_INPUT);  // Then to KOREAN_INPUT
    }
}

// ============================================================================
// KOREAN INPUT SCREEN COMPONENTS
// ============================================================================

static lv_obj_t *create_korean_input_title_bar(lv_obj_t *parent) {
    lv_obj_t *title_bar = lv_obj_create(parent);
    lv_obj_set_size(title_bar, SCREEN_WIDTH, TITLE_BAR_HEIGHT);
    lv_obj_align(title_bar, LV_ALIGN_TOP_MID, 0, 0);
    apply_bar_style(title_bar, COLOR_BG_TITLE);

    // Back button (circular)
    lv_obj_t *back_btn = lv_btn_create(title_bar);
    lv_obj_set_size(back_btn, TITLE_BAR_HEIGHT - 20, TITLE_BAR_HEIGHT - 20);
    lv_obj_align(back_btn, LV_ALIGN_LEFT_MID, PADDING_HORIZONTAL, 0);
    apply_circle_button_style(back_btn, COLOR_BUTTON_BACK);

    lv_obj_t *back_img = lv_img_create(back_btn);
    lv_img_set_src(back_img, IMG_BACK_BUTTON);
    lv_obj_align(back_img, LV_ALIGN_CENTER, 0, 0);

    lv_obj_add_event_cb(back_btn, back_btn_callback, LV_EVENT_CLICKED, NULL);

    // Title label
    lv_obj_t *title_label = lv_label_create(title_bar);
    lv_label_set_text(title_label, "");
    apply_label_style(title_label);
    lv_obj_align(title_label, LV_ALIGN_LEFT_MID, (TITLE_BAR_HEIGHT - 20) + PADDING_HORIZONTAL * 2, 0);

    // Store the title label
    extern AppState app_state;
    app_state.current_title_label = title_label;

    // Update the title with breadcrumb path
    extern void update_title_bar_location(int screen_id);
    update_title_bar_location(SCREEN_KOREAN_INPUT);

    return title_bar;
}

static lv_obj_t *create_korean_input_content(lv_obj_t *parent) {
    lv_obj_t *content = lv_obj_create(parent);
    lv_obj_set_size(content, SCREEN_WIDTH, SCREEN_HEIGHT - TITLE_BAR_HEIGHT - STATUS_BAR_HEIGHT);
    lv_obj_align(content, LV_ALIGN_TOP_MID, 0, TITLE_BAR_HEIGHT);
    lv_obj_set_style_bg_color(content, lv_color_hex(COLOR_BG_DARK), 0);
    lv_obj_set_style_border_width(content, 0, 0);
    lv_obj_clear_flag(content, LV_OBJ_FLAG_SCROLLABLE);

    int y_offset = 10;

    // Mode indicator
    mode_label = lv_label_create(content);
    lv_label_set_text(mode_label, "한글");
    apply_label_style(mode_label);
    lv_obj_set_style_text_align(mode_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(mode_label, LV_ALIGN_TOP_MID, 0, y_offset);
    y_offset += 30;

    // Text display area
    lv_obj_t *text_container = lv_obj_create(content);
    lv_obj_set_size(text_container, SCREEN_WIDTH - 20, 100);
    lv_obj_align(text_container, LV_ALIGN_TOP_MID, 0, y_offset);
    lv_obj_set_style_bg_color(text_container, lv_color_hex(0x000000), 0);
    lv_obj_set_style_border_color(text_container, lv_color_hex(COLOR_BORDER), 0);
    lv_obj_set_style_border_width(text_container, 2, 0);
    lv_obj_set_style_pad_all(text_container, 10, 0);

    text_display = lv_label_create(text_container);
    lv_label_set_long_mode(text_display, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(text_display, SCREEN_WIDTH - 60);
    apply_label_style(text_display);
    lv_label_set_text(text_display, "");
    lv_obj_align(text_display, LV_ALIGN_TOP_LEFT, 0, 0);

    y_offset += 110;

    // Keyboard grid - matching Chunjiin8.4 layout
    // Button positions: each row has 3 buttons
    // Row 0: 천(1), 지(2), 인(3)
    // Row 1: ㄱ(4), ㄴ(5), ㄷ(6)
    // Row 2: ㅂ(7), ㅅ(8), ㅈ(9)
    // Row 3: 공백(10), ㅇㅁ(0), 삭제(11)
    int btn_width = 90;
    int btn_height = 60;
    int btn_spacing = 10;
    int grid_width = btn_width * 3 + btn_spacing * 2;
    int grid_height = btn_height * 4 + btn_spacing * 3;

    // Create a centered container for the button grid
    lv_obj_t *button_grid = lv_obj_create(content);
    lv_obj_set_size(button_grid, grid_width, grid_height);
    lv_obj_align(button_grid, LV_ALIGN_TOP_MID, 0, y_offset);
    lv_obj_set_style_bg_opa(button_grid, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(button_grid, 0, 0);
    lv_obj_set_style_pad_all(button_grid, 0, 0);

    // Button positions mapping from reference implementation
    // positions[button_num][0] = column, positions[button_num][1] = row
    int positions[12][2] = {
        {1, 3}, // 0: Row 3, Col 1 (ㅇㅁ)
        {0, 0}, {1, 0}, {2, 0}, // 1-3: Row 0 (천, 지, 인)
        {0, 1}, {1, 1}, {2, 1}, // 4-6: Row 1 (ㄱ, ㄴ, ㄷ)
        {0, 2}, {1, 2}, {2, 2}, // 7-9: Row 2 (ㅂ, ㅅ, ㅈ)
        {0, 3}, {2, 3}  // 10-11: Row 3 (Space, Del)
    };

    // Create 12 buttons with correct positions (relative to button_grid)
    for (int i = 0; i < 12; i++) {
        const wchar_t *btn_text = get_button_text(MODE_HANGUL, i);
        char *utf8_text = wchar_to_utf8(btn_text, 16);

        lv_obj_t *btn = lv_btn_create(button_grid);
        lv_obj_set_size(btn, btn_width, btn_height);
        lv_obj_set_pos(btn,
            positions[i][0] * (btn_width + btn_spacing),
            positions[i][1] * (btn_height + btn_spacing));
        apply_button_style(btn, COLOR_BUTTON_BG);

        lv_obj_t *label = lv_label_create(btn);
        lv_label_set_text(label, utf8_text);
        apply_label_style(label);
        lv_obj_center(label);

        lv_obj_add_event_cb(btn, keyboard_btn_callback, LV_EVENT_CLICKED, (void *)(intptr_t)i);

        keyboard_buttons[i] = btn;
    }

    y_offset += grid_height + 10;

    // Control buttons row - centered container
    int ctrl_btn_width = 130;
    int ctrl_btn_spacing = 10;
    int ctrl_btn_height = 50;
    int ctrl_row_width = ctrl_btn_width * 2 + ctrl_btn_spacing;

    lv_obj_t *ctrl_container = lv_obj_create(content);
    lv_obj_set_size(ctrl_container, ctrl_row_width, ctrl_btn_height);
    lv_obj_align(ctrl_container, LV_ALIGN_TOP_MID, 0, y_offset);
    lv_obj_set_style_bg_opa(ctrl_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(ctrl_container, 0, 0);
    lv_obj_set_style_pad_all(ctrl_container, 0, 0);

    // Mode switch button
    lv_obj_t *mode_btn = lv_btn_create(ctrl_container);
    lv_obj_set_size(mode_btn, ctrl_btn_width, ctrl_btn_height);
    lv_obj_set_pos(mode_btn, 0, 0);
    apply_button_style(mode_btn, COLOR_BUTTON_BG);

    lv_obj_t *mode_btn_label = lv_label_create(mode_btn);
    lv_label_set_text(mode_btn_label, "모드");
    apply_label_style(mode_btn_label);
    lv_obj_center(mode_btn_label);

    lv_obj_add_event_cb(mode_btn, mode_switch_callback, LV_EVENT_CLICKED, NULL);

    // Clear button
    lv_obj_t *clear_btn = lv_btn_create(ctrl_container);
    lv_obj_set_size(clear_btn, ctrl_btn_width, ctrl_btn_height);
    lv_obj_set_pos(clear_btn, ctrl_btn_width + ctrl_btn_spacing, 0);
    apply_button_style(clear_btn, COLOR_BUTTON_BG);

    lv_obj_t *clear_btn_label = lv_label_create(clear_btn);
    lv_label_set_text(clear_btn_label, "지우기");
    apply_label_style(clear_btn_label);
    lv_obj_center(clear_btn_label);

    lv_obj_add_event_cb(clear_btn, clear_btn_callback, LV_EVENT_CLICKED, NULL);

    return content;
}

static lv_obj_t *create_korean_input_status_bar(lv_obj_t *parent) {
    lv_obj_t *status_bar = lv_obj_create(parent);
    lv_obj_set_size(status_bar, SCREEN_WIDTH, STATUS_BAR_HEIGHT);
    lv_obj_align(status_bar, LV_ALIGN_BOTTOM_MID, 0, 0);
    apply_bar_style(status_bar, COLOR_BG_TITLE);

    // Image button configuration
    int img_btn_size = 40;
    int spacing = 10;
    int start_x = PADDING_HORIZONTAL;

    // Config button with image
    lv_obj_t *config_btn = lv_btn_create(status_bar);
    lv_obj_set_size(config_btn, img_btn_size, img_btn_size);
    lv_obj_set_pos(config_btn, start_x, (STATUS_BAR_HEIGHT - img_btn_size) / 2);
    apply_circle_button_style(config_btn, COLOR_BUTTON_BACK);

    lv_obj_t *config_img = lv_img_create(config_btn);
    lv_img_set_src(config_img, IMG_CONFIG);
    lv_obj_center(config_img);
    lv_obj_add_event_cb(config_btn, admin_btn_callback, LV_EVENT_CLICKED, NULL);

    // Korean input button with image
    lv_obj_t *korean_btn = lv_btn_create(status_bar);
    lv_obj_set_size(korean_btn, img_btn_size, img_btn_size);
    lv_obj_set_pos(korean_btn, start_x + img_btn_size + spacing, (STATUS_BAR_HEIGHT - img_btn_size) / 2);
    apply_circle_button_style(korean_btn, COLOR_BUTTON_BACK);

    lv_obj_t *korean_img = lv_img_create(korean_btn);
    lv_img_set_src(korean_img, IMG_KOREAN);
    lv_obj_center(korean_img);
    lv_obj_add_event_cb(korean_btn, korean_input_btn_callback, LV_EVENT_CLICKED, NULL);

    // Info button with image
    lv_obj_t *info_btn = lv_btn_create(status_bar);
    lv_obj_set_size(info_btn, img_btn_size, img_btn_size);
    lv_obj_set_pos(info_btn, start_x + (img_btn_size + spacing) * 2, (STATUS_BAR_HEIGHT - img_btn_size) / 2);
    apply_circle_button_style(info_btn, COLOR_BUTTON_BACK);

    lv_obj_t *info_img = lv_img_create(info_btn);
    lv_img_set_src(info_img, IMG_INFO);
    lv_obj_center(info_img);
    lv_obj_add_event_cb(info_btn, info_btn_callback, LV_EVENT_CLICKED, NULL);

    // Network button with image
    lv_obj_t *network_btn = lv_btn_create(status_bar);
    lv_obj_set_size(network_btn, img_btn_size, img_btn_size);
    lv_obj_set_pos(network_btn, start_x + (img_btn_size + spacing) * 3, (STATUS_BAR_HEIGHT - img_btn_size) / 2);
    apply_circle_button_style(network_btn, COLOR_BUTTON_BACK);

    lv_obj_t *network_img = lv_img_create(network_btn);
    lv_img_set_src(network_img, IMG_NETWORK);
    lv_obj_center(network_img);
    lv_obj_add_event_cb(network_btn, network_btn_callback, LV_EVENT_CLICKED, NULL);

    return status_bar;
}

// ============================================================================
// KOREAN INPUT SCREEN CREATION
// ============================================================================

void create_korean_input_screen(void) {
    // Initialize Chunjiin state
    chunjiin_init(&chunjiin_state);

    lv_obj_t *korean_input_screen = lv_obj_create(NULL);
    lv_obj_set_size(korean_input_screen, SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_obj_set_style_bg_color(korean_input_screen, lv_color_hex(COLOR_BG_DARK), 0);

    // Disable scrolling
    lv_obj_set_scrollbar_mode(korean_input_screen, LV_SCROLLBAR_MODE_OFF);
    lv_obj_clear_flag(korean_input_screen, LV_OBJ_FLAG_SCROLLABLE);

    // Add to screen stack
    if (screen_stack_top + 1 < MAX_SCREENS) {
        screen_stack_top++;
        screen_stack[screen_stack_top].screen = korean_input_screen;
        screen_stack[screen_stack_top].screen_id = SCREEN_KOREAN_INPUT;
    }

    create_korean_input_title_bar(korean_input_screen);
    create_korean_input_content(korean_input_screen);
    create_korean_input_status_bar(korean_input_screen);

    lv_scr_load(korean_input_screen);
}
