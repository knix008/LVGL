/*
* Chunjiin Korean Input Method - LVGL GUI
* Main application file
*/

#include "lvgl/lvgl.h"
#include "lvgl/src/drivers/sdl/lv_sdl_window.h"
#include "lvgl/src/drivers/sdl/lv_sdl_mouse.h"
#include "lvgl/src/drivers/sdl/lv_sdl_mousewheel.h"
#include "lvgl/src/drivers/sdl/lv_sdl_keyboard.h"
#include "lvgl/src/libs/freetype/lv_freetype.h"
#include <stdio.h>
#include <unistd.h>
#include <locale.h>
#include <string.h>
#include "chunjiin.h"

typedef struct {
    lv_obj_t *text_area;
    lv_obj_t *buttons[12];
    lv_obj_t *mode_button;
    lv_obj_t *clear_button;
    lv_obj_t *enter_button;
    ChunjiinState state;
} AppWidgets;

static AppWidgets app_widgets;
static lv_obj_t *active_mbox = NULL; // Track active message box
static lv_font_t *korean_font_16 = NULL;
static lv_font_t *korean_font_20 = NULL;

// wchar_t buffer to UTF-8 string conversion helper

// Button click event handler
static void on_button_clicked(lv_event_t *e) {
    int button_num = (int)(intptr_t)lv_event_get_user_data(e);
    //printf("Button clicked: %d, Mode: %d\n", button_num, app_widgets.state.now_mode);

    // Process input
    chunjiin_process_input(&app_widgets.state, button_num);

    // Update text area
    char *utf8_text = wchar_to_utf8(app_widgets.state.text_buffer, MAX_TEXT_LEN);
    //printf("Text buffer after input: %ls\n", app_widgets.state.text_buffer);
    //printf("UTF-8 text: %s\n", utf8_text);
    lv_textarea_set_text(app_widgets.text_area, utf8_text);
}

// Mode button click handler
static void on_mode_button_clicked(lv_event_t *e) {
    (void)e;
    change_mode(&app_widgets.state);

    // Update button labels
    for (int i = 0; i < 12; i++) {
        if (app_widgets.buttons[i] == NULL) continue;
        const wchar_t *wtext = get_button_text(app_widgets.state.now_mode, i);
        char *utf8_text = wchar_to_utf8(wtext, 20);
        lv_obj_t *label = lv_obj_get_child(app_widgets.buttons[i], 0);
        if (label) {
            lv_label_set_text(label, utf8_text);
        }
    }
}

// Clear button handler
static void on_clear_clicked(lv_event_t *e) {
    (void)e;
    
    // Defensive: check if text area is valid
    if (!app_widgets.text_area) {
        printf("Error: Text area not initialized\n");
        return;
    }
    
    // Save current mode
    InputMode current_mode = app_widgets.state.now_mode;

    // Clear text (preserve mode)
    chunjiin_init(&app_widgets.state);
    app_widgets.state.now_mode = current_mode;
    app_widgets.state.cursor_pos = 0;
    memset(app_widgets.state.text_buffer, 0, sizeof(app_widgets.state.text_buffer));
    lv_textarea_set_text(app_widgets.text_area, "");
    
    // Defensive: reset message box pointer
    if (active_mbox) {
        if (lv_obj_is_valid(active_mbox)) {
            lv_obj_del(active_mbox);
        }
        active_mbox = NULL;
    }
}

// Timer callback to auto-dismiss popup
static void popup_timer_cb(lv_timer_t *timer) {
    if (active_mbox && lv_obj_is_valid(active_mbox)) {
        lv_obj_del(active_mbox);
        active_mbox = NULL;
    }
    lv_timer_del(timer);
}

// Safe popup creation function
static lv_obj_t* create_safe_popup(const char* title, const char* message) {
    // Create a container for the popup
    lv_obj_t *popup = lv_obj_create(lv_screen_active());
    if (!popup) return NULL;
    
    // Set popup properties
    lv_obj_set_size(popup, 300, 150);
    lv_obj_center(popup);
    lv_obj_set_style_bg_opa(popup, LV_OPA_90, 0);
    lv_obj_set_style_bg_color(popup, lv_color_black(), 0);
    lv_obj_set_style_border_width(popup, 2, 0);
    lv_obj_set_style_border_color(popup, lv_color_white(), 0);
    lv_obj_set_style_radius(popup, 10, 0);
    lv_obj_set_style_pad_all(popup, 15, 0);
    
    // Create title label
    lv_obj_t *title_label = lv_label_create(popup);
    if (title_label) {
        lv_label_set_text(title_label, title);
        lv_obj_set_style_text_color(title_label, lv_color_white(), 0);
        lv_obj_set_style_text_font(title_label, korean_font_16, 0);
        lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 10);
    }
    
    // Create message label
    lv_obj_t *msg_label = lv_label_create(popup);
    if (msg_label) {
        lv_label_set_text(msg_label, message);
        lv_obj_set_style_text_color(msg_label, lv_color_white(), 0);
        lv_obj_set_style_text_font(msg_label, korean_font_16, 0);
        lv_obj_align(msg_label, LV_ALIGN_CENTER, 0, 0);
        lv_label_set_long_mode(msg_label, LV_LABEL_LONG_WRAP);
        lv_obj_set_width(msg_label, 250);
    }
    
    return popup;
}

// Enter button handler - show result popup then clear
static void on_enter_clicked(lv_event_t *e) {
    (void)e;
    
    // Save current mode
    InputMode current_mode = app_widgets.state.now_mode;

    // Clean up any existing message box first
    if (active_mbox) {
        if (lv_obj_is_valid(active_mbox)) {
            lv_obj_del(active_mbox);
        }
        active_mbox = NULL;
    }

    // Get current text
    const char *text = lv_textarea_get_text(app_widgets.text_area);
    
    // Create popup based on content
    if (text == NULL || text[0] == '\0') {
        // If buffer is empty, show a warning popup
        active_mbox = create_safe_popup("주의!!!", "입력된 내용이 없습니다.");
    } else {
        // Create popup with input result
        active_mbox = create_safe_popup("입력 결과", text);
        
        // Clear text (preserve mode)
        chunjiin_init(&app_widgets.state);
        app_widgets.state.now_mode = current_mode;
        lv_textarea_set_text(app_widgets.text_area, "");
    }
    
    // Auto-dismiss popup after 3 seconds
    if (active_mbox) {
        lv_timer_t *timer = lv_timer_create(popup_timer_cb, 3000, NULL);
        lv_timer_set_repeat_count(timer, 1);
    }
}

void create_ui(void) {
    chunjiin_init(&app_widgets.state);

    // Initialize Korean fonts from TrueType files
    lv_freetype_init(LV_FREETYPE_CACHE_FT_GLYPH_CNT);

    // Load NanumGothic font at size 20 (for title)
    korean_font_20 = lv_freetype_font_create("assets/NanumGothic-Regular.ttf",
                                             LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                             20,
                                             LV_FREETYPE_FONT_STYLE_NORMAL);

    // Load NanumGothic font at size 16 (for text area)
    korean_font_16 = lv_freetype_font_create("assets/NanumGothic-Regular.ttf",
                                             LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                             16,
                                             LV_FREETYPE_FONT_STYLE_NORMAL);

    // Load NanumGothic font at size 14 (for buttons)
    lv_font_t *korean_font_14 = lv_freetype_font_create("assets/NanumGothic-Regular.ttf",
                                                         LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                                         14,
                                                         LV_FREETYPE_FONT_STYLE_NORMAL);

    // Load NanumGothic font at size 12 (for info label)
    lv_font_t *korean_font_12 = lv_freetype_font_create("assets/NanumGothic-Regular.ttf",
                                                         LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                                         12,
                                                         LV_FREETYPE_FONT_STYLE_NORMAL);

    if (!korean_font_16 || !korean_font_20 || !korean_font_14 || !korean_font_12) {
        LV_LOG_ERROR("Failed to load Korean fonts!");
        return;
    }

    // Create main container
    lv_obj_t *main_cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(main_cont, 320, 640);
    lv_obj_center(main_cont);
    lv_obj_set_flex_flow(main_cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(main_cont, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(main_cont, 10, 0);
    lv_obj_set_style_pad_row(main_cont, 10, 0);

    // Title label
    lv_obj_t *title_label = lv_label_create(main_cont);
    lv_label_set_text(title_label, "천지인 한글/영어/숫자/특수키 입력기 예제");
    lv_obj_set_style_text_font(title_label, korean_font_16, 0);

    // Text area (scrollable)
    app_widgets.text_area = lv_textarea_create(main_cont);
    lv_obj_set_size(app_widgets.text_area, 300, 150);
    lv_textarea_set_text(app_widgets.text_area, "");
    lv_obj_set_style_text_font(app_widgets.text_area, korean_font_16, 0);
    
    // Set font for the textarea's internal label
    lv_obj_t *textarea_label = lv_textarea_get_label(app_widgets.text_area);
    if (textarea_label) {
        lv_obj_set_style_text_font(textarea_label, korean_font_16, 0);
    }

    // Button grid container
    lv_obj_t *button_grid = lv_obj_create(main_cont);
    lv_obj_set_size(button_grid, 275, 250);
    lv_obj_set_style_pad_all(button_grid, 3, 0);
    lv_obj_set_style_pad_row(button_grid, 3, 0);
    lv_obj_set_style_pad_column(button_grid, 3, 0);
    lv_obj_set_layout(button_grid, LV_LAYOUT_GRID);

    // Grid: 3 columns, 5 rows (smaller buttons - 45x45)
    static int32_t col_dsc[] = {85, 85, 85, LV_GRID_TEMPLATE_LAST};
    static int32_t row_dsc[] = {45, 45, 45, 45, 45, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(button_grid, col_dsc, row_dsc);

    // Button positions: each row has 3 buttons
    // Row 0: 천(1), 지(2), 인(3)
    // Row 1: ㄱ(4), ㄴ(5), ㄷ(6)
    // Row 2: ㅂ(7), ㅅ(8), ㅈ(9)
    // Row 3: 공백(10), ㅇㅁ(0), 삭제(11)
    // Row 4: 모드, 지우기, 엔터
    int positions[12][2] = {
        {1, 3}, // 0: Row 3, Col 1 (ㅇㅁ)
        {0, 0}, {1, 0}, {2, 0}, // 1-3: Row 0 (천, 지, 인)
        {0, 1}, {1, 1}, {2, 1}, // 4-6: Row 1 (ㄱ, ㄴ, ㄷ)
        {0, 2}, {1, 2}, {2, 2}, // 7-9: Row 2 (ㅂ, ㅅ, ㅈ)
        {0, 3}, {2, 3}  // 10-11: Row 3 (Space, Del)
    };

    // Create number buttons (0-11)
    for (int i = 0; i < 12; i++) {
        const wchar_t *wtext = get_button_text(app_widgets.state.now_mode, i);
        char *utf8_text = wchar_to_utf8(wtext, 20);

        app_widgets.buttons[i] = lv_button_create(button_grid);
        lv_obj_set_grid_cell(app_widgets.buttons[i], LV_GRID_ALIGN_STRETCH, positions[i][0], 1,
                            LV_GRID_ALIGN_STRETCH, positions[i][1], 1);

        lv_obj_t *label = lv_label_create(app_widgets.buttons[i]);
        lv_label_set_text(label, utf8_text);
        lv_obj_set_style_text_font(label, korean_font_14, 0);
        lv_obj_center(label);

        lv_obj_add_event_cb(app_widgets.buttons[i], on_button_clicked, LV_EVENT_CLICKED, (void*)(intptr_t)i);
    }

    // Row 4: Mode, Clear, Enter buttons
    app_widgets.mode_button = lv_button_create(button_grid);
    lv_obj_set_grid_cell(app_widgets.mode_button, LV_GRID_ALIGN_STRETCH, 0, 1,
                        LV_GRID_ALIGN_STRETCH, 4, 1);
    lv_obj_t *mode_label = lv_label_create(app_widgets.mode_button);
    lv_label_set_text(mode_label, "한/영/숫/특");
    lv_obj_set_style_text_font(mode_label, korean_font_14, 0);
    lv_obj_center(mode_label);
    lv_obj_add_event_cb(app_widgets.mode_button, on_mode_button_clicked, LV_EVENT_CLICKED, NULL);

    app_widgets.clear_button = lv_button_create(button_grid);
    lv_obj_set_grid_cell(app_widgets.clear_button, LV_GRID_ALIGN_STRETCH, 1, 1,
                        LV_GRID_ALIGN_STRETCH, 4, 1);
    lv_obj_t *clear_label = lv_label_create(app_widgets.clear_button);
    lv_label_set_text(clear_label, "Clear");
    lv_obj_set_style_text_font(clear_label, korean_font_14, 0);
    lv_obj_center(clear_label);
    lv_obj_add_event_cb(app_widgets.clear_button, on_clear_clicked, LV_EVENT_CLICKED, NULL);

    app_widgets.enter_button = lv_button_create(button_grid);
    lv_obj_set_grid_cell(app_widgets.enter_button, LV_GRID_ALIGN_STRETCH, 2, 1,
                        LV_GRID_ALIGN_STRETCH, 4, 1);
    lv_obj_t *enter_label = lv_label_create(app_widgets.enter_button);
    lv_label_set_text(enter_label, "Enter");
    lv_obj_set_style_text_font(enter_label, korean_font_14, 0);
    lv_obj_center(enter_label);
    lv_obj_add_event_cb(app_widgets.enter_button, on_enter_clicked, LV_EVENT_CLICKED, NULL);

    // Info label
    lv_obj_t *info_label = lv_label_create(main_cont);
    lv_label_set_text(info_label, "천지인 한글/영어/숫자/특수키 입력 방식");
    lv_obj_set_style_text_font(info_label, korean_font_12, 0);
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    // Set locale
    setlocale(LC_ALL, "");

    // Initialize LVGL
    lv_init();

    // Create SDL window and display
    lv_display_t *disp = lv_sdl_window_create(320, 640);
    if (disp == NULL) {
        printf("Failed to create SDL window!\n");
        return -1;
    }

    // Set window title
    lv_sdl_window_set_title(disp, "천지인 한글 입력기");

    // Create input devices
    lv_indev_t *mouse = lv_sdl_mouse_create();
    lv_indev_t *mousewheel = lv_sdl_mousewheel_create();
    lv_indev_t *keyboard = lv_sdl_keyboard_create();
    (void)mouse;
    (void)mousewheel;
    (void)keyboard;

    // Create UI
    create_ui();

    // Main loop - call lv_timer_handler periodically (every 5ms)
    while(1) {
        lv_timer_handler();
        usleep(5000);  // 5ms
    }

    return 0;
}
