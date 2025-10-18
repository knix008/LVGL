#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../ip_address.h"

/* Test result tracking */
static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;

/* Test macro */
#define TEST_ASSERT(condition, message) \
    do { \
        tests_run++; \
        if (condition) { \
            tests_passed++; \
            printf("✓ %s\n", message); \
        } else { \
            tests_failed++; \
            printf("✗ %s\n", message); \
        } \
    } while(0)

/* Comprehensive test cases */
void test_ipv4_comprehensive(void) {
    printf("\n=== Comprehensive IPv4 Tests ===\n");
    
    /* Valid cases */
    const char *valid_ipv4[] = {
        "192.168.1.1",
        "10.0.0.1",
        "172.16.0.1",
        "8.8.8.8",
        "127.0.0.1",
        "255.255.255.255",
        "0.0.0.0",
        "1.1.1.1",
        "192.168.0.1",
        "10.10.10.10"
    };
    
    for (int i = 0; i < 10; i++) {
        char test_msg[100];
        snprintf(test_msg, sizeof(test_msg), "Valid IPv4: %s", valid_ipv4[i]);
        TEST_ASSERT(ipv4_validate(valid_ipv4[i]), test_msg);
    }
    
    /* Invalid cases */
    const char *invalid_ipv4[] = {
        "",
        "192.168.1",
        "192.168.1.1.1",
        "192.168.1.",
        ".192.168.1.1",
        "192..168.1.1",
        "256.168.1.1",
        "192.256.1.1",
        "192.168.256.1",
        "192.168.1.256",
        "192.168.1.-1",
        "192.168.1.abc",
        "192.168.1.01",
        "192.168.1.00",
        "192.168.1.000"
    };
    
    for (int i = 0; i < 15; i++) {
        char test_msg[100];
        snprintf(test_msg, sizeof(test_msg), "Invalid IPv4: %s", invalid_ipv4[i]);
        TEST_ASSERT(!ipv4_validate(invalid_ipv4[i]), test_msg);
    }
}

void test_ipv6_comprehensive(void) {
    printf("\n=== Comprehensive IPv6 Tests ===\n");
    
    /* Valid cases */
    const char *valid_ipv6[] = {
        "2001:0db8:85a3:0000:0000:8a2e:0370:7334",
        "2001:db8:85a3:0:0:8a2e:370:7334",
        "2001:db8:85a3::8a2e:370:7334",
        "::1",
        "::",
        "2001:db8::1",
        "::ffff:192.168.1.1",
        "::ffff:c0a8:101",
        "fe80::1",
        "ff02::1",
        "2001:0:0:0:0:0:0:1",
        "2001::1"
    };
    
    for (int i = 0; i < 12; i++) {
        char test_msg[100];
        snprintf(test_msg, sizeof(test_msg), "Valid IPv6: %s", valid_ipv6[i]);
        TEST_ASSERT(ipv6_validate(valid_ipv6[i]), test_msg);
    }
    
    /* Invalid cases */
    const char *invalid_ipv6[] = {
        "",
        "2001:0db8:85a3:0000:0000:8a2e:0370:7334:1234",
        "2001:0db8:85a3:0000:0000:8a2e:0370",
        "2001:0db8:85a3:0000:0000:8a2e:0370:7334::",
        "::2001:0db8:85a3:0000:0000:8a2e:0370:7334",
        "2001::0db8::85a3",
        "2001:0db8:85a3:0000:0000:8a2e:0370:7334:",
        ":2001:0db8:85a3:0000:0000:8a2e:0370:7334",
        "2001:0db8:85a3:0000:0000:8a2e:0370:733g"
    };
    
    for (int i = 0; i < 10; i++) {
        char test_msg[100];
        snprintf(test_msg, sizeof(test_msg), "Invalid IPv6: %s", invalid_ipv6[i]);
        TEST_ASSERT(!ipv6_validate(invalid_ipv6[i]), test_msg);
    }
}

void test_edge_cases(void) {
    printf("\n=== Edge Cases Tests ===\n");
    
    /* IPv4 edge cases */
    TEST_ASSERT(ipv4_validate("0.0.0.0"), "IPv4 all zeros should be valid");
    TEST_ASSERT(ipv4_validate("255.255.255.255"), "IPv4 all 255s should be valid");
    TEST_ASSERT(!ipv4_validate("192.168.1.01"), "IPv4 leading zero should be invalid");
    TEST_ASSERT(!ipv4_validate("192.168.1.00"), "IPv4 leading zeros should be invalid");
    
    /* IPv6 edge cases */
    TEST_ASSERT(ipv6_validate("0:0:0:0:0:0:0:0"), "IPv6 all zeros should be valid");
    TEST_ASSERT(ipv6_validate("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"), "IPv6 all f's should be valid");
    TEST_ASSERT(ipv6_validate("::"), "IPv6 compressed all zeros should be valid");
    TEST_ASSERT(ipv6_validate("::1"), "IPv6 localhost should be valid");
    
    /* Mixed case tests */
    TEST_ASSERT(ipv6_validate("2001:0db8:85a3:0000:0000:8a2e:0370:7334"), "IPv6 lowercase should be valid");
    TEST_ASSERT(ipv6_validate("2001:0DB8:85A3:0000:0000:8A2E:0370:7334"), "IPv6 uppercase should be valid");
    TEST_ASSERT(ipv6_validate("2001:0Db8:85A3:0000:0000:8a2E:0370:7334"), "IPv6 mixed case should be valid");
}

void test_parse_functions(void) {
    printf("\n=== Parse Functions Tests ===\n");
    
    /* IPv4 parse octet */
    int octet;
    TEST_ASSERT(ipv4_parse_octet("0", &octet) && octet == 0, "Parse IPv4 octet '0' should return 0");
    TEST_ASSERT(ipv4_parse_octet("255", &octet) && octet == 255, "Parse IPv4 octet '255' should return 255");
    TEST_ASSERT(ipv4_parse_octet("192", &octet) && octet == 192, "Parse IPv4 octet '192' should return 192");
    TEST_ASSERT(!ipv4_parse_octet("256", &octet), "Parse IPv4 octet '256' should fail");
    TEST_ASSERT(!ipv4_parse_octet("abc", &octet), "Parse IPv4 octet 'abc' should fail");
    
    /* IPv6 parse hex */
    int hex;
    TEST_ASSERT(ipv6_parse_hex("0", &hex) && hex == 0, "Parse IPv6 hex '0' should return 0");
    TEST_ASSERT(ipv6_parse_hex("ffff", &hex) && hex == 0xffff, "Parse IPv6 hex 'ffff' should return 0xffff");
    TEST_ASSERT(ipv6_parse_hex("1234", &hex) && hex == 0x1234, "Parse IPv6 hex '1234' should return 0x1234");
    TEST_ASSERT(ipv6_parse_hex("abcd", &hex) && hex == 0xabcd, "Parse IPv6 hex 'abcd' should return 0xabcd");
    TEST_ASSERT(!ipv6_parse_hex("10000", &hex), "Parse IPv6 hex '10000' should fail");
    TEST_ASSERT(!ipv6_parse_hex("g", &hex), "Parse IPv6 hex 'g' should fail");
}

void test_format_functions(void) {
    printf("\n=== Format Functions Tests ===\n");
    
    char output[50];
    
    /* Test IPv4 formatting */
    const char *ipv4_fields[] = {"192", "168", "1", "1"};
    ip_format_from_fields(ipv4_fields, 4, output, sizeof(output));
    TEST_ASSERT(strcmp(output, "192.168.1.1") == 0, "IPv4 format should be correct");
    
    /* Test IPv6 formatting */
    const char *ipv6_fields[] = {"2001", "0db8", "85a3", "0000", "0000", "8a2e", "0370", "7334"};
    ip_format_from_fields(ipv6_fields, 8, output, sizeof(output));
    TEST_ASSERT(strcmp(output, "2001:0db8:85a3:0000:0000:8a2e:0370:7334") == 0, "IPv6 format should be correct");
}

int main(void) {
    printf("Comprehensive IP Address Validation Tests\n");
    printf("==========================================\n");
    
    test_ipv4_comprehensive();
    test_ipv6_comprehensive();
    test_edge_cases();
    test_parse_functions();
    test_format_functions();
    
    printf("\n=== Final Test Summary ===\n");
    printf("Total tests: %d\n", tests_run);
    printf("Passed: %d\n", tests_passed);
    printf("Failed: %d\n", tests_failed);
    printf("Success rate: %.1f%%\n", (float)tests_passed / tests_run * 100);
    
    if (tests_failed == 0) {
        printf("\n🎉 All tests passed! 🎉\n");
    } else {
        printf("\n❌ Some tests failed! ❌\n");
    }
    
    return (tests_failed == 0) ? 0 : 1;
}
