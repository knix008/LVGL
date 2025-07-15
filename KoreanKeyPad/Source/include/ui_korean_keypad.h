#ifndef UI_KOREAN_KEYPAD_H
#define UI_KOREAN_KEYPAD_H

#include "lvgl.h"
#include "ui_callbacks.h"

// Korean keypad display label (extern for callbacks)
extern lv_obj_t * korean_keypad_display_label;

// Korean keypad buffer (extern for callbacks)
extern char korean_keypad_buffer[128];

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

// Global keypad state
extern korean_keypad_state_t korean_keypad_state;

// Create Korean keypad UI components
void create_korean_keypad_ui(lv_obj_t * parent);

// Initialize Korean keypad
void init_korean_keypad(void);

// Korean keypad callback functions
void korean_keypad_char_cb(lv_event_t * e);
void korean_keypad_mode_cb(lv_event_t * e);
void korean_keypad_clear_cb(lv_event_t * e);
void korean_keypad_backspace_cb(lv_event_t * e);
void korean_keypad_enter_cb(lv_event_t * e);
void korean_keypad_space_cb(lv_event_t * e);

// Hangul composition functions
uint32_t compose_hangul_syllable(hangul_composition_t * comp);
void reset_hangul_composition(hangul_composition_t * comp);
void add_choseong(hangul_composition_t * comp, uint16_t choseong);
void add_jungseong(hangul_composition_t * comp, uint16_t jungseong);
void add_jongseong(hangul_composition_t * comp, uint16_t jongseong);

// Utility functions
void update_korean_display(void);
void add_char_to_buffer(const char * str);
void remove_char_from_buffer(void);

#endif // UI_KOREAN_KEYPAD_H 