/*
 * tooltip.h
 * tooltip for every key.
 *
 * wangjian<wangjian@minigui.org>
 * 2008-7-24.
 */


//480*272��Ļtip���
#define TTW_W           30
#define TTW_H           42

#define TTW_MASK_R      0xff
#define TTW_MASK_G      0x0
#define TTW_MASK_B      0x0

#define TTW_PAL_LEN     512

#define MSG_SETCHAR     MSG_USER + 10

typedef struct 
{
    /* bk gnd bitmap */
    BITMAP      bk_bmp;
    /* the logfont for showing text */
    PLOGFONT    pfont;
    /* mask for window shape */
    MYBITMAP    mask;
    /* color palette */
    RGB         pal[TTW_PAL_LEN];
} TTW_DATA, *TTW_PDATA;

HWND CreateToolTip (HWND host);

int ShowToolTip (HWND hwnd, int x, int y, char key_char);

int HideToolTip (HWND hwnd);

extern unsigned int tooltip_w;
extern unsigned int tooltip_h;
extern unsigned int tooltip_x;
extern unsigned int tooltip_y;
