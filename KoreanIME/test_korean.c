#include "korean.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <locale.h>

// Test result tracking
int tests_passed = 0;
int tests_failed = 0;

// Test helper function
void test_composition(const char* input, const wchar_t* expected, const char* test_name) {
    wchar_t output_buffer[MAX_OUTPUT_LEN] = L"";
    char input_buffer[MAX_OUTPUT_LEN] = "";
    size_t input_len = 0;
    
    // Pass all characters to the IME, including unmappable ones
    for (int i = 0; input[i] != '\0'; i++) {
        if (input_len < MAX_OUTPUT_LEN - 1) {
            input_buffer[input_len++] = input[i];
            input_buffer[input_len] = '\0';
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

// Test basic choseong + jungseong combinations
void test_basic_composition() {
    printf("\n=== Testing Basic Composition ===\n");
    
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
}

// Test choseong + jungseong + jongseong combinations
void test_jongseong_composition() {
    printf("\n=== Testing Jongseong Composition ===\n");
    
    test_composition("ekr", L"닥", "Choseong + jungseong + jongseong (ㄷ + ㅏ + ㄱ)");
    test_composition("eks", L"단", "Choseong + jungseong + jongseong (ㄷ + ㅏ + ㄴ)");
    test_composition("eke", L"닫", "Choseong + jungseong + jongseong (ㄷ + ㅏ + ㄷ)");
    test_composition("ekf", L"달", "Choseong + jungseong + jongseong (ㄷ + ㅏ + ㄹ)");
    test_composition("eka", L"담", "Choseong + jungseong + jongseong (ㄷ + ㅏ + ㅁ)");
    test_composition("ekq", L"답", "Choseong + jungseong + jongseong (ㄷ + ㅏ + ㅂ)");
    test_composition("ekt", L"닷", "Choseong + jungseong + jongseong (ㄷ + ㅏ + ㅅ)");
    test_composition("ekd", L"당", "Choseong + jungseong + jongseong (ㄷ + ㅏ + ㅇ)");
    test_composition("ekw", L"닺", "Choseong + jungseong + jongseong (ㄷ + ㅏ + ㅈ)");
    test_composition("ekc", L"닻", "Choseong + jungseong + jongseong (ㄷ + ㅏ + ㅊ)");
}

// Test compound jongseong combinations
void test_compound_jongseong() {
    printf("\n=== Testing Compound Jongseong ===\n");
    
    test_composition("ekfr", L"닭", "Compound jongseong (ㄷ + ㅏ + ㄺ)");
    test_composition("ekfa", L"닮", "Compound jongseong (ㄷ + ㅏ + ㄻ)");
    test_composition("ekfq", L"닯", "Compound jongseong (ㄷ + ㅏ + ㄼ)");
    test_composition("ekft", L"닰", "Compound jongseong (ㄷ + ㅏ + ㄽ)");
    test_composition("ekfx", L"닱", "Compound jongseong (ㄷ + ㅏ + ㄾ)");
    test_composition("ekfv", L"닲", "Compound jongseong (ㄷ + ㅏ + ㄿ)");
    test_composition("ekfg", L"닳", "Compound jongseong (ㄷ + ㅏ + ㅀ)");
    test_composition("qkqt", L"밦", "Compound jongseong (ㅂ + ㅏ + ㅄ)");
    test_composition("tkrt", L"삯", "Compound jongseong (ㅅ + ㅏ + ㄳ)");
    test_composition("tksw", L"삱", "Compound jongseong (ㅅ + ㅏ + ㄵ)");
    test_composition("tksg", L"삲", "Compound jongseong (ㅅ + ㅏ + ㄶ)");
    
    // Additional compound jongseong tests with different choseong
    test_composition("qkfr", L"밝", "Compound jongseong with ㅂ choseong (ㅂ + ㅏ + ㄺ)");
    test_composition("qkfa", L"밞", "Compound jongseong with ㅂ choseong (ㅂ + ㅏ + ㄻ)");
    test_composition("qkfq", L"밟", "Compound jongseong with ㅂ choseong (ㅂ + ㅏ + ㄼ)");
    test_composition("qkft", L"밠", "Compound jongseong with ㅂ choseong (ㅂ + ㅏ + ㄽ)");
    test_composition("qkfx", L"밡", "Compound jongseong with ㅂ choseong (ㅂ + ㅏ + ㄾ)");
    test_composition("qkfv", L"밢", "Compound jongseong with ㅂ choseong (ㅂ + ㅏ + ㄿ)");
    test_composition("qkfg", L"밣", "Compound jongseong with ㅂ choseong (ㅂ + ㅏ + ㅀ)");
    
    // Compound jongseong with ㅅ choseong
    test_composition("tkfr", L"삵", "Compound jongseong with ㅅ choseong (ㅅ + ㅏ + ㄺ)");
    test_composition("tkfa", L"삶", "Compound jongseong with ㅅ choseong (ㅅ + ㅏ + ㄻ)");
    test_composition("tkfq", L"삷", "Compound jongseong with ㅅ choseong (ㅅ + ㅏ + ㄼ)");
    test_composition("tkft", L"삸", "Compound jongseong with ㅅ choseong (ㅅ + ㅏ + ㄽ)");
    test_composition("tkfx", L"삹", "Compound jongseong with ㅅ choseong (ㅅ + ㅏ + ㄾ)");
    test_composition("tkfv", L"삺", "Compound jongseong with ㅅ choseong (ㅅ + ㅏ + ㄿ)");
    test_composition("tkfg", L"삻", "Compound jongseong with ㅅ choseong (ㅅ + ㅏ + ㅀ)");
    
    // Compound jongseong with ㅇ choseong
    test_composition("dkfr", L"앍", "Compound jongseong with ㅇ choseong (ㅇ + ㅏ + ㄺ)");
    test_composition("dkfa", L"앎", "Compound jongseong with ㅇ choseong (ㅇ + ㅏ + ㄻ)");
    test_composition("dkfq", L"앏", "Compound jongseong with ㅇ choseong (ㅇ + ㅏ + ㄼ)");
    test_composition("dkft", L"앐", "Compound jongseong with ㅇ choseong (ㅇ + ㅏ + ㄽ)");
    test_composition("dkfx", L"앑", "Compound jongseong with ㅇ choseong (ㅇ + ㅏ + ㄾ)");
    test_composition("dkfv", L"앒", "Compound jongseong with ㅇ choseong (ㅇ + ㅏ + ㄿ)");
    test_composition("dkfg", L"앓", "Compound jongseong with ㅇ choseong (ㅇ + ㅏ + ㅀ)");
    
    // Compound jongseong with ㅈ choseong
    test_composition("wkfr", L"잙", "Compound jongseong with ㅈ choseong (ㅈ + ㅏ + ㄺ)");
    test_composition("wkfa", L"잚", "Compound jongseong with ㅈ choseong (ㅈ + ㅏ + ㄻ)");
    test_composition("wkfq", L"잛", "Compound jongseong with ㅈ choseong (ㅈ + ㅏ + ㄼ)");
    test_composition("wkft", L"잜", "Compound jongseong with ㅈ choseong (ㅈ + ㅏ + ㄽ)");
    test_composition("wkfx", L"잝", "Compound jongseong with ㅈ choseong (ㅈ + ㅏ + ㄾ)");
    test_composition("wkfv", L"잞", "Compound jongseong with ㅈ choseong (ㅈ + ㅏ + ㄿ)");
    test_composition("wkfg", L"잟", "Compound jongseong with ㅈ choseong (ㅈ + ㅏ + ㅀ)");
    
    // Compound jongseong with ㅊ choseong
    test_composition("ckfr", L"찱", "Compound jongseong with ㅊ choseong (ㅊ + ㅏ + ㄺ)");
    test_composition("ckfa", L"찲", "Compound jongseong with ㅊ choseong (ㅊ + ㅏ + ㄻ)");
    test_composition("ckfq", L"찳", "Compound jongseong with ㅊ choseong (ㅊ + ㅏ + ㄼ)");
    test_composition("ckft", L"찴", "Compound jongseong with ㅊ choseong (ㅊ + ㅏ + ㄽ)");
    test_composition("ckfx", L"찵", "Compound jongseong with ㅊ choseong (ㅊ + ㅏ + ㄾ)");
    test_composition("ckfv", L"찶", "Compound jongseong with ㅊ choseong (ㅊ + ㅏ + ㄿ)");
    test_composition("ckfg", L"찷", "Compound jongseong with ㅊ choseong (ㅊ + ㅏ + ㅀ)");
    
    // Compound jongseong with ㅋ choseong
    test_composition("zkfr", L"칽", "Compound jongseong with ㅋ choseong (ㅋ + ㅏ + ㄺ)");
    test_composition("zkfa", L"칾", "Compound jongseong with ㅋ choseong (ㅋ + ㅏ + ㄻ)");
    test_composition("zkfq", L"칿", "Compound jongseong with ㅋ choseong (ㅋ + ㅏ + ㄼ)");
    test_composition("zkft", L"캀", "Compound jongseong with ㅋ choseong (ㅋ + ㅏ + ㄽ)");
    test_composition("zkfx", L"캁", "Compound jongseong with ㅋ choseong (ㅋ + ㅏ + ㄾ)");
    test_composition("zkfv", L"캂", "Compound jongseong with ㅋ choseong (ㅋ + ㅏ + ㄿ)");
    test_composition("zkfg", L"캃", "Compound jongseong with ㅋ choseong (ㅋ + ㅏ + ㅀ)");
    
    // Compound jongseong with ㅌ choseong
    test_composition("xkfr", L"탉", "Compound jongseong with ㅌ choseong (ㅌ + ㅏ + ㄺ)");
    test_composition("xkfa", L"탊", "Compound jongseong with ㅌ choseong (ㅌ + ㅏ + ㄻ)");
    test_composition("xkfq", L"탋", "Compound jongseong with ㅌ choseong (ㅌ + ㅏ + ㄼ)");
    test_composition("xkft", L"탌", "Compound jongseong with ㅌ choseong (ㅌ + ㅏ + ㄽ)");
    test_composition("xkfx", L"탍", "Compound jongseong with ㅌ choseong (ㅌ + ㅏ + ㄾ)");
    test_composition("xkfv", L"탎", "Compound jongseong with ㅌ choseong (ㅌ + ㅏ + ㄿ)");
    test_composition("xkfg", L"탏", "Compound jongseong with ㅌ choseong (ㅌ + ㅏ + ㅀ)");
    
    // Compound jongseong with ㅍ choseong
    test_composition("vkfr", L"팕", "Compound jongseong with ㅍ choseong (ㅍ + ㅏ + ㄺ)");
    test_composition("vkfa", L"팖", "Compound jongseong with ㅍ choseong (ㅍ + ㅏ + ㄻ)");
    test_composition("vkfq", L"팗", "Compound jongseong with ㅍ choseong (ㅍ + ㅏ + ㄼ)");
    test_composition("vkft", L"팘", "Compound jongseong with ㅍ choseong (ㅍ + ㅏ + ㄽ)");
    test_composition("vkfx", L"팙", "Compound jongseong with ㅍ choseong (ㅍ + ㅏ + ㄾ)");
    test_composition("vkfv", L"팚", "Compound jongseong with ㅍ choseong (ㅍ + ㅏ + ㄿ)");
    test_composition("vkfg", L"팛", "Compound jongseong with ㅍ choseong (ㅍ + ㅏ + ㅀ)");
    
    // Compound jongseong with ㅎ choseong
    test_composition("gkfr", L"핡", "Compound jongseong with ㅎ choseong (ㅎ + ㅏ + ㄺ)");
    test_composition("gkfa", L"핢", "Compound jongseong with ㅎ choseong (ㅎ + ㅏ + ㄻ)");
    test_composition("gkfq", L"핣", "Compound jongseong with ㅎ choseong (ㅎ + ㅏ + ㄼ)");
    test_composition("gkft", L"핤", "Compound jongseong with ㅎ choseong (ㅎ + ㅏ + ㄽ)");
    test_composition("gkfx", L"핥", "Compound jongseong with ㅎ choseong (ㅎ + ㅏ + ㄾ)");
    test_composition("gkfv", L"핦", "Compound jongseong with ㅎ choseong (ㅎ + ㅏ + ㄿ)");
    test_composition("gkfg", L"핧", "Compound jongseong with ㅎ choseong (ㅎ + ㅏ + ㅀ)");
}

// Test compound jongseong with different jungseong
void test_compound_jongseong_with_different_jungseong() {
    printf("\n=== Testing Compound Jongseong with Different Jungseong ===\n");
    
    // Test with ㅣ jungseong
    test_composition("elfr", L"딝", "Compound jongseong with ㅣ jungseong (ㄷ + ㅣ + ㄺ)");
    test_composition("elfa", L"딞", "Compound jongseong with ㅣ jungseong (ㄷ + ㅣ + ㄻ)");
    test_composition("elfq", L"딟", "Compound jongseong with ㅣ jungseong (ㄷ + ㅣ + ㄼ)");
    test_composition("elft", L"딠", "Compound jongseong with ㅣ jungseong (ㄷ + ㅣ + ㄽ)");
    test_composition("elfx", L"딡", "Compound jongseong with ㅣ jungseong (ㄷ + ㅣ + ㄾ)");
    test_composition("elfv", L"딢", "Compound jongseong with ㅣ jungseong (ㄷ + ㅣ + ㄿ)");
    test_composition("elfg", L"딣", "Compound jongseong with ㅣ jungseong (ㄷ + ㅣ + ㅀ)");
    
    // Test with ㅜ jungseong
    test_composition("enf", L"둘", "Basic jongseong with ㅜ jungseong (ㄷ + ㅜ + ㄹ)");
    test_composition("enfr", L"둙", "Compound jongseong with ㅜ jungseong (ㄷ + ㅜ + ㄺ)");
    test_composition("enfa", L"둚", "Compound jongseong with ㅜ jungseong (ㄷ + ㅜ + ㄻ)");
    test_composition("enfq", L"둛", "Compound jongseong with ㅜ jungseong (ㄷ + ㅜ + ㄼ)");
    test_composition("enft", L"둜", "Compound jongseong with ㅜ jungseong (ㄷ + ㅜ + ㄽ)");
    test_composition("enfx", L"둝", "Compound jongseong with ㅜ jungseong (ㄷ + ㅜ + ㄾ)");
    test_composition("enfv", L"둞", "Compound jongseong with ㅜ jungseong (ㄷ + ㅜ + ㄿ)");
    test_composition("enfg", L"둟", "Compound jongseong with ㅜ jungseong (ㄷ + ㅜ + ㅀ)");
    
    // Test with ㅗ jungseong
    test_composition("ehf", L"돌", "Basic jongseong with ㅗ jungseong (ㄷ + ㅗ + ㄹ)");
    test_composition("ehfr", L"돍", "Compound jongseong with ㅗ jungseong (ㄷ + ㅗ + ㄺ)");
    test_composition("ehfa", L"돎", "Compound jongseong with ㅗ jungseong (ㄷ + ㅗ + ㄻ)");
    test_composition("ehfq", L"돏", "Compound jongseong with ㅗ jungseong (ㄷ + ㅗ + ㄼ)");
    test_composition("ehft", L"돐", "Compound jongseong with ㅗ jungseong (ㄷ + ㅗ + ㄽ)");
    test_composition("ehfx", L"돑", "Compound jongseong with ㅗ jungseong (ㄷ + ㅗ + ㄾ)");
    test_composition("ehfv", L"돒", "Compound jongseong with ㅗ jungseong (ㄷ + ㅗ + ㄿ)");
    test_composition("ehfg", L"돓", "Compound jongseong with ㅗ jungseong (ㄷ + ㅗ + ㅀ)");
    
    // Test with ㅓ jungseong
    test_composition("ejf", L"덜", "Basic jongseong with ㅓ jungseong (ㄷ + ㅓ + ㄹ)");
    test_composition("ejfr", L"덝", "Compound jongseong with ㅓ jungseong (ㄷ + ㅓ + ㄺ)");
    test_composition("ejfa", L"덞", "Compound jongseong with ㅓ jungseong (ㄷ + ㅓ + ㄻ)");
    test_composition("ejfq", L"덟", "Compound jongseong with ㅓ jungseong (ㄷ + ㅓ + ㄼ)");
    test_composition("ejft", L"덠", "Compound jongseong with ㅓ jungseong (ㄷ + ㅓ + ㄽ)");
    test_composition("ejfx", L"덡", "Compound jongseong with ㅓ jungseong (ㄷ + ㅓ + ㄾ)");
    test_composition("ejfv", L"덢", "Compound jongseong with ㅓ jungseong (ㄷ + ㅓ + ㄿ)");
    test_composition("ejfg", L"덣", "Compound jongseong with ㅓ jungseong (ㄷ + ㅓ + ㅀ)");
    
    // Test with ㅔ jungseong
    test_composition("epf", L"델", "Basic jongseong with ㅔ jungseong (ㄷ + ㅔ + ㄹ)");
    test_composition("epfr", L"덹", "Compound jongseong with ㅔ jungseong (ㄷ + ㅔ + ㄺ)");
    test_composition("epfa", L"덺", "Compound jongseong with ㅔ jungseong (ㄷ + ㅔ + ㄻ)");
    test_composition("epfq", L"덻", "Compound jongseong with ㅔ jungseong (ㄷ + ㅔ + ㄼ)");
    test_composition("epft", L"덼", "Compound jongseong with ㅔ jungseong (ㄷ + ㅔ + ㄽ)");
    test_composition("epfx", L"덽", "Compound jongseong with ㅔ jungseong (ㄷ + ㅔ + ㄾ)");
    test_composition("epfv", L"덾", "Compound jongseong with ㅔ jungseong (ㄷ + ㅔ + ㄿ)");
    test_composition("epfg", L"덿", "Compound jongseong with ㅔ jungseong (ㄷ + ㅔ + ㅀ)");
}

// Test compound jongseong edge cases
void test_compound_jongseong_edge_cases() {
    printf("\n=== Testing Compound Jongseong Edge Cases ===\n");
    
    // Test with compound jungseong
    test_composition("dhkfr", L"왉", "Compound jongseong with compound jungseong ㅘ (ㅇ + ㅘ + ㄺ)");
    test_composition("dhkfa", L"왊", "Compound jongseong with compound jungseong ㅘ (ㅇ + ㅘ + ㄻ)");
    test_composition("dhkfq", L"왋", "Compound jongseong with compound jungseong ㅘ (ㅇ + ㅘ + ㄼ)");
    test_composition("dhkft", L"왌", "Compound jongseong with compound jungseong ㅘ (ㅇ + ㅘ + ㄽ)");
    test_composition("dhkfx", L"왍", "Compound jongseong with compound jungseong ㅘ (ㅇ + ㅘ + ㄾ)");
    test_composition("dhkfv", L"왎", "Compound jongseong with compound jungseong ㅘ (ㅇ + ㅘ + ㄿ)");
    test_composition("dhkfg", L"왏", "Compound jongseong with compound jungseong ㅘ (ㅇ + ㅘ + ㅀ)");
    
    // Test with other compound jungseong
    test_composition("dhlfr", L"욁", "Compound jongseong with compound jungseong ㅚ (ㅇ + ㅚ + ㄺ)");
    test_composition("dhlfa", L"욂", "Compound jongseong with compound jungseong ㅚ (ㅇ + ㅚ + ㄻ)");
    test_composition("dhlfq", L"욃", "Compound jongseong with compound jungseong ㅚ (ㅇ + ㅚ + ㄼ)");
    test_composition("dhlft", L"욄", "Compound jongseong with compound jungseong ㅚ (ㅇ + ㅚ + ㄽ)");
    test_composition("dhlfx", L"욅", "Compound jongseong with compound jungseong ㅚ (ㅇ + ㅚ + ㄾ)");
    test_composition("dhlfv", L"욆", "Compound jongseong with compound jungseong ㅚ (ㅇ + ㅚ + ㄿ)");
    test_composition("dhlfg", L"욇", "Compound jongseong with compound jungseong ㅚ (ㅇ + ㅚ + ㅀ)");
    
    // Test with ㅝ compound jungseong
    test_composition("dnjfr", L"웕", "Compound jongseong with compound jungseong ㅝ (ㅇ + ㅝ + ㄺ)");
    test_composition("dnjfa", L"웖", "Compound jongseong with compound jungseong ㅝ (ㅇ + ㅝ + ㄻ)");
    test_composition("dnjfq", L"웗", "Compound jongseong with compound jungseong ㅝ (ㅇ + ㅝ + ㄼ)");
    test_composition("dnjft", L"웘", "Compound jongseong with compound jungseong ㅝ (ㅇ + ㅝ + ㄽ)");
    test_composition("dnjfx", L"웙", "Compound jongseong with compound jungseong ㅝ (ㅇ + ㅝ + ㄾ)");
    test_composition("dnjfv", L"웚", "Compound jongseong with compound jungseong ㅝ (ㅇ + ㅝ + ㄿ)");
    test_composition("dnjfg", L"웛", "Compound jongseong with compound jungseong ㅝ (ㅇ + ㅝ + ㅀ)");
    
    // Test with ㅞ compound jungseong
    test_composition("dnpfr", L"웱", "Compound jongseong with compound jungseong ㅞ (ㅇ + ㅞ + ㄺ)");
    test_composition("dnpfa", L"웲", "Compound jongseong with compound jungseong ㅞ (ㅇ + ㅞ + ㄻ)");
    test_composition("dnpfq", L"웳", "Compound jongseong with compound jungseong ㅞ (ㅇ + ㅞ + ㄼ)");
    test_composition("dnpft", L"웴", "Compound jongseong with compound jungseong ㅞ (ㅇ + ㅞ + ㄽ)");
    test_composition("dnpfx", L"웵", "Compound jongseong with compound jungseong ㅞ (ㅇ + ㅞ + ㄾ)");
    test_composition("dnpfv", L"웶", "Compound jongseong with compound jungseong ㅞ (ㅇ + ㅞ + ㄿ)");
    test_composition("dnpfg", L"웷", "Compound jongseong with compound jungseong ㅞ (ㅇ + ㅞ + ㅀ)");
    
    // Test with ㅙ compound jungseong
    test_composition("dhofr", L"왥", "Compound jongseong with compound jungseong ㅙ (ㅇ + ㅙ + ㄺ)");
    test_composition("dhofa", L"왦", "Compound jongseong with compound jungseong ㅙ (ㅇ + ㅙ + ㄻ)");
    test_composition("dhofq", L"왧", "Compound jongseong with compound jungseong ㅙ (ㅇ + ㅙ + ㄼ)");
    test_composition("dhoft", L"왨", "Compound jongseong with compound jungseong ㅙ (ㅇ + ㅙ + ㄽ)");
    test_composition("dhofx", L"왩", "Compound jongseong with compound jungseong ㅙ (ㅇ + ㅙ + ㄾ)");
    test_composition("dhofv", L"왪", "Compound jongseong with compound jungseong ㅙ (ㅇ + ㅙ + ㄿ)");
    test_composition("dhofg", L"왫", "Compound jongseong with compound jungseong ㅙ (ㅇ + ㅙ + ㅀ)");
}

// Test compound jongseong followed by jungseong
void test_compound_jongseong_with_jungseong() {
    printf("\n=== Testing Compound Jongseong with Following Jungseong ===\n");
    
    test_composition("ekfrk", L"달가", "Compound jongseong + jungseong (ㄷ + ㅏ + ㄹ + ㄱ + ㅏ)");
    test_composition("ekfrl", L"달기", "Compound jongseong + jungseong (ㄷ + ㅏ + ㄹ + ㄱ + ㅣ)");
    test_composition("qkfrk", L"발가", "Compound jongseong + jungseong (ㅂ + ㅏ + ㄹ + ㄱ + ㅏ)");
    test_composition("tkfrk", L"살가", "Compound jongseong + jungseong (ㅅ + ㅏ + ㄹ + ㄱ + ㅏ)");
    test_composition("ekfak", L"달마", "Compound jongseong + jungseong (ㄷ + ㅏ + ㄹ + ㅁ + ㅏ)");
    test_composition("ekfqk", L"달바", "Compound jongseong + jungseong (ㄷ + ㅏ + ㄹ + ㅂ + ㅏ)");
    
    // Additional tests with different compound jongseong
    test_composition("ekfak", L"달마", "Compound jongseong ㄻ + jungseong (ㄷ + ㅏ + ㄹ + ㅁ + ㅏ)");
    test_composition("ekfql", L"달비", "Compound jongseong ㄼ + jungseong (ㄷ + ㅏ + ㄹ + ㅂ + ㅣ)");
    test_composition("ekftk", L"달사", "Compound jongseong ㄽ + jungseong (ㄷ + ㅏ + ㄹ + ㅅ + ㅏ)");
    test_composition("ekfxk", L"달타", "Compound jongseong ㄾ + jungseong (ㄷ + ㅏ + ㄹ + ㅌ + ㅏ)");
    test_composition("ekfvk", L"달파", "Compound jongseong ㄿ + jungseong (ㄷ + ㅏ + ㄹ + ㅍ + ㅏ)");
    test_composition("ekfgk", L"달하", "Compound jongseong ㅀ + jungseong (ㄷ + ㅏ + ㄹ + ㅎ + ㅏ)");
    
    // Test with compound jungseong following
    test_composition("ekfrdhk", L"닭와", "Compound jongseong + compound jungseong (ㄷ + ㅏ + ㄺ + ㅇ + ㅘ)");
    test_composition("ekfrdhl", L"닭외", "Compound jongseong + compound jungseong (ㄷ + ㅏ + ㄺ + ㅇ + ㅚ)");
    test_composition("ekfrdnj", L"닭워", "Compound jongseong + compound jungseong (ㄷ + ㅏ + ㄺ + ㅇ + ㅝ)");
    test_composition("ekfrdnp", L"닭웨", "Compound jongseong + compound jungseong (ㄷ + ㅏ + ㄺ + ㅇ + ㅞ)");
    test_composition("ekfrdho", L"닭왜", "Compound jongseong + compound jungseong (ㄷ + ㅏ + ㄺ + ㅇ + ㅙ)");
    
    // Test with different choseong following compound jongseong
    test_composition("ekfrqk", L"닭바", "Compound jongseong + different choseong (ㄷ + ㅏ + ㄺ + ㅂ + ㅏ)");
    test_composition("ekfrtk", L"닭사", "Compound jongseong + different choseong (ㄷ + ㅏ + ㄺ + ㅅ + ㅏ)");
    test_composition("ekfrdk", L"닭아", "Compound jongseong + different choseong (ㄷ + ㅏ + ㄺ + ㅇ + ㅏ)");
    test_composition("ekfrwk", L"닭자", "Compound jongseong + different choseong (ㄷ + ㅏ + ㄺ + ㅈ + ㅏ)");
    test_composition("ekfrck", L"닭차", "Compound jongseong + different choseong (ㄷ + ㅏ + ㄺ + ㅊ + ㅏ)");
}

// Test multiple compound jongseong in sequence
void test_multiple_compound_jongseong() {
    printf("\n=== Testing Multiple Compound Jongseong in Sequence ===\n");
    
    test_composition("ekfrkfr", L"달갉", "Multiple compound jongseong (ㄷ + ㅏ + ㄹ + ㄱ + ㅏ + ㄷ + ㅏ + ㄹ + ㄱ)");
    test_composition("ekfrkfa", L"달갊", "Multiple compound jongseong (ㄷ + ㅏ + ㄹ + ㄱ + ㅏ + ㄷ + ㅏ + ㄹ + ㅁ)");
    test_composition("ekfrkfq", L"달갋", "Multiple compound jongseong (ㄷ + ㅏ + ㄹ + ㄱ + ㅏ + ㄷ + ㅏ + ㄹ + ㅂ)");
    test_composition("ekfrkft", L"달갌", "Multiple compound jongseong (ㄷ + ㅏ + ㄹ + ㄱ + ㅏ + ㄷ + ㅏ + ㄹ + ㅅ)");
    test_composition("ekfrkfx", L"달갍", "Multiple compound jongseong (ㄷ + ㅏ + ㄹ + ㄱ + ㅏ + ㄷ + ㅏ + ㄹ + ㅌ)");
    test_composition("ekfrkfv", L"달갎", "Multiple compound jongseong (ㄷ + ㅏ + ㄹ + ㄱ + ㅏ + ㄷ + ㅏ + ㄹ + ㅍ)");
    test_composition("ekfrkfg", L"달갏", "Multiple compound jongseong (ㄷ + ㅏ + ㄹ + ㄱ + ㅏ + ㄷ + ㅏ + ㄹ + ㅎ)");
    
    // Test with different choseong in sequence
    test_composition("qkfrkfr", L"발갉", "Multiple compound jongseong with different choseong");
    test_composition("tkfrkfr", L"살갉", "Multiple compound jongseong with different choseong");
    test_composition("dkfrkfr", L"알갉", "Multiple compound jongseong with different choseong");
    test_composition("wkfrkfr", L"잘갉", "Multiple compound jongseong with different choseong");
    test_composition("ckfrkfr", L"찰갉", "Multiple compound jongseong with different choseong");
    
    // Test with compound jungseong in sequence
    test_composition("dhkfrkfr", L"왈갉", "Compound jungseong + compound jongseong sequence");
    test_composition("dhlfrkfr", L"욀갉", "Compound jungseong + compound jongseong sequence");
    test_composition("dnjfrkfr", L"월갉", "Compound jungseong + compound jongseong sequence");
    test_composition("dnpfrkfr", L"웰갉", "Compound jungseong + compound jongseong sequence");
    test_composition("dhofrkfr", L"왤갉", "Compound jungseong + compound jongseong sequence");
}

// Test compound jongseong with backspace simulation
void test_compound_jongseong_backspace() {
    printf("\n=== Testing Compound Jongseong Backspace Simulation ===\n");
    
    // Simulate typing "ekfrk" and then backspacing
    test_composition("ekfr", L"닭", "Before backspace (complete compound jongseong)");
    test_composition("ekf", L"달", "After backspace (removed 'r' from compound jongseong)");
    test_composition("ek", L"다", "After another backspace (removed 'f' from jongseong)");
    test_composition("e", L"ㄷ", "After another backspace (removed 'k' from jungseong)");
    test_composition("", L"", "After final backspace (removed 'e' from choseong)");
    
    // Test backspace with different compound jongseong
    test_composition("ekfa", L"닮", "Compound jongseong ㄻ before backspace");
    test_composition("ekf", L"달", "After backspace (removed 'a' from compound jongseong)");
    
    test_composition("ekfq", L"닯", "Compound jongseong ㄼ before backspace");
    test_composition("ekf", L"달", "After backspace (removed 'q' from compound jongseong)");
    
    test_composition("ekft", L"닰", "Compound jongseong ㄽ before backspace");
    test_composition("ekf", L"달", "After backspace (removed 't' from compound jongseong)");
    
    test_composition("ekfx", L"닱", "Compound jongseong ㄾ before backspace");
    test_composition("ekf", L"달", "After backspace (removed 'x' from compound jongseong)");
    
    test_composition("ekfv", L"닲", "Compound jongseong ㄿ before backspace");
    test_composition("ekf", L"달", "After backspace (removed 'v' from compound jongseong)");
    
    test_composition("ekfg", L"닳", "Compound jongseong ㅀ before backspace");
    test_composition("ekf", L"달", "After backspace (removed 'g' from compound jongseong)");
    
    // Test backspace with compound jongseong followed by jungseong
    test_composition("ekfrk", L"달가", "Compound jongseong + jungseong before backspace");
    test_composition("ekfr", L"닭", "After backspace (removed 'k' from following jungseong)");
    test_composition("ekf", L"달", "After another backspace (removed 'r' from compound jongseong)");
    test_composition("ek", L"다", "After another backspace (removed 'f' from jongseong)");
}

// Test compound jongseong with invalid combinations
void test_compound_jongseong_invalid_combinations() {
    printf("\n=== Testing Compound Jongseong Invalid Combinations ===\n");
    
    // Test invalid compound jongseong combinations (should fall back to individual jamos)
    test_composition("ekfr", L"닭", "Valid compound jongseong ㄺ");
    test_composition("ekfa", L"닮", "Valid compound jongseong ㄻ");
    test_composition("ekfq", L"닯", "Valid compound jongseong ㄼ");
    test_composition("ekft", L"닰", "Valid compound jongseong ㄽ");
    test_composition("ekfx", L"닱", "Valid compound jongseong ㄾ");
    test_composition("ekfv", L"닲", "Valid compound jongseong ㄿ");
    test_composition("ekfg", L"닳", "Valid compound jongseong ㅀ");
    
    // Test with invalid second character in compound jongseong
    test_composition("ekf1", L"달", "Invalid second character in compound jongseong (ignored)");
    test_composition("ekf2", L"달", "Invalid second character in compound jongseong (ignored)");
    test_composition("ekf3", L"달", "Invalid second character in compound jongseong (ignored)");
    test_composition("ekf!", L"달", "Invalid second character in compound jongseong (ignored)");
    test_composition("ekf@", L"달", "Invalid second character in compound jongseong (ignored)");
    
    // Test with invalid first character in compound jongseong
    test_composition("ek1r", L"다ㄱ", "Invalid first character in compound jongseong (displayed as individual)");
    test_composition("ek2r", L"다ㄱ", "Invalid first character in compound jongseong (displayed as individual)");
    test_composition("ek!r", L"다ㄱ", "Invalid first character in compound jongseong (displayed as individual)");
    test_composition("ek@r", L"다ㄱ", "Invalid first character in compound jongseong (displayed as individual)");
}

// Test compound jungseong combinations
void test_compound_jungseong() {
    printf("\n=== Testing Compound Jungseong ===\n");
    
    test_composition("dhk", L"와", "Compound jungseong ㅘ (와)");
    test_composition("dho", L"왜", "Compound jungseong ㅙ (왜)");
    test_composition("dhl", L"외", "Compound jungseong ㅚ (외)");
    test_composition("dnj", L"워", "Compound jungseong ㅝ (워)");
    test_composition("dnp", L"웨", "Compound jungseong ㅞ (웨)");
    
    // Test with different choseong
    test_composition("ehk", L"돠", "Compound jungseong with ㄷ choseong");
    test_composition("qhk", L"봐", "Compound jungseong with ㅂ choseong");
    test_composition("thk", L"솨", "Compound jungseong with ㅅ choseong");
}

// Test multiple syllables
void test_multiple_syllables() {
    printf("\n=== Testing Multiple Syllables ===\n");
    
    test_composition("ekek", L"다다", "Multiple syllables (ㄷ + ㅏ + ㄷ + ㅏ)");
    test_composition("ekqk", L"다바", "Multiple syllables (ㄷ + ㅏ + ㅂ + ㅏ)");
    test_composition("qkek", L"바다", "Multiple syllables (ㅂ + ㅏ + ㄷ + ㅏ)");
    test_composition("ekekek", L"다다다", "Three syllables (ㄷ + ㅏ + ㄷ + ㅏ + ㄷ + ㅏ)");
    test_composition("ekfrkek", L"달가다", "Compound jongseong + multiple syllables");
    test_composition("ekfrkqk", L"달가바", "Compound jongseong + multiple syllables");
}

// Test individual jamos
void test_individual_jamos() {
    printf("\n=== Testing Individual Jamos ===\n");
    
    test_composition("e", L"ㄷ", "Individual choseong (displayed)");
    test_composition("k", L"ㅏ", "Individual jungseong (displayed)");
    test_composition("r", L"ㄱ", "Individual jongseong (displayed)");
    test_composition("ek", L"다", "Choseong + jungseong (not individual)");
    test_composition("ekr", L"닥", "Complete syllable (not individual)");
    
    // Test individual jamos after complete syllables
    test_composition("ekl", L"다ㅣ", "Complete syllable + individual jungseong");
    test_composition("eklr", L"다ㅣㄱ", "Complete syllable + individual jungseong + individual jongseong");
    test_composition("ekrl", L"다기", "Complete syllable + new syllable");
}

// Test edge cases
void test_edge_cases() {
    printf("\n=== Testing Edge Cases ===\n");
    
    test_composition("", L"", "Empty input");
    test_composition("1", L"", "Single unmappable character (ignored)");
    test_composition("ek1", L"다", "Unmappable character in middle (ignored)");
    test_composition("1ek", L"다", "Unmappable character at start (ignored)");
    test_composition("ek1", L"다", "Unmappable character at end (ignored)");
    test_composition("ekekekekekekekekekek", L"다다다다다다다다다다", "Very long input");
}

// Test space handling
void test_space_handling() {
    printf("\n=== Testing Space Handling ===\n");
    
    test_composition("ek k", L"다ㅏ", "Space between syllables (processed as jungseong)");
    test_composition(" ek", L"다", "Space at start (ignored)");
    test_composition("ek ", L"다", "Space at end (ignored)");
    test_composition("ek k ek", L"다ㅏ다", "Multiple spaces (processed as jungseong)");
}

// Test backspace simulation
void test_backspace_simulation() {
    printf("\n=== Testing Backspace Simulation ===\n");
    
    // Simulate typing "ekfrk" and then backspacing
    test_composition("ekfr", L"닭", "Before backspace");
    test_composition("ekf", L"달", "After backspace (removed 'r')");
    test_composition("ek", L"다", "After another backspace (removed 'f')");
    test_composition("e", L"ㄷ", "After another backspace (removed 'k')");
    test_composition("", L"", "After final backspace (removed 'e')");
}

// Test complex combinations
void test_complex_combinations() {
    printf("\n=== Testing Complex Combinations ===\n");
    
    test_composition("dhkek", L"와다", "Compound jungseong + basic syllable");
    test_composition("ekdhk", L"다와", "Basic syllable + compound jungseong");
    test_composition("dhkfrk", L"왈가", "Compound jungseong + compound jongseong");
    test_composition("ekfrkdhk", L"달가와", "Compound jongseong + compound jungseong");
    test_composition("dhkdhk", L"와와", "Multiple compound jungseong");
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
    assert(strcmp(get_jamo_buffer("k", jung_keymap, 23), "ㅏ") == 0);
    assert(strcmp(get_jamo_buffer("r", jong_keymap, 28), "ㄱ") == 0);
    assert(strcmp(get_jamo_buffer("fr", jong_keymap, 28), "ㄺ") == 0);
    assert(strcmp(get_jamo_buffer("hl", jung_keymap, 23), "ㅚ") == 0);
    assert(get_jamo_buffer("invalid", cho_keymap, 19) == NULL);
    
    printf("✓ PASS: All utility function tests passed\n");
    tests_passed++;
}

// Main test runner
int main() {
    setlocale(LC_ALL, "ko_KR.UTF-8");
    printf("Starting Korean IME Tests...\n");
    printf("================================\n");
    
    test_basic_composition();
    test_jongseong_composition();
    test_compound_jongseong();
    test_compound_jongseong_with_different_jungseong();
    test_compound_jongseong_edge_cases();
    test_compound_jongseong_with_jungseong();
    test_multiple_compound_jongseong();
    test_compound_jongseong_backspace();
    test_compound_jongseong_invalid_combinations();
    test_compound_jungseong();
    test_multiple_syllables();
    test_individual_jamos();
    test_edge_cases();
    test_space_handling();
    test_backspace_simulation();
    test_complex_combinations();
    test_mappable_character();
    test_utility_functions();
    
    printf("\n================================\n");
    printf("Test Results:\n");
    printf("✓ Passed: %d\n", tests_passed);
    printf("✗ Failed: %d\n", tests_failed);
    printf("Total: %d\n", tests_passed + tests_failed);
    
    if (tests_failed == 0) {
        printf("\n🎉 All tests passed! Korean IME is working correctly.\n");
        return 0;
    } else {
        printf("\n❌ Some tests failed. Please check the Korean IME implementation.\n");
        return 1;
    }
} 