#include "../include/menu.h"
#include "../include/config.h"
#include "../include/types.h"
#include "../include/style.h"
#include "../include/screen.h"

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

// ============================================================================
// MENU SCREEN COMPONENTS
// ============================================================================

static lv_obj_t *create_menu_title_bar(lv_obj_t *parent) {
    lv_obj_t *title_bar = lv_obj_create(parent);
    lv_obj_set_size(title_bar, SCREEN_WIDTH, TITLE_BAR_HEIGHT);
    lv_obj_align(title_bar, LV_ALIGN_TOP_MID, 0, 0);
    apply_bar_style(title_bar, COLOR_BG_TITLE);

    // Title label
    lv_obj_t *title_label = lv_label_create(title_bar);
    lv_label_set_text(title_label, "홈");
    apply_label_style(title_label);
    lv_obj_align(title_label, LV_ALIGN_CENTER, 0, 0);

    // Back button
    lv_obj_t *back_btn = lv_btn_create(title_bar);
    lv_obj_set_size(back_btn, BUTTON_WIDTH, BUTTON_HEIGHT);
    lv_obj_align(back_btn, LV_ALIGN_LEFT_MID, PADDING_HORIZONTAL, 0);
    apply_button_style(back_btn, COLOR_BUTTON_BACK);

    lv_obj_t *back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, "이전");
    apply_label_style(back_label);
    lv_obj_align(back_label, LV_ALIGN_CENTER, 0, 0);

    lv_obj_add_event_cb(back_btn, back_btn_callback, LV_EVENT_CLICKED, NULL);

    return title_bar;
}

static lv_obj_t *create_menu_content(lv_obj_t *parent) {
    lv_obj_t *content = lv_obj_create(parent);
    lv_obj_set_size(content, SCREEN_WIDTH, SCREEN_HEIGHT - TITLE_BAR_HEIGHT - STATUS_BAR_HEIGHT);
    lv_obj_align(content, LV_ALIGN_TOP_MID, 0, TITLE_BAR_HEIGHT);
    lv_obj_set_style_bg_color(content, lv_color_hex(COLOR_BG_DARK), 0);
    lv_obj_set_style_border_width(content, 0, 0);

    // Create menu buttons
    const char *menu_labels[] = {"메뉴 1", "메뉴 2", "메뉴 3", "메뉴 4"};

    for (int i = 0; i < MENU_ITEMS_COUNT; i++) {
        lv_obj_t *btn = lv_btn_create(content);
        lv_obj_set_size(btn, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
        lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, OFFSET_BUTTON_START_Y + i * (MENU_BUTTON_HEIGHT + MENU_BUTTON_MARGIN));
        apply_button_style(btn, COLOR_BUTTON_BG);

        lv_obj_t *label = lv_label_create(btn);
        lv_label_set_text(label, menu_labels[i]);
        apply_label_style(label);
        lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    }

    return content;
}

static lv_obj_t *create_menu_status_bar(lv_obj_t *parent) {
    lv_obj_t *status_bar = lv_obj_create(parent);
    lv_obj_set_size(status_bar, SCREEN_WIDTH, STATUS_BAR_HEIGHT);
    lv_obj_align(status_bar, LV_ALIGN_BOTTOM_MID, 0, 0);
    apply_bar_style(status_bar, COLOR_BG_TITLE);

    return status_bar;
}

// ============================================================================
// MENU SCREEN CREATION
// ============================================================================

void create_menu_screen(void) {
    lv_obj_t *menu_screen = lv_obj_create(NULL);
    lv_obj_set_size(menu_screen, SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_obj_set_style_bg_color(menu_screen, lv_color_hex(COLOR_BG_DARK), 0);

    create_menu_title_bar(menu_screen);
    create_menu_content(menu_screen);
    create_menu_status_bar(menu_screen);

    if (screen_stack_top + 1 < MAX_SCREENS) {
        screen_stack_top++;
        screen_stack[screen_stack_top].screen = menu_screen;
        screen_stack[screen_stack_top].screen_id = SCREEN_MENU;
    }

    lv_scr_load(menu_screen);
}
