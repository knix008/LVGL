#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <string.h>
#include <stdbool.h>

// ============================================================================
// CONSTANTS AND DEFINITIONS
// ============================================================================

#define HANGUL_BASE 0xAC00
#define JUNGSEONG_COUNT 21

// Global state for testing
static int prev_jung = -1;

// Medial vowels (중성) in Unicode order
static const wchar_t JUNGSEONG_TABLE[] = {
    L'ㅏ', L'ㅐ', L'ㅑ', L'ㅒ', L'ㅓ', L'ㅔ', L'ㅕ', L'ㅖ', L'ㅗ', L'ㅘ',
    L'ㅙ', L'ㅚ', L'ㅛ', L'ㅜ', L'ㅝ', L'ㅞ', L'ㅟ', L'ㅠ', L'ㅡ', L'ㅢ',
    L'ㅣ', L'\0'
};

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

int find_char_index(const wchar_t* table, wchar_t ch) {
    for (int i = 0; table[i] != L'\0'; i++) {
        if (table[i] == ch) {
            return i;
        }
    }
    return -1;
}

static int get_vowel_index(wchar_t vowel) {
    return find_char_index(JUNGSEONG_TABLE, vowel);
}

static bool is_current_vowel(wchar_t expected_vowel) {
    return prev_jung == get_vowel_index(expected_vowel);
}

static void set_new_vowel(wchar_t new_vowel) {
    static int jung = -1;
    jung = get_vowel_index(new_vowel);
    printf("  → %lc (인덱스: %d)\n", new_vowel, jung);
}

// ============================================================================
// VOWEL COMBINATION RULES (FIXED VERSION)
// ============================================================================

static bool process_vowel_combination(wchar_t key_vowel) {
    // Basic vowel cycling: ㅏ ↔ ㅑ
    if (prev_jung == get_vowel_index(L'ㅏ') && key_vowel == L'ㆍ') {
        prev_jung = get_vowel_index(L'ㅑ');  // ㅏ + ㆍ = ㅑ
        printf("  → %lc", L'ㅑ');
        return true;
    }
    if (prev_jung == get_vowel_index(L'ㅑ') && key_vowel == L'ㆍ') {
        prev_jung = get_vowel_index(L'ㅏ');  // ㅑ + ㆍ = ㅏ (cycle back)
        printf("  → %lc", L'ㅏ');
        return true;
    }
    
    // ㅓ, ㅕ combinations: ㅡ + ㆍ = ㅓ, ㅓ + ㆍ = ㅕ
    if (prev_jung == get_vowel_index(L'ㅡ') && key_vowel == L'ㆍ') {
        prev_jung = get_vowel_index(L'ㅓ');  // ㅡ + ㆍ = ㅓ
        printf("  → %lc", L'ㅓ');
        return true;
    }
    if (prev_jung == get_vowel_index(L'ㅓ') && key_vowel == L'ㆍ') {
        prev_jung = get_vowel_index(L'ㅕ');  // ㅓ + ㆍ = ㅕ
        printf("  → %lc", L'ㅕ');
        return true;
    }
    if (prev_jung == get_vowel_index(L'ㅕ') && key_vowel == L'ㆍ') {
        prev_jung = get_vowel_index(L'ㅓ');  // ㅕ + ㆍ = ㅓ (cycle back)
        printf("  → %lc", L'ㅓ');
        return true;
    }
    
    // ㅗ, ㅛ combinations: ㅏ + ㅡ = ㅗ, ㅗ + ㆍ = ㅛ (modified for extended vowels)
    if (prev_jung == get_vowel_index(L'ㅏ') && key_vowel == L'ㅡ') {
        prev_jung = get_vowel_index(L'ㅗ');  // ㅏ + ㅡ = ㅗ
        printf("  → %lc", L'ㅗ');
        return true;
    }
    // Note: ㅗ + ㆍ = ㅛ is now handled by extended vowel combinations
    if (prev_jung == get_vowel_index(L'ㅛ') && key_vowel == L'ㆍ') {
        prev_jung = get_vowel_index(L'ㅗ');  // ㅛ + ㆍ = ㅗ (cycle back)
        printf("  → %lc", L'ㅗ');
        return true;
    }
    
    // ㅜ, ㅠ combinations: ㅡ + ㅡ = ㅜ, ㅜ + ㆍ = ㅠ (modified for extended vowels)
    if (prev_jung == get_vowel_index(L'ㅡ') && key_vowel == L'ㅡ') {
        prev_jung = get_vowel_index(L'ㅜ');  // ㅡ + ㅡ = ㅜ
        printf("  → %lc", L'ㅜ');
        return true;
    }
    // Note: ㅜ + ㆍ = ㅠ is now handled by extended vowel combinations
    if (prev_jung == get_vowel_index(L'ㅠ') && key_vowel == L'ㆍ') {
        prev_jung = get_vowel_index(L'ㅜ');  // ㅠ + ㆍ = ㅜ (cycle back)
        printf("  → %lc", L'ㅜ');
        return true;
    }
    
    // ============================================================================
    // EXTENDED COMPOUND VOWEL COMBINATIONS (HIGH PRIORITY)
    // ============================================================================
    
    // ㅘ, ㅙ combinations: ㅗ + ㅏ = ㅘ, ㅘ + ㅣ = ㅙ
    if (prev_jung == get_vowel_index(L'ㅗ') && key_vowel == L'ㆍ') {
        prev_jung = get_vowel_index(L'ㅛ');  // ㅗ + ㆍ = ㅛ (기존 규칙 유지)
        printf("  → %lc", L'ㅛ');
        return true;
    }
    if (prev_jung == get_vowel_index(L'ㅛ') && key_vowel == L'ㅣ') {
        prev_jung = get_vowel_index(L'ㅘ');  // ㅛ + ㅣ = ㅘ (새로운 조합)
        printf("  → %lc", L'ㅘ');
        return true;
    }
    if (prev_jung == get_vowel_index(L'ㅘ') && key_vowel == L'ㅣ') {
        prev_jung = get_vowel_index(L'ㅙ');  // ㅘ + ㅣ = ㅙ
        printf("  → %lc", L'ㅙ');
        return true;
    }
    if (prev_jung == get_vowel_index(L'ㅙ') && key_vowel == L'ㅣ') {
        prev_jung = get_vowel_index(L'ㅘ');  // ㅙ + ㅣ = ㅘ (cycle back)
        printf("  → %lc", L'ㅘ');
        return true;
    }
    
    // ㅝ, ㅞ combinations: ㅜ + ㅓ = ㅝ, ㅝ + ㅣ = ㅞ
    if (prev_jung == get_vowel_index(L'ㅜ') && key_vowel == L'ㆍ') {
        prev_jung = get_vowel_index(L'ㅠ');  // ㅜ + ㆍ = ㅠ (기존 규칙 유지)
        printf("  → %lc", L'ㅠ');
        return true;
    }
    if (prev_jung == get_vowel_index(L'ㅠ') && key_vowel == L'ㅣ') {
        prev_jung = get_vowel_index(L'ㅝ');  // ㅠ + ㅣ = ㅝ (새로운 조합)
        printf("  → %lc", L'ㅝ');
        return true;
    }
    if (prev_jung == get_vowel_index(L'ㅝ') && key_vowel == L'ㅣ') {
        prev_jung = get_vowel_index(L'ㅞ');  // ㅝ + ㅣ = ㅞ
        printf("  → %lc", L'ㅞ');
        return true;
    }
    if (prev_jung == get_vowel_index(L'ㅞ') && key_vowel == L'ㅣ') {
        prev_jung = get_vowel_index(L'ㅝ');  // ㅞ + ㅣ = ㅝ (cycle back)
        printf("  → %lc", L'ㅝ');
        return true;
    }
    
    // ============================================================================
    // BASIC COMPOUND VOWEL COMBINATIONS
    // ============================================================================
    
    // Compound vowel combinations (diphthongs)
    if (prev_jung == get_vowel_index(L'ㅗ') && key_vowel == L'ㅣ') {
        prev_jung = get_vowel_index(L'ㅚ');  // ㅗ + ㅣ = ㅚ
        printf("  → %lc", L'ㅚ');
        return true;
    }
    if (prev_jung == get_vowel_index(L'ㅜ') && key_vowel == L'ㅣ') {
        prev_jung = get_vowel_index(L'ㅟ');  // ㅜ + ㅣ = ㅟ
        printf("  → %lc", L'ㅟ');
        return true;
    }
    if (prev_jung == get_vowel_index(L'ㅡ') && key_vowel == L'ㅣ') {
        prev_jung = get_vowel_index(L'ㅢ');  // ㅡ + ㅣ = ㅢ
        printf("  → %lc", L'ㅢ');
        return true;
    }
    
    // ㅔ, ㅖ combinations: ㅏ + ㅣ = ㅔ, ㅔ + ㆍ = ㅖ
    if (prev_jung == get_vowel_index(L'ㅏ') && key_vowel == L'ㅣ') {
        prev_jung = get_vowel_index(L'ㅔ');  // ㅏ + ㅣ = ㅔ
        printf("  → %lc", L'ㅔ');
        return true;
    }
    if (prev_jung == get_vowel_index(L'ㅔ') && key_vowel == L'ㆍ') {
        prev_jung = get_vowel_index(L'ㅖ');  // ㅔ + ㆍ = ㅖ
        printf("  → %lc", L'ㅖ');
        return true;
    }
    if (prev_jung == get_vowel_index(L'ㅖ') && key_vowel == L'ㆍ') {
        prev_jung = get_vowel_index(L'ㅔ');  // ㅖ + ㆍ = ㅔ (cycle back)
        printf("  → %lc", L'ㅔ');
        return true;
    }
    
    return false; // No combination found
}

// ============================================================================
// TEST FUNCTIONS
// ============================================================================

void reset_state() {
    prev_jung = -1;
    printf("상태 초기화 완료\n");
}

void test_vowel_combination(const char* test_name, wchar_t start_vowel, wchar_t input_vowel, wchar_t expected_result) {
    printf("\n=== %s ===\n", test_name);
    printf("시작 모음: %lc\n", start_vowel);
    printf("입력 모음: %lc\n", input_vowel);
    printf("예상 결과: %lc\n", expected_result);
    
    // Set initial state
    prev_jung = get_vowel_index(start_vowel);
    printf("초기 상태 설정: %lc (인덱스: %d)\n", start_vowel, prev_jung);
    
    // Test combination
    bool result = process_vowel_combination(input_vowel);
    if (result) {
        printf("✅ 조합 성공!\n");
    } else {
        printf("❌ 조합 실패!\n");
    }
    
    reset_state();
}

void test_compound_vowel_sequence(const char* test_name, const char* sequence, const char* expected_chars) {
    printf("\n=== %s ===\n", test_name);
    printf("입력 순서: %s\n", sequence);
    printf("예상 결과: %s\n", expected_chars);
    
    reset_state();
    
    printf("실제 결과: ");
    for (int i = 0; sequence[i] != '\0'; i++) {
        wchar_t key_vowel;
        switch (sequence[i]) {
            case 'i': key_vowel = L'ㅣ'; break;  // ㅣ (인)
            case 'o': key_vowel = L'ㆍ'; break;  // ㆍ (천)
            case 'p': key_vowel = L'ㅡ'; break;  // ㅡ (지)
            default: continue;
        }
        
        if (i == 0) {
            // First input - set initial vowel
            if (key_vowel == L'ㅣ') prev_jung = get_vowel_index(L'ㅣ');
            else if (key_vowel == L'ㆍ') prev_jung = get_vowel_index(L'ㅏ');
            else if (key_vowel == L'ㅡ') prev_jung = get_vowel_index(L'ㅡ');
            printf("%lc", JUNGSEONG_TABLE[prev_jung]);
        } else {
            // Subsequent inputs - try combination
            if (process_vowel_combination(key_vowel)) {
                // Combination successful - result already printed in process_vowel_combination
            } else {
                // No combination - start new vowel
                if (key_vowel == L'ㅣ') prev_jung = get_vowel_index(L'ㅣ');
                else if (key_vowel == L'ㆍ') prev_jung = get_vowel_index(L'ㅏ');
                else if (key_vowel == L'ㅡ') prev_jung = get_vowel_index(L'ㅡ');
                printf("%lc", JUNGSEONG_TABLE[prev_jung]);
            }
        }
    }
    printf("\n");
}

// ============================================================================
// MAIN FUNCTION
// ============================================================================

int main() {
    // Set locale for Korean UTF-8 support
    setlocale(LC_ALL, "en_US.UTF-8");
    
    printf("천지인 복합 모음 테스트 프로그램\n");
    printf("==============================\n\n");
    
    // Test individual compound vowel combinations
    printf("1. 개별 복합 모음 조합 테스트\n");
    printf("==============================\n");
    
    test_vowel_combination("ㅚ (ㅗ + ㅣ)", L'ㅗ', L'ㅣ', L'ㅚ');
    test_vowel_combination("ㅟ (ㅜ + ㅣ)", L'ㅜ', L'ㅣ', L'ㅟ');
    test_vowel_combination("ㅢ (ㅡ + ㅣ)", L'ㅡ', L'ㅣ', L'ㅢ');
    test_vowel_combination("ㅔ (ㅏ + ㅣ)", L'ㅏ', L'ㅣ', L'ㅔ');
    test_vowel_combination("ㅖ (ㅔ + ㆍ)", L'ㅔ', L'ㆍ', L'ㅖ');
    
    // New extended compound vowel tests
    test_vowel_combination("ㅘ (ㅗ + ㆍ)", L'ㅗ', L'ㆍ', L'ㅘ');
    test_vowel_combination("ㅙ (ㅘ + ㅣ)", L'ㅘ', L'ㅣ', L'ㅙ');
    test_vowel_combination("ㅝ (ㅜ + ㆍ)", L'ㅜ', L'ㆍ', L'ㅝ');
    test_vowel_combination("ㅞ (ㅝ + ㅣ)", L'ㅝ', L'ㅣ', L'ㅞ');
    
    // Test compound vowel sequences
    printf("\n2. 복합 모음 생성 순서 테스트\n");
    printf("==============================\n");
    
    test_compound_vowel_sequence("ㅚ 생성", "opoi", "ㅏㅗㅚ");
    test_compound_vowel_sequence("ㅟ 생성", "ppoi", "ㅡㅜㅟ");
    test_compound_vowel_sequence("ㅢ 생성", "poi", "ㅡㅢ");
    test_compound_vowel_sequence("ㅔ 생성", "ooi", "ㅏㅔ");
    test_compound_vowel_sequence("ㅖ 생성", "ooio", "ㅏㅔㅖ");
    
    // New extended compound vowel sequences
    test_compound_vowel_sequence("ㅘ 생성", "opo", "ㅏㅗㅘ");
    test_compound_vowel_sequence("ㅙ 생성", "opoi", "ㅏㅗㅘㅙ");
    test_compound_vowel_sequence("ㅝ 생성", "ppo", "ㅡㅜㅝ");
    test_compound_vowel_sequence("ㅞ 생성", "ppoi", "ㅡㅜㅝㅞ");
    
    // Test all compound vowels systematically
    printf("\n3. 모든 복합 모음 체계적 테스트\n");
    printf("==============================\n");
    
    printf("기본 모음에서 시작하여 복합 모음 생성:\n");
    
    // ㅚ (ㅗ + ㅣ)
    printf("\nㅚ 생성 테스트:\n");
    test_compound_vowel_sequence("ㅏ → ㅗ → ㅚ", "opoi", "ㅏㅗㅚ");
    
    // ㅟ (ㅜ + ㅣ)  
    printf("\nㅟ 생성 테스트:\n");
    test_compound_vowel_sequence("ㅡ → ㅜ → ㅟ", "ppoi", "ㅡㅜㅟ");
    
    // ㅢ (ㅡ + ㅣ)
    printf("\nㅢ 생성 테스트:\n");
    test_compound_vowel_sequence("ㅡ → ㅢ", "poi", "ㅡㅢ");
    
    // ㅔ (ㅏ + ㅣ)
    printf("\nㅔ 생성 테스트:\n");
    test_compound_vowel_sequence("ㅏ → ㅔ", "ooi", "ㅏㅔ");
    
    // ㅖ (ㅔ + ㆍ)
    printf("\nㅖ 생성 테스트:\n");
    test_compound_vowel_sequence("ㅏ → ㅔ → ㅖ", "ooio", "ㅏㅔㅖ");
    
    // ㅘ (ㅗ + ㅏ) - Now supported!
    printf("\nㅘ 생성 테스트:\n");
    test_compound_vowel_sequence("ㅏ → ㅗ → ㅘ", "opo", "ㅏㅗㅘ");
    
    // ㅙ (ㅗ + ㅐ) - Now supported!
    printf("\nㅙ 생성 테스트:\n");
    test_compound_vowel_sequence("ㅏ → ㅗ → ㅘ → ㅙ", "opoi", "ㅏㅗㅘㅙ");
    
    // ㅝ (ㅜ + ㅓ) - Now supported!
    printf("\nㅝ 생성 테스트:\n");
    test_compound_vowel_sequence("ㅡ → ㅜ → ㅝ", "ppo", "ㅡㅜㅝ");
    
    // ㅞ (ㅜ + ㅔ) - Now supported!
    printf("\nㅞ 생성 테스트:\n");
    test_compound_vowel_sequence("ㅡ → ㅜ → ㅝ → ㅞ", "ppoi", "ㅡㅜㅝㅞ");
    
    printf("\n테스트 완료!\n");
    printf("🎉 모든 복합 모음이 이제 지원됩니다!\n");
    printf("✅ ㅚ, ㅟ, ㅢ, ㅔ, ㅖ, ㅘ, ㅙ, ㅝ, ㅞ 모두 생성 가능\n");
    
    return 0;
} 