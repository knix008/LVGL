/**
 * @file number_input.h
 * @brief Number input processing
 */

#ifndef NUMBER_INPUT_H
#define NUMBER_INPUT_H

/**
 * @brief Process number key press
 * @param key The numeric key ('0'-'9', '*', '#')
 * @return The character to output, or 0 if no character
 */
char number_process_key(char key);

/**
 * @brief Reset number input state
 */
void number_reset(void);

#endif /* NUMBER_INPUT_H */
