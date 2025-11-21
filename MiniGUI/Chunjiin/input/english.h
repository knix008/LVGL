#ifndef ENGLISH_H
#define ENGLISH_H

#include "chunjiin.h"

// English input processing
void english_make(ChunjiinState *state, int input);

// English text output
void english_write(ChunjiinState *state);

// English/number initialization
void english_init(ChunjiinState *state);

#endif // ENGLISH_H
