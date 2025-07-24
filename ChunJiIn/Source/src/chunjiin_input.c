#include "chunjiin_input.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// 전역 상태 변수
chunjiin_state_t chunjiin_global_state;
int group_toggle_states[7] = {0, 0, 0, 0, 0, 0, 0};

// 한글 유니코드 상수
#define HANGUL_BASE 0xAC00

// 초기화 함수 (Java Chunjiin 생성자 기반)
void chunjiin_init(void) {
    memset(&chunjiin_global_state, 0, sizeof(chunjiin_state_t));
    memset(group_toggle_states, 0, sizeof(group_toggle_states));
    
    // Java Hangul.init() 기반
    chunjiin_global_state.hangul.step = 0;
    chunjiin_global_state.hangul.flag_writing = 0;
    chunjiin_global_state.hangul.flag_dotused = 0;
    chunjiin_global_state.hangul.flag_doubled = 0;
    chunjiin_global_state.hangul.flag_addcursor = 0;
    chunjiin_global_state.hangul.flag_space = 0;
    
    // Java init_engnum() 기반
    chunjiin_global_state.flag_initengnum = 0;
    chunjiin_global_state.flag_engdelete = 0;
    chunjiin_global_state.flag_upper = 1;
    
    chunjiin_global_state.input_mode = INPUT_MODE_KOREAN;
    
    printf("ChunJiIn initialized (Java-based implementation)\n");
}

// Java Hangul.init() 함수
static void hangul_init(hangul_state_t* hangul) {
    memset(hangul->chosung, 0, sizeof(hangul->chosung));
    memset(hangul->jungsung, 0, sizeof(hangul->jungsung));
    memset(hangul->jongsung, 0, sizeof(hangul->jongsung));
    memset(hangul->jongsung2, 0, sizeof(hangul->jongsung2));
    hangul->step = 0;
    hangul->flag_writing = 0;
    hangul->flag_dotused = 0;
    hangul->flag_doubled = 0;
    hangul->flag_addcursor = 0;
    hangul->flag_space = 0;
}

// Java init_engnum() 함수
static void init_engnum(void) {
    memset(chunjiin_global_state.engnum, 0, sizeof(chunjiin_global_state.engnum));
    chunjiin_global_state.flag_initengnum = 0;
    chunjiin_global_state.flag_engdelete = 0;
}

// Java getUnicode() 함수 변환
int chunjiin_get_unicode(const char* real_jong) {
    int cho, jung, jong;
    
    // 초성 처리
    if (strlen(chunjiin_global_state.hangul.chosung) == 0) {
        if (strlen(chunjiin_global_state.hangul.jungsung) == 0 || 
            strcmp(chunjiin_global_state.hangul.jungsung, "·") == 0 || 
            strcmp(chunjiin_global_state.hangul.jungsung, "‥") == 0) {
            return 0;
        }
    }
    
    // 초성 인덱스 찾기
    if (strcmp(chunjiin_global_state.hangul.chosung, "ㄱ") == 0) cho = 0;
    else if (strcmp(chunjiin_global_state.hangul.chosung, "ㄲ") == 0) cho = 1;
    else if (strcmp(chunjiin_global_state.hangul.chosung, "ㄴ") == 0) cho = 2;
    else if (strcmp(chunjiin_global_state.hangul.chosung, "ㄷ") == 0) cho = 3;
    else if (strcmp(chunjiin_global_state.hangul.chosung, "ㄸ") == 0) cho = 4;
    else if (strcmp(chunjiin_global_state.hangul.chosung, "ㄹ") == 0) cho = 5;
    else if (strcmp(chunjiin_global_state.hangul.chosung, "ㅁ") == 0) cho = 6;
    else if (strcmp(chunjiin_global_state.hangul.chosung, "ㅂ") == 0) cho = 7;
    else if (strcmp(chunjiin_global_state.hangul.chosung, "ㅃ") == 0) cho = 8;
    else if (strcmp(chunjiin_global_state.hangul.chosung, "ㅅ") == 0) cho = 9;
    else if (strcmp(chunjiin_global_state.hangul.chosung, "ㅆ") == 0) cho = 10;
    else if (strcmp(chunjiin_global_state.hangul.chosung, "ㅇ") == 0) cho = 11;
    else if (strcmp(chunjiin_global_state.hangul.chosung, "ㅈ") == 0) cho = 12;
    else if (strcmp(chunjiin_global_state.hangul.chosung, "ㅉ") == 0) cho = 13;
    else if (strcmp(chunjiin_global_state.hangul.chosung, "ㅊ") == 0) cho = 14;
    else if (strcmp(chunjiin_global_state.hangul.chosung, "ㅋ") == 0) cho = 15;
    else if (strcmp(chunjiin_global_state.hangul.chosung, "ㅌ") == 0) cho = 16;
    else if (strcmp(chunjiin_global_state.hangul.chosung, "ㅍ") == 0) cho = 17;
    else cho = 18; // ㅎ
    
    if (strlen(chunjiin_global_state.hangul.jungsung) == 0 && strlen(chunjiin_global_state.hangul.jongsung) == 0) {
        return 0x1100 + cho;
    }
    if (strcmp(chunjiin_global_state.hangul.jungsung, "·") == 0 || strcmp(chunjiin_global_state.hangul.jungsung, "‥") == 0) {
        return 0x1100 + cho;
    }
    
    // 중성 인덱스 찾기
    if (strcmp(chunjiin_global_state.hangul.jungsung, "ㅏ") == 0) jung = 0;
    else if (strcmp(chunjiin_global_state.hangul.jungsung, "ㅐ") == 0) jung = 1;
    else if (strcmp(chunjiin_global_state.hangul.jungsung, "ㅑ") == 0) jung = 2;
    else if (strcmp(chunjiin_global_state.hangul.jungsung, "ㅒ") == 0) jung = 3;
    else if (strcmp(chunjiin_global_state.hangul.jungsung, "ㅓ") == 0) jung = 4;
    else if (strcmp(chunjiin_global_state.hangul.jungsung, "ㅔ") == 0) jung = 5;
    else if (strcmp(chunjiin_global_state.hangul.jungsung, "ㅕ") == 0) jung = 6;
    else if (strcmp(chunjiin_global_state.hangul.jungsung, "ㅖ") == 0) jung = 7;
    else if (strcmp(chunjiin_global_state.hangul.jungsung, "ㅗ") == 0) jung = 8;
    else if (strcmp(chunjiin_global_state.hangul.jungsung, "ㅘ") == 0) jung = 9;
    else if (strcmp(chunjiin_global_state.hangul.jungsung, "ㅙ") == 0) jung = 10;
    else if (strcmp(chunjiin_global_state.hangul.jungsung, "ㅚ") == 0) jung = 11;
    else if (strcmp(chunjiin_global_state.hangul.jungsung, "ㅛ") == 0) jung = 12;
    else if (strcmp(chunjiin_global_state.hangul.jungsung, "ㅜ") == 0) jung = 13;
    else if (strcmp(chunjiin_global_state.hangul.jungsung, "ㅝ") == 0) jung = 14;
    else if (strcmp(chunjiin_global_state.hangul.jungsung, "ㅞ") == 0) jung = 15;
    else if (strcmp(chunjiin_global_state.hangul.jungsung, "ㅟ") == 0) jung = 16;
    else if (strcmp(chunjiin_global_state.hangul.jungsung, "ㅠ") == 0) jung = 17;
    else if (strcmp(chunjiin_global_state.hangul.jungsung, "ㅡ") == 0) jung = 18;
    else if (strcmp(chunjiin_global_state.hangul.jungsung, "ㅢ") == 0) jung = 19;
    else jung = 20; // ㅣ
    
    if (strlen(chunjiin_global_state.hangul.chosung) == 0 && strlen(chunjiin_global_state.hangul.jongsung) == 0) {
        return 0x1161 + jung;
    }
    
    // 종성 인덱스 찾기
    if (strlen(real_jong) == 0) jong = 0;
    else if (strcmp(real_jong, "ㄱ") == 0) jong = 1;
    else if (strcmp(real_jong, "ㄲ") == 0) jong = 2;
    else if (strcmp(real_jong, "ㄳ") == 0) jong = 3;
    else if (strcmp(real_jong, "ㄴ") == 0) jong = 4;
    else if (strcmp(real_jong, "ㄵ") == 0) jong = 5;
    else if (strcmp(real_jong, "ㄶ") == 0) jong = 6;
    else if (strcmp(real_jong, "ㄷ") == 0) jong = 7;
    else if (strcmp(real_jong, "ㄹ") == 0) jong = 8;
    else if (strcmp(real_jong, "ㄺ") == 0) jong = 9;
    else if (strcmp(real_jong, "ㄻ") == 0) jong = 10;
    else if (strcmp(real_jong, "ㄼ") == 0) jong = 11;
    else if (strcmp(real_jong, "ㄽ") == 0) jong = 12;
    else if (strcmp(real_jong, "ㄾ") == 0) jong = 13;
    else if (strcmp(real_jong, "ㄿ") == 0) jong = 14;
    else if (strcmp(real_jong, "ㅀ") == 0) jong = 15;
    else if (strcmp(real_jong, "ㅁ") == 0) jong = 16;
    else if (strcmp(real_jong, "ㅂ") == 0) jong = 17;
    else if (strcmp(real_jong, "ㅄ") == 0) jong = 18;
    else if (strcmp(real_jong, "ㅅ") == 0) jong = 19;
    else if (strcmp(real_jong, "ㅆ") == 0) jong = 20;
    else if (strcmp(real_jong, "ㅇ") == 0) jong = 21;
    else if (strcmp(real_jong, "ㅈ") == 0) jong = 22;
    else if (strcmp(real_jong, "ㅊ") == 0) jong = 23;
    else if (strcmp(real_jong, "ㅋ") == 0) jong = 24;
    else if (strcmp(real_jong, "ㅌ") == 0) jong = 25;
    else if (strcmp(real_jong, "ㅍ") == 0) jong = 26;
    else jong = 27; // ㅎ
    
    if (strlen(chunjiin_global_state.hangul.chosung) == 0 && strlen(chunjiin_global_state.hangul.jungsung) == 0) {
        return 0x11a8 + jong;
    }
    
    return HANGUL_BASE + cho * 588 + jung * 28 + jong;
}

// Java checkDouble() 함수 변환
const char* chunjiin_check_double(const char* jong, const char* jong2) {
    if (strcmp(jong, "ㄱ") == 0 && strcmp(jong2, "ㅅ") == 0) return "ㄳ";
    else if (strcmp(jong, "ㄴ") == 0 && strcmp(jong2, "ㅈ") == 0) return "ㄵ";
    else if (strcmp(jong, "ㄴ") == 0 && strcmp(jong2, "ㅎ") == 0) return "ㄶ";
    else if (strcmp(jong, "ㄹ") == 0 && strcmp(jong2, "ㄱ") == 0) return "ㄺ";
    else if (strcmp(jong, "ㄹ") == 0 && strcmp(jong2, "ㅁ") == 0) return "ㄻ";
    else if (strcmp(jong, "ㄹ") == 0 && strcmp(jong2, "ㅂ") == 0) return "ㄼ";
    else if (strcmp(jong, "ㄹ") == 0 && strcmp(jong2, "ㅅ") == 0) return "ㄽ";
    else if (strcmp(jong, "ㄹ") == 0 && strcmp(jong2, "ㅌ") == 0) return "ㄾ";
    else if (strcmp(jong, "ㄹ") == 0 && strcmp(jong2, "ㅍ") == 0) return "ㄿ";
    else if (strcmp(jong, "ㄹ") == 0 && strcmp(jong2, "ㅎ") == 0) return "ㅀ";
    else if (strcmp(jong, "ㅂ") == 0 && strcmp(jong2, "ㅅ") == 0) return "ㅄ";
    return "";
}

// Java hangulMake() 함수 변환
void chunjiin_input_key(int input) {
    char beforedata[8] = "";
    char nowdata[8] = "";
    char overdata[8] = "";
    
    if (input == 10) { // 띄어쓰기
        if (chunjiin_global_state.hangul.flag_writing) {
            hangul_init(&chunjiin_global_state.hangul);
        } else {
            chunjiin_global_state.hangul.flag_space = 1;
        }
    } else if (input == 11) { // 지우기
        if (chunjiin_global_state.hangul.step == 0) {
            if (strlen(chunjiin_global_state.hangul.chosung) == 0) {
                chunjiin_delete();
                chunjiin_global_state.hangul.flag_writing = 0;
            } else {
                memset(chunjiin_global_state.hangul.chosung, 0, sizeof(chunjiin_global_state.hangul.chosung));
            }
        } else if (chunjiin_global_state.hangul.step == 1) {
            if (strcmp(chunjiin_global_state.hangul.jungsung, "·") == 0 || strcmp(chunjiin_global_state.hangul.jungsung, "‥") == 0) {
                chunjiin_delete();
                if (strlen(chunjiin_global_state.hangul.chosung) == 0) {
                    chunjiin_global_state.hangul.flag_writing = 0;
                }
            }
            memset(chunjiin_global_state.hangul.jungsung, 0, sizeof(chunjiin_global_state.hangul.jungsung));
            chunjiin_global_state.hangul.step = 0;
        } else if (chunjiin_global_state.hangul.step == 2) {
            memset(chunjiin_global_state.hangul.jongsung, 0, sizeof(chunjiin_global_state.hangul.jongsung));
            chunjiin_global_state.hangul.step = 1;
        } else if (chunjiin_global_state.hangul.step == 3) {
            memset(chunjiin_global_state.hangul.jongsung2, 0, sizeof(chunjiin_global_state.hangul.jongsung2));
            chunjiin_global_state.hangul.step = 2;
        }
    } else if (input == 1 || input == 2 || input == 3) { // 모음
        int batchim = 0;
        if (chunjiin_global_state.hangul.step == 2) {
            chunjiin_delete();
            strcpy(beforedata, chunjiin_global_state.hangul.jongsung);
            if (!chunjiin_global_state.hangul.flag_doubled) {
                memset(chunjiin_global_state.hangul.jongsung, 0, sizeof(chunjiin_global_state.hangul.jongsung));
                chunjiin_global_state.hangul.flag_writing = 0;
                chunjiin_update_input_buffer();
            }
            hangul_init(&chunjiin_global_state.hangul);
            strcpy(chunjiin_global_state.hangul.chosung, beforedata);
            chunjiin_global_state.hangul.step = 0;
            batchim = 1;
        } else if (chunjiin_global_state.hangul.step == 3) {
            strcpy(beforedata, chunjiin_global_state.hangul.jongsung2);
            if (chunjiin_global_state.hangul.flag_doubled) {
                chunjiin_delete();
            } else {
                chunjiin_delete();
                memset(chunjiin_global_state.hangul.jongsung2, 0, sizeof(chunjiin_global_state.hangul.jongsung2));
                chunjiin_global_state.hangul.flag_writing = 0;
                chunjiin_update_input_buffer();
            }
            hangul_init(&chunjiin_global_state.hangul);
            strcpy(chunjiin_global_state.hangul.chosung, beforedata);
            chunjiin_global_state.hangul.step = 0;
            batchim = 1;
        }
        
        strcpy(beforedata, chunjiin_global_state.hangul.jungsung);
        chunjiin_global_state.hangul.step = 1;
        
        if (input == 1) { // ㅣ ㅓ ㅕ ㅐ ㅔ ㅖㅒ ㅚ ㅟ ㅙ ㅝ ㅞ ㅢ
            if (strlen(beforedata) == 0) strcpy(nowdata, "ㅣ");
            else if (strcmp(beforedata, "·") == 0) {
                strcpy(nowdata, "ㅓ");
                chunjiin_global_state.hangul.flag_dotused = 1;
            } else if (strcmp(beforedata, "‥") == 0) {
                strcpy(nowdata, "ㅕ");
                chunjiin_global_state.hangul.flag_dotused = 1;
            } else if (strcmp(beforedata, "ㅏ") == 0) strcpy(nowdata, "ㅐ");
            else if (strcmp(beforedata, "ㅑ") == 0) strcpy(nowdata, "ㅒ");
            else if (strcmp(beforedata, "ㅓ") == 0) strcpy(nowdata, "ㅔ");
            else if (strcmp(beforedata, "ㅕ") == 0) strcpy(nowdata, "ㅖ");
            else if (strcmp(beforedata, "ㅗ") == 0) strcpy(nowdata, "ㅚ");
            else if (strcmp(beforedata, "ㅜ") == 0) strcpy(nowdata, "ㅟ");
            else if (strcmp(beforedata, "ㅠ") == 0) strcpy(nowdata, "ㅝ");
            else if (strcmp(beforedata, "ㅘ") == 0) strcpy(nowdata, "ㅙ");
            else if (strcmp(beforedata, "ㅝ") == 0) strcpy(nowdata, "ㅞ");
            else if (strcmp(beforedata, "ㅡ") == 0) strcpy(nowdata, "ㅢ");
            else {
                hangul_init(&chunjiin_global_state.hangul);
                chunjiin_global_state.hangul.step = 1;
                strcpy(nowdata, "ㅣ");
            }
        } else if (input == 2) { // ·,‥,ㅏ,ㅑ,ㅜ,ㅠ,ㅘ
            if (strlen(beforedata) == 0) {
                strcpy(nowdata, "·");
                if (batchim) chunjiin_global_state.hangul.flag_addcursor = 1;
            } else if (strcmp(beforedata, "·") == 0) {
                strcpy(nowdata, "‥");
                chunjiin_global_state.hangul.flag_dotused = 1;
            } else if (strcmp(beforedata, "‥") == 0) {
                strcpy(nowdata, "·");
                chunjiin_global_state.hangul.flag_dotused = 1;
            } else if (strcmp(beforedata, "ㅣ") == 0) strcpy(nowdata, "ㅏ");
            else if (strcmp(beforedata, "ㅏ") == 0) strcpy(nowdata, "ㅑ");
            else if (strcmp(beforedata, "ㅡ") == 0) strcpy(nowdata, "ㅜ");
            else if (strcmp(beforedata, "ㅜ") == 0) strcpy(nowdata, "ㅠ");
            else if (strcmp(beforedata, "ㅚ") == 0) strcpy(nowdata, "ㅘ");
            else {
                hangul_init(&chunjiin_global_state.hangul);
                chunjiin_global_state.hangul.step = 1;
                strcpy(nowdata, "·");
            }
        } else if (input == 3) { // ㅡ, ㅗ, ㅛ
            if (strlen(beforedata) == 0) strcpy(nowdata, "ㅡ");
            else if (strcmp(beforedata, "·") == 0) {
                strcpy(nowdata, "ㅗ");
                chunjiin_global_state.hangul.flag_dotused = 1;
            } else if (strcmp(beforedata, "‥") == 0) {
                strcpy(nowdata, "ㅛ");
                chunjiin_global_state.hangul.flag_dotused = 1;
            } else {
                hangul_init(&chunjiin_global_state.hangul);
                chunjiin_global_state.hangul.step = 1;
                strcpy(nowdata, "ㅡ");
            }
        }
        strcpy(chunjiin_global_state.hangul.jungsung, nowdata);
    } else { // 자음
        if (chunjiin_global_state.hangul.step == 1) {
            if (strcmp(chunjiin_global_state.hangul.jungsung, "·") == 0 || strcmp(chunjiin_global_state.hangul.jungsung, "‥") == 0) {
                hangul_init(&chunjiin_global_state.hangul);
            } else {
                chunjiin_global_state.hangul.step = 2;
            }
        }
        
        if (chunjiin_global_state.hangul.step == 0) strcpy(beforedata, chunjiin_global_state.hangul.chosung);
        else if (chunjiin_global_state.hangul.step == 2) strcpy(beforedata, chunjiin_global_state.hangul.jongsung);
        else if (chunjiin_global_state.hangul.step == 3) strcpy(beforedata, chunjiin_global_state.hangul.jongsung2);
        
        // Java 자음 처리 로직을 그대로 변환
        if (input == 4) { // ㄱ, ㅋ, ㄲ, ㄺ
            if (strlen(beforedata) == 0) {
                if (chunjiin_global_state.hangul.step == 2) {
                    if (strlen(chunjiin_global_state.hangul.chosung) == 0) {
                        strcpy(overdata, "ㄱ");
                    } else {
                        strcpy(nowdata, "ㄱ");
                    }
                } else {
                    strcpy(nowdata, "ㄱ");
                }
            } else if (strcmp(beforedata, "ㄱ") == 0) strcpy(nowdata, "ㅋ");
            else if (strcmp(beforedata, "ㅋ") == 0) strcpy(nowdata, "ㄲ");
            else if (strcmp(beforedata, "ㄲ") == 0) strcpy(nowdata, "ㄱ");
            else if (strcmp(beforedata, "ㄹ") == 0 && chunjiin_global_state.hangul.step == 2) {
                chunjiin_global_state.hangul.step = 3;
                strcpy(nowdata, "ㄱ");
            } else {
                strcpy(overdata, "ㄱ");
            }
        } else if (input == 5) { // ㄴ ㄹ
            if (strlen(beforedata) == 0) {
                if (chunjiin_global_state.hangul.step == 2) {
                    if (strlen(chunjiin_global_state.hangul.chosung) == 0) {
                        strcpy(overdata, "ㄴ");
                    } else {
                        strcpy(nowdata, "ㄴ");
                    }
                } else {
                    strcpy(nowdata, "ㄴ");
                }
            } else if (strcmp(beforedata, "ㄴ") == 0) strcpy(nowdata, "ㄹ");
            else if (strcmp(beforedata, "ㄹ") == 0) strcpy(nowdata, "ㄴ");
            else {
                strcpy(overdata, "ㄴ");
            }
        } else if (input == 6) { // ㄷ, ㅌ, ㄸ, ㄾ
            if (strlen(beforedata) == 0) {
                if (chunjiin_global_state.hangul.step == 2) {
                    if (strlen(chunjiin_global_state.hangul.chosung) == 0) {
                        strcpy(overdata, "ㄷ");
                    } else {
                        strcpy(nowdata, "ㄷ");
                    }
                } else {
                    strcpy(nowdata, "ㄷ");
                }
            } else if (strcmp(beforedata, "ㄷ") == 0) strcpy(nowdata, "ㅌ");
            else if (strcmp(beforedata, "ㅌ") == 0) strcpy(nowdata, "ㄸ");
            else if (strcmp(beforedata, "ㄸ") == 0) strcpy(nowdata, "ㄷ");
            else if (strcmp(beforedata, "ㄹ") == 0 && chunjiin_global_state.hangul.step == 2) {
                chunjiin_global_state.hangul.step = 3;
                strcpy(nowdata, "ㄷ");
            } else {
                strcpy(overdata, "ㄷ");
            }
        } else if (input == 7) { // ㅂ, ㅍ, ㅃ, ㄼ, ㄿ
            if (strlen(beforedata) == 0) {
                if (chunjiin_global_state.hangul.step == 2) {
                    if (strlen(chunjiin_global_state.hangul.chosung) == 0) {
                        strcpy(overdata, "ㅂ");
                    } else {
                        strcpy(nowdata, "ㅂ");
                    }
                } else {
                    strcpy(nowdata, "ㅂ");
                }
            } else if (strcmp(beforedata, "ㅂ") == 0) strcpy(nowdata, "ㅍ");
            else if (strcmp(beforedata, "ㅍ") == 0) strcpy(nowdata, "ㅃ");
            else if (strcmp(beforedata, "ㅃ") == 0) strcpy(nowdata, "ㅂ");
            else if (strcmp(beforedata, "ㄹ") == 0 && chunjiin_global_state.hangul.step == 2) {
                chunjiin_global_state.hangul.step = 3;
                strcpy(nowdata, "ㅂ");
            } else {
                strcpy(overdata, "ㅂ");
            }
        } else if (input == 8) { // ㅅ, ㅎ, ㅆ, ㄳ, ㄶ, ㄽ, ㅀ, ㅄ
            if (strlen(beforedata) == 0) {
                if (chunjiin_global_state.hangul.step == 2) {
                    if (strlen(chunjiin_global_state.hangul.chosung) == 0) {
                        strcpy(overdata, "ㅅ");
                    } else {
                        strcpy(nowdata, "ㅅ");
                    }
                } else {
                    strcpy(nowdata, "ㅅ");
                }
            } else if (strcmp(beforedata, "ㅅ") == 0) strcpy(nowdata, "ㅎ");
            else if (strcmp(beforedata, "ㅎ") == 0) strcpy(nowdata, "ㅆ");
            else if (strcmp(beforedata, "ㅆ") == 0) strcpy(nowdata, "ㅅ");
            else if (strcmp(beforedata, "ㄱ") == 0 && chunjiin_global_state.hangul.step == 2) {
                chunjiin_global_state.hangul.step = 3;
                strcpy(nowdata, "ㅅ");
            } else if (strcmp(beforedata, "ㄴ") == 0 && chunjiin_global_state.hangul.step == 2) {
                chunjiin_global_state.hangul.step = 3;
                strcpy(nowdata, "ㅅ");
            } else if (strcmp(beforedata, "ㄹ") == 0 && chunjiin_global_state.hangul.step == 2) {
                chunjiin_global_state.hangul.step = 3;
                strcpy(nowdata, "ㅅ");
            } else if (strcmp(beforedata, "ㅂ") == 0 && chunjiin_global_state.hangul.step == 2) {
                chunjiin_global_state.hangul.step = 3;
                strcpy(nowdata, "ㅅ");
            } else {
                strcpy(overdata, "ㅅ");
            }
        } else if (input == 9) { // ㅈ, ㅊ, ㅉ, ㄵ
            if (strlen(beforedata) == 0) {
                if (chunjiin_global_state.hangul.step == 2) {
                    if (strlen(chunjiin_global_state.hangul.chosung) == 0) {
                        strcpy(overdata, "ㅈ");
                    } else {
                        strcpy(nowdata, "ㅈ");
                    }
                } else {
                    strcpy(nowdata, "ㅈ");
                }
            } else if (strcmp(beforedata, "ㅈ") == 0) strcpy(nowdata, "ㅊ");
            else if (strcmp(beforedata, "ㅊ") == 0) strcpy(nowdata, "ㅉ");
            else if (strcmp(beforedata, "ㅉ") == 0) strcpy(nowdata, "ㅈ");
            else if (strcmp(beforedata, "ㄴ") == 0 && chunjiin_global_state.hangul.step == 2) {
                chunjiin_global_state.hangul.step = 3;
                strcpy(nowdata, "ㅈ");
            } else {
                strcpy(overdata, "ㅈ");
            }
        } else if (input == 0) { // ㅇ, ㅁ, ㄻ
            if (strlen(beforedata) == 0) {
                if (chunjiin_global_state.hangul.step == 2) {
                    if (strlen(chunjiin_global_state.hangul.chosung) == 0) {
                        strcpy(overdata, "ㅇ");
                    } else {
                        strcpy(nowdata, "ㅇ");
                    }
                } else {
                    strcpy(nowdata, "ㅇ");
                }
            } else if (strcmp(beforedata, "ㅇ") == 0) strcpy(nowdata, "ㅁ");
            else if (strcmp(beforedata, "ㅁ") == 0) strcpy(nowdata, "ㅇ");
            else if (strcmp(beforedata, "ㄹ") == 0 && chunjiin_global_state.hangul.step == 2) {
                chunjiin_global_state.hangul.step = 3;
                strcpy(nowdata, "ㅇ");
            } else {
                strcpy(overdata, "ㅇ");
            }
        }
        
        if (strlen(nowdata) > 0) {
            if (chunjiin_global_state.hangul.step == 0) {
                strcpy(chunjiin_global_state.hangul.chosung, nowdata);
            } else if (chunjiin_global_state.hangul.step == 2) {
                strcpy(chunjiin_global_state.hangul.jongsung, nowdata);
            } else {
                strcpy(chunjiin_global_state.hangul.jongsung2, nowdata);
            }
        }
        if (strlen(overdata) > 0) {
            chunjiin_global_state.hangul.flag_writing = 0;
            hangul_init(&chunjiin_global_state.hangul);
            strcpy(chunjiin_global_state.hangul.chosung, overdata);
        }
    }
    
    chunjiin_update_input_buffer();
}

// 기존 함수들을 Java 기반으로 수정
void chunjiin_input_element(chunjiin_element_t element) {
    int input = -1;
    
    switch (element) {
        case CHUNJIIN_I: 
            input = 1; 
            break;
        case CHUNJIIN_DOT: 
            input = 2; 
            break;
        case CHUNJIIN_EU: 
            input = 3; 
            break;
    }
    
    if (input != -1) {
        chunjiin_input_key(input);
    }
}

void chunjiin_input_consonant_group(chunjiin_consonant_group_t group) {
    int input = -1;
    
    switch (group) {
        case CHUNJIIN_GIYEOK_KIYEOK: 
            input = 4; 
            break;
        case CHUNJIIN_NIEUN_RIEUL: 
            input = 5; 
            break;
        case CHUNJIIN_DIGEUT_TIEUT: 
            input = 6; 
            break;
        case CHUNJIIN_BIEUP_PIEUP: 
            input = 7; 
            break;
        case CHUNJIIN_SIOT_HIEUT: 
            input = 8; 
            break;
        case CHUNJIIN_JIEUT_CHIEUT: 
            input = 9; 
            break;
        case CHUNJIIN_IEUNG_MIEUM: 
            input = 0; 
            break;
    }
    
    if (input != -1) {
        chunjiin_input_key(input);
    }
}

// Java write() 함수 기반
void chunjiin_update_input_buffer(void) {
    // Java write() 함수의 로직을 구현
    char real_jongsung[8] = "";
    const char* double_result = chunjiin_check_double(chunjiin_global_state.hangul.jongsung, chunjiin_global_state.hangul.jongsung2);
    
    if (strlen(double_result) == 0) {
        strcpy(real_jongsung, chunjiin_global_state.hangul.jongsung);
        if (strlen(chunjiin_global_state.hangul.jongsung2) != 0) {
            chunjiin_global_state.hangul.flag_doubled = 1;
        }
    } else {
        strcpy(real_jongsung, double_result);
    }
    
    int unicode = chunjiin_get_unicode(real_jongsung);
    
    // UTF-8 변환
    char utf8_char[8] = "";
    if (unicode > 0) {
        if (unicode < 0x80) {
            utf8_char[0] = (char)unicode;
        } else if (unicode < 0x800) {
            utf8_char[0] = 0xC0 | (unicode >> 6);
            utf8_char[1] = 0x80 | (unicode & 0x3F);
        } else {
            utf8_char[0] = 0xE0 | (unicode >> 12);
            utf8_char[1] = 0x80 | ((unicode >> 6) & 0x3F);
            utf8_char[2] = 0x80 | (unicode & 0x3F);
        }
    }
    
    // 출력 버퍼 업데이트
    if (strlen(utf8_char) > 0) {
        strcpy(chunjiin_global_state.output_buffer, utf8_char);
        chunjiin_global_state.output_length = strlen(chunjiin_global_state.output_buffer);
    }
    
    printf("Updated output buffer: '%s'\n", chunjiin_global_state.output_buffer);
}

// Java delete() 함수
void chunjiin_delete(void) {
    // 간단한 삭제 구현
    if (chunjiin_global_state.output_length > 0) {
        chunjiin_global_state.output_buffer[chunjiin_global_state.output_length - 1] = '\0';
        chunjiin_global_state.output_length--;
    }
}

// 나머지 함수들
void chunjiin_input_keyboard(char key) {
    // 키보드 입력 처리 - 직접 출력 버퍼에 추가
    char key_str[2] = {key, '\0'};
    strcat(chunjiin_global_state.output_buffer, key_str);
    chunjiin_global_state.output_length = strlen(chunjiin_global_state.output_buffer);
    chunjiin_update_input_buffer();
}

void chunjiin_clear_keyboard_buffer(void) {
    chunjiin_clear_all_buffers();
}

void chunjiin_toggle_input_mode(void) {
    if (chunjiin_global_state.input_mode == INPUT_MODE_KOREAN) {
        chunjiin_global_state.input_mode = INPUT_MODE_NUMBER;
        printf("입력 모드 변경: 한글 → 숫자\n");
    } else {
        chunjiin_global_state.input_mode = INPUT_MODE_KOREAN;
        printf("입력 모드 변경: 숫자 → 한글\n");
    }
}

input_mode_t chunjiin_get_input_mode(void) {
    return chunjiin_global_state.input_mode;
}

void chunjiin_clear_all_buffers(void) {
    chunjiin_global_state.output_buffer[0] = '\0';
    chunjiin_global_state.output_length = 0;
    hangul_init(&chunjiin_global_state.hangul);
    chunjiin_update_input_buffer();
}

void chunjiin_enter(void) {
    // Enter 키 처리
    chunjiin_update_input_buffer();
    printf("Enter pressed, output buffer: '%s'\n", chunjiin_global_state.output_buffer);
}

void chunjiin_backspace(void) {
    chunjiin_delete();
    chunjiin_update_input_buffer();
}

void chunjiin_clear(void) {
    chunjiin_clear_all_buffers();
}

const char* chunjiin_get_current_text(void) {
    return chunjiin_global_state.output_buffer;
}