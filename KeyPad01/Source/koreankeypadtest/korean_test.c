#include "korean_hangul.h"
#include <stdio.h>
#include <string.h>
#include <stddef.h>

// 함수 프로토타입 선언
void test_jamo_composition(void);
void test_complex_syllables(void);
void test_dak_input(void);
void test_alm_input(void);
void test_compound_jongseong(void);

// Test function for individual jamo composition
void test_jamo_composition(void) {
    printf("=== Korean Jamo Composition Test ===\n");
    
    // Test some common syllables
    const char* test_cases[] = {
        "rk",   // 가
        "rk1",  // 각
        "rk2",  // 간
        "rk3",  // 갇
        "rk4",  // 갈
        "rk5",  // 감
        "rk6",  // 갑
        "rk7",  // 갓
        "rk8",  // 강
        "rk9",  // 갖
        "rk0",  // 갗
        "ekfr", // 닭
        // 복합 종성 테스트
        "rk7r",  // 갓ㄱ → 갓 + ㄱ = 갛 (복합 불가, ㄱ은 다음 초성)
        "rk19",  // 갓ㅅ → 갓 + ㅅ = 갓 (복합 불가, ㅅ은 다음 초성)
        "rk7t",  // 갓ㅅ → 갓 + ㅅ = 갓 (복합 불가, ㅅ은 다음 초성)
        "rk19",  // 갓ㅅ → 갓 + ㅅ = 갓 (복합 불가, ㅅ은 다음 초성)
        "rk1",   // 각
        "rk19",  // 각ㅅ → ㄱ+ㅅ=ㄳ (갃)
        "rk1t",  // 각ㅅ → ㄱ+ㅅ=ㄳ (갃)
        "rk1r",  // 각ㄱ → ㄱ+ㄱ=각 (복합 불가, ㄱ은 다음 초성)
        "rk4g",  // 갈ㅎ → ㄹ+ㅎ=ㅀ (갏)
        "rk4m",  // 갈ㅁ → ㄹ+ㅁ=ㄻ (앎)
        "dkfa",  // 앎
        "rk4b",  // 갈ㅂ → ㄹ+ㅂ=ㄼ (갋)
        "rk4t",  // 갈ㅌ → ㄹ+ㅌ=ㄾ (갏)
        "rk4v",  // 갈ㅍ → ㄹ+ㅍ=ㄿ (감)
        "rk4h",  // 갈ㅎ → ㄹ+ㅎ=ㅀ (갏)
        "rk6t",  // 갑ㅅ → ㅂ+ㅅ=ㅄ (값)
    };
    
    int num_tests = sizeof(test_cases) / sizeof(test_cases[0]);
    
    for (int i = 0; i < num_tests; i++) {
        // Clear buffer
        memset(korean_keypad_buffer, 0, sizeof(korean_keypad_buffer));
        reset_two_set_state(&korean_keypad_state.two_set_state);
        
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
        reset_two_set_state(&korean_keypad_state.two_set_state);
        
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

// Test function specifically for "닭" (chicken)
void test_dak_input(void) {
    printf("=== Test for '닭' (chicken) Input ===\n");
    
    // "닭" is composed of: 초성 ㄷ(e) + 중성 ㅏ(k) + 종성 ㄹ(f) + ㄱ(r)
    const char* dak_input = "ekfr";
    
    printf("Input sequence for '닭': %s\n", dak_input);
    printf("Expected output: 닭\n");
    
    // Clear buffer
    memset(korean_keypad_buffer, 0, sizeof(korean_keypad_buffer));
    reset_two_set_state(&korean_keypad_state.two_set_state);
    
    // Process each character step by step
    printf("Processing step by step:\n");
    for (int i = 0; dak_input[i] != '\0'; i++) {
        printf("  Step %d: Input '%c' -> ", i + 1, dak_input[i]);
        process_two_set_input(dak_input[i]);
        printf("Buffer: %s\n", korean_keypad_buffer);
    }
    
    // Complete any pending syllable
    complete_current_syllable();
    
    printf("Final result: %s\n", korean_keypad_buffer);
    
    // Verify the result
    if (strcmp(korean_keypad_buffer, "닭") == 0) {
        printf("✓ SUCCESS: '닭' input test passed!\n");
    } else {
        printf("✗ FAILED: Expected '닭', got '%s'\n", korean_keypad_buffer);
    }
    printf("\n");
}

// 복합 종성 자동 테스트 함수
void test_compound_jongseong(void) {
    printf("=== 복합 종성 자동 테스트 (조합 가능 음절만) ===\n");
    struct {
        const char* input;
        const char* expected;
        const char* desc;
    } cases[] = {
        {"rkqt", "값", "값 (ㄱ+ㅏ+ㅂ+ㅅ)"}, 
        {"qkfq", "밟", "밟 (ㅂ+ㅏ+ㅂ+ㅅ)"},
        {"aksg", "많", "많 (ㅁ+ㅏ+ㄴ+ㅎ)"},
        {"dlfr", "읽", "읽 (ㅇ+ㅣ+ㄹ+ㄱ)"},
        {"rkt", "갓", "갓 (ㄱ+ㅏ+ㅅ)"},
        {"ekfr", "닭", "ㄺ (달+ㄱ)"},
        {"dkfa", "앎", "ㄻ (알+ㅁ)"}, 
        {"dksw", "앉", "ㄵ (안+ㅈ)"},  
        {"djqt", "없", "ㅄ (업+ㅅ)"}, 
        {"ekft", "닰", "ㄽ (달+ㅅ)"}, 
        {"ekfx", "닱", "ㄾ (달+ㅌ)"}, 
        {"ekfv", "닲", "ㄿ (달+ㅍ)"}, 
        {"ekfg", "닳", "ㅀ (달+ㅎ)"},
        {"dlfr", "읽", "ㄺ (일+ㄱ)"},
        {"dmfv", "읊", "ㄿ (을+ㅍ)"},
    };
    int num = sizeof(cases)/sizeof(cases[0]);
    int pass = 0;
    for (int i = 0; i < num; i++) {
        memset(korean_keypad_buffer, 0, sizeof(korean_keypad_buffer));
        reset_two_set_state(&korean_keypad_state.two_set_state);
        for (int j = 0; cases[i].input[j] != '\0'; j++) {
            process_two_set_input(cases[i].input[j]);
        }
        complete_current_syllable();
        int ok = strcmp(korean_keypad_buffer, cases[i].expected) == 0;
        printf("%-10s: %-8s → %-4s (기대: %-4s) %s\n", cases[i].desc, cases[i].input, korean_keypad_buffer, cases[i].expected, ok ? "✓" : "✗");
        if (ok) pass++;
    }
    printf("합격: %d / %d\n\n", pass, num);
}

// Main test function
int main(void) {
    printf("Korean Hangul Input System Test\n");
    printf("================================\n\n");
    
    // Run tests
    test_jamo_composition();
    test_complex_syllables();
    test_dak_input();
    test_alm_input();
    test_compound_jongseong();
    
    printf("Test completed.\n");
    return 0;
}

// Test function specifically for "앎"
void test_alm_input(void) {
    printf("=== Test for '앎' Input ===\n");
    const char* alm_input = "dkfa";
    printf("Input sequence for '앎': %s\n", alm_input);
    printf("Expected output: 앎\n");
    memset(korean_keypad_buffer, 0, sizeof(korean_keypad_buffer));
    reset_two_set_state(&korean_keypad_state.two_set_state);
    printf("Processing step by step:\n");
    for (int i = 0; alm_input[i] != '\0'; i++) {
        printf("  Step %d: Input '%c' -> ", i + 1, alm_input[i]);
        process_two_set_input(alm_input[i]);
        printf("Buffer: %s\n", korean_keypad_buffer);
    }
    complete_current_syllable();
    printf("Final result: %s\n", korean_keypad_buffer);
    if (strcmp(korean_keypad_buffer, "앎") == 0) {
        printf("✓ SUCCESS: '앎' input test passed!\n");
    } else {
        printf("✗ FAILED: Expected '앎', got '%s'\n", korean_keypad_buffer);
    }
    printf("\n");
} 