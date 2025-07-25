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

// UTF-8 conversion function
static int wchar_to_utf8(wchar_t wc, char *utf8_buffer, size_t buffer_size) {
    if (wc == 0) {
        utf8_buffer[0] = '\0';
        return 0;
    }
    
    // Simple UTF-8 encoding for Korean characters
    if (wc < 0x80) {
        utf8_buffer[0] = (char)wc;
        utf8_buffer[1] = '\0';
        return 1;
    } else if (wc < 0x800) {
        utf8_buffer[0] = 0xC0 | ((wc >> 6) & 0x1F);
        utf8_buffer[1] = 0x80 | (wc & 0x3F);
        utf8_buffer[2] = '\0';
        return 2;
    } else if (wc < 0x10000) {
        utf8_buffer[0] = 0xE0 | ((wc >> 12) & 0x0F);
        utf8_buffer[1] = 0x80 | ((wc >> 6) & 0x3F);
        utf8_buffer[2] = 0x80 | (wc & 0x3F);
        utf8_buffer[3] = '\0';
        return 3;
    }
    
    utf8_buffer[0] = '\0';
    return 0;
}

// Update display function - show current composing character
static void update_display() {
    if (g_current_char_label) {
        wchar_t composing_char = get_composing_char();
        char composing_utf8[8] = "";
        if (composing_char != 0) {
            wchar_to_utf8(composing_char, composing_utf8, sizeof(composing_utf8));
            lv_label_set_text(g_current_char_label, composing_utf8);
        } else {
            lv_label_set_text(g_current_char_label, "");
        }
    }
}

// 천지인 자음 그룹 버튼 콜백 함수
static void chunjiin_consonant_group_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        int* group_index = (int*)lv_event_get_user_data(e);
        chunjiin_consonant_group_t group = (chunjiin_consonant_group_t)*group_index;
        
        printf("천지인: 자음 그룹 버튼 클릭 - group: %d\n", group);
        
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
        
        printf("DEBUG: Calling process_input with char: %c\n", input_char);
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
        printf("천지인: ㆍ(천) 버튼 클릭\n");
        process_input('a'); // ㆍ is 'a' character in process_input
        update_display();
    }
}

static void chunjiin_hbar_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("천지인: ㅡ(지) 버튼 클릭\n");
        process_input('e'); // ㅡ is 'e' character in process_input
        update_display();
    }
}

static void chunjiin_vbar_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("천지인: ㅣ(인) 버튼 클릭\n");
        process_input('i'); // ㅣ is 'i' character in process_input
        update_display();
    }
}

// 완성 버튼 콜백 함수
static void complete_syllable_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("천지인: Enter 버튼 클릭\n");
        // Use process_input with Enter key - need to check what character represents Enter
        process_input('\n'); // Try newline for Enter
        update_display();
    }
}

// 백스페이스 버튼 콜백 함수
static void backspace_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("천지인: 백스페이스 버튼 클릭\n");
        process_input('<'); // Backspace is '<' in process_input
        update_display();
    }
}

// 스페이스 버튼 콜백 함수
static void space_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("천지인: 스페이스 버튼 클릭\n");
        process_input(' '); // Space is ' ' in process_input
        update_display();
    }
}

// 키보드 이벤트 콜백 함수
static void keyboard_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_KEY) {
        uint32_t key = lv_event_get_key(e);
        printf("키보드 입력: %c (0x%02x)\n", (char)key, key);
        
        // Map keyboard keys to process_input characters
        if (key == LV_KEY_BACKSPACE) {
            printf("천지인: 키보드 백스페이스 키 입력\n");
            process_input('<');
        } else if (key == ' ') {
            printf("천지인: 키보드 스페이스 키 입력\n");
            process_input(' ');
        } else if (key == LV_KEY_ENTER) {
            printf("천지인: 키보드 Enter 키 입력\n");
            process_input('\n');
        } else {
            // For other keys, try to map them to process_input characters
            printf("천지인: 키보드 입력을 process_input으로 처리\n");
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
    lv_obj_set_style_bg_color(g_current_char_label, lv_color_make(255, 255, 0), 0); // Yellow background
    lv_obj_set_style_border_color(g_current_char_label, lv_color_make(255, 0, 0), 0); // Red border
    lv_obj_set_style_border_width(g_current_char_label, 3, 0); // Thicker border
    lv_obj_set_style_pad_all(g_current_char_label, 15, 0); // More padding
    lv_obj_set_style_text_font(g_current_char_label, korean_font, 0); // Apply Korean font
    lv_label_set_text(g_current_char_label, ""); // Initial empty text

    // Button dimensions and spacing
    int btn_width = 60, btn_height = 40, btn_spacing = 10;
    int start_x = 25; // Move buttons to the left
    int start_y = 90;

    // Row 1: 천지인 (vowels) - reordered: ㅣ, ㆍ, ㅡ
    lv_obj_t* vbar_btn = lv_btn_create(tab);
    lv_obj_set_size(vbar_btn, btn_width, btn_height);
    lv_obj_align(vbar_btn, LV_ALIGN_TOP_LEFT, start_x, start_y);
    lv_obj_t* vbar_label = lv_label_create(vbar_btn);
    lv_label_set_text(vbar_label, "ㅣ");
    lv_obj_set_style_text_font(vbar_label, korean_font, 0);
    lv_obj_center(vbar_label);
    lv_obj_add_event_cb(vbar_btn, chunjiin_vbar_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t* dot_btn = lv_btn_create(tab);
    lv_obj_set_size(dot_btn, btn_width, btn_height);
    lv_obj_align(dot_btn, LV_ALIGN_TOP_LEFT, start_x + (btn_width + btn_spacing), start_y);
    lv_obj_t* dot_label = lv_label_create(dot_btn);
    lv_label_set_text(dot_label, "ㆍ");
    lv_obj_set_style_text_font(dot_label, korean_font, 0);
    lv_obj_center(dot_label);
    lv_obj_add_event_cb(dot_btn, chunjiin_dot_cb, LV_EVENT_CLICKED, NULL);

    lv_obj_t* hbar_btn = lv_btn_create(tab);
    lv_obj_set_size(hbar_btn, btn_width, btn_height);
    lv_obj_align(hbar_btn, LV_ALIGN_TOP_LEFT, start_x + 2 * (btn_width + btn_spacing), start_y);
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
        lv_obj_align(btn, LV_ALIGN_TOP_LEFT, start_x + i * (btn_width + btn_spacing), start_y + (btn_height + btn_spacing));
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
        lv_obj_align(btn, LV_ALIGN_TOP_LEFT, start_x + (i-3) * (btn_width + btn_spacing), start_y + 2 * (btn_height + btn_spacing));
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
    lv_obj_align(space_btn, LV_ALIGN_TOP_LEFT, start_x, start_y + 3 * (btn_height + btn_spacing));
    lv_obj_t* space_label = lv_label_create(space_btn);
    lv_label_set_text(space_label, "Space");
    lv_obj_set_style_text_font(space_label, korean_font, 0);
    lv_obj_center(space_label);
    lv_obj_add_event_cb(space_btn, space_cb, LV_EVENT_CLICKED, NULL);

    // ㅇㅁ button
    lv_obj_t* ng_btn = lv_btn_create(tab);
    lv_obj_set_size(ng_btn, btn_width, btn_height);
    lv_obj_align(ng_btn, LV_ALIGN_TOP_LEFT, start_x + (btn_width + btn_spacing), start_y + 3 * (btn_height + btn_spacing));
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
    lv_obj_align(backspace_btn, LV_ALIGN_TOP_LEFT, start_x + 2 * (btn_width + btn_spacing), start_y + 3 * (btn_height + btn_spacing));
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
