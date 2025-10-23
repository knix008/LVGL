/**
 * @file password.c
 * @brief Password validation logic implementation (LVGL-independent)
 *
 * This module implements password validation functionality without
 * any dependency on LVGL or GUI components.
 */

#include "password.h"
#include <string.h>
#include <ctype.h>

/**
 * @brief Validates password according to requirements
 */
bool password_validate(const char *password) {
    password_validation_t result;
    password_get_validation_info(password, &result);
    return result.is_valid;
}

/**
 * @brief Gets detailed validation information for a password
 */
void password_get_validation_info(const char *password, password_validation_t *result) {
    if (!result) {
        return;
    }

    // Initialize result structure
    result->is_valid = false;
    result->length = 0;
    result->has_capital = false;
    result->has_number = false;
    result->has_special = false;

    // Handle NULL or empty password
    if (!password) {
        return;
    }

    result->length = strlen(password);

    // Check each character
    for (int i = 0; i < result->length; i++) {
        if (isupper(password[i])) {
            result->has_capital = true;
        } else if (isdigit(password[i])) {
            result->has_number = true;
        } else if (!isalnum(password[i])) {
            result->has_special = true;
        }
    }

    // Check if all requirements are met
    result->is_valid = (result->length >= MIN_PASSWORD_LENGTH) &&
                       result->has_capital &&
                       result->has_number &&
                       result->has_special;
}

/**
 * @brief Checks if password meets minimum length requirement
 */
bool password_check_length(const char *password) {
    if (!password) {
        return false;
    }
    return strlen(password) >= MIN_PASSWORD_LENGTH;
}

/**
 * @brief Checks if password contains at least one capital letter
 */
bool password_has_capital(const char *password) {
    if (!password) {
        return false;
    }

    int len = strlen(password);
    for (int i = 0; i < len; i++) {
        if (isupper(password[i])) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Checks if password contains at least one number
 */
bool password_has_number(const char *password) {
    if (!password) {
        return false;
    }

    int len = strlen(password);
    for (int i = 0; i < len; i++) {
        if (isdigit(password[i])) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Checks if password contains at least one special character
 */
bool password_has_special(const char *password) {
    if (!password) {
        return false;
    }

    int len = strlen(password);
    for (int i = 0; i < len; i++) {
        if (!isalnum(password[i])) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Checks if a character is allowed in User ID
 */
bool userid_is_valid_char(char c) {
    // Allow alphanumeric characters
    if (isalnum(c)) {
        return true;
    }

    // Allow specific special characters: . - _ @
    if (c == '.' || c == '-' || c == '_' || c == '@') {
        return true;
    }

    return false;
}

/**
 * @brief Checks if User ID is in email format
 */
bool userid_is_email_format(const char *userid) {
    if (!userid) {
        return false;
    }

    // Simple check: contains @ and has characters before and after it
    const char *at_sign = strchr(userid, '@');
    if (!at_sign) {
        return false;
    }

    // Check there's at least one character before @
    if (at_sign == userid) {
        return false;
    }

    // Check there's at least one character after @
    if (*(at_sign + 1) == '\0') {
        return false;
    }

    // Check for dot after @ (basic domain validation)
    const char *dot = strchr(at_sign + 1, '.');
    if (!dot || *(dot + 1) == '\0') {
        return false;
    }

    return true;
}

/**
 * @brief Validates User ID format
 */
bool userid_validate(const char *userid) {
    if (!userid) {
        return false;
    }

    int len = strlen(userid);

    // Check length constraints
    if (len < 3 || len > 64) {
        return false;
    }

    // Check first character (must be alphanumeric)
    if (!isalnum(userid[0])) {
        return false;
    }

    // Check all characters are valid
    for (int i = 0; i < len; i++) {
        if (!userid_is_valid_char(userid[i])) {
            return false;
        }
    }

    // If it contains @, validate as email
    if (strchr(userid, '@')) {
        return userid_is_email_format(userid);
    }

    // Otherwise, it's a valid username
    return true;
}
