#ifndef INPUT_H
#define INPUT_H

#include "chunjiin.h"

// Initialization functions
void input_init(ChunjiinState *state);
void input_clear_preserve_mode(ChunjiinState *state);

// Input processing function (main dispatcher)
void input_process(ChunjiinState *state, int input);

// Text manipulation functions
void input_delete_char(ChunjiinState *state);

// Mode management
void input_change_mode(ChunjiinState *state);

// Button text retrieval
const wchar_t* input_get_button_text(InputMode mode, int button_num);

#endif // INPUT_H
