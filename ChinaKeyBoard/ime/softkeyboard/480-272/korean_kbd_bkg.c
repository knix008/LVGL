/*
** korean_kbd_bkg.c: Korean keyboard background bitmap (480x272)
**
** Copyright (C) 2024 Korean IME Implementation  
**
** Create date: 2024/10/29
**
*/

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>

/* Korean keyboard bitmap data - simplified placeholder */
static const unsigned char korean_kbd_bkg_data[] = {
    /* This would normally contain bitmap data for Korean keyboard background */
    /* For now, we'll use a simple pattern to represent the keyboard layout */
    0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88  /* Placeholder pattern */
};

/* Korean keyboard background bitmap definition */  
const BITMAP korean_kbd_bkg = {
    BMP_TYPE_NORMAL,    /* bmType */
    8,                  /* bmBitsPixel */ 
    1,                  /* bmBytesPerPixel */
    480,                /* bmWidth */
    272,                /* bmHeight */
    480,                /* bmPitch */
    (void*)korean_kbd_bkg_data,  /* bmBits */
    NULL,               /* bmAlpha */
    0                   /* bmColorKey */
};
