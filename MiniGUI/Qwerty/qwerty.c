#include "qwerty.h"
#include <string.h>
#include <stdlib.h>
#include <locale.h>

// Hangul constants
#define HANGUL_BASE 0xAC00
#define CHO_COUNT 19
#define JUNG_COUNT 21
#define JONG_COUNT 28

// Hangul jamo tables
static const wchar_t CHO_JAMO[] = {
    L'ㄱ', L'ㄲ', L'ㄴ', L'ㄷ', L'ㄸ', L'ㄹ', L'ㅁ', L'ㅂ', L'ㅃ', L'ㅅ',
    L'ㅆ', L'ㅇ', L'ㅈ', L'ㅉ', L'ㅊ', L'ㅋ', L'ㅌ', L'ㅍ', L'ㅎ'
};

static const wchar_t JUNG_JAMO[] = {
    L'ㅏ', L'ㅐ', L'ㅑ', L'ㅒ', L'ㅓ', L'ㅔ', L'ㅕ', L'ㅖ', L'ㅗ', L'ㅘ',
    L'ㅙ', L'ㅚ', L'ㅛ', L'ㅜ', L'ㅝ', L'ㅞ', L'ㅟ', L'ㅠ', L'ㅡ', L'ㅢ', L'ㅣ'
};

static const wchar_t JONG_JAMO[] = {
    0, L'ㄱ', L'ㄲ', L'ㄳ', L'ㄴ', L'ㄵ', L'ㄶ', L'ㄷ', L'ㄹ', L'ㄺ',
    L'ㄻ', L'ㄼ', L'ㄽ', L'ㄾ', L'ㄿ', L'ㅀ', L'ㅁ', L'ㅂ', L'ㅄ', L'ㅅ',
    L'ㅆ', L'ㅇ', L'ㅈ', L'ㅊ', L'ㅋ', L'ㅌ', L'ㅍ', L'ㅎ'
};

// Key mappings for QWERTY layout
KeyMap key_maps[47] = {
    // Row 0
    {"'", "~", "'", "~"},  // Changed to apostrophe for visibility in both button and text
    {"1", "!", "1", "!"},
    {"2", "@", "2", "@"},
    {"3", "#", "3", "#"},
    {"4", "$", "4", "$"},
    {"5", "%", "5", "%"},
    {"6", "^", "6", "^"},
    {"7", "&", "7", "&"},
    {"8", "*", "8", "*"},
    {"9", "(", "9", "("},
    {"0", ")", "0", ")"},
    {"-", "_", "-", "_"},
    {"=", "+", "=", "+"},
    // Row 1
    {"q", "Q", "ㅂ", "ㅃ"},
    {"w", "W", "ㅈ", "ㅉ"},
    {"e", "E", "ㄷ", "ㄸ"},
    {"r", "R", "ㄱ", "ㄲ"},
    {"t", "T", "ㅅ", "ㅆ"},
    {"y", "Y", "ㅛ", "ㅛ"},
    {"u", "U", "ㅕ", "ㅕ"},
    {"i", "I", "ㅑ", "ㅑ"},
    {"o", "O", "ㅐ", "ㅒ"},
    {"p", "P", "ㅔ", "ㅖ"},
    {"[", "{", "[", "{"},
    {"]", "}", "]", "}"},
    {"\\", "|", "\\", "|"},
    // Row 2
    {"a", "A", "ㅁ", "ㅁ"},
    {"s", "S", "ㄴ", "ㄴ"},
    {"d", "D", "ㅇ", "ㅇ"},
    {"f", "F", "ㄹ", "ㄹ"},
    {"g", "G", "ㅎ", "ㅎ"},
    {"h", "H", "ㅗ", "ㅗ"},
    {"j", "J", "ㅓ", "ㅓ"},
    {"k", "K", "ㅏ", "ㅏ"},
    {"l", "L", "ㅣ", "ㅣ"},
    {";", ":", ";", ":"},
    {"'", "\"", "'", "\""},
    // Row 3
    {"z", "Z", "ㅋ", "ㅋ"},
    {"x", "X", "ㅌ", "ㅌ"},
    {"c", "C", "ㅊ", "ㅊ"},
    {"v", "V", "ㅍ", "ㅍ"},
    {"b", "B", "ㅠ", "ㅠ"},
    {"n", "N", "ㅜ", "ㅜ"},
    {"m", "M", "ㅡ", "ㅡ"},
    {",", "<", ",", "<"},
    {".", ">", ".", ">"},
    {"/", "?", "/", "?"},
};

// Check if character is a consonant (초성/종성)
int qwerty_is_consonant(wchar_t ch) {
    for (int i = 0; i < CHO_COUNT; i++) {
        if (CHO_JAMO[i] == ch) return 1;
    }
    return 0;
}

// Check if character is a vowel (중성)
int qwerty_is_vowel(wchar_t ch) {
    for (int i = 0; i < JUNG_COUNT; i++) {
        if (JUNG_JAMO[i] == ch) return 1;
    }
    return 0;
}

// Get index in cho/jung/jong arrays
static int get_cho_index(wchar_t ch) {
    for (int i = 0; i < CHO_COUNT; i++) {
        if (CHO_JAMO[i] == ch) return i;
    }
    return -1;
}

static int get_jung_index(wchar_t ch) {
    for (int i = 0; i < JUNG_COUNT; i++) {
        if (JUNG_JAMO[i] == ch) return i;
    }
    return -1;
}

static int get_jong_index(wchar_t ch) {
    for (int i = 1; i < JONG_COUNT; i++) {
        if (JONG_JAMO[i] == ch) return i;
    }
    return -1;
}

// Combine two vowels into a compound vowel if possible
static wchar_t combine_vowels(wchar_t first, wchar_t second) {
    // Define vowel combinations that form compound vowels
    struct {
        wchar_t first;
        wchar_t second;
        wchar_t result;
    } combinations[] = {
        {L'ㅗ', L'ㅏ', L'ㅘ'},  // ㅗ + ㅏ = ㅘ
        {L'ㅗ', L'ㅐ', L'ㅙ'},  // ㅗ + ㅐ = ㅙ
        {L'ㅗ', L'ㅣ', L'ㅚ'},  // ㅗ + ㅣ = ㅚ
        {L'ㅜ', L'ㅓ', L'ㅝ'},  // ㅜ + ㅓ = ㅝ (this is the key one for 권!)
        {L'ㅜ', L'ㅔ', L'ㅞ'},  // ㅜ + ㅔ = ㅞ
        {L'ㅜ', L'ㅣ', L'ㅟ'},  // ㅜ + ㅣ = ㅟ
        {L'ㅡ', L'ㅣ', L'ㅢ'},  // ㅡ + ㅣ = ㅢ
    };
    
    for (int i = 0; i < sizeof(combinations) / sizeof(combinations[0]); i++) {
        if (combinations[i].first == first && combinations[i].second == second) {
            return combinations[i].result;
        }
    }
    
    return 0; // No combination possible
}

// Combine two consonants into a compound jongsung if possible
static wchar_t combine_consonants(wchar_t first, wchar_t second) {
    // Define consonant combinations that form compound jongsung
    struct {
        wchar_t first;
        wchar_t second;
        wchar_t result;
    } combinations[] = {
        {L'ㄱ', L'ㅅ', L'ㄳ'},  // ㄱ + ㅅ = ㄳ
        {L'ㄴ', L'ㅈ', L'ㄵ'},  // ㄴ + ㅈ = ㄵ
        {L'ㄴ', L'ㅎ', L'ㄶ'},  // ㄴ + ㅎ = ㄶ
        {L'ㄹ', L'ㄱ', L'ㄺ'},  // ㄹ + ㄱ = ㄺ
        {L'ㄹ', L'ㅁ', L'ㄻ'},  // ㄹ + ㅁ = ㄻ
        {L'ㄹ', L'ㅂ', L'ㄼ'},  // ㄹ + ㅂ = ㄼ
        {L'ㄹ', L'ㅅ', L'ㄽ'},  // ㄹ + ㅅ = ㄽ
        {L'ㄹ', L'ㅌ', L'ㄾ'},  // ㄹ + ㅌ = ㄾ
        {L'ㄹ', L'ㅍ', L'ㄿ'},  // ㄹ + ㅍ = ㄿ
        {L'ㄹ', L'ㅎ', L'ㅀ'},  // ㄹ + ㅎ = ㅀ
        {L'ㅂ', L'ㅅ', L'ㅄ'},  // ㅂ + ㅅ = ㅄ
    };
    
    for (int i = 0; i < sizeof(combinations) / sizeof(combinations[0]); i++) {
        if (combinations[i].first == first && combinations[i].second == second) {
            return combinations[i].result;
        }
    }
    
    return 0; // No combination possible
}

// Compose Hangul syllable from cho, jung, jong
static wchar_t compose_hangul(wchar_t cho, wchar_t jung, wchar_t jong) {
    int cho_idx = get_cho_index(cho);
    int jung_idx = get_jung_index(jung);
    int jong_idx = (jong == 0) ? 0 : get_jong_index(jong);

    if (cho_idx < 0 || jung_idx < 0 || jong_idx < 0) return 0;

    return HANGUL_BASE + (cho_idx * JUNG_COUNT * JONG_COUNT) +
           (jung_idx * JONG_COUNT) + jong_idx;
}

// Initialize qwerty state
void qwerty_init(QwertyState *state) {
    state->current_language = LANG_KOREAN;
    state->shift_pressed = 0;
    state->caps_lock = 0;
    state->hangul.cho = 0;
    state->hangul.jung = 0;
    state->hangul.jong = 0;
    state->hangul.composing = 0;
}

// Reset composition state
void qwerty_reset_composition(QwertyState *state) {
    state->hangul.cho = 0;
    state->hangul.jung = 0;
    state->hangul.jong = 0;
    state->hangul.composing = 0;
}

// Get the appropriate character for a key
const char* qwerty_get_key_char(QwertyState *state, KeyMap *key_map) {
    if (state->current_language == LANG_KOREAN) {
        if (state->shift_pressed || state->caps_lock) {
            return key_map->korean_shift;
        } else {
            return key_map->korean;
        }
    } else {
        if (state->shift_pressed || state->caps_lock) {
            return key_map->shift;
        } else {
            return key_map->normal;
        }
    }
}

// Process Korean character with composition
// output: buffer to store the result (should be at least 7 bytes)
// delete_previous: set to 1 if previous character should be deleted first
void qwerty_process_korean_char(QwertyState *state, const char *jamo_str,
                                 char *output, int *delete_previous) {
    wchar_t wch;
    *delete_previous = 0;
    output[0] = '\0';

    mbtowc(&wch, jamo_str, MB_CUR_MAX);

    if (qwerty_is_consonant(wch)) {
        if (!state->hangul.composing) {
            // Start new syllable with cho
            state->hangul.cho = wch;
            state->hangul.jung = 0;
            state->hangul.jong = 0;
            state->hangul.composing = 1;
            strcpy(output, jamo_str);
        } else if (state->hangul.jung == 0) {
            // Already have cho, but no jung yet
            // Cannot combine initial consonants, so start new syllable
            state->hangul.cho = wch;
            state->hangul.jung = 0;
            state->hangul.jong = 0;
            state->hangul.composing = 1;
            strcpy(output, jamo_str);
        } else if (state->hangul.jong == 0) {
            // Have cho+jung, add jong
            *delete_previous = 1;
            state->hangul.jong = wch;
            wchar_t composed = compose_hangul(state->hangul.cho, state->hangul.jung, state->hangul.jong);
            if (composed) {
                int len = wctomb(output, composed);
                if (len > 0) {
                    output[len] = '\0';
                }
            } else {
                // Can't compose, insert separately
                *delete_previous = 0;
                strcpy(output, jamo_str);
                state->hangul.composing = 0;
            }
        } else {
            // Already have complete syllable (cho+jung+jong)
            // Try to combine jong with new consonant for compound jongsung
            wchar_t compound = combine_consonants(state->hangul.jong, wch);
            if (compound != 0) {
                // Successful compound jongsung combination
                *delete_previous = 1;
                state->hangul.jong = compound;
                wchar_t composed = compose_hangul(state->hangul.cho, state->hangul.jung, state->hangul.jong);
                if (composed) {
                    int len = wctomb(output, composed);
                    if (len > 0) {
                        output[len] = '\0';
                    }
                } else {
                    // Can't compose, insert separately
                    *delete_previous = 0;
                    strcpy(output, jamo_str);
                    state->hangul.composing = 0;
                }
            } else {
                // No compound possible, start new syllable
                state->hangul.cho = wch;
                state->hangul.jung = 0;
                state->hangul.jong = 0;
                strcpy(output, jamo_str);
            }
        }
    } else if (qwerty_is_vowel(wch)) {
        if (!state->hangul.composing || state->hangul.cho == 0) {
            // No cho, just insert vowel
            strcpy(output, jamo_str);
            state->hangul.composing = 0;
        } else if (state->hangul.jung == 0) {
            // Have cho, add jung to compose
            *delete_previous = 1;
            state->hangul.jung = wch;
            wchar_t composed = compose_hangul(state->hangul.cho, state->hangul.jung, 0);
            if (composed) {
                int len = wctomb(output, composed);
                if (len > 0) {
                    output[len] = '\0';
                }
            } else {
                // Can't compose, insert separately
                *delete_previous = 0;
                strcpy(output, jamo_str);
                state->hangul.composing = 0;
            }
        } else if (state->hangul.jong != 0) {
            // Have complete syllable, split jong and start new syllable
            *delete_previous = 1;
            wchar_t composed = compose_hangul(state->hangul.cho, state->hangul.jung, 0);
            char utf8_syllable[7] = {0};
            int len = wctomb(utf8_syllable, composed);
            if (len > 0) {
                utf8_syllable[len] = '\0';
            }
            strcpy(output, utf8_syllable);

            // Start new syllable with jong as cho
            state->hangul.cho = state->hangul.jong;
            state->hangul.jung = wch;
            state->hangul.jong = 0;
            wchar_t new_composed = compose_hangul(state->hangul.cho, state->hangul.jung, 0);
            char utf8_new[7] = {0};
            len = wctomb(utf8_new, new_composed);
            if (len > 0) {
                utf8_new[len] = '\0';
            }
            strcat(output, utf8_new);
        } else {
            // Already have cho+jung, try to combine vowels for compound vowel
            wchar_t compound = combine_vowels(state->hangul.jung, wch);
            if (compound != 0) {
                // Successful compound vowel combination
                *delete_previous = 1;
                state->hangul.jung = compound;
                wchar_t composed = compose_hangul(state->hangul.cho, state->hangul.jung, 0);
                if (composed) {
                    int len = wctomb(output, composed);
                    if (len > 0) {
                        output[len] = '\0';
                    }
                } else {
                    // Can't compose, insert separately
                    *delete_previous = 0;
                    strcpy(output, jamo_str);
                    state->hangul.composing = 0;
                }
            } else {
                // No compound possible, start new syllable
                state->hangul.composing = 0;
                strcpy(output, jamo_str);
            }
        }
    } else {
        // Not a jamo, just insert
        strcpy(output, jamo_str);
        state->hangul.composing = 0;
    }
}
