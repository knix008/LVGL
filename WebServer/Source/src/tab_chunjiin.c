#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "lvgl.h"
#include "chunjiin_input.h"
#include "tab_chunjiin.h"
#include "ui_components.h"

// Forward declarations for chunjiin_input.c functions
extern void initialize();
extern void process_input(char key);
extern wchar_t get_composing_char();

// Static variables for display
static lv_obj_t * g_current_char_label = NULL;

// Update display function - show current composing character
static void update_display() {
    if (g_current_char_label) {
        wchar_t buffer[1024];
        char utf8_buffer[4096]; // UTF-8 needs more space than wchar_t
        chunjiin_get_current_text(buffer);
        
        // Convert wchar_t string to UTF-8
        size_t len = wcslen(buffer);
        size_t utf8_pos = 0;
        
        for (size_t i = 0; i < len && utf8_pos < sizeof(utf8_buffer) - 4; i++) {
            int bytes = wchar_to_utf8(buffer[i], &utf8_buffer[utf8_pos], sizeof(utf8_buffer) - utf8_pos);
            if (bytes > 0) {
                utf8_pos += bytes;
            }
        }
        utf8_buffer[utf8_pos] = '\0';
        
        lv_label_set_text(g_current_char_label, utf8_buffer);
    }
}

// 천지인 자음 그룹 버튼 콜백 함수
static void chunjiin_consonant_group_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        int* group_index = (int*)lv_event_get_user_data(e);
        chunjiin_consonant_group_t group = (chunjiin_consonant_group_t)*group_index;
        
        // Map group to character and call process_input
        char input_char;
        switch(group) {
            case CHUNJIIN_GIYEOK_KIYEOK: input_char = 'g'; break; // g: ㄱ → ㅋ → ㄲ
            case CHUNJIIN_NIEUN_RIEUL: input_char = 'n'; break;   // n: ㄴ → ㄹ
            case CHUNJIIN_DIGEUT_TIEUT: input_char = 'd'; break;  // d: ㄷ → ㅌ → ㄸ
            case CHUNJIIN_BIEUP_PIEUP: input_char = 'b'; break;   // b: ㅂ → ㅍ → ㅃ
            case CHUNJIIN_SIOT_HIEUT: input_char = 's'; break;    // s: ㅅ → ㅎ → ㅆ
            case CHUNJIIN_JIEUT_CHIEUT: input_char = 'j'; break;  // j: ㅈ → ㅊ → ㅉ
            case CHUNJIIN_IEUNG_MIEUM: input_char = 'm'; break;   // m: ㅇ → ㅁ
            default: return;
        }
        process_input(input_char);
        update_display();
        
        // 버튼 텍스트 업데이트를 위한 자음 텍스트 설정
        lv_obj_t * btn = lv_event_get_target(e);
        lv_obj_t * label = lv_obj_get_child(btn, 0);
        if (label) {
            const char* consonant_chars[] = {"ㄱㅋ", "ㄴㄹ", "ㄷㅌ", "ㅂㅍ", "ㅅㅎ", "ㅈㅊ", "ㅇㅁ"};
            if (group < 7) {
                lv_label_set_text(label, consonant_chars[group]);
            }
        }
    }
}

// 천지인 요소 버튼 콜백 함수들
static void chunjiin_dot_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        process_input('a'); // ㆍ is 'a' character in process_input
        update_display();
    }
}

static void chunjiin_hbar_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        process_input('e'); // ㅡ is 'e' character in process_input
        update_display();
    }
}

static void chunjiin_vbar_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        process_input('i'); // ㅣ is 'i' character in process_input
        update_display();
    }
}

// Callback function for closing popup dialog
static void close_dialog_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        // Clear the result when Close button is clicked
        chunjiin_enter_key_handler();
        update_display();
        
        // Close the dialog
        lv_obj_t * btn = lv_event_get_target(e);
        lv_obj_t * dialog = lv_obj_get_parent(btn);
        lv_obj_del(dialog);
    }
}

// 완성 버튼 콜백 함수
static void complete_syllable_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("천지인: Enter 버튼 클릭\n");
        
        // Get current text from the display label BEFORE processing
        const char* current_text = lv_label_get_text(g_current_char_label);
        
        // Create popup dialog to show the result BEFORE clearing
        lv_obj_t * parent = lv_obj_get_parent(lv_event_get_target(e));
        lv_obj_t * dialog = lv_obj_create(parent);
        lv_obj_set_size(dialog, 250, 150);
        lv_obj_align(dialog, LV_ALIGN_CENTER, 0, 0);
        lv_obj_set_style_bg_color(dialog, lv_color_white(), 0);
        lv_obj_set_style_bg_opa(dialog, LV_OPA_COVER, 0);
        lv_obj_set_style_border_width(dialog, 2, 0);
        lv_obj_set_style_border_color(dialog, lv_color_hex(0x333333), 0);
        lv_obj_set_style_radius(dialog, 10, 0);
        
        // Title label
        lv_obj_t * title_label = lv_label_create(dialog);
        lv_label_set_text(title_label, "ChunJiIn Input Result");
        lv_obj_set_style_text_font(title_label, &lv_font_montserrat_14, 0);
        lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 10);
        
        // Result text label
        lv_obj_t * popup_result_label = lv_label_create(dialog);
        lv_obj_set_style_text_font(popup_result_label, get_korean_font_small(), 0);
        lv_obj_set_style_text_color(popup_result_label, lv_color_hex(0x00AA00), 0); // Green text
        lv_obj_align(popup_result_label, LV_ALIGN_CENTER, 0, 0);
        
        char popup_text[300];
        snprintf(popup_text, sizeof(popup_text), "Result: %s", current_text);
        lv_label_set_text(popup_result_label, popup_text);
        
        // Close button
        lv_obj_t * close_btn = lv_btn_create(dialog);
        lv_obj_set_size(close_btn, 80, 30);
        lv_obj_align(close_btn, LV_ALIGN_BOTTOM_MID, 0, -10);
        lv_obj_t * close_label = lv_label_create(close_btn);
        lv_label_set_text(close_label, "Close");
        lv_obj_center(close_label);
        
        // Close button callback - pass the current text to be cleared when dialog closes
        lv_obj_add_event_cb(close_btn, close_dialog_cb, LV_EVENT_CLICKED, NULL);
        
        // Don't clear the result immediately - keep it visible until Close is clicked
        // The result will be cleared when the Close button is clicked
    }
}

// 백스페이스 버튼 콜백 함수
static void backspace_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        process_input('<'); // Backspace is '<' in process_input
        update_display();
    }
}

// 스페이스 버튼 콜백 함수
static void space_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        process_input(' '); // Space is ' ' in process_input
        update_display();
    }
}



// 키보드 이벤트 콜백 함수
static void keyboard_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_KEY) {
        uint32_t key = lv_event_get_key(e);
        
        // Map keyboard keys to process_input characters
        if (key == LV_KEY_BACKSPACE) {
            process_input('<');
        } else if (key == ' ') {
            process_input(' ');
        } else if (key == LV_KEY_ENTER) {
            process_input('\n');
        } else {
            // For other keys, try to map them to process_input characters
            process_input((char)key);
        }
        update_display();
    }
}

// 천지인 탭 생성 함수
lv_obj_t* create_chunjiin_tab(lv_obj_t* parent) {
    initialize();
    lv_font_t * korean_font = get_korean_font_small(); // Get font

    // Create tab object (parent container)
    lv_obj_t* tab = lv_obj_create(parent);
    lv_obj_set_size(tab, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_all(tab, 10, 0);

    // Create display label
    g_current_char_label = lv_label_create(tab);
    lv_obj_set_size(g_current_char_label, 200, 60); // Original size
    lv_obj_align(g_current_char_label, LV_ALIGN_TOP_MID, 0, 10); // Original position
    lv_obj_set_style_bg_color(g_current_char_label, lv_color_make(0, 255, 0), 0); // Green background
    lv_obj_set_style_bg_opa(g_current_char_label, LV_OPA_COVER, 0); // Make background fully opaque
    lv_obj_set_style_border_color(g_current_char_label, lv_color_make(128, 128, 128), 0); // Gray border
    lv_obj_set_style_border_width(g_current_char_label, 3, 0); // Thicker border
    lv_obj_set_style_pad_all(g_current_char_label, 15, 0); // More padding
    lv_obj_set_style_text_font(g_current_char_label, korean_font, 0); // Apply Korean font
    lv_obj_set_style_text_color(g_current_char_label, lv_color_make(0, 0, 0), 0); // Black text for better contrast
    lv_label_set_text(g_current_char_label, ""); // Initial empty text

    // Button dimensions and spacing
    int btn_width = 60, btn_height = 40, btn_spacing = 10;
    int start_y = 90;

    // Row 1: 천지인 (vowels) - reordered: ㅣ, ㆍ, ㅡ
    lv_obj_t* vbar_btn = lv_btn_create(tab);
    lv_obj_set_size(vbar_btn, btn_width, btn_height);
    lv_obj_align(vbar_btn, LV_ALIGN_TOP_MID, -(btn_width + btn_spacing), start_y);
    lv_obj_t* vbar_label = lv_label_create(vbar_btn);
    lv_label_set_text(vbar_label, "ㅣ");
    lv_obj_set_style_text_font(vbar_label, korean_font, 0);
    lv_obj_center(vbar_label);
    lv_obj_add_event_cb(vbar_btn, chunjiin_vbar_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t* dot_btn = lv_btn_create(tab);
    lv_obj_set_size(dot_btn, btn_width, btn_height);
    lv_obj_align(dot_btn, LV_ALIGN_TOP_MID, 0, start_y);
    lv_obj_t* dot_label = lv_label_create(dot_btn);
    lv_label_set_text(dot_label, "ㆍ");
    lv_obj_set_style_text_font(dot_label, korean_font, 0);
    lv_obj_center(dot_label);
    lv_obj_add_event_cb(dot_btn, chunjiin_dot_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t* hbar_btn = lv_btn_create(tab);
    lv_obj_set_size(hbar_btn, btn_width, btn_height);
    lv_obj_align(hbar_btn, LV_ALIGN_TOP_MID, (btn_width + btn_spacing), start_y);
    lv_obj_t* hbar_label = lv_label_create(hbar_btn);
    lv_label_set_text(hbar_label, "ㅡ");
    lv_obj_set_style_text_font(hbar_label, korean_font, 0);
    lv_obj_center(hbar_label);
    lv_obj_add_event_cb(hbar_btn, chunjiin_hbar_cb, LV_EVENT_CLICKED, NULL);

    // Row 2: Consonant groups 1-3
    const char* consonant_labels[] = {"ㄱㅋ", "ㄴㄹ", "ㄷㅌ", "ㅂㅍ", "ㅅㅎ", "ㅈㅊ", "ㅇㅁ"};
    for (int i = 0; i < 3; i++) {
        lv_obj_t* btn = lv_btn_create(tab);
        lv_obj_set_size(btn, btn_width, btn_height);
        lv_obj_align(btn, LV_ALIGN_TOP_MID, (i - 1) * (btn_width + btn_spacing), start_y + (btn_height + btn_spacing));
        lv_obj_t* label = lv_label_create(btn);
        lv_label_set_text(label, consonant_labels[i]);
        lv_obj_set_style_text_font(label, korean_font, 0);
        lv_obj_center(label);
        int* group_index = malloc(sizeof(int));
        *group_index = i;
        lv_obj_add_event_cb(btn, chunjiin_consonant_group_cb, LV_EVENT_CLICKED, group_index);
    }

    // Row 3: Consonant groups 4-6
    for (int i = 3; i < 6; i++) {
        lv_obj_t* btn = lv_btn_create(tab);
        lv_obj_set_size(btn, btn_width, btn_height);
        lv_obj_align(btn, LV_ALIGN_TOP_MID, (i - 4) * (btn_width + btn_spacing), start_y + 2 * (btn_height + btn_spacing));
        lv_obj_t* label = lv_label_create(btn);
        lv_label_set_text(label, consonant_labels[i]);
        lv_obj_set_style_text_font(label, korean_font, 0);
        lv_obj_center(label);
        int* group_index = malloc(sizeof(int));
        *group_index = i;
        lv_obj_add_event_cb(btn, chunjiin_consonant_group_cb, LV_EVENT_CLICKED, group_index);
    }

    // Row 4: Space, ㅇㅁ, Backspace
    // Space button
    lv_obj_t* space_btn = lv_btn_create(tab);
    lv_obj_set_size(space_btn, btn_width, btn_height);
    lv_obj_align(space_btn, LV_ALIGN_TOP_MID, -(btn_width + btn_spacing), start_y + 3 * (btn_height + btn_spacing));
    lv_obj_t* space_label = lv_label_create(space_btn);
    lv_label_set_text(space_label, "Space");
    lv_obj_set_style_text_font(space_label, korean_font, 0);
    lv_obj_center(space_label);
    lv_obj_add_event_cb(space_btn, space_cb, LV_EVENT_CLICKED, NULL);

    // ㅇㅁ button
    lv_obj_t* ng_btn = lv_btn_create(tab);
    lv_obj_set_size(ng_btn, btn_width, btn_height);
    lv_obj_align(ng_btn, LV_ALIGN_TOP_MID, 0, start_y + 3 * (btn_height + btn_spacing));
    lv_obj_t* ng_label = lv_label_create(ng_btn);
    lv_label_set_text(ng_label, consonant_labels[6]); // "ㅇㅁ"
    lv_obj_set_style_text_font(ng_label, korean_font, 0);
    lv_obj_center(ng_label);
    int* group_index_ng = malloc(sizeof(int));
    *group_index_ng = 6;
    lv_obj_add_event_cb(ng_btn, chunjiin_consonant_group_cb, LV_EVENT_CLICKED, group_index_ng);

    // Backspace button
    lv_obj_t* backspace_btn = lv_btn_create(tab);
    lv_obj_set_size(backspace_btn, btn_width, btn_height);
    lv_obj_align(backspace_btn, LV_ALIGN_TOP_MID, (btn_width + btn_spacing), start_y + 3 * (btn_height + btn_spacing));
    lv_obj_t* backspace_label = lv_label_create(backspace_btn);
    lv_label_set_text(backspace_label, "←");
    lv_obj_set_style_text_font(backspace_label, korean_font, 0);
    lv_obj_center(backspace_label);
    lv_obj_add_event_cb(backspace_btn, backspace_cb, LV_EVENT_CLICKED, NULL);

    // Row 5: Enter button (centered)
    lv_obj_t* complete_btn = lv_btn_create(tab);
    lv_obj_set_size(complete_btn, 80, 40);
    lv_obj_align(complete_btn, LV_ALIGN_TOP_MID, 0, start_y + 4 * (btn_height + btn_spacing));
    lv_obj_t* complete_label = lv_label_create(complete_btn);
    lv_label_set_text(complete_label, "Enter");
    lv_obj_set_style_text_font(complete_label, korean_font, 0);
    lv_obj_center(complete_label);
    lv_obj_add_event_cb(complete_btn, complete_syllable_cb, LV_EVENT_CLICKED, NULL);

    // Keyboard event handler
    lv_obj_add_event_cb(tab, keyboard_event_cb, LV_EVENT_KEY, NULL);
    
    return tab;
}
