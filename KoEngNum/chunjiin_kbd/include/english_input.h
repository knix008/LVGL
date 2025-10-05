/**
 * @file english_input.h
 * @brief English T9 input processing
 */

#ifndef ENGLISH_INPUT_H
#define ENGLISH_INPUT_H

/**
 * @brief Process English T9 key press
 * @param key The numeric key ('0'-'9', '*', '#')
 * @param is_replacement Output parameter: 1 if should replace last char, 0 if new char
 * @return The character to output, or 0 if no character
 */
char english_process_key(char key, int* is_replacement);

/**
 * @brief Reset English input state
 */
void english_reset(void);

/**
 * @brief Set shift mode (uppercase/lowercase)
 * @param enabled 1 for uppercase, 0 for lowercase
 */
void english_set_shift(int enabled);

/**
 * @brief Get current shift mode
 * @return 1 if uppercase, 0 if lowercase
 */
int english_get_shift(void);

#endif /* ENGLISH_INPUT_H */
