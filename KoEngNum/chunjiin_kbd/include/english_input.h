/**
 * @file english_input.h
 * @brief English T9 input processing
 */

#ifndef ENGLISH_INPUT_H
#define ENGLISH_INPUT_H

/**
 * @brief Process English T9 key press
 * @param key The numeric key ('0'-'9', '*', '#')
 * @return The character to output, or 0 if no character
 */
char english_process_key(char key);

/**
 * @brief Reset English input state
 */
void english_reset(void);

#endif /* ENGLISH_INPUT_H */
