/*
 * japanese_qwerty.h
 * Japanese Input Method Editor - Core Logic
 */

#ifndef JAPANESE_QWERTY_H
#define JAPANESE_QWERTY_H

#include <stdint.h>
#include <stdbool.h>

// Screen dimensions
#define SCREEN_WIDTH  640
#define SCREEN_HEIGHT 480

// Input mode enumeration
typedef enum {
    MODE_HIRAGANA,
    MODE_KATAKANA,
    MODE_ENGLISH
} InputMode;

// IME State structure
typedef struct {
    char buffer[256];          // Input buffer (romaji)
    char output[512];          // Output buffer (kana/kanji)
    char display[768];         // Complete display text
    int buffer_pos;            // Current position in buffer
    int output_pos;            // Current position in output
    int display_pos;           // Current position in display
    InputMode mode;            // Current input mode
    bool converting;           // Currently converting romaji
} IMEState;

// Romaji to Kana mapping structure
typedef struct {
    const char *romaji;
    const char *hiragana;
    const char *katakana;
} RomajiMap;

// Function declarations
void ime_init(IMEState *state);
void ime_set_mode(IMEState *state, InputMode mode);
void ime_process_char(IMEState *state, char c);
void ime_process_backspace(IMEState *state);
void ime_process_enter(IMEState *state);
void ime_process_space(IMEState *state);
void ime_clear(IMEState *state);
const char *ime_get_display_text(IMEState *state);
const char *ime_get_mode_string(IMEState *state);

// Internal conversion functions
void convert_buffer_to_kana(IMEState *state);
const RomajiMap *find_romaji_match(const char *str, int len);

#endif // JAPANESE_QWERTY_H

