/**
 * @file ip_address.h
 * @brief IP Address Validation Logic
 * @details IPv4 and IPv6 address validation functions
 */

#ifndef IP_ADDRESS_H
#define IP_ADDRESS_H

#include <stdbool.h>
#include <stddef.h>

/* Maximum string lengths */
#define MAX_IPV4_LEN 15  /* 255.255.255.255 */
#define MAX_IPV6_LEN 39  /* 2001:0db8:85a3:0000:0000:8a2e:0370:7334 */

/**
 * @brief Validate IPv4 address
 * @param ip_str IP address string to validate
 * @return true if valid, false otherwise
 */
bool ipv4_validate(const char *ip_str);

/**
 * @brief Parse IPv4 octet
 * @param str String to parse
 * @param octet Output octet value
 * @return true if valid, false otherwise
 */
bool ipv4_parse_octet(const char *str, int *octet);

/**
 * @brief Validate IPv6 address
 * @param ip_str IP address string to validate
 * @return true if valid, false otherwise
 */
bool ipv6_validate(const char *ip_str);

/**
 * @brief Validate compressed IPv6 address (with ::)
 * @param ip_str IP address string to validate
 * @return true if valid, false otherwise
 */
bool ipv6_validate_compressed(const char *ip_str);

/**
 * @brief Parse hexadecimal value
 * @param str String to parse
 * @param hex Output hex value
 * @return true if valid, false otherwise
 */
bool ipv6_parse_hex(const char *str, int *hex);

/**
 * @brief Get IP address from text areas
 * @param fields Array of field values
 * @param num_fields Number of fields (4 for IPv4, 8 for IPv6)
 * @param output Output buffer for formatted IP address
 * @param max_len Maximum length of output buffer
 */
void ip_format_from_fields(const char **fields, int num_fields, char *output, size_t max_len);

#endif /* IP_ADDRESS_H */

