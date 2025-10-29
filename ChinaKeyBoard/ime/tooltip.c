/*
 * tooltip.c
 * 
 * Create the tooltip window for soft keyboard.
 * 
 * wangjian<wangjian@minigui.org>
 * 
 * 2008-07-23.
 */

#include <stdio.h>
#include <string.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "tooltip.h"
#define LCD_SIZE_320X240		1
#define LCD_SIZE_400X240		2
#define LCD_SIZE_240X320		3
#define LCD_SIZE_480X272		4
#define LCD_SIZE_480X800		5
#define LCD_SIZE_600X1024		6
#define LCD_SIZE_800X1280		7
#define LCD_SIZE_720X1280		8
#define LCD_SIZE_1280X720		9
#define LCD_SIZE_800X480		10


//static BITMAP bmp_f;
//static DEVFONT *dev_font = NULL;
extern char *getSoftkeyboardPublicPath(const char *fileName);

unsigned int tooltip_w = TTW_W;
unsigned int tooltip_h = TTW_H;
unsigned int tooltip_x = 0;
unsigned int tooltip_y = 0;
static int init_ttw_data (HWND hWnd, TTW_PDATA padd)
{              
    int i;
   
    
    //if (get_kbd_mybitmap (&(padd->mask), padd->pal, TTW_MASK_BMP)) {
	if (LoadMyBitmap(&(padd->mask), padd->pal, getSoftkeyboardPublicPath("tooltip_mask.bmp"))) {
        fprintf (stderr, "Fail to load bitmap for tooltip window mask. \n");
        return 1;
    }
   // if (get_kbd_bitmap (HDC_SCREEN, &(padd->bk_bmp), TTW_BK_BMP)) {
	if (LoadBitmap(HDC_SCREEN, &(padd->bk_bmp), getSoftkeyboardPublicPath("tooltip_bk.png"))){
        fprintf (stderr, "Fail to load bitmap for tooltip window BKGND. \n");
        return 1;
    }

#if 0
    InitFreetypeLibrary();

    LoadDevFontFromFile ("ttf-times-rrncnn-0-0-ISO8859-1", "times.ttf");
    
    padd->pfont = CreateLogFont ("ttf", "times", "ISO8859-1", 
                        FONT_WEIGHT_BOOK, FONT_SLANT_ROMAN, 
                        FONT_FLIP_NIL, FONT_OTHER_NIL, 
                        FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 
                        40, 0);
#endif
    if(LCD_SIZE_480X800 <= getLCDSize())
    {
    	padd->pfont = CreateLogFont ("ttf", "SourceHanSansCNMedium", "UTF-8", FONT_WEIGHT_BOOK,
				FONT_SLANT_ROMAN, FONT_FLIP_NIL, FONT_OTHER_NIL, FONT_UNDERLINE_NONE,
				FONT_STRUCKOUT_NONE, 48, 0);
    }
    else
    {  
    	padd->pfont = CreateLogFont ("ttf", "times", "ISO8859-1",
                        FONT_WEIGHT_BOOK, FONT_SLANT_ROMAN, 
                        FONT_FLIP_NIL, FONT_OTHER_NIL, 
                        FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 
                        40, 0);
	} 
/*
    else
    {
		if(LoadBitmap (HDC_SCREEN, &bmp_f, getSoftkeyboardPublicPath("bitmap_font.png")))
	    {
	        fprintf (stderr, "Fail to load bitmap for tooltip window font. \n");
	        return 1;
	    }

	   	dev_font = CreateBMPDevFont ("bmp-iphone-rrncnn-30-33-ISO8859-1", 
	            &bmp_f, "!", 94, 30);
	   		
	   	
	    padd->pfont = CreateLogFont (FONT_TYPE_NAME_BITMAP_BMP, "iphone", "ISO8859-1",
	                                FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, 
	                                FONT_SETWIDTH_NORMAL, FONT_SPACING_CHARCELL, 
	                                FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 
	                                10, 0);
    }
	*/
    for (i = 0; i < TTW_PAL_LEN; ++i) {
        if(padd->pal[i].r == TTW_MASK_R 
                && padd->pal[i].g == TTW_MASK_G 
                &&  padd->pal[i].b == TTW_MASK_B) {
            padd->mask.transparent = i;
            break;
        }
    }

    return 0;
}

static int destroy_ttw_data (HWND hWnd)
{
    TTW_PDATA padd ;

    padd = (TTW_PDATA) GetWindowAdditionalData (hWnd);
    
    UnloadMyBitmap (&(padd->mask));
    UnloadBitmap (&(padd->bk_bmp));
    DestroyLogFont (padd->pfont);
    free(padd);

    return 0;
}

static char str[2];
static int TTWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    TTW_PDATA padd;

    switch (message) 
    {
        case MSG_CREATE:
		{
			str[0] = ' ';
			str[1] = '\0';
#if 0
			padd = (TTW_PDATA) calloc( 1, sizeof (TTW_DATA));

			if( 0 != init_ttw_data (hWnd, padd)){
				fprintf(stderr, "init nr window error!");
				return -1;
			}
#else
			padd = (TTW_PDATA) GetWindowAdditionalData (hWnd);
#endif

			if (!SetWindowMask (hWnd, &padd->mask)){
				fprintf(stderr, "set window mask erro!");
				destroy_ttw_data (hWnd);
				return -1;
			}
#if 0
			SetWindowAdditionalData (hWnd, (DWORD)padd);
#endif
			break;
		}
        case MSG_LBUTTONUP:
		{
			int x_pos = LOSWORD (lParam);
			int y_pos = HISWORD (lParam);
			ClientToScreen (hWnd, &x_pos, &y_pos);
			ScreenToClient (GetHosting(hWnd), &x_pos, &y_pos);
			SendMessage (GetHosting(hWnd), MSG_LBUTTONUP,
					0, MAKELONG (x_pos, y_pos));
			ShowWindow (hWnd, SW_HIDE);
			break;
		}

        case MSG_ERASEBKGND:
		{
			padd = (TTW_PDATA) GetWindowAdditionalData (hWnd);
			if (padd) {
				BOOL fGetDC = FALSE;
				hdc = (HDC)wParam;
				if (!hdc) {
					hdc = GetClientDC (hWnd);
					fGetDC = TRUE;
				}
				FillBoxWithBitmap (hdc, 0, 0, 0, 0, &(padd->bk_bmp));
				if (fGetDC)
					ReleaseDC (hdc);
			}
			return 0;
		}
        case MSG_PAINT:
		{
			padd = (TTW_PDATA) GetWindowAdditionalData (hWnd);

			if (padd) {
				hdc = BeginPaint (hWnd);
				RECT rc = {0, 0, tooltip_w, tooltip_h};

				SetBkMode (hdc, BM_TRANSPARENT);
				SelectFont (hdc, padd->pfont);
				/* space have not bitmap glyph. */
				if (str[0] != 0x20){
					DrawText (hdc, str, -1, &rc, DT_CENTER);

				}
				EndPaint (hWnd, hdc);
			}
			return 0;
		}
        case MSG_SETCHAR:
		{
			str[0] = (char)wParam;
			str[1] = '\0';

			InvalidateRect(hWnd, NULL, TRUE);
			return 0;
		}
        case MSG_CLOSE:
		{
		//	if (dev_font)
		//		DestroyBMPFont (dev_font);
		//	UnloadBitmap (&bmp_f);

			DestroyMainWindow (hWnd);
			PostQuitMessage (hWnd);
		   // destroy_ttw_data (hWnd);
			return 0;
		}
        case MSG_DESTROY:
		{
			destroy_ttw_data (hWnd);
			return 0;
		}
    }
    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

HWND CreateToolTip (HWND host)
{
    HWND hNrWnd;
    MAINWINCREATE CreateInfo;

    TTW_PDATA padd = (TTW_PDATA) calloc( 1, sizeof (TTW_DATA));

	if( 0 != init_ttw_data (host, padd)){
		fprintf(stderr, "init nr window error!");
		return -1;
	}

    //CreateInfo.dwStyle      = WS_VISIBLE;
    CreateInfo.dwStyle      = WS_NONE;
    CreateInfo.dwExStyle    = WS_EX_TOOLWINDOW; //WS_EX_TOPMOST;
    CreateInfo.spCaption    = "Char Tooltip Window";
    CreateInfo.hMenu        = 0;
    CreateInfo.hCursor      = GetSystemCursor(0);
    CreateInfo.hIcon        = 0;
    CreateInfo.MainWindowProc = TTWinProc;
    CreateInfo.lx           = 0;
    CreateInfo.ty           = 0;
    CreateInfo.iBkColor     = PIXEL_lightgray;
    CreateInfo.dwAddData    = (unsigned int)padd;
    CreateInfo.hHosting     = host;

    if(padd )
    {
    	tooltip_w = padd->bk_bmp.bmWidth;
    	tooltip_h = padd->bk_bmp.bmHeight;
    }
    else
    {
		if(LCD_SIZE_800X1280 <= getLCDSize())
		{
			tooltip_w = 90;
			tooltip_h = 128;
		}
		else
		{
			tooltip_w = TTW_W;
			tooltip_h = TTW_H;
		}
    }
    tooltip_x = 0;
    tooltip_y = 0;
	CreateInfo.rx = tooltip_w;
	CreateInfo.by = tooltip_h;

	hNrWnd = CreateMainWindow (&CreateInfo);

    ShowWindow (hNrWnd, SW_HIDE);

    return hNrWnd;
}

int ShowToolTip (HWND hwnd, int x, int y, char key_char)
{
	tooltip_x = x;
	tooltip_y = y;
	MoveWindow (hwnd, x, y, tooltip_w, tooltip_h, FALSE);
	str[0] = (char)key_char;
	str[1] = '\0';
   // SendMessage (hwnd, MSG_SETCHAR, key_char, 0L);
	SendMessage (hwnd, MSG_PAINT, 0, 0L);
    ShowWindow (hwnd, SW_SHOWNORMAL);
    
    return 0;
}

int HideToolTip (HWND hwnd)
{
    ShowWindow (hwnd, SW_HIDE);
}

void UpdateToolTip(HWND hwnd)
{
	SendMessage (hwnd, MSG_PAINT, 0, 0L);
}

