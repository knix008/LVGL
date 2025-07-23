#ifndef KOREAN_INPUT_H
#define KOREAN_INPUT_H

#include <stddef.h>

// Korean input state structure
typedef struct {
    int choseong_index;
    int jungseong_index;
    int jongseong_index;
    char buffer[64];
} korean_input_state_t;

// Function declarations
int korean_jamo_to_utf8_syllable(int choseong_idx, int jungseong_idx, int jongseong_idx, char* output, size_t output_size);

// Get Korean character arrays
const char** get_korean_choseong(void);
const char** get_korean_jungseong(void);
const char** get_korean_jongseong(void);

// Get array sizes
size_t get_korean_choseong_size(void);
size_t get_korean_jungseong_size(void);
size_t get_korean_jongseong_size(void);

// Initialize Korean input state
void korean_input_init(korean_input_state_t* state);

// Korean input operations
int korean_input_add_syllable(korean_input_state_t* state, char* output, size_t output_size);
int korean_input_backspace(korean_input_state_t* state);
void korean_input_clear(korean_input_state_t* state);

// Navigation functions
void korean_input_next_choseong(korean_input_state_t* state);
void korean_input_prev_choseong(korean_input_state_t* state);
void korean_input_next_jungseong(korean_input_state_t* state);
void korean_input_prev_jungseong(korean_input_state_t* state);
void korean_input_next_jongseong(korean_input_state_t* state);
void korean_input_prev_jongseong(korean_input_state_t* state);

#endif // KOREAN_INPUT_H 