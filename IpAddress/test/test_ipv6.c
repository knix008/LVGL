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

/* Test cases for IPv6 validation */
void test_ipv6_valid_addresses(void) {
    printf("\n=== Testing Valid IPv6 Addresses ===\n");
    
    TEST_ASSERT(ipv6_validate("2001:0db8:85a3:0000:0000:8a2e:0370:7334"), "Full IPv6 should be valid");
    TEST_ASSERT(ipv6_validate("2001:db8:85a3:0:0:8a2e:370:7334"), "Compressed zeros should be valid");
    TEST_ASSERT(ipv6_validate("2001:db8:85a3::8a2e:370:7334"), "Double colon compression should be valid");
    TEST_ASSERT(ipv6_validate("::1"), "Localhost IPv6 should be valid");
    TEST_ASSERT(ipv6_validate("::"), "All zeros IPv6 should be valid");
    TEST_ASSERT(ipv6_validate("2001:db8::1"), "Partial compression should be valid");
    TEST_ASSERT(ipv6_validate("::ffff:192.168.1.1"), "IPv4-mapped IPv6 should be valid");
    TEST_ASSERT(ipv6_validate("::ffff:c0a8:101"), "IPv4-mapped IPv6 (hex) should be valid");
    TEST_ASSERT(ipv6_validate("fe80::1"), "Link-local should be valid");
    TEST_ASSERT(ipv6_validate("ff02::1"), "Multicast should be valid");
    TEST_ASSERT(ipv6_validate("2001:0:0:0:0:0:0:1"), "Full zeros should be valid");
    TEST_ASSERT(ipv6_validate("2001::1"), "Single compression should be valid");
}

void test_ipv6_invalid_addresses(void) {
    printf("\n=== Testing Invalid IPv6 Addresses ===\n");
    
    TEST_ASSERT(!ipv6_validate(""), "Empty string should be invalid");
    TEST_ASSERT(!ipv6_validate("2001:0db8:85a3:0000:0000:8a2e:0370:7334:1234"), "Too many groups should be invalid");
    TEST_ASSERT(!ipv6_validate("2001:0db8:85a3:0000:0000:8a2e:0370"), "Too few groups should be invalid");
    TEST_ASSERT(!ipv6_validate("2001:0db8:85a3:0000:0000:8a2e:0370:7334::"), "Double colon at end should be invalid");
    TEST_ASSERT(!ipv6_validate("::2001:0db8:85a3:0000:0000:8a2e:0370:7334"), "Double colon at start should be invalid");
    TEST_ASSERT(!ipv6_validate("2001::0db8::85a3"), "Multiple double colons should be invalid");
    TEST_ASSERT(!ipv6_validate("2001:0db8:85a3:0000:0000:8a2e:0370:7334:"), "Trailing colon should be invalid");
    TEST_ASSERT(!ipv6_validate(":2001:0db8:85a3:0000:0000:8a2e:0370:7334"), "Leading colon should be invalid");
    TEST_ASSERT(!ipv6_validate("2001:0db8:85a3:0000:0000:8a2e:0370:7334 "), "Trailing space should be invalid");
    TEST_ASSERT(!ipv6_validate(" 2001:0db8:85a3:0000:0000:8a2e:0370:7334"), "Leading space should be invalid");
    TEST_ASSERT(!ipv6_validate("2001:0db8:85a3:0000:0000:8a2e:0370:733g"), "Invalid hex character should be invalid");
    TEST_ASSERT(!ipv6_validate("2001:0db8:85a3:0000:0000:8a2e:0370:73345"), "Too many hex digits should be invalid");
}

void test_ipv6_compressed_notation(void) {
    printf("\n=== Testing IPv6 Compressed Notation ===\n");
    
    TEST_ASSERT(ipv6_validate("::"), "All zeros compressed should be valid");
    TEST_ASSERT(ipv6_validate("::1"), "Localhost compressed should be valid");
    TEST_ASSERT(ipv6_validate("1::"), "Leading zeros compressed should be valid");
    TEST_ASSERT(ipv6_validate("2001:db8::1"), "Middle zeros compressed should be valid");
    TEST_ASSERT(ipv6_validate("2001:db8:85a3::8a2e:370:7334"), "Multiple zeros compressed should be valid");
    TEST_ASSERT(ipv6_validate("::ffff:192.168.1.1"), "IPv4-mapped compressed should be valid");
    TEST_ASSERT(ipv6_validate("2001:db8::"), "Trailing zeros compressed should be valid");
    TEST_ASSERT(ipv6_validate("::2001:db8"), "Leading zeros compressed should be valid");
}

void test_ipv6_edge_cases(void) {
    printf("\n=== Testing IPv6 Edge Cases ===\n");
    
    TEST_ASSERT(ipv6_validate("0:0:0:0:0:0:0:0"), "All zeros should be valid");
    TEST_ASSERT(ipv6_validate("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff"), "All f's should be valid");
    TEST_ASSERT(ipv6_validate("1:2:3:4:5:6:7:8"), "Sequential numbers should be valid");
    TEST_ASSERT(ipv6_validate("a:b:c:d:e:f:1:2"), "Mixed case should be valid");
    TEST_ASSERT(ipv6_validate("A:B:C:D:E:F:1:2"), "Uppercase should be valid");
    TEST_ASSERT(ipv6_validate("2001:0db8:85a3:0000:0000:8a2e:0370:7334"), "Lowercase should be valid");
    TEST_ASSERT(ipv6_validate("2001:0DB8:85A3:0000:0000:8A2E:0370:7334"), "Uppercase should be valid");
}

void test_ipv6_parse_hex(void) {
    printf("\n=== Testing IPv6 Hex Parsing ===\n");
    
    int hex;
    
    TEST_ASSERT(ipv6_parse_hex("0", &hex) && hex == 0, "Parse '0' should return 0");
    TEST_ASSERT(ipv6_parse_hex("ffff", &hex) && hex == 0xffff, "Parse 'ffff' should return 0xffff");
    TEST_ASSERT(ipv6_parse_hex("1234", &hex) && hex == 0x1234, "Parse '1234' should return 0x1234");
    TEST_ASSERT(ipv6_parse_hex("abcd", &hex) && hex == 0xabcd, "Parse 'abcd' should return 0xabcd");
    TEST_ASSERT(ipv6_parse_hex("ABCD", &hex) && hex == 0xabcd, "Parse 'ABCD' should return 0xabcd");
    TEST_ASSERT(ipv6_parse_hex("a", &hex) && hex == 0xa, "Parse 'a' should return 0xa");
    TEST_ASSERT(ipv6_parse_hex("F", &hex) && hex == 0xf, "Parse 'F' should return 0xf");
    
    TEST_ASSERT(!ipv6_parse_hex("10000", &hex), "Parse '10000' should fail (too many digits)");
    TEST_ASSERT(!ipv6_parse_hex("", &hex), "Parse empty string should fail");
    TEST_ASSERT(!ipv6_parse_hex("g", &hex), "Parse 'g' should fail (invalid hex)");
    TEST_ASSERT(!ipv6_parse_hex("12g", &hex), "Parse '12g' should fail (invalid hex)");
    TEST_ASSERT(!ipv6_parse_hex("-1", &hex), "Parse '-1' should fail (negative)");
}

int main(void) {
    printf("IPv6 Address Validation Tests\n");
    printf("=============================\n");
    
    test_ipv6_valid_addresses();
    test_ipv6_invalid_addresses();
    test_ipv6_compressed_notation();
    test_ipv6_edge_cases();
    test_ipv6_parse_hex();
    
    printf("\n=== Test Summary ===\n");
    printf("Total tests: %d\n", tests_run);
    printf("Passed: %d\n", tests_passed);
    printf("Failed: %d\n", tests_failed);
    printf("Success rate: %.1f%%\n", (float)tests_passed / tests_run * 100);
    
    return (tests_failed == 0) ? 0 : 1;
}
