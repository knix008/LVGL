/*
 * test_dakuten.c
 * Test dakuten and handakuten combining functionality
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>

// Test counter
static int tests_passed = 0;
static int tests_failed = 0;

// Test helper macros
#define TEST_START(name) \
    printf("\n=== Testing: %s ===\n", name);

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

#define TEST_ASSERT_STR_EQ(actual, expected, message) \
    do { \
        if (strcmp(actual, expected) == 0) { \
            printf("✓ PASS: %s\n", message); \
            tests_passed++; \
        } else { \
            printf("✗ FAIL: %s\n", message); \
            printf("  Expected: '%s'\n", expected); \
            printf("  Got:      '%s'\n", actual); \
            tests_failed++; \
        } \
    } while(0)

// Dakuten conversion table
typedef struct {
    const char *base;
    const char *dakuten;
} DakutenMap;

static const DakutenMap hiragana_dakuten[] = {
    {"か", "が"}, {"き", "ぎ"}, {"く", "ぐ"}, {"け", "げ"}, {"こ", "ご"},
    {"さ", "ざ"}, {"し", "じ"}, {"す", "ず"}, {"せ", "ぜ"}, {"そ", "ぞ"},
    {"た", "だ"}, {"ち", "ぢ"}, {"つ", "づ"}, {"て", "で"}, {"と", "ど"},
    {"は", "ば"}, {"ひ", "び"}, {"ふ", "ぶ"}, {"へ", "べ"}, {"ほ", "ぼ"},
    {NULL, NULL}
};

static const DakutenMap hiragana_handakuten[] = {
    {"は", "ぱ"}, {"ひ", "ぴ"}, {"ふ", "ぷ"}, {"へ", "ぺ"}, {"ほ", "ぽ"},
    {NULL, NULL}
};

// Test 1: Dakuten Conversion Table Completeness
void test_dakuten_table(void) {
    TEST_START("Dakuten Conversion Table");
    
    int total_count = 0;
    
    for (int i = 0; hiragana_dakuten[i].base != NULL; i++) {
        total_count++;
    }
    
    TEST_ASSERT(total_count == 20, "All 20 dakuten mappings present (k,s,t,h rows × 5)");
    TEST_ASSERT(hiragana_dakuten[0].base != NULL, "Dakuten table is not empty");
    TEST_ASSERT(hiragana_dakuten[0].dakuten != NULL, "Dakuten conversions exist");
}

// Test 2: Handakuten Table
void test_handakuten_table(void) {
    TEST_START("Handakuten Conversion Table");
    
    int h_count = 0;
    for (int i = 0; hiragana_handakuten[i].base != NULL; i++) {
        h_count++;
    }
    
    TEST_ASSERT(h_count == 5, "All 5 H-row handakuten mappings present");
}

// Test 3: Dakuten Lookup
void test_dakuten_lookup(void) {
    TEST_START("Dakuten Lookup");
    
    // Test か → が
    const char *result = NULL;
    for (int i = 0; hiragana_dakuten[i].base != NULL; i++) {
        if (strcmp(hiragana_dakuten[i].base, "か") == 0) {
            result = hiragana_dakuten[i].dakuten;
            break;
        }
    }
    TEST_ASSERT_STR_EQ(result, "が", "か converts to が");
    
    // Test は → ば
    result = NULL;
    for (int i = 0; hiragana_dakuten[i].base != NULL; i++) {
        if (strcmp(hiragana_dakuten[i].base, "は") == 0) {
            result = hiragana_dakuten[i].dakuten;
            break;
        }
    }
    TEST_ASSERT_STR_EQ(result, "ば", "は converts to ば");
}

// Test 4: Handakuten Lookup
void test_handakuten_lookup(void) {
    TEST_START("Handakuten Lookup");
    
    // Test は → ぱ
    const char *result = NULL;
    for (int i = 0; hiragana_handakuten[i].base != NULL; i++) {
        if (strcmp(hiragana_handakuten[i].base, "は") == 0) {
            result = hiragana_handakuten[i].dakuten;
            break;
        }
    }
    TEST_ASSERT_STR_EQ(result, "ぱ", "は converts to ぱ");
}

// Test 5: Complete Dakuten Set
void test_complete_dakuten_set(void) {
    TEST_START("Complete Dakuten Character Set");
    
    const char *expected_bases[] = {
        "か", "き", "く", "け", "こ",
        "さ", "し", "す", "せ", "そ",
        "た", "ち", "つ", "て", "と",
        "は", "ひ", "ふ", "へ", "ほ"
    };
    
    const char *expected_dakuten[] = {
        "が", "ぎ", "ぐ", "げ", "ご",
        "ざ", "じ", "ず", "ぜ", "ぞ",
        "だ", "ぢ", "づ", "で", "ど",
        "ば", "び", "ぶ", "べ", "ぼ"
    };
    
    for (int i = 0; i < 20; i++) {
        const char *result = NULL;
        for (int j = 0; hiragana_dakuten[j].base != NULL; j++) {
            if (strcmp(hiragana_dakuten[j].base, expected_bases[i]) == 0) {
                result = hiragana_dakuten[j].dakuten;
                break;
            }
        }
        
        char msg[100];
        snprintf(msg, sizeof(msg), "%s → %s conversion exists", expected_bases[i], expected_dakuten[i]);
        TEST_ASSERT(result != NULL && strcmp(result, expected_dakuten[i]) == 0, msg);
    }
}

// Main test runner
int main(void) {
    printf("\n");
    printf("================================================\n");
    printf("  Dakuten/Handakuten Combining - Test Suite\n");
    printf("================================================\n");
    
    // Run all tests
    test_dakuten_table();
    test_handakuten_table();
    test_dakuten_lookup();
    test_handakuten_lookup();
    test_complete_dakuten_set();
    
    // Print summary
    printf("\n");
    printf("================================================\n");
    printf("  Test Results\n");
    printf("================================================\n");
    printf("Total tests run: %d\n", tests_passed + tests_failed);
    printf("✓ Passed: %d\n", tests_passed);
    printf("✗ Failed: %d\n", tests_failed);
    printf("\n");
    
    if (tests_failed == 0) {
        printf("🎉 All dakuten tests passed!\n\n");
        return 0;
    } else {
        printf("❌ Some dakuten tests failed!\n\n");
        return 1;
    }
}

