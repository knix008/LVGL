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

// Decompose compound jongsung into first and second consonants
// Returns 1 if successful decomposition, 0 otherwise
static int decompose_consonants(wchar_t compound, wchar_t *first, wchar_t *second) {
    struct {
        wchar_t compound;
        wchar_t first;
        wchar_t second;
    } combinations[] = {
        {L'ㄳ', L'ㄱ', L'ㅅ'},  // ㄳ = ㄱ + ㅅ
        {L'ㄵ', L'ㄴ', L'ㅈ'},  // ㄵ = ㄴ + ㅈ
        {L'ㄶ', L'ㄴ', L'ㅎ'},  // ㄶ = ㄴ + ㅎ
        {L'ㄺ', L'ㄹ', L'ㄱ'},  // ㄺ = ㄹ + ㄱ
        {L'ㄻ', L'ㄹ', L'ㅁ'},  // ㄻ = ㄹ + ㅁ
        {L'ㄼ', L'ㄹ', L'ㅂ'},  // ㄼ = ㄹ + ㅂ
        {L'ㄽ', L'ㄹ', L'ㅅ'},  // ㄽ = ㄹ + ㅅ
        {L'ㄾ', L'ㄹ', L'ㅌ'},  // ㄾ = ㄹ + ㅌ
        {L'ㄿ', L'ㄹ', L'ㅍ'},  // ㄿ = ㄹ + ㅍ
        {L'ㅀ', L'ㄹ', L'ㅎ'},  // ㅀ = ㄹ + ㅎ
        {L'ㅄ', L'ㅂ', L'ㅅ'},  // ㅄ = ㅂ + ㅅ
    };

    for (int i = 0; i < sizeof(combinations) / sizeof(combinations[0]); i++) {
        if (combinations[i].compound == compound) {
            *first = combinations[i].first;
            *second = combinations[i].second;
            return 1;
        }
    }

    return 0; // No decomposition possible
}

// Map doubled consonants to their base form for jongsung
static wchar_t map_doubled_to_base(wchar_t ch) {
    switch (ch) {
        case L'ㅃ': return L'ㅂ';  // ㅃ -> ㅂ
        case L'ㄸ': return L'ㄷ';  // ㄸ -> ㄷ
        case L'ㅉ': return L'ㅈ';  // ㅉ -> ㅈ
        default: return ch;
    }
}

// Compose Hangul syllable from cho, jung, jong
static wchar_t compose_hangul(wchar_t cho, wchar_t jung, wchar_t jong) {
    int cho_idx = get_cho_index(cho);
    int jung_idx = get_jung_index(jung);

    int jong_idx = 0;
    if (jong != 0) {
        jong_idx = get_jong_index(jong);
        // If jong is not found, try mapping doubled consonants to their base form
        if (jong_idx < 0) {
            wchar_t base_jong = map_doubled_to_base(jong);
            if (base_jong != jong) {
                jong_idx = get_jong_index(base_jong);
            }
        }
    }

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

    custom_mbtowc(&wch, jamo_str, MB_CUR_MAX);

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
            // Cannot combine initial consonants, keep previous cho displayed and add new consonant
            *delete_previous = 0;
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
                int len = custom_wctomb(output, composed);
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
                    int len = custom_wctomb(output, composed);
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
                *delete_previous = 0;
                state->hangul.cho = wch;
                state->hangul.jung = 0;
                state->hangul.jong = 0;
                state->hangul.composing = 1;
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
                int len = custom_wctomb(output, composed);
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

            // Check if jong is a compound consonant that needs decomposition
            wchar_t jong_first = state->hangul.jong;
            wchar_t jong_second = 0;
            int is_compound = decompose_consonants(state->hangul.jong, &jong_first, &jong_second);

            if (is_compound) {
                // For compound jongsung, keep the first part and use second as new cho
                wchar_t composed = compose_hangul(state->hangul.cho, state->hangul.jung, jong_first);
                char utf8_syllable[7] = {0};
                int len = custom_wctomb(utf8_syllable, composed);
                if (len > 0) {
                    utf8_syllable[len] = '\0';
                }
                strcpy(output, utf8_syllable);

                // Start new syllable with jong_second as cho
                state->hangul.cho = jong_second;
                state->hangul.jung = wch;
                state->hangul.jong = 0;
            } else {
                // For simple jongsung, move it to become the cho of new syllable
                wchar_t composed = compose_hangul(state->hangul.cho, state->hangul.jung, 0);
                char utf8_syllable[7] = {0};
                int len = custom_wctomb(utf8_syllable, composed);
                if (len > 0) {
                    utf8_syllable[len] = '\0';
                }
                strcpy(output, utf8_syllable);

                // Start new syllable with jong as cho
                state->hangul.cho = state->hangul.jong;
                state->hangul.jung = wch;
                state->hangul.jong = 0;
            }

            wchar_t new_composed = compose_hangul(state->hangul.cho, state->hangul.jung, 0);
            char utf8_new[7] = {0};
            int len = custom_wctomb(utf8_new, new_composed);
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
                    int len = custom_wctomb(output, composed);
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
                *delete_previous = 0;
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

// Custom implementation of wctomb() - converts wide character to multibyte UTF-8
int custom_wctomb(char *s, wchar_t wc) {
    if (!s) {
        return 0; // Not state-dependent
    }

    if (wc < 0x80) {
        // 1-byte UTF-8 (ASCII)
        s[0] = (char)wc;
        return 1;
    } else if (wc < 0x800) {
        // 2-byte UTF-8
        s[0] = (char)(0xC0 | (wc >> 6));
        s[1] = (char)(0x80 | (wc & 0x3F));
        return 2;
    } else if (wc < 0x10000) {
        // 3-byte UTF-8
        s[0] = (char)(0xE0 | (wc >> 12));
        s[1] = (char)(0x80 | ((wc >> 6) & 0x3F));
        s[2] = (char)(0x80 | (wc & 0x3F));
        return 3;
    } else if (wc < 0x110000) {
        // 4-byte UTF-8
        s[0] = (char)(0xF0 | (wc >> 18));
        s[1] = (char)(0x80 | ((wc >> 12) & 0x3F));
        s[2] = (char)(0x80 | ((wc >> 6) & 0x3F));
        s[3] = (char)(0x80 | (wc & 0x3F));
        return 4;
    }
    
    return -1; // Invalid wide character
}

// Custom implementation of mbtowc() - converts multibyte UTF-8 to wide character
int custom_mbtowc(wchar_t *pwc, const char *s, size_t n) {
    if (!s) {
        return 0; // Not state-dependent
    }
    
    if (n == 0) {
        return -1;
    }
    
    unsigned char byte1 = (unsigned char)s[0];
    wchar_t wc;
    int len;
    
    if (byte1 < 0x80) {
        // 1-byte UTF-8 (ASCII)
        wc = byte1;
        len = 1;
    } else if ((byte1 & 0xE0) == 0xC0) {
        // 2-byte UTF-8
        if (n < 2) {
            return -1; // Incomplete sequence
        }
        unsigned char byte2 = (unsigned char)s[1];
        if ((byte2 & 0xC0) != 0x80) {
            return -1; // Invalid continuation byte
        }
        wc = ((byte1 & 0x1F) << 6) | (byte2 & 0x3F);
        len = 2;
    } else if ((byte1 & 0xF0) == 0xE0) {
        // 3-byte UTF-8
        if (n < 3) {
            return -1; // Incomplete sequence
        }
        unsigned char byte2 = (unsigned char)s[1];
        unsigned char byte3 = (unsigned char)s[2];
        if ((byte2 & 0xC0) != 0x80 || (byte3 & 0xC0) != 0x80) {
            return -1; // Invalid continuation byte
        }
        wc = ((byte1 & 0x0F) << 12) | ((byte2 & 0x3F) << 6) | (byte3 & 0x3F);
        len = 3;
    } else if ((byte1 & 0xF8) == 0xF0) {
        // 4-byte UTF-8
        if (n < 4) {
            return -1; // Incomplete sequence
        }
        unsigned char byte2 = (unsigned char)s[1];
        unsigned char byte3 = (unsigned char)s[2];
        unsigned char byte4 = (unsigned char)s[3];
        if ((byte2 & 0xC0) != 0x80 || (byte3 & 0xC0) != 0x80 || (byte4 & 0xC0) != 0x80) {
            return -1; // Invalid continuation byte
        }
        wc = ((byte1 & 0x07) << 18) | ((byte2 & 0x3F) << 12) | ((byte3 & 0x3F) << 6) | (byte4 & 0x3F);
        len = 4;
    } else {
        // Invalid UTF-8 start byte
        return -1;
    }
    
    if (pwc) {
        *pwc = wc;
    }
    
    return len;
}

// Custom implementation of mbstowcs() - converts multibyte string to wide character string
size_t custom_mbstowcs(wchar_t *dest, const char *src, size_t n) {
    size_t wc_count = 0;
    size_t i = 0;
    
    if (!src) {
        return (size_t)-1;
    }
    
    while (src[i] != '\0' && wc_count < n) {
        unsigned char byte1 = (unsigned char)src[i];
        wchar_t wc;
        
        if (byte1 < 0x80) {
            // 1-byte UTF-8 (ASCII)
            wc = byte1;
            i++;
        } else if ((byte1 & 0xE0) == 0xC0) {
            // 2-byte UTF-8
            if (src[i + 1] == '\0') {
                return (size_t)-1; // Incomplete sequence
            }
            unsigned char byte2 = (unsigned char)src[i + 1];
            if ((byte2 & 0xC0) != 0x80) {
                return (size_t)-1; // Invalid continuation byte
            }
            wc = ((byte1 & 0x1F) << 6) | (byte2 & 0x3F);
            i += 2;
        } else if ((byte1 & 0xF0) == 0xE0) {
            // 3-byte UTF-8
            if (src[i + 1] == '\0' || src[i + 2] == '\0') {
                return (size_t)-1; // Incomplete sequence
            }
            unsigned char byte2 = (unsigned char)src[i + 1];
            unsigned char byte3 = (unsigned char)src[i + 2];
            if ((byte2 & 0xC0) != 0x80 || (byte3 & 0xC0) != 0x80) {
                return (size_t)-1; // Invalid continuation byte
            }
            wc = ((byte1 & 0x0F) << 12) | ((byte2 & 0x3F) << 6) | (byte3 & 0x3F);
            i += 3;
        } else if ((byte1 & 0xF8) == 0xF0) {
            // 4-byte UTF-8
            if (src[i + 1] == '\0' || src[i + 2] == '\0' || src[i + 3] == '\0') {
                return (size_t)-1; // Incomplete sequence
            }
            unsigned char byte2 = (unsigned char)src[i + 1];
            unsigned char byte3 = (unsigned char)src[i + 2];
            unsigned char byte4 = (unsigned char)src[i + 3];
            if ((byte2 & 0xC0) != 0x80 || (byte3 & 0xC0) != 0x80 || (byte4 & 0xC0) != 0x80) {
                return (size_t)-1; // Invalid continuation byte
            }
            wc = ((byte1 & 0x07) << 18) | ((byte2 & 0x3F) << 12) | ((byte3 & 0x3F) << 6) | (byte4 & 0x3F);
            i += 4;
        } else {
            // Invalid UTF-8 start byte
            return (size_t)-1;
        }
        
        if (dest) {
            dest[wc_count] = wc;
        }
        wc_count++;
    }
    
    // Add null terminator if there's space
    if (dest && wc_count < n) {
        dest[wc_count] = L'\0';
    }
    
    return wc_count;
}
