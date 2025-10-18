/**
 * @file ip_address.c
 * @brief IP Address Validation Logic Implementation
 * @details IPv4 and IPv6 address validation functions
 */

#include "ip_address.h"
#include <string.h>
#include <stdio.h>
#include <ctype.h>

/* IPv4 Validation Functions */

bool ipv4_validate(const char *ip_str) {
    if (ip_str == NULL) return false;
    
    int len = strlen(ip_str);
    if (len < 7 || len > 15) return false; /* Min: 0.0.0.0, Max: 255.255.255.255 */
    
    int octets[4];
    int num_octets = 0;
    int i = 0;
    
    while (i < len && num_octets < 4) {
        int start = i;
        while (i < len && ip_str[i] != '.') {
            if (!isdigit(ip_str[i])) return false;
            i++;
        }
        
        if (i == start) return false; /* Empty octet */
        if (i - start > 3) return false; /* Too many digits */
        
        /* Extract octet substring */
        char octet_str[4];
        int octet_len = i - start;
        strncpy(octet_str, &ip_str[start], octet_len);
        octet_str[octet_len] = '\0';
        
        if (!ipv4_parse_octet(octet_str, &octets[num_octets])) {
            return false;
        }
        
        num_octets++;
        
        /* If we have 4 octets, we should be at the end */
        if (num_octets == 4) {
            if (i != len) return false; /* No trailing characters allowed */
            return true;
        }
        
        if (i < len) {
            if (ip_str[i] != '.') return false;
            i++; /* Skip the dot */
        }
    }
    
    return false; /* Should not reach here if we have exactly 4 octets */
}

bool ipv4_parse_octet(const char *str, int *octet) {
    if (str == NULL || octet == NULL) return false;
    
    int len = strlen(str);
    if (len == 0 || len > 3) return false;
    
    /* Reject leading zeros (except for single zero) */
    if (len > 1 && str[0] == '0') return false;
    
    int value = 0;
    for (int i = 0; i < len; i++) {
        if (!isdigit(str[i])) return false;
        value = value * 10 + (str[i] - '0');
    }
    
    if (value > 255) return false;
    
    *octet = value;
    return true;
}

/* IPv6 Validation Functions */

bool ipv6_validate(const char *ip_str) {
    if (ip_str == NULL) return false;
    
    int len = strlen(ip_str);
    if (len < 2 || len > 39) return false; /* Min: ::, Max: full expanded */
    
    /* Check for compressed notation (::) */
    if (strstr(ip_str, "::") != NULL) {
        return ipv6_validate_compressed(ip_str);
    }
    
    /* Validate full IPv6 format */
    int groups = 0;
    int i = 0;
    
    while (i < len && groups < 8) {
        int start = i;
        while (i < len && ip_str[i] != ':') {
            if (!isxdigit(ip_str[i])) return false;
            i++;
        }
        
        if (i == start) return false; /* Empty group */
        if (i - start > 4) return false; /* Too many hex digits */
        
        groups++;
        
        if (i < len) {
            if (ip_str[i] != ':') return false;
            i++; /* Skip the colon */
        }
    }
    
    /* Must have exactly 8 groups and consume entire string */
    if (groups != 8 || i != len) return false;
    
    return true;
}

bool ipv6_validate_compressed(const char *ip_str) {
    if (ip_str == NULL) return false;
    
    /* Special case: "::" means all zeros */
    if (strcmp(ip_str, "::") == 0) return true;
    
    /* Count occurrences of :: */
    int double_colon_count = 0;
    int len = strlen(ip_str);
    
    for (int i = 0; i < len - 1; i++) {
        if (ip_str[i] == ':' && ip_str[i + 1] == ':') {
            double_colon_count++;
        }
    }
    
    if (double_colon_count != 1) return false; /* Must have exactly one :: */
    
    /* Split by :: */
    char *double_colon = strstr(ip_str, "::");
    if (double_colon == NULL) return false;
    
    /* Validate left part */
    int left_groups = 0;
    if (double_colon > ip_str) {
        char left_part[40];
        size_t left_len = (size_t)(double_colon - ip_str);
        if (left_len >= sizeof(left_part)) return false;
        
        strncpy(left_part, ip_str, left_len);
        left_part[left_len] = '\0';
        
        /* Count groups in left part */
        for (size_t i = 0; i < left_len; i++) {
            if (left_part[i] == ':') left_groups++;
        }
        left_groups++; /* Add one for the last group */
        
        if (left_groups > 7) return false; /* Too many groups before :: */
    }
    
    /* Validate right part */
    int right_groups = 0;
    char *right_part = double_colon + 2;
    if (*right_part != '\0') {
        for (int i = 0; right_part[i] != '\0'; i++) {
            if (right_part[i] == ':') right_groups++;
        }
        right_groups++; /* Add one for the last group */
        
        if (right_groups > 7) return false; /* Too many groups after :: */
    }
    
    /* Total groups must not exceed 8 */
    if (left_groups + right_groups > 8) return false;
    
    return true;
}

bool ipv6_parse_hex(const char *str, int *hex) {
    if (str == NULL || hex == NULL) return false;
    
    int len = strlen(str);
    if (len == 0 || len > 4) return false;
    
    int value = 0;
    for (int i = 0; i < len; i++) {
        char c = tolower(str[i]);
        if (!isxdigit(c)) return false;
        
        if (isdigit(c)) {
            value = value * 16 + (c - '0');
        } else {
            value = value * 16 + (c - 'a' + 10);
        }
    }
    
    if (value > 0xFFFF) return false;
    
    *hex = value;
    return true;
}

void ip_format_from_fields(const char **fields, int num_fields, char *output, size_t max_len) {
    if (fields == NULL || output == NULL || max_len == 0) return;
    
    output[0] = '\0';
    
    if (num_fields == 4) {
        /* IPv4 format: xxx.xxx.xxx.xxx */
        snprintf(output, max_len, "%s.%s.%s.%s",
                 fields[0], fields[1], fields[2], fields[3]);
    } else if (num_fields == 8) {
        /* IPv6 format: xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx */
        snprintf(output, max_len, "%s:%s:%s:%s:%s:%s:%s:%s",
                 fields[0], fields[1], fields[2], fields[3],
                 fields[4], fields[5], fields[6], fields[7]);
    }
}

