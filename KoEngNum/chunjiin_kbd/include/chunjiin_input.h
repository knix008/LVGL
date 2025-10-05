/**
 * @file chunjiin_input.h
 * @brief Chunjiin Korean input method implementation
 *
 * Chunjiin (천지인) is a Korean input method using letter keys:
 * Consonants: g,n,d,b,s,j,m
 * Vowels: a(ㆍ), e(ㅡ), i(ㅣ)
 */

#ifndef CHUNJIIN_INPUT_H
#define CHUNJIIN_INPUT_H

#include <stddef.h>

/* Function prototypes */

/**
 * @brief Initialize Chunjiin input system
 */
void chunjiin_init(void);

/**
 * @brief Reset Chunjiin state
 */
void chunjiin_reset(void);

/**
 * @brief Process a key press
 * @param key Character key (g,n,d,b,s,j,m for consonants; a,e,i for vowels; space,<,. for special)
 */
void chunjiin_process_input(char key);

/**
 * @brief Get current text as UTF-8
 * @param utf8_buffer Output buffer
 * @param buffer_size Size of output buffer
 */
void chunjiin_get_current_text_utf8(char* utf8_buffer, size_t buffer_size);

#endif /* CHUNJIIN_INPUT_H */
