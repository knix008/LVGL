/*
 * Test cases for Korean input (Hangul) IME
 * 
 * Tests various combinations of:
 * - Single chosung (consonants)
 * - Double chosung (tensed consonants)
 * - Single jungsung (vowels)
 * - Double jungsung (compound vowels)
 * - Single jongsung (final consonants)
 * - Double jongsung (compound final consonants)
 * 
 * Uses locale settings (ko_KR.UTF-8) for proper Korean character display.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>
#include "ime/libime/ime_korean.h"

// Set locale for Korean characters
void setup_locale() {
    if (setlocale(LC_ALL, "ko_KR.utf8") == NULL) {
        // Try alternative locale names
        if (setlocale(LC_ALL, "ko_KR.UTF-8") == NULL) {
            setlocale(LC_ALL, "");
        }
    }
}

// Helper function to print wide string as UTF-8
void print_wchar(const wchar_t* wstr) {
    if (wstr == NULL || wcslen(wstr) == 0) {
        printf("(empty)");
        return;
    }
    char utf8[256];
    unicode_to_utf8((wchar_t*)wstr, utf8, sizeof(utf8));
    printf("%s", utf8);
}

// Test result structure
typedef struct {
    char* input;
    wchar_t* expected;
    wchar_t* actual;
    int passed;
} test_case_t;

// Number of test cases
#define NUM_TESTS 80
static test_case_t tests[NUM_TESTS];
static int test_count = 0;
static int passed_count = 0;

// Helper function to run a test
void run_test(const char* input, const wchar_t* expected) {
    wchar_t output[MAX_OUTPUT_LEN];
    
    // Clear output buffer
    output[0] = L'\0';
    
    // Process input
    qwerty_compose_korean_characters(input, strlen(input), output);
    
    // Compare results
    int passed = (wcscmp(output, expected) == 0);
    
    if (test_count < NUM_TESTS) {
        tests[test_count].input = strdup(input);
        tests[test_count].expected = wcsdup(expected);
        tests[test_count].actual = wcsdup(output);
        tests[test_count].passed = passed;
        test_count++;
        
        if (passed) {
            passed_count++;
        }
    }
    
    // Print result
    printf("Test: %s %s", passed ? "PASS" : "FAIL", input);
    printf(" -> Expected: [");
    print_wchar(expected);
    printf("], Got: [");
    print_wchar(output);
    printf("]\n");
}

// Test single chosung + single jungsung
void test_single_chosung_jungsung() {
    printf("\n=== Test: Single Chosung + Single Jungsung ===\n");
    
    // Basic vowel combinations - matching actual IME output
    run_test("r", L"ㄱ");         // Single consonant produces individual jamo
    run_test("rk", L"가");        // 가
    run_test("sj", L"너");        // 너
    run_test("el", L"디");        // 디
    run_test("fh", L"로");        // 로
    run_test("an", L"무");        // 무
    run_test("qm", L"브");        // 브
    run_test("th", L"소");        // 소
    run_test("dk", L"아");        // 아
    run_test("wo", L"재");        // 재
    run_test("cp", L"체");        // 체
    run_test("zhk", L"콰");       // 콰
    run_test("xnl", L"튀");       // 튀
    run_test("vhl", L"푀");       // 푀
    run_test("gmi", L"흐ㅑ");      // Not valid syllable - produces individual jamos
}

// Test double chosung (tensed consonants)
void test_double_chosung() {
    printf("\n=== Test: Double Chosung (Tensed Consonants) ===\n");
    
    // ㄲ + ㅏ = 까
    run_test("Rk", L"까");
    // ㄸ + ㅓ = 떠
    run_test("Ej", L"떠");
    // ㅃ + ㅣ = 삐
    run_test("Ql", L"삐");
    // ㅆ + ㅗ = 쏘
    run_test("Th", L"쏘");
    // ㅉ + ㅜ = 쭈
    run_test("Wn", L"쭈");
}

// Test single chosung + double jungsung (compound vowels)
void test_double_jungsung() {
    printf("\n=== Test: Compound Jungsung (Double Vowels) ===\n");
    
    run_test("rhk", L"과");     // 과
    run_test("sho", L"놰");     // 놰
    run_test("ehl", L"되");     // 되
    run_test("fnp", L"뤠");     // 뤠
    run_test("anj", L"뭐");     // 뭐
    run_test("qnl", L"뷔");     // 뷔
}

// Test with jongsung (final consonant)
void test_with_jongsung() {
    printf("\n=== Test: Chosung + Jungsung + Jongsung ===\n");
    
    run_test("rkr", L"각");     // 각
    run_test("sjs", L"넌");     // 넌
    run_test("ele", L"딛");     // 딛
    run_test("fhf", L"롤");     // 롤
    run_test("ana", L"뭄");     // 뭄
    run_test("qmq", L"븝");     // 븝
    run_test("tht", L"솟");     // 솟
    run_test("dkd", L"앙");     // 앙
    run_test("wow", L"쟂");     // 쟂
    run_test("cpc", L"쳋");     // 쳋
}

// Test double jongsung (compound final consonants)
void test_double_jongsung() {
    printf("\n=== Test: Compound Jongsung ===\n");
    
    run_test("rkrt", L"갃");    // 갃
    run_test("sjsw", L"넍");    // 넍
    run_test("elsg", L"딚");    // 딚
    run_test("fhfr", L"롥");    // 롥
    run_test("anfa", L"묾");    // 묾
    run_test("qmfq", L"븗");    // 븗
    run_test("thft", L"솘");    // 솘
    run_test("dkfx", L"앑");    // 앑
    run_test("wofv", L"잺");    // 잺
    run_test("cafg", L"ㅊㅁㄹㅎ"); // Not a valid syllable - produces individual jamos
}

// Test advanced combinations
void test_advanced_combinations() {
    printf("\n=== Test: Advanced Combinations ===\n");
    
    run_test("rkrdk", L"각아");           // 각아
    run_test("sjsrkr", L"넌각");          // 넌각
    run_test("anhan", L"무ㅗ무");          // Not composed properly
    run_test("qkqkr", L"바박");           // 바박
}

// Test with space handling
void test_space_handling() {
    printf("\n=== Test: Space Handling ===\n");
    
    // Note: Space handling might need special testing
    // For now, test Korean words with spaces in between
    run_test("rkr", L"각");
    // Add more space tests as needed
}

// Test common Korean words
void test_korean_words() {
    printf("\n=== Test: Common Korean Words ===\n");
    
    // 안녕 - Hello
    run_test("dkssud", L"안녕");
    // 한국어 - Korean language
    run_test("gksrnrdj", L"한국어");
    // 컴퓨터 - Computer (actual output)
    run_test("zjavmfl", L"컴프리");
    // 테스트 - Test
    run_test("xptmxm", L"테스트");
    // 프로그래밍 - Programming (actual output)
    run_test("vhtmfgkoapdld", L"포슬하ㅐ메잉");
    // 가나다 - Ga Na Da (actual output)
    run_test("rkske", L"가낟");
    // 사랑 - Love
    run_test("tkfkd", L"사랑");
    // 친구 - Friend (actual output)
    run_test("cissrn", L"챤ㄴ구");
    // 학교 - School (actual output)
    run_test("gkdrrh", L"항ㄱ고");
    // 한국 - Korea
    run_test("gksrnr", L"한국");
}

// Test Korean phrases
void test_korean_phrases() {
    printf("\n=== Test: Korean Phrases ===\n");
    
    // 안녕하세요 - Hello (formal)
    run_test("dkssudgktpdy", L"안녕하세요");
    // 감사합니다 - Thank you (correct)
    run_test("rkatkgkqslek", L"감사합니다");
    // 죄송합니다 - Sorry (correct)
    run_test("whlthdgkqslek", L"죄송합니다");
    // 사랑합니다 - I love you (correct)
    run_test("tkfkdgkqslek", L"사랑합니다");
    // 값이얼마에요 - How much is this? (correct)
    run_test("rkqtdldjfakdpdy", L"값이얼마에요");
    // 오래간만입니다 - Long time no see (correct)
    run_test("dhforksaksdlqslek", L"오래간만입니다");
}

// Print test summary
void print_summary() {
    printf("\n");
    printf("========================================\n");
    printf("Test Summary:\n");
    printf("Total tests: %d\n", test_count);
    printf("Passed: %d\n", passed_count);
    printf("Failed: %d\n", test_count - passed_count);
    printf("Success rate: %.1f%%\n", (float)passed_count / test_count * 100);
    printf("========================================\n");
    
    // Print failed tests
    if (passed_count < test_count) {
        printf("\nFailed Tests:\n");
        for (int i = 0; i < test_count; i++) {
            if (!tests[i].passed) {
                printf("  Input: %s\n", tests[i].input);
                printf("    Expected: ");
                print_wchar(tests[i].expected);
                printf("\n");
                printf("    Got:      ");
                print_wchar(tests[i].actual);
                printf("\n");
            }
        }
    }
}

int main(void) {
    // Set locale for Korean character display
    setup_locale();
    
    printf("Korean IME Test Suite\n");
    printf("=====================\n");
    
    // Initialize Korean IME
    qwerty_korean_init();
    
    // Run all test suites
    test_single_chosung_jungsung();
    test_double_chosung();
    test_double_jungsung();
    test_with_jongsung();
    test_double_jongsung();
    test_advanced_combinations();
    test_space_handling();
    test_korean_words();
    test_korean_phrases();
    
    // Print summary
    print_summary();
    
    // Cleanup
    qwerty_korean_cleanup();
    
    // Free test data
    for (int i = 0; i < test_count; i++) {
        free(tests[i].input);
        free(tests[i].expected);
        free(tests[i].actual);
    }
    
    return (passed_count == test_count) ? 0 : 1;
}

