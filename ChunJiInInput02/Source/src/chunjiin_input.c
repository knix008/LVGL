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
int wchar_to_utf8(wchar_t wc, char *utf8_buffer, size_t buffer_size);

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
            // Check buffer bounds before appending
            size_t current_len = wcslen(g_output_buffer);
            if (current_len < 1023) {
                g_output_buffer[current_len] = ch;
                g_output_buffer[current_len + 1] = L'\0';
            }
        }
        
        // Handle dots after jongseong before resetting
        if (g_current_syllable.state == STATE_JONGSEONG && g_current_syllable.temp_vowel > 0) {
            int dot_count = (g_current_syllable.temp_vowel == 200) ? 2 : 1;
            size_t current_len = wcslen(g_output_buffer);
            
            for (int i = 0; i < dot_count && current_len + i < 1023; i++) {
                g_output_buffer[current_len + i] = 0x318D; // ㆍ (dot) character
            }
            g_output_buffer[current_len + dot_count] = L'\0';
        }
        
        // Handle dot after choseong before resetting
        if (g_current_syllable.state == STATE_CHOSEONG && g_current_syllable.temp_vowel > 0) {
            size_t current_len = wcslen(g_output_buffer);
            if (current_len < 1023) {
                g_output_buffer[current_len] = 0x318D; // ㆍ (dot) character
                g_output_buffer[current_len + 1] = L'\0';
            }
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
    // If we have temp_vowel == 200 (ㆍㆍ) and input a consonant,
    // finalize the vowel to ㅗ (jungseong 8) first
    if (g_current_syllable.temp_vowel == 200) {
        g_current_syllable.jung = 8; // ㅗ
        g_current_syllable.state = STATE_JUNGSEONG;
        g_current_syllable.temp_vowel = 0;
    }
    
    g_current_syllable.temp_vowel = 0; // 자음이 입력되면 모음 조합 상태는 초기화

    // 1. 초성이 비어있는 경우 (새 글자 시작)
    if (g_current_syllable.state == STATE_START) {
        const int choseong_map[] = {-1, 0, 2, 3, 7, 9, 12, 11}; // ㄱ, ㄴ, ㄷ, ㅂ, ㅅ, ㅈ, ㅇ
        g_current_syllable.cho = choseong_map[key_code];
        g_current_syllable.state = STATE_CHOSEONG;
        g_current_syllable.temp_consonant = key_code;
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
        }
        return;
    }

    // 4. 종성까지 입력된 경우 (복합 종성 조합 또는 새 글자 시작)
    if (g_current_syllable.state == STATE_JONGSEONG) {
        int new_jong = -1;
        
        // 같은 자음키가 다시 입력된 경우 - 종성 순환
        if (g_current_syllable.temp_consonant == key_code) {
            switch(key_code) {
                case 1: // g: ㄱ -> ㅋ -> ㄲ -> ㄱ -> ㅋ -> ㄲ -> ...
                    if (g_current_syllable.jong == 1) g_current_syllable.jong = 24;     // ㄱ -> ㅋ
                    else if (g_current_syllable.jong == 24) g_current_syllable.jong = 2; // ㅋ -> ㄲ
                    else g_current_syllable.jong = 1;                                   // ㄲ -> ㄱ
                    break;
                case 2: // n: ㄴ -> ㄹ
                    if (g_current_syllable.jong == 4) g_current_syllable.jong = 8;      // ㄴ -> ㄹ
                    else g_current_syllable.jong = 4;                                   // ㄹ -> ㄴ
                    break;
                case 3: // d: ㄷ -> ㅌ (ㄸ is not a valid final consonant)
                    if (g_current_syllable.jong == 7) g_current_syllable.jong = 25;      // ㄷ -> ㅌ
                    else g_current_syllable.jong = 7;                                    // ㅌ -> ㄷ
                    break;
                case 4: // b: ㅂ -> ㅍ -> ㅂ -> ㅍ -> ...
                    if (g_current_syllable.jong == 17) g_current_syllable.jong = 26;     // ㅂ -> ㅍ
                    else g_current_syllable.jong = 17;                                   // ㅍ -> ㅂ
                    break;
                case 5: // s: ㅅ -> ㅎ -> ㅆ
                    if (g_current_syllable.jong == 19) g_current_syllable.jong = 27;     // ㅅ -> ㅎ
                    else if (g_current_syllable.jong == 27) g_current_syllable.jong = 20; // ㅎ -> ㅆ
                    else g_current_syllable.jong = 19;                                    // ㅆ -> ㅅ
                    break;
                case 6: // j: ㅈ -> ㅊ -> ㅈ -> ㅊ -> ...
                    if (g_current_syllable.jong == 22) g_current_syllable.jong = 23;     // ㅈ -> ㅊ
                    else g_current_syllable.jong = 22;                                   // ㅊ -> ㅈ
                    break;
                case 7: // m: ㅇ -> ㅁ
                    if (g_current_syllable.jong == 21) g_current_syllable.jong = 16;     // ㅇ -> ㅁ
                    else g_current_syllable.jong = 21;                                   // ㅁ -> ㅇ
                    break;
            }
            return;
        }
        
        // 복합 종성 조합 시도 (only if not cycling)
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
        // Removed the ㅅ + ㅅ = ㅆ rule to ensure cycling takes precedence

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
        // Special handling for dot (ㆍ) - don't decompose jongseong immediately
        if (key_code == 1) { // dot
            // Cycle between single dot and double dots
            if (g_current_syllable.temp_vowel == 100) {
                g_current_syllable.temp_vowel = 200; // Two dots
            } else if (g_current_syllable.temp_vowel == 200) {
                g_current_syllable.temp_vowel = 100; // Back to single dot
            } else {
                g_current_syllable.temp_vowel = 100; // First dot
            }
            return;
        }
        
        // If we have temp_vowel == 100 (dot) or 200 (two dots) and input another vowel,
        // decompose jongseong and start new syllable
        if (g_current_syllable.temp_vowel == 100 || g_current_syllable.temp_vowel == 200) {
            // Save the temp_vowel value BEFORE resetting
            int original_temp_vowel = g_current_syllable.temp_vowel;
            
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
                // Check buffer bounds before appending
                size_t current_len = wcslen(g_output_buffer);
                if (current_len < 1023) {
                    g_output_buffer[current_len] = prev_char;
                    g_output_buffer[current_len + 1] = L'\0';
                }
            }

            g_current_syllable.cho = new_cho;
            g_current_syllable.state = STATE_CHOSEONG;
            
            // Handle the vowel input for the new syllable with proper dot combination
            
            if (key_code == 1) { 
                g_current_syllable.temp_vowel = 100; 
                return; 
            } // ㆍ
            if (key_code == 2) {
                // ㆍㆍ + ㅡ = ㅛ, ㆍ + ㅡ = ㅗ
                if (original_temp_vowel == 200) {
                    g_current_syllable.jung = 12; // ㅛ (ㆍㆍ + ㅡ = ㅛ)
                } else {
                    g_current_syllable.jung = 8; // ㅗ (ㆍ + ㅡ = ㅗ)
                }
                g_current_syllable.state = STATE_JUNGSEONG;
                g_current_syllable.temp_vowel = 1;
                return;
            }
            if (key_code == 3) {
                // ㆍㆍ + ㅣ = ㅕ, ㆍ + ㅣ = ㅓ
                if (original_temp_vowel == 200) {
                    g_current_syllable.jung = 6; // ㅕ (ㆍㆍ + ㅣ = ㅕ)
                } else {
                    g_current_syllable.jung = 4; // ㅓ (ㆍ + ㅣ = ㅓ)
                }
                g_current_syllable.state = STATE_JUNGSEONG;
                g_current_syllable.temp_vowel = 1;
                return;
            }
        }
        
        // For other vowels, proceed with normal jongseong decomposition
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
            // Check buffer bounds before appending
            size_t current_len = wcslen(g_output_buffer);
            if (current_len < 1023) {
                g_output_buffer[current_len] = prev_char;
                g_output_buffer[current_len + 1] = L'\0';
            }
        }

        g_current_syllable.cho = new_cho;
        g_current_syllable.state = STATE_CHOSEONG;
    }

    if (g_current_syllable.state == STATE_START) {
        g_current_syllable.cho = 11; // 'ㅇ'
        g_current_syllable.state = STATE_CHOSEONG;
    }

    int new_jung = -1;
    
    if (g_current_syllable.temp_vowel == 0) {
        if (key_code == 1) { 
            g_current_syllable.temp_vowel = 100; 
            return; 
        } // ㆍ
        if (key_code == 2) new_jung = 18; // ㅡ
        if (key_code == 3) new_jung = 20; // ㅣ
    }
    else if (g_current_syllable.temp_vowel == 100) { // prev: ㆍ
        if (key_code == 1) { 
            g_current_syllable.temp_vowel = 200; 
            return; 
        } // ㆍ + ㆍ
        if (key_code == 2) new_jung = 8;  // ㆍ + ㅡ = ㅗ
        if (key_code == 3) new_jung = 4;  // ㆍ + ㅣ = ㅓ
    }
    else if (g_current_syllable.temp_vowel == 200) { // prev: ㆍㆍ
        if (key_code == 1) { 
            g_current_syllable.temp_vowel = 100; 
            return; 
        } // ㆍㆍ + ㆍ = ㆍ (cycle back to single dot)
        if (key_code == 2) new_jung = 12; // ㆍㆍ + ㅡ = ㅛ
        if (key_code == 3) new_jung = 6;  // ㆍㆍ + ㅣ = ㅕ
    }
    else if (g_current_syllable.temp_vowel == 300) { // prev: ㅡ + ㆍㆍ
        if (key_code == 1) new_jung = 4;  // ㅡ + ㆍㆍ = ㅓ
        if (key_code == 2) new_jung = 17; // ㅡ + ㆍㆍ + ㅡ = ㅠ
        if (key_code == 3) new_jung = 5;  // ㅡ + ㆍㆍ + ㅣ = ㅔ
    }
    else {
        int prev_jung = g_current_syllable.jung;
        
        if (prev_jung == 8 && key_code == 3) new_jung = 11;  // ㅗ + ㅣ = ㅚ
        else if (prev_jung == 13 && key_code == 3) new_jung = 16; // ㅜ + ㅣ = ㅟ
        else if (prev_jung == 18 && key_code == 3) new_jung = 19; // ㅡ + ㅣ = ㅢ
        else if (prev_jung == 0 && key_code == 3) new_jung = 1;   // ㅏ + ㅣ = ㅐ
        else if (prev_jung == 4 && key_code == 3) new_jung = 5;   // ㅓ + ㅣ = ㅔ
        else if (prev_jung == 2 && key_code == 3) new_jung = 3;   // ㅑ + ㅣ = ㅒ
        else if (prev_jung == 6 && key_code == 3) new_jung = 7;   // ㅕ + ㅣ = ㅖ
        else if (prev_jung == 20 && key_code == 1) new_jung = 0;  // ㅣ + ㆍ = ㅏ
        else if (prev_jung == 0 && key_code == 1) new_jung = 2;   // ㅏ + ㆍ = ㅑ
        else if (prev_jung == 18 && key_code == 1 && g_current_syllable.temp_vowel != 200) new_jung = 13; // ㅡ + ㆍ = ㅜ (only if not already ㆍㆍ)
        else if (prev_jung == 13 && key_code == 1) new_jung = 17; // ㅜ + ㆍ = ㅠ
        else if (prev_jung == 18 && g_current_syllable.temp_vowel == 200) new_jung = 17; // ㅡ + ㆍㆍ = ㅠ
        else if (prev_jung == 11 && key_code == 1) new_jung = 9;  // ㅘ + ㆍ = ㅘ (과: ㄱ + ㅗ + ㅏ)
    }

    if (new_jung != -1) {
        g_current_syllable.jung = new_jung;
        g_current_syllable.state = STATE_JUNGSEONG;
        g_current_syllable.temp_vowel = 1; 
    } else {
        // If we have temp_vowel == 200 (ㆍㆍ) and no matching vowel combination,
        // finalize it to ㅗ (jungseong 8)
        if (g_current_syllable.temp_vowel == 200) {
            g_current_syllable.jung = 8; // ㅗ
            g_current_syllable.state = STATE_JUNGSEONG;
            g_current_syllable.temp_vowel = 1;
        } else {
            finalize_syllable();
            handle_vowel(key_code);
        }
    }
}

// --- 특수키 처리 함수 ---
void handle_special(char key) {
    switch (key) {
        case ' ':
            // Space input - ignore for now (as per test expectation)
            // 조합 중인 글자가 있으면(미완성 포함), 현재 글자를 확정하고 새 입력을 준비.
            // (예: 'ㄱ' 입력 후 space -> 'ㄱ' 확정)
            if (g_current_syllable.state != STATE_START) {
                finalize_syllable();
            } 
            // 조합 중인 글자가 없을 때는 무시 (테스트 요구사항에 따라)
            break;
        case '.': // Enter
            // Finalize any current syllable first
            finalize_syllable();
            
            // Clear all buffers
            g_output_buffer[0] = L'\0';
            reset_current_syllable();
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
    if (g_current_syllable.state == STATE_START) {
        return 0; // 조합 중인 글자 없음
    }
    
    // Handle dot (ㆍ) character when temp_vowel is set but jung is not yet assigned
    // But if we're in jongseong or choseong state, show the complete syllable/choseong instead
    if (g_current_syllable.temp_vowel == 100 && g_current_syllable.state != STATE_JONGSEONG && g_current_syllable.state != STATE_CHOSEONG) {
        return 0x318D; // ㆍ (dot) character
    }
    
    // Handle double dot (ㆍㆍ) character when temp_vowel is 200
    // For jongseong state, show the complete syllable (dots will be added by UI)
    if (g_current_syllable.temp_vowel == 200 && g_current_syllable.state == STATE_JONGSEONG) {
        wchar_t result = combine_syllable(g_current_syllable.cho, g_current_syllable.jung, g_current_syllable.jong);
        return result;
    }
    
    // ** 미완성 글자 처리 로직 **
    // 초성만 입력된 상태라면, 완성형 글자로 조합할 수 없으므로
    // 초성 자모를 그대로 반환함.
    if (g_current_syllable.state == STATE_CHOSEONG) {
        // If we have a choseong but temp_vowel is set, show the choseong
        // (the dot will be added by the UI function)
        if (g_current_syllable.temp_vowel > 0) {
            const wchar_t choseong_jamo[] = L"ㄱㄲㄴㄷㄸㄹㅁㅂㅃㅅㅆㅇㅈㅉㅊㅋㅌㅍㅎ";
            wchar_t result = choseong_jamo[g_current_syllable.cho];
            return result;
        }
        const wchar_t choseong_jamo[] = L"ㄱㄲㄴㄷㄸㄹㅁㅂㅃㅅㅆㅇㅈㅉㅊㅋㅌㅍㅎ";
        wchar_t result = choseong_jamo[g_current_syllable.cho];
        return result;
    }
    

    

    
    // If we're in jongseong state but have temp_vowel set (after dot input),
    // show the complete syllable with jongseong
    if (g_current_syllable.state == STATE_JONGSEONG && g_current_syllable.temp_vowel > 0) {
        wchar_t result = combine_syllable(g_current_syllable.cho, g_current_syllable.jung, g_current_syllable.jong);
        return result;
    }
    
    // 중성 또는 종성까지 조합된 경우, 완성형 글자로 조합하여 반환
    wchar_t result = combine_syllable(g_current_syllable.cho, g_current_syllable.jung, g_current_syllable.jong);
    return result;
}

void chunjiin_get_current_text(wchar_t * buffer) {
    if (buffer == NULL) {
        return;
    }
    
    // Always start with an empty buffer
    buffer[0] = L'\0';
    
    // Copy the output buffer content (completed text)
    if (wcslen(g_output_buffer) > 0) {
        wcscpy(buffer, g_output_buffer);
    }
    
    // Add the current composing character if any
    wchar_t ch = get_composing_char();
    if (ch != 0) {
        // Check buffer bounds before appending
        size_t current_len = wcslen(buffer);
        if (current_len < 1023) {
            buffer[current_len] = ch;
            buffer[current_len + 1] = L'\0';
        }
    }
    
    // Special case: if we're in jongseong state with temp_vowel (dot),
    // we need to show the syllable + dot(s)
    if (g_current_syllable.state == STATE_JONGSEONG && g_current_syllable.temp_vowel > 0) {
        // The syllable is already shown by get_composing_char(), 
        // but we need to add the dot(s)
        size_t current_len = wcslen(buffer);
        int dot_count = (g_current_syllable.temp_vowel == 200) ? 2 : 1;
        
        for (int i = 0; i < dot_count && current_len + i < 1023; i++) {
            buffer[current_len + i] = 0x318D; // ㆍ (dot) character
        }
        buffer[current_len + dot_count] = L'\0';
    }
    
    // Special case: if we're in choseong state with temp_vowel (dot),
    // we need to show the choseong + dot(s)
    if (g_current_syllable.state == STATE_CHOSEONG && g_current_syllable.temp_vowel > 0) {
        // The choseong is already shown by get_composing_char(), 
        // but we need to add the dot(s)
        size_t current_len = wcslen(buffer);
        int dot_count = (g_current_syllable.temp_vowel == 200) ? 2 : 1;
        
        for (int i = 0; i < dot_count && current_len + i < 1023; i++) {
            buffer[current_len + i] = 0x318D; // ㆍ (dot) character
        }
        buffer[current_len + dot_count] = L'\0';
    }
    
    // Ensure proper null termination
    size_t len = wcslen(buffer);
    if (len < 1023) {
        buffer[len] = L'\0';
    } else {
        buffer[1023] = L'\0';
    }
}

// Function to handle Enter key - get text and clear buffers
void chunjiin_enter_key_handler() {
    // First, finalize any current syllable to include it in the result
    finalize_syllable();
    
    // Get the complete result
    wchar_t complete_buffer[4096];
    chunjiin_get_current_text(complete_buffer);
    
    // Display the complete result on console
    printf("\n=== COMPLETE SYLLABLE RESULT ===\n");
    printf("Completed: [");
    
    // Convert wide characters to UTF-8 for proper display
    for (size_t i = 0; i < wcslen(complete_buffer); i++) {
        char utf8_char[8];
        int bytes = wchar_to_utf8(complete_buffer[i], utf8_char, sizeof(utf8_char));
        if (bytes > 0) {
            printf("%s", utf8_char);
        }
    }
    printf("]\n");
    printf("================================\n\n");
    
    // Clear all buffers
    g_output_buffer[0] = L'\0';
    reset_current_syllable();
}

// UTF-8 conversion function
int wchar_to_utf8(wchar_t wc, char *utf8_buffer, size_t buffer_size) {
    if (buffer_size < 4) {
        utf8_buffer[0] = '\0';
        return 0;
    }
    
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