#ifndef CHUNJIIN_H
#define CHUNJIIN_H

#include <stdbool.h>
#include <wchar.h>

#define MAX_TEXT_LEN 1024

// Input mode enumeration
typedef enum {
    MODE_HANGUL = 0,
    MODE_UPPER_ENGLISH = 1,
    MODE_ENGLISH = 2,
    MODE_NUMBER = 3,
    MODE_SPECIAL = 4
} InputMode;

// Hangul-specific composition state
typedef struct {
    wchar_t chosung[16];      // 초성 (initial consonants)
    wchar_t jungsung[16];     // 중성 (medial vowels)
    wchar_t jongsung[16];     // 종성 (final consonants)
    wchar_t jongsung2[16];    // 종성2 (double final consonants)
    int step;                 // Current composition step (0:초성, 1:중성, 2:종성, 3:겹받침)
    bool flag_writing;        // Composition in progress flag
    bool flag_dotused;        // Dot (·, ‥) used flag
    bool flag_doubled;        // Double final consonant flag
    bool flag_addcursor;      // Add cursor flag
    bool flag_space;          // Space flag
} HangulState;

// Core input state
typedef struct {
    HangulState hangul;
    InputMode now_mode;

    wchar_t engnum[16];       // English/number buffer
    bool flag_initengnum;     // English/number initialization flag
    bool flag_engdelete;      // English delete flag
    bool flag_upper;          // Uppercase flag

    wchar_t text_buffer[MAX_TEXT_LEN];  // Main text buffer
    int cursor_pos;           // Current cursor position
} ChunjiinState;

// Hangul composition engine
void hangul_init(HangulState *hangul);
void hangul_make(ChunjiinState *state, int input);
void hangul_write(ChunjiinState *state);
int hangul_get_unicode(HangulState *hangul, const wchar_t *real_jong);
void hangul_check_double(const wchar_t *jong, const wchar_t *jong2, wchar_t *result);

#endif // CHUNJIIN_H
