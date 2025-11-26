#include "english.h"
#include "input.h"
#include <string.h>
#include <wchar.h>

// Helper function to convert wide char to lowercase
static wchar_t towlower_simple(wchar_t ch) {
    if (ch >= L'A' && ch <= L'Z') {
        return ch + (L'a' - L'A');
    }
    return ch;
}

void english_init(ChunjiinState *state) {
    memset(state->engnum, 0, sizeof(state->engnum));
    state->flag_initengnum = false;
    state->flag_engdelete = false;
}

void english_make(ChunjiinState *state, int input) {
    if (input == 10) { // Space
        if (wcslen(state->engnum) == 0) {
            wcscpy(state->engnum, L" ");
        } else {
            state->engnum[0] = 0;
        }
        state->flag_initengnum = true;
    } else if (input == 11) { // Delete
        input_delete_char(state);
        english_init(state);
    } else {
        // Determine mode: English or Special
        const wchar_t *str = L"";

        if (state->now_mode == MODE_SPECIAL) {
            // Special character mode
            switch (input) {
                case 0: str = L"~`^"; break;
                case 1: str = L"!#"; break;
                case 2: str = L"$%&"; break;
                case 3: str = L"*()"; break;
                case 4: str = L"+{}"; break;
                case 5: str = L"[]="; break;
                case 6: str = L"<>|"; break;
                case 7: str = L"-_"; break;
                case 8: str = L":;"; break;
                case 9: str = L"\"'/"; break;
                default: return;
            }
        } else {
            // English mode (uppercase or lowercase)
            switch (input) {
                case 0: str = L"@"; break;
                case 1: str = L"ABC"; break;
                case 2: str = L"DEF"; break;
                case 3: str = L"GHI"; break;
                case 4: str = L"JKL"; break;
                case 5: str = L"MNO"; break;
                case 6: str = L"PQR"; break;
                case 7: str = L"STU"; break;
                case 8: str = L"VWX"; break;
                case 9: str = L"YZ"; break;
                default: return;
            }
        }

        wchar_t ch[4];
        ch[0] = str[0];
        ch[1] = wcslen(str) > 1 ? str[1] : 0;
        ch[2] = wcslen(str) > 2 ? str[2] : 0;
        ch[3] = 0;

        if (wcslen(state->engnum) == 0) {
            state->engnum[0] = ch[0];
            state->engnum[1] = 0;
        } else if (state->engnum[0] == ch[0]) {
            if (ch[1] != 0) {
                state->engnum[0] = ch[1];
                state->engnum[1] = 0;
                state->flag_engdelete = true;
            }
        } else if (ch[1] != 0 && state->engnum[0] == ch[1]) {
            if (ch[2] != 0) {
                state->engnum[0] = ch[2];
                state->engnum[1] = 0;
                state->flag_engdelete = true;
            } else {
                state->engnum[0] = ch[0];
                state->engnum[1] = 0;
                state->flag_engdelete = true;
            }
        } else if (ch[2] != 0 && state->engnum[0] == ch[2]) {
            if (wcslen(str) > 3) {
                state->engnum[0] = str[3];
                state->engnum[1] = 0;
                state->flag_engdelete = true;
            } else {
                state->engnum[0] = ch[0];
                state->engnum[1] = 0;
                state->flag_engdelete = true;
            }
        } else if (wcslen(str) > 3 && state->engnum[0] == str[3]) {
            state->engnum[0] = ch[0];
            state->engnum[1] = 0;
            state->flag_engdelete = true;
        } else {
            state->engnum[0] = ch[0];
            state->engnum[1] = 0;
        }
    }
}

void english_write(ChunjiinState *state) {
    int position = state->cursor_pos;
    wchar_t str[MAX_TEXT_LEN] = {0};

    // Build string before cursor
    if (state->flag_engdelete) {
        wcsncpy(str, state->text_buffer, position - 1);
        str[position - 1] = 0;
    } else {
        wcsncpy(str, state->text_buffer, position);
        str[position] = 0;
    }

    // Add engnum (uppercase or lowercase)
    if (state->flag_upper || state->now_mode == MODE_NUMBER) {
        wcscat(str, state->engnum);
    } else {
        // Convert to lowercase
        wchar_t lower_engnum[16];
        size_t i;
        for (i = 0; i < wcslen(state->engnum); i++) {
            lower_engnum[i] = towlower_simple(state->engnum[i]);
        }
        lower_engnum[i] = 0;
        wcscat(str, lower_engnum);
    }

    // Handle delete case
    if (state->flag_engdelete) {
        // Same button clicked - replace character at same position
        wcscat(str, &state->text_buffer[position]);
        wcscpy(state->text_buffer, str);
        // Keep cursor at same position (position is already cursor_pos from line 638)
        // Don't increment cursor - just stay at current position for next cycle
        state->cursor_pos = wcslen(str) - wcslen(&state->text_buffer[position]);
        state->flag_engdelete = false;
    } else {
        // Different button or first click - add new character
        wcscat(str, &state->text_buffer[position]);
        wcscpy(state->text_buffer, str);
        if (wcslen(state->engnum) == 0) {
            state->cursor_pos = position;
        } else {
            state->cursor_pos = position + 1;
        }
    }

    // Initialize engnum if flag is set
    if (state->flag_initengnum) {
        english_init(state);
    }
}
