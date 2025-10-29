/*
** ime_korean.c: Korean IME implementation for Hangul composition
**
** Copyright (C) 2024 Korean IME Implementation
**
** Create date: 2024/10/29
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include "ime_korean.h"

/* Unicode ranges for Hangul */
#define HANGUL_SYLLABLE_BASE    0xAC00
#define HANGUL_SYLLABLE_END     0xD7A3
#define HANGUL_JAMO_INITIAL_BASE 0x1100
#define HANGUL_JAMO_MEDIAL_BASE  0x1161  
#define HANGUL_JAMO_FINAL_BASE   0x11A8

/* Number of jamo */
#define INITIAL_COUNT 19
#define MEDIAL_COUNT  21
#define FINAL_COUNT   28

/* Korean QWERTY layout mapping */
/* Consonants: ㄱㄲㄴㄷㄸㄹㅁㅂㅃㅅㅆㅇㅈㅉㅊㅋㅌㅍㅎ */
const char korean_qwerty_consonants[] = "rRseEfaqQtTdwWczxvg";

/* Vowels: ㅏㅐㅑㅒㅓㅔㅕㅖㅗㅛㅜㅠㅡㅣ */
/* Standard Korean QWERTY (Dubeolsik) layout */
const char korean_qwerty_vowels[] = "yuiophjklbnm";

/* Double vowel (diphthong) combinations */
typedef struct {
    int base_vowel;     /* Base vowel index */
    int add_vowel;      /* Added vowel index */
    int result_vowel;   /* Resulting diphthong index */
} diphthong_rule_t;

/* Korean diphthong rules: base + add = result */
static const diphthong_rule_t diphthong_rules[] = {
    /* ㅗ + ㅏ = ㅘ (wa) */
    {8, 0, 9},   /* ㅗ + ㅏ = ㅘ */
    /* ㅗ + ㅐ = ㅙ (wae) */
    {8, 1, 10},  /* ㅗ + ㅐ = ㅙ */
    /* ㅗ + ㅣ = ㅚ (oe) */
    {8, 20, 11}, /* ㅗ + ㅣ = ㅚ */
    /* ㅜ + ㅓ = ㅝ (wo) */
    {13, 4, 14}, /* ㅜ + ㅓ = ㅝ */
    /* ㅜ + ㅔ = ㅞ (we) */
    {13, 5, 15}, /* ㅜ + ㅔ = ㅞ */
    /* ㅜ + ㅣ = ㅟ (wi) */
    {13, 20, 16}, /* ㅜ + ㅣ = ㅟ */
    /* ㅡ + ㅣ = ㅢ (ui) */
    {18, 20, 19}, /* ㅡ + ㅣ = ㅢ */
    /* End marker */
    {-1, -1, -1}
};

/* Jamo mapping for Korean QWERTY layout */
/* Maps QWERTY keys to jamo indices */
static const int consonant_map[] = {
    0,  /* r -> ㄱ */
    1,  /* R -> ㄲ */
    2,  /* s -> ㄴ */
    3,  /* e -> ㄷ */
    4,  /* E -> ㄸ */
    5,  /* f -> ㄹ */
    6,  /* a -> ㅁ */
    7,  /* q -> ㅂ */
    8,  /* Q -> ㅃ */
    9,  /* t -> ㅅ */
    10, /* T -> ㅆ */
    11, /* d -> ㅇ */
    12, /* w -> ㅈ */
    13, /* W -> ㅉ */
    14, /* c -> ㅊ */
    15, /* z -> ㅋ */
    16, /* x -> ㅌ */
    17, /* v -> ㅍ */
    18  /* g -> ㅎ */
};

/* CORRECTED vowel mapping for Korean QWERTY layout */
/* Standard Dubeolsik layout: kKioOjpuhynbml */
/* Maps to jamo indices: 0,1,2,3,4,5,6,7,8,12,13,17,18,20 */
static const int vowel_map[] = {
    12, /* y -> ㅛ */
    6,  /* u -> ㅕ */
    2,  /* i -> ㅑ */
    1,  /* o -> ㅐ */
    5,  /* p -> ㅔ */
    8,  /* h -> ㅗ */
    4,  /* j -> ㅓ */
    0,  /* k -> ㅏ */
    20, /* l -> ㅣ */
    17, /* b -> ㅠ */
    13, /* n -> ㅜ */
    18  /* m -> ㅡ */
};

static hangul_syllable_t current_syllable = {-1, -1, -1, HANGUL_STATE_NONE};

/* Map initial-consonant index (0..18) to single final index (0..27), -1 if not valid as single final */
static const int initial_to_final_index[19] = {
    /* ㄱ ㄲ ㄴ ㄷ ㄸ ㄹ ㅁ ㅂ ㅃ ㅅ ㅆ ㅇ ㅈ ㅉ ㅊ ㅋ ㅌ ㅍ ㅎ */
       0,  1,  3,  6, -1,  7, 15, 16, -1, 18, 19, 20, 21, -1, 22, 23, 24, 25, 26
};

/* Map single final index back to initial-consonant index (0..18) */
static const int final_to_initial_index[27] = {
    /* 0:ㄱ 1:ㄲ 2:ㄳ 3:ㄴ 4:ㄵ 5:ㄶ 6:ㄷ 7:ㄹ 8:ㄺ 9:ㄻ 10:ㄼ 11:ㄽ 12:ㄾ 13:ㄿ 14:ㅀ 15:ㅁ 16:ㅂ 17:ㅄ 18:ㅅ 19:ㅆ 20:ㅇ 21:ㅈ 22:ㅊ 23:ㅋ 24:ㅌ 25:ㅍ 26:ㅎ */
    /* map composite entries to their leading element for safety where needed */
     0,  1,  0,  2,  2,  2,  3,  5,  5,  5,   5,   5,   5,   5,   5,   6,   7,   7,   9,  10,  11,  12,  14,  15,  16,  17,  18
};

typedef struct {
    int base_final;       /* existing single final index */
    int add_final_single; /* additional single final index */
    int combined_final;   /* resulting combined final index */
} final_combine_rule_t;

/* Rules to combine two finals into a complex final (종성 결합) */
static const final_combine_rule_t final_combine_rules[] = {
    {0, 18, 2},   /* ㄱ + ㅅ = ㄳ */
    {3, 21, 4},   /* ㄴ + ㅈ = ㄵ */
    {3, 26, 5},   /* ㄴ + ㅎ = ㄶ */
    {7, 0,  8},   /* ㄹ + ㄱ = ㄺ */
    {7, 15, 9},   /* ㄹ + ㅁ = ㄻ */
    {7, 16, 10},  /* ㄹ + ㅂ = ㄼ */
    {7, 18, 11},  /* ㄹ + ㅅ = ㄽ */
    {7, 24, 12},  /* ㄹ + ㅌ = ㄾ */
    {7, 25, 13},  /* ㄹ + ㅍ = ㄿ */
    {7, 26, 14},  /* ㄹ + ㅎ = ㅀ */
    {16, 18, 17}, /* ㅂ + ㅅ = ㅄ */
    {-1,-1,-1}
};

typedef struct {
    int combined_final;   /* combined final index */
    int left_final;       /* remaining single final index to keep */
    int moved_initial;    /* initial index to move to next syllable */
} final_split_rule_t;

/* Rules to split complex final when a vowel follows: keep left, move right to next onset */
static const final_split_rule_t final_split_rules[] = {
    {2,  0,  9},  /* ㄳ -> ㄱ + ㅅ */
    {4,  3, 12},  /* ㄵ -> ㄴ + ㅈ */
    {5,  3, 18},  /* ㄶ -> ㄴ + ㅎ */
    {8,  7,  0},  /* ㄺ -> ㄹ + ㄱ */
    {9,  7,  6},  /* ㄻ -> ㄹ + ㅁ */
    {10, 7,  7},  /* ㄼ -> ㄹ + ㅂ */
    {11, 7,  9},  /* ㄽ -> ㄹ + ㅅ */
    {12, 7, 16},  /* ㄾ -> ㄹ + ㅌ */
    {13, 7, 17},  /* ㄿ -> ㄹ + ㅍ */
    {14, 7, 18},  /* ㅀ -> ㄹ + ㅎ */
    {17, 16, 9},  /* ㅄ -> ㅂ + ㅅ */
    {-1,-1,-1}
};

/* Try to combine existing final with an added consonant; returns new final index or -1 if not combinable */
static int try_combine_final(int current_final_index, int new_consonant_initial_index)
{
    int add_final = initial_to_final_index[new_consonant_initial_index];
    if (current_final_index < 0 || add_final < 0) return -1;
    const final_combine_rule_t* r = final_combine_rules;
    while (r->base_final != -1) {
        if (r->base_final == current_final_index && r->add_final_single == add_final)
            return r->combined_final;
        r++;
    }
    return -1;
}

/* If current final is combined and a vowel follows, split it; returns 1 if split done */
static int try_split_final_on_vowel(int* final_index, int* next_initial_index)
{
    const final_split_rule_t* r = final_split_rules;
    while (r->combined_final != -1) {
        if (*final_index == r->combined_final) {
            *final_index = r->left_final;
            *next_initial_index = r->moved_initial;
            return 1;
        }
        r++;
    }
    return 0;
}

/* Check if two vowels can form a diphthong */
static int check_diphthong(int base_vowel, int add_vowel, int* result_vowel)
{
    const diphthong_rule_t* rule = diphthong_rules;
    
    while (rule->base_vowel != -1) {
        if (rule->base_vowel == base_vowel && rule->add_vowel == add_vowel) {
            *result_vowel = rule->result_vowel;
            return 1; /* Found matching diphthong rule */
        }
        rule++;
    }
    
    return 0; /* No diphthong rule found */
}

/* Map Korean QWERTY key to jamo */
int korean_char_to_jamo(char c, int* jamo_type, int* jamo_index)
{
    const char* pos;
    
    /* Check if it's a consonant */
    pos = strchr(korean_qwerty_consonants, c);
    if (pos) {
        *jamo_type = 0; /* consonant */
        *jamo_index = consonant_map[pos - korean_qwerty_consonants];
        return 1;
    }
    
    /* Check if it's a vowel */
    pos = strchr(korean_qwerty_vowels, c);
    if (pos) {
        *jamo_type = 1; /* vowel */
        *jamo_index = vowel_map[pos - korean_qwerty_vowels];
        return 1;
    }
    
    return 0; /* Not a Korean character */
}

/* Compose Hangul syllable from jamo */
int hangul_compose_syllable(int initial, int medial, int final)
{
    if (initial < 0 || initial >= INITIAL_COUNT || 
        medial < 0 || medial >= MEDIAL_COUNT ||
        final < -1 || final >= FINAL_COUNT) {
        return 0;
    }
    
    int syllable = HANGUL_SYLLABLE_BASE + 
                   (initial * 21 * 28) + 
                   (medial * 28) + 
                   (final + 1);
    
    return syllable;
}

/* Decompose Hangul syllable to jamo */
int hangul_decompose_syllable(int syllable, int* initial, int* medial, int* final)
{
    if (syllable < HANGUL_SYLLABLE_BASE || syllable > HANGUL_SYLLABLE_END) {
        return 0;
    }
    
    int code = syllable - HANGUL_SYLLABLE_BASE;
    *initial = code / (21 * 28);
    *medial = (code % (21 * 28)) / 28;
    *final = (code % 28) - 1;
    
    return 1;
}

/* Korean IME callback for keystroke matching and composition */
int cb_hangul_match_keystrokes(const char* strokes, char* buffer, int buffer_len, int index, int case_mode)
{
    (void)index;
    (void)case_mode;
    if (!strokes || !buffer || buffer_len <= 0) {
        return 0;
    }
    
    int len = strlen(strokes);
    if (len == 0) {
        buffer[0] = '\0';
        current_syllable.state = HANGUL_STATE_NONE;
        current_syllable.initial = current_syllable.medial = current_syllable.final = -1;
        return 0;
    }
    
    /* Get the last input character */
    char last_char = strokes[len - 1];
    int jamo_type, jamo_index;
    
    if (!korean_char_to_jamo(last_char, &jamo_type, &jamo_index)) {
        /* Not a Korean character, treat as Latin */
        if (buffer_len > 1) {
            buffer[0] = last_char;
            buffer[1] = '\0';
        }
        return 1;
    }
    
    /* Process Korean character composition */
    if (jamo_type == 0) { /* Consonant */
        if (current_syllable.state == HANGUL_STATE_NONE) {
            /* Start new syllable */
            current_syllable.initial = jamo_index;
            current_syllable.medial = -1;
            current_syllable.final = -1;
            current_syllable.state = HANGUL_STATE_CONSONANT;
        } else if (current_syllable.state == HANGUL_STATE_VOWEL) {
            /* Add final consonant (as single final index) */
            int fin = initial_to_final_index[jamo_index];
            if (fin >= 0) {
                current_syllable.final = fin;
                current_syllable.state = HANGUL_STATE_FINAL_CONSONANT;
            }
        } else {
            /* We already have a final; try to make complex final, otherwise start new syllable */
            int combined = try_combine_final(current_syllable.final, jamo_index);
            if (combined >= 0) {
                current_syllable.final = combined;
                current_syllable.state = HANGUL_STATE_FINAL_CONSONANT;
            } else {
                current_syllable.initial = jamo_index;
                current_syllable.medial = -1;
                current_syllable.final = -1;
                current_syllable.state = HANGUL_STATE_CONSONANT;
            }
        }
    } else { /* Vowel */
        if (current_syllable.state == HANGUL_STATE_CONSONANT) {
            /* Add vowel to form syllable (no diphthong check here) */
            current_syllable.medial = jamo_index;
            current_syllable.state = HANGUL_STATE_VOWEL;
        } else if (current_syllable.state == HANGUL_STATE_VOWEL) {
            /* Only check for diphthong if previous input was a vowel */
            int diphthong_result;
            if (check_diphthong(current_syllable.medial, jamo_index, &diphthong_result)) {
                /* Form diphthong */
                current_syllable.medial = diphthong_result;
                printf("Diphthong formed: %d + %d = %d\n", current_syllable.medial, jamo_index, diphthong_result);
            } else {
                /* Cannot form diphthong, start new syllable */
                current_syllable.initial = 11; /* ㅇ as placeholder */
                current_syllable.medial = jamo_index;
                current_syllable.final = -1;
                current_syllable.state = HANGUL_STATE_VOWEL;
            }
        } else {
            /* We have a final consonant and a new vowel comes: try to split complex final */
            int moved_initial = -1;
            if (current_syllable.state == HANGUL_STATE_FINAL_CONSONANT &&
                try_split_final_on_vowel(&current_syllable.final, &moved_initial)) {
                /* Compose previous syllable into buffer now */
            } else if (current_syllable.state == HANGUL_STATE_FINAL_CONSONANT && current_syllable.final >= 0) {
                /* Single final: move entire final to next initial */
                moved_initial = final_to_initial_index[current_syllable.final];
                current_syllable.final = -1; /* previous syllable loses final */
            }

            /* Compose previous syllable to buffer */
            if (current_syllable.initial >= 0 && current_syllable.medial >= 0) {
                int prev = hangul_compose_syllable(current_syllable.initial, current_syllable.medial, current_syllable.final);
                if (prev > 0 && buffer_len >= 4) {
                    buffer[0] = 0xE0 | ((prev >> 12) & 0x0F);
                    buffer[1] = 0x80 | ((prev >> 6) & 0x3F);
                    buffer[2] = 0x80 | (prev & 0x3F);
                    buffer[3] = '\0';
                }
            }

            /* Start next syllable with moved initial and current vowel */
            if (moved_initial < 0) moved_initial = 11; /* fallback ㅇ */
            current_syllable.initial = moved_initial;
            current_syllable.medial = jamo_index;
            current_syllable.final = -1;
            current_syllable.state = HANGUL_STATE_VOWEL;
        }
    }
    
    /* Compose the syllable */
    if (current_syllable.initial >= 0 && current_syllable.medial >= 0) {
        int syllable = hangul_compose_syllable(current_syllable.initial, 
                                               current_syllable.medial, 
                                               current_syllable.final);
        
        /* Convert to UTF-8 */
        if (syllable > 0 && buffer_len >= 4) {
            /* Simple UTF-8 encoding for Korean syllables */
            buffer[0] = 0xE0 | ((syllable >> 12) & 0x0F);
            buffer[1] = 0x80 | ((syllable >> 6) & 0x3F);
            buffer[2] = 0x80 | (syllable & 0x3F);
            buffer[3] = '\0';
        }
    } else if (current_syllable.state == HANGUL_STATE_CONSONANT) {
        /* Just a consonant, output as jamo */
        if (buffer_len >= 4) {
            int jamo_code = HANGUL_JAMO_INITIAL_BASE + current_syllable.initial;
            buffer[0] = 0xE0 | ((jamo_code >> 12) & 0x0F);
            buffer[1] = 0x80 | ((jamo_code >> 6) & 0x3F);
            buffer[2] = 0x80 | (jamo_code & 0x3F);
            buffer[3] = '\0';
        }
    }
    
    return 1;
}

/* Compose function - same as match for Korean */
int cb_hangul_compose(const char* strokes, char* buffer, int buffer_len, int index, int case_mode)
{
    return cb_hangul_match_keystrokes(strokes, buffer, buffer_len, index, case_mode);
}
