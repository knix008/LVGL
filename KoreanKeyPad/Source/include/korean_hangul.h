#ifndef KOREAN_HANGUL_H
#define KOREAN_HANGUL_H

#include <stdint.h>
#include <stdbool.h>
#include "lvgl.h"

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

// Korean keypad state
typedef struct {
    korean_input_mode_t mode;
    hangul_composition_t composition;
    char buffer[128];
    int buffer_pos;
    lv_obj_t * display_label;
    lv_obj_t * mode_btn;
} korean_keypad_state_t;

// Global variables
extern char korean_keypad_buffer[128];
extern korean_keypad_state_t korean_keypad_state;
extern lv_font_t * korean_font;

// Character mapping arrays
extern const char* hangul_choseong[];
extern const char* hangul_jungseong[];
extern const char* hangul_jongseong[];
extern const char* english_chars[];
extern const char* number_chars[];

// Font initialization
void init_korean_font(void);

// Korean keypad initialization
void init_korean_keypad(void);

// Hangul composition functions
uint32_t compose_hangul_syllable(hangul_composition_t * comp);
void reset_hangul_composition(hangul_composition_t * comp);
void add_choseong(hangul_composition_t * comp, uint16_t choseong);
void add_jungseong(hangul_composition_t * comp, uint16_t jungseong);
void add_jongseong(hangul_composition_t * comp, uint16_t jongseong);

// Text processing functions
void update_korean_display(void);
void add_char_to_buffer(const char * str);
void remove_char_from_buffer(void);

// Character processing
void process_hangul_character(int char_index);

#endif // KOREAN_HANGUL_H 