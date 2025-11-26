#include "input.h"
#include "chunjiin.h"
#include "english.h"
#include "number.h"
#include <string.h>

void input_init(ChunjiinState *state) {
    hangul_init(&state->hangul);
    state->now_mode = MODE_HANGUL;
    english_init(state);
    memset(state->text_buffer, 0, sizeof(state->text_buffer));
    state->cursor_pos = 0;
}

void input_clear_preserve_mode(ChunjiinState *state) {
    InputMode saved_mode = state->now_mode;
    input_init(state);
    state->now_mode = saved_mode;
}

void input_delete_char(ChunjiinState *state) {
    if (state->cursor_pos <= 0) return;

    int i;
    for (i = state->cursor_pos - 1; i < MAX_TEXT_LEN - 1; i++) {
        state->text_buffer[i] = state->text_buffer[i + 1];
    }
    state->cursor_pos--;
}

void input_process(ChunjiinState *state, int input) {
    if (input < 0 || input > 11) return;

    if (state->now_mode == MODE_HANGUL) {
        hangul_make(state, input);
        hangul_write(state);
    } else if (state->now_mode == MODE_ENGLISH || state->now_mode == MODE_UPPER_ENGLISH) {
        english_make(state, input);
        english_write(state);
    } else if (state->now_mode == MODE_NUMBER) {
        number_make(state, input);
        number_write(state);
    } else { // MODE_SPECIAL
        english_make(state, input);  // Special mode uses same cycling logic as English
        english_write(state);
    }
}

void input_change_mode(ChunjiinState *state) {
    state->now_mode = (state->now_mode == MODE_SPECIAL) ? MODE_HANGUL : state->now_mode + 1;
    hangul_init(&state->hangul);
    english_init(state);
    if (state->now_mode == MODE_UPPER_ENGLISH) {
        state->flag_upper = true;
    } else if (state->now_mode == MODE_ENGLISH) {
        state->flag_upper = false;
    }
}

const wchar_t* input_get_button_text(InputMode mode, int button_num) {
    static const wchar_t *hangul_texts[] = {
        L"ㅇㅁ", L"ㅣ", L"·", L"ㅡ", L"ㄱㅋ",
        L"ㄴㄹ", L"ㄷㅌ", L"ㅂㅍ", L"ㅅㅎ", L"ㅈㅊ",
        L"Space", L"Del"
    };
    static const wchar_t *upper_eng_texts[] = {
        L"@", L"ABC", L"DEF", L"GHI", L"JKL",
        L"MNO", L"PQR", L"STU", L"VWX", L"YZ",
        L"Space", L"Del"
    };
    static const wchar_t *lower_eng_texts[] = {
        L"@", L"abc", L"def", L"ghi", L"jkl",
        L"mno", L"pqr", L"stu", L"vwx", L"yz",
        L"Space", L"Del"
    };
    static const wchar_t *number_texts[] = {
        L"0", L"1", L"2", L"3", L"4",
        L"5", L"6", L"7", L"8", L"9",
        L"Space", L"Del"
    };
    static const wchar_t *special_texts[] = {
        L"~`^", L"!#", L"$%&", L"*()=", L"+{}",
        L"[]=", L"<>|", L"-_", L":;", L"\"'/",
        L"Space", L"Del"
    };

    if (button_num < 0 || button_num > 11) return L"";

    switch (mode) {
        case MODE_HANGUL: return hangul_texts[button_num];
        case MODE_UPPER_ENGLISH: return upper_eng_texts[button_num];
        case MODE_ENGLISH: return lower_eng_texts[button_num];
        case MODE_NUMBER: return number_texts[button_num];
        case MODE_SPECIAL:
            if (button_num == 0) return L"~`^";
            return special_texts[button_num];
        default: return L"";
    }
}
