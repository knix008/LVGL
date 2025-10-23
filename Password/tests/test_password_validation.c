/**
 * @file test_password_validation.c
 * @brief Unit tests for password validation functions
 */

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../password.h"

// Test counter
static int tests_run = 0;
static int tests_passed = 0;

#define TEST(name) \
    printf("Running test: %s\n", name); \
    tests_run++;

#define ASSERT(condition, message) \
    if (!(condition)) { \
        printf("  ❌ FAILED: %s\n", message); \
        return 0; \
    } \
    printf("  ✓ %s\n", message); \
    return 1;

#define RUN_TEST(test) \
    if (test()) { \
        tests_passed++; \
        printf("  ✅ PASSED\n\n"); \
    } else { \
        printf("  ❌ FAILED\n\n"); \
    }

/**
 * Test: Valid password with all requirements
 */
int test_valid_password_all_requirements() {
    TEST("Valid password with all requirements");
    
    const char *password = "MyPassword123!";
    int result = password_validate(password);
    
    ASSERT(result == 1, "Password should be valid");
}

/**
 * Test: Password too short
 */
int test_password_too_short() {
    TEST("Password too short");
    
    const char *password = "Pass1!";
    int result = password_validate(password);
    
    ASSERT(result == 0, "Password should be invalid (too short)");
}

/**
 * Test: Password without capital letter
 */
int test_password_no_capital() {
    TEST("Password without capital letter");
    
    const char *password = "mypassword123!";
    int result = password_validate(password);
    
    ASSERT(result == 0, "Password should be invalid (no capital)");
}

/**
 * Test: Password without number
 */
int test_password_no_number() {
    TEST("Password without number");
    
    const char *password = "MyPassword!!!";
    int result = password_validate(password);
    
    ASSERT(result == 0, "Password should be invalid (no number)");
}

/**
 * Test: Password without special character
 */
int test_password_no_special() {
    TEST("Password without special character");
    
    const char *password = "MyPassword123";
    int result = password_validate(password);
    
    ASSERT(result == 0, "Password should be invalid (no special char)");
}

/**
 * Test: Minimum valid password
 */
int test_minimum_valid_password() {
    TEST("Minimum valid password");
    
    const char *password = "MyPassword1!"; // Exactly 12 chars
    int result = password_validate(password);
    
    ASSERT(result == 1, "Minimum length password should be valid");
}

/**
 * Test: Empty password
 */
int test_empty_password() {
    TEST("Empty password");
    
    const char *password = "";
    int result = password_validate(password);
    
    ASSERT(result == 0, "Empty password should be invalid");
}

/**
 * Test: NULL password
 */
int test_null_password() {
    TEST("NULL password");
    
    const char *password = NULL;
    int result = password_validate(password);
    
    ASSERT(result == 0, "NULL password should be invalid");
}

/**
 * Test: Password exactly at maximum length (32 chars)
 */
int test_maximum_length_password() {
    TEST("Password at maximum length");
    
    const char *password = "MyLongPassword123!0123456789ab"; // 32 chars
    int result = password_validate(password);
    
    ASSERT(result == 1, "32-character password should be valid");
}

/**
 * Test: Password validation info
 */
int test_password_validation_info() {
    TEST("Password validation info");
    
    const char *password = "Test123!";
    password_validation_t info;
    
    password_get_validation_info(password, &info);
    
    ASSERT(info.length == 8, "Length should be 8");
    ASSERT(info.has_capital == 1, "Should have capital");
    ASSERT(info.has_number == 1, "Should have number");
    ASSERT(info.has_special == 1, "Should have special char");
    ASSERT(info.is_valid == 0, "Should be invalid (too short)");
}

/**
 * Main test runner
 */
int main() {
    printf("\n=================================\n");
    printf("PASSWORD VALIDATION TESTS\n");
    printf("=================================\n\n");
    
    RUN_TEST(test_valid_password_all_requirements);
    RUN_TEST(test_password_too_short);
    RUN_TEST(test_password_no_capital);
    RUN_TEST(test_password_no_number);
    RUN_TEST(test_password_no_special);
    RUN_TEST(test_minimum_valid_password);
    RUN_TEST(test_empty_password);
    RUN_TEST(test_null_password);
    RUN_TEST(test_maximum_length_password);
    RUN_TEST(test_password_validation_info);
    
    printf("=================================\n");
    printf("RESULTS: %d/%d tests passed\n", tests_passed, tests_run);
    printf("=================================\n\n");
    
    return (tests_passed == tests_run) ? 0 : 1;
}

