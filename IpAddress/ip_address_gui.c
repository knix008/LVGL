/**
 * @file ip_address_gui.c
 * @brief IP Address Input GUI Implementation using LVGL
 * @details Supports both IPv4 and IPv6 input modes with validation
 */

#include "ip_address_gui.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/* Global GUI state */
static ip_gui_state_t *g_gui_state = NULL;

/* IPv4 keyboard map */
static const char *ipv4_kb_map[] = {
    "1", "2", "3", "\n",
    "4", "5", "6", "\n", 
    "7", "8", "9", "\n",
    ".", "0", LV_SYMBOL_BACKSPACE, "\n",
    " ", " ", LV_SYMBOL_NEW_LINE, ""
};

static const lv_buttonmatrix_ctrl_t ipv4_kb_ctrl[] = {
    1, 1, 1,
    1, 1, 1,
    1, 1, 1,
    1, 1, 1,
    LV_BUTTONMATRIX_CTRL_HIDDEN, LV_BUTTONMATRIX_CTRL_HIDDEN, 1
};

/* IPv6 keyboard map */
static const char *ipv6_kb_map[] = {
    "1", "2", "3", "\n",
    "4", "5", "6", "\n",
    "7", "8", "9", "\n",
    "a", "b", "c", "\n",
    "d", "e", "f", "\n",
    "0", ":", LV_SYMBOL_BACKSPACE, "\n",
    " ", " ", LV_SYMBOL_NEW_LINE, ""
};

static const lv_buttonmatrix_ctrl_t ipv6_kb_ctrl[] = {
    1, 1, 1,
    1, 1, 1,
    1, 1, 1,
    1, 1, 1,
    1, 1, 1,
    1, 1, 1,
    LV_BUTTONMATRIX_CTRL_HIDDEN, LV_BUTTONMATRIX_CTRL_HIDDEN, 1
};

/* Function Implementations */

ip_gui_state_t* ip_gui_init(void) {
    if (g_gui_state != NULL) {
        return g_gui_state;
    }
    
    g_gui_state = lv_malloc(sizeof(ip_gui_state_t));
    if (g_gui_state == NULL) {
        return NULL;
    }
    
    memset(g_gui_state, 0, sizeof(ip_gui_state_t));
    g_gui_state->current_mode = IP_MODE_IPV4;
    g_gui_state->is_valid = false;
    
    return g_gui_state;
}

void ip_gui_create_ui(ip_gui_state_t *state) {
    if (state == NULL) return;
    
    /* Create main screen */
    state->main_screen = lv_obj_create(NULL);
    lv_obj_set_size(state->main_screen, WINDOW_WIDTH, WINDOW_HEIGHT);
    lv_obj_center(state->main_screen);
    lv_obj_set_style_bg_color(state->main_screen, lv_color_hex(0xf0f0f0), 0);
    lv_obj_set_style_pad_all(state->main_screen, MARGIN, 0);
    
    /* Create title label */
    state->title_label = lv_label_create(state->main_screen);
    lv_label_set_text(state->title_label, "IPv4/IPv6 Address Input");
    lv_obj_set_style_text_font(state->title_label, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_align(state->title_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(state->title_label, LV_ALIGN_TOP_MID, 0, 10);
    
    /* Create mode switch */
    state->mode_switch = lv_switch_create(state->main_screen);
    lv_obj_set_size(state->mode_switch, 60, 30);
    lv_obj_align(state->mode_switch, LV_ALIGN_TOP_RIGHT, -10, 50);
    lv_obj_add_event_cb(state->mode_switch, ip_gui_mode_switch_event_cb, LV_EVENT_VALUE_CHANGED, state);
    
    /* Create mode label */
    state->mode_label = lv_label_create(state->main_screen);
    lv_label_set_text(state->mode_label, "IPv4");
    lv_obj_align_to(state->mode_label, state->mode_switch, LV_ALIGN_OUT_LEFT_MID, -10, 0);
    
    /* Create single IP input text area */
    state->ip_textarea = lv_textarea_create(state->main_screen);
    lv_obj_set_size(state->ip_textarea, WINDOW_WIDTH - 2 * MARGIN, TEXT_AREA_HEIGHT);
    lv_obj_align(state->ip_textarea, LV_ALIGN_TOP_MID, 0, 100);
    lv_textarea_set_one_line(state->ip_textarea, true);
    lv_textarea_set_text(state->ip_textarea, "");
    lv_textarea_set_placeholder_text(state->ip_textarea, "e.g. 192.168.1.1");
    lv_textarea_set_max_length(state->ip_textarea, MAX_IPV4_LEN);  /* Start with IPv4 max */
    
    /* Create validate button */
    state->validate_btn = lv_button_create(state->main_screen);
    lv_obj_set_size(state->validate_btn, 120, BUTTON_HEIGHT);
    lv_obj_align(state->validate_btn, LV_ALIGN_TOP_MID, -70, 170);
    lv_obj_t *validate_label = lv_label_create(state->validate_btn);
    lv_label_set_text(validate_label, "Validate");
    lv_obj_center(validate_label);
    lv_obj_add_event_cb(state->validate_btn, ip_gui_validate_event_cb, LV_EVENT_CLICKED, state);
    
    /* Create clear button */
    state->clear_btn = lv_button_create(state->main_screen);
    lv_obj_set_size(state->clear_btn, 120, BUTTON_HEIGHT);
    lv_obj_align(state->clear_btn, LV_ALIGN_TOP_MID, 70, 170);
    lv_obj_t *clear_label = lv_label_create(state->clear_btn);
    lv_label_set_text(clear_label, "Clear");
    lv_obj_center(clear_label);
    lv_obj_add_event_cb(state->clear_btn, ip_gui_clear_event_cb, LV_EVENT_CLICKED, state);
    
    /* Create result label */
    state->result_label = lv_label_create(state->main_screen);
    lv_label_set_text(state->result_label, "");
    lv_obj_set_style_text_align(state->result_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(state->result_label, LV_ALIGN_TOP_MID, 0, 230);
    lv_obj_set_style_text_color(state->result_label, lv_color_hex(0x333333), 0);
    
    /* Create keyboard */
    state->keyboard = lv_keyboard_create(state->main_screen);
    lv_keyboard_set_map(state->keyboard, LV_KEYBOARD_MODE_USER_1, ipv4_kb_map, ipv4_kb_ctrl);
    lv_keyboard_set_mode(state->keyboard, LV_KEYBOARD_MODE_USER_1);
    lv_obj_set_size(state->keyboard, WINDOW_WIDTH - 2 * MARGIN, 280);  /* Fixed height for both modes */
    lv_obj_align(state->keyboard, LV_ALIGN_TOP_MID, 0, 270);  /* Position from top instead of bottom */
    
    /* Set keyboard to control the textarea */
    lv_keyboard_set_textarea(state->keyboard, state->ip_textarea);
    
    /* Add event handler for Enter key */
    lv_obj_add_event_cb(state->keyboard, ip_gui_keyboard_event_cb, LV_EVENT_VALUE_CHANGED, state);
    
    /* Keep keyboard visible always */
    lv_obj_remove_flag(state->keyboard, LV_OBJ_FLAG_HIDDEN);
}

void ip_gui_switch_mode(ip_gui_state_t *state, ip_input_mode_t mode) {
    if (state == NULL) return;
    
    state->current_mode = mode;
    
    if (mode == IP_MODE_IPV4) {
        /* Switch to IPv4 mode */
        lv_label_set_text(state->mode_label, "IPv4");
        lv_keyboard_set_map(state->keyboard, LV_KEYBOARD_MODE_USER_1, ipv4_kb_map, ipv4_kb_ctrl);
        lv_textarea_set_placeholder_text(state->ip_textarea, "e.g. 192.168.1.1");
        lv_textarea_set_max_length(state->ip_textarea, MAX_IPV4_LEN);
    } else {
        /* Switch to IPv6 mode */
        lv_label_set_text(state->mode_label, "IPv6");
        lv_keyboard_set_map(state->keyboard, LV_KEYBOARD_MODE_USER_1, ipv6_kb_map, ipv6_kb_ctrl);
        lv_textarea_set_placeholder_text(state->ip_textarea, "e.g. 2001:0db8:85a3:0000:0000:8a2e:0370:7334");
        lv_textarea_set_max_length(state->ip_textarea, MAX_IPV6_LEN);
    }
    
    /* Clear current input */
    ip_gui_clear_input(state);
}

void ip_gui_clear_input(ip_gui_state_t *state) {
    if (state == NULL) return;
    
    state->is_valid = false;
    
    /* Clear the text area */
    if (state->ip_textarea != NULL) {
        lv_textarea_set_text(state->ip_textarea, "");
    }
    
    lv_label_set_text(state->result_label, "");
}

void ip_gui_textarea_event_cb(lv_event_t *e) {
    /* No longer needed - keyboard is always attached to the single textarea */
    (void)e;
}

void ip_gui_keyboard_event_cb(lv_event_t *e) {
    lv_obj_t *kb = lv_event_get_target(e);
    ip_gui_state_t *state = (ip_gui_state_t *)lv_event_get_user_data(e);
    
    if (state == NULL) return;
    
    /* Check if Enter/OK button was pressed */
    uint16_t btn_id = lv_keyboard_get_selected_button(kb);
    if (btn_id == LV_BUTTONMATRIX_BUTTON_NONE) return;
    
    const char *btn_text = lv_keyboard_get_button_text(kb, btn_id);
    if (btn_text == NULL) return;
    
    /* If Enter button was pressed, validate and show popup */
    if (strcmp(btn_text, LV_SYMBOL_NEW_LINE) == 0) {
        const char *ip_address = lv_textarea_get_text(state->ip_textarea);
        
        if (ip_address == NULL || strlen(ip_address) == 0) {
            ip_gui_show_result_popup(state, false, "");
            return;
        }
        
        /* Validate the IP address */
        bool is_valid = ip_gui_validate_address(ip_address, state->current_mode);
        state->is_valid = is_valid;
        
        /* Show result in popup */
        ip_gui_show_result_popup(state, is_valid, ip_address);
    }
}

void ip_gui_show_result_popup(ip_gui_state_t *state, bool is_valid, const char *ip_address) {
    if (state == NULL) return;
    
    const char *title;
    char message[200];
    
    if (ip_address == NULL || strlen(ip_address) == 0) {
        title = "Error";
        snprintf(message, sizeof(message), "Please enter an IP address");
    } else if (is_valid) {
        title = "Valid IP Address";
        const char *mode_str = (state->current_mode == IP_MODE_IPV4) ? "IPv4" : "IPv6";
        snprintf(message, sizeof(message), "Valid %s address:\n\n%s", mode_str, ip_address);
    } else {
        title = "Invalid IP Address";
        const char *mode_str = (state->current_mode == IP_MODE_IPV4) ? "IPv4" : "IPv6";
        snprintf(message, sizeof(message), "Invalid %s address:\n\n%s", mode_str, ip_address);
    }
    
    /* Create message box popup */
    lv_obj_t *mbox = lv_msgbox_create(NULL);
    lv_msgbox_add_title(mbox, title);
    lv_msgbox_add_text(mbox, message);
    lv_msgbox_add_close_button(mbox);
    lv_obj_center(mbox);
}

void ip_gui_mode_switch_event_cb(lv_event_t *e) {
    lv_obj_t *sw = lv_event_get_target(e);
    ip_gui_state_t *state = (ip_gui_state_t *)lv_event_get_user_data(e);
    
    if (state == NULL) return;
    
    bool is_on = lv_obj_has_state(sw, LV_STATE_CHECKED);
    ip_input_mode_t new_mode = is_on ? IP_MODE_IPV6 : IP_MODE_IPV4;
    
    ip_gui_switch_mode(state, new_mode);
}

void ip_gui_validate_event_cb(lv_event_t *e) {
    ip_gui_state_t *state = (ip_gui_state_t *)lv_event_get_user_data(e);
    
    if (state == NULL) return;
    
    /* Get IP address from the single text area */
    const char *ip_address = lv_textarea_get_text(state->ip_textarea);
    
    if (ip_address == NULL || strlen(ip_address) == 0) {
        ip_gui_update_result(state, false, "Please enter an IP address");
        return;
    }
    
    /* Validate the IP address */
    bool is_valid = ip_gui_validate_address(ip_address, state->current_mode);
    state->is_valid = is_valid;
    
    if (is_valid) {
        char formatted[MAX_IPV6_LEN + 50];
        ip_gui_format_address(ip_address, state->current_mode, formatted, sizeof(formatted));
        ip_gui_update_result(state, true, formatted);
    } else {
        const char *mode_str = (state->current_mode == IP_MODE_IPV4) ? "IPv4" : "IPv6";
        char error_msg[100];
        snprintf(error_msg, sizeof(error_msg), "Invalid %s address", mode_str);
        ip_gui_update_result(state, false, error_msg);
    }
}

void ip_gui_clear_event_cb(lv_event_t *e) {
    ip_gui_state_t *state = (ip_gui_state_t *)lv_event_get_user_data(e);
    
    if (state == NULL) return;
    
    ip_gui_clear_input(state);
}

void ip_gui_update_result(ip_gui_state_t *state, bool is_valid, const char *message) {
    if (state == NULL || state->result_label == NULL) return;
    
    lv_label_set_text(state->result_label, message);
    
    if (is_valid) {
        lv_obj_set_style_text_color(state->result_label, lv_color_hex(0x00aa00), 0);
    } else {
        lv_obj_set_style_text_color(state->result_label, lv_color_hex(0xaa0000), 0);
    }
}

bool ip_gui_validate_address(const char *ip_str, ip_input_mode_t mode) {
    if (ip_str == NULL || strlen(ip_str) == 0) {
        return false;
    }
    
    if (mode == IP_MODE_IPV4) {
        return ipv4_validate(ip_str);
    } else {
        return ipv6_validate(ip_str);
    }
}

void ip_gui_format_address(const char *ip_str, ip_input_mode_t mode, 
                          char *formatted, size_t max_len) {
    if (ip_str == NULL || formatted == NULL || max_len == 0) {
        return;
    }
    
    if (mode == IP_MODE_IPV4) {
        snprintf(formatted, max_len, "Valid IPv4: %s", ip_str);
    } else {
        snprintf(formatted, max_len, "Valid IPv6: %s", ip_str);
    }
}

void ip_gui_cleanup(ip_gui_state_t *state) {
    if (state == NULL) return;
    
    if (state->main_screen != NULL) {
        lv_obj_del(state->main_screen);
    }
    
    lv_free(state);
    g_gui_state = NULL;
}
