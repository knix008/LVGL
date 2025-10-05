/**
 * @file number_input.c
 * @brief Number input implementation
 */

#include "number_input.h"

char number_process_key(char key) {
    /* Direct passthrough for number input */
    if ((key >= '0' && key <= '9') || key == '*' || key == '#') {
        return key;
    }
    return 0;
}

void number_reset(void) {
    /* No state to reset for number input */
}
