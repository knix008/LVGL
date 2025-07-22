#ifndef KOREAN_H
#define KOREAN_H

#include <stdio.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <wctype.h>

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
int get_index(const char *jamo, const char *list[], int size);
const char* get_jamo_buffer(const char *buffer, KeyMap *map, int size);
int is_mappable_character(char ch);
void compose_korean_characters(const char* input_buffer, size_t input_len, wchar_t* output_buffer);

// Input handling functions for GUI
void handle_backspace_gui(char* input_buffer, size_t* input_len, wchar_t* output_buffer);
void handle_enter_gui(char* input_buffer, size_t* input_len, wchar_t* output_buffer);
void handle_space_gui(char* input_buffer, size_t* input_len, wchar_t* output_buffer);
void handle_character_gui(char* input_buffer, size_t* input_len, wchar_t* output_buffer, int ch);

// Main processing function for GUI
void process_input_gui(char* input_buffer, size_t* input_len, wchar_t* output_buffer, int ch);

// Legacy terminal functions (for compatibility)
void set_raw_mode(int enable);
void print_buffers(char *input_buf, wchar_t *output_buf);
void handle_backspace(char* input_buffer, size_t* input_len, wchar_t* output_buffer);
void handle_enter(char* input_buffer, size_t* input_len, wchar_t* output_buffer);
void handle_space(char* input_buffer, size_t* input_len, wchar_t* output_buffer);
void handle_character(char* input_buffer, size_t* input_len, wchar_t* output_buffer, int ch);
void init_korean_ime(void);
void cleanup_korean_ime(void);
void process_input(char* input_buffer, size_t* input_len, wchar_t* output_buffer, int ch);

#endif // KOREAN_H 