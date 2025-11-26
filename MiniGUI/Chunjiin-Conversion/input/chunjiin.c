#include "chunjiin.h"
#include "input.h"
#include <string.h>
#include <wchar.h>

void hangul_init(HangulState *hangul) {
    memset(hangul->chosung, 0, sizeof(hangul->chosung));
    memset(hangul->jungsung, 0, sizeof(hangul->jungsung));
    memset(hangul->jongsung, 0, sizeof(hangul->jongsung));
    memset(hangul->jongsung2, 0, sizeof(hangul->jongsung2));
    hangul->step = 0;
    hangul->flag_writing = false;
    hangul->flag_dotused = false;
    hangul->flag_doubled = false;
    hangul->flag_addcursor = false;
    hangul->flag_space = false;
}

int hangul_get_unicode(HangulState *hangul, const wchar_t *real_jong) {
    // Compatibility Jamo arrays for standalone display
    static const int compat_cho[] = {
        0x3131, 0x3132, 0x3134, 0x3137, 0x3138, 0x3139, 0x3141, 0x3142,
        0x3143, 0x3145, 0x3146, 0x3147, 0x3148, 0x3149, 0x314A, 0x314B,
        0x314C, 0x314D, 0x314E
    };
    static const int compat_jung[] = {
        0x314F, 0x3150, 0x3151, 0x3152, 0x3153, 0x3154, 0x3155, 0x3156,
        0x3157, 0x3158, 0x3159, 0x315A, 0x315B, 0x315C, 0x315D, 0x315E,
        0x315F, 0x3160, 0x3161, 0x3162, 0x3163
    };
    static const int compat_jong[] = {
        0, 0x3131, 0x3132, 0x3133, 0x3134, 0x3135, 0x3136, 0x3137, 0x3139,
        0x313A, 0x313B, 0x313C, 0x313D, 0x313E, 0x313F, 0x3140, 0x3141, 0x3142,
        0x3144, 0x3145, 0x3146, 0x3147, 0x3148, 0x314A, 0x314B, 0x314C, 0x314D, 0x314E
    };

    int cho, jung, jong;

    // 초성이 없고 중성도 없거나 점만 있으면
    if (wcslen(hangul->chosung) == 0) {
        if (wcslen(hangul->jungsung) == 0 ||
            wcscmp(hangul->jungsung, L"·") == 0 ||
            wcscmp(hangul->jungsung, L"‥") == 0) {
            return 0;
        }
    }

    // 초성 처리
    if (wcscmp(hangul->chosung, L"ㄱ") == 0) cho = 0;
    else if (wcscmp(hangul->chosung, L"ㄲ") == 0) cho = 1;
    else if (wcscmp(hangul->chosung, L"ㄴ") == 0) cho = 2;
    else if (wcscmp(hangul->chosung, L"ㄷ") == 0) cho = 3;
    else if (wcscmp(hangul->chosung, L"ㄸ") == 0) cho = 4;
    else if (wcscmp(hangul->chosung, L"ㄹ") == 0) cho = 5;
    else if (wcscmp(hangul->chosung, L"ㅁ") == 0) cho = 6;
    else if (wcscmp(hangul->chosung, L"ㅂ") == 0) cho = 7;
    else if (wcscmp(hangul->chosung, L"ㅃ") == 0) cho = 8;
    else if (wcscmp(hangul->chosung, L"ㅅ") == 0) cho = 9;
    else if (wcscmp(hangul->chosung, L"ㅆ") == 0) cho = 10;
    else if (wcscmp(hangul->chosung, L"ㅇ") == 0) cho = 11;
    else if (wcscmp(hangul->chosung, L"ㅈ") == 0) cho = 12;
    else if (wcscmp(hangul->chosung, L"ㅉ") == 0) cho = 13;
    else if (wcscmp(hangul->chosung, L"ㅊ") == 0) cho = 14;
    else if (wcscmp(hangul->chosung, L"ㅋ") == 0) cho = 15;
    else if (wcscmp(hangul->chosung, L"ㅌ") == 0) cho = 16;
    else if (wcscmp(hangul->chosung, L"ㅍ") == 0) cho = 17;
    else cho = 18; // ㅎ

    if (wcslen(hangul->jungsung) == 0 && wcslen(hangul->jongsung) == 0) {
        return compat_cho[cho];
    }
    if (wcscmp(hangul->jungsung, L"·") == 0 || wcscmp(hangul->jungsung, L"‥") == 0) {
        return compat_cho[cho];
    }

    // 중성 처리
    if (wcscmp(hangul->jungsung, L"ㅏ") == 0) jung = 0;
    else if (wcscmp(hangul->jungsung, L"ㅐ") == 0) jung = 1;
    else if (wcscmp(hangul->jungsung, L"ㅑ") == 0) jung = 2;
    else if (wcscmp(hangul->jungsung, L"ㅒ") == 0) jung = 3;
    else if (wcscmp(hangul->jungsung, L"ㅓ") == 0) jung = 4;
    else if (wcscmp(hangul->jungsung, L"ㅔ") == 0) jung = 5;
    else if (wcscmp(hangul->jungsung, L"ㅕ") == 0) jung = 6;
    else if (wcscmp(hangul->jungsung, L"ㅖ") == 0) jung = 7;
    else if (wcscmp(hangul->jungsung, L"ㅗ") == 0) jung = 8;
    else if (wcscmp(hangul->jungsung, L"ㅘ") == 0) jung = 9;
    else if (wcscmp(hangul->jungsung, L"ㅙ") == 0) jung = 10;
    else if (wcscmp(hangul->jungsung, L"ㅚ") == 0) jung = 11;
    else if (wcscmp(hangul->jungsung, L"ㅛ") == 0) jung = 12;
    else if (wcscmp(hangul->jungsung, L"ㅜ") == 0) jung = 13;
    else if (wcscmp(hangul->jungsung, L"ㅝ") == 0) jung = 14;
    else if (wcscmp(hangul->jungsung, L"ㅞ") == 0) jung = 15;
    else if (wcscmp(hangul->jungsung, L"ㅟ") == 0) jung = 16;
    else if (wcscmp(hangul->jungsung, L"ㅠ") == 0) jung = 17;
    else if (wcscmp(hangul->jungsung, L"ㅡ") == 0) jung = 18;
    else if (wcscmp(hangul->jungsung, L"ㅢ") == 0) jung = 19;
    else jung = 20; // ㅣ

    if (wcslen(hangul->chosung) == 0 && wcslen(hangul->jongsung) == 0) {
        return compat_jung[jung];
    }

    // 종성 처리
    if (wcslen(real_jong) == 0) jong = 0;
    else if (wcscmp(real_jong, L"ㄱ") == 0) jong = 1;
    else if (wcscmp(real_jong, L"ㄲ") == 0) jong = 2;
    else if (wcscmp(real_jong, L"ㄳ") == 0) jong = 3;
    else if (wcscmp(real_jong, L"ㄴ") == 0) jong = 4;
    else if (wcscmp(real_jong, L"ㄵ") == 0) jong = 5;
    else if (wcscmp(real_jong, L"ㄶ") == 0) jong = 6;
    else if (wcscmp(real_jong, L"ㄷ") == 0) jong = 7;
    else if (wcscmp(real_jong, L"ㄹ") == 0) jong = 8;
    else if (wcscmp(real_jong, L"ㄺ") == 0) jong = 9;
    else if (wcscmp(real_jong, L"ㄻ") == 0) jong = 10;
    else if (wcscmp(real_jong, L"ㄼ") == 0) jong = 11;
    else if (wcscmp(real_jong, L"ㄽ") == 0) jong = 12;
    else if (wcscmp(real_jong, L"ㄾ") == 0) jong = 13;
    else if (wcscmp(real_jong, L"ㄿ") == 0) jong = 14;
    else if (wcscmp(real_jong, L"ㅀ") == 0) jong = 15;
    else if (wcscmp(real_jong, L"ㅁ") == 0) jong = 16;
    else if (wcscmp(real_jong, L"ㅂ") == 0) jong = 17;
    else if (wcscmp(real_jong, L"ㅄ") == 0) jong = 18;
    else if (wcscmp(real_jong, L"ㅅ") == 0) jong = 19;
    else if (wcscmp(real_jong, L"ㅆ") == 0) jong = 20;
    else if (wcscmp(real_jong, L"ㅇ") == 0) jong = 21;
    else if (wcscmp(real_jong, L"ㅈ") == 0) jong = 22;
    else if (wcscmp(real_jong, L"ㅊ") == 0) jong = 23;
    else if (wcscmp(real_jong, L"ㅋ") == 0) jong = 24;
    else if (wcscmp(real_jong, L"ㅌ") == 0) jong = 25;
    else if (wcscmp(real_jong, L"ㅍ") == 0) jong = 26;
    else jong = 27; // ㅎ

    if (wcslen(hangul->chosung) == 0 && wcslen(hangul->jungsung) == 0) {
        return compat_jong[jong];
    }

    return 44032 + cho * 588 + jung * 28 + jong;
}

void hangul_check_double(const wchar_t *jong, const wchar_t *jong2, wchar_t *result) {
    result[0] = 0;

    if (wcscmp(jong, L"ㄱ") == 0) {
        if (wcscmp(jong2, L"ㅅ") == 0) wcscpy(result, L"ㄳ");
    } else if (wcscmp(jong, L"ㄴ") == 0) {
        if (wcscmp(jong2, L"ㅈ") == 0) wcscpy(result, L"ㄵ");
        else if (wcscmp(jong2, L"ㅎ") == 0) wcscpy(result, L"ㄶ");
    } else if (wcscmp(jong, L"ㄹ") == 0) {
        if (wcscmp(jong2, L"ㄱ") == 0) wcscpy(result, L"ㄺ");
        else if (wcscmp(jong2, L"ㅁ") == 0) wcscpy(result, L"ㄻ");
        else if (wcscmp(jong2, L"ㅂ") == 0) wcscpy(result, L"ㄼ");
        else if (wcscmp(jong2, L"ㅅ") == 0) wcscpy(result, L"ㄽ");
        else if (wcscmp(jong2, L"ㅌ") == 0) wcscpy(result, L"ㄾ");
        else if (wcscmp(jong2, L"ㅍ") == 0) wcscpy(result, L"ㄿ");
        else if (wcscmp(jong2, L"ㅎ") == 0) wcscpy(result, L"ㅀ");
    } else if (wcscmp(jong, L"ㅂ") == 0) {
        if (wcscmp(jong2, L"ㅅ") == 0) wcscpy(result, L"ㅄ");
    }
}

void hangul_make(ChunjiinState *state, int input) {
    HangulState *hangul = &state->hangul;
    wchar_t beforedata[16] = {0};
    wchar_t nowdata[16] = {0};
    wchar_t overdata[16] = {0};

    if (input == 10) { // Space
        if (hangul->flag_writing) {
            hangul_init(hangul);
        } else {
            hangul->flag_space = true;
        }
    } else if (input == 11) { // Delete
        if (hangul->step == 0) {
            if (wcslen(hangul->chosung) == 0) {
                input_delete_char(state);
                hangul->flag_writing = false;
            } else {
                hangul->chosung[0] = 0;
            }
        } else if (hangul->step == 1) {
            if (wcscmp(hangul->jungsung, L"·") == 0 || wcscmp(hangul->jungsung, L"‥") == 0) {
                input_delete_char(state);
                if (wcslen(hangul->chosung) == 0) {
                    hangul->flag_writing = false;
                }
            }
            hangul->jungsung[0] = 0;
            hangul->step = 0;
        } else if (hangul->step == 2) {
            hangul->jongsung[0] = 0;
            hangul->step = 1;
        } else if (hangul->step == 3) {
            hangul->jongsung2[0] = 0;
            hangul->step = 2;
        }
    } else if (input == 1 || input == 2 || input == 3) { // Vowels
        bool batchim = false;
        if (hangul->step == 2) {
            input_delete_char(state);
            wchar_t s[16];
            wcscpy(s, hangul->jongsung);
            // Bug fixed, 16.4.22
            if (!hangul->flag_doubled) {
                hangul->jongsung[0] = 0;
                hangul->flag_writing = false;
                hangul_write(state);
            }
            hangul_init(hangul);
            wcscpy(hangul->chosung, s);
            hangul->step = 0;
            batchim = true;
        } else if (hangul->step == 3) {
            wchar_t s[16];
            wcscpy(s, hangul->jongsung2);
            if (hangul->flag_doubled) {
                input_delete_char(state);
            } else {
                input_delete_char(state);
                hangul->jongsung2[0] = 0;
                hangul->flag_writing = false;
                hangul_write(state);
            }
            hangul_init(hangul);
            wcscpy(hangul->chosung, s);
            hangul->step = 0;
            batchim = true;
        }

        wcscpy(beforedata, hangul->jungsung);
        hangul->step = 1;

        if (input == 1) { // ㅣ ㅓ ㅕ ㅐ ㅔ ㅖㅒ ㅚ ㅟ ㅙ ㅝ ㅞ ㅢ
            if (wcslen(beforedata) == 0) {
                wcscpy(nowdata, L"ㅣ");
            } else if (wcscmp(beforedata, L"·") == 0) {
                wcscpy(nowdata, L"ㅓ");
                hangul->flag_dotused = true;
            } else if (wcscmp(beforedata, L"‥") == 0) {
                wcscpy(nowdata, L"ㅕ");
                hangul->flag_dotused = true;
            } else if (wcscmp(beforedata, L"ㅏ") == 0) {
                wcscpy(nowdata, L"ㅐ");
            } else if (wcscmp(beforedata, L"ㅑ") == 0) {
                wcscpy(nowdata, L"ㅒ");
            } else if (wcscmp(beforedata, L"ㅓ") == 0) {
                wcscpy(nowdata, L"ㅔ");
            } else if (wcscmp(beforedata, L"ㅕ") == 0) {
                wcscpy(nowdata, L"ㅖ");
            } else if (wcscmp(beforedata, L"ㅗ") == 0) {
                wcscpy(nowdata, L"ㅚ");
            } else if (wcscmp(beforedata, L"ㅜ") == 0) {
                wcscpy(nowdata, L"ㅟ");
            } else if (wcscmp(beforedata, L"ㅠ") == 0) {
                wcscpy(nowdata, L"ㅝ");
            } else if (wcscmp(beforedata, L"ㅘ") == 0) {
                wcscpy(nowdata, L"ㅙ");
            } else if (wcscmp(beforedata, L"ㅝ") == 0) {
                wcscpy(nowdata, L"ㅞ");
            } else if (wcscmp(beforedata, L"ㅡ") == 0) {
                wcscpy(nowdata, L"ㅢ");
            } else {
                hangul_init(hangul);
                hangul->step = 1;
                wcscpy(nowdata, L"ㅣ");
            }
        } else if (input == 2) { // ·,‥,ㅏ,ㅑ,ㅜ,ㅠ,ㅘ
            if (wcslen(beforedata) == 0) {
                wcscpy(nowdata, L"·");
                if (batchim) {
                    hangul->flag_addcursor = true;
                }
            } else if (wcscmp(beforedata, L"·") == 0) {
                wcscpy(nowdata, L"‥");
                hangul->flag_dotused = true;
            } else if (wcscmp(beforedata, L"‥") == 0) {
                wcscpy(nowdata, L"·");
                hangul->flag_dotused = true;
            } else if (wcscmp(beforedata, L"ㅣ") == 0) {
                wcscpy(nowdata, L"ㅏ");
            } else if (wcscmp(beforedata, L"ㅏ") == 0) {
                wcscpy(nowdata, L"ㅑ");
            } else if (wcscmp(beforedata, L"ㅡ") == 0) {
                wcscpy(nowdata, L"ㅜ");
            } else if (wcscmp(beforedata, L"ㅜ") == 0) {
                wcscpy(nowdata, L"ㅠ");
            } else if (wcscmp(beforedata, L"ㅚ") == 0) {
                wcscpy(nowdata, L"ㅘ");
            } else {
                hangul_init(hangul);
                hangul->step = 1;
                wcscpy(nowdata, L"·");
            }
        } else if (input == 3) { // ㅡ, ㅗ, ㅛ
            if (wcslen(beforedata) == 0) {
                wcscpy(nowdata, L"ㅡ");
            } else if (wcscmp(beforedata, L"·") == 0) {
                wcscpy(nowdata, L"ㅗ");
                hangul->flag_dotused = true;
            } else if (wcscmp(beforedata, L"‥") == 0) {
                wcscpy(nowdata, L"ㅛ");
                hangul->flag_dotused = true;
            } else {
                hangul_init(hangul);
                hangul->step = 1;
                wcscpy(nowdata, L"ㅡ");
            }
        }
        wcscpy(hangul->jungsung, nowdata);
    } else { // Consonants
        if (hangul->step == 1) {
            if (wcscmp(hangul->jungsung, L"·") == 0 || wcscmp(hangul->jungsung, L"‥") == 0) {
                hangul_init(hangul);
            } else {
                hangul->step = 2;
            }
        }

        if (hangul->step == 0) {
            wcscpy(beforedata, hangul->chosung);
        } else if (hangul->step == 2) {
            wcscpy(beforedata, hangul->jongsung);
        } else if (hangul->step == 3) {
            wcscpy(beforedata, hangul->jongsung2);
        }

        if (input == 4) { // ㄱ, ㅋ, ㄲ, ㄺ
            if (wcslen(beforedata) == 0) {
                if (hangul->step == 2) {
                    if (wcslen(hangul->chosung) == 0) {
                        wcscpy(overdata, L"ㄱ");
                    } else {
                        wcscpy(nowdata, L"ㄱ");
                    }
                } else {
                    wcscpy(nowdata, L"ㄱ");
                }
            } else if (wcscmp(beforedata, L"ㄱ") == 0) {
                wcscpy(nowdata, L"ㅋ");
            } else if (wcscmp(beforedata, L"ㅋ") == 0) {
                wcscpy(nowdata, L"ㄲ");
            } else if (wcscmp(beforedata, L"ㄲ") == 0) {
                wcscpy(nowdata, L"ㄱ");
            } else if (wcscmp(beforedata, L"ㄹ") == 0 && hangul->step == 2) {
                hangul->step = 3;
                wcscpy(nowdata, L"ㄱ");
            } else {
                wcscpy(overdata, L"ㄱ");
            }
        } else if (input == 5) { // ㄴ ㄹ
            if (wcslen(beforedata) == 0) {
                if (hangul->step == 2) {
                    if (wcslen(hangul->chosung) == 0) {
                        wcscpy(overdata, L"ㄴ");
                    } else {
                        wcscpy(nowdata, L"ㄴ");
                    }
                } else {
                    wcscpy(nowdata, L"ㄴ");
                }
            } else if (wcscmp(beforedata, L"ㄴ") == 0) {
                wcscpy(nowdata, L"ㄹ");
            } else if (wcscmp(beforedata, L"ㄹ") == 0) {
                wcscpy(nowdata, L"ㄴ");
            } else {
                wcscpy(overdata, L"ㄴ");
            }
        } else if (input == 6) { // ㄷ, ㅌ, ㄸ, ㄾ
            if (wcslen(beforedata) == 0) {
                if (hangul->step == 2) {
                    if (wcslen(hangul->chosung) == 0) {
                        wcscpy(overdata, L"ㄷ");
                    } else {
                        wcscpy(nowdata, L"ㄷ");
                    }
                } else {
                    wcscpy(nowdata, L"ㄷ");
                }
            } else if (wcscmp(beforedata, L"ㄷ") == 0) {
                wcscpy(nowdata, L"ㅌ");
            } else if (wcscmp(beforedata, L"ㅌ") == 0) {
                wcscpy(nowdata, L"ㄸ");
            } else if (wcscmp(beforedata, L"ㄸ") == 0) {
                wcscpy(nowdata, L"ㄷ");
            } else if (wcscmp(beforedata, L"ㄹ") == 0 && hangul->step == 2) {
                hangul->step = 3;
                wcscpy(nowdata, L"ㄷ");
            } else {
                wcscpy(overdata, L"ㄷ");
            }
        } else if (input == 7) { // ㅂ, ㅍ, ㅃ, ㄼ, ㄿ
            if (wcslen(beforedata) == 0) {
                if (hangul->step == 2) {
                    if (wcslen(hangul->chosung) == 0) {
                        wcscpy(overdata, L"ㅂ");
                    } else {
                        wcscpy(nowdata, L"ㅂ");
                    }
                } else {
                    wcscpy(nowdata, L"ㅂ");
                }
            } else if (wcscmp(beforedata, L"ㅂ") == 0) {
                wcscpy(nowdata, L"ㅍ");
            } else if (wcscmp(beforedata, L"ㅍ") == 0) {
                wcscpy(nowdata, L"ㅃ");
            } else if (wcscmp(beforedata, L"ㅃ") == 0) {
                wcscpy(nowdata, L"ㅂ");
            } else if (wcscmp(beforedata, L"ㄹ") == 0 && hangul->step == 2) {
                hangul->step = 3;
                wcscpy(nowdata, L"ㅂ");
            } else {
                wcscpy(overdata, L"ㅂ");
            }
        } else if (input == 8) { // ㅅ, ㅎ, ㅆ, ㄳ, ㄶ, ㄽ, ㅀ, ㅄ
            if (wcslen(beforedata) == 0) {
                if (hangul->step == 2) {
                    if (wcslen(hangul->chosung) == 0) {
                        wcscpy(overdata, L"ㅅ");
                    } else {
                        wcscpy(nowdata, L"ㅅ");
                    }
                } else {
                    wcscpy(nowdata, L"ㅅ");
                }
            } else if (wcscmp(beforedata, L"ㅅ") == 0) {
                wcscpy(nowdata, L"ㅎ");
            } else if (wcscmp(beforedata, L"ㅎ") == 0) {
                wcscpy(nowdata, L"ㅆ");
            } else if (wcscmp(beforedata, L"ㅆ") == 0) {
                wcscpy(nowdata, L"ㅅ");
            } else if (wcscmp(beforedata, L"ㄱ") == 0 && hangul->step == 2) {
                hangul->step = 3;
                wcscpy(nowdata, L"ㅅ");
            } else if (wcscmp(beforedata, L"ㄴ") == 0 && hangul->step == 2) {
                hangul->step = 3;
                wcscpy(nowdata, L"ㅅ");
            } else if (wcscmp(beforedata, L"ㄹ") == 0 && hangul->step == 2) {
                hangul->step = 3;
                wcscpy(nowdata, L"ㅅ");
            } else if (wcscmp(beforedata, L"ㅂ") == 0 && hangul->step == 2) {
                hangul->step = 3;
                wcscpy(nowdata, L"ㅅ");
            } else {
                wcscpy(overdata, L"ㅅ");
            }
        } else if (input == 9) { // ㅈ, ㅊ, ㅉ, ㄵ
            if (wcslen(beforedata) == 0) {
                if (hangul->step == 2) {
                    if (wcslen(hangul->chosung) == 0) {
                        wcscpy(overdata, L"ㅈ");
                    } else {
                        wcscpy(nowdata, L"ㅈ");
                    }
                } else {
                    wcscpy(nowdata, L"ㅈ");
                }
            } else if (wcscmp(beforedata, L"ㅈ") == 0) {
                wcscpy(nowdata, L"ㅊ");
            } else if (wcscmp(beforedata, L"ㅊ") == 0) {
                wcscpy(nowdata, L"ㅉ");
            } else if (wcscmp(beforedata, L"ㅉ") == 0) {
                wcscpy(nowdata, L"ㅈ");
            } else if (wcscmp(beforedata, L"ㄴ") == 0 && hangul->step == 2) {
                hangul->step = 3;
                wcscpy(nowdata, L"ㅈ");
            } else {
                wcscpy(overdata, L"ㅈ");
            }
        } else if (input == 0) { // ㅇ, ㅁ, ㄻ
            if (wcslen(beforedata) == 0) {
                if (hangul->step == 2) {
                    if (wcslen(hangul->chosung) == 0) {
                        wcscpy(overdata, L"ㅇ");
                    } else {
                        wcscpy(nowdata, L"ㅇ");
                    }
                } else {
                    wcscpy(nowdata, L"ㅇ");
                }
            } else if (wcscmp(beforedata, L"ㅇ") == 0) {
                wcscpy(nowdata, L"ㅁ");
            } else if (wcscmp(beforedata, L"ㅁ") == 0) {
                wcscpy(nowdata, L"ㅇ");
            } else if (wcscmp(beforedata, L"ㄹ") == 0 && hangul->step == 2) {
                hangul->step = 3;
                wcscpy(nowdata, L"ㅇ");
            } else {
                wcscpy(overdata, L"ㅇ");
            }
        }

        if (wcslen(nowdata) > 0) {
            if (hangul->step == 0) {
                wcscpy(hangul->chosung, nowdata);
            } else if (hangul->step == 2) {
                wcscpy(hangul->jongsung, nowdata);
            } else { // if (hangul->step == 3)
                wcscpy(hangul->jongsung2, nowdata);
            }
        }
        if (wcslen(overdata) > 0) {
            hangul->flag_writing = false;
            hangul_init(hangul);
            wcscpy(hangul->chosung, overdata);
        }
    }
}

void hangul_write(ChunjiinState *state) {
    HangulState *hangul = &state->hangul;
    int position = state->cursor_pos;
    wchar_t str[MAX_TEXT_LEN] = {0};
    wchar_t real_jongsung[16] = {0};

    bool dotflag = false;
    bool doubleflag = false;
    bool spaceflag = false;
    bool impossiblejongsungflag = false;
    wchar_t unicode;

    // Check for double jongsung
    hangul_check_double(hangul->jongsung, hangul->jongsung2, real_jongsung);
    if (wcslen(real_jongsung) == 0) {
        wcscpy(real_jongsung, hangul->jongsung);
        if (wcslen(hangul->jongsung2) != 0) {
            doubleflag = true;
        }
    }

    // Bug fixed, 16.4.22: added impossible jongsungflag
    if (wcscmp(hangul->jongsung, L"ㅃ") == 0 ||
        wcscmp(hangul->jongsung, L"ㅉ") == 0 ||
        wcscmp(hangul->jongsung, L"ㄸ") == 0) {
        doubleflag = true;
        impossiblejongsungflag = true;
        unicode = (wchar_t)hangul_get_unicode(hangul, L"");
    } else {
        unicode = (wchar_t)hangul_get_unicode(hangul, real_jongsung);
    }

    // Build the string before cursor
    if (!hangul->flag_writing) {
        wcsncpy(str, state->text_buffer, position);
        str[position] = 0;
    } else if (hangul->flag_dotused) {
        if (wcslen(hangul->chosung) == 0) {
            wcsncpy(str, state->text_buffer, position - 1);
            str[position - 1] = 0;
        } else {
            wcsncpy(str, state->text_buffer, position - 2);
            str[position - 2] = 0;
        }
    } else if (hangul->flag_doubled) {
        wcsncpy(str, state->text_buffer, position - 2);
        str[position - 2] = 0;
    } else {
        wcsncpy(str, state->text_buffer, position - 1);
        str[position - 1] = 0;
    }

    // Add the unicode character
    if (unicode != 0) {
        size_t len = wcslen(str);
        str[len] = unicode;
        str[len + 1] = 0;
    }

    // Add space if needed
    if (hangul->flag_space) {
        wcscat(str, L" ");
        hangul->flag_space = false;
        spaceflag = true;
    }

    // Add double jongsung if needed
    if (doubleflag) {
        if (impossiblejongsungflag) {
            wcscat(str, hangul->jongsung);
        } else {
            wcscat(str, hangul->jongsung2);
        }
    }

    // Add dot if jungsung is dot
    if (wcscmp(hangul->jungsung, L"·") == 0) {
        wcscat(str, L"·");
        dotflag = true;
    } else if (wcscmp(hangul->jungsung, L"‥") == 0) {
        wcscat(str, L"‥");
        dotflag = true;
    }

    // Add the rest of the text after cursor
    wcscat(str, &state->text_buffer[position]);

    // Copy back to text buffer
    wcscpy(state->text_buffer, str);

    // Adjust cursor position
    if (dotflag) {
        position++;
    }
    if (doubleflag) {
        if (!hangul->flag_doubled) {
            position++;
        }
        hangul->flag_doubled = true;
    } else {
        if (hangul->flag_doubled) {
            position--;
        }
        hangul->flag_doubled = false;
    }
    if (spaceflag) {
        position++;
    }
    if (unicode == 0 && dotflag == false) {
        position--;
    }
    if (hangul->flag_addcursor) {
        hangul->flag_addcursor = false;
        position++;
    }

    // Set final cursor position
    if (hangul->flag_dotused) {
        if (wcslen(hangul->chosung) == 0 && dotflag == false) {
            state->cursor_pos = position;
        } else {
            state->cursor_pos = position - 1;
        }
    } else if (!hangul->flag_writing && dotflag == false) {
        state->cursor_pos = position + 1;
    } else {
        state->cursor_pos = position;
    }

    hangul->flag_dotused = false;
    hangul->flag_writing = (unicode == 0 && dotflag == false) ? false : true;
}
