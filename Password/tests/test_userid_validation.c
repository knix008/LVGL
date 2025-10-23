/**
 * @file test_userid_validation.c
 * @brief Unit tests for User ID validation functions
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
 * Test: Valid simple username
 */
int test_valid_username() {
    TEST("Valid simple username");
    
    const char *userid = "john_doe";
    int result = userid_validate(userid);
    
    ASSERT(result == 1, "Username should be valid");
}

/**
 * Test: Valid email format
 */
int test_valid_email() {
    TEST("Valid email format");
    
    const char *userid = "user@example.com";
    int result = userid_validate(userid);
    
    ASSERT(result == 1, "Email should be valid");
}

/**
 * Test: Email format detection
 */
int test_email_format_detection() {
    TEST("Email format detection");
    
    const char *email = "test@example.com";
    const char *username = "testuser";
    
    ASSERT(userid_is_email_format(email) == 1, "Should detect email");
    ASSERT(userid_is_email_format(username) == 0, "Should not detect username as email");
}

/**
 * Test: User ID too short
 */
int test_userid_too_short() {
    TEST("User ID too short");
    
    const char *userid = "ab";
    int result = userid_validate(userid);
    
    ASSERT(result == 0, "User ID should be invalid (too short)");
}

/**
 * Test: User ID too long
 */
int test_userid_too_long() {
    TEST("User ID too long");
    
    char userid[100];
    memset(userid, 'a', 65);
    userid[65] = '\0';
    
    int result = userid_validate(userid);
    
    ASSERT(result == 0, "User ID should be invalid (too long)");
}

/**
 * Test: User ID with invalid characters
 */
int test_userid_invalid_chars() {
    TEST("User ID with invalid characters");
    
    const char *userid = "user#name";
    int result = userid_validate(userid);
    
    ASSERT(result == 0, "User ID should be invalid (invalid chars)");
}

/**
 * Test: User ID starting with number (valid)
 */
int test_userid_starts_with_number() {
    TEST("User ID starting with number");
    
    const char *userid = "123user";
    int result = userid_validate(userid);
    
    ASSERT(result == 1, "User ID starting with number should be valid");
}

/**
 * Test: User ID starting with special char (invalid)
 */
int test_userid_starts_with_special() {
    TEST("User ID starting with special character");
    
    const char *userid = "_username";
    int result = userid_validate(userid);
    
    ASSERT(result == 0, "User ID starting with underscore should be invalid");
}

/**
 * Test: Empty User ID
 */
int test_empty_userid() {
    TEST("Empty User ID");
    
    const char *userid = "";
    int result = userid_validate(userid);
    
    ASSERT(result == 0, "Empty User ID should be invalid");
}

/**
 * Test: NULL User ID
 */
int test_null_userid() {
    TEST("NULL User ID");
    
    const char *userid = NULL;
    int result = userid_validate(userid);
    
    ASSERT(result == 0, "NULL User ID should be invalid");
}

/**
 * Test: User ID with dots and dashes
 */
int test_userid_with_dots_dashes() {
    TEST("User ID with dots and dashes");
    
    const char *userid = "john.doe-123";
    int result = userid_validate(userid);
    
    ASSERT(result == 1, "User ID with dots and dashes should be valid");
}

/**
 * Test: Invalid email format
 */
int test_invalid_email_format() {
    TEST("Invalid email format");
    
    const char *userid = "user@";
    int result = userid_validate(userid);
    
    ASSERT(result == 0, "Invalid email format should be invalid");
}

/**
 * Test: Maximum length valid User ID (32 chars)
 */
int test_maximum_length_userid() {
    TEST("Maximum length User ID");
    
    const char *userid = "user1234567890123456789012345678"; // 32 chars
    int result = userid_validate(userid);
    
    ASSERT(result == 1, "32-character User ID should be valid");
}

/**
 * Main test runner
 */
int main() {
    printf("\n=================================\n");
    printf("USER ID VALIDATION TESTS\n");
    printf("=================================\n\n");
    
    RUN_TEST(test_valid_username);
    RUN_TEST(test_valid_email);
    RUN_TEST(test_email_format_detection);
    RUN_TEST(test_userid_too_short);
    RUN_TEST(test_userid_too_long);
    RUN_TEST(test_userid_invalid_chars);
    RUN_TEST(test_userid_starts_with_number);
    RUN_TEST(test_userid_starts_with_special);
    RUN_TEST(test_empty_userid);
    RUN_TEST(test_null_userid);
    RUN_TEST(test_userid_with_dots_dashes);
    RUN_TEST(test_invalid_email_format);
    RUN_TEST(test_maximum_length_userid);
    
    printf("=================================\n");
    printf("RESULTS: %d/%d tests passed\n", tests_passed, tests_run);
    printf("=================================\n\n");
    
    return (tests_passed == tests_run) ? 0 : 1;
}

