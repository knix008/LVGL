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
        // 기본 음절
        "rk",   // 가
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
        
        // 다양한 중성
        "rj",   // 거
        "rp",   // 게
        "ru",   // 겨
        "rP",   // 계
        "rh",   // 고
        "ry",   // 교
        "rn",   // 구
        "rb",   // 규
        "rm",   // 그
        "rl",   // 기
        "rL",   // 긔
        
        // 복합 종성 테스트 (실제 입력 가능한 것만)
        "rkt",  // 갓
        "rkr",  // 각
        "rks",  // 간
        "rke",  // 갇
        "rkf",  // 갈
        "rka",  // 감
        "rkq",  // 갑
        "rkt",  // 갓
        "rkd",  // 강
        "rkw",  // 갖
        "rkc",  // 갗
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
        // 기본 단어들
        "rkskekfk",  // 안녕하세요
        "dkssud",    // 안녕
        "rksk",      // 안녕
        "ekfk",      // 하세요
        "qkrtk",     // 바보
        "dkdld",     // 아야
        "skfk",      // 나라
        "ekfk",      // 다라
        
        // 긴 문장들
        "dkssudgktpdy",  // 안녕하세요
        "rkskekfkdkssud", // 안녕하세요안녕
        "qkrtkdkdld",     // 바보아야
        "skfkekfk",       // 나라다라
        
        // 특수 조합들
        "rkskekfk",       // 안녕하세요
        "dkssud",         // 안녕
        "rksk",           // 안녕
        "ekfk",           // 하세요
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
    printf("=== 복합 종성 자동 테스트 (실제 입력 가능한 케이스만) ===\n");
    struct {
        const char* input;
        const char* expected;
        const char* desc;
    } cases[] = {
        // 기본 복합 종성 (실제 입력 가능한 것만)
        {"rkqt", "값", "값 (ㄱ+ㅏ+ㅂ+ㅅ)"},
        {"qkfq", "밟", "밟 (ㅂ+ㅏ+ㅂ+ㅅ)"},
        {"aksg", "많", "많 (ㅁ+ㅏ+ㄴ+ㅎ)"},
        {"dlfr", "읽", "읽 (ㅇ+ㅣ+ㄹ+ㄱ)"},
        {"rkt", "갓", "갓 (ㄱ+ㅏ+ㅅ)"},
        {"ekfr", "닭", "닭 (ㄷ+ㅏ+ㄹ+ㄱ)"},
        {"dkfa", "앎", "앎 (ㄷ+ㅏ+ㄹ+ㅁ)"},
        {"dksw", "앉", "앉 (ㅇ+ㅏ+ㄴ+ㅈ)"},
        {"djqt", "없", "없 (ㅇ+ㅓ+ㅂ+ㅅ)"},
        
        // ㄹ 계열 복합 종성
        {"ekft", "닰", "닰 (ㄷ+ㅏ+ㄹ+ㅅ)"},
        {"ekfx", "닱", "닱 (ㄷ+ㅏ+ㄹ+ㅌ)"},
        {"ekfv", "닲", "닲 (ㄷ+ㅏ+ㄹ+ㅍ)"},
        {"ekfg", "닳", "닳 (ㄷ+ㅏ+ㄹ+ㅎ)"},
        {"dlfr", "읽", "읽 (ㅇ+ㅣ+ㄹ+ㄱ)"},
        {"dmfv", "읊", "읊 (ㅇ+ㅡ+ㄹ+ㅍ)"},
        
        // ㄱ, ㄴ 계열 복합 종성
        {"rkt", "갓", "갓 (ㄱ+ㅏ+ㅅ)"},
        {"sjst", "넌", "넌 (ㄴ+ㅓ+ㄴ)"},
        {"sjgt", "넣", "넣 (ㄴ+ㅓ+ㅎ)"},
        
        // ㅂ 계열 복합 종성
        {"qkqt", "밦", "밦 (ㅂ+ㅏ+ㅂ+ㅅ)"},
        
        // 특수 케이스들 (실제 입력 가능한 것만)
        {"rkqt", "값", "값 (ㄱ+ㅏ+ㅂ+ㅅ)"},
        {"aksg", "많", "많 (ㅁ+ㅏ+ㄴ+ㅎ)"},
        {"dlfr", "읽", "읽 (ㅇ+ㅣ+ㄹ+ㄱ)"},
        {"dmfv", "읊", "읊 (ㅇ+ㅡ+ㄹ+ㅍ)"},
        
        // 추가 복합 종성 테스트 (실제 입력 가능한 것만)
        {"ekfq", "닯", "닯 (ㄷ+ㅏ+ㄹ+ㅂ)"},
        {"ekfa", "닮", "닮 (ㄷ+ㅏ+ㄹ+ㅁ)"},
        {"ekfx", "닱", "닱 (ㄷ+ㅏ+ㄹ+ㅌ)"},
        {"ekfv", "닲", "닲 (ㄷ+ㅏ+ㄹ+ㅍ)"},
        {"ekfg", "닳", "닳 (ㄷ+ㅏ+ㄹ+ㅎ)"},
        
        // 다양한 초성 + 복합 종성 (실제 입력 가능한 것만)
        {"skft", "낤", "낤 (ㄴ+ㅏ+ㄹ+ㅅ)"},
        {"akft", "맔", "맔 (ㅁ+ㅏ+ㄹ+ㅅ)"},
        {"qkft", "밠", "밠 (ㅂ+ㅏ+ㄹ+ㅅ)"},
        {"tkft", "삸", "삸 (ㅅ+ㅏ+ㄹ+ㅅ)"},
        
        // 중성 변화 테스트 (실제 입력 가능한 것만)
        {"rkqt", "값", "값 (ㄱ+ㅏ+ㅂ+ㅅ)"},
        {"rjqt", "겂", "겂 (ㄱ+ㅓ+ㅂ+ㅅ)"},
        {"rpqt", "겞", "겞 (ㄱ+ㅔ+ㅂ+ㅅ)"},
        {"rhqt", "곲", "곲 (ㄱ+ㅗ+ㅂ+ㅅ)"},
        {"rnqt", "굾", "굾 (ㄱ+ㅜ+ㅂ+ㅅ)"},
        {"rmqt", "긊", "긊 (ㄱ+ㅡ+ㅂ+ㅅ)"},
        {"rlqt", "깂", "깂 (ㄱ+ㅣ+ㅂ+ㅅ)"},
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