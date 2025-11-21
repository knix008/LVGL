#ifndef HANGUL_H
#define HANGUL_H

#include "chunjiin.h"

// Hangul-specific initialization
void hangul_init(HangulState *hangul);

// Hangul input processing
void hangul_make(ChunjiinState *state, int input);

// Hangul text output
void hangul_write(ChunjiinState *state);

// Unicode conversion helpers
int hangul_get_unicode(HangulState *hangul, const wchar_t *real_jong);
void hangul_check_double(const wchar_t *jong, const wchar_t *jong2, wchar_t *result);

#endif // HANGUL_H
