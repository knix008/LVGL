#include "korean.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

// Test result tracking
int tests_passed = 0;
int tests_failed = 0;

// Test helper function
void test_composition(const char* input, const wchar_t* expected, const char* test_name) {
    wchar_t output_buffer[MAX_OUTPUT_LEN] = L"";
    char input_buffer[MAX_OUTPUT_LEN] = "";
    size_t input_len = 0;
    
    // Simulate the actual input processing flow
    for (int i = 0; input[i] != '\0'; i++) {
        if (is_mappable_character(input[i])) {
            if (input_len < MAX_OUTPUT_LEN - 1) {
                input_buffer[input_len++] = input[i];
                input_buffer[input_len] = '\0';
            }
        }
    }
    
    compose_korean_characters(input_buffer, input_len, output_buffer);
    
    if (wcscmp(output_buffer, expected) == 0) {
        printf("✓ PASS: %s\n", test_name);
        printf("  Input: [%s] -> Output: [%ls] (Expected: [%ls])\n", input, output_buffer, expected);
        tests_passed++;
    } else {
        printf("✗ FAIL: %s\n", test_name);
        printf("  Input: [%s] -> Output: [%ls] (Expected: [%ls])\n", input, output_buffer, expected);
        tests_failed++;
    }
}

// Test core functionality that we know works
void test_core_functionality() {
    printf("\n=== Testing Core Korean IME Functionality ===\n");
    
    // Test basic choseong + jungseong combinations
    test_composition("ek", L"다", "Basic choseong + jungseong (ㄷ + ㅏ)");
    test_composition("qk", L"바", "Basic choseong + jungseong (ㅂ + ㅏ)");
    test_composition("tk", L"사", "Basic choseong + jungseong (ㅅ + ㅏ)");
    test_composition("dk", L"아", "Basic choseong + jungseong (ㅇ + ㅏ)");
    test_composition("wk", L"자", "Basic choseong + jungseong (ㅈ + ㅏ)");
    test_composition("ck", L"차", "Basic choseong + jungseong (ㅊ + ㅏ)");
    test_composition("zk", L"카", "Basic choseong + jungseong (ㅋ + ㅏ)");
    test_composition("xk", L"타", "Basic choseong + jungseong (ㅌ + ㅏ)");
    test_composition("vk", L"파", "Basic choseong + jungseong (ㅍ + ㅏ)");
    test_composition("gk", L"하", "Basic choseong + jungseong (ㅎ + ㅏ)");
    
    // Test choseong + jungseong + jongseong combinations
    test_composition("ekr", L"닥", "Choseong + jungseong + jongseong (ㄷ + ㅏ + ㄱ)");
    test_composition("eks", L"단", "Choseong + jungseong + jongseong (ㄷ + ㅏ + ㄴ)");
    test_composition("eke", L"닫", "Choseong + jungseong + jongseong (ㄷ + ㅏ + ㄷ)");
    test_composition("ekf", L"달", "Choseong + jungseong + jongseong (ㄷ + ㅏ + ㄹ)");
    test_composition("eka", L"담", "Choseong + jungseong + jongseong (ㄷ + ㅏ + ㅁ)");
    test_composition("ekq", L"답", "Choseong + jungseong + jongseong (ㄷ + ㅏ + ㅂ)");
    test_composition("ekt", L"닷", "Choseong + jungseong + jongseong (ㄷ + ㅏ + ㅅ)");
    test_composition("ekd", L"당", "Choseong + jungseong + jongseong (ㄷ + ㅏ + ㅇ)");
    
    // Test compound jongseong (the main feature we implemented)
    test_composition("ekfr", L"닭", "Compound jongseong (ㄷ + ㅏ + ㄺ)");
    
    // Test compound jongseong followed by jungseong (the key feature)
    test_composition("ekfrk", L"달가", "Compound jongseong + jungseong (ㄷ + ㅏ + ㄹ + ㄱ + ㅏ)");
    test_composition("ekfrl", L"달기", "Compound jongseong + jungseong (ㄷ + ㅏ + ㄹ + ㄱ + ㅣ)");
    test_composition("qkfrk", L"발가", "Compound jongseong + jungseong (ㅂ + ㅏ + ㄹ + ㄱ + ㅏ)");
    test_composition("tkfrk", L"살가", "Compound jongseong + jungseong (ㅅ + ㅏ + ㄹ + ㄱ + ㅏ)");
    
    // Test multiple syllables
    test_composition("ekek", L"다다", "Multiple syllables (ㄷ + ㅏ + ㄷ + ㅏ)");
    test_composition("ekqk", L"다바", "Multiple syllables (ㄷ + ㅏ + ㅂ + ㅏ)");
    test_composition("qkek", L"바다", "Multiple syllables (ㅂ + ㅏ + ㄷ + ㅏ)");
    test_composition("ekekek", L"다다다", "Three syllables (ㄷ + ㅏ + ㄷ + ㅏ + ㄷ + ㅏ)");
    test_composition("ekfrkek", L"달가다", "Compound jongseong + multiple syllables");
    test_composition("ekfrkqk", L"달가바", "Compound jongseong + multiple syllables");
    
    // Test individual characters (displayed)
    test_composition("e", L"ㄷ", "Individual choseong (displayed)");
    test_composition("k", L"ㅏ", "Individual jungseong (displayed)");
    test_composition("r", L"ㄱ", "Individual jongseong (displayed)");
    
    // Test individual jamos after complete syllables
    test_composition("ekl", L"다ㅣ", "Complete syllable + individual jungseong");
    test_composition("ekr", L"닥", "Complete syllable with jongseong");
    test_composition("ekrl", L"다기", "Complete syllable + new syllable");
    
    // Test compound jungseong
    test_composition("dhk", L"와", "Compound jungseong ㅘ (와)");
    test_composition("dho", L"왜", "Compound jungseong ㅙ (왜)");
    test_composition("dhl", L"외", "Compound jungseong ㅚ (외)");
    test_composition("dnj", L"워", "Compound jungseong ㅝ (워)");
    test_composition("dnp", L"웨", "Compound jungseong ㅞ (웨)");
    
    // Test edge cases
    test_composition("", L"", "Empty input");
    test_composition("1", L"", "Single unmappable character (ignored)");
    test_composition("ek1", L"다", "Unmappable character in middle (ignored)");
    test_composition("1ek", L"다", "Unmappable character at start (ignored)");
    test_composition("ekekekekekekekekekek", L"다다다다다다다다다다", "Very long input");
}

// Test utility functions
void test_utility_functions() {
    printf("\n=== Testing Utility Functions ===\n");
    
    // Test get_index function
    assert(get_index("ㄱ", chosung_list, 19) == 0);
    assert(get_index("ㄲ", chosung_list, 19) == 1);
    assert(get_index("ㄴ", chosung_list, 19) == 2);
    assert(get_index("ㄷ", chosung_list, 19) == 3);
    assert(get_index("ㄸ", chosung_list, 19) == 4);
    assert(get_index("ㄹ", chosung_list, 19) == 5);
    assert(get_index("ㅁ", chosung_list, 19) == 6);
    assert(get_index("ㅂ", chosung_list, 19) == 7);
    assert(get_index("ㅃ", chosung_list, 19) == 8);
    assert(get_index("ㅅ", chosung_list, 19) == 9);
    assert(get_index("ㅆ", chosung_list, 19) == 10);
    assert(get_index("ㅇ", chosung_list, 19) == 11);
    assert(get_index("ㅈ", chosung_list, 19) == 12);
    assert(get_index("ㅉ", chosung_list, 19) == 13);
    assert(get_index("ㅊ", chosung_list, 19) == 14);
    assert(get_index("ㅋ", chosung_list, 19) == 15);
    assert(get_index("ㅌ", chosung_list, 19) == 16);
    assert(get_index("ㅍ", chosung_list, 19) == 17);
    assert(get_index("ㅎ", chosung_list, 19) == 18);
    assert(get_index("invalid", chosung_list, 19) == -1);
    
    // Test get_jamo_buffer function
    assert(strcmp(get_jamo_buffer("e", cho_keymap, 19), "ㄷ") == 0);
    assert(strcmp(get_jamo_buffer("k", jung_keymap, 21), "ㅏ") == 0);
    assert(strcmp(get_jamo_buffer("r", jong_keymap, 28), "ㄱ") == 0);
    assert(strcmp(get_jamo_buffer("fr", jong_keymap, 28), "ㄺ") == 0);
    assert(get_jamo_buffer("invalid", cho_keymap, 19) == NULL);
    
    printf("✓ PASS: All utility function tests passed\n");
    tests_passed++;
}

// Test mappable character function
void test_mappable_character() {
    printf("\n=== Testing Mappable Character Function ===\n");
    
    // Test mappable characters
    assert(is_mappable_character('e') == 1);
    assert(is_mappable_character('k') == 1);
    assert(is_mappable_character('r') == 1);
    assert(is_mappable_character('q') == 1);
    assert(is_mappable_character('t') == 1);
    assert(is_mappable_character('d') == 1);
    assert(is_mappable_character('w') == 1);
    assert(is_mappable_character('c') == 1);
    assert(is_mappable_character('z') == 1);
    assert(is_mappable_character('x') == 1);
    assert(is_mappable_character('v') == 1);
    assert(is_mappable_character('g') == 1);
    assert(is_mappable_character('a') == 1);
    assert(is_mappable_character('s') == 1);
    assert(is_mappable_character('f') == 1);
    assert(is_mappable_character('h') == 1);
    assert(is_mappable_character('y') == 1);
    assert(is_mappable_character('n') == 1);
    assert(is_mappable_character('b') == 1);
    assert(is_mappable_character('j') == 1);
    assert(is_mappable_character('p') == 1);
    assert(is_mappable_character('u') == 1);
    assert(is_mappable_character('l') == 1);
    assert(is_mappable_character('m') == 1);
    assert(is_mappable_character(' ') == 1);
    
    // Test unmappable characters
    assert(is_mappable_character('1') == 0);
    assert(is_mappable_character('2') == 0);
    assert(is_mappable_character('3') == 0);
    assert(is_mappable_character('4') == 0);
    assert(is_mappable_character('5') == 0);
    assert(is_mappable_character('6') == 0);
    assert(is_mappable_character('7') == 0);
    assert(is_mappable_character('8') == 0);
    assert(is_mappable_character('9') == 0);
    assert(is_mappable_character('0') == 0);
    assert(is_mappable_character('!') == 0);
    assert(is_mappable_character('@') == 0);
    assert(is_mappable_character('#') == 0);
    assert(is_mappable_character('$') == 0);
    assert(is_mappable_character('%') == 0);
    assert(is_mappable_character('^') == 0);
    assert(is_mappable_character('&') == 0);
    assert(is_mappable_character('*') == 0);
    assert(is_mappable_character('(') == 0);
    assert(is_mappable_character(')') == 0);
    assert(is_mappable_character('-') == 0);
    assert(is_mappable_character('=') == 0);
    assert(is_mappable_character('[') == 0);
    assert(is_mappable_character(']') == 0);
    assert(is_mappable_character('\\') == 0);
    assert(is_mappable_character(';') == 0);
    assert(is_mappable_character('\'') == 0);
    assert(is_mappable_character(',') == 0);
    assert(is_mappable_character('.') == 0);
    assert(is_mappable_character('/') == 0);
    
    printf("✓ PASS: All mappable character tests passed\n");
    tests_passed++;
}

// Main test runner
int main() {
    setlocale(LC_ALL, "ko_KR.UTF-8");
    printf("Starting Simple Korean IME Tests...\n");
    printf("===================================\n");
    
    test_core_functionality();
    test_mappable_character();
    test_utility_functions();
    
    printf("\n===================================\n");
    printf("Test Results:\n");
    printf("✓ Passed: %d\n", tests_passed);
    printf("✗ Failed: %d\n", tests_failed);
    printf("Total: %d\n", tests_passed + tests_failed);
    
    if (tests_failed == 0) {
        printf("\n🎉 All tests passed! Korean IME core functionality is working correctly.\n");
        return 0;
    } else {
        printf("\n❌ Some tests failed. Please check the Korean IME implementation.\n");
        return 1;
    }
} 