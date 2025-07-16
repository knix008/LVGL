#include "korean_hangul.h"
#include <string.h>
#include <stdio.h>
#include <wchar.h>
#include <locale.h>

// Global variables for Korean keypad
char korean_keypad_buffer[128] = "";
korean_keypad_state_t korean_keypad_state;

// Korean character mappings
const char* hangul_choseong[] = {
    "ㄱ", "ㄲ", "ㄴ", "ㄷ", "ㄸ", "ㄹ", "ㅁ", "ㅂ", "ㅃ", "ㅅ", "ㅆ", "ㅇ", "ㅈ", "ㅉ", "ㅊ", "ㅋ", "ㅌ", "ㅍ", "ㅎ"
};

const char* hangul_jungseong[] = {
    "ㅏ", "ㅐ", "ㅑ", "ㅒ", "ㅓ", "ㅔ", "ㅕ", "ㅖ", "ㅗ", "ㅘ", "ㅙ", "ㅚ", "ㅛ", "ㅜ", "ㅝ", "ㅞ", "ㅟ", "ㅠ", "ㅡ", "ㅢ", "ㅣ"
};

const char* hangul_jongseong[] = {
    "", "ㄱ", "ㄲ", "ㄳ", "ㄴ", "ㄵ", "ㄶ", "ㄷ", "ㄹ", "ㄺ", "ㄻ", "ㄼ", "ㄽ", "ㄾ", "ㄿ", "ㅀ", "ㅁ", "ㅂ", "ㅄ", "ㅅ", "ㅆ", "ㅇ", "ㅈ", "ㅊ", "ㅋ", "ㅌ", "ㅍ", "ㅎ"
};

// Two-set keyboard mappings (QWERTY to Hangul jamo)
const char* two_set_choseong[] = {
    "ㄱ", "ㄲ", "ㄴ", "ㄷ", "ㄸ", "ㄹ", "ㅁ", "ㅂ", "ㅃ", "ㅅ", "ㅆ", "ㅇ", "ㅈ", "ㅉ", "ㅊ", "ㅋ", "ㅌ", "ㅍ", "ㅎ"
};

const char* two_set_jungseong[] = {
    "ㅏ", "ㅐ", "ㅑ", "ㅒ", "ㅓ", "ㅔ", "ㅕ", "ㅖ", "ㅗ", "ㅘ", "ㅙ", "ㅚ", "ㅛ", "ㅜ", "ㅝ", "ㅞ", "ㅟ", "ㅠ", "ㅡ", "ㅢ", "ㅣ"
};

const char* two_set_jongseong[] = {
    "", "ㄱ", "ㄲ", "ㄳ", "ㄴ", "ㄵ", "ㄶ", "ㄷ", "ㄹ", "ㄺ", "ㄻ", "ㄼ", "ㄽ", "ㄾ", "ㄿ", "ㅀ", "ㅁ", "ㅂ", "ㅄ", "ㅅ", "ㅆ", "ㅇ", "ㅈ", "ㅊ", "ㅋ", "ㅌ", "ㅍ", "ㅎ"
};

// Two-set keyboard key to jamo mapping (from standalone)
const key_jamo_map_t key_to_jamo[] = {
    // Choseong (초성)
    {'r', "ㄱ"}, {'R', "ㄲ"}, {'s', "ㄴ"}, {'e', "ㄷ"}, {'E', "ㄸ"},
    {'f', "ㄹ"}, {'a', "ㅁ"}, {'q', "ㅂ"}, {'Q', "ㅃ"}, {'t', "ㅅ"},
    {'T', "ㅆ"}, {'d', "ㅇ"}, {'w', "ㅈ"}, {'W', "ㅉ"}, {'c', "ㅊ"},
    {'z', "ㅋ"}, {'x', "ㅌ"}, {'v', "ㅍ"}, {'g', "ㅎ"},
    // Jungseong (중성)
    {'k', "ㅏ"}, {'o', "ㅐ"}, {'i', "ㅑ"}, {'O', "ㅒ"}, {'j', "ㅓ"},
    {'p', "ㅔ"}, {'u', "ㅕ"}, {'P', "ㅖ"}, {'h', "ㅗ"}, {'y', "ㅛ"},
    {'n', "ㅜ"}, {'b', "ㅠ"}, {'m', "ㅡ"}, {'l', "ㅣ"}, {'L', "ㅢ"},
    // Jongseong (종성) - 추가 매핑
    {'1', "ㄱ"}, {'2', "ㄴ"}, {'3', "ㄷ"}, {'4', "ㄹ"}, {'5', "ㅁ"},
    {'6', "ㅂ"}, {'7', "ㅅ"}, {'8', "ㅇ"}, {'9', "ㅈ"}, {'0', "ㅊ"}
};

const char* english_chars[] = {
    "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P",
    "A", "S", "D", "F", "G", "H", "J", "K", "L",
    "Z", "X", "C", "V", "B", "N", "M"
};

const char* number_chars[] = {
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "0"
};

// 복합 종성 조합 테이블 - Unicode 표준에 맞는 정확한 인덱스
typedef struct {
    const char* first;
    const char* second;
    int compound_idx; // Unicode 조합에 맞는 정확한 인덱스
} jongseong_compound_t;

static const jongseong_compound_t jongseong_compounds[] = {
    {"ㄱ", "ㅅ", 3},   // ㄳ (Unicode: 0x11A8 + 2)
    {"ㄴ", "ㅈ", 5},   // ㄵ (Unicode: 0x11A8 + 4)
    {"ㄴ", "ㅎ", 6},   // ㄶ (Unicode: 0x11A8 + 5)
    {"ㄷ", "ㅎ", 7},   // ㄷ (Unicode: 0x11A8 + 6)
    {"ㄹ", "ㄱ", 9},   // ㄺ (Unicode: 0x11A8 + 8)
    {"ㄹ", "ㅁ", 10},  // ㄻ (Unicode: 0x11A8 + 9)
    {"ㄹ", "ㅂ", 11},  // ㄼ (Unicode: 0x11A8 + 10)
    {"ㄹ", "ㅅ", 12},  // ㄽ (Unicode: 0x11A8 + 11)
    {"ㄹ", "ㅌ", 13},  // ㄾ (Unicode: 0x11A8 + 12)
    {"ㄹ", "ㅍ", 14},  // ㄿ (Unicode: 0x11A8 + 13)
    {"ㄹ", "ㅎ", 15},  // ㅀ (Unicode: 0x11A8 + 14)
    {"ㅁ", "ㅎ", 16},  // ㅁ (Unicode: 0x11A8 + 15)
    {"ㅂ", "ㅅ", 18},  // ㅄ (Unicode: 0x11A8 + 17)
    {"ㅂ", "ㅂ", 17},  // ㅂ (Unicode: 0x11A8 + 16) - 추가
    {"ㅅ", "ㅅ", 19},  // ㅆ (Unicode: 0x11A8 + 18)
    {"ㅇ", "ㅅ", 20},  // ㅇ (Unicode: 0x11A8 + 19)
    {"ㅈ", "ㅎ", 21},  // ㅈ (Unicode: 0x11A8 + 20)
    {"ㅊ", "ㅎ", 22},  // ㅊ (Unicode: 0x11A8 + 21)
    {"ㅋ", "ㅎ", 23},  // ㅋ (Unicode: 0x11A8 + 22)
    {"ㅌ", "ㅎ", 24},  // ㅌ (Unicode: 0x11A8 + 23)
    {"ㅍ", "ㅎ", 25},  // ㅍ (Unicode: 0x11A8 + 24)
    {"ㅎ", "ㅎ", 26},  // ㅎ (Unicode: 0x11A8 + 25)
};
#define NUM_JONGSEONG_COMPOUNDS (sizeof(jongseong_compounds)/sizeof(jongseong_compounds[0]))

// 복합 종성 조합 가능 여부 확인
static int can_form_compound_jongseong(const char* first, const char* second) {
    for (size_t i = 0; i < NUM_JONGSEONG_COMPOUNDS; i++) {
        if (strcmp(first, jongseong_compounds[i].first) == 0 &&
            strcmp(second, jongseong_compounds[i].second) == 0) {
            return jongseong_compounds[i].compound_idx;
        }
    }
    return -1;
}

// 복합 종성 조합 처리 개선 - 더 세밀한 로직
static void handle_compound_jongseong(two_set_input_state_t * state, const char* jamo) {
    if (state->jong_idx == 0) {
        // 첫 종성 입력
        int jong_idx = find_jamo_index(jamo, two_set_jongseong, 28);
        if (jong_idx > 0) {
            state->jong_idx = jong_idx;
            // 다음 입력이 복합 종성이 될 수 있는지 확인하기 위해 대기
            return;
        }
    } else {
        // 이미 종성이 있는 상태에서 추가 입력
        const char* current_jong = two_set_jongseong[state->jong_idx];
        int compound_idx = can_form_compound_jongseong(current_jong, jamo);
        if (compound_idx >= 0) {
            // 복합 종성 가능 - 정확한 인덱스로 설정
            state->jong_idx = compound_idx;
            complete_current_syllable();
            return;
        } else {
            // 복합 종성 불가능 - 현재 음절 완성하고, 마지막 입력 자음을 새 초성으로 넘김
            complete_current_syllable();
            int cho_idx = find_jamo_index(jamo, two_set_choseong, 19);
            if (cho_idx >= 0) {
                state->cho_idx = cho_idx;
                state->jung_idx = -1;
                state->jong_idx = 0;
                state->step = 1;
            } else {
                // 만약 초성이 아니면 상태 초기화
                reset_two_set_state(state);
            }
            return;
        }
    }
}

// Initialize Korean keypad
void init_korean_keypad(void) {
    memset(&korean_keypad_state, 0, sizeof(korean_keypad_state));
    korean_keypad_state.mode = KOREAN_MODE_HANGUL;
    korean_keypad_buffer[0] = '\0';
    reset_hangul_composition(&korean_keypad_state.composition);
    reset_two_set_state(&korean_keypad_state.two_set_state);
}

// Reset Hangul composition
void reset_hangul_composition(hangul_composition_t * comp) {
    comp->choseong = 0;
    comp->jungseong = 0;
    comp->jongseong = 0;
    comp->has_choseong = false;
    comp->has_jungseong = false;
    comp->has_jongseong = false;
}

// Reset two-set input state
void reset_two_set_state(two_set_input_state_t * state) {
    state->step = 0;
    state->cho_idx = -1;
    state->jung_idx = -1;
    state->jong_idx = 0;
}

// Add choseong (initial consonant)
void add_choseong(hangul_composition_t * comp, uint16_t choseong) {
    if (!comp->has_choseong) {
        comp->choseong = choseong;
        comp->has_choseong = true;
    }
}

// Add jungseong (vowel)
void add_jungseong(hangul_composition_t * comp, uint16_t jungseong) {
    if (!comp->has_jungseong) {
        comp->jungseong = jungseong;
        comp->has_jungseong = true;
    }
}

// Add jongseong (final consonant)
void add_jongseong(hangul_composition_t * comp, uint16_t jongseong) {
    if (!comp->has_jongseong && comp->has_choseong && comp->has_jungseong) {
        comp->jongseong = jongseong;
        comp->has_jongseong = true;
    }
}

// Find jamo index in array (already present, skip if exists)
int find_jamo_index(const char* jamo, const char* jamo_array[], int array_size) {
    for (int i = 0; i < array_size; i++) {
        if (strcmp(jamo, jamo_array[i]) == 0) {
            return i;
        }
    }
    return -1;
}

// Compose Hangul syllable from components
uint32_t compose_hangul_syllable(hangul_composition_t * comp) {
    if (!comp->has_choseong || !comp->has_jungseong) {
        return 0;
    }
    
    // Calculate syllable code
    uint32_t choseong_idx = comp->choseong - HANGUL_CHOSEONG_START;
    uint32_t jungseong_idx = comp->jungseong - HANGUL_JUNGSEONG_START;
    uint32_t jongseong_idx = comp->has_jongseong ? (comp->jongseong - HANGUL_JONGSEONG_START + 1) : 0;
    
    return HANGUL_SYLLABLE_START + (choseong_idx * 21 + jungseong_idx) * 28 + jongseong_idx;
}

// Compose Hangul from indices (Unicode 조합 공식 정확히 적용)
uint32_t compose_hangul_from_indices(int cho_idx, int jung_idx, int jong_idx) {
    if (cho_idx < 0 || jung_idx < 0) {
        return 0;
    }
    
    // Unicode 조합 공식: 0xAC00 + (cho_idx * 21 + jung_idx) * 28 + jong_idx
    uint32_t syllable = HANGUL_SYLLABLE_START + (cho_idx * 21 + jung_idx) * 28 + jong_idx;
    
    // 유효한 한글 음절 범위 확인 (0xAC00 ~ 0xD7A3)
    if (syllable >= 0xAC00 && syllable <= 0xD7A3) {
        return syllable;
    }
    
    return 0;
}

// Complete current syllable and add to buffer
void complete_current_syllable(void) {
    two_set_input_state_t * state = &korean_keypad_state.two_set_state;
    
    if (state->cho_idx >= 0 && state->jung_idx >= 0) {
        uint32_t syllable = compose_hangul_from_indices(state->cho_idx, state->jung_idx, state->jong_idx);
        if (syllable > 0) {
            char utf8_char[8];
            int len = 0;
            
            // Convert Unicode to UTF-8
            if (syllable < 0x80) {
                utf8_char[len++] = syllable;
            } else if (syllable < 0x800) {
                utf8_char[len++] = 0xC0 | (syllable >> 6);
                utf8_char[len++] = 0x80 | (syllable & 0x3F);
            } else if (syllable < 0x10000) {
                utf8_char[len++] = 0xE0 | (syllable >> 12);
                utf8_char[len++] = 0x80 | ((syllable >> 6) & 0x3F);
                utf8_char[len++] = 0x80 | (syllable & 0x3F);
            }
            utf8_char[len] = '\0';
            
            add_char_to_buffer(utf8_char);
        }
        reset_two_set_state(state);
    }
}

// Process two-set keyboard input
void process_two_set_input(char key) {
    two_set_input_state_t * state = &korean_keypad_state.two_set_state;
    
    // Find jamo for the key
    const char* jamo = NULL;
    for (size_t i = 0; i < sizeof(key_to_jamo) / sizeof(key_to_jamo[0]); i++) {
        if (key_to_jamo[i].key == key) {
            jamo = key_to_jamo[i].jamo;
            break;
        }
    }
    
    if (jamo == NULL) {
        return; // Invalid key
    }
    
    // Check if it's a choseong
    int cho_idx = find_jamo_index(jamo, two_set_choseong, 19);
    if (cho_idx >= 0 && state->step == 0) {
        state->cho_idx = cho_idx;
        state->step = 1;
        return;
    }
    
    // Check if it's a jungseong
    int jung_idx = find_jamo_index(jamo, two_set_jungseong, 21);
    if (jung_idx >= 0 && state->step == 1) {
        state->jung_idx = jung_idx;
        state->step = 2;
        return;
    }
    
    // Check if it's a jongseong
    int jong_idx = find_jamo_index(jamo, two_set_jongseong, 28);
    if (jong_idx >= 0 && state->step == 2) {
        handle_compound_jongseong(state, jamo);
        return;
    }
    
    // Handle incomplete input - complete current syllable and start new one
    if (state->cho_idx >= 0 && state->jung_idx >= 0) {
        complete_current_syllable();
        
        // Try to start new syllable with current input
        if (cho_idx >= 0) {
            state->cho_idx = cho_idx;
            state->step = 1;
        }
    }
}

// Add character to buffer
void add_char_to_buffer(const char * str) {
    int len = strlen(korean_keypad_buffer);
    int str_len = strlen(str);
    
    if (len + str_len < (int)sizeof(korean_keypad_buffer) - 1) {
        strcat(korean_keypad_buffer, str);
    }
}

// Remove character from buffer
void remove_char_from_buffer(void) {
    int len = strlen(korean_keypad_buffer);
    if (len > 0) {
        // Handle UTF-8 multi-byte characters
        while (len > 0 && (korean_keypad_buffer[len - 1] & 0xC0) == 0x80) {
            len--;
        }
        if (len > 0) {
            korean_keypad_buffer[len - 1] = '\0';
        }
    }
}

// Process Hangul character input
void process_hangul_character(int char_index) {
    if (korean_keypad_state.mode == KOREAN_MODE_HANGUL) {
        hangul_composition_t * comp = &korean_keypad_state.composition;
        
        if (char_index < 19) {
            // This is a consonant (자음)
            if (!comp->has_choseong) {
                // First consonant - add as choseong (초성)
                add_choseong(comp, HANGUL_CHOSEONG_START + char_index);
                printf("Added choseong: %s (index %d)\n", hangul_choseong[char_index], char_index);
            } else if (comp->has_choseong && !comp->has_jungseong) {
                // Second consonant without vowel - replace choseong
                reset_hangul_composition(comp);
                add_choseong(comp, HANGUL_CHOSEONG_START + char_index);
                printf("Replaced choseong: %s (index %d)\n", hangul_choseong[char_index], char_index);
            } else if (comp->has_choseong && comp->has_jungseong && !comp->has_jongseong) {
                // Third consonant after choseong + jungseong - try to add as jongseong
                // Check if this consonant can be used as jongseong
                int jongseong_idx = -1;
                for (int i = 1; i < 28; i++) { // Skip index 0 (empty)
                    if (strcmp(hangul_choseong[char_index], hangul_jongseong[i]) == 0) {
                        jongseong_idx = i;
                        break;
                    }
                }
                
                if (jongseong_idx >= 0) {
                    // Valid jongseong - add it and update the last character in buffer
                    add_jongseong(comp, HANGUL_JONGSEONG_START + jongseong_idx - 1);
                    printf("Added jongseong: %s (index %d)\n", hangul_jongseong[jongseong_idx], jongseong_idx);
                    
                    // Update the last character in buffer with the new syllable including jongseong
                    uint32_t syllable = compose_hangul_syllable(comp);
                    if (syllable > 0) {
                        char utf8_char[8];
                        int len = 0;
                        
                        // Convert Unicode to UTF-8
                        if (syllable < 0x80) {
                            utf8_char[len++] = syllable;
                        } else if (syllable < 0x800) {
                            utf8_char[len++] = 0xC0 | (syllable >> 6);
                            utf8_char[len++] = 0x80 | (syllable & 0x3F);
                        } else if (syllable < 0x10000) {
                            utf8_char[len++] = 0xE0 | (syllable >> 12);
                            utf8_char[len++] = 0x80 | ((syllable >> 6) & 0x3F);
                            utf8_char[len++] = 0x80 | (syllable & 0x3F);
                        }
                        utf8_char[len] = '\0';
                        
                        // Remove the last character and add the updated one
                        remove_char_from_buffer();
                        add_char_to_buffer(utf8_char);
                    }
                    reset_hangul_composition(comp);
                } else {
                    // Not a valid jongseong - complete current syllable and start new one
                    printf("Invalid jongseong, completing syllable and starting new one\n");
                    uint32_t syllable = compose_hangul_syllable(comp);
                    if (syllable > 0) {
                        char utf8_char[8];
                        int len = 0;
                        
                        // Convert Unicode to UTF-8
                        if (syllable < 0x80) {
                            utf8_char[len++] = syllable;
                        } else if (syllable < 0x800) {
                            utf8_char[len++] = 0xC0 | (syllable >> 6);
                            utf8_char[len++] = 0x80 | (syllable & 0x3F);
                        } else if (syllable < 0x10000) {
                            utf8_char[len++] = 0xE0 | (syllable >> 12);
                            utf8_char[len++] = 0x80 | ((syllable >> 6) & 0x3F);
                            utf8_char[len++] = 0x80 | (syllable & 0x3F);
                        }
                        utf8_char[len] = '\0';
                        
                        add_char_to_buffer(utf8_char);
                    }
                    reset_hangul_composition(comp);
                    add_choseong(comp, HANGUL_CHOSEONG_START + char_index);
                }
            } else {
                // Already have choseong + jungseong + jongseong - complete and start new
                printf("Already complete syllable, starting new one\n");
                uint32_t syllable = compose_hangul_syllable(comp);
                if (syllable > 0) {
                    char utf8_char[8];
                    int len = 0;
                    
                    // Convert Unicode to UTF-8
                    if (syllable < 0x80) {
                        utf8_char[len++] = syllable;
                    } else if (syllable < 0x800) {
                        utf8_char[len++] = 0xC0 | (syllable >> 6);
                        utf8_char[len++] = 0x80 | (syllable & 0x3F);
                    } else if (syllable < 0x10000) {
                        utf8_char[len++] = 0xE0 | (syllable >> 12);
                        utf8_char[len++] = 0x80 | ((syllable >> 6) & 0x3F);
                        utf8_char[len++] = 0x80 | (syllable & 0x3F);
                    }
                    utf8_char[len] = '\0';
                    
                    add_char_to_buffer(utf8_char);
                }
                reset_hangul_composition(comp);
                add_choseong(comp, HANGUL_CHOSEONG_START + char_index);
            }
        } else if (char_index >= 100 && char_index < 121) {
            // This is a vowel (모음)
            int vowel_idx = char_index - 100;
            if (comp->has_choseong && !comp->has_jungseong) {
                // Valid sequence - add jungseong
                add_jungseong(comp, HANGUL_JUNGSEONG_START + vowel_idx);
                printf("Added jungseong: %s (index %d)\n", hangul_jungseong[vowel_idx], vowel_idx);
            } else if (!comp->has_choseong) {
                // No choseong - start with this vowel (not valid in Korean)
                printf("Warning: Vowel without choseong - ignoring\n");
                return;
            } else {
                // Already have jungseong - replace it
                comp->jungseong = HANGUL_JUNGSEONG_START + vowel_idx;
                printf("Replaced jungseong: %s (index %d)\n", hangul_jungseong[vowel_idx], vowel_idx);
            }
        }
        
        // Try to compose syllable (for choseong + jungseong without jongseong)
        if (comp->has_choseong && comp->has_jungseong && !comp->has_jongseong) {
            uint32_t syllable = compose_hangul_syllable(comp);
            if (syllable > 0) {
                char utf8_char[8];
                int len = 0;
                
                // Convert Unicode to UTF-8
                if (syllable < 0x80) {
                    utf8_char[len++] = syllable;
                } else if (syllable < 0x800) {
                    utf8_char[len++] = 0xC0 | (syllable >> 6);
                    utf8_char[len++] = 0x80 | (syllable & 0x3F);
                } else if (syllable < 0x10000) {
                    utf8_char[len++] = 0xE0 | (syllable >> 12);
                    utf8_char[len++] = 0x80 | ((syllable >> 6) & 0x3F);
                    utf8_char[len++] = 0x80 | (syllable & 0x3F);
                }
                utf8_char[len] = '\0';
                
                add_char_to_buffer(utf8_char);
                reset_hangul_composition(comp);
            }
        }
    }
} 