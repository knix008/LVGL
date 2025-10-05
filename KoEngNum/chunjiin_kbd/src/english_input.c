/**
 * @file english_input.c
 * @brief English T9 input implementation
 */

#include "english_input.h"
#include <time.h>

/* T9 character mappings */
static const char* key_chars[] = {
    " ",        /* 0 */
    ".,!?",     /* 1 */
    "abc",      /* 2 */
    "def",      /* 3 */
    "ghi",      /* 4 */
    "jkl",      /* 5 */
    "mno",      /* 6 */
    "pqrs",     /* 7 */
    "tuv",      /* 8 */
    "wxyz",     /* 9 */
};

static char last_key = 0;
static int current_index = 0;
static time_t last_press_time = 0;

#define TIMEOUT_MS 1000

char english_process_key(char key) {
    if (key < '0' || key > '9') {
        return 0;
    }

    time_t current_time = time(NULL);
    int key_num = key - '0';
    const char* chars = key_chars[key_num];
    int chars_len = 0;
    while (chars[chars_len]) chars_len++;

    /* Check if this is a repeat of the same key within timeout */
    if (key == last_key && (current_time - last_press_time) < 2) {
        /* Cycle to next character */
        current_index = (current_index + 1) % chars_len;
    } else {
        /* New key or timeout - start from first character */
        current_index = 0;
        last_key = key;
    }

    last_press_time = current_time;
    return chars[current_index];
}

void english_reset(void) {
    last_key = 0;
    current_index = 0;
    last_press_time = 0;
}
