#ifndef KOREAN_HANGUL_H
#define KOREAN_HANGUL_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// Hangul Unicode ranges
#define HANGUL_CHOSEONG_START 0x1100
#define HANGUL_JUNGSEONG_START 0x1161
#define HANGUL_JONGSEONG_START 0x11A8
#define HANGUL_SYLLABLE_START 0xAC00

// Input mode enumeration
typedef enum {
    KOREAN_MODE_HANGUL = 0,
    KOREAN_MODE_ENGLISH,
    KOREAN_MODE_NUMBER
} korean_input_mode_t;

// Hangul composition state
typedef struct {
    uint16_t choseong;  // Initial consonant
    uint16_t jungseong; // Vowel
    uint16_t jongseong; // Final consonant
    bool has_choseong;
    bool has_jungseong;
    bool has_jongseong;
} hangul_composition_t;

// Two-set keyboard input state
typedef struct {
    int step;  // 0: choseong, 1: jungseong, 2: jongseong
    int cho_idx;
    int jung_idx;
    int jong_idx;
} two_set_input_state_t;

// Korean keypad state
typedef struct {
    korean_input_mode_t mode;
    hangul_composition_t composition;
    two_set_input_state_t two_set_state;
    char buffer[128];
    int buffer_pos;
} korean_keypad_state_t;

// Global variables
extern char korean_keypad_buffer[128];
extern korean_keypad_state_t korean_keypad_state;

// Character mapping arrays
extern const char* hangul_choseong[];
extern const char* hangul_jungseong[];
extern const char* hangul_jongseong[];
extern const char* english_chars[];
extern const char* number_chars[];

// Two-set keyboard mapping
extern const char* two_set_choseong[];
extern const char* two_set_jungseong[];
extern const char* two_set_jongseong[];

// Two-set keyboard key to jamo mapping
typedef struct {
    char key;
    const char* jamo;
} key_jamo_map_t;

extern const key_jamo_map_t key_to_jamo[];

// Font initialization
void init_korean_font(void);

// Korean keypad initialization
void init_korean_keypad(void);

// Hangul composition functions
uint32_t compose_hangul_syllable(hangul_composition_t * comp);
uint32_t compose_hangul_from_indices(int cho_idx, int jung_idx, int jong_idx);
void reset_hangul_composition(hangul_composition_t * comp);
void add_choseong(hangul_composition_t * comp, uint16_t choseong);
void add_jungseong(hangul_composition_t * comp, uint16_t jungseong);
void add_jongseong(hangul_composition_t * comp, uint16_t jongseong);

// Two-set keyboard processing
void reset_two_set_state(two_set_input_state_t * state);
int find_jamo_index(const char* jamo, const char* jamo_array[], int array_size);
void process_two_set_input(char key);
void complete_current_syllable(void);

// Text processing functions
void add_char_to_buffer(const char * str);
void remove_char_from_buffer(void);

// Character processing
void process_hangul_character(int char_index);

// Utility functions
void unicode_to_utf8(uint32_t unicode, char* utf8_buffer);
const char* get_jamo_for_key(char key);
const char* process_single_key(char key);

// Test function declarations
void test_key_processing(void);
void test_hangul_composition(void);

#endif // KOREAN_HANGUL_H 