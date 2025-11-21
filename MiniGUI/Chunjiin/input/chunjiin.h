#ifndef CHUNJIIN_INPUT_H
#define CHUNJIIN_INPUT_H

#include <stdbool.h>
#include <wchar.h>

#define MAX_TEXT_LEN 1024

typedef enum {
    MODE_HANGUL = 0,
    MODE_UPPER_ENGLISH = 1,
    MODE_ENGLISH = 2,
    MODE_NUMBER = 3,
    MODE_SPECIAL = 4
} InputMode;

typedef struct {
    wchar_t chosung[16];      // 초성
    wchar_t jungsung[16];     // 중성
    wchar_t jongsung[16];     // 종성
    wchar_t jongsung2[16];    // 종성2 (겹받침)
    int step;                 // 현재 단계 (0:초성, 1:중성, 2:종성, 3:겹받침)
    bool flag_writing;        // 작성 중 플래그
    bool flag_dotused;        // 점(·, ‥) 사용 플래그
    bool flag_doubled;        // 겹받침 플래그
    bool flag_addcursor;      // 커서 추가 플래그
    bool flag_space;          // 스페이스 플래그
} HangulState;

typedef struct {
    HangulState hangul;
    InputMode now_mode;

    wchar_t engnum[16];       // 영문/숫자 버퍼
    bool flag_initengnum;     // 영문/숫자 초기화 플래그
    bool flag_engdelete;      // 영문 삭제 플래그
    bool flag_upper;          // 대문자 플래그

    wchar_t text_buffer[MAX_TEXT_LEN];  // 텍스트 버퍼
    int cursor_pos;           // 커서 위치
} ChunjiinState;

#endif // CHUNJIIN_INPUT_H
