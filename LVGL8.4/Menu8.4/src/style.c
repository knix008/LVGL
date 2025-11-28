#include "../include/style.h"
#include "../include/config.h"
#include "../include/types.h"

// ============================================================================
// STYLE HELPER FUNCTIONS
// ============================================================================

void apply_button_style(lv_obj_t *btn, uint32_t bg_color) {
    lv_obj_set_style_bg_color(btn, lv_color_hex(bg_color), 0);
    lv_obj_set_style_border_width(btn, 1, 0);
    lv_obj_set_style_border_color(btn, lv_color_hex(COLOR_BORDER), 0);
}

void apply_label_style(lv_obj_t *label) {
    extern AppState app_state;
    lv_obj_set_style_text_color(label, lv_color_hex(COLOR_TEXT), 0);
    if (app_state.font_20) {
        lv_obj_set_style_text_font(label, app_state.font_20, 0);
    }
}

void apply_bar_style(lv_obj_t *bar, uint32_t bg_color) {
    lv_obj_set_style_bg_color(bar, lv_color_hex(bg_color), 0);
    lv_obj_set_style_bg_opa(bar, COLOR_TRANSPARENT, 0);
    lv_obj_set_style_border_width(bar, 0, 0);
    lv_obj_set_style_pad_all(bar, 5, 0);
}
