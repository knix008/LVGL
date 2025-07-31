#ifndef QWERTY_KOREAN_H
#define QWERTY_KOREAN_H

#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <termios.h>
#include <unistd.h>
#include <wctype.h>
#include <stdlib.h>

// Constants
#define MAX_KEY_LEN 3
#define MAX_OUTPUT_LEN 256

// Key mapping structure
typedef struct {
    const char *key;
    const char *jamo;
} KeyMap;

// External declarations for key mappings
extern KeyMap cho_keymap[];
extern KeyMap jung_keymap[];
extern KeyMap jong_keymap[];

// External declarations for Korean character lists
extern const char* chosung_list[19];
extern const char* jungsung_list[21];
extern const char* jongsung_list[28];

// Function prototypes
int qwerty_get_index(const char *jamo, const char *list[], int size);
const char* qwerty_get_jamo_buffer(const char *buffer, KeyMap *map, int size);
void qwerty_print_buffers(char *input_buf, wchar_t *output_buf);
int qwerty_is_mappable_character(char ch);
void qwerty_compose_korean_characters(const char* input_buffer, size_t input_len, wchar_t* output_buffer);

// Input handling functions
void qwerty_handle_backspace(char* input_buffer, size_t* input_len, wchar_t* output_buffer);
void qwerty_handle_enter(char* input_buffer, size_t* input_len, wchar_t* output_buffer);
void qwerty_handle_space(char* input_buffer, size_t* input_len, wchar_t* output_buffer);
void qwerty_handle_character(char* input_buffer, size_t* input_len, wchar_t* output_buffer, int ch);

// Main processing function
void qwerty_korean_init(void);
void qwerty_korean_cleanup(void);
void qwerty_process_input(char* input_buffer, size_t* input_len, wchar_t* output_buffer, int ch);

#endif // QWERTY_KOREAN_H 