#ifndef CHUNJIIN_INPUT_H
#define CHUNJIIN_INPUT_H

#include <stddef.h>
#include <stdbool.h>

// 천지인 기본 요소
typedef enum {
    CHUNJIIN_DOT = 0,    // ㆍ (천) - "."
    CHUNJIIN_EU = 1,     // ㅡ (지) - "ㅡ"
    CHUNJIIN_I = 2       // ㅣ (인) - "ㅣ"
} chunjiin_element_t;

// 천지인 키보드 자음 그룹
typedef enum {
    CHUNJIIN_GIYEOK_KIYEOK = 0,    // ㄱㅋ
    CHUNJIIN_NIEUN_RIEUL = 1,      // ㄴㄹ
    CHUNJIIN_DIGEUT_TIEUT = 2,     // ㄷㅌ
    CHUNJIIN_BIEUP_PIEUP = 3,      // ㅂㅍ
    CHUNJIIN_SIOT_HIEUT = 4,       // ㅅㅎ
    CHUNJIIN_JIEUT_CHIEUT = 5,     // ㅈㅊ
    CHUNJIIN_IEUNG_MIEUM = 6       // ㅇㅁ
} chunjiin_consonant_group_t;

// 함수 선언
void chunjiin_init(void);
void chunjiin_clear(void);
void chunjiin_input_consonant_group(chunjiin_consonant_group_t group);
void chunjiin_input_element(chunjiin_element_t element);
void chunjiin_enter(void);
void chunjiin_backspace(void);
const char* chunjiin_get_current_text(void);

#endif // CHUNJIIN_INPUT_H
