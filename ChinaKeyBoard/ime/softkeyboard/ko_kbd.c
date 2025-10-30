/*
** ko_kbd.c: Korean QWERTY soft keyboard implementation
**
** Copyright (C) 2024 Korean IME Implementation
**
** Create date: 2024/10/29
**
*/

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include "softkeyboard.h"
#include "../libime/ime_korean.h"
#include "printlog.h"
#include "../properties.h"
#ifdef KBD_TOOLTIP
#include "tooltip.h"  
#endif

#define KOREAN26_KBD_NUM 1

static char fontname[32] = {0};
static void* handleCfg = NULL;
/* Korean keyboard shift state */
static int korean_shift_state = 0; /* 0 = normal, 1 = shifted */

/* Korean character display mapping for keys */
static const char* korean_key_chars[] = {
    /* First row QWERTY -> Korean */
    "ㅂ", "ㅈ", "ㄷ", "ㄱ", "ㅅ", "ㅛ", "ㅕ", "ㅑ", "ㅐ", "ㅔ",
    /* Second row ASDF -> Korean */ 
    "ㅁ", "ㄴ", "ㅇ", "ㄹ", "ㅎ", "ㅗ", "ㅓ", "ㅏ", "ㅣ",
    /* Third row ZXCV -> Korean */
    "Shift", "ㅋ", "ㅌ", "ㅊ", "ㅍ", "ㅠ", "ㅜ", "ㅡ", "←",
    /* Fourth row */
    "한/영", "스페이스", "엔터"
};

/* Korean IME definition */
static md_ime_t korean_ime = 
{
    300,
    "korean_hangul",
    cb_hangul_match_keystrokes,
    cb_hangul_compose,
    NULL
};

#define  KEY_MOVE_X	6
#define  KEY_MOVE_Y	10

/* Forward declarations */
static void korean_key_update(md_key_t *mk, HWND hWnd);
static int ShowKoreanToolTip(HWND hwnd, int x, int y, const char* korean_text);
static char get_ime_char_for_scancode(int scan_code);

/* Korean QWERTY keyboard layout */
#if SOFTKBD_800_1280
static md_key_t korean_key_pads[] = {
/* First row: ㅂㅈㄷㄱㅅㅛㅕㅑㅐㅔ */
{ {0,KEY_ROW1_TY-KEY_MOVE_Y,73+KEY_MOVE_X,KEY_ROW1_BY+KEY_MOVE_Y}	,'q', SCANCODE_Q,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {86-KEY_MOVE_X,KEY_ROW1_TY-KEY_MOVE_Y,153+KEY_MOVE_X,KEY_ROW1_BY+KEY_MOVE_Y}	,'w', SCANCODE_W,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {166-KEY_MOVE_X,KEY_ROW1_TY-KEY_MOVE_Y,233+KEY_MOVE_X,KEY_ROW1_BY+KEY_MOVE_Y}	,'e', SCANCODE_E,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {246-KEY_MOVE_X,KEY_ROW1_TY-KEY_MOVE_Y,313+KEY_MOVE_X,KEY_ROW1_BY+KEY_MOVE_Y}	,'r', SCANCODE_R,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {326-KEY_MOVE_X,KEY_ROW1_TY-KEY_MOVE_Y,393+KEY_MOVE_X,KEY_ROW1_BY+KEY_MOVE_Y}	,'t', SCANCODE_T,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {406-KEY_MOVE_X,KEY_ROW1_TY-KEY_MOVE_Y,473+KEY_MOVE_X,KEY_ROW1_BY+KEY_MOVE_Y}	,'y', SCANCODE_Y,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {486-KEY_MOVE_X,KEY_ROW1_TY-KEY_MOVE_Y,553+KEY_MOVE_X,KEY_ROW1_BY+KEY_MOVE_Y}	,'u', SCANCODE_U,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {566-KEY_MOVE_X,KEY_ROW1_TY-KEY_MOVE_Y,633+KEY_MOVE_X,KEY_ROW1_BY+KEY_MOVE_Y}	,'i', SCANCODE_I,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {646-KEY_MOVE_X,KEY_ROW1_TY-KEY_MOVE_Y,713+KEY_MOVE_X,KEY_ROW1_BY+KEY_MOVE_Y}	,'o', SCANCODE_O,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {726-KEY_MOVE_X,KEY_ROW1_TY-KEY_MOVE_Y,800,KEY_ROW1_BY+KEY_MOVE_Y}	,'p', SCANCODE_P,         KEY_PAD_CHAR, NULL, korean_key_update},

/* Second row: ㅁㄴㅇㄹㅎㅗㅓㅏㅣ */
{ {20-KEY_MOVE_X,KEY_ROW2_TY-KEY_MOVE_Y,113+KEY_MOVE_X,KEY_ROW2_BY+KEY_MOVE_Y}	,'a', SCANCODE_A,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {126-KEY_MOVE_X,KEY_ROW2_TY-KEY_MOVE_Y,193+KEY_MOVE_X,KEY_ROW2_BY+KEY_MOVE_Y}	,'s', SCANCODE_S,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {206-KEY_MOVE_X,KEY_ROW2_TY-KEY_MOVE_Y,273+KEY_MOVE_X,KEY_ROW2_BY+KEY_MOVE_Y}	,'d', SCANCODE_D,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {286-KEY_MOVE_X,KEY_ROW2_TY-KEY_MOVE_Y,353+KEY_MOVE_X,KEY_ROW2_BY+KEY_MOVE_Y}	,'f', SCANCODE_F,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {366-KEY_MOVE_X,KEY_ROW2_TY-KEY_MOVE_Y,433+KEY_MOVE_X,KEY_ROW2_BY+KEY_MOVE_Y}	,'g', SCANCODE_G,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {446-KEY_MOVE_X,KEY_ROW2_TY-KEY_MOVE_Y,513+KEY_MOVE_X,KEY_ROW2_BY+KEY_MOVE_Y}	,'h', SCANCODE_H,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {526-KEY_MOVE_X,KEY_ROW2_TY-KEY_MOVE_Y,593+KEY_MOVE_X,KEY_ROW2_BY+KEY_MOVE_Y}	,'j', SCANCODE_J,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {606-KEY_MOVE_X,KEY_ROW2_TY-KEY_MOVE_Y,673+KEY_MOVE_X,KEY_ROW2_BY+KEY_MOVE_Y}	,'k', SCANCODE_K,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {686-KEY_MOVE_X,KEY_ROW2_TY-KEY_MOVE_Y,753+KEY_MOVE_X,KEY_ROW2_BY+KEY_MOVE_Y}	,'l', SCANCODE_L,         KEY_PAD_CHAR, NULL, korean_key_update},

/* Third row: Shift, ㅋㅌㅊㅍㅠㅜㅡ */
{ {0,KEY_ROW3_TY-KEY_MOVE_Y,96+KEY_MOVE_X,KEY_ROW3_BY+KEY_MOVE_Y}	,' ', SCANCODE_LEFTSHIFT, KEY_PAD_FUNC, NULL, korean_key_update},
{ {126-KEY_MOVE_X,KEY_ROW3_TY-KEY_MOVE_Y,193+KEY_MOVE_X,KEY_ROW3_BY+KEY_MOVE_Y}	,'z', SCANCODE_Z,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {206-KEY_MOVE_X,KEY_ROW3_TY-KEY_MOVE_Y,273+KEY_MOVE_X,KEY_ROW3_BY+KEY_MOVE_Y}	,'x', SCANCODE_X,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {286-KEY_MOVE_X,KEY_ROW3_TY-KEY_MOVE_Y,353+KEY_MOVE_X,KEY_ROW3_BY+KEY_MOVE_Y}	,'c', SCANCODE_C,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {366-KEY_MOVE_X,KEY_ROW3_TY-KEY_MOVE_Y,433+KEY_MOVE_X,KEY_ROW3_BY+KEY_MOVE_Y}	,'v', SCANCODE_V,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {446-KEY_MOVE_X,KEY_ROW3_TY-KEY_MOVE_Y,513+KEY_MOVE_X,KEY_ROW3_BY+KEY_MOVE_Y}	,'b', SCANCODE_B,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {526-KEY_MOVE_X,KEY_ROW3_TY-KEY_MOVE_Y,593+KEY_MOVE_X,KEY_ROW3_BY+KEY_MOVE_Y}	,'n', SCANCODE_N,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {606-KEY_MOVE_X,KEY_ROW3_TY-KEY_MOVE_Y,673+KEY_MOVE_X,KEY_ROW3_BY+KEY_MOVE_Y}	,'m', SCANCODE_M,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {686-KEY_MOVE_X,KEY_ROW3_TY-KEY_MOVE_Y,800,KEY_ROW3_BY+KEY_MOVE_Y}	,8, SCANCODE_BACKSPACE,     KEY_PAD_FUNC, NULL, korean_key_update},

/* Fourth row: Korean/English switch, Space, Enter */
{ {0,KEY_ROW4_TY-KEY_MOVE_Y,113+KEY_MOVE_X,KEY_ROW4_BY+KEY_MOVE_Y}	,' ', SCANCODE_TOKOREAN,   KEY_PAD_FUNC, NULL, korean_key_update},
{ {126-KEY_MOVE_X,KEY_ROW4_TY-KEY_MOVE_Y,593+KEY_MOVE_X,KEY_ROW4_BY+KEY_MOVE_Y}	,' ', SCANCODE_SPACE,      KEY_PAD_CHAR, NULL, korean_key_update},
{ {606-KEY_MOVE_X,KEY_ROW4_TY-KEY_MOVE_Y,800,KEY_ROW4_BY+KEY_MOVE_Y}	,'\n', SCANCODE_ENTER,      KEY_PAD_FUNC, NULL, korean_key_update},
};

static int korean_key_pads_num = sizeof(korean_key_pads) / sizeof(md_key_t);

/* For other screen sizes - use simplified layout */
#else
static md_key_t korean_key_pads[] = {
/* Simplified Korean QWERTY for smaller screens */
{ {5,130,75,170}	,'q', SCANCODE_Q,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {85,130,155,170}	,'w', SCANCODE_W,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {165,130,235,170}	,'e', SCANCODE_E,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {245,130,315,170}	,'r', SCANCODE_R,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {325,130,395,170}	,'t', SCANCODE_T,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {405,130,475,170}	,'y', SCANCODE_Y,         KEY_PAD_CHAR, NULL, korean_key_update},

{ {25,180,95,220}	,'a', SCANCODE_A,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {105,180,175,220}	,'s', SCANCODE_S,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {185,180,255,220}	,'d', SCANCODE_D,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {265,180,335,220}	,'f', SCANCODE_F,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {345,180,415,220}	,'g', SCANCODE_G,         KEY_PAD_CHAR, NULL, korean_key_update},

{ {45,230,115,270}	,'z', SCANCODE_Z,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {125,230,195,270}	,'x', SCANCODE_X,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {205,230,275,270}	,'c', SCANCODE_C,         KEY_PAD_CHAR, NULL, korean_key_update},
{ {285,230,355,270}	,'v', SCANCODE_V,         KEY_PAD_CHAR, NULL, korean_key_update},

{ {5,280,95,320}	,' ', SCANCODE_TOKOREAN,   KEY_PAD_FUNC, NULL, korean_key_update},
{ {105,280,315,320}	,' ', SCANCODE_SPACE,      KEY_PAD_CHAR, NULL, korean_key_update},
{ {325,280,415,320}	,8, SCANCODE_BACKSPACE,     KEY_PAD_FUNC, NULL, korean_key_update},
{ {425,280,475,320}	,'\n', SCANCODE_ENTER,      KEY_PAD_FUNC, NULL, korean_key_update},
};

static int korean_key_pads_num = sizeof(korean_key_pads) / sizeof(md_key_t);
#endif

/* Get Korean character string for display based on IME mapping */
static const char* get_korean_char_for_key(int scan_code) 
{
    /* Map to match Korean IME expectations - using standard Korean QWERTY layout */
    switch (scan_code) {
        /* Korean Consonants (matching ime_korean.c mapping) */
        case SCANCODE_R: return "ㄱ";  /* r -> ㄱ */
        case SCANCODE_S: return "ㄴ";  /* s -> ㄴ */
        case SCANCODE_E: return "ㄷ";  /* e -> ㄷ */
        case SCANCODE_F: return "ㄹ";  /* f -> ㄹ */
        case SCANCODE_A: return "ㅁ";  /* a -> ㅁ */
        case SCANCODE_Q: return "ㅂ";  /* q -> ㅂ */
        case SCANCODE_T: return "ㅅ";  /* t -> ㅅ */
        case SCANCODE_D: return "ㅇ";  /* d -> ㅇ */
        case SCANCODE_W: return "ㅈ";  /* w -> ㅈ */
        case SCANCODE_C: return "ㅊ";  /* c -> ㅊ */
        case SCANCODE_Z: return "ㅋ";  /* z -> ㅋ */
        case SCANCODE_X: return "ㅌ";  /* x -> ㅌ */
        case SCANCODE_V: return "ㅍ";  /* v -> ㅍ */
        case SCANCODE_G: return "ㅎ";  /* g -> ㅎ */
        
        /* Korean Vowels (matching ime_korean.c mapping) */
        case SCANCODE_K: return "ㅏ";  /* k -> ㅏ */
        case SCANCODE_I: return "ㅑ";  /* i -> ㅑ */
        case SCANCODE_J: return "ㅔ";  /* j -> ㅔ */
        case SCANCODE_P: return "ㅕ";  /* p -> ㅕ */
        case SCANCODE_H: return "ㅗ";  /* h -> ㅗ (different from display, but matches IME) */
        case SCANCODE_Y: return "ㅛ";  /* y -> ㅛ (not standard but keeping) */
        case SCANCODE_N: return "ㅚ";  /* n -> ㅚ */
        case SCANCODE_U: return "ㅖ";  /* u -> ㅖ */
        case SCANCODE_B: return "ㅛ";  /* b -> ㅛ */
        case SCANCODE_M: return "ㅜ";  /* m -> ㅜ */
        case SCANCODE_L: return "ㅣ";  /* l -> ㅣ */
        
        /* Double consonants for shift */
        case SCANCODE_R | 0x100: return korean_shift_state ? "ㄲ" : "ㄱ";  /* R -> ㄲ */
        case SCANCODE_E | 0x100: return korean_shift_state ? "ㄸ" : "ㄷ";  /* E -> ㄸ */
        case SCANCODE_Q | 0x100: return korean_shift_state ? "ㅃ" : "ㅂ";  /* Q -> ㅃ */
        case SCANCODE_T | 0x100: return korean_shift_state ? "ㅆ" : "ㅅ";  /* T -> ㅆ */
        case SCANCODE_W | 0x100: return korean_shift_state ? "ㅉ" : "ㅈ";  /* W -> ㅉ */
        
        /* Alternative vowels for shift */
        case SCANCODE_O: return korean_shift_state ? "ㅒ" : "ㅐ";  /* o/O -> ㅐ/ㅒ */
        
        /* Special keys */
        case SCANCODE_LEFTSHIFT: return "⇧";
        case SCANCODE_BACKSPACE: return "⌫";
        case SCANCODE_TOKOREAN: return "한/영";
        case SCANCODE_SPACE: return "스페이스";
        case SCANCODE_ENTER: return "엔터";
        
        default: return "?";
    }
}

/* Convert scan code to the ASCII character that the Korean IME expects */
static char get_ime_char_for_scancode(int scan_code)
{
    /* This maps scan codes to the ASCII characters the Korean IME expects */
    switch (scan_code) {
        /* Korean consonant mapping (matches korean_qwerty_consonants in ime_korean.c) */
        case SCANCODE_R: return korean_shift_state ? 'R' : 'r';  /* ㄲ/ㄱ */
        case SCANCODE_S: return 's';  /* ㄴ */
        case SCANCODE_E: return korean_shift_state ? 'E' : 'e';  /* ㄸ/ㄷ */
        case SCANCODE_F: return 'f';  /* ㄹ */
        case SCANCODE_A: return 'a';  /* ㅁ */
        case SCANCODE_Q: return korean_shift_state ? 'Q' : 'q';  /* ㅃ/ㅂ */
        case SCANCODE_T: return korean_shift_state ? 'T' : 't';  /* ㅆ/ㅅ */
        case SCANCODE_D: return 'd';  /* ㅇ */
        case SCANCODE_W: return korean_shift_state ? 'W' : 'w';  /* ㅉ/ㅈ */
        case SCANCODE_C: return 'c';  /* ㅊ */
        case SCANCODE_Z: return 'z';  /* ㅋ */
        case SCANCODE_X: return 'x';  /* ㅌ */
        case SCANCODE_V: return 'v';  /* ㅍ */
        case SCANCODE_G: return 'g';  /* ㅎ */
        
        /* Korean vowel mapping (matches korean_qwerty_vowels in ime_korean.c) */
        case SCANCODE_K: return 'k';  /* ㅏ */
        case SCANCODE_I: return 'i';  /* ㅑ */
        case SCANCODE_O: return korean_shift_state ? 'o' : 'O';  /* ㅐ/ㅒ */
        case SCANCODE_J: return 'j';  /* ㅔ */
        case SCANCODE_P: return 'p';  /* ㅕ */
        case SCANCODE_U: return 'u';  /* ㅖ */
        case SCANCODE_H: return 'h';  /* ㅗ */
        case SCANCODE_Y: return 'y';  /* ㅙ */
        case SCANCODE_N: return 'n';  /* ㅚ */
        case SCANCODE_B: return 'b';  /* ㅛ */
        case SCANCODE_M: return 'm';  /* ㅜ */
        case SCANCODE_L: return 'l';  /* ㅝ */
        
        /* Note: Double consonants handled above with shift state in individual cases */
        
        /* Non-Korean keys pass through */
        case SCANCODE_SPACE: return ' ';
        case SCANCODE_ENTER: return '\n';
        case SCANCODE_BACKSPACE: return '\b';
        
        default: return 0;  /* Invalid/unsupported key */
    }
}

/* Custom Korean key update function that displays Korean characters */
static void korean_key_update(md_key_t *mk, HWND hWnd)
{
    HWND ttw;
    HDC hdc;
    PBITMAP pbmp;
    int x, y;
    const char* korean_char;
    SOFTKBD_DATA* pdata = NULL;

    if (mk->data == NULL) {
        return;
    }

#ifdef KBD_TOOLTIP
    pdata = (SOFTKBD_DATA*)GetWindowAdditionalData(hWnd);
    ttw = (HWND)pdata->tooltip_win;
    
    if (mk->style & KEY_PAD_PRESSED) {
        if ((mk->style & KEY_PAD_CHAR) && (pdata->skb_ime != MGIN_SKB9IME || pdata->current_board_idx > 1)) {
            x = mk->bound.left + RECTW(mk->bound)/2 - tooltip_w/2;
            y = mk->bound.top - tooltip_h;
            
            korean_char = get_korean_char_for_key(mk->scan_code);
            ClientToScreen(hWnd, &x, &y);
            
            /* Show Korean character in tooltip - use custom display */
            ShowKoreanToolTip(ttw, x, y, korean_char);
        } else {
            hdc = GetDC(hWnd);
            pbmp = (PBITMAP)mk->data;
            if (mk->bound.bottom > 208) {
                FillBoxWithBitmap(hdc, mk->bound.left, mk->bound.top, 
                    RECTW(mk->bound), RECTH(mk->bound)-5, pbmp);
            } else {
                FillBoxWithBitmap(hdc, mk->bound.left, mk->bound.top, 
                    RECTW(mk->bound), RECTH(mk->bound), pbmp);
            }
            ReleaseDC(hdc);
        }
    } else {
        HideToolTip(ttw);
        EraseBbGround(hWnd, &mk->bound);
    }
#else
    if (mk->style & KEY_PAD_PRESSED) {
        hdc = GetDC(hWnd);
        pbmp = (PBITMAP)mk->data;
        FillBoxWithBitmap(hdc, mk->bound.left, mk->bound.top, 
                RECTW(mk->bound), RECTH(mk->bound), pbmp);
        ReleaseDC(hdc);
    } else {
        EraseBbGround(hWnd, &mk->bound);
    }
#endif
    return;
}

/* Custom Korean tooltip function */
static int ShowKoreanToolTip(HWND hwnd, int x, int y, const char* korean_text)
{
    MoveWindow(hwnd, x, y, tooltip_w, tooltip_h, FALSE);
    
    /* Store the Korean text for display */
    static char korean_tooltip_str[16] = {0};
    if (korean_text && strlen(korean_text) < sizeof(korean_tooltip_str)) {
        strcpy(korean_tooltip_str, korean_text);
    } else {
        strcpy(korean_tooltip_str, "?");
    }
    
    /* Set window text to Korean character */
    SetWindowText(hwnd, korean_tooltip_str);
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd, TRUE);
    
    return 0;
}

/* Korean keyboard window definition */
static key_window_t korean_key_window = 
{
    {0, 0, 0, 0}, /* bound */
    {0, 0},       /* press */
    NULL,         /* key */
    0,            /* key_num */
    NULL          /* data */
};

/* Korean keyboard definition */
static key_board_t korean_keyboard = 
{
    800,  /* skb_win_w */
    580,  /* skb_win_h */
    {690, 10, 790, 70}, /* close_rc */
    NULL, /* close_normal */
    NULL, /* close_press */
    NULL, /* view_window */
    NULL, /* stroke_window */
    NULL, /* select_window */
    &korean_key_window, /* key_window */
    NULL, /* data */
    NULL, /* update - will be set later */
    &korean_ime, /* ime */
    {AC_NULL, 0, 0, 0, NULL}, /* action */
    NULL, /* proceed_msg - will be set later */
    NULL /* clear - will be set later */
};

/* Korean keyboard message processing */
static int korean_proc_msg(key_board_t* key_board, HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
    md_key_t *key;
    POINT p;
    
    switch (message)
    {
        case MSG_LBUTTONDOWN:
        {
            p.x = LOWORD(lParam);
            p.y = HIWORD(lParam);
            key_board->action.operation = AC_NULL;
            
            if (hit_rect(key_board->key_window->bound, p))
            {
                key = key_board->key_window->get_key(key_board->key_window, p);
                if (key != NULL && key->scan_code == SCANCODE_LEFTSHIFT)
                {
                    /* Toggle shift state */
                    korean_shift_state = !korean_shift_state;
                    printf("Korean shift state: %s\n", korean_shift_state ? "ON (ㅃㅉㄸㄲㅆㅒㅖ)" : "OFF (ㅂㅈㄷㄱㅅㅐㅔ)");
                    
                    /* Update key display to show shift state change */
                    key->update(key, hwnd);
                    
                    /* Trigger a refresh of all key displays to show shifted characters */
                    PostMessage(hwnd, MSG_ERASEBKGND, 0, 0L);
                    return AC_NULL;
                }
            }
            
            if (key_board->action.operation != AC_NULL)
            {
                return key_board->action.operation;
            }
            break;
        }
        case MSG_LBUTTONUP:
        case MSG_MOUSEMOVE:
        {
            POINT p = {LOWORD(lParam), HIWORD(lParam)};
            if (key_board->action.operation != AC_NULL)
            {
                return key_board->action.operation;
            }
            break;
        }
        default:
            break;
    }
    return 0;
}

/* Korean keyboard update function */
static void korean_kb_update(key_board_t* key_board, HWND hwnd, WPARAM wParam, RECT* rect)
{
    if (key_board->data && key_board->key_window)
    {
        /* Draw Korean keyboard background */
        HDC hdc = GetClientDC(hwnd);
        if (key_board->data)
        {
            FillBoxWithBitmap(hdc, 0, 0, 800, 580, (PBITMAP)key_board->data);
        }
        
        /* Update key window */
        key_board->key_window->update(key_board->key_window, hwnd);
        ReleaseDC(hdc);
    }
}

/* Korean keyboard clear function */
static void korean_kb_clear(key_board_t* kb)
{
    if (kb && kb->key_window)
    {
        if (kb->key_window && kb->key_window->data) {
            /* Clear key window data if needed */
        }
    }
    /* Reset Korean composition state */
    cb_hangul_match_keystrokes("", NULL, 0, 0, 0);
}

/* Initialize Korean keyboard */
int init_korean_keyboard(const void* handle, HWND hWnd, key_board_t* keyboard)
{
    PRECT pRect = NULL;
    int valueCfg = 0;
    
    handleCfg = (void*)handle;
    
    /* Configure keyboard dimensions based on screen size */
    if ((NULL != handleCfg) && (pRect = (PRECT) FindMigCfgRectValue(handleCfg, "RECT_SKB_26_CLOSE")) != NULL)
    {
        korean_keyboard.close_rc = *pRect;
    }
    
    if ((NULL != handleCfg) && (valueCfg = FindMigCfgIntValue(handleCfg, "SKB_WIN_H")) >= 0)
    {
        korean_keyboard.skb_win_h = valueCfg;
    }
    
    if ((NULL != handleCfg) && (valueCfg = FindMigCfgIntValue(handleCfg, "SKB_WIN_W")) >= 0)
    {
        korean_keyboard.skb_win_w = valueCfg;
    }
    
    /* Initialize key window */
    korean_key_window.bound.left = 0;
    korean_key_window.bound.top = 80; 
    korean_key_window.bound.right = korean_keyboard.skb_win_w;
    korean_key_window.bound.bottom = korean_keyboard.skb_win_h;
    korean_key_window.key = korean_key_pads;
    korean_key_window.key_num = korean_key_pads_num;
    
    /* Load Korean keyboard bitmap - for now use English bitmap as placeholder */
    if (get_keyboard_bitmap(HDC_SCREEN, (PBITMAP*)(&keyboard->data), EN26_KBD_BKG))
    {
        PrintImeInfo("Failed to get Korean keyboard bitmap.\n");
        return -1;
    }
    
    *keyboard = korean_keyboard;
    keyboard->key_window = &korean_key_window;
    keyboard->update = korean_kb_update;
    keyboard->proceed_msg = korean_proc_msg;
    keyboard->clear = korean_kb_clear;
    
    PrintImeInfo("Korean keyboard initialized successfully.\n");
    return 0;
}

/* Destroy Korean keyboard */
void destroy_korean_keyboard(key_board_t* keyboard)
{
    if (keyboard && keyboard->data)
    {
        /* Release bitmap resources */
        keyboard->data = NULL;
    }
    
    PrintImeInfo("Korean keyboard destroyed.\n");
}
