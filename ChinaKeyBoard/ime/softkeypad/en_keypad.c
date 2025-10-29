/**
 * FileName: en_keypad.c
 *
 * Copyright (C) 2014-2015, ZKSoftware Inc.
 *
 * Created: 2014-8-5
 * Author: wilsn
 *
 * Description:
 */



//#include "common.h"
#include "../libime/ime.h"
#include "softkeypad.h"
#ifdef KBD_TOOLTIP
#include "tooltip.h"
#endif

static char fontname[32] = {0};
static void *handleCfg;

static md_ime_t en_ime =
{
    100,
    "mgpti",
    cb_pti_match_keystrokes,
    cb_pti_match_keystrokes,
    NULL
};
static const unsigned char alpha_list[10][5] = {
	{'0'},
	{'0'},

	{'a', 'b', 'c'},
	{'d', 'e', 'f'},
	{'g', 'h', 'i'},
	{'j', 'k', 'l'},
	{'m', 'n', 'o'},
	{'p', 'q', 'r', 's'},
	{'t', 'u', 'v'},
	{'w', 'x', 'y', 'z'}

};

static St_T9key en_key ;

static md_key_t en_key_pads[] = {
    { {4,72,59,114}		, ' ',   SCANCODE_LEFTSHIFT,     KEY_PAD_FUNC, NULL, share_key_update},
    { {63,72,118,114}	, '2', 	SCANCODE_2,         	KEY_PAD_CHAR, NULL, share_key_update},
    { {122,72,177,114}	, '3', 	SCANCODE_3,         	KEY_PAD_CHAR, NULL, share_key_update},
    { {181,72,236,114}	, ' ',   SCANCODE_BACKSPACE,     KEY_PAD_FUNC, NULL, share_key_update},
    { {4,119,59,161}	, '4', 	SCANCODE_4,         	KEY_PAD_CHAR, NULL, share_key_update},
    { {63,119,118,161}	, '5', 	SCANCODE_5,         	KEY_PAD_CHAR, NULL, share_key_update},
    { {122,119,177,161}	, '6', 	SCANCODE_6,         	KEY_PAD_CHAR, NULL, share_key_update},
    { {181,119,236,161}	, ' ',   SCANCODE_CURSORBLOCKUP,        KEY_PAD_FUNC, NULL, share_key_update},
    { {4,166,59,208}	, '7',	SCANCODE_7,         	KEY_PAD_CHAR, NULL, share_key_update},
    { {63,166,118,208}	, '8', 	SCANCODE_8,         	KEY_PAD_CHAR, NULL, share_key_update},
    { {122,166,177,208}	, '9',	SCANCODE_9,         	KEY_PAD_CHAR, NULL, share_key_update},
    { {181,166,236,208}	, ' ',   SCANCODE_CURSORBLOCKDOWN,      KEY_PAD_FUNC, NULL, share_key_update},
    { {4,213,59,260}	, ' ',   SCANCODE_ESCAPE,        KEY_PAD_FUNC, NULL, share_key_update},
    { {63,213,118,260}	, ' ',   SCANCODE_SPACE,         KEY_PAD_FUNC, NULL, share_key_update},
    { {122,213,177,260}	, ' ',   SCANCODE_TOPY,          KEY_PAD_FUNC, NULL, share_key_update},
    { {181,213,236,260}	, ' ',   SCANCODE_ENTER,         KEY_PAD_FUNC, NULL, share_key_update}

};

static void init_en_key_pads()
{
	int i,count;
	count = TABLESIZE(en_key_pads);
	for(i=0;i<count;i++)
	{
	   char key[20] = {0};
	   PRECT pRect = NULL;
	   sprintf(key,"RECT_EN_9_KEY_%d",i+1);
		if((NULL != handleCfg) && (pRect = (PRECT) FindMigCfgRectValue(handleCfg, key)) != NULL)
		{
			CopyRect(&en_key_pads[i].bound, pRect);
		}

	}

}
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

static int set_slw_buff(void *key,char *buffer)
{
	char stralp[10] = { 0 };
	char*p = NULL;
	int size = 0;
	St_T9key *stKey = (St_T9key *)key;
	if(NULL == key || NULL == buffer)
	{
		return 0;
	}

	p = alpha_list[stKey->key - '0'];
	size = strlen(p);

	if (stKey->case_mode == PTI_CASE_ABC)
	{
		int i, j;
		for (i = j = 0; i < size; i++)
		{
			stralp[j++] = toupper(p[i]);
			stralp[j++] = ' ';
		}
	}
	else
	{
		int i, j;
		for (i = j = 0; i < size; i++)
		{
			stralp[j++] = p[i];
			stralp[j++] = ' ';
		}
	
	}
	
	strcpy(buffer, stralp);

	return size;
}
static void slw_set_elements (struct _view_window_t* select_window, HWND hwnd)
{
	int i;
	HDC hdc = GetDC(hwnd);
	PLOGFONT oldfont;
	RECT *rc = NULL;
	RECT temRc;
	SIZE textsize;
	SIZE blank_size;
	int interval , textLen,selWinLen;

	RECT r = select_window->bound;
	select_window->element_num = set_slw_buff(select_window->data,select_window->buffer);
	oldfont = SelectFont(hdc, select_window->view_font);
	selWinLen = RECTW(r);
	if(select_window->element_num == 0)
	{
	   select_window->clear_elements(select_window);
	   goto END;
	}
	GetTextExtent(hdc, " ", 1, &blank_size);
	GetTextExtent(hdc, select_window->buffer, strlen(select_window->buffer), &textsize);
	textLen = textsize.cx - blank_size.cx*(select_window->element_num);
	interval = (selWinLen - textLen)/(select_window->element_num+1);
	SetRect(&temRc,0,0,r.left+interval/2,0);
   
	for (textLen=0,i = 0; i < select_window->element_num; i++ )
	{
	   SIZE textsize, cur_size, blank_size;
	   char* substr = NULL;
	   const char* str = NULL;
	   vw_element_t* e = NULL; 
	   RECT* off = &r;
	   
	   get_word (select_window->buffer, i, &select_window->elements[i]);
	   str = select_window->buffer;
	   e = &select_window->elements[i]; 
	   rc = &select_window->elements[i].bound;
	   substr = e->string;
	   select_window->elements[i].len = strlen(substr);
	   
	   GetTextExtent(hdc, substr, strlen(substr), &cur_size);
   
	   SetRect(&temRc,
			   temRc.right,
			   off->top + 1,
			   interval+temRc.right+cur_size.cx,
			   off->bottom);
	   CopyRect(rc,&temRc);
   
	}
	SelectFont(hdc, oldfont);
END:
	ReleaseDC (hdc);
	return;
   
}

static void sw_update (stroke_window_t *sw, HWND hWnd)
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

#ifdef FUFU
		old_tecolor = SetTextColor (hdc, PIXEL_green);
#else
		old_tecolor = SetTextColor (hdc, RGB2Pixel(hdc, 66, 66, 66));
#endif

        old_font = SelectFont(hdc, sw->stroke_font);
        DrawText (hdc, sw->str, -1, &(sw->bound), DT_LEFT | DT_SINGLELINE);
        //SelectFont(hdc, old_font);
        SetTextColor(hdc, old_tecolor);
        
        ReleaseDC(hdc);
    }
}


static void slw_update(view_window_t *slw, HWND hWnd, vw_element_t* element)
{
	int i;
	RECT r;
	static int old_style = 0;
	HDC hdc = GetDC(hWnd);
	RECT rc;
	PLOGFONT oldfont;

	r.left = slw->bound.left;
	r.top = slw->bound.top;
	r.right = slw->bound.right;
	r.bottom = slw->bound.bottom;

	oldfont = SelectFont(hdc, slw->view_font);
	SetBkMode (hdc, BM_TRANSPARENT);
	if (slw->style & VW_DRAW_ELMTS)
	{
		if (element == NULL)
		{
			element = slw->elements;
			EraseBbGround(hWnd, &r);

			for (i = 0; i < slw->element_num; i++)
			{
				//    printf("element[i].string=%s\n", element[i].string);
				//    printf("element[i].bound:l=%d,t=%d,r=%d,b=%d\n", element[i].bound.left,
				//        element[i].bound.top, element[i].bound.right, element[i].bound.bottom);

#ifdef FUFU
				SetTextColor(hdc, COLOR_lightwhite);
#else
				SetTextColor(hdc, RGB2Pixel(hdc, 0x66, 0x66, 0x66));
#endif
				DrawText(hdc, element[i].string, -1, &element[i].bound, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			}
		}
		else
		{
			if (slw->style & VW_EL_PRESSED)
			{

				//SetBrushColor(hdc, COLOR_lightgray);
#if 0				
				SetBrushColor(hdc, RGB2Pixel(hdc, 30, 255, 0));
				SetTextColor(hdc, COLOR_red);
#else

#ifdef FUFU
				SetBrushColor(hdc, RGB2Pixel(hdc, 0x38, 0x39, 0x3A));
				SetTextColor(hdc, COLOR_lightwhite);
#else
				SetBrushColor(hdc, RGB2Pixel(hdc, 0x7a, 0xc1, 0x43));
				SetTextColor(hdc, RGB2Pixel(hdc, 0x66, 0x66, 0x66));
#endif
#endif

				FillBox(hdc, element->bound.left, element->bound.top, element->bound.right - element->bound.left,
						element->bound.bottom - element->bound.top);
				DrawText(hdc, element->string, -1, &element->bound, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

			}
			else
			{
				EraseBbGround(hWnd, &slw->bound);
			}

		}
	}
	else if (old_style & VW_DRAW_ELMTS)
	{
		EraseBbGround(hWnd, &r);
	}

	old_style = slw->style;
	SelectFont(hdc, oldfont);
	ReleaseDC(hdc);
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

#ifdef FUFU
				SetTextColor(hdc, COLOR_lightwhite);
#else
				SetTextColor(hdc, RGB2Pixel(hdc, 0x66, 0x66, 0x66));
#endif

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

#if 0				
				SetBrushColor(hdc, RGB2Pixel(hdc, 30, 255, 0));
				SetTextColor(hdc, COLOR_red);
#else


#ifdef FUFU
				SetBrushColor(hdc, RGB2Pixel(hdc, 0x38, 0x39, 0x3A));
				SetTextColor(hdc, COLOR_lightwhite);
#else
				SetBrushColor(hdc, RGB2Pixel(hdc, 0x7a, 0xc1, 0x43));
				SetTextColor(hdc, RGB2Pixel(hdc, 0x66, 0x66, 0x66));
#endif

#endif

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

			if (hit_rect(key_board->view_window->bound, p))
			{
				vw_proceed_hit(hwnd, key_board->view_window, key_board->stroke_window, &key_board->action, TRUE, p, CN,
						key_board->ime);
				break;
			}

			if (hit_rect(key_board->select_window->bound, p))
			{

				slw_proceed_hit(hwnd, key_board->view_window, key_board->select_window, key_board->stroke_window,
						&key_board->action, TRUE, p, EN, key_board->ime);
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

				kp_proceed_hit(hwnd, key_board->view_window, key_board->select_window, key_board->stroke_window, key,
						&key_board->action, TRUE, p, EN, key_board->ime, wParam, lParam);

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
				SendMessage(hwnd, IME_CLOSE, 0, 0);

				return AC_NULL;
			}

			if (hit_rect(key_board->view_window->bound, p))
			{
				vw_proceed_hit(hwnd, key_board->view_window, key_board->stroke_window, &key_board->action, FALSE, p,
						EN, key_board->ime);
				break;
			}

			if (hit_rect(key_board->select_window->bound, p))
			{
				slw_proceed_hit(hwnd, key_board->view_window, key_board->select_window, key_board->stroke_window,
						&key_board->action, FALSE, p, EN, key_board->ime);
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

				kp_proceed_hit(hwnd, key_board->view_window, key_board->select_window, key_board->stroke_window, key,
						&key_board->action, FALSE, p, EN, key_board->ime, wParam, lParam);
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


static int init_en_stroke_window(HWND hWnd, stroke_window_t *sw)
{
	PRECT pRect = NULL;
	if((NULL != handleCfg) && (pRect = (PRECT) FindMigCfgRectValue(handleCfg, "RECT_SKB_9_SW")) != NULL)
	{
		CopyRect(&sw->bound, pRect);
	}
	else
	{
		SetRect(&sw->bound,SKB_SW_DEFAULT_L,SKB_SW_DEFAULT_T,SKB_SW_DEFAULT_R,SKB_SW_DEFAULT_B);
	}

	memset(sw->str, 0, SW_STR_LEN);

	if(LCD_SIZE_800X480 == getLCDSize())
	{
		sw->stroke_font = CreateLogFont ("ttf", "SourceHanSansCNNormal", "UTF-8", FONT_WEIGHT_BOOK,
			FONT_SLANT_ROMAN, FONT_FLIP_NIL, FONT_OTHER_NIL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 20, 0);
	}
	else if(LCD_SIZE_720X1280 == getLCDSize())
	{
		sw->stroke_font = CreateLogFont ("ttf", "SourceHanSansCNNormal", "UTF-8",FONT_WEIGHT_REGULAR,
			FONT_SLANT_ROMAN, FONT_FLIP_NIL, FONT_OTHER_NIL, FONT_UNDERLINE_NONE,
			FONT_STRUCKOUT_NONE, 24, 0);
	}
	else if(LCD_SIZE_240X320 == getLCDSize() || LCD_SIZE_320X240 == getLCDSize())
	{
		sw->stroke_font = CreateLogFontByName ("*-FixedSys-rrncnn-8-8-ISO8859-1");
	}
	else
	{
		sw->stroke_font = CreateLogFontByName ("*-SansSerif-rrncnn-*-24-ISO8859-1");
	}

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

static int init_en_key_window(HWND hWnd, key_window_t *kw)
{
	int i;
	md_key_t *key;
	PRECT pRect = NULL;
	if((NULL != handleCfg) && (pRect = (PRECT) FindMigCfgRectValue(handleCfg, "RECT_SKB_9_KW")) != NULL)
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
			key->data = (((kw_add_data_t *) (kw->data))->char_key_press);
		}
		else
		{
			key->data = (((kw_add_data_t *) (kw->data))->func_key_press);
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

static int init_en_select_window(HWND hWnd, view_window_t *slw)
{
	PRECT pRect = NULL;
	if((NULL != handleCfg) && (pRect = (PRECT) FindMigCfgRectValue(handleCfg,"RECT_SKB_9_SELECT")) != NULL)
	{
		CopyRect(&slw->bound, pRect);
	}
	else
	{
		SetRect(&slw->bound,SKB_SELECT_DEFAULT_L,SKB_SELECT_DEFAULT_T,SKB_SELECT_DEFAULT_R,SKB_SELECT_DEFAULT_B);
	}
                                        										  
    SetRect(&slw->key_pg_up,0,0,slw->bound.left,0);
    SetRect(&slw->key_pg_down,0,0,slw->bound.right,0);

    slw->update          = slw_update;
    slw->set_elements    = slw_set_elements;
    slw->clear_elements  = vw_clear_elements;
    slw->get_element     = vw_get_element;
	slw->data = (char*)&en_key;
#if 0   //change by caona
    	vw->view_font = CreateLogFont (NULL, "fixed", "GB2312", 
                        FONT_WEIGHT_BOOK, FONT_SLANT_ROMAN, 
                        FONT_SETWIDTH_NORMAL, FONT_OTHER_NIL, 
                        FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 
                        14, 0);
#else

#ifdef SOFTKBD_800_480
	slw->view_font = CreateLogFontByName ("*-SansSerif-rrncnn-*-24-ISO8859-1");
#else
	//slw->view_font = CreateLogFontByName ("*-SansSerif-rrncnn-*-12-ISO8859-1");
//	slw->view_font = CreateLogFontByName ("*-fixed-rrncnn-*-16-ISO8859-1");

#endif
#endif

	if(LCD_SIZE_800X480 == getLCDSize())
	{
		slw->view_font = CreateLogFont ("ttf", "SourceHanSansCNNormal", "UTF-8", FONT_WEIGHT_BOOK,
			FONT_SLANT_ROMAN, FONT_FLIP_NIL, FONT_OTHER_NIL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 20, 0);
	}
	else if(LCD_SIZE_480X800 < getLCDSize())
	{
		slw->view_font = CreateLogFont ("ttf", "SourceHanSansCNNormal", "UTF-8",FONT_WEIGHT_REGULAR,
			FONT_SLANT_ROMAN, FONT_FLIP_NIL, FONT_OTHER_NIL, FONT_UNDERLINE_NONE,
			FONT_STRUCKOUT_NONE, 34, 0);
	}
	else if(LCD_SIZE_480X800 == getLCDSize())
	{
		slw->view_font = CreateLogFont ("ttf", "SourceHanSansCNNormal", "UTF-8",FONT_WEIGHT_REGULAR,
			FONT_SLANT_ROMAN, FONT_FLIP_NIL, FONT_OTHER_NIL, FONT_UNDERLINE_NONE,
			FONT_STRUCKOUT_NONE, 24, 0);
	}
	else if(LCD_SIZE_240X320 == getLCDSize())
	{
		slw->view_font = CreateLogFontByName ("*-SansSerif-rrncnn-*-12-ISO8859-1");
	}
	else
	{
		slw->view_font = CreateLogFontByName("*-fixed-rrncnn-*-12-ISO8859-1");
	}

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
    if (NULL == slw->view_font)
    {
        _MY_PRINTF("create logfont for view window error.\n");
        return -1;
    }

    
    return 0;
}

static void destroy_en_select_window (view_window_t *slw)
{
    
    if (slw->view_font)
    {
        DestroyLogFont(slw->view_font);	
    }
}


static int init_en_view_window(HWND hWnd, view_window_t *vw)
{
	PRECT pRect = NULL;

		
	if((NULL != handleCfg) && (pRect = (PRECT) FindMigCfgRectValue(handleCfg, "RECT_SKB_9_VW")) != NULL)
	{
		CopyRect(&vw->bound, pRect);
	//	vw->bound.bottom += 8;
	}
	else
	{
		SetRect(&vw->bound,SKB_VW_DEFAULT_L,SKB_VW_DEFAULT_T,SKB_VW_DEFAULT_R,SKB_VW_DEFAULT_B+8);
	}

	if((NULL != handleCfg) && (pRect = (PRECT) FindMigCfgRectValue(handleCfg, "RECT_SKB_9_VW_PU")) != NULL)
	{
		CopyRect(&vw->key_pg_up, pRect);
	//	vw->key_pg_up.top += 2;
	//	vw->key_pg_up.bottom += 8;
	}
	else
	{
		SetRect(&vw->key_pg_up,SKB_VW_PU_DEFAULT_L,SKB_VW_PU_DEFAULT_T+2,SKB_VW_PU_DEFAULT_R,SKB_VW_PU_DEFAULT_B+8);
	}

	if((NULL != handleCfg) && (pRect = (PRECT)FindMigCfgRectValue(handleCfg, "RECT_SKB_9_VW_PD")) != NULL)
	{
		CopyRect(&vw->key_pg_down, pRect);
	//	vw->key_pg_down.top += 2;
	//	vw->key_pg_down.bottom += 8;
	}
	else
	{
		SetRect(&vw->key_pg_down,SKB_VW_PD_DEFAULT_L,SKB_VW_PD_DEFAULT_T+2,SKB_VW_PD_DEFAULT_R,SKB_VW_PD_DEFAULT_B+8);
	}

	if (LCD_SIZE_320X240 == getLCDSize())
	{
		vw->max_str_len = 22;
		vw->view_font = CreateLogFontByName("*-fixed-rrncnn-*-16-ISO8859-1");
	}
	else if (LCD_SIZE_480X272 == getLCDSize())
	{
		vw->max_str_len = 30;
		vw->view_font = CreateLogFontByName("*-fixed-rrncnn-*-16-ISO8859-1");
	}
	else if (LCD_SIZE_480X800 == getLCDSize())
	{
		vw->max_str_len = 24;
		vw->view_font = CreateLogFont("ttf", "SourceHanSansCNNormal", "UTF-8",FONT_WEIGHT_REGULAR,
					FONT_SLANT_ROMAN, FONT_FLIP_NIL, FONT_OTHER_NIL, FONT_UNDERLINE_NONE,
					FONT_STRUCKOUT_NONE, 24, 0);
	}
	else if (LCD_SIZE_800X480 == getLCDSize())
	{
		vw->max_str_len = 24;
		vw->view_font = CreateLogFont("ttf", "SourceHanSansCNNormal", "UTF-8", FONT_WEIGHT_BOOK,
					FONT_SLANT_ROMAN, FONT_FLIP_NIL, FONT_OTHER_NIL, FONT_UNDERLINE_NONE,
					FONT_STRUCKOUT_NONE, 20, 0);
	}
	else if (LCD_SIZE_480X800 < getLCDSize())
	{
		vw->max_str_len = 24;
		vw->view_font = CreateLogFont("ttf", "SourceHanSansCNNormal", "UTF-8",FONT_WEIGHT_REGULAR,
					FONT_SLANT_ROMAN, FONT_FLIP_NIL, FONT_OTHER_NIL, FONT_UNDERLINE_NONE,
					FONT_STRUCKOUT_NONE, 36, 0);
	}
	else 
	{
		vw->max_str_len = 20;
		vw->view_font = CreateLogFontByName("*-fixed-rrncnn-*-16-ISO8859-1");
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
		//        free(vw->data);
	}

	if (vw->view_font)
	{
		DestroyLogFont(vw->view_font);
	}
}


int init_en_keypad(const void *handle, HWND hWnd, key_board_t *kb)
{
	int valueCfg = -1;
	make_font();
	handleCfg = handle;

	if((NULL != handleCfg) && (valueCfg = FindMigCfgIntValue(handleCfg, "SKB_WIN_H")) >= 0)
	{
		kb->skb_win_h = valueCfg;
	}
	else
	{
		kb->skb_win_h = SKB_WIN_DEFAULT_H;
	}

	if((NULL != handleCfg) && (valueCfg = FindMigCfgIntValue(handleCfg, "SKB_WIN_W")) >= 0)
	{
		kb->skb_win_w = valueCfg;
	}
	else
	{
		kb->skb_win_w = SKB_WIN_DEFAULT_W;
	}

	SetRectEmpty(&kb->close_rc);

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
	if (init_en_view_window(hWnd, kb->view_window))
	{
		_MY_PRINTF("error for initalizing view window.\n");
		return -1;
	}

	/*initalize for select window.*/
	kb->select_window = (view_window_t *) calloc(1, sizeof(view_window_t));
	if (kb->select_window == NULL)
	{
		_MY_PRINTF("no memory for select window.\n");
		return -1;
	}
	if (init_en_select_window(hWnd, kb->select_window))
	{
		_MY_PRINTF("error for initalizing select window.\n");
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
	if (get_keyboard_bitmap(HDC_SCREEN, (PBITMAP) (&kb->data), EN9_KBD_BKG))
	{
		_MY_PRINTF ("Fail to get en keyboard back ground bitmap.\n");
		return -1;
	}

	kb->update = keyboard_update;

	kb->ime = &en_ime;

	memset(&(kb->action), 0, sizeof(action_t));

	kb->proceed_msg = en_proc_msg;

	kb->clear = clear_keyboard;

	return 0;
}

void destroy_en_keypad (key_board_t *kb)
{
	if(NULL == kb)
	{
		return;
	}

     /*view window*/
    destroy_en_view_window (kb->view_window);
    free(kb->view_window);
	
	destroy_en_select_window (kb->select_window);
	free(kb->select_window);

    /*stroke window*/
    destroy_en_stroke_window (kb->stroke_window);
    free(kb->stroke_window);

    /*ky window*/
    destroy_en_key_window (kb->key_window);
    free(kb->key_window);

    /*add data*/
    //free(kb->data);
}
 
