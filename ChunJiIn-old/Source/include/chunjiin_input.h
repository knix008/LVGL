#ifndef CHUNJIIN_INPUT_H
#define CHUNJIIN_INPUT_H

#include <stddef.h>

// 천지인 기본 요소들
typedef enum {
    CHUNJIIN_DOT,      // ㆍ (천)
    CHUNJIIN_EU,       // ㅡ (지)
    CHUNJIIN_I         // ㅣ (인)
} chunjiin_element_t;

// 천지인 자음 그룹들
typedef enum {
    CHUNJIIN_GIYEOK_KIYEOK,   // ㄱㅋ
    CHUNJIIN_NIEUN_RIEUL,     // ㄴㄹ
    CHUNJIIN_DIGEUT_TIEUT,    // ㄷㅌ
    CHUNJIIN_BIEUP_PIEUP,     // ㅂㅍ
    CHUNJIIN_SIOT_HIEUT,      // ㅅㅎ
    CHUNJIIN_JIEUT_CHIEUT,    // ㅈㅊ
    CHUNJIIN_IEUNG_MIEUM      // ㅇㅁ
} chunjiin_consonant_group_t;

// 입력 모드 열거형
typedef enum {
    INPUT_MODE_KOREAN,    // 한글 입력 모드
    INPUT_MODE_NUMBER     // 숫자 입력 모드
} input_mode_t;

// 한글 조합 상태 (Java Hangul 클래스 기반)
typedef struct {
    char chosung[8];      // 초성
    char jungsung[8];     // 중성
    char jongsung[8];     // 종성
    char jongsung2[8];    // 종성2 (겹받침용)
    int step;             // 조합 단계 (0=초성, 1=중성, 2=종성, 3=종성2)
    int flag_writing;     // 작성 중 플래그
    int flag_dotused;     // 점 사용 플래그
    int flag_doubled;     // 겹받침 플래그
    int flag_addcursor;   // 커서 추가 플래그
    int flag_space;       // 스페이스 플래그
} hangul_state_t;

// 천지인 입력 상태 구조체 (Java Chunjiin 클래스 기반)
typedef struct {
    // 출력 버퍼 (변환된 한글 텍스트를 저장)
    char output_buffer[256];
    size_t output_length;
    
    // 한글 조합 상태
    hangul_state_t hangul;
    
    // 자음 그룹 토글 상태 (0=첫번째, 1=두번째, 2=세번째)
    int group_toggle_states[7];
    
    // 입력 모드 (한글/숫자)
    input_mode_t input_mode;
    
    // 영어/숫자 입력용
    char engnum[32];
    int flag_initengnum;
    int flag_engdelete;
    int flag_upper;
} chunjiin_state_t;

// 전역 상태 변수 선언
extern chunjiin_state_t chunjiin_global_state;
extern int group_toggle_states[7];

// 초기화 함수
void chunjiin_init(void);

// 입력 처리 함수들 (Java hangulMake 기반)
void chunjiin_input_element(chunjiin_element_t element);
void chunjiin_input_consonant_group(chunjiin_consonant_group_t group);
void chunjiin_input_key(int input);  // Java input 매개변수 기반

// 키보드 입력 처리 함수
void chunjiin_input_keyboard(char key);
void chunjiin_clear_keyboard_buffer(void);

// 입력 모드 전환 함수
void chunjiin_toggle_input_mode(void);
input_mode_t chunjiin_get_input_mode(void);

// 버퍼 관리 함수들
void chunjiin_clear_all_buffers(void);

// 텍스트 변환 및 출력 함수들 (Java write 함수 기반)
void chunjiin_enter(void);
void chunjiin_backspace(void);
void chunjiin_clear(void);
void chunjiin_delete(void);

// 현재 텍스트 가져오기 (출력 버퍼 반환)
const char* chunjiin_get_current_text(void);

// 실시간 변환 함수
void chunjiin_update_input_buffer(void);

// 한글 유니코드 생성 함수 (Java getUnicode 기반)
int chunjiin_get_unicode(const char* real_jong);

// 겹받침 확인 함수 (Java checkDouble 기반)
const char* chunjiin_check_double(const char* jong, const char* jong2);

#endif // CHUNJIIN_INPUT_H 