#ifndef NUMBER_H
#define NUMBER_H

#include "chunjiin.h"

// Number input processing
void number_make(ChunjiinState *state, int input);

// Number text output (uses english_write)
void number_write(ChunjiinState *state);

#endif // NUMBER_H
