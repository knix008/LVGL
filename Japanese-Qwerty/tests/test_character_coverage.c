/*
 * test_character_coverage.c
 * Verify all Japanese characters from 50-sound chart are accessible
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Test counter
static int tests_passed = 0;
static int tests_failed = 0;

#define TEST_ASSERT(condition, message) \
    do { \
        if (condition) { \
            printf("✓ PASS: %s\n", message); \
            tests_passed++; \
        } else { \
            printf("✗ FAIL: %s\n", message); \
            tests_failed++; \
        } \
    } while(0)

// Define all Japanese character arrays from japanese_gui.c (Custom layout)
static const char *hiragana_hints[] = {
    // Row 0: ` 1 2 3 4 5 6 7 8 9 0 - = (13 keys)
    "ろ", "ぬ", "ふ", "あ", "う", "え", "お", "や", "ゆ", "よ", "わ", "ほ", "へ",
    // Row 1: Q W E R T Y U I O P [ ] (12 keys)
    "た", "て", "い", "す", "か", "ん", "な", "に", "ら", "se", "を", "む",
    // Row 2: A S D F G H J K L ; ' (11 keys)
    "ち", "と", "し", "は", "き", "く", "ま", "の", "り", "れ", "け",
    // Row 3: Z X C V B N M , . / (10 keys)
    "つ", "さ", "そ", "ひ", "こ", "み", "も", "ね", "る", "め"
};

static const char *hiragana_small[] = {
    // Row 0: ` 1 2 3 4 5 6 7 8 9 0 - = (13 keys with small variants)
    "ろ", "ぬ", "ふ", "ぁ", "ぅ", "ぇ", "ぉ", "ゃ", "ゅ", "ょ", "ゎ", "ほ", "へ",
    // Row 1: Q W E R T Y U I O P [ ] (12 keys with small variants)
    "た", "て", "ぃ", "す", "か", "ん", "な", "に", "ら", "ぜ", "を", "む",
    // Row 2: A S D F G H J K L ; ' (11 keys)
    "ち", "と", "し", "は", "き", "く", "ま", "の", "り", "れ", "け",
    // Row 3: Z X C V B N M , . / (10 keys with small tsu)
    "っ", "さ", "そ", "ひ", "こ", "み", "も", "ね", "る", "め"
};

static const char *katakana_hints[] = {
    // Row 0: ` 1 2 3 4 5 6 7 8 9 0 - = (13 keys)
    "ロ", "ヌ", "フ", "ア", "ウ", "エ", "オ", "ヤ", "ユ", "ヨ", "ワ", "ホ", "ヘ",
    // Row 1: Q W E R T Y U I O P [ ] (12 keys)
    "タ", "テ", "イ", "ス", "カ", "ン", "ナ", "ニ", "ラ", "ゼ", "ヲ", "ム",
    // Row 2: A S D F G H J K L ; ' (11 keys)
    "チ", "ト", "シ", "ハ", "キ", "ク", "マ", "ノ", "リ", "レ", "ケ",
    // Row 3: Z X C V B N M , . / (10 keys)
    "ツ", "サ", "ソ", "ヒ", "コ", "ミ", "モ", "ネ", "ル", "メ"
};

static const char *katakana_small[] = {
    // Row 0: ` 1 2 3 4 5 6 7 8 9 0 - = (13 keys with small variants)
    "ロ", "ヌ", "フ", "ァ", "ゥ", "ェ", "ォ", "ャ", "ュ", "ョ", "ヮ", "ホ", "ヘ",
    // Row 1: Q W E R T Y U I O P [ ] (12 keys with small variants)
    "タ", "テ", "ィ", "ス", "カ", "ン", "ナ", "ニ", "ラ", "ゼ", "ヲ", "ム",
    // Row 2: A S D F G H J K L ; ' (11 keys)
    "チ", "ト", "シ", "ハ", "キ", "ク", "マ", "ノ", "リ", "レ", "ケ",
    // Row 3: Z X C V B N M , . / (10 keys with small tsu)
    "ッ", "サ", "ソ", "ヒ", "コ", "ミ", "モ", "ネ", "ル", "メ"
};

// Helper function to check if a character exists in array
bool char_exists_in_array(const char **array, int size, const char *target) {
    for (int i = 0; i < size; i++) {
        if (strcmp(array[i], target) == 0) {
            return true;
        }
    }
    return false;
}

// Test all あ行 (A-row) characters
void test_a_row_hiragana() {
    printf("\n=== Testing あ行 (A-row) Hiragana ===\n");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "あ"), "あ (a) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "い"), "い (i) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "う"), "う (u) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "え"), "え (e) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "お"), "お (o) exists");
}

// Test all か行 (K-row) characters
void test_ka_row_hiragana() {
    printf("\n=== Testing か行 (K-row) Hiragana ===\n");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "か"), "か (ka) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "き"), "き (ki) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "く"), "く (ku) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "け"), "け (ke) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "こ"), "こ (ko) exists");
}

// Test all さ行 (S-row) characters
void test_sa_row_hiragana() {
    printf("\n=== Testing さ行 (S-row) Hiragana ===\n");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "さ"), "さ (sa) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "し"), "し (shi) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "す"), "す (su) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "せ"), "せ (se) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "そ"), "そ (so) exists");
}

// Test all た行 (T-row) characters
void test_ta_row_hiragana() {
    printf("\n=== Testing た行 (T-row) Hiragana ===\n");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "た"), "た (ta) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "ち"), "ち (chi) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "つ"), "つ (tsu) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "て"), "て (te) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "と"), "と (to) exists");
}

// Test all な行 (N-row) characters
void test_na_row_hiragana() {
    printf("\n=== Testing な行 (N-row) Hiragana ===\n");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "な"), "な (na) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "に"), "に (ni) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "ぬ"), "ぬ (nu) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "ね"), "ね (ne) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "の"), "の (no) exists");
}

// Test all は行 (H-row) characters
void test_ha_row_hiragana() {
    printf("\n=== Testing は行 (H-row) Hiragana ===\n");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "は"), "は (ha) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "ひ"), "ひ (hi) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "ふ"), "ふ (fu) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "へ"), "へ (he) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "ほ"), "ほ (ho) exists");
}

// Test all ま行 (M-row) characters
void test_ma_row_hiragana() {
    printf("\n=== Testing ま行 (M-row) Hiragana ===\n");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "ま"), "ま (ma) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "み"), "み (mi) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "む"), "む (mu) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "め"), "め (me) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "も"), "も (mo) exists");
}

// Test all や行 (Y-row) characters
void test_ya_row_hiragana() {
    printf("\n=== Testing や行 (Y-row) Hiragana ===\n");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "や"), "や (ya) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "ゆ"), "ゆ (yu) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "よ"), "よ (yo) exists");
}

// Test all ら行 (R-row) characters
void test_ra_row_hiragana() {
    printf("\n=== Testing ら行 (R-row) Hiragana ===\n");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "ら"), "ら (ra) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "り"), "り (ri) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "る"), "る (ru) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "れ"), "れ (re) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_small, 46, "ろ") ||
                char_exists_in_array(hiragana_hints, 46, "ろ"), "ろ (ro) exists");
}

// Test all わ行 (W-row) characters
void test_wa_row_hiragana() {
    printf("\n=== Testing わ行 (W-row) Hiragana ===\n");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "わ"), "わ (wa) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_small, 46, "を") ||
                char_exists_in_array(hiragana_hints, 46, "を"), "を (wo) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_hints, 46, "ん"), "ん (n) exists");
}

// Test all Katakana characters
void test_katakana_coverage() {
    printf("\n=== Testing Katakana Coverage ===\n");
    TEST_ASSERT(char_exists_in_array(katakana_hints, 46, "ア"), "ア (a) exists");
    TEST_ASSERT(char_exists_in_array(katakana_hints, 46, "カ"), "カ (ka) exists");
    TEST_ASSERT(char_exists_in_array(katakana_hints, 46, "サ"), "サ (sa) exists");
    TEST_ASSERT(char_exists_in_array(katakana_hints, 46, "タ"), "タ (ta) exists");
    TEST_ASSERT(char_exists_in_array(katakana_hints, 46, "チ"), "チ (chi) exists");
    TEST_ASSERT(char_exists_in_array(katakana_hints, 46, "ツ"), "ツ (tsu) exists");
    TEST_ASSERT(char_exists_in_array(katakana_hints, 46, "ナ"), "ナ (na) exists");
    TEST_ASSERT(char_exists_in_array(katakana_hints, 46, "ハ"), "ハ (ha) exists");
    TEST_ASSERT(char_exists_in_array(katakana_hints, 46, "マ"), "マ (ma) exists");
    TEST_ASSERT(char_exists_in_array(katakana_hints, 46, "ミ"), "ミ (mi) exists");
    TEST_ASSERT(char_exists_in_array(katakana_hints, 46, "ム"), "ム (mu) exists");
    TEST_ASSERT(char_exists_in_array(katakana_hints, 46, "ヤ"), "ヤ (ya) exists");
    TEST_ASSERT(char_exists_in_array(katakana_hints, 46, "ラ"), "ラ (ra) exists");
    TEST_ASSERT(char_exists_in_array(katakana_hints, 46, "ワ"), "ワ (wa) exists");
}

// Test small characters accessibility
void test_small_characters() {
    printf("\n=== Testing Small Characters ===\n");
    TEST_ASSERT(char_exists_in_array(hiragana_small, 46, "ぁ"), "ぁ (small a) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_small, 46, "ぃ"), "ぃ (small i) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_small, 46, "ぅ"), "ぅ (small u) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_small, 46, "ぇ"), "ぇ (small e) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_small, 46, "ぉ"), "ぉ (small o) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_small, 46, "ゃ"), "ゃ (small ya) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_small, 46, "ゅ"), "ゅ (small yu) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_small, 46, "ょ"), "ょ (small yo) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_small, 46, "っ"), "っ (small tsu) exists");
    TEST_ASSERT(char_exists_in_array(hiragana_small, 46, "ゎ"), "ゎ (small wa) exists");
}

// Main test runner
int main(void) {
    printf("\n");
    printf("================================================\n");
    printf("  Character Coverage Test Suite\n");
    printf("================================================\n");
    
    test_a_row_hiragana();
    test_ka_row_hiragana();
    test_sa_row_hiragana();
    test_ta_row_hiragana();
    test_na_row_hiragana();
    test_ha_row_hiragana();
    test_ma_row_hiragana();
    test_ya_row_hiragana();
    test_ra_row_hiragana();
    test_wa_row_hiragana();
    test_katakana_coverage();
    test_small_characters();
    
    printf("\n");
    printf("================================================\n");
    printf("  Test Results\n");
    printf("================================================\n");
    printf("Total tests run: %d\n", tests_passed + tests_failed);
    printf("✓ Passed: %d\n", tests_passed);
    printf("✗ Failed: %d\n", tests_failed);
    printf("\n");
    
    if (tests_failed == 0) {
        printf("🎉 All character coverage tests passed!\n\n");
        return 0;
    } else {
        printf("❌ Some tests failed!\n\n");
        return 1;
    }
}

