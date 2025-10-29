/*
** $Id: pinyin_kbd.c 432 2008-07-21 09:07:59Z xkwu $
**
** en_kbd.c: This file include english soft keyboard code. 
**
** Copyright (C) 2003 ~ 2008 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** Create date: 2008/07/11
**
*/

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>
#include "softkeyboard.h"
#include "../libime/ime.h"
#include "printlog.h"
#ifdef KBD_TOOLTIP
#include "tooltip.h"
#endif

#define EN26_KBD_NUM 2

extern int en_kb_Index;
static char fontname[32] = {0};
static void* handleCfg = NULL;

static void make_font(void)
{
#if 0
	static DEVFONT *devfont;
	int i;
	int j;


#if (MINIGUI_MAJOR_VERSION > 2)
    InitFreetypeLibrary ();
	devfont = LoadDevFontFromFile("ttf-noname-rrncnn-0-0-ISO8859-1",
			"ttf-fonts/cn.ttf");
#else
	InitFreeTypeFonts();
	LoadDevFontFromFile("ttf-noname-rrncnn-0-0-ISO8859-1",
			"ttf-fonts/en.ttf", &devfont);
#endif

	if(devfont && devfont->name) {
		for(i=0; devfont->name[i] != '-' 
				&& devfont->name[i] != 0; i++);

		for(i++, j=0; devfont->name[i] != '-' 
				&& devfont->name[i] != 0; i++,j++) {
			fontname[j] = devfont->name[i];
		}
	}
#endif
}

static md_ime_t en_ime =
{
    100,
    "mgpti",
    cb_pti_match_keystrokes,
    cb_pti_match_keystrokes,
    NULL
};

#define  KEY_MOVE_X	6
#define  KEY_MOVE_Y	10

#if SOFTKBD_800_1280
static md_key_t en_key_pads[] = {
{ {0,KEY_ROW1_TY-KEY_MOVE_Y,73+KEY_MOVE_X,KEY_ROW1_BY+KEY_MOVE_Y}	,'q', SCANCODE_Q,         KEY_PAD_CHAR, NULL, share_key_update},
{ {86-KEY_MOVE_X,KEY_ROW1_TY-KEY_MOVE_Y,153+KEY_MOVE_X,KEY_ROW1_BY+KEY_MOVE_Y}	,'w', SCANCODE_W,         KEY_PAD_CHAR, NULL, share_key_update},
{ {166-KEY_MOVE_X,KEY_ROW1_TY-KEY_MOVE_Y,233+KEY_MOVE_X,KEY_ROW1_BY+KEY_MOVE_Y}	,'e', SCANCODE_E,         KEY_PAD_CHAR, NULL, share_key_update},
{ {246-KEY_MOVE_X,KEY_ROW1_TY-KEY_MOVE_Y,313+KEY_MOVE_X,KEY_ROW1_BY+KEY_MOVE_Y}	,'r', SCANCODE_R,         KEY_PAD_CHAR, NULL, share_key_update},
{ {326-KEY_MOVE_X,KEY_ROW1_TY-KEY_MOVE_Y,393+KEY_MOVE_X,KEY_ROW1_BY+KEY_MOVE_Y}	,'t', SCANCODE_T,         KEY_PAD_CHAR, NULL, share_key_update},
{ {406-KEY_MOVE_X,KEY_ROW1_TY-KEY_MOVE_Y,473+KEY_MOVE_X,KEY_ROW1_BY+KEY_MOVE_Y}	,'y', SCANCODE_Y,         KEY_PAD_CHAR, NULL, share_key_update},
{ {486-KEY_MOVE_X,KEY_ROW1_TY-KEY_MOVE_Y,553+KEY_MOVE_X,KEY_ROW1_BY+KEY_MOVE_Y}	,'u', SCANCODE_U,         KEY_PAD_CHAR, NULL, share_key_update},
{ {566-KEY_MOVE_X,KEY_ROW1_TY-KEY_MOVE_Y,633+KEY_MOVE_X,KEY_ROW1_BY+KEY_MOVE_Y}	,'i', SCANCODE_I,         KEY_PAD_CHAR, NULL, share_key_update},
{ {646-KEY_MOVE_X,KEY_ROW1_TY-KEY_MOVE_Y,713+KEY_MOVE_X,KEY_ROW1_BY+KEY_MOVE_Y}	,'o', SCANCODE_O,         KEY_PAD_CHAR, NULL, share_key_update},
{ {726-KEY_MOVE_X,KEY_ROW1_TY-KEY_MOVE_Y,800,KEY_ROW1_BY+KEY_MOVE_Y}	,'p', SCANCODE_P,         KEY_PAD_CHAR, NULL, share_key_update},
{ {20-KEY_MOVE_X,KEY_ROW2_TY-KEY_MOVE_Y,113+KEY_MOVE_X,KEY_ROW2_BY+KEY_MOVE_Y}	,'a', SCANCODE_A,         KEY_PAD_CHAR, NULL, share_key_update},
{ {126-KEY_MOVE_X,KEY_ROW2_TY-KEY_MOVE_Y,193+KEY_MOVE_X,KEY_ROW2_BY+KEY_MOVE_Y}	,'s', SCANCODE_S,         KEY_PAD_CHAR, NULL, share_key_update},
{ {206-KEY_MOVE_X,KEY_ROW2_TY-KEY_MOVE_Y,273+KEY_MOVE_X,KEY_ROW2_BY+KEY_MOVE_Y}	,'d', SCANCODE_D,         KEY_PAD_CHAR, NULL, share_key_update},
{ {286-KEY_MOVE_X,KEY_ROW2_TY-KEY_MOVE_Y,353+KEY_MOVE_X,KEY_ROW2_BY+KEY_MOVE_Y}	,'f', SCANCODE_F,         KEY_PAD_CHAR, NULL, share_key_update},
{ {366-KEY_MOVE_X,KEY_ROW2_TY-KEY_MOVE_Y,433+KEY_MOVE_X,KEY_ROW2_BY+KEY_MOVE_Y}	,'g', SCANCODE_G,         KEY_PAD_CHAR, NULL, share_key_update},
{ {446-KEY_MOVE_X,KEY_ROW2_TY-KEY_MOVE_Y,513+KEY_MOVE_X,KEY_ROW2_BY+KEY_MOVE_Y}	,'h', SCANCODE_H,         KEY_PAD_CHAR, NULL, share_key_update},
{ {526-KEY_MOVE_X,KEY_ROW2_TY-KEY_MOVE_Y,593+KEY_MOVE_X,KEY_ROW2_BY+KEY_MOVE_Y}	,'j', SCANCODE_J,         KEY_PAD_CHAR, NULL, share_key_update},
{ {606-KEY_MOVE_X,KEY_ROW2_TY-KEY_MOVE_Y,673+KEY_MOVE_X,KEY_ROW2_BY+KEY_MOVE_Y}	,'k', SCANCODE_K,         KEY_PAD_CHAR, NULL, share_key_update},
{ {686-KEY_MOVE_X,KEY_ROW2_TY-KEY_MOVE_Y,753+KEY_MOVE_X,KEY_ROW2_BY+KEY_MOVE_Y}	,'l', SCANCODE_L,         KEY_PAD_CHAR, NULL, share_key_update},
{ {0,KEY_ROW3_TY-KEY_MOVE_Y,96+KEY_MOVE_X,KEY_ROW3_BY+KEY_MOVE_Y}	,' ', SCANCODE_LEFTSHIFT, KEY_PAD_FUNC, NULL, share_key_update},
{ {126-KEY_MOVE_X,KEY_ROW3_TY-KEY_MOVE_Y,193+KEY_MOVE_X,KEY_ROW3_BY+KEY_MOVE_Y}	,'z', SCANCODE_Z,         KEY_PAD_CHAR, NULL, share_key_update},
{ {206-KEY_MOVE_X,KEY_ROW3_TY-KEY_MOVE_Y,273+KEY_MOVE_X,KEY_ROW3_BY+KEY_MOVE_Y}	,'x', SCANCODE_X,         KEY_PAD_CHAR, NULL, share_key_update},
{ {286-KEY_MOVE_X,KEY_ROW3_TY-KEY_MOVE_Y,353+KEY_MOVE_X,KEY_ROW3_BY+KEY_MOVE_Y}	,'c', SCANCODE_C,         KEY_PAD_CHAR, NULL, share_key_update},
{ {366-KEY_MOVE_X,KEY_ROW3_TY-KEY_MOVE_Y,433+KEY_MOVE_X,KEY_ROW3_BY+KEY_MOVE_Y}	,'v', SCANCODE_V,         KEY_PAD_CHAR, NULL, share_key_update},
{ {446-KEY_MOVE_X,KEY_ROW3_TY-KEY_MOVE_Y,513+KEY_MOVE_X,KEY_ROW3_BY+KEY_MOVE_Y}	,'b', SCANCODE_B,         KEY_PAD_CHAR, NULL, share_key_update},
{ {526-KEY_MOVE_X,KEY_ROW3_TY-KEY_MOVE_Y,593+KEY_MOVE_X,KEY_ROW3_BY+KEY_MOVE_Y}	,'n', SCANCODE_N,         KEY_PAD_CHAR, NULL, share_key_update},
{ {606-KEY_MOVE_X,KEY_ROW3_TY-KEY_MOVE_Y,673+KEY_MOVE_X,KEY_ROW3_BY+KEY_MOVE_Y}	,'m', SCANCODE_M,         KEY_PAD_CHAR, NULL, share_key_update},
{ {702-KEY_MOVE_X,KEY_ROW3_TY-KEY_MOVE_Y,800,KEY_ROW3_BY}	,' ', SCANCODE_BACKSPACE, KEY_PAD_FUNC, NULL, share_key_update},
{ {0,KEY_ROW4_TY-KEY_MOVE_Y,92+KEY_MOVE_X,KEY_ROW4_BY+KEY_MOVE_Y}	,' ', SCANCODE_TOSYMBOL,     KEY_PAD_FUNC, NULL, share_key_update},
{ {105-KEY_MOVE_X,KEY_ROW4_TY-KEY_MOVE_Y,213+KEY_MOVE_X,KEY_ROW4_BY+KEY_MOVE_Y}	,' ', SCANCODE_TOPY,      KEY_PAD_FUNC, NULL, share_key_update},
{ {226-KEY_MOVE_X,KEY_ROW4_TY-KEY_MOVE_Y,594+KEY_MOVE_X,KEY_ROW4_BY+KEY_MOVE_Y}	,' ', SCANCODE_SPACE,     KEY_PAD_CHAR, NULL, share_key_update},
{ {607-KEY_MOVE_X,KEY_ROW4_TY-KEY_MOVE_Y,800,KEY_ROW4_BY+KEY_MOVE_Y}	,' ', SCANCODE_ENTER,     KEY_PAD_FUNC, NULL, share_key_update},
};
#elif SOFTKBD_720_1280
static md_key_t en_key_pads[] = {
{ {1,KEY_ROW1_TY,73,KEY_ROW1_BY}	,'q', SCANCODE_Q,         KEY_PAD_CHAR, NULL, share_key_update},
{ {86,KEY_ROW1_TY,153,KEY_ROW1_BY}	,'w', SCANCODE_W,         KEY_PAD_CHAR, NULL, share_key_update},
{ {166,KEY_ROW1_TY,233,KEY_ROW1_BY}	,'e', SCANCODE_E,         KEY_PAD_CHAR, NULL, share_key_update},
{ {246,KEY_ROW1_TY,313,KEY_ROW1_BY}	,'r', SCANCODE_R,         KEY_PAD_CHAR, NULL, share_key_update},
{ {326,KEY_ROW1_TY,393,KEY_ROW1_BY}	,'t', SCANCODE_T,         KEY_PAD_CHAR, NULL, share_key_update},
{ {406,KEY_ROW1_TY,473,KEY_ROW1_BY}	,'y', SCANCODE_Y,         KEY_PAD_CHAR, NULL, share_key_update},
{ {486,KEY_ROW1_TY,553,KEY_ROW1_BY}	,'u', SCANCODE_U,         KEY_PAD_CHAR, NULL, share_key_update},
{ {566,KEY_ROW1_TY,633,KEY_ROW1_BY}	,'i', SCANCODE_I,         KEY_PAD_CHAR, NULL, share_key_update},
{ {646,KEY_ROW1_TY,713,KEY_ROW1_BY}	,'o', SCANCODE_O,         KEY_PAD_CHAR, NULL, share_key_update},
{ {726,KEY_ROW1_TY,793,KEY_ROW1_BY}	,'p', SCANCODE_P,         KEY_PAD_CHAR, NULL, share_key_update},
{ {46,KEY_ROW2_TY,113,KEY_ROW2_BY}	,'a', SCANCODE_A,         KEY_PAD_CHAR, NULL, share_key_update},
{ {126,KEY_ROW2_TY,193,KEY_ROW2_BY}	,'s', SCANCODE_S,         KEY_PAD_CHAR, NULL, share_key_update},
{ {206,KEY_ROW2_TY,273,KEY_ROW2_BY}	,'d', SCANCODE_D,         KEY_PAD_CHAR, NULL, share_key_update},
{ {286,KEY_ROW2_TY,353,KEY_ROW2_BY}	,'f', SCANCODE_F,         KEY_PAD_CHAR, NULL, share_key_update},
{ {366,KEY_ROW2_TY,433,KEY_ROW2_BY}	,'g', SCANCODE_G,         KEY_PAD_CHAR, NULL, share_key_update},
{ {446,KEY_ROW2_TY,513,KEY_ROW2_BY}	,'h', SCANCODE_H,         KEY_PAD_CHAR, NULL, share_key_update},
{ {526,KEY_ROW2_TY,593,KEY_ROW2_BY}	,'j', SCANCODE_J,         KEY_PAD_CHAR, NULL, share_key_update},
{ {606,KEY_ROW2_TY,673,KEY_ROW2_BY}	,'k', SCANCODE_K,         KEY_PAD_CHAR, NULL, share_key_update},
{ {686,KEY_ROW2_TY,753,KEY_ROW2_BY}	,'l', SCANCODE_L,         KEY_PAD_CHAR, NULL, share_key_update},
{ {1,KEY_ROW3_TY,96,KEY_ROW3_BY}	,' ', SCANCODE_LEFTSHIFT, KEY_PAD_FUNC, NULL, share_key_update},
{ {126,KEY_ROW3_TY,193,KEY_ROW3_BY}	,'z', SCANCODE_Z,         KEY_PAD_CHAR, NULL, share_key_update},
{ {206,KEY_ROW3_TY,273,KEY_ROW3_BY}	,'x', SCANCODE_X,         KEY_PAD_CHAR, NULL, share_key_update},
{ {286,KEY_ROW3_TY,353,KEY_ROW3_BY}	,'c', SCANCODE_C,         KEY_PAD_CHAR, NULL, share_key_update},
{ {366,KEY_ROW3_TY,433,KEY_ROW3_BY}	,'v', SCANCODE_V,         KEY_PAD_CHAR, NULL, share_key_update},
{ {446,KEY_ROW3_TY,513,KEY_ROW3_BY}	,'b', SCANCODE_B,         KEY_PAD_CHAR, NULL, share_key_update},
{ {526,KEY_ROW3_TY,593,KEY_ROW3_BY}	,'n', SCANCODE_N,         KEY_PAD_CHAR, NULL, share_key_update},
{ {606,KEY_ROW3_TY,673,KEY_ROW3_BY}	,'m', SCANCODE_M,         KEY_PAD_CHAR, NULL, share_key_update},
{ {702,KEY_ROW3_TY,794,KEY_ROW3_BY}	,' ', SCANCODE_BACKSPACE, KEY_PAD_FUNC, NULL, share_key_update},
{ {1,KEY_ROW4_TY,92,KEY_ROW4_BY}	,' ', SCANCODE_TOSYMBOL,     KEY_PAD_FUNC, NULL, share_key_update},
{ {105,KEY_ROW4_TY,213,KEY_ROW4_BY}	,' ', SCANCODE_TOPY,      KEY_PAD_FUNC, NULL, share_key_update},
{ {226,KEY_ROW4_TY,594,KEY_ROW4_BY}	,' ', SCANCODE_SPACE,     KEY_PAD_CHAR, NULL, share_key_update},
{ {607,KEY_ROW4_TY,795,KEY_ROW4_BY}	,' ', SCANCODE_ENTER,     KEY_PAD_FUNC, NULL, share_key_update},
};
#else
static md_key_t en_key_pads[] = {
    { {2,63,46,96}		,'q', SCANCODE_Q,         KEY_PAD_CHAR, NULL, share_key_update},
    { {50,63,94,96}		,'w', SCANCODE_W,         KEY_PAD_CHAR, NULL, share_key_update},
    { {98,63,142,96}	,'e', SCANCODE_E,         KEY_PAD_CHAR, NULL, share_key_update},
    { {146,63,190,96}	,'r', SCANCODE_R,         KEY_PAD_CHAR, NULL, share_key_update},
    { {194,63,238,96}	,'t', SCANCODE_T,         KEY_PAD_CHAR, NULL, share_key_update},
    { {242,63,286,96}	,'y', SCANCODE_Y,         KEY_PAD_CHAR, NULL, share_key_update},
    { {289,63,334,96}	,'u', SCANCODE_U,         KEY_PAD_CHAR, NULL, share_key_update},
    { {338,63,382,96}	,'i', SCANCODE_I,         KEY_PAD_CHAR, NULL, share_key_update},
    { {386,63,430,96}	,'o', SCANCODE_O,         KEY_PAD_CHAR, NULL, share_key_update},
    { {434,63,478,96}	,'p', SCANCODE_P,         KEY_PAD_CHAR, NULL, share_key_update},
    { {26,100,70,133}	,'a', SCANCODE_A,         KEY_PAD_CHAR, NULL, share_key_update},
    { {74,100,118,133}	,'s', SCANCODE_S,         KEY_PAD_CHAR, NULL, share_key_update},
    { {122,100,166,133}	,'d', SCANCODE_D,         KEY_PAD_CHAR, NULL, share_key_update},
    { {170,100,214,133}	,'f', SCANCODE_F,         KEY_PAD_CHAR, NULL, share_key_update},
    { {218,100,262,133}	,'g', SCANCODE_G,         KEY_PAD_CHAR, NULL, share_key_update},
    { {266,100,310,133}	,'h', SCANCODE_H,         KEY_PAD_CHAR, NULL, share_key_update},
    { {313,100,358,133}	,'j', SCANCODE_J,         KEY_PAD_CHAR, NULL, share_key_update},
    { {362,100,406,133}	,'k', SCANCODE_K,         KEY_PAD_CHAR, NULL, share_key_update},
    { {409,100,454,133}	,'l', SCANCODE_L,         KEY_PAD_CHAR, NULL, share_key_update},
    { {2,137,70,170}	,' ', SCANCODE_LEFTSHIFT, KEY_PAD_FUNC, NULL, share_key_update},
    { {74,137,118,170}	,'z', SCANCODE_Z,         KEY_PAD_CHAR, NULL, share_key_update},
    { {122,137,166,170}	,'x', SCANCODE_X,         KEY_PAD_CHAR, NULL, share_key_update},
    { {170,137,214,170}	,'c', SCANCODE_C,         KEY_PAD_CHAR, NULL, share_key_update},
    { {218,137,262,170}	,'v', SCANCODE_V,         KEY_PAD_CHAR, NULL, share_key_update},
    { {266,137,310,170}	,'b', SCANCODE_B,         KEY_PAD_CHAR, NULL, share_key_update},
    { {314,137,358,170}	,'n', SCANCODE_N,         KEY_PAD_CHAR, NULL, share_key_update},
    { {362,137,406,170}	,'m', SCANCODE_M,         KEY_PAD_CHAR, NULL, share_key_update},
    { {409,137,478,170}	,' ', SCANCODE_BACKSPACE, KEY_PAD_FUNC, NULL, share_key_update},
    { {2,175,70,213}	,' ', SCANCODE_TOSYMBOL,     KEY_PAD_FUNC, NULL, share_key_update},
    { {74,175,166,213}	,' ', SCANCODE_TOPY,      KEY_PAD_FUNC, NULL, share_key_update},
    { {170,175,358,213}	,' ', SCANCODE_SPACE,     KEY_PAD_CHAR, NULL, share_key_update},
    { {362,175,478,213}	,' ', SCANCODE_ENTER,     KEY_PAD_FUNC, NULL, share_key_update},
};
#endif

static void init_en_key_pads()
{
	int i, count;
	count = TABLESIZE(en_key_pads);
	for (i = 0; i < count; i++)
	{
		char key[20] = { 0 };
		PRECT pRect = NULL;
		sprintf(key, "RECT_EN_26_KEY_%d", i + 1);
		if((NULL != handleCfg) && (pRect = (PRECT) FindMigCfgRectValue(handleCfg, key)) != NULL)
		{
			CopyRect(&en_key_pads[i].bound, pRect);
		}
	}

}


static md_key_t* get_en_key (key_window_t *kw, POINT point)
{
    int i;
    md_key_t* keys = kw->key;

    for(i = 0; i < kw->key_num; i++)
    {
        if (hit_rect( keys[i].bound, point))
        {
        	return &keys[i];
        }
    }
    return NULL;
}

/*
static void kw_update (key_window_t *kw, HWND hWnd)
{
    //TODO
    //printf("==== update key window ====\n");
}
*/
static void sw_update(stroke_window_t *sw, HWND hWnd)
{
	HDC hdc;
	PLOGFONT old_font;
	gal_pixel old_tecolor;

	EraseBbGround(hWnd, &sw->bound);
	if (strlen(sw->str) != 0)
	{
		hdc = GetDC(hWnd);

		SetBkMode (hdc, BM_TRANSPARENT);
#if 0
#ifdef SOFTKBD_800_480
		old_tecolor = SetTextColor (hdc, PIXEL_black);
#else
		old_tecolor = SetTextColor (hdc, PIXEL_green);
#endif

#endif

#ifdef ZMM500
		if(LCD_SIZE_480X272 <= getLCDSize())//zmm500平台采用的白底风格的UI
#else
		if(LCD_SIZE_480X800 <= getLCDSize())
#endif
		{
			old_tecolor = SetTextColor (hdc, RGB2Pixel(hdc, 0x66, 0x66, 0x66));
		}
		else
		{
			old_tecolor = SetTextColor (hdc, PIXEL_green);
		}
		//printf("caona bound left =%d, tip=%d, right=%d, bottom =%d---\n", sw->bound.left,  sw->bound.top,  sw->bound.right, sw->bound.bottom);

		old_font = SelectFont(hdc, sw->stroke_font);
		DrawText (hdc, sw->str, -1, &(sw->bound), DT_LEFT | DT_SINGLELINE);
		//SelectFont(hdc, old_font);
		SetTextColor(hdc, old_tecolor);

		ReleaseDC(hdc);
	}
}

static void vw_update(view_window_t *vw, HWND hWnd, vw_element_t* element)
{
	int i;
	RECT r;
	PBITMAP pbmp;
	static int old_style = 0;
	HDC hdc = GetDC(hWnd);
	RECT rc;
	PLOGFONT oldfont;

	r.left = vw->key_pg_up.right;
	r.top = vw->bound.top;
	r.right = vw->key_pg_down.left;
	r.bottom = vw->bound.bottom;

	oldfont = SelectFont(hdc, vw->view_font);
	SetBkMode (hdc, BM_TRANSPARENT);
	if (vw->style & VW_DRAW_ELMTS)
	{
		if (element == NULL)
		{
			element = vw->elements;
			EraseBbGround(hWnd, &r);

			for (i = 0; i < vw->element_num; i++)
			{
#ifndef __FILL_DIRECT__ 
				SetTextColor(hdc, RGB2Pixel(hdc, 190, 190, 190));
				DrawText(hdc, element[i].string, -1, &element[i].bound, 0);

				rc = element[i].bound;
				rc.top --;
				rc.bottom --;
				rc.left --;
				rc.right --;
				SetTextColor(hdc, RGB2Pixel(hdc, 155, 50, 155));
				DrawText(hdc, element[i].string, -1, &rc, 0);
#else
				//    printf("element[i].string=%s\n", element[i].string);
				//    printf("element[i].bound:l=%d,t=%d,r=%d,b=%d\n", element[i].bound.left,
				//        element[i].bound.top, element[i].bound.right, element[i].bound.bottom);
#ifdef ZMM500
				if(LCD_SIZE_480X272 <= getLCDSize())
#else
				if(LCD_SIZE_480X800 <= getLCDSize())
#endif
				{
					SetTextColor(hdc, RGB2Pixel(hdc, 0x66, 0x66, 0x66));
				}
				else
				{
					SetTextColor(hdc, COLOR_lightwhite);
				}
				DrawText(hdc, element[i].string, -1, &element[i].bound, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
#endif
			}
		}
		else
		{
			if (vw->style & VW_EL_PRESSED)
			{
				if (vw->data)
#ifdef __FILL_DIRECT__
#if 0
					FillBoxWithBitmap(hdc,element->bound.left,
							element->bound.top,
							element->bound.right - element->bound.left,
							element->bound.bottom - element->bound.top,
							&(((vw_add_data_t *)(vw->data))->sel_bk) );
#else

#ifdef ZMM500
				if(LCD_SIZE_480X272 <= getLCDSize())
#else
				if(LCD_SIZE_480X800 <= getLCDSize())
#endif
				{
					SetBrushColor(hdc, RGB2Pixel(hdc, 0x7a, 0xc1, 0x43));
					SetTextColor(hdc, RGB2Pixel(hdc, 0x66, 0x66, 0x66));
				}
				else
				{
					SetBrushColor(hdc, RGB2Pixel(hdc, 0x38, 0x39, 0x3A));
					SetTextColor(hdc, COLOR_lightwhite);
				}

				FillBox(hdc, element->bound.left, element->bound.top, element->bound.right - element->bound.left,
						element->bound.bottom - element->bound.top);
#endif
				DrawText(hdc, element->string, -1, &element->bound, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

#else
				rc = element->bound;
				rc.top --;
				rc.left --;
				EraseBbGround(hWnd, &rc);
				rc.top += 2;
				rc.left += 2;
				rc.bottom ++;
				rc.right ++;
				SetTextColor(hdc, RGB2Pixel(hdc, 100, 50, 100));
				DrawText(hdc, element->string, -1, &rc, 0);
#endif
			}
			else
			{
				EraseBbGround(hWnd, &vw->bound);
			}

		}
	}
	else if (old_style & VW_DRAW_ELMTS)
	{
		EraseBbGround(hWnd, &r);
	}

	EraseBbGround(hWnd, &vw->key_pg_up);
	if (vw->style & VW_SHOW_PU)
	{
		if (vw->style & VW_PU_PRESSED)
		{
			pbmp = ((vw_add_data_t *) (vw->data))->pu_press;
		}
		else
		{
			pbmp = ((vw_add_data_t *) (vw->data))->pu_normal;
		}
		FillBoxWithBitmap(hdc, vw->key_pg_up.left, vw->key_pg_up.top, vw->key_pg_up.right - vw->key_pg_up.left,
				vw->key_pg_up.bottom - vw->key_pg_up.top, pbmp);
	}

	EraseBbGround(hWnd, &vw->key_pg_down);
	if (vw->style & VW_SHOW_PD)
	{
		if (vw->style & VW_PD_PRESSED)
		{
			pbmp = ((vw_add_data_t *) (vw->data))->pd_press;
		}
		else
		{
			pbmp = ((vw_add_data_t *) (vw->data))->pd_normal;
		}
		FillBoxWithBitmap(hdc, vw->key_pg_down.left, vw->key_pg_down.top, vw->key_pg_down.right - vw->key_pg_down.left,
				vw->key_pg_down.bottom - vw->key_pg_down.top, pbmp);
	}

	old_style = vw->style;
	SelectFont(hdc, oldfont);
	ReleaseDC(hdc);
}

int get_kpd_en_bitmap (HDC hdc, PBITMAP* pbmp, int id)
{
	if (id  < 0 || id > EN26_KBD_NUM)
    {
    	return -1;
	}

	if(id == 0)
	{
		get_keyboard_bitmap(hdc,pbmp,EN26_KBD_BKG);
	}
	else
	{
		get_keyboard_bitmap(hdc,pbmp,EN26_KBD_BKG1);
	}

    return 0;
}

static int en_proc_msg(key_board_t* key_board, HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
	static md_key_t *key_down;
	static int lbuttondown = 0;
	static POINT p;
	md_key_t *key;

	switch (message)
	{
		case MSG_LBUTTONDOWN:
		{
			p.x = LOSWORD(lParam);
			p.y = HISWORD(lParam);
			lbuttondown = 1;
			key_board->action.operation = AC_NULL;

			if (hit_rect(key_board->close_rc, p))
			{
				en_kb_Index = 0;
				get_kpd_en_bitmap(HDC_SCREEN, (PBITMAP) (&key_board->data), 0);
				key_board->action.operation = AC_NULL;
				close_update(key_board, hwnd, TRUE);
				return AC_NULL;
			}

			if (hit_rect(key_board->view_window->bound, p))
			{
				vw_proceed_hit(hwnd, key_board->view_window, key_board->stroke_window, &key_board->action, TRUE, p, CN,
						key_board->ime);
				break;
			}

			if (hit_rect(key_board->key_window->bound, p))
			{
				key = key_board->key_window->get_key(key_board->key_window, p);
				if (key == NULL)
				{
					key_board->action.operation = AC_NULL;
					break;
				}
				else if (key->scan_code == SCANCODE_LEFTSHIFT)
				{
					key->update(key, hwnd);

#ifdef ZMM500
					if ((en_kb_Index < EN26_KBD_NUM) && (LCD_SIZE_480X272 <= getLCDSize()))//�л���Сд��ĸ����ʱ�滻����ͼƬ��add by chenlf
#else
					if ((en_kb_Index < EN26_KBD_NUM) && (LCD_SIZE_480X800 <= getLCDSize()))//�л���Сд��ĸ����ʱ�滻����ͼƬ��add by chenlf
#endif
					{
						en_kb_Index = (en_kb_Index + 1)%2;
						get_kpd_en_bitmap(HDC_SCREEN, (PBITMAP) (&key_board->data), en_kb_Index);
						PostMessage(hwnd, MSG_ERASEBKGND, 0, 0L);
					}
				}

				kw_proceed_hit(hwnd, key_board->view_window, key_board->stroke_window, key, &key_board->action, TRUE,
						p, EN, key_board->ime, wParam, lParam);
				key_down = key;

				break;
			}

			break;
		}
		case MSG_LBUTTONUP:
		{
			if (lbuttondown == 0)
			{
				key_board->action.operation = AC_NULL;
				break;
			}

			lbuttondown = 0;
			p.x = LOSWORD(lParam);
			p.y = HISWORD(lParam);

			if (hit_rect(key_board->close_rc, p))
			{

				key_board->action.operation = AC_NULL;
				close_update(key_board, hwnd, FALSE);
				SendMessage(hwnd, IME_CLOSE, 0, 0);
				return AC_NULL;
			}

			if (hit_rect(key_board->view_window->bound, p))
			{
				vw_proceed_hit(hwnd, key_board->view_window, key_board->stroke_window, &key_board->action, FALSE, p,
						EN, key_board->ime);
				break;
			}

			if (hit_rect(key_board->key_window->bound, p))
			{
				key = key_board->key_window->get_key(key_board->key_window, p);
				if (key == NULL)
				{
					key_board->action.operation = AC_NULL;
					break;
				}

				kw_proceed_hit(hwnd, key_board->view_window, key_board->stroke_window, key, &key_board->action, FALSE,
						p, EN, key_board->ime, wParam, lParam);
#ifdef KBD_TOOLTIP
				if(tooltip_y < key_board->view_window->bound.bottom + g_rcScr.bottom - key_board->skb_win_h)
					UpdateToolTip(hwnd);
#endif
				break;
			}
			key_down = NULL;
			break;
		}
		case MSG_MOUSEMOVE:
		{
			p.x = LOSWORD(lParam);
			p.y = HISWORD(lParam);
			proceed_move(hwnd, key_board, lbuttondown, p, EN);
			break;
		}
		case MSG_MOUSEMOVEIN:
		{
			lbuttondown = lbuttondown && wParam;
			key_board->action.operation = AC_NULL;
			break;
		}
	}

	return key_board->action.operation;
}

static int init_en_view_window(HWND hWnd, view_window_t *vw)
{
	PRECT pRect = NULL;
	int lcdType = getLCDSize();

	if((NULL != handleCfg) && (pRect = (PRECT) FindMigCfgRectValue(handleCfg, "RECT_SKB_26_VW")) != NULL)
	{
		CopyRect(&vw->bound, pRect);
	}
	else
	{
		SetRect(&vw->bound,SKB_VW_DEFAULT_L,SKB_VW_DEFAULT_T,SKB_VW_DEFAULT_R,SKB_VW_DEFAULT_B);
	}

	if((NULL != handleCfg) && (pRect = (PRECT) FindMigCfgRectValue(handleCfg, "RECT_SKB_26_VW_PU")) != NULL)
	{
		CopyRect(&vw->key_pg_up, pRect);
	}
	else
	{
		SetRect(&vw->key_pg_up,SKB_VW_PU_DEFAULT_L,SKB_VW_PU_DEFAULT_T,SKB_VW_PU_DEFAULT_R,SKB_VW_PU_DEFAULT_B);
	}

	if((NULL != handleCfg) && (pRect = (PRECT)FindMigCfgRectValue(handleCfg, "RECT_SKB_26_VW_PD")) != NULL)
	{
		CopyRect(&vw->key_pg_down, pRect);
	}
	else
	{
		SetRect(&vw->key_pg_down,SKB_VW_PD_DEFAULT_L,SKB_VW_PD_DEFAULT_T,SKB_VW_PD_DEFAULT_R,SKB_VW_PD_DEFAULT_B);
	}

	if (LCD_SIZE_320X240 == lcdType)
	{
		vw->max_str_len = 22;
		vw->view_font = CreateLogFontByName("*-fixed-rrncnn-*-12-ISO8859-1");
	}
	else if (LCD_SIZE_480X272 == lcdType)
	{
		vw->max_str_len = 32;
		vw->view_font = CreateLogFontByName("*-fixed-rrncnn-*-16-ISO8859-1");
	}
	else if (LCD_SIZE_480X800 == lcdType)
	{
		vw->max_str_len = 24;
		vw->view_font = CreateLogFont ("ttf", "SourceHanSansCNNormal", "UTF-8",FONT_WEIGHT_REGULAR,
			FONT_SLANT_ROMAN, FONT_FLIP_NIL, FONT_OTHER_NIL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 24, 0);
	}
	else if(LCD_SIZE_800X480 == lcdType)
	{
		vw->max_str_len = 24;
		vw->view_font = CreateLogFont ("ttf", "SourceHanSansCNNormal", "UTF-8", FONT_WEIGHT_BOOK,
			FONT_SLANT_ROMAN, FONT_FLIP_NIL, FONT_OTHER_NIL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 20, 0);
	}
	else if (LCD_SIZE_480X800 < lcdType)
	{
		vw->max_str_len = 24;
		vw->view_font = CreateLogFont ("ttf", "SourceHanSansCNNormal", "UTF-8",FONT_WEIGHT_BOOK,
			FONT_SLANT_ROMAN, FONT_FLIP_NIL, FONT_OTHER_NIL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 28, 0);
	}
	else 
	{
		vw->max_str_len = 20;
		vw->view_font = CreateLogFontByName("*-fixed-rrncnn-*-12-ISO8859-1");
	}

	vw->update = vw_update;
	vw->set_elements = vw_set_elements;
	vw->clear_elements = vw_clear_elements;
	vw->get_element = vw_get_element;

#if 0
	vw->view_font = CreateLogFont ("ttf", fontname, "ISO8859-1",
			FONT_WEIGHT_REGULAR,
			FONT_SLANT_ROMAN,
			FONT_FLIP_NIL,
			FONT_OTHER_NIL,
			FONT_UNDERLINE_NONE,
			FONT_STRUCKOUT_NONE,
			12, 0);
#endif 
	if (NULL == vw->view_font)
	{
		_MY_PRINTF("create logfont for view window error.\n");
		return -1;
	}

	if (NULL == (vw->data = (void *) calloc(1, sizeof(vw_add_data_t))))
	{
		_MY_PRINTF("no memory for key window add data.\n");
		return -1;
	}

	if (get_keyboard_bitmap(HDC_SCREEN, &(((vw_add_data_t *) (vw->data))->pu_normal), LEFT_ARROW_ENABLE))
	{
		_MY_PRINTF ("Fail to get page-up mask bitmap.\n");
		return -1;
	}

	if (get_keyboard_bitmap(HDC_SCREEN, &(((vw_add_data_t *) (vw->data))->pu_press), LEFT_ARROW_DISABLE))
	{
		_MY_PRINTF ("Fail to get page-up mask bitmap.\n");
		return -1;
	}

	if (get_keyboard_bitmap(HDC_SCREEN, &(((vw_add_data_t *) (vw->data))->pd_normal), RIGHT_ARROW_ENABLE))
	{
		_MY_PRINTF ("Fail to get page-down mask bitmap.\n");
		return -1;
	}

	if (get_keyboard_bitmap(HDC_SCREEN, &(((vw_add_data_t *) (vw->data))->pd_press), RIGHT_ARROW_DISABLE))
	{
		_MY_PRINTF ("Fail to get page-down mask bitmap.\n");
		return -1;
	}
	/*
	 if (get_kbd_bitmap(HDC_SCREEN,
	 &(((vw_add_data_t *)(vw->data))->sel_bk),
	 SELECT_TEXT_BKGND)) {
	 _MY_PRINTF ("Fail to get sellect text bkgnd bitmap.\n");
	 return -1;
	 }
	 */
	return 0;
}

static void destroy_en_view_window(view_window_t *vw)
{
	if (vw->data)
	{
		// free(vw->data);
	}

	if (vw->view_font)
	{
		DestroyLogFont(vw->view_font);
	}
}

static int init_en_stroke_window(HWND hWnd, stroke_window_t *sw)
{
	PRECT pRect = NULL;
	int lcdType = getLCDSize();
	
	if((NULL != handleCfg) && (pRect = (PRECT) FindMigCfgRectValue(handleCfg, "RECT_SKB_26_SW")) != NULL)
	{
		CopyRect(&sw->bound, pRect);
	}
	else
	{
		SetRect(&sw->bound,SKB_SW_DEFAULT_L,SKB_SW_DEFAULT_T,SKB_SW_DEFAULT_R,SKB_SW_DEFAULT_B);
	}

	memset(sw->str, 0, SW_STR_LEN);
#if 0   //change by caona
	sw->stroke_font = CreateLogFont (NULL, "fixed", "GB2312",
			FONT_WEIGHT_BOOK, FONT_SLANT_ROMAN,
			FONT_SETWIDTH_NORMAL, FONT_OTHER_NIL,
			FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
			18, 0);
#else
	//sw->stroke_font = CreateLogFontByName ("*-fixed-rrncnn-*-12-ISO8859-1");

#ifdef SOFTKBD_800_480
	sw->stroke_font = CreateLogFontByName ("*-SansSerif-rrncnn-*-24-ISO8859-1");
#else
//	sw->stroke_font = CreateLogFontByName("*-SansSerif-rrncnn-*-12-ISO8859-1");
#endif
#endif

	if(LCD_SIZE_800X480 == lcdType)
	{
		sw->stroke_font = CreateLogFont ("ttf", "SourceHanSansCNNormal", "UTF-8", FONT_WEIGHT_BOOK,
			FONT_SLANT_ROMAN, FONT_FLIP_NIL, FONT_OTHER_NIL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 20, 0);
	}
	else if(LCD_SIZE_480X800 == lcdType)
	{
		sw->stroke_font = CreateLogFont ("ttf", "SourceHanSansCNNormal", "UTF-8", FONT_WEIGHT_REGULAR,
			FONT_SLANT_ROMAN, FONT_FLIP_NIL, FONT_OTHER_NIL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 24, 0);
	}
	else if(LCD_SIZE_480X800 < lcdType)
	{
		sw->stroke_font = CreateLogFont ("ttf", "SourceHanSansCNNormal", "UTF-8", FONT_WEIGHT_REGULAR,
			FONT_SLANT_ROMAN, FONT_FLIP_NIL, FONT_OTHER_NIL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 36, 0);
	}
	else
	{
		sw->stroke_font = CreateLogFontByName("*-fixed-rrncnn-*-12-ISO8859-1");
	}

#if 0
	sw->stroke_font = CreateLogFont ("rbf", "fixed", "ISO8859-1",
			FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN,
			FONT_FLIP_NIL, FONT_OTHER_NIL,
			FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE,
			8, 0);
#endif
	if (NULL == sw->stroke_font)
	{
		_MY_PRINTF("create logfont for stroke window error.\n");
		return -1;
	}

	sw->update = sw_update;

	return 0;
}

static void destroy_en_stroke_window (stroke_window_t *sw)
{
    if (sw->stroke_font)
    {
    	DestroyLogFont (sw->stroke_font);
    }
    
    return;
}

/* return 0 on succes , less than 0 on error */
static int init_en_key_window(HWND hWnd, key_window_t *kw)
{
	int i;
	md_key_t *key;
	PRECT pRect = NULL;

	if((NULL != handleCfg) && (pRect = (PRECT) FindMigCfgRectValue(handleCfg, "RECT_SKB_26_KW")) != NULL)
	{
		CopyRect(&kw->bound, pRect);
	}
	else
	{
		SetRect(&kw->bound,SKB_KW_DEFAULT_L,SKB_KW_DEFAULT_T,SKB_KW_DEFAULT_R,SKB_KW_DEFAULT_B);
	}

	kw->press.x = 0;
	kw->press.y = 0;

	kw->key = en_key_pads;
	kw->key_num = TABLESIZE(en_key_pads);

	kw->get_key = get_en_key;

	if (NULL == (kw->data = (void *) calloc(1, sizeof(kw_add_data_t))))
	{
		_MY_PRINTF("no memory for key window add data.\n");
		return -1;
	}

	if (get_keyboard_bitmap(HDC_SCREEN, &(((kw_add_data_t *) (kw->data))->char_key_press), CHAR_KEY_MASK))
	{
		_MY_PRINTF ("Fail to get en char key mask bitmap.\n");
		return -1;
	}

	if (get_keyboard_bitmap(HDC_SCREEN, &(((kw_add_data_t *) (kw->data))->func_key_press), FUNC_KEY_MASK))
	{
		_MY_PRINTF ("Fail to get en func key mask bitmap.\n");
		return -1;
	}

	//kw->update = kw_update;
	kw->update = NULL;

	/*use the shared bitmap for press key painting.*/
	key = kw->key;
	for (i = 0; i < kw->key_num; i++)
	{
		if (key->style & KEY_PAD_CHAR)
		{
			key->data = ((kw_add_data_t *) (kw->data))->char_key_press;
		}
		else
		{
			key->data = ((kw_add_data_t *) (kw->data))->func_key_press;
		}
		key++;
	}
	return 0;
}

static void destroy_en_key_window (key_window_t *kw)
{
    if (kw->data)
    {
//        free(kw->data);
    }
}

int init_en_keyboard(const void* handle, HWND hWnd, key_board_t *kb)
{
	PRECT pRect = NULL;
	int valueCfg = -1;
	make_font();
	handleCfg = handle;

	if((NULL != handleCfg) && (pRect = (PRECT) FindMigCfgRectValue(handleCfg, "RECT_SKB_26_CLOSE")) != NULL)
	{
		CopyRect(&kb->close_rc, pRect);
	}
	else
	{
		SetRect(&kb->close_rc,SKB_CLOSE_DEFAULT_L,SKB_CLOSE_DEFAULT_T,SKB_CLOSE_DEFAULT_R,SKB_CLOSE_DEFAULT_B);
	}

	if ((NULL != handleCfg) && (valueCfg = FindMigCfgIntValue(handleCfg, "SKB_WIN_H")) >= 0)
	{
		kb->skb_win_h = valueCfg;
	}
	else
	{
		kb->skb_win_h = SKB_WIN_DEFAULT_H;
	}

	if ((NULL != handleCfg) && (valueCfg = FindMigCfgIntValue(handleCfg, "SKB_WIN_W")) >= 0)
	{
		kb->skb_win_w = valueCfg;
	}
	else
	{
		kb->skb_win_w = SKB_WIN_DEFAULT_W;
	}
	if(NULL != handleCfg)
	{
		init_en_key_pads();
	}

	/*initalize for view window.*/
	kb->view_window = (view_window_t *) calloc(1, sizeof(view_window_t));
	if (kb->view_window == NULL)
	{
		_MY_PRINTF("no memory for view window.\n");
		return -1;
	}
	if (0 != init_en_view_window(hWnd, kb->view_window))
	{
		_MY_PRINTF("error for initalizing view window.\n");
		return -1;
	}

	/*initalize for stroke window.*/
	kb->stroke_window = (stroke_window_t *) calloc(1, sizeof(stroke_window_t));
	if (kb->stroke_window == NULL)
	{
		_MY_PRINTF("no memory for stroke window.\n");
		return -1;
	}
	if (0 != init_en_stroke_window(hWnd, kb->stroke_window))
	{
		_MY_PRINTF("error for initalizing stroke window.\n");
		return -1;
	}

	/*initalize for key window.*/
	kb->key_window = (key_window_t *) calloc(1, sizeof(key_window_t));
	if (kb->key_window == NULL)
	{
		_MY_PRINTF("no memory for key window.\n");
		return -1;
	}
	if (0 != init_en_key_window(hWnd, kb->key_window))
	{
		_MY_PRINTF("error for initalizing key window.\n");
		return -1;
	}

	/*initalize for bk image.
	 kb->data = calloc (1, sizeof(BITMAP));
	 if (kb->data == NULL){
	 _MY_PRINTF("no memory for bk image.\n");
	 return -1;
	 }
	 */

	if (get_keyboard_bitmap(HDC_SCREEN, (PBITMAP) (&kb->data), EN26_KBD_BKG))
	{
		_MY_PRINTF ("Fail to get en keyboard back ground bitmap.\n");
		return -1;
	}

	if (get_keyboard_bitmap(HDC_SCREEN, &kb->close_normal, CLOSE_NORMAL))
	{
		_MY_PRINTF ("Fail to get en keyboard CLOSE_NORMAL bitmap.\n");
		return -1;
	}

	if (get_keyboard_bitmap(HDC_SCREEN, &kb->close_press, CLOSE_PRESSED))
	{
		_MY_PRINTF ("Fail to get en keyboard CLOSE_PRESSED bitmap.\n");
		return -1;
	}

	memset(&(kb->action), 0, sizeof(action_t));

	kb->update = keyboard_update;
	kb->ime = &en_ime;
	kb->proceed_msg = en_proc_msg;
	kb->clear = clear_keyboard;

	return 0;
}

void destroy_en_keyboard (key_board_t *kb)
{
	if(NULL == kb)
	{
		return;
	}
    /*view window*/
    destroy_en_view_window (kb->view_window);
    free(kb->view_window);

    /*stroke window*/
    destroy_en_stroke_window (kb->stroke_window);
    free(kb->stroke_window);

    /*ky window*/
    destroy_en_key_window (kb->key_window);
    free(kb->key_window);

    /*add data*/
    //free(kb->data);
	
}

