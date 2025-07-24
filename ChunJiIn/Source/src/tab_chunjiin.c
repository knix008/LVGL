#include "tab_chunjiin.h"
#include "chunjiin_input.h"
#include "ui_components.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// 전역 변수
static lv_obj_t * input_label = NULL;
static lv_obj_t * output_label = NULL;

// 천지인 자음 그룹 토글 상태 (외부에서 선언된 변수)
extern int group_toggle_states[7];

// UI 업데이트 함수
static void update_chunjiin_display() {
    if (!input_label || !output_label) {
        return;
    }
    
    // 출력 버퍼의 텍스트를 가져와서 화면에 표시
    const char* output_text = chunjiin_get_current_text();
    
    // 입력 라벨에는 현재 조합 중인 상태 표시
    char input_display[300];
    if (strlen(chunjiin_global_state.hangul.chosung) > 0 || 
        strlen(chunjiin_global_state.hangul.jungsung) > 0 || 
        strlen(chunjiin_global_state.hangul.jongsung) > 0) {
        snprintf(input_display, sizeof(input_display), "조합 중: %s%s%s", 
                chunjiin_global_state.hangul.chosung,
                chunjiin_global_state.hangul.jungsung,
                chunjiin_global_state.hangul.jongsung);
    } else {
        snprintf(input_display, sizeof(input_display), "조합 중: ");
    }
    lv_label_set_text(input_label, input_display);
    
    // 출력 텍스트 표시
    char output_display[300];
    snprintf(output_display, sizeof(output_display), "결과: %s", output_text);
    lv_label_set_text(output_label, output_display);
    
    // Force refresh
    lv_obj_invalidate(input_label);
    lv_obj_invalidate(output_label);
    
    // Print current Korean character to terminal
    if (strlen(output_text) > 0) {
        printf("현재 결과 텍스트: %s\n", output_text);
    }
}

// 천지인 표준 자음 그룹 버튼 콜백 (토글 방식)
static void chunjiin_consonant_group_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        int* group_index = (int*)lv_event_get_user_data(e);
        chunjiin_consonant_group_t group = (chunjiin_consonant_group_t)*group_index;
        
        printf("천지인: 자음 그룹 버튼 클릭 - group: %d, current state: %d\n", 
               group, group_toggle_states[group]);
        
        // Call input function (handles cycling internally)
        chunjiin_input_consonant_group(group);
        
        printf("천지인: 자음 그룹 버튼 클릭 - group: %d, new state: %d\n", 
               group, group_toggle_states[group]);
        
        update_chunjiin_display();
        
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
        chunjiin_input_element(CHUNJIIN_DOT);
        update_chunjiin_display();
    }
}

static void chunjiin_hbar_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("천지인: ㅡ(지) 버튼 클릭\n");
        chunjiin_input_element(CHUNJIIN_EU);
        update_chunjiin_display();
    }
}

static void chunjiin_vbar_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("천지인: ㅣ(인) 버튼 클릭\n");
        chunjiin_input_element(CHUNJIIN_I);
        update_chunjiin_display();
    }
}

// 기능 버튼 콜백 함수들
static void complete_syllable_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("천지인: Enter 버튼 클릭\n");
        
        // 현재 조합 중인 문자를 먼저 완성하여 출력 버퍼에 추가
        chunjiin_enter();
        
        // 출력 버퍼의 완성된 결과를 가져와서 터미널에 출력
        const char* output_result = chunjiin_get_current_text();
        printf("=== 천지인 입력 결과 ===\n");
        printf("출력 버퍼: %s\n", output_result);
        printf("====================\n");
        
        update_chunjiin_display();
    }
}

static void backspace_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("천지인: 백스페이스 버튼 클릭\n");
        chunjiin_backspace();
        update_chunjiin_display();
    }
}

static void space_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("천지인: 스페이스 버튼 클릭\n");
        chunjiin_enter();  // 현재 문자 완성
        chunjiin_input_keyboard(' ');  // 스페이스 추가
        update_chunjiin_display();
    }
}

static void clear_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("천지인: 클리어 버튼 클릭\n");
        chunjiin_clear_all_buffers();
        update_chunjiin_display();
    }
}

// 기호 버튼 콜백
static void symbol_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        char* symbol = (char*)lv_event_get_user_data(e);
        printf("천지인: 기호 버튼 클릭 - %s\n", symbol);
        chunjiin_input_keyboard(symbol[0]);
        update_chunjiin_display();
    }
}

// 모드 전환 버튼 콜백
static void mode_toggle_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("천지인: 모드 전환 버튼 클릭\n");
        chunjiin_toggle_input_mode();
        
        // 버튼 텍스트 업데이트
        lv_obj_t * btn = lv_event_get_target(e);
        lv_obj_t * label = lv_obj_get_child(btn, 0);
        if (label) {
            input_mode_t current_mode = chunjiin_get_input_mode();
            if (current_mode == INPUT_MODE_KOREAN) {
                lv_label_set_text(label, "한글");
            } else {
                lv_label_set_text(label, "숫자");
            }
        }
        
        update_chunjiin_display();
    }
}

// 키보드 이벤트 핸들러
static void keyboard_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_KEY) {
        uint32_t key = lv_event_get_key(e);
        
        // 소문자로 변환
        char key_char = tolower(key);
        
        // 천지인 키보드 입력 처리
        chunjiin_input_keyboard(key_char);
        update_chunjiin_display();
    }
}

// 천지인 탭 생성
void create_chunjiin_tab(lv_obj_t * parent) {
    // 천지인 상태 초기화
    chunjiin_init();
    
    // 한글 폰트 가져오기 (작은 폰트 사용)
    lv_font_t * korean_font = get_korean_font_small();
    
    // 타이틀 라벨
    lv_obj_t * title_label = lv_label_create(parent);
    lv_label_set_text(title_label, "ChunJiIn Input System");
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 10);
    
    // 입력 결과 표시 영역
    input_label = lv_label_create(parent);
    lv_label_set_text(input_label, "입력: 입력 대기...");
    lv_obj_set_style_text_font(input_label, korean_font, 0);
    lv_obj_set_style_bg_color(input_label, lv_color_hex(0xe0ffe0), 0); // 연한 초록색
    lv_obj_set_style_bg_opa(input_label, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(input_label, 1, 0);
    lv_obj_set_style_pad_all(input_label, 5, 0);
    lv_obj_set_size(input_label, 260, 30);
    lv_obj_align(input_label, LV_ALIGN_TOP_MID, 0, 40);
    lv_label_set_long_mode(input_label, LV_LABEL_LONG_WRAP);
    
    // 출력 결과 표시 영역
    output_label = lv_label_create(parent);
    lv_label_set_text(output_label, "결과: ");
    lv_obj_set_style_text_font(output_label, korean_font, 0);
    lv_obj_set_style_bg_color(output_label, lv_color_hex(0xffe0e0), 0); // 연한 빨간색
    lv_obj_set_style_bg_opa(output_label, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(output_label, 1, 0);
    lv_obj_set_style_pad_all(output_label, 5, 0);
    lv_obj_set_size(output_label, 260, 30);
    lv_obj_align(output_label, LV_ALIGN_TOP_MID, 0, 75);
    lv_label_set_long_mode(output_label, LV_LABEL_LONG_WRAP);
    
    // 버튼 크기와 간격 설정
    int btn_width = 45;
    int btn_height = 25;
    int btn_spacing = 3;
    int start_y = 5; // 시작 Y 위치
    
    // 첫 번째 줄: 천지인 기본 요소들 (ㅣ, ㆍ, ㅡ)
    // ㅣ (인) 버튼
    lv_obj_t * vbar_btn = lv_btn_create(parent);
    lv_obj_set_size(vbar_btn, btn_width, btn_height);
    lv_obj_align(vbar_btn, LV_ALIGN_CENTER, -(btn_width + btn_spacing), start_y);
    lv_obj_t * vbar_label = lv_label_create(vbar_btn);
    lv_label_set_text(vbar_label, "ㅣ");
    lv_obj_set_style_text_font(vbar_label, korean_font, 0);
    lv_obj_center(vbar_label);
    lv_obj_add_event_cb(vbar_btn, chunjiin_vbar_cb, LV_EVENT_CLICKED, NULL);
    
    // ㆍ (천) 버튼
    lv_obj_t * dot_btn = lv_btn_create(parent);
    lv_obj_set_size(dot_btn, btn_width, btn_height);
    lv_obj_align(dot_btn, LV_ALIGN_CENTER, 0, start_y);
    lv_obj_t * dot_label = lv_label_create(dot_btn);
    lv_label_set_text(dot_label, "ㆍ");
    lv_obj_set_style_text_font(dot_label, korean_font, 0);
    lv_obj_center(dot_label);
    lv_obj_add_event_cb(dot_btn, chunjiin_dot_cb, LV_EVENT_CLICKED, NULL);
    
    // ㅡ (지) 버튼
    lv_obj_t * hbar_btn = lv_btn_create(parent);
    lv_obj_set_size(hbar_btn, btn_width, btn_height);
    lv_obj_align(hbar_btn, LV_ALIGN_CENTER, btn_width + btn_spacing, start_y);
    lv_obj_t * hbar_label = lv_label_create(hbar_btn);
    lv_label_set_text(hbar_label, "ㅡ");
    lv_obj_set_style_text_font(hbar_label, korean_font, 0);
    lv_obj_center(hbar_label);
    lv_obj_add_event_cb(hbar_btn, chunjiin_hbar_cb, LV_EVENT_CLICKED, NULL);
    
    // 두 번째 줄: 자음 그룹들 (ㄱㅋ, ㄴㄹ, ㄷㅌ)
    start_y += btn_height + btn_spacing;
    
    // 자음 그룹 인덱스
    static int group_indices1[] = {
        CHUNJIIN_GIYEOK_KIYEOK,
        CHUNJIIN_NIEUN_RIEUL, 
        CHUNJIIN_DIGEUT_TIEUT
    };
    
    const char* group_labels1[] = {"ㄱㅋ", "ㄴㄹ", "ㄷㅌ"};
    
    for (int i = 0; i < 3; i++) {
        lv_obj_t * btn = lv_btn_create(parent);
        lv_obj_set_size(btn, btn_width, btn_height);
        lv_obj_align(btn, LV_ALIGN_CENTER, (i - 1) * (btn_width + btn_spacing), start_y);
        lv_obj_t * label = lv_label_create(btn);
        lv_label_set_text(label, group_labels1[i]);
        lv_obj_set_style_text_font(label, korean_font, 0);
        lv_obj_center(label);
        lv_obj_add_event_cb(btn, chunjiin_consonant_group_cb, LV_EVENT_CLICKED, &group_indices1[i]);
    }
    
    // 세 번째 줄: 자음 그룹들 (ㅂㅍ, ㅅㅎ, ㅈㅊ)
    start_y += btn_height + btn_spacing;
    
    static int group_indices2[] = {
        CHUNJIIN_BIEUP_PIEUP,
        CHUNJIIN_SIOT_HIEUT,
        CHUNJIIN_JIEUT_CHIEUT
    };
    
    const char* group_labels2[] = {"ㅂㅍ", "ㅅㅎ", "ㅈㅊ"};
    
    for (int i = 0; i < 3; i++) {
        lv_obj_t * btn = lv_btn_create(parent);
        lv_obj_set_size(btn, btn_width, btn_height);
        lv_obj_align(btn, LV_ALIGN_CENTER, (i - 1) * (btn_width + btn_spacing), start_y);
        lv_obj_t * label = lv_label_create(btn);
        lv_label_set_text(label, group_labels2[i]);
        lv_obj_set_style_text_font(label, korean_font, 0);
        lv_obj_center(label);
        lv_obj_add_event_cb(btn, chunjiin_consonant_group_cb, LV_EVENT_CLICKED, &group_indices2[i]);
    }
    
    // 네 번째 줄: 기능 버튼들 (backspace, ㅇㅁ, enter)
    start_y += btn_height + btn_spacing;
    
    // Backspace 버튼
    lv_obj_t * backspace_btn = lv_btn_create(parent);
    lv_obj_set_size(backspace_btn, btn_width, btn_height);
    lv_obj_align(backspace_btn, LV_ALIGN_CENTER, -(btn_width + btn_spacing), start_y);
    lv_obj_t * backspace_label = lv_label_create(backspace_btn);
    lv_label_set_text(backspace_label, "←");
    lv_obj_center(backspace_label);
    lv_obj_add_event_cb(backspace_btn, backspace_cb, LV_EVENT_CLICKED, NULL);
    
    // ㅇㅁ 버튼 (중앙에 배치)
    static int ieung_mieum_index = CHUNJIIN_IEUNG_MIEUM;
    lv_obj_t * ieung_mieum_btn = lv_btn_create(parent);
    lv_obj_set_size(ieung_mieum_btn, btn_width, btn_height);
    lv_obj_align(ieung_mieum_btn, LV_ALIGN_CENTER, 0, start_y);
    lv_obj_t * ieung_mieum_label = lv_label_create(ieung_mieum_btn);
    lv_label_set_text(ieung_mieum_label, "ㅇㅁ");
    lv_obj_set_style_text_font(ieung_mieum_label, korean_font, 0);
    lv_obj_center(ieung_mieum_label);
    lv_obj_add_event_cb(ieung_mieum_btn, chunjiin_consonant_group_cb, LV_EVENT_CLICKED, &ieung_mieum_index);
    
    // Enter 버튼
    lv_obj_t * enter_btn = lv_btn_create(parent);
    lv_obj_set_size(enter_btn, btn_width, btn_height);
    lv_obj_align(enter_btn, LV_ALIGN_CENTER, btn_width + btn_spacing, start_y);
    lv_obj_t * enter_label = lv_label_create(enter_btn);
    lv_label_set_text(enter_label, "Enter");
    lv_obj_center(enter_label);
    lv_obj_add_event_cb(enter_btn, complete_syllable_cb, LV_EVENT_CLICKED, NULL);
    
    // 다섯 번째 줄: 추가 기능 버튼들 (Space, 모드 전환, Clear)
    start_y += btn_height + btn_spacing;
    
    // Space 버튼
    lv_obj_t * space_btn = lv_btn_create(parent);
    lv_obj_set_size(space_btn, btn_width, btn_height);
    lv_obj_align(space_btn, LV_ALIGN_CENTER, -(btn_width + btn_spacing), start_y);
    lv_obj_t * space_label = lv_label_create(space_btn);
    lv_label_set_text(space_label, "Space");
    lv_obj_center(space_label);
    lv_obj_add_event_cb(space_btn, space_cb, LV_EVENT_CLICKED, NULL);
    
    // 모드 전환 버튼
    lv_obj_t * mode_btn = lv_btn_create(parent);
    lv_obj_set_size(mode_btn, btn_width, btn_height);
    lv_obj_align(mode_btn, LV_ALIGN_CENTER, 0, start_y);
    lv_obj_t * mode_label = lv_label_create(mode_btn);
    lv_label_set_text(mode_label, "한글");
    lv_obj_center(mode_label);
    lv_obj_add_event_cb(mode_btn, mode_toggle_cb, LV_EVENT_CLICKED, NULL);
    
    // Clear 버튼
    lv_obj_t * clear_btn = lv_btn_create(parent);
    lv_obj_set_size(clear_btn, btn_width, btn_height);
    lv_obj_align(clear_btn, LV_ALIGN_CENTER, btn_width + btn_spacing, start_y);
    lv_obj_t * clear_label = lv_label_create(clear_btn);
    lv_label_set_text(clear_label, "Clear");
    lv_obj_center(clear_label);
    lv_obj_add_event_cb(clear_btn, clear_cb, LV_EVENT_CLICKED, NULL);
    
    // 여섯 번째 줄: 기호 버튼들
    start_y += btn_height + btn_spacing;
    
    // 쉼표 버튼
    static char comma = ',';
    lv_obj_t * comma_btn = lv_btn_create(parent);
    lv_obj_set_size(comma_btn, btn_width, btn_height);
    lv_obj_align(comma_btn, LV_ALIGN_CENTER, -(btn_width + btn_spacing), start_y);
    lv_obj_t * comma_label = lv_label_create(comma_btn);
    lv_label_set_text(comma_label, ",");
    lv_obj_center(comma_label);
    lv_obj_add_event_cb(comma_btn, symbol_cb, LV_EVENT_CLICKED, &comma);
    
    // 마침표 버튼
    static char period = '.';
    lv_obj_t * period_btn = lv_btn_create(parent);
    lv_obj_set_size(period_btn, btn_width, btn_height);
    lv_obj_align(period_btn, LV_ALIGN_CENTER, 0, start_y);
    lv_obj_t * period_label = lv_label_create(period_btn);
    lv_label_set_text(period_label, ".");
    lv_obj_center(period_label);
    lv_obj_add_event_cb(period_btn, symbol_cb, LV_EVENT_CLICKED, &period);
    
    // 느낌표 버튼
    static char exclamation = '!';
    lv_obj_t * exclamation_btn = lv_btn_create(parent);
    lv_obj_set_size(exclamation_btn, btn_width, btn_height);
    lv_obj_align(exclamation_btn, LV_ALIGN_CENTER, btn_width + btn_spacing, start_y);
    lv_obj_t * exclamation_label = lv_label_create(exclamation_btn);
    lv_label_set_text(exclamation_label, "!");
    lv_obj_center(exclamation_label);
    lv_obj_add_event_cb(exclamation_btn, symbol_cb, LV_EVENT_CLICKED, &exclamation);
    
    // 키보드 이벤트 핸들러 추가
    lv_obj_add_event_cb(parent, keyboard_event_cb, LV_EVENT_KEY, NULL);
    
    // 초기 디스플레이 업데이트
    update_chunjiin_display();
    printf("천지인 탭이 성공적으로 생성되었습니다.\n");
}
