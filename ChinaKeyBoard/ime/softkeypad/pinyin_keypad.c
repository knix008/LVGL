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


#include "softkeypad.h"
#include "../libime/ime.h"
#include "pyTb.h"

#ifdef KBD_TOOLTIP
#include "tooltip.h"
#endif

static char fontname[32] = { 0 };
static void *handleCfg;
static St_T9key gPy_key ;

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
	InitFreeTypeFonts ();
	LoadDevFontFromFile("ttf-noname-rrncnn-0-0-ISO8859-1",
			"ttf-fonts/cn.ttf", &devfont);
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

static md_key_t py_key_pads[] = {
    { {4,72,59,114}		,'1',    SCANCODE_1,     KEY_PAD_CHAR, NULL, share_key_update},
    { {63,72,118,114}	,'2', 	SCANCODE_2,         	KEY_PAD_CHAR, NULL, share_key_update},
    { {122,72,177,114}	,'3', 	SCANCODE_3,         	KEY_PAD_CHAR, NULL, share_key_update},
    { {181,72,236,114}	,' ',    SCANCODE_BACKSPACE,     KEY_PAD_FUNC, NULL, share_key_update},
    { {4,119,59,161}	,'4', 	SCANCODE_4,         	KEY_PAD_CHAR, NULL, share_key_update},
    { {63,119,118,161}	,'5', 	SCANCODE_5,         	KEY_PAD_CHAR, NULL, share_key_update},
    { {122,119,177,161} ,'6', 	SCANCODE_6,         	KEY_PAD_CHAR, NULL, share_key_update},
    { {181,119,236,161} ,' ',    SCANCODE_CURSORBLOCKUP,        KEY_PAD_FUNC, NULL, share_key_update},
    { {4,166,59,208}	,'7',	SCANCODE_7,         	KEY_PAD_CHAR, NULL, share_key_update},
    { {63,166,118,208}	,'8', 	SCANCODE_8,         	KEY_PAD_CHAR, NULL, share_key_update},
    { {122,166,177,208} ,'9',	SCANCODE_9,         	KEY_PAD_CHAR, NULL, share_key_update},
    { {181,166,236,208} ,' ',    SCANCODE_CURSORBLOCKDOWN,      KEY_PAD_FUNC, NULL, share_key_update},
    { {4,213,59,260}	,' ',    SCANCODE_ESCAPE,        KEY_PAD_FUNC, NULL, share_key_update},
    { {63,213,118,260}	,' ',    SCANCODE_SPACE,         KEY_PAD_FUNC, NULL, share_key_update},
    { {122,213,177,260} ,' ',    SCANCODE_TONUM,          KEY_PAD_FUNC, NULL, share_key_update},
    { {181,213,236,260} ,' ',    SCANCODE_ENTER,         KEY_PAD_FUNC, NULL, share_key_update}

};

static int cb_pinyin_match_translate_keystrokes(const char *strokes, char *buffer, int buffer_len, int index, int mode)
{
	int i;
	int cursor = -1;
	int len = strlen(strokes);
	char bufStrokes[16] = {0};

	if(NULL == strokes || NULL == buffer)
	{
		return -1;
	}
	
	for (i = 0; i < len; i++) 
	{
	   bufStrokes[i] = strokes[i];
		   /* the candidates will be stored in buff */
	   cursor = cb_pinyin_translate_word (bufStrokes, buffer, 
			   buffer_len,index, mode);
	}
	if(-1 == cursor)
	{
		buffer[0] = '\0';
	}
    return cursor;
}


static md_ime_t py_ime = {
    100,
    "pinyin",
    NULL,
	cb_pinyin_match_translate_keystrokes,
	cb_pinyin_predict_word,
	cb_pinyin_init
};

static void init_py_key_pads()
{
	int i, count;
	count = TABLESIZE(py_key_pads);
	for (i = 0; i < count; i++)
	{
		char key[20] = { 0 };
		PRECT pRect = NULL;
		sprintf(key, "RECT_EN_9_KEY_%d", i + 1);
		if((NULL != handleCfg) && (pRect = (PRECT) FindMigCfgRectValue(handleCfg, key)) != NULL)
		{
			CopyRect(&py_key_pads[i].bound, pRect);
		}
	}
}

static md_key_t* get_py_key (key_window_t *kw, POINT point)
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

static inline int binSearch(int x, T9PY_IDX a[], int n)
{
   int low, high, mid,iValue;
   low = 0;
   high = n-1;
   //注意，这里必须用<=, 用<不对，一直返回-1
   while(low <= high)
   {
      mid = (low + high) / 2;
      iValue = atoi(a[mid].key);
      if(x < iValue)
	  {
    	  high = mid - 1;
	  }
	  else if(x > iValue)
	  {
		  low = mid + 1;
	  }
	  else
	  {
		  return mid;
	  }
   }
   return -1;
}

static int set_slw_buff(void *key,char *buffer)
{
	char*p = NULL;
	int iStrLen=0,iValue;
	int index = -1;
	St_T9key *pyKey = (St_T9key *)key;
	if(NULL == key || NULL == buffer)
	{
		return 0;
	}

	iStrLen = strlen(pyKey->szpy);
	if(pyKey->key > '1' && pyKey->key <= '9')
	{
		pyKey->szpy[iStrLen] = pyKey->key;
	}

	iValue = atoi(pyKey->szpy);
	if(iValue < 2 )
	{
		memset(pyKey,0,sizeof(St_T9key));
		memset(buffer,0,sizeof(buffer));
		return 0;
	}

	index = binSearch(iValue,gPyTb,T9PY_IDX_ROW);

	if(index >= 0)
	{
		strcpy(buffer,gPyTb[index].Letter);
	}
	else
	{
		pyKey->szpy[iStrLen] = '\0';
	}

	return get_words_num(buffer);
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
	St_T9key *pyKey = (St_T9key *)select_window->data;
	LPARAM lParam;
	
	RECT r = select_window->bound;
	select_window->element_num = set_slw_buff(pyKey,select_window->buffer);
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
	if(select_window->element_num >= 3)
	{
		interval = (selWinLen - textLen)/(select_window->element_num);
		SetRect(&temRc,0,0,r.left,0);
	}
	else
	{
		interval = (selWinLen - textLen)/(select_window->element_num+2);
		SetRect(&temRc,0,0,r.left + (interval*2)/2,0);
	}
	
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
	
	if(NULL != pyKey && pyKey->element_index < select_window->element_num)
	{	
		int x,y;
		PRECT prect;
		
		prect = &select_window->elements[pyKey->element_index].bound;
		x = prect->left + RECTW(*prect)/2;
		y = prect->top+ RECTH(*prect)/2;
		lParam =MAKELONG(x,y);
		PostMessage(hwnd,MSG_LBUTTONDOWN,0,lParam);
	}

END:
	SelectFont(hdc, oldfont);
	ReleaseDC (hdc);
	return;
   
}

static void slw_clear_elements (struct _view_window_t* view_window)
{
	
    memset (view_window->elements, 0, sizeof(view_window->elements));
    view_window->element_num = 0;
	memset(&gPy_key,0,sizeof(St_T9key));
	
    return;
}


static void kw_update (key_window_t *kw, HWND hWnd)
{
    //TODO
    //printf("==== update key window ====\n");
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
		if (slw->element_num > 0)
		{
			EraseBbGround(hWnd, &r);

			for (i = 0; i < slw->element_num; i++)
			{

#ifdef FUFU
				SetTextColor(hdc, COLOR_lightwhite);
#else
				SetTextColor(hdc, RGB2Pixel(hdc, 0x66, 0x66, 0x66));
#endif

				DrawText(hdc, slw->elements[i].string, -1, &slw->elements[i].bound, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
			}
		}
		
		if (element != NULL)
		{
			if (slw->style & VW_EL_PRESSED)
			{

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


static void sw_update(stroke_window_t *sw, HWND hWnd)
{
	HDC hdc;
	PLOGFONT old_font;
	gal_pixel old_tecolor;

	EraseBbGround(hWnd, &sw->bound);
/*
	if (strlen(sw->str) != 0)
	{
		hdc = GetDC(hWnd);

		SetBkMode (hdc, BM_TRANSPARENT);
		//old_tecolor = SetTextColor (hdc, PIXEL_lightwhite);

		old_tecolor = SetTextColor (hdc, PIXEL_green);

		old_font = SelectFont(hdc, sw->stroke_font);
		DrawText (hdc, sw->str, -1, &(sw->bound), DT_LEFT);
		SelectFont(hdc, old_font);
		SetTextColor(hdc, old_tecolor);

		ReleaseDC(hdc);
	}
*/
	if ((strlen(sw->str) == 0) && (NULL != sw->data))
	{
		view_window_t *select_window = (view_window_t *)sw->data;
		if (select_window->buffer[0])
		{
			select_window->buffer[0] = '\0';
			select_window->style = 0;
			if (select_window->clear_elements)
			{
				select_window->clear_elements(select_window);
			}
			select_window->update(select_window, hWnd, NULL);
		}
		
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
				DrawText(hdc, element[i].string, -1, &element[i].bound, 0);
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
					//SetBrushColor(hdc, RGB2Pixel(hdc, 120, 187, 250));
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
				DrawText(hdc, element[i].string, -1, &element[i].bound, 0);
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
	//SelectFont(hdc, oldfont);
	ReleaseDC(hdc);
}

static int pinyin_proceed_msg(key_board_t* key_board, HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
	static RECT old_hit_rect;
	static int lbuttondown = 0;
	static POINT p;

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
						&key_board->action, TRUE, p, CN, key_board->ime);
				break;
			}

			if (hit_rect(key_board->key_window->bound, p))
			{
				md_key_t* key = key_board->key_window->get_key(key_board->key_window, p);
				if (key == NULL)
				{
					key_board->action.operation = AC_NULL;
					break;
				}
				kp_proceed_hit(hwnd, key_board->view_window, key_board->select_window, key_board->stroke_window, key,
						&key_board->action, TRUE, p, CN, key_board->ime, wParam, lParam);

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
						CN, key_board->ime);
				break;
			}

			if (hit_rect(key_board->select_window->bound, p))
			{

				slw_proceed_hit(hwnd, key_board->view_window, key_board->select_window, key_board->stroke_window,
						&key_board->action, FALSE, p, CN, key_board->ime);
				break;
			}

			if (hit_rect(key_board->key_window->bound, p))
			{
				md_key_t* key = key_board->key_window->get_key(key_board->key_window, p);
				if (key == NULL || key->key_char == '1')
				{
					key_board->action.operation = AC_NULL;
					break;
				}

				kp_proceed_hit(hwnd, key_board->view_window, key_board->select_window, key_board->stroke_window, key,
						&key_board->action, FALSE, p, CN, key_board->ime, wParam, lParam);

				break;
			}
			break;
		}
		case MSG_MOUSEMOVE:
		{
			p.x = LOSWORD(lParam);
			p.y = HISWORD(lParam);
			proceed_move(hwnd, key_board, lbuttondown, p, CN);
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

static int init_py_view_window(HWND hWnd, view_window_t *vw)
{
	PRECT pRect = NULL;
	int lcdType = 0;

	if((NULL != handleCfg) && (pRect = (PRECT) FindMigCfgRectValue(handleCfg, "RECT_SKB_9_VW")) != NULL)
	{
		CopyRect(&vw->bound, pRect);
		vw->bound.bottom += 8;
	}
	else
	{
		SetRect(&vw->bound,SKB_VW_DEFAULT_L,SKB_VW_DEFAULT_T,SKB_VW_DEFAULT_R,SKB_VW_DEFAULT_B+8);
	}

	if((NULL != handleCfg) && (pRect = (PRECT) FindMigCfgRectValue(handleCfg, "RECT_SKB_9_VW_PU")) != NULL)
	{
		CopyRect(&vw->key_pg_up, pRect);
		vw->key_pg_up.top+= 2;
		vw->key_pg_up.bottom += 8;
	}
	else
	{
		SetRect(&vw->key_pg_up,SKB_VW_PU_DEFAULT_L,SKB_VW_PU_DEFAULT_T+2,SKB_VW_PU_DEFAULT_R,SKB_VW_PU_DEFAULT_B+8);
	}

	if((NULL != handleCfg) && (pRect = (PRECT)FindMigCfgRectValue(handleCfg, "RECT_SKB_9_VW_PD")) != NULL)
	{
		CopyRect(&vw->key_pg_down, pRect);
		vw->key_pg_down.top+= 2;
		vw->key_pg_down.bottom += 8;
	}
	else
	{
		SetRect(&vw->key_pg_down,SKB_VW_PD_DEFAULT_L,SKB_VW_PD_DEFAULT_T+2,SKB_VW_PD_DEFAULT_R,SKB_VW_PD_DEFAULT_B+8);
	}

	lcdType = getLCDSize();
	switch (lcdType)
	{
		case LCD_SIZE_320X240:
		{
			vw->max_str_len = 22;			
			vw->view_font = CreateLogFont ("upf", "fmsong", "GBK", FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, \
					FONT_FLIP_NIL, FONT_OTHER_NIL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 12, 0);
			break;
		}
		case LCD_SIZE_480X272:
		{
			vw->max_str_len = 28;
			vw->view_font = CreateLogFont ("upf", "fmsong", "GBK", FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, \
					FONT_FLIP_NIL, FONT_OTHER_NIL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 12, 0);
			break;
		}
		case LCD_SIZE_480X800:
		{
			vw->max_str_len = 28;
			vw->view_font = CreateLogFont ("ttf", "SourceHanSansCNNormal", "GB2312-0", FONT_WEIGHT_BOOK, FONT_SLANT_ROMAN, \
					FONT_FLIP_NIL, FONT_OTHER_NIL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 24, 0);
			break;
		}
		case LCD_SIZE_800X1280:
		{
			vw->max_str_len = 28;
			vw->view_font = CreateLogFont ("ttf", "SourceHanSansCNNormal", "GB2312-0", FONT_WEIGHT_BOOK, FONT_SLANT_ROMAN, \
					FONT_FLIP_NIL, FONT_OTHER_NIL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 30, 0);
		}
		case LCD_SIZE_720X1280:
		{
			vw->max_str_len = 28;
			vw->view_font = CreateLogFont ("ttf", "SourceHanSansCNNormal", "GB2312-0", FONT_WEIGHT_BOOK, FONT_SLANT_ROMAN, \
					FONT_FLIP_NIL, FONT_OTHER_NIL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 36, 0);
		}
		case LCD_SIZE_240X320:
		{
			vw->max_str_len = 22;
			vw->view_font = CreateLogFont ("upf", "fmsong", "GBK", FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN, \
					FONT_FLIP_NIL, FONT_OTHER_NIL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 12, 0);
		}
		case LCD_SIZE_800X480:
		{
			vw->max_str_len = 20;
			vw->view_font = CreateLogFont ("ttf", "SourceHanSansCNNormal", "GB2312-0", FONT_WEIGHT_BOOK, FONT_SLANT_ROMAN,
					FONT_FLIP_NIL, FONT_OTHER_NIL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 20, 0);
		}
		default: 
		{
			vw->max_str_len = 15;
			vw->view_font = CreateLogFont ("upf", "fmsong", "GBK", FONT_WEIGHT_REGULAR, FONT_SLANT_ROMAN,
								FONT_FLIP_NIL, FONT_OTHER_NIL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 12, 0);
			break;
		}
	}

	vw->update = vw_update;
	vw->set_elements = vw_set_elements;
	vw->clear_elements = vw_clear_elements;
	vw->get_element = vw_get_element;


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
	 if (get_kpd_bitmap(HDC_SCREEN,
	 &(((vw_add_data_t *)(vw->data))->sel_bk),
	 SELECT_TEXT_BKGND)) {
	 _MY_PRINTF ("Fail to get sellect text bkgnd bitmap.\n");
	 return -1;
	 }
	 */
	return 0;
}

static void destroy_py_view_window (view_window_t *vw)
{

	if (vw->data)
	{
		//free(vw->data);
	}

	if (vw->view_font)
	{
		DestroyLogFont(vw->view_font);
	}
}

static int init_py_select_window(HWND hWnd, view_window_t *slw)
{
	PRECT pRect = NULL;
	int lcdType = 0;

	if((NULL != handleCfg) && (pRect = (PRECT) FindMigCfgRectValue(handleCfg,"RECT_SKB_9_SELECT")) != NULL)
	{
		CopyRect(&slw->bound, pRect);
	}
	else
	{
		SetRect(&slw->bound,SKB_SELECT_DEFAULT_L,SKB_SELECT_DEFAULT_T,SKB_SELECT_DEFAULT_R,SKB_SELECT_DEFAULT_B);
	}

	SetRect(&slw->key_pg_up, 0, 0, slw->bound.left, 0);
	SetRect(&slw->key_pg_down, 0, 0, slw->bound.right, 0);

	slw->update = slw_update;
	slw->set_elements = slw_set_elements;
	slw->clear_elements = slw_clear_elements;
	slw->get_element = vw_get_element;
	slw->data = &gPy_key;

	lcdType = getLCDSize();
	if(LCD_SIZE_480X272 == lcdType)
	{
		slw->view_font = CreateLogFontByName("*-fixed-rrncnn-*-16-ISO8859-1");
	}
	else if(LCD_SIZE_800X480 == lcdType)
	{
		slw->view_font = CreateLogFont ("ttf", "SourceHanSansCNNormal", "UTF-8", FONT_WEIGHT_BOOK, FONT_SLANT_ROMAN,
						FONT_FLIP_NIL, FONT_OTHER_NIL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 20, 0);
	}
	else if(LCD_SIZE_480X800 < lcdType)
	{
		slw->view_font = CreateLogFont ("ttf", "SourceHanSansCNNormal", "UTF-8", FONT_WEIGHT_BOOK, FONT_SLANT_ROMAN,
						FONT_FLIP_NIL, FONT_OTHER_NIL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 34, 0);
	}
	else if(LCD_SIZE_480X800 == lcdType)
	{
		slw->view_font = CreateLogFont ("ttf", "SourceHanSansCNNormal", "UTF-8", FONT_WEIGHT_BOOK, FONT_SLANT_ROMAN,
						FONT_FLIP_NIL, FONT_OTHER_NIL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 24, 0);
	}
	else 
	{
		slw->view_font = CreateLogFontByName ("*-FixedSys-rrncnn-8-16-ISO8859-1");
	}

	if (NULL == slw->view_font)
	{
		_MY_PRINTF("create logfont for view window error.\n");
		return -1;
	}

	return 0;
}

static void destroy_py_select_window (view_window_t *slw)
{
    if (slw->view_font)
    {
        DestroyLogFont(slw->view_font);	
    }
}

static int init_py_stroke_window(HWND hWnd, stroke_window_t *sw)
{
	PRECT pRect = NULL;
	int lcdType = getLCDSize();

	if((NULL != handleCfg) && (pRect = (PRECT) FindMigCfgRectValue(handleCfg, "RECT_SKB_9_SW")) != NULL)
	{
		CopyRect(&sw->bound, pRect);
	}
	else
	{
		SetRect(&sw->bound,SKB_SW_DEFAULT_L,SKB_SW_DEFAULT_T,SKB_SW_DEFAULT_R,SKB_SW_DEFAULT_B);
	}
	memset(sw->str, 0, SW_STR_LEN);

	if(LCD_SIZE_480X272 == lcdType)
	{
		sw->stroke_font = CreateLogFontByName ("*-FixedSys-rrncnn-8-8-ISO8859-1");
	}
	else if(LCD_SIZE_320X240 == lcdType)
	{
		sw->stroke_font = CreateLogFontByName("*-SansSerif-rrncnn-*-8-ISO8859-1");
	}
	else if(LCD_SIZE_480X800 == lcdType)
	{
		sw->stroke_font = CreateLogFont ("ttf", "SourceHanSansCNNormal", "UTF-8",FONT_WEIGHT_REGULAR,
			FONT_SLANT_ROMAN, FONT_FLIP_NIL, FONT_OTHER_NIL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 20, 0);
	}
	else if(LCD_SIZE_800X480 == lcdType)
	{
		sw->stroke_font = CreateLogFont ("ttf", "SourceHanSansCNNormal", "UTF-8", FONT_WEIGHT_BOOK,
			FONT_SLANT_ROMAN, FONT_FLIP_NIL, FONT_OTHER_NIL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 20, 0);
	}
	else if(LCD_SIZE_480X800 < lcdType)
	{
		sw->stroke_font = CreateLogFont ("ttf", "SourceHanSansCNNormal", "UTF-8",FONT_WEIGHT_REGULAR,
			FONT_SLANT_ROMAN, FONT_FLIP_NIL, FONT_OTHER_NIL, FONT_UNDERLINE_NONE, FONT_STRUCKOUT_NONE, 32, 0);
	}
	else 
	{
		sw->stroke_font = CreateLogFontByName("*-SansSerif-rrncnn-*-12-ISO8859-1");
	}

	if (NULL == sw->stroke_font)
	{
		_MY_PRINTF("create logfont for stroke window error.\n");
		return -1;
	}

	sw->update = sw_update;

	return 0;
}

static void destroy_py_stroke_window (stroke_window_t *sw)
{
    if (sw->stroke_font)
    {
    	DestroyLogFont (sw->stroke_font);
    }
    
    return;
}

static int init_py_key_window(HWND hWnd, key_window_t *kw)
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

	kw->key = py_key_pads;
	kw->key_num = TABLESIZE(py_key_pads);

	kw->get_key = get_py_key;

	if (NULL == (kw->data = (void *) calloc(1, sizeof(kw_add_data_t))))
	{
		_MY_PRINTF("no memory for key window add data.\n");
		return -1;
	}

	if (get_keyboard_bitmap(HDC_SCREEN, &(((kw_add_data_t *) (kw->data))->char_key_press), CHAR_KEY_MASK))
	{
		_MY_PRINTF ("Fail to get py char key mask bitmap.\n");
		return -1;
	}

	if (get_keyboard_bitmap(HDC_SCREEN, &(((kw_add_data_t *) (kw->data))->func_key_press), FUNC_KEY_MASK))
	{
		_MY_PRINTF ("Fail to get py func key mask bitmap.\n");
		return -1;
	}

	kw->update = kw_update;

	/*use the shared bitmap for press key painting.*/
	key = kw->key;
	for (i = 0; i < kw->key_num; i++)
	{
		if (key->style == KEY_PAD_CHAR)
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

static void destroy_py_key_window (key_window_t *kw)
{
    if (kw->data)
    {
     //   free(kw->data);
    }
}

int init_py_keypad(const void *handle, HWND hWnd, key_board_t *kb)
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
		init_py_key_pads();
	}
	/*initalize for view window.*/
	kb->view_window = (view_window_t *) calloc(1, sizeof(view_window_t));
	if (kb->view_window == NULL)
	{
		_MY_PRINTF("no memory for view window.\n");
		return -1;
	}
	if (0 != init_py_view_window(hWnd, kb->view_window))
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
	if (init_py_select_window(hWnd, kb->select_window))
	{
		_MY_PRINTF("error for initalizing select window.\n");
		return -1;
	}

	/*initalize for stroke window.*/
	kb->stroke_window = (stroke_window_t *) calloc(1, sizeof(stroke_window_t));
	if (kb->stroke_window == NULL)
	{
		_MY_PRINTF("no memory for view window.\n");
		return -1;
	}

	if (0 != init_py_stroke_window(hWnd, kb->stroke_window))
	{
		_MY_PRINTF("error for initalizing stroke window.\n");
		return -1;
	}

	kb->stroke_window->data = kb->select_window;

	/*initalize for key window.*/
	kb->key_window = (key_window_t *) calloc(1, sizeof(key_window_t));
	if (kb->key_window == NULL)
	{
		_MY_PRINTF("no memory for key window.\n");
		return -1;
	}
	if (0 != init_py_key_window(hWnd, kb->key_window))
	{
		_MY_PRINTF("error for initalizing key window.\n");
		return -1;
	}

	/*initalize for bk image.
	 kb->data = calloc (1, sizeof(BITMAP));
	 if(kb->data == NULL){
	 _MY_PRINTF("no memory for bk image.\n");
	 return -1;
	 }
	 */

	if (get_keyboard_bitmap(HDC_SCREEN, (PBITMAP) (&kb->data), PINYIN9_KBD_BKG))
	{
		_MY_PRINTF ("Fail to get pinyin kbd bkgnd bitmap.\n");
		return -1;
	}

	kb->update = keyboard_update;

	kb->ime = &py_ime;
	kb->ime->init(NULL);

	memset(&(kb->action), 0, sizeof(action_t));

	kb->proceed_msg = pinyin_proceed_msg;
	kb->clear = clear_keyboard;

	return 0;
}

void destroy_py_keypad (key_board_t *kb)
{
	if(NULL == kb)
	{
		return;
	}

    /*view window*/
    destroy_py_view_window (kb->view_window);
    free(kb->view_window);

    /*select window*/
    destroy_py_select_window (kb->select_window);
    free(kb->select_window);

    /*stroke window*/
    destroy_py_stroke_window (kb->stroke_window);
    free(kb->stroke_window);

    /*ky window*/
    destroy_py_key_window (kb->key_window);
    free(kb->key_window);

    /*add data*/
    //free(kb->data);
}

