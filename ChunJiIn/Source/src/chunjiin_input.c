#include "chunjiin_input.h"
#include "korean_input.h"
#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <string.h>
#include <stdlib.h>

// --- 한글 유니코드 상수 정의 ---
#define HANGUL_BASE 0xAC00      // '가'의 유니코드 값
#define CHOSEONG_COUNT 19       // 초성 개수
#define JUNGSEONG_COUNT 21      // 중성 개수
#define JONGSEONG_COUNT 28      // 종성 개수 (종성 없음 포함)

// --- 입력 상태 정의 ---
typedef enum {
    STATE_START,       // 시작 상태
    STATE_CHOSEONG,    // 초성 입력 중
    STATE_JUNGSEONG,   // 중성 입력 중
    STATE_JONGSEONG,   // 종성 입력 중
    STATE_JONGSEONG_COMPOUND // 복합 종성 입력 중
} InputState;

// --- Jamo(자모) 테이블 ---
// 유니코드상 순서에 따른 초성, 중성, 종성 목록
static const wchar_t CHOSEONG_TABLE[] = {
    L'ㄱ', L'ㄲ', L'ㄴ', L'ㄷ', L'ㄸ', L'ㄹ', L'ㅁ', L'ㅂ', L'ㅃ', L'ㅅ',
    L'ㅆ', L'ㅇ', L'ㅈ', L'ㅉ', L'ㅊ', L'ㅋ', L'ㅌ', L'ㅍ', L'ㅎ', L'\0'
};
static const wchar_t JUNGSEONG_TABLE[] = {
    L'ㅏ', L'ㅐ', L'ㅑ', L'ㅒ', L'ㅓ', L'ㅔ', L'ㅕ', L'ㅖ', L'ㅗ', L'ㅘ',
    L'ㅙ', L'ㅚ', L'ㅛ', L'ㅜ', L'ㅝ', L'ㅞ', L'ㅟ', L'ㅠ', L'ㅡ', L'ㅢ',
    L'ㅣ', L'\0'
};
static const wchar_t JONGSEONG_TABLE[] = {
    L' ', L'ㄱ', L'ㄲ', L'ㄳ', L'ㄴ', L'ㄵ', L'ㄶ', L'ㄷ', L'ㄹ', L'ㄺ',
    L'ㄻ', L'ㄼ', L'ㄽ', L'ㄾ', L'ㄿ', L'ㅀ', L'ㅁ', L'ㅂ', L'ㅄ', L'ㅅ',
    L'ㅆ', L'ㅇ', L'ㅈ', L'ㅊ', L'ㅋ', L'ㅌ', L'ㅍ', L'ㅎ', L'\0'
};

// 천지인 자음 그룹 매핑
typedef struct {
    chunjiin_consonant_group_t group;
    wchar_t first_consonant;
    wchar_t second_consonant;
} consonant_group_mapping_t;

static const consonant_group_mapping_t consonant_groups[] = {
    {CHUNJIIN_GIYEOK_KIYEOK, L'ㄱ', L'ㅋ'},
    {CHUNJIIN_NIEUN_RIEUL, L'ㄴ', L'ㄹ'},
    {CHUNJIIN_DIGEUT_TIEUT, L'ㄷ', L'ㅌ'},
    {CHUNJIIN_BIEUP_PIEUP, L'ㅂ', L'ㅍ'},
    {CHUNJIIN_SIOT_HIEUT, L'ㅅ', L'ㅎ'},
    {CHUNJIIN_JIEUT_CHIEUT, L'ㅈ', L'ㅊ'},
    {CHUNJIIN_IEUNG_MIEUM, L'ㅇ', L'ㅁ'}
};

// 전역 입력 상태
static InputState current_state = STATE_START;
static wchar_t buffer[256] = {0};
static int buffer_idx = 0;
static int cho = -1, jung = -1, jong = -1;
static int prev_jung = -1;
static int prev_jong = -1;

// --- 함수 구현 ---
wchar_t compose_hangul(int cho, int jung, int jong) {
    if (cho < 0 || cho >= CHOSEONG_COUNT || jung < 0 || jung >= JUNGSEONG_COUNT || jong < 0 || jong >= JONGSEONG_COUNT) {
        return 0;
    }
    return HANGUL_BASE + (cho * JUNGSEONG_COUNT + jung) * JONGSEONG_COUNT + jong;
}

int find_char_index(const wchar_t* table, wchar_t ch) {
    for (int i = 0; table[i] != L'\0'; i++) {
        if (table[i] == ch) {
            return i;
        }
    }
    return -1;
}

int get_key_type(char c) {
    // 0: 자음, 1: 모음, 2: 기타
    if (c == 'q' || c == 'w' || c == 'e' || c == 'r' || c == 't' || c == 'y' || c == 'u') {
        return 0; // 자음
    }
    if (c == 'i' || c == 'o' || c == 'p') {
        return 1; // 모음
    }
    return 2; // 기타
}

// Helper functions for input processing
static void process_consonant_input(char c) {
    printf("DEBUG: process_consonant_input called with c='%c'\n", c);
    wchar_t key_char = 0;
    
    // 키에 해당하는 기본 자음 설정
    if (c == 'q') { key_char = L'ㄱ'; }
    else if (c == 'w') { key_char = L'ㄴ'; }
    else if (c == 'e') { key_char = L'ㄷ'; }
    else if (c == 'r') { key_char = L'ㅂ'; }
    else if (c == 't') { key_char = L'ㅅ'; }
    else if (c == 'y') { key_char = L'ㅈ'; }
    else if (c == 'u') { key_char = L'ㅇ'; }
    else return;

    printf("DEBUG: key_char = %lc, current_state = %d\n", key_char, current_state);

    if (current_state == STATE_START) {
        printf("DEBUG: In STATE_START branch\n");
        cho = find_char_index(CHOSEONG_TABLE, key_char);
        current_state = STATE_CHOSEONG;
        printf("DEBUG: Set cho=%d, changed state to STATE_CHOSEONG\n", cho);
    }
    else if (current_state == STATE_CHOSEONG) {
        // 같은 자음 연속 입력 시 격음/경음 처리 (ㄱ -> ㅋ -> ㄲ)
        if (cho == find_char_index(CHOSEONG_TABLE, key_char)) {
            if (key_char == L'ㄱ') cho = find_char_index(CHOSEONG_TABLE, L'ㅋ');
            else if (cho == find_char_index(CHOSEONG_TABLE, L'ㅋ')) cho = find_char_index(CHOSEONG_TABLE, L'ㄲ');
        } else { // 다른 자음 입력 시, 이전 글자 완성 후 새 글자 시작
            if (cho >= 0) buffer[buffer_idx++] = CHOSEONG_TABLE[cho];
            cho = find_char_index(CHOSEONG_TABLE, key_char);
        }
    }
    else if (current_state == STATE_JUNGSEONG) {
        int new_jong = find_char_index(JONGSEONG_TABLE, key_char);
        if (new_jong != -1 && new_jong != 0) { // 종성으로 사용 가능한 자음이면
            jong = new_jong;
            prev_jong = jong;
            current_state = STATE_JONGSEONG;
        } else { // 종성으로 사용 불가하면, 이전 글자 완성 후 새 글자 시작
            if (cho >= 0 && jung >= 0) {
                buffer[buffer_idx++] = compose_hangul(cho, jung, 0);
            }
            cho = find_char_index(CHOSEONG_TABLE, key_char);
            jung = jong = -1;
            current_state = STATE_CHOSEONG;
        }
    }
    else if (current_state == STATE_JONGSEONG || current_state == STATE_JONGSEONG_COMPOUND) {
        // 복합 종성 시도 (예: ㄱ + ㅅ -> ㄳ)
        if (jong == find_char_index(JONGSEONG_TABLE, L'ㄱ') && key_char == L'ㅅ') {
            jong = find_char_index(JONGSEONG_TABLE, L'ㄳ');
            current_state = STATE_JONGSEONG_COMPOUND;
        }
        // 다른 복합 종성 규칙들도 여기에 추가...
        else { // 복합 종성 불가 시, 이전 글자 완성 후 새 글자 시작
            if (cho >= 0 && jung >= 0 && jong >= 0) {
                buffer[buffer_idx++] = compose_hangul(cho, jung, jong);
            }
            cho = find_char_index(CHOSEONG_TABLE, key_char);
            jung = jong = -1;
            current_state = STATE_CHOSEONG;
        }
    }
}

static void process_vowel_input(char c) {
    printf("DEBUG: process_vowel_input called with c='%c'\n", c);
    wchar_t key_vowel = (c == 'i') ? L'ㅣ' : (c == 'o') ? L'ㆍ' : L'ㅡ';
    printf("DEBUG: key_vowel = %lc, current_state = %d\n", key_vowel, current_state);

    if (current_state == STATE_START) {
        printf("DEBUG: In STATE_START branch (vowel-only input)\n");
        cho = -1;  // No consonant
        if (key_vowel == L'ㅣ') jung = find_char_index(JUNGSEONG_TABLE, L'ㅣ');
        else if (key_vowel == L'ㆍ') jung = find_char_index(JUNGSEONG_TABLE, L'ㅏ');
        else if (key_vowel == L'ㅡ') jung = find_char_index(JUNGSEONG_TABLE, L'ㅡ');
        current_state = STATE_JUNGSEONG;
        prev_jung = jung;
        printf("DEBUG: Set jung=%d for vowel-only, changed state to STATE_JUNGSEONG\n", jung);
    }
    else if (current_state == STATE_CHOSEONG) {
        printf("DEBUG: In STATE_CHOSEONG branch\n");
        if (key_vowel == L'ㅣ') jung = find_char_index(JUNGSEONG_TABLE, L'ㅣ');
        else if (key_vowel == L'ㆍ') jung = find_char_index(JUNGSEONG_TABLE, L'ㅏ');
        else if (key_vowel == L'ㅡ') jung = find_char_index(JUNGSEONG_TABLE, L'ㅡ');
        current_state = STATE_JUNGSEONG;
        prev_jung = jung;
        printf("DEBUG: Set jung=%d, changed state to STATE_JUNGSEONG\n", jung);
    }
    else if (current_state == STATE_JUNGSEONG) {
        // 모음 조합 규칙
        if (prev_jung == find_char_index(JUNGSEONG_TABLE, L'ㅏ') && key_vowel == L'ㆍ') {
            jung = find_char_index(JUNGSEONG_TABLE, L'ㅑ');
        }
        else if (prev_jung == find_char_index(JUNGSEONG_TABLE, L'ㅗ') && key_vowel == L'ㆍ') {
            jung = find_char_index(JUNGSEONG_TABLE, L'ㅛ');
        }
        else if (prev_jung == find_char_index(JUNGSEONG_TABLE, L'ㅗ') && key_vowel == L'ㅣ') {
            jung = find_char_index(JUNGSEONG_TABLE, L'ㅚ');
        }
        else if (prev_jung == find_char_index(JUNGSEONG_TABLE, L'ㅜ') && key_vowel == L'ㅣ') {
            jung = find_char_index(JUNGSEONG_TABLE, L'ㅟ');
        }
        else if (prev_jung == find_char_index(JUNGSEONG_TABLE, L'ㅡ') && key_vowel == L'ㅣ') {
            jung = find_char_index(JUNGSEONG_TABLE, L'ㅢ');
        }
        // 조합 불가 시, 이전 글자 완성 후 새 모음 시작
        else {
            if (cho >= 0 && jung >= 0) {
                buffer[buffer_idx++] = compose_hangul(cho, jung, 0);
            }
            cho = jong = -1;
            if (key_vowel == L'ㅣ') jung = find_char_index(JUNGSEONG_TABLE, L'ㅣ');
            else if (key_vowel == L'ㆍ') jung = find_char_index(JUNGSEONG_TABLE, L'ㅏ');
            else if (key_vowel == L'ㅡ') jung = find_char_index(JUNGSEONG_TABLE, L'ㅡ');
            current_state = STATE_JUNGSEONG;
        }
        prev_jung = jung;
    }
    else if (current_state == STATE_JONGSEONG || current_state == STATE_JONGSEONG_COMPOUND) {
        // 종성 분리: "값" + 'ㅣ' -> "갑" + "시"
        int old_jong = jong;
        jong = 0; // 이전 글자의 종성은 없음으로 처리
        
        // 복합 종성이었으면 분리
        if (current_state == STATE_JONGSEONG_COMPOUND) {
            if (old_jong == find_char_index(JONGSEONG_TABLE, L'ㄳ')) {
                jong = find_char_index(JONGSEONG_TABLE, L'ㄱ');
                cho = find_char_index(CHOSEONG_TABLE, L'ㅅ');
            }
            // 다른 복합 종성 분리 규칙들...
        } else {
            cho = find_char_index(CHOSEONG_TABLE, JONGSEONG_TABLE[old_jong]);
        }

        if (cho >= 0 && jung >= 0) {
            buffer[buffer_idx++] = compose_hangul(cho, jung, jong);
        }
        
        // 새 글자 시작
        jung = jong = -1;
        if (key_vowel == L'ㅣ') jung = find_char_index(JUNGSEONG_TABLE, L'ㅣ');
        else if (key_vowel == L'ㆍ') jung = find_char_index(JUNGSEONG_TABLE, L'ㅏ');
        else if (key_vowel == L'ㅡ') jung = find_char_index(JUNGSEONG_TABLE, L'ㅡ');
        current_state = STATE_JUNGSEONG;
    }
}

// === GUI API Functions ===

/**
 * @brief ChunJiIn 입력 시스템을 초기화합니다.
 */
void chunjiin_init() {
    printf("DEBUG: chunjiin_init() called\n");
    
    // Set locale for Korean UTF-8 support
    setlocale(LC_ALL, "en_US.UTF-8");
    printf("DEBUG: Locale set to UTF-8\n");
    
    current_state = STATE_START;
    memset(buffer, 0, sizeof(buffer));
    buffer_idx = 0;
    cho = jung = jong = -1;
    prev_jung = prev_jong = -1;
    printf("DEBUG: chunjiin_init() completed - ChunJiIn system ready!\n");
}

/**
 * @brief 현재 조합 중인 문자열을 UTF-8로 반환합니다.
 * @return UTF-8 인코딩된 현재 텍스트
 */
const char* chunjiin_get_current_text() {
    static char utf8_buffer[1024];
    wchar_t temp_buffer[256];
    int temp_idx = 0;
    
    printf("DEBUG: chunjiin_get_current_text called, state=%d, buffer_idx=%d, cho=%d, jung=%d, jong=%d\n", 
           current_state, buffer_idx, cho, jung, jong);
    
    // 완성된 문자들 복사
    for (int i = 0; i < buffer_idx; i++) {
        temp_buffer[temp_idx++] = buffer[i];
        printf("DEBUG: buffer[%d] = %lc (U+%04X)\n", i, buffer[i], (unsigned int)buffer[i]);
    }
    
    // 현재 조합 중인 문자 추가
    if (current_state != STATE_START) {
        wchar_t current_char = 0;
        if (current_state == STATE_CHOSEONG && cho >= 0) {
            current_char = CHOSEONG_TABLE[cho];
            printf("DEBUG: STATE_CHOSEONG, cho=%d, current_char = %lc (U+%04X)\n", cho, current_char, (unsigned int)current_char);
        } else if (current_state == STATE_JUNGSEONG && jung >= 0) {
            if (cho >= 0) {
                current_char = compose_hangul(cho, jung, 0);
                printf("DEBUG: STATE_JUNGSEONG, cho=%d, jung=%d, composed hangul = %lc (U+%04X)\n", cho, jung, current_char, (unsigned int)current_char);
            } else {
                current_char = JUNGSEONG_TABLE[jung];
                printf("DEBUG: STATE_JUNGSEONG, jung=%d, vowel only = %lc (U+%04X)\n", jung, current_char, (unsigned int)current_char);
            }
        } else if ((current_state == STATE_JONGSEONG || current_state == STATE_JONGSEONG_COMPOUND) 
                   && cho >= 0 && jung >= 0 && jong >= 0) {
            current_char = compose_hangul(cho, jung, jong);
            printf("DEBUG: STATE_JONGSEONG, cho=%d, jung=%d, jong=%d, composed hangul = %lc (U+%04X)\n", cho, jung, jong, current_char, (unsigned int)current_char);
        }
        
        if (current_char != 0) {
            temp_buffer[temp_idx++] = current_char;
            printf("DEBUG: Added current_char to temp_buffer, temp_idx now = %d\n", temp_idx);
        } else {
            printf("DEBUG: current_char is 0, not adding to buffer\n");
        }
    }
    
    temp_buffer[temp_idx] = L'\0';
    
    // Clear the UTF-8 buffer first
    memset(utf8_buffer, 0, sizeof(utf8_buffer));
    
    // UTF-8로 변환 with error checking
    size_t result = wcstombs(utf8_buffer, temp_buffer, sizeof(utf8_buffer) - 1);
    if (result == (size_t)-1) {
        printf("DEBUG: wcstombs conversion failed!\n");
        strcpy(utf8_buffer, "[CONVERSION ERROR]");
    } else {
        printf("DEBUG: wcstombs successful, converted %zu bytes\n", result);
    }
    
    printf("DEBUG: Final UTF-8 result: '%s' (temp_idx=%d, strlen=%zu)\n", utf8_buffer, temp_idx, strlen(utf8_buffer));
    
    // Also test with a simple Korean character for debugging
    if (temp_idx == 0) {
        wchar_t test_char = L'가';  // Simple Korean character for testing
        temp_buffer[0] = test_char;
        temp_buffer[1] = L'\0';
        size_t test_result = wcstombs(utf8_buffer, temp_buffer, sizeof(utf8_buffer) - 1);
        printf("DEBUG: Test conversion of '가': result=%zu, utf8='%s'\n", test_result, utf8_buffer);
        // Reset to empty for actual return
        utf8_buffer[0] = '\0';
    }
    
    return utf8_buffer;
}

/**
 * @brief 모음 요소(ㅣ, ㆍ, ㅡ)를 입력합니다.
 * @param element 입력할 모음 요소
 */
void chunjiin_input_element(chunjiin_element_t element) {
    printf("DEBUG: chunjiin_input_element called with element=%d\n", element);
    char key_char = 0;
    
    switch (element) {
        case CHUNJIIN_I:
            key_char = 'i';
            break;
        case CHUNJIIN_DOT:
            key_char = 'o';
            break;
        case CHUNJIIN_EU:
            key_char = 'p';
            break;
    }
    
    printf("DEBUG: Mapped element to key_char='%c'\n", key_char);
    if (key_char) {
        process_vowel_input(key_char);
    }
}

/**
 * @brief 자음 그룹을 입력합니다.
 * @param group 입력할 자음 그룹
 */
void chunjiin_input_consonant_group(chunjiin_consonant_group_t group) {
    printf("DEBUG: chunjiin_input_consonant_group called with group=%d\n", group);
    char key_char = 0;
    
    switch (group) {
        case CHUNJIIN_GIYEOK_KIYEOK:
            key_char = 'q';
            break;
        case CHUNJIIN_NIEUN_RIEUL:
            key_char = 'w';
            break;
        case CHUNJIIN_DIGEUT_TIEUT:
            key_char = 'e';
            break;
        case CHUNJIIN_BIEUP_PIEUP:
            key_char = 'r';
            break;
        case CHUNJIIN_SIOT_HIEUT:
            key_char = 't';
            break;
        case CHUNJIIN_JIEUT_CHIEUT:
            key_char = 'y';
            break;
        case CHUNJIIN_IEUNG_MIEUM:
            key_char = 'u';
            break;
    }
    
    printf("DEBUG: Mapped group to key_char='%c'\n", key_char);
    if (key_char) {
        process_consonant_input(key_char);
    }
}

/**
 * @brief 백스페이스 처리
 */
void chunjiin_backspace() {
    if (current_state == STATE_START) {
        if (buffer_idx > 0) {
            buffer[--buffer_idx] = 0;
        }
    } else if (current_state == STATE_JONGSEONG_COMPOUND) {
        jong = prev_jong;
        prev_jong = -1;
        current_state = STATE_JONGSEONG;
    } else if (current_state == STATE_JONGSEONG) {
        jong = -1;
        current_state = STATE_JUNGSEONG;
    } else if (current_state == STATE_JUNGSEONG) {
        jung = -1;
        current_state = STATE_CHOSEONG;
    } else if (current_state == STATE_CHOSEONG) {
        cho = -1;
        current_state = STATE_START;
    }
}

/**
 * @brief 현재 조합 중인 문자를 완성하고 버퍼에 추가
 */
void chunjiin_enter() {
    if (current_state != STATE_START) {
        wchar_t current_char = 0;
        
        if (current_state == STATE_CHOSEONG && cho >= 0) {
            current_char = CHOSEONG_TABLE[cho];
        } else if (current_state == STATE_JUNGSEONG && jung >= 0) {
            if (cho >= 0) {
                current_char = compose_hangul(cho, jung, 0);
            } else {
                current_char = JUNGSEONG_TABLE[jung];
            }
        } else if ((current_state == STATE_JONGSEONG || current_state == STATE_JONGSEONG_COMPOUND) 
                   && cho >= 0 && jung >= 0 && jong >= 0) {
            current_char = compose_hangul(cho, jung, jong);
        }
        
        if (current_char != 0 && buffer_idx < 255) {
            buffer[buffer_idx++] = current_char;
        }
        
        // 상태 초기화
        current_state = STATE_START;
        cho = jung = jong = -1;
        prev_jung = prev_jong = -1;
    }
}

/**
 * @brief 모든 텍스트를 지웁니다.
 */
void chunjiin_clear() {
    current_state = STATE_START;
    memset(buffer, 0, sizeof(buffer));
    buffer_idx = 0;
    cho = jung = jong = -1;
    prev_jung = prev_jong = -1;
}


