#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// wchar_t 와 wprintf 등 와이드 캐릭터 지원을 위해 필요한 헤더
#include <wchar.h>
#include <locale.h>

// --- 상태 정의 ---
// 현재 글자가 어떤 단계까지 조합되었는지 나타내는 상태
typedef enum {
    STATE_START,          // 초기 상태
    STATE_CHOSEONG,       // 초성 입력됨
    STATE_JUNGSEONG,      // 중성 입력됨
    STATE_JONGSEONG,      // 종성 입력됨
} SyllableState;

// --- 현재 조합 중인 글자의 정보를 담는 구조체 ---
typedef struct {
    int cho;              // 초성 인덱스
    int jung;             // 중성 인덱스
    int jong;             // 종성 인덱스
    SyllableState state;  // 현재 조합 상태

    // 모음 조합을 위해 마지막으로 입력된 모음 키를 임시 저장
    int temp_vowel;
    // 자음 조합(쌍자음 등)을 위해 마지막으로 입력된 자음 키를 임시 저장
    int temp_consonant;

} CurrentSyllable;

// --- 전역 변수 ---
wchar_t g_output_buffer[1024];      // 최종 완성된 문자열을 저장하는 버퍼
CurrentSyllable g_current_syllable; // 현재 조합 중인 글자

// --- 한글 자모 유니코드 인덱스 테이블 ---
// 초성 (19개): ㄱ, ㄲ, ㄴ, ㄷ, ㄸ, ㄹ, ㅁ, ㅂ, ㅃ, ㅅ, ㅆ, ㅇ, ㅈ, ㅉ, ㅊ, ㅋ, ㅌ, ㅍ, ㅎ
const int CHOSEONG_TABLE[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18};
// 중성 (21개): ㅏ, ㅐ, ㅑ, ㅒ, ㅓ, ㅔ, ㅕ, ㅖ, ㅗ, ㅘ, ㅙ, ㅚ, ㅛ, ㅜ, ㅝ, ㅞ, ㅟ, ㅠ, ㅡ, ㅢ, ㅣ
const int JUNGSEONG_TABLE[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
// 종성 (28개, 0은 종성 없음): (없음), ㄱ, ㄲ, ㄳ, ㄴ, ㄵ, ㄶ, ㄷ, ㄹ, ㄺ, ㄻ, ㄼ, ㄽ, ㄾ, ㄿ, ㅀ, ㅁ, ㅂ, ㅄ, ㅅ, ㅆ, ㅇ, ㅈ, ㅊ, ㅋ, ㅌ, ㅍ
const int JONGSEONG_TABLE[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27};

// --- 함수 프로토타입 ---
void initialize();
void process_input(char key);
void handle_consonant(int key_code);
void handle_vowel(int key_code);
void handle_special(char key);
void finalize_syllable();
void reset_current_syllable();
void display();
wchar_t get_composing_char();
wchar_t combine_syllable(int cho, int jung, int jong);

// --- 메인 함수 --- (removed for library use)
// int main() {
//     // 시스템 로케일을 설정하여 wprintf가 유니코드 문자를 올바르게 출력하도록 함
//     setlocale(LC_ALL, "");
// 
//     initialize();
// 
//     wprintf(L"--- 천지인 키보드 시뮬레이터 ---\n");
//     wprintf(L"자음: g, n, d, b, s, j, m\n");
//     wprintf(L"모음: a(ㆍ), e(ㅡ), i(ㅣ)\n");
//     wprintf(L"특수: space, <(백스페이스), .(엔터)\n");
//     wprintf(L"----------------------------------\n");
// 
//     char input_key;
//     while (1) {
//         display();
//         input_key = getchar();
//         // getchar()로 인한 개행문자 입력을 무시
//         if (input_key == '\n' || input_key == '\r') {
//             continue;
//         }
//         process_input(input_key);
//     }
// 
//     return 0;
// }

// --- 시스템 초기화 함수 ---
void initialize() {
    wmemset(g_output_buffer, 0, 1024); // 출력 버퍼 초기화
    reset_current_syllable();          // 현재 조합 글자 초기화
}

// --- 현재 조합 중인 글자 초기화 함수 ---
void reset_current_syllable() {
    g_current_syllable.cho = -1;
    g_current_syllable.jung = -1;
    g_current_syllable.jong = -1;
    g_current_syllable.state = STATE_START;
    g_current_syllable.temp_vowel = 0;
    g_current_syllable.temp_consonant = 0;
}

// --- 글자 조합 완료 및 버퍼 추가 함수 ---
void finalize_syllable() {
    if (g_current_syllable.state != STATE_START) {
        // 현재 조합 상태에 따라 완성된 글자 또는 자모를 가져옴
        wchar_t ch = get_composing_char();
        if (ch != 0) {
            wcscat(g_output_buffer, &ch);
        }
    }
    // 다음 입력을 위해 현재 조합 상태 초기화
    reset_current_syllable();
}

// --- 입력 키 처리 라우팅 함수 ---
void process_input(char key) {
    // 자음 입력 처리
    if (strchr("gndbsjm", key)) {
        int key_code = 0;
        switch(key) {
            case 'g': key_code = 1; break; // ㄱ, ㅋ, ㄲ
            case 'n': key_code = 2; break; // ㄴ, ㄹ
            case 'd': key_code = 3; break; // ㄷ, ㅌ, ㄸ
            case 'b': key_code = 4; break; // ㅂ, ㅍ, ㅃ
            case 's': key_code = 5; break; // ㅅ, ㅎ, ㅆ
            case 'j': key_code = 6; break; // ㅈ, ㅊ, ㅉ
            case 'm': key_code = 7; break; // ㅇ, ㅁ
        }
        handle_consonant(key_code);
    }
    // 모음 입력 처리
    else if (strchr("aei", key)) {
        int key_code = 0;
        switch(key) {
            case 'a': key_code = 1; break; // ㆍ (아래아)
            case 'e': key_code = 2; break; // ㅡ
            case 'i': key_code = 3; break; // ㅣ
        }
        handle_vowel(key_code);
    }
    // 특수키 입력 처리
    else if (strchr(" <.", key)) {
        handle_special(key);
    }
}

// --- 자음 입력 처리 함수 ---
void handle_consonant(int key_code) {
    g_current_syllable.temp_vowel = 0; // 자음이 입력되면 모음 조합 상태는 초기화

    // 1. 초성이 비어있는 경우 (새 글자 시작)
    if (g_current_syllable.state == STATE_START) {
        const int choseong_map[] = {-1, 0, 2, 3, 7, 9, 12, 11}; // ㄱ, ㄴ, ㄷ, ㅂ, ㅅ, ㅈ, ㅇ
        g_current_syllable.cho = choseong_map[key_code];
        g_current_syllable.state = STATE_CHOSEONG;
        g_current_syllable.temp_consonant = key_code;
        printf("DEBUG: handle_consonant - new syllable started, cho = %d\n", g_current_syllable.cho);
        return;
    }

    // 2. 초성만 입력된 경우 (초성 변경 또는 쌍자음 조합)
    if (g_current_syllable.state == STATE_CHOSEONG) {
        // 같은 자음키가 다시 입력된 경우
        if (g_current_syllable.temp_consonant == key_code) {
            switch(key_code) {
                case 1: // g: ㄱ -> ㅋ -> ㄲ
                    if (g_current_syllable.cho == 0) g_current_syllable.cho = 15;      // ㄱ -> ㅋ
                    else if (g_current_syllable.cho == 15) g_current_syllable.cho = 1; // ㅋ -> ㄲ
                    else g_current_syllable.cho = 0;                                   // ㄲ -> ㄱ
                    break;
                case 2: // n: ㄴ -> ㄹ
                    g_current_syllable.cho = (g_current_syllable.cho == 2) ? 5 : 2;
                    break;
                case 3: // d: ㄷ -> ㅌ -> ㄸ
                    if (g_current_syllable.cho == 3) g_current_syllable.cho = 16;      // ㄷ -> ㅌ
                    else if (g_current_syllable.cho == 16) g_current_syllable.cho = 4; // ㅌ -> ㄸ
                    else g_current_syllable.cho = 3;                                   // ㄸ -> ㄷ
                    break;
                case 4: // b: ㅂ -> ㅍ -> ㅃ
                    if (g_current_syllable.cho == 7) g_current_syllable.cho = 17;      // ㅂ -> ㅍ
                    else if (g_current_syllable.cho == 17) g_current_syllable.cho = 8; // ㅍ -> ㅃ
                    else g_current_syllable.cho = 7;                                   // ㅃ -> ㅂ
                    break;
                case 5: // s: ㅅ -> ㅎ -> ㅆ
                    if (g_current_syllable.cho == 9) g_current_syllable.cho = 18;       // ㅅ -> ㅎ
                    else if (g_current_syllable.cho == 18) g_current_syllable.cho = 10; // ㅎ -> ㅆ
                    else g_current_syllable.cho = 9;                                    // ㅆ -> ㅅ
                    break;
                case 6: // j: ㅈ -> ㅊ -> ㅉ
                    if (g_current_syllable.cho == 12) g_current_syllable.cho = 14;      // ㅈ -> ㅊ
                    else if (g_current_syllable.cho == 14) g_current_syllable.cho = 13; // ㅊ -> ㅉ
                    else g_current_syllable.cho = 12;                                   // ㅉ -> ㅈ
                    break;
                case 7: // m: ㅇ -> ㅁ
                    g_current_syllable.cho = (g_current_syllable.cho == 11) ? 6 : 11;
                    break;
            }
        } else { // 다른 자음키가 입력된 경우 -> 현재 글자 완성 후 새 글자 시작
            finalize_syllable();
            handle_consonant(key_code);
        }
        return;
    }

    // 3. 중성까지 입력된 경우 (종성으로 추가)
    if (g_current_syllable.state == STATE_JUNGSEONG) {
        const int jongseong_map[] = {-1, 1, 4, 7, 17, 19, 22, 21}; // ㄱ, ㄴ, ㄷ, ㅂ, ㅅ, ㅈ, ㅇ
        g_current_syllable.jong = jongseong_map[key_code];
        if (g_current_syllable.jong != -1) {
            g_current_syllable.state = STATE_JONGSEONG;
            g_current_syllable.temp_consonant = key_code;
            printf("DEBUG: handle_consonant - jongseong added, jong = %d\n", g_current_syllable.jong);
        }
        return;
    }

    // 4. 종성까지 입력된 경우 (복합 종성 조합 또는 새 글자 시작)
    if (g_current_syllable.state == STATE_JONGSEONG) {
        int new_jong = -1;
        
        // 같은 자음키가 다시 입력된 경우 - 종성 순환
        if (g_current_syllable.temp_consonant == key_code) {
            switch(key_code) {
                case 1: // g: ㄱ -> ㄲ
                    if (g_current_syllable.jong == 1) g_current_syllable.jong = 2;      // ㄱ -> ㄲ
                    else g_current_syllable.jong = 1;                                   // ㄲ -> ㄱ
                    break;
                case 2: // n: ㄴ -> ㄹ
                    if (g_current_syllable.jong == 4) g_current_syllable.jong = 8;      // ㄴ -> ㄹ
                    else g_current_syllable.jong = 4;                                   // ㄹ -> ㄴ
                    break;
                case 3: // d: ㄷ (종성에서는 ㄷ만 가능)
                    // ㄷ은 종성에서 순환 없음
                    break;
                case 4: // b: ㅂ (종성에서는 ㅂ만 가능)
                    // ㅂ은 종성에서 순환 없음
                    break;
                case 5: // s: ㅅ -> ㅆ
                    if (g_current_syllable.jong == 19) g_current_syllable.jong = 20;     // ㅅ -> ㅆ
                    else g_current_syllable.jong = 19;                                   // ㅆ -> ㅅ
                    break;
                case 6: // j: ㅈ (종성에서는 ㅈ만 가능)
                    // ㅈ은 종성에서 순환 없음
                    break;
                case 7: // m: ㅇ -> ㅁ
                    if (g_current_syllable.jong == 21) g_current_syllable.jong = 16;     // ㅇ -> ㅁ
                    else g_current_syllable.jong = 21;                                   // ㅁ -> ㅇ
                    break;
            }
            return;
        }
        
        // 복합 종성 조합 시도
        if (g_current_syllable.jong == 1 && key_code == 5) new_jong = 3;   // ㄱ + ㅅ = ㄳ
        else if (g_current_syllable.jong == 4 && key_code == 6) new_jong = 5;   // ㄴ + ㅈ = ㄵ
        else if (g_current_syllable.jong == 4 && key_code == 5) new_jong = 6;   // ㄴ + ㅎ = ㄶ
        else if (g_current_syllable.jong == 8 && key_code == 1) new_jong = 9;   // ㄹ + ㄱ = ㄺ
        else if (g_current_syllable.jong == 8 && key_code == 7) new_jong = 10;  // ㄹ + ㅁ = ㄻ
        else if (g_current_syllable.jong == 8 && key_code == 4) new_jong = 11;  // ㄹ + ㅂ = ㄼ
        else if (g_current_syllable.jong == 8 && key_code == 5) new_jong = 12;  // ㄹ + ㅅ = ㄽ
        else if (g_current_syllable.jong == 8 && key_code == 3) new_jong = 13;  // ㄹ + ㅌ = ㄾ
        else if (g_current_syllable.jong == 8 && key_code == 4) new_jong = 14;  // ㄹ + ㅍ = ㄿ
        else if (g_current_syllable.jong == 8 && key_code == 5) new_jong = 15;  // ㄹ + ㅎ = ㅀ
        else if (g_current_syllable.jong == 17 && key_code == 5) new_jong = 18; // ㅂ + ㅅ = ㅄ
        else if (g_current_syllable.jong == 1 && key_code == 1) new_jong = 2; // ㄱ + ㄱ = ㄲ
        else if (g_current_syllable.jong == 19 && key_code == 5) new_jong = 20; // ㅅ + ㅅ = ㅆ

        if (new_jong != -1) {
            g_current_syllable.jong = new_jong;
            g_current_syllable.temp_consonant = 0; 
        } else { 
            finalize_syllable();
            handle_consonant(key_code);
        }
    }
}

// --- 모음 입력 처리 함수 ---
void handle_vowel(int key_code) {
    g_current_syllable.temp_consonant = 0; 

    if (g_current_syllable.state == STATE_JONGSEONG) {
        int prev_cho = g_current_syllable.cho;
        int prev_jung = g_current_syllable.jung;
        int prev_jong = g_current_syllable.jong;
        
        int remaining_jong = 0; 
        int new_cho = -1;       

        switch (prev_jong) {
            case 3:  remaining_jong = 1; new_cho = 9; break;  // ㄳ -> ㄱ, ㅅ
            case 5:  remaining_jong = 4; new_cho = 12; break; // ㄵ -> ㄴ, ㅈ
            case 6:  remaining_jong = 4; new_cho = 18; break; // ㄶ -> ㄴ, ㅎ
            case 9:  remaining_jong = 8; new_cho = 0; break;  // ㄺ -> ㄹ, ㄱ
            case 10: remaining_jong = 8; new_cho = 6; break;  // ㄻ -> ㄹ, ㅁ
            case 11: remaining_jong = 8; new_cho = 7; break;  // ㄼ -> ㄹ, ㅂ
            case 12: remaining_jong = 8; new_cho = 9; break;  // ㄽ -> ㄹ, ㅅ
            case 13: remaining_jong = 8; new_cho = 16; break; // ㄾ -> ㄹ, ㅌ
            case 14: remaining_jong = 8; new_cho = 17; break; // ㄿ -> ㄹ, ㅍ
            case 15: remaining_jong = 8; new_cho = 18; break; // ㅀ -> ㄹ, ㅎ
            case 18: remaining_jong = 17; new_cho = 9; break; // ㅄ -> ㅂ, ㅅ
            default: 
                remaining_jong = 0; 
                switch(prev_jong) {
                    case 1: new_cho = 0; break;  // ㄱ
                    case 2: new_cho = 1; break;  // ㄲ
                    case 4: new_cho = 2; break;  // ㄴ
                    case 7: new_cho = 3; break;  // ㄷ
                    case 8: new_cho = 5; break;  // ㄹ
                    case 16: new_cho = 6; break; // ㅁ
                    case 17: new_cho = 7; break; // ㅂ
                    case 19: new_cho = 9; break; // ㅅ
                    case 20: new_cho = 10; break;// ㅆ
                    case 21: new_cho = 11; break;// ㅇ
                    case 22: new_cho = 12; break;// ㅈ
                    case 23: new_cho = 14; break;// ㅊ
                    case 24: new_cho = 15; break;// ㅋ
                    case 25: new_cho = 16; break;// ㅌ
                    case 26: new_cho = 17; break;// ㅍ
                    case 27: new_cho = 18; break;// ㅎ
                }
                break;
        }

        reset_current_syllable();
        wchar_t prev_char = combine_syllable(prev_cho, prev_jung, remaining_jong);
        if (prev_char != 0) {
            wcscat(g_output_buffer, &prev_char);
        }

        g_current_syllable.cho = new_cho;
        g_current_syllable.state = STATE_CHOSEONG;
    }

    if (g_current_syllable.state == STATE_START) {
        g_current_syllable.cho = 11; // 'ㅇ'
        g_current_syllable.state = STATE_CHOSEONG;
    }

    int new_jung = -1;
    printf("DEBUG: handle_vowel - key_code = %d, temp_vowel = %d, state = %d\n", 
           key_code, g_current_syllable.temp_vowel, g_current_syllable.state);
    
    if (g_current_syllable.temp_vowel == 0) {
        if (key_code == 1) { 
            g_current_syllable.temp_vowel = 100; 
            printf("DEBUG: handle_vowel - ㆍ entered, temp_vowel = 100\n");
            return; 
        } // ㆍ
        if (key_code == 2) new_jung = 18; // ㅡ
        if (key_code == 3) new_jung = 20; // ㅣ
    }
    else if (g_current_syllable.temp_vowel == 100) { // prev: ㆍ
        if (key_code == 1) { 
            g_current_syllable.temp_vowel = 200; 
            printf("DEBUG: handle_vowel - ㆍㆍ entered, temp_vowel = 200\n");
            return; 
        } // ㆍ + ㆍ
        if (key_code == 2) new_jung = 8;  // ㆍ + ㅡ = ㅗ
        if (key_code == 3) new_jung = 4;  // ㆍ + ㅣ = ㅓ
    }
    else if (g_current_syllable.temp_vowel == 200) { // prev: ㆍㆍ
        if (key_code == 2) new_jung = 12; // ㆍㆍ + ㅡ = ㅛ
        if (key_code == 3) new_jung = 6;  // ㆍㆍ + ㅣ = ㅕ
    }
    else {
        int prev_jung = g_current_syllable.jung;
        printf("DEBUG: handle_vowel - prev_jung = %d\n", prev_jung);
        
        if (prev_jung == 8 && key_code == 3) new_jung = 11;  // ㅗ + ㅣ = ㅚ
        else if (prev_jung == 13 && key_code == 3) new_jung = 16; // ㅜ + ㅣ = ㅟ
        else if (prev_jung == 18 && key_code == 3) new_jung = 19; // ㅡ + ㅣ = ㅢ
        else if (prev_jung == 0 && key_code == 3) new_jung = 1;   // ㅏ + ㅣ = ㅐ
        else if (prev_jung == 4 && key_code == 3) new_jung = 5;   // ㅓ + ㅣ = ㅔ
        else if (prev_jung == 2 && key_code == 3) new_jung = 3;   // ㅑ + ㅣ = ㅒ
        else if (prev_jung == 6 && key_code == 3) new_jung = 7;   // ㅕ + ㅣ = ㅖ
        else if (prev_jung == 20 && key_code == 1) new_jung = 0;  // ㅣ + ㆍ = ㅏ
        else if (prev_jung == 18 && key_code == 1) new_jung = 13; // ㅡ + ㆍ = ㅜ
    }

    if (new_jung != -1) {
        g_current_syllable.jung = new_jung;
        g_current_syllable.state = STATE_JUNGSEONG;
        g_current_syllable.temp_vowel = 1; 
    } else {
        finalize_syllable();
        handle_vowel(key_code);
    }
}

// --- 특수키 처리 함수 ---
void handle_special(char key) {
    switch (key) {
        case ' ':
            // 조합 중인 글자가 있으면(미완성 포함), 현재 글자를 확정하고 새 입력을 준비.
            // (예: 'ㄱ' 입력 후 space -> 'ㄱ' 확정)
            if (g_current_syllable.state != STATE_START) {
                finalize_syllable();
            } 
            // 조합 중인 글자가 없을 때만 띄어쓰기를 추가.
            else {
                wcscat(g_output_buffer, L" ");
            }
            break;
        case '.': // Enter
            finalize_syllable();
            wcscat(g_output_buffer, L"\n");
            break;
        case '<': // Backspace
            // 조합 중인 글자가 있으면 조합을 취소
            if (g_current_syllable.state != STATE_START) {
                reset_current_syllable();
            } 
            // 조합 중인 글자가 없으면 버퍼에서 마지막 글자 삭제
            else if (wcslen(g_output_buffer) > 0) {
                g_output_buffer[wcslen(g_output_buffer) - 1] = L'\0';
            }
            break;
    }
}

// --- 유니코드 조합 함수 ---
wchar_t combine_syllable(int cho, int jung, int jong) {
    // 중성이 없으면 완성된 글자가 아니므로 0을 반환 (get_composing_char에서 처리)
    if (cho == -1 || jung == -1) return 0;
    if (jong == -1) jong = 0; // 종성이 없으면 0
    return (wchar_t)(0xAC00 + (cho * 21 * 28) + (jung * 28) + jong);
}

// --- 현재 조합 중인 글자를 반환하는 함수 ---
wchar_t get_composing_char() {
    printf("DEBUG: get_composing_char() called, state = %d, cho = %d, jung = %d, jong = %d\n", 
           g_current_syllable.state, g_current_syllable.cho, g_current_syllable.jung, g_current_syllable.jong);
    
    if (g_current_syllable.state == STATE_START) {
        printf("DEBUG: Returning 0 (no composing character)\n");
        return 0; // 조합 중인 글자 없음
    }
    // ** 미완성 글자 처리 로직 **
    // 초성만 입력된 상태라면, 완성형 글자로 조합할 수 없으므로
    // 초성 자모를 그대로 반환함.
    if (g_current_syllable.state == STATE_CHOSEONG) {
        const wchar_t choseong_jamo[] = L"ㄱㄲㄴㄷㄸㄹㅁㅂㅃㅅㅆㅇㅈㅉㅊㅋㅌㅍㅎ";
        wchar_t result = choseong_jamo[g_current_syllable.cho];
        printf("DEBUG: Returning choseong jamo: 0x%04x\n", (unsigned int)result);
        return result;
    }
    // 중성 또는 종성까지 조합된 경우, 완성형 글자로 조합하여 반환
    wchar_t result = combine_syllable(g_current_syllable.cho, g_current_syllable.jung, g_current_syllable.jong);
    printf("DEBUG: Returning combined syllable: 0x%04x\n", (unsigned int)result);
    return result;
}