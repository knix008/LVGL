#include "tab_chunjiin.h"
#include "chunjiin_input.h"
#include "ui_components.h"
#include <stdio.h>
#include <string.h>

// 전역 변수
static lv_obj_t * result_label = NULL;

// 천지인 자음 그룹 상태 (토글용)
static bool group_toggle_states[7] = {false, false, false, false, false, false, false};

// UI 업데이트 함수
static void update_chunjiin_display() {
    if (!result_label) {
        printf("DEBUG: update_chunjiin_display called but result_label is NULL\n");
        return;
    }
    
    // 결과 텍스트 업데이트
    const char* current_text = chunjiin_get_current_text();
    printf("DEBUG: update_chunjiin_display called, current_text='%s'\n", current_text);
    lv_label_set_text(result_label, current_text);
}

// 천지인 표준 자음 그룹 버튼 콜백 (토글 방식)
static void chunjiin_consonant_group_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        int* group_index = (int*)lv_event_get_user_data(e);
        chunjiin_consonant_group_t group = (chunjiin_consonant_group_t)*group_index;
        
        // 현재 그룹의 상태 토글
        group_toggle_states[group] = !group_toggle_states[group];
        
        printf("천지인: 자음 그룹 버튼 클릭 - group: %d, second: %s\n", 
               group, group_toggle_states[group] ? "true" : "false");
        
        chunjiin_input_consonant_group(group);
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
        
        // 현재 조합 중인 문자를 먼저 완성
        chunjiin_enter();
        
        // 전체 결합된 결과를 가져와서 터미널에 출력
        const char* combined_result = chunjiin_get_current_text();
        printf("=== 천지인 입력 결과 ===\n");
        printf("Combined characters: %s\n", combined_result);
        printf("====================\n");
        
        // 버퍼 전체 클리어
        chunjiin_clear();
        update_chunjiin_display();
    }
}

static void space_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("천지인: 스페이스 버튼 클릭\n");
        chunjiin_enter();  // Enter behavior for space
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

static void clear_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        printf("천지인: 클리어 버튼 클릭\n");
        chunjiin_clear();
        update_chunjiin_display();
    }
}

// 천지인 탭 생성
void create_chunjiin_tab(lv_obj_t * parent) {
    // 천지인 상태 초기화
    printf("DEBUG: About to call chunjiin_init()\n");
    chunjiin_init();
    
    // 한글 폰트 가져오기 (작은 폰트 사용)
    lv_font_t * korean_font = get_korean_font_small();
    printf("DEBUG: Korean font loaded: %p\n", korean_font);
    
    // Test the chunjiin system immediately
    printf("DEBUG: Testing ChunJiIn system...\n");
    chunjiin_input_element(CHUNJIIN_I);
    const char* test_result = chunjiin_get_current_text();
    printf("DEBUG: After inputting ㅣ, result: '%s'\n", test_result);
    
    // 메인 컨테이너 생성
    lv_obj_t * container = lv_obj_create(parent);
    lv_obj_set_size(container, LV_PCT(100), LV_PCT(100));
    lv_obj_set_flex_flow(container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(container, 1, 0);  // Reduced from 3 to 1
    
    // 제목
    lv_obj_t * title = lv_label_create(container);
    lv_label_set_text(title, "천지인 입력법");
    lv_obj_set_style_text_font(title, korean_font, 0);
    
    // 결과 표시 영역
    result_label = lv_label_create(container);
    lv_label_set_text(result_label, "Test Korean: 테스트");  // Add test text
    lv_obj_set_style_text_font(result_label, korean_font, 0);
    lv_obj_set_style_bg_color(result_label, lv_color_hex(0xf0f0f0), 0);
    lv_obj_set_style_bg_opa(result_label, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(result_label, 1, 0);
    lv_obj_set_style_pad_all(result_label, 2, 0);  // Reduced from 5 to 2
    lv_obj_set_size(result_label, LV_PCT(95), 25);  // Reduced from 40 to 25
    lv_label_set_long_mode(result_label, LV_LABEL_LONG_WRAP);
    printf("DEBUG: result_label created with test text\n");
    
    // 첫 번째 줄: 천지인 기본 요소들 (ㅣ, ., ㅡ)
    lv_obj_t * basic_container = lv_obj_create(container);
    lv_obj_set_size(basic_container, LV_PCT(95), 33);  // Increased from 28 to 33
    lv_obj_set_flex_flow(basic_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(basic_container, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_border_width(basic_container, 0, 0);
    lv_obj_set_style_bg_opa(basic_container, LV_OPA_TRANSP, 0);
    
    // ㅣ (인) 버튼
    lv_obj_t * vbar_btn = lv_btn_create(basic_container);
    lv_obj_set_size(vbar_btn, 50, 30);  // Increased height from 25 to 30
    lv_obj_t * vbar_label = lv_label_create(vbar_btn);
    lv_label_set_text(vbar_label, "ㅣ");
    lv_obj_set_style_text_font(vbar_label, korean_font, 0);
    lv_obj_center(vbar_label);
    lv_obj_add_event_cb(vbar_btn, chunjiin_vbar_cb, LV_EVENT_CLICKED, NULL);
    
    // . (천) 버튼
    lv_obj_t * dot_btn = lv_btn_create(basic_container);
    lv_obj_set_size(dot_btn, 50, 30);  // Increased height from 25 to 30
    lv_obj_t * dot_label = lv_label_create(dot_btn);
    lv_label_set_text(dot_label, "ㆍ");
    lv_obj_set_style_text_font(dot_label, korean_font, 0);
    lv_obj_center(dot_label);
    lv_obj_add_event_cb(dot_btn, chunjiin_dot_cb, LV_EVENT_CLICKED, NULL);
    
    // ㅡ (지) 버튼
    lv_obj_t * hbar_btn = lv_btn_create(basic_container);
    lv_obj_set_size(hbar_btn, 50, 30);  // Increased height from 25 to 30
    lv_obj_t * hbar_label = lv_label_create(hbar_btn);
    lv_label_set_text(hbar_label, "ㅡ");
    lv_obj_set_style_text_font(hbar_label, korean_font, 0);
    lv_obj_center(hbar_label);
    lv_obj_add_event_cb(hbar_btn, chunjiin_hbar_cb, LV_EVENT_CLICKED, NULL);
    
    // 두 번째 줄: 자음 그룹들 (ㄱㅋ, ㄴㄹ, ㄷㅌ)
    lv_obj_t * consonant_container1 = lv_obj_create(container);
    lv_obj_set_size(consonant_container1, LV_PCT(95), 33);  // Increased from 28 to 33
    lv_obj_set_flex_flow(consonant_container1, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(consonant_container1, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_border_width(consonant_container1, 0, 0);
    lv_obj_set_style_bg_opa(consonant_container1, LV_OPA_TRANSP, 0);
    
    // 자음 그룹 인덱스
    static int group_indices1[] = {
        CHUNJIIN_GIYEOK_KIYEOK,
        CHUNJIIN_NIEUN_RIEUL, 
        CHUNJIIN_DIGEUT_TIEUT
    };
    
    const char* group_labels1[] = {"ㄱㅋ", "ㄴㄹ", "ㄷㅌ"};
    
    for (int i = 0; i < 3; i++) {
        lv_obj_t * btn = lv_btn_create(consonant_container1);
        lv_obj_set_size(btn, 50, 30);  // Increased height from 25 to 30
        lv_obj_t * label = lv_label_create(btn);
        lv_label_set_text(label, group_labels1[i]);
        lv_obj_set_style_text_font(label, korean_font, 0);
        lv_obj_center(label);
        lv_obj_add_event_cb(btn, chunjiin_consonant_group_cb, LV_EVENT_CLICKED, &group_indices1[i]);
    }
    
    // 세 번째 줄: 자음 그룹들 (ㅂㅍ, ㅅㅎ, ㅈㅊ)
    lv_obj_t * consonant_container2 = lv_obj_create(container);
    lv_obj_set_size(consonant_container2, LV_PCT(95), 33);  // Increased from 28 to 33
    lv_obj_set_flex_flow(consonant_container2, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(consonant_container2, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_border_width(consonant_container2, 0, 0);
    lv_obj_set_style_bg_opa(consonant_container2, LV_OPA_TRANSP, 0);
    
    static int group_indices2[] = {
        CHUNJIIN_BIEUP_PIEUP,
        CHUNJIIN_SIOT_HIEUT,
        CHUNJIIN_JIEUT_CHIEUT
    };
    
    const char* group_labels2[] = {"ㅂㅍ", "ㅅㅎ", "ㅈㅊ"};
    
    for (int i = 0; i < 3; i++) {
        lv_obj_t * btn = lv_btn_create(consonant_container2);
        lv_obj_set_size(btn, 50, 30);  // Increased height from 25 to 30
        lv_obj_t * label = lv_label_create(btn);
        lv_label_set_text(label, group_labels2[i]);
        lv_obj_set_style_text_font(label, korean_font, 0);
        lv_obj_center(label);
        lv_obj_add_event_cb(btn, chunjiin_consonant_group_cb, LV_EVENT_CLICKED, &group_indices2[i]);
    }
    
    // 네 번째 줄: 기능 버튼들 (backspace, ㅇㅁ, enter)
    lv_obj_t * function_container = lv_obj_create(container);
    lv_obj_set_size(function_container, LV_PCT(95), 33);  // Increased from 28 to 33
    lv_obj_set_flex_flow(function_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(function_container, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_border_width(function_container, 0, 0);
    lv_obj_set_style_bg_opa(function_container, LV_OPA_TRANSP, 0);
    
    // Backspace 버튼
    lv_obj_t * backspace_btn = lv_btn_create(function_container);
    lv_obj_set_size(backspace_btn, 60, 30);  // Increased height from 25 to 30
    lv_obj_t * backspace_label = lv_label_create(backspace_btn);
    lv_label_set_text(backspace_label, "Back");
    lv_obj_center(backspace_label);
    lv_obj_add_event_cb(backspace_btn, backspace_cb, LV_EVENT_CLICKED, NULL);
    
    // ㅇㅁ 버튼 (중앙에 배치)
    static int ieung_mieum_index = CHUNJIIN_IEUNG_MIEUM;
    lv_obj_t * ieung_mieum_btn = lv_btn_create(function_container);
    lv_obj_set_size(ieung_mieum_btn, 50, 30);  // 다른 자음 버튼과 같은 크기
    lv_obj_t * ieung_mieum_label = lv_label_create(ieung_mieum_btn);
    lv_label_set_text(ieung_mieum_label, "ㅇㅁ");
    lv_obj_set_style_text_font(ieung_mieum_label, korean_font, 0);
    lv_obj_center(ieung_mieum_label);
    lv_obj_add_event_cb(ieung_mieum_btn, chunjiin_consonant_group_cb, LV_EVENT_CLICKED, &ieung_mieum_index);
    
    // Enter 버튼
    lv_obj_t * enter_btn = lv_btn_create(function_container);
    lv_obj_set_size(enter_btn, 60, 30);  // Increased height from 25 to 30
    lv_obj_t * enter_label = lv_label_create(enter_btn);
    lv_label_set_text(enter_label, "Enter");
    lv_obj_center(enter_label);
    lv_obj_add_event_cb(enter_btn, complete_syllable_cb, LV_EVENT_CLICKED, NULL);
    
    // 초기 디스플레이 업데이트
    update_chunjiin_display();
    
    printf("천지인 탭이 성공적으로 생성되었습니다.\n");
}
