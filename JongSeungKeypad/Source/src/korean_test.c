#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// Simplified Korean Hangul structures and functions for testing
typedef struct {
    int step;  // 0: choseong, 1: jungseong, 2: jongseong
    int cho_idx;
    int jung_idx;
    int jong_idx;
} two_set_input_state_t;

// Global variables for testing
char korean_keypad_buffer[128] = "";
two_set_input_state_t two_set_state;

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

// Two-set keyboard key to jamo mapping
typedef struct {
    char key;
    const char* jamo;
} key_jamo_map_t;

static const key_jamo_map_t key_to_jamo[] = {
    {'r', "ㄱ"}, {'R', "ㄲ"}, {'s', "ㄴ"}, {'e', "ㄷ"}, {'E', "ㄸ"},
    {'f', "ㄹ"}, {'a', "ㅁ"}, {'q', "ㅂ"}, {'Q', "ㅃ"}, {'t', "ㅅ"},
    {'T', "ㅆ"}, {'d', "ㅇ"}, {'w', "ㅈ"}, {'W', "ㅉ"}, {'c', "ㅊ"},
    {'z', "ㅋ"}, {'x', "ㅌ"}, {'v', "ㅍ"}, {'g', "ㅎ"},
    {'k', "ㅏ"}, {'o', "ㅐ"}, {'i', "ㅑ"}, {'O', "ㅒ"}, {'j', "ㅓ"},
    {'p', "ㅔ"}, {'u', "ㅕ"}, {'P', "ㅖ"}, {'h', "ㅗ"}, {'y', "ㅛ"},
    {'n', "ㅜ"}, {'b', "ㅠ"}, {'m', "ㅡ"}, {'l', "ㅣ"}, {'L', "ㅢ"}
};

// Hangul Unicode ranges
#define HANGUL_SYLLABLE_START 0xAC00

// Reset two-set input state
void reset_two_set_state(two_set_input_state_t * state) {
    state->step = 0;
    state->cho_idx = -1;
    state->jung_idx = -1;
    state->jong_idx = 0;
}

// Find jamo index in array
int find_jamo_index(const char* jamo, const char* jamo_array[], int array_size) {
    for (int i = 0; i < array_size; i++) {
        if (strcmp(jamo, jamo_array[i]) == 0) {
            return i;
        }
    }
    return -1;
}

// Compose Hangul from indices
uint32_t compose_hangul_from_indices(int cho_idx, int jung_idx, int jong_idx) {
    if (cho_idx < 0 || jung_idx < 0) {
        return 0;
    }
    
    return HANGUL_SYLLABLE_START + (cho_idx * 21 * 28) + (jung_idx * 28) + jong_idx;
}

// Add character to buffer
void add_char_to_buffer(const char * str) {
    int len = strlen(korean_keypad_buffer);
    int str_len = strlen(str);
    
    if (len + str_len < sizeof(korean_keypad_buffer) - 1) {
        strcat(korean_keypad_buffer, str);
    }
}

// Complete current syllable and add to buffer
void complete_current_syllable(void) {
    if (two_set_state.cho_idx >= 0 && two_set_state.jung_idx >= 0) {
        uint32_t syllable = compose_hangul_from_indices(two_set_state.cho_idx, two_set_state.jung_idx, two_set_state.jong_idx);
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
        reset_two_set_state(&two_set_state);
    }
}

// Process two-set keyboard input
void process_two_set_input(char key) {
    // Find jamo for the key
    const char* jamo = NULL;
    for (int i = 0; i < sizeof(key_to_jamo) / sizeof(key_to_jamo[0]); i++) {
        if (key_to_jamo[i].key == key) {
            jamo = key_to_jamo[i].jamo;
            break;
        }
    }
    
    if (jamo == NULL) {
        return; // Invalid key
    }
    
    // Check if it's a choseong
    int cho_idx = find_jamo_index(jamo, hangul_choseong, 19);
    if (cho_idx >= 0 && two_set_state.step == 0) {
        two_set_state.cho_idx = cho_idx;
        two_set_state.step = 1;
        return;
    }
    
    // Check if it's a jungseong
    int jung_idx = find_jamo_index(jamo, hangul_jungseong, 21);
    if (jung_idx >= 0 && two_set_state.step == 1) {
        two_set_state.jung_idx = jung_idx;
        two_set_state.step = 2;
        return;
    }
    
    // Check if it's a jongseong
    int jong_idx = find_jamo_index(jamo, hangul_jongseong, 28);
    if (jong_idx >= 0 && two_set_state.step == 2) {
        two_set_state.jong_idx = jong_idx;
        // Complete the syllable
        complete_current_syllable();
        return;
    }
    
    // Handle incomplete input - complete current syllable and start new one
    if (two_set_state.cho_idx >= 0 && two_set_state.jung_idx >= 0) {
        complete_current_syllable();
        
        // Try to start new syllable with current input
        if (cho_idx >= 0) {
            two_set_state.cho_idx = cho_idx;
            two_set_state.step = 1;
        }
    }
}

// Test function to demonstrate two-set keyboard input
void test_two_set_input(void) {
    printf("=== Korean Two-Set Keyboard Input Test ===\n");
    printf("Enter two-set keyboard input (e.g., rkskekfk for '안녕하세요'):\n");
    printf("Press Enter to finish, 'q' to quit\n\n");
    
    char input[256];
    while (1) {
        printf("Input: ");
        if (fgets(input, sizeof(input), stdin) == NULL) break;
        
        // Remove newline
        input[strcspn(input, "\n")] = 0;
        
        if (strcmp(input, "q") == 0) break;
        
        // Clear buffer
        memset(korean_keypad_buffer, 0, sizeof(korean_keypad_buffer));
        reset_two_set_state(&two_set_state);
        
        // Process each character
        for (int i = 0; input[i] != '\0'; i++) {
            process_two_set_input(input[i]);
        }
        
        // Complete any pending syllable
        complete_current_syllable();
        
        printf("Output: %s\n\n", korean_keypad_buffer);
    }
}

// Test function for individual jamo composition
void test_jamo_composition(void) {
    printf("=== Korean Jamo Composition Test ===\n");
    
    // Test some common syllables
    const char* test_cases[] = {
        "rk",   // 가
        "rks",  // 간
        "sk",   // 나
        "ek",   // 다
        "fk",   // 라
        "ak",   // 마
        "qk",   // 바
        "tk",   // 사
        "dk",   // 아
        "wk",   // 자
        "ck",   // 차
        "zk",   // 카
        "xk",   // 타
        "vk",   // 파
        "gk",   // 하
    };
    
    int num_tests = sizeof(test_cases) / sizeof(test_cases[0]);
    
    for (int i = 0; i < num_tests; i++) {
        // Clear buffer
        memset(korean_keypad_buffer, 0, sizeof(korean_keypad_buffer));
        reset_two_set_state(&two_set_state);
        
        // Process test case
        for (int j = 0; test_cases[i][j] != '\0'; j++) {
            process_two_set_input(test_cases[i][j]);
        }
        
        // Complete any pending syllable
        complete_current_syllable();
        
        printf("Input: %s -> Output: %s\n", test_cases[i], korean_keypad_buffer);
    }
    printf("\n");
}

// Test function for complex syllables
void test_complex_syllables(void) {
    printf("=== Complex Syllable Test ===\n");
    
    const char* complex_tests[] = {
        "rkskekfk",  // 안녕하세요
        "dkssud",    // 안녕
        "rksk",      // 안녕
        "ekfk",      // 하세요
        "qkrtk",     // 바보
        "dkdld",     // 아야
        "skfk",      // 나라
        "ekfk",      // 다라
    };
    
    int num_tests = sizeof(complex_tests) / sizeof(complex_tests[0]);
    
    for (int i = 0; i < num_tests; i++) {
        // Clear buffer
        memset(korean_keypad_buffer, 0, sizeof(korean_keypad_buffer));
        reset_two_set_state(&two_set_state);
        
        // Process test case
        for (int j = 0; complex_tests[i][j] != '\0'; j++) {
            process_two_set_input(complex_tests[i][j]);
        }
        
        // Complete any pending syllable
        complete_current_syllable();
        
        printf("Input: %s -> Output: %s\n", complex_tests[i], korean_keypad_buffer);
    }
    printf("\n");
}

// Main test function
int main(void) {
    printf("Korean Hangul Input System Test\n");
    printf("================================\n\n");
    
    // Run tests
    test_jamo_composition();
    test_complex_syllables();
    test_two_set_input();
    
    printf("Test completed.\n");
    return 0;
} 