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

/* Test cases for IPv4 validation */
void test_ipv4_valid_addresses(void) {
    printf("\n=== Testing Valid IPv4 Addresses ===\n");
    
    TEST_ASSERT(ipv4_validate("192.168.1.1"), "192.168.1.1 should be valid");
    TEST_ASSERT(ipv4_validate("10.0.0.1"), "10.0.0.1 should be valid");
    TEST_ASSERT(ipv4_validate("172.16.0.1"), "172.16.0.1 should be valid");
    TEST_ASSERT(ipv4_validate("8.8.8.8"), "8.8.8.8 should be valid");
    TEST_ASSERT(ipv4_validate("127.0.0.1"), "127.0.0.1 should be valid");
    TEST_ASSERT(ipv4_validate("255.255.255.255"), "255.255.255.255 should be valid");
    TEST_ASSERT(ipv4_validate("0.0.0.0"), "0.0.0.0 should be valid");
    TEST_ASSERT(ipv4_validate("1.1.1.1"), "1.1.1.1 should be valid");
}

void test_ipv4_invalid_addresses(void) {
    printf("\n=== Testing Invalid IPv4 Addresses ===\n");
    
    TEST_ASSERT(!ipv4_validate(""), "Empty string should be invalid");
    TEST_ASSERT(!ipv4_validate("192.168.1"), "Missing octet should be invalid");
    TEST_ASSERT(!ipv4_validate("192.168.1.1.1"), "Too many octets should be invalid");
    TEST_ASSERT(!ipv4_validate("192.168.1."), "Trailing dot should be invalid");
    TEST_ASSERT(!ipv4_validate(".192.168.1.1"), "Leading dot should be invalid");
    TEST_ASSERT(!ipv4_validate("192..168.1.1"), "Double dots should be invalid");
    TEST_ASSERT(!ipv4_validate("192.168.1.1."), "Trailing dot after valid IP should be invalid");
    TEST_ASSERT(!ipv4_validate("256.168.1.1"), "Octet > 255 should be invalid");
    TEST_ASSERT(!ipv4_validate("192.256.1.1"), "Octet > 255 should be invalid");
    TEST_ASSERT(!ipv4_validate("192.168.256.1"), "Octet > 255 should be invalid");
    TEST_ASSERT(!ipv4_validate("192.168.1.256"), "Octet > 255 should be invalid");
    TEST_ASSERT(!ipv4_validate("192.168.1.-1"), "Negative octet should be invalid");
    TEST_ASSERT(!ipv4_validate("192.168.1.abc"), "Non-numeric octet should be invalid");
    TEST_ASSERT(!ipv4_validate("192.168.1.1.1"), "Too many octets should be invalid");
    TEST_ASSERT(!ipv4_validate("192.168.1"), "Too few octets should be invalid");
    TEST_ASSERT(!ipv4_validate("192.168.1.1 "), "Trailing space should be invalid");
    TEST_ASSERT(!ipv4_validate(" 192.168.1.1"), "Leading space should be invalid");
}

void test_ipv4_edge_cases(void) {
    printf("\n=== Testing IPv4 Edge Cases ===\n");
    
    TEST_ASSERT(ipv4_validate("0.0.0.0"), "0.0.0.0 should be valid");
    TEST_ASSERT(ipv4_validate("255.255.255.255"), "255.255.255.255 should be valid");
    TEST_ASSERT(ipv4_validate("1.2.3.4"), "1.2.3.4 should be valid");
    TEST_ASSERT(ipv4_validate("10.10.10.10"), "10.10.10.10 should be valid");
    TEST_ASSERT(!ipv4_validate("192.168.1.01"), "Leading zero should be invalid");
    TEST_ASSERT(!ipv4_validate("192.168.1.00"), "Leading zeros should be invalid");
    TEST_ASSERT(!ipv4_validate("192.168.1.000"), "Multiple leading zeros should be invalid");
}

void test_ipv4_parse_octet(void) {
    printf("\n=== Testing IPv4 Octet Parsing ===\n");
    
    int octet;
    
    TEST_ASSERT(ipv4_parse_octet("0", &octet) && octet == 0, "Parse '0' should return 0");
    TEST_ASSERT(ipv4_parse_octet("255", &octet) && octet == 255, "Parse '255' should return 255");
    TEST_ASSERT(ipv4_parse_octet("192", &octet) && octet == 192, "Parse '192' should return 192");
    TEST_ASSERT(ipv4_parse_octet("1", &octet) && octet == 1, "Parse '1' should return 1");
    
    TEST_ASSERT(!ipv4_parse_octet("256", &octet), "Parse '256' should fail");
    TEST_ASSERT(!ipv4_parse_octet("-1", &octet), "Parse '-1' should fail");
    TEST_ASSERT(!ipv4_parse_octet("abc", &octet), "Parse 'abc' should fail");
    TEST_ASSERT(!ipv4_parse_octet("", &octet), "Parse empty string should fail");
    TEST_ASSERT(!ipv4_parse_octet("12a", &octet), "Parse '12a' should fail");
}

int main(void) {
    printf("IPv4 Address Validation Tests\n");
    printf("=============================\n");
    
    test_ipv4_valid_addresses();
    test_ipv4_invalid_addresses();
    test_ipv4_edge_cases();
    test_ipv4_parse_octet();
    
    printf("\n=== Test Summary ===\n");
    printf("Total tests: %d\n", tests_run);
    printf("Passed: %d\n", tests_passed);
    printf("Failed: %d\n", tests_failed);
    printf("Success rate: %.1f%%\n", (float)tests_passed / tests_run * 100);
    
    return (tests_failed == 0) ? 0 : 1;
}
