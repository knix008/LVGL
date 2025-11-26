#include "number.h"
#include "input.h"
#include "english.h"
#include <string.h>
#include <wchar.h>

void number_make(ChunjiinState *state, int input) {
    if (input == 10) { // Space
        wcscpy(state->engnum, L" ");
    } else if (input == 11) { // Delete
        input_delete_char(state);
    } else {
        swprintf(state->engnum, 16, L"%d", input);
    }

    state->flag_initengnum = true;
}

void number_write(ChunjiinState *state) {
    // Number mode uses the same write logic as English mode
    english_write(state);
}
