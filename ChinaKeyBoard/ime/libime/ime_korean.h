/*
** ime_korean.h: Korean IME header file for Hangul composition
**
** Copyright (C) 2024 Korean IME Implementation
**
** Create date: 2024/10/29
**
*/

#ifndef __IME_KOREAN_H__
#define __IME_KOREAN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Korean character composition states */
typedef enum {
    HANGUL_STATE_NONE = 0,
    HANGUL_STATE_CONSONANT,
    HANGUL_STATE_VOWEL,
    HANGUL_STATE_FINAL_CONSONANT,
    HANGUL_STATE_COMPLETE
} hangul_state_t;

/* Korean syllable structure */
typedef struct {
    int initial;    /* Initial consonant (초성) */
    int medial;     /* Medial vowel (중성) */
    int final;      /* Final consonant (종성) */
    hangul_state_t state;
} hangul_syllable_t;

/* Korean IME callback functions */
int cb_hangul_match_keystrokes(const char* strokes, char* buffer, int buffer_len, int index, int case_mode);
int cb_hangul_compose(const char* strokes, char* buffer, int buffer_len, int index, int case_mode);

/* Korean character mapping functions */
int korean_char_to_jamo(char c, int* jamo_type, int* jamo_index);
int hangul_compose_syllable(int initial, int medial, int final);
int hangul_decompose_syllable(int syllable, int* initial, int* medial, int* final);

/* Korean QWERTY layout mapping */
extern const char korean_qwerty_consonants[];
extern const char korean_qwerty_vowels[];
extern const int korean_qwerty_jamo_map[];

#ifdef __cplusplus
}
#endif

#endif /* __IME_KOREAN_H__ */
