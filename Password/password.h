/**
 * @file password.h
 * @brief Password validation logic (LVGL-independent)
 *
 * This module provides password validation functionality without
 * any dependency on LVGL or GUI components.
 */

#ifndef PASSWORD_H
#define PASSWORD_H

#include <stdbool.h>

// Password validation rules
#define MIN_PASSWORD_LENGTH 11

/**
 * @brief Validation result structure
 */
typedef struct {
    bool is_valid;          /**< Overall validation result */
    int length;             /**< Password length */
    bool has_capital;       /**< Has at least one capital letter */
    bool has_number;        /**< Has at least one number */
    bool has_special;       /**< Has at least one special character */
} password_validation_t;

/**
 * @brief Validates password according to requirements
 *
 * Requirements:
 * - Minimum 11 characters
 * - At least one capital letter (A-Z)
 * - At least one number (0-9)
 * - At least one special character
 *
 * @param password Password string to validate
 * @return true if password meets all requirements, false otherwise
 */
bool password_validate(const char *password);

/**
 * @brief Gets detailed validation information for a password
 *
 * @param password Password string to analyze
 * @param result Pointer to validation result structure to fill
 */
void password_get_validation_info(const char *password, password_validation_t *result);

/**
 * @brief Checks if password meets minimum length requirement
 *
 * @param password Password string to check
 * @return true if password meets minimum length, false otherwise
 */
bool password_check_length(const char *password);

/**
 * @brief Checks if password contains at least one capital letter
 *
 * @param password Password string to check
 * @return true if password has capital letter, false otherwise
 */
bool password_has_capital(const char *password);

/**
 * @brief Checks if password contains at least one number
 *
 * @param password Password string to check
 * @return true if password has number, false otherwise
 */
bool password_has_number(const char *password);

/**
 * @brief Checks if password contains at least one special character
 *
 * @param password Password string to check
 * @return true if password has special character, false otherwise
 */
bool password_has_special(const char *password);

/**
 * @brief Validates User ID format
 *
 * User ID can be:
 * - Alphanumeric with allowed special characters: . - _ @
 * - Email address format (contains @)
 * - Minimum 3 characters
 * - Maximum 64 characters
 *
 * @param userid User ID string to validate
 * @return true if valid, false otherwise
 */
bool userid_validate(const char *userid);

/**
 * @brief Checks if a character is allowed in User ID
 *
 * Allowed characters:
 * - Letters: a-z, A-Z
 * - Numbers: 0-9
 * - Special: . - _ @
 *
 * @param c Character to check
 * @return true if allowed, false otherwise
 */
bool userid_is_valid_char(char c);

/**
 * @brief Checks if User ID is in email format
 *
 * @param userid User ID string to check
 * @return true if it looks like an email, false otherwise
 */
bool userid_is_email_format(const char *userid);

#endif /* PASSWORD_H */
