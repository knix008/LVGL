/**
 * FileName: punct_keypad.c
 *
 * Copyright (C) 2014-2015, ZKSoftware Inc.
 *
 * Created: 2014-8-5
 * Author: wilsn
 *
 * Description:
 */


#include "softkeypad.h"

#ifdef KBD_TOOLTIP
#include "tooltip.h"
#endif
static void *handleCfg;
static int m_nSymbIndex = 0;
const unsigned char symbol_list[4][9] = {
	{'!', '\"', '#', '/', '%','&', '\'', '(', ')'}, 
	{'*', '+', ',', '-', '.', ':', ';', '<', '=' }, 
	{'>', '?', '@', '[', '\\', ']', '^', '_', '`'},
	{'{', '|', '}', '~', '$' ,',', '.', '(',  ')'}
	
};

static md_key_t punct_key_pads[] = {
    { {4,72,59,114}		,'!',    SCANCODE_1,     				KEY_PAD_CHAR, NULL, share_key_update},
    { {63,72,118,114}	,'\"', 	SCANCODE_2,         			KEY_PAD_CHAR, NULL, share_key_update},
    { {122,72,177,114}	,'#', 	SCANCODE_3,         			KEY_PAD_CHAR, NULL, share_key_update},
    { {181,72,236,114}	,' ',    SCANCODE_BACKSPACE,     		KEY_PAD_FUNC, NULL, share_key_update},
    { {4,119,59,161}	,'/', 	SCANCODE_4,         			KEY_PAD_CHAR, NULL, share_key_update},
    { {63,119,118,161}	,'%', 	SCANCODE_5,         			KEY_PAD_CHAR, NULL, share_key_update},
    { {122,119,177,161}	,'&', 	SCANCODE_6,         			KEY_PAD_CHAR, NULL, share_key_update},
    { {181,119,236,161}	,' ',    SCANCODE_CURSORBLOCKUP,        	KEY_PAD_FUNC, NULL, share_key_update},
    { {4,166,59,208}	,'\'',	SCANCODE_7,         			KEY_PAD_CHAR, NULL, share_key_update},
    { {63,166,118,208}	,'(', 	SCANCODE_8,         			KEY_PAD_CHAR, NULL, share_key_update},
    { {122,166,177,208}	,')',	SCANCODE_9,         			KEY_PAD_CHAR, NULL, share_key_update},
    { {181,166,236,208}	,' ',    SCANCODE_CURSORBLOCKDOWN,      	KEY_PAD_FUNC, NULL, share_key_update},
    { {4,213,59,260}	,' ',    SCANCODE_ESCAPE,        		KEY_PAD_FUNC, NULL, share_key_update},
    { {63,213,118,260}	,' ',    SCANCODE_SPACE,         		KEY_PAD_FUNC, NULL, share_key_update},
    { {122,213,177,260}	,' ',    SCANCODE_TOEN,          		KEY_PAD_FUNC, NULL, share_key_update},
    { {181,213,236,260}	,' ',    SCANCODE_ENTER,         		KEY_PAD_FUNC, NULL, share_key_update}

};

static void init_punct_key_pads()
{
	int i, count;
	count = TABLESIZE(punct_key_pads);
	for (i = 0; i < count; i++)
	{
		char key[20] = { 0 };
		PRECT pRect = NULL;
		sprintf(key, "RECT_EN_9_KEY_%d", i + 1);
		if((NULL != handleCfg) && (pRect = (PRECT) FindMigCfgRectValue(handleCfg, key)) != NULL)
		{
			CopyRect(&punct_key_pads[i].bound, pRect);
		}
	}

}

static md_key_t* get_punct_key(key_window_t *kw, POINT point)
{
	int i;
	md_key_t* keys = kw->key;

	for (i = 0; i < kw->key_num; i++)
	{
		if (PtInRect(&keys[i].bound, point.x, point.y))
		{
			return &keys[i];
		}
	}

	return NULL;
}

static void kw_update(key_window_t *kw, HWND hWnd)
{
	int i, j;

	for (i = 0, j = 0; i < kw->key_num; i++)
	{
		md_key_t* key = &kw->key[i];
		if (key->style == KEY_PAD_CHAR)
		{
			key->key_char = symbol_list[m_nSymbIndex][j];
			j++;
		}
	}

}

static int get_kpd_sysbol_bitmap (HDC hdc, PBITMAP* pbmp, int id)
{
	if (id  < 0 || id > PUNCT_KBD_NUM) 
    {
    	return -1;
	}
	get_keyboard_bitmap(hdc,pbmp,PUNCT9_KBD_BKG0+id);
	
    return 0;
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
		old_tecolor = SetTextColor (hdc, PIXEL_black);
		old_font = SelectFont(hdc, sw->stroke_font);
		DrawText (hdc, sw->str, -1, &(sw->bound), DT_LEFT);
		SetTextColor(hdc, old_tecolor);
		ReleaseDC(hdc);
	}
}


static int symbol_kpd_proc_msg(key_board_t* key_board, HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
	static char lbuttondown = 0;
	static POINT p;
	md_key_t *key;

	switch (message)
	{
		case MSG_LBUTTONDOWN:
		{
			p.x = LOSWORD(lParam);
			p.y = HISWORD(lParam);
			lbuttondown = 1;
			break;
		}
		case MSG_LBUTTONUP:
		{
			if (lbuttondown == 0)
			{
				break;
			}

			lbuttondown = 0;
			p.x = LOSWORD(lParam);
			p.y = HISWORD(lParam);

			if (hit_rect(key_board->key_window->bound, p))
			{
				key = key_board->key_window->get_key(key_board->key_window, p);
				if (key == NULL)
				{
					goto RETURN;
				}
				else if (key->style & KEY_PAD_CHAR && key->key_char == ' ')
				{
					key->style &= ~KEY_PAD_PRESSED;
					key->update(key, hwnd);
					goto RETURN;
				}
				else if (key->scan_code == SCANCODE_CURSORBLOCKUP)
				{
					key->style &= ~KEY_PAD_PRESSED;
					key->update(key, hwnd);

					if (m_nSymbIndex > 0)
					{
						m_nSymbIndex--;
						kw_update(key_board->key_window, hwnd);
						get_kpd_sysbol_bitmap(HDC_SCREEN, (PBITMAP) (&key_board->data), m_nSymbIndex);
						PostMessage(hwnd, MSG_ERASEBKGND, 0, 0L);
					}
					goto RETURN;
				}
				else if (key->scan_code == SCANCODE_CURSORBLOCKDOWN)
				{
					key->style &= ~KEY_PAD_PRESSED;
					key->update(key, hwnd);

					if (m_nSymbIndex < TABLESIZE(symbol_list) - 1)
					{
						m_nSymbIndex++;
						kw_update(key_board->key_window, hwnd);
						get_kpd_sysbol_bitmap(HDC_SCREEN, (PBITMAP) (&key_board->data), m_nSymbIndex);
						PostMessage(hwnd, MSG_ERASEBKGND, 0, 0L);
					}
					goto RETURN;
				}
				else if (key->scan_code == SCANCODE_TOEN)
				{
					m_nSymbIndex = 0;
					kw_update(key_board->key_window, hwnd);
					get_kpd_sysbol_bitmap(HDC_SCREEN, (PBITMAP) (&key_board->data), 0);
				}

				break;
			}
			break;
		}
	}

	return symbol_proc_msg(key_board, hwnd, message, wParam, lParam);

RETURN:
	key_board->action.operation = AC_NULL;
	return AC_NULL;

}

static int init_punct_stroke_window(HWND hWnd, stroke_window_t *sw)
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
	sw->stroke_font = CreateLogFontByName("*-fixed-rrncnn-*-24-ISO8859-1");
	if (NULL == sw->stroke_font)
	{
		_MY_PRINTF("create logfont for stroke window error.\n");
		return -1;
	}
	sw->update = sw_update;

	return 0;
}

static void destroy_punct_stroke_window (stroke_window_t *sw)
{
	if (sw->stroke_font)
	{
		DestroyLogFont (sw->stroke_font);
	}
	return;
}

/* return 0 on succes , less than 0 on error */
static int init_punct_key_window(HWND hWnd, key_window_t *kw)
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

	kw->key = punct_key_pads;
	kw->key_num = TABLESIZE(punct_key_pads);

	kw->get_key = get_punct_key;

	//if (NULL == (kw->data = (void *)calloc (1, sizeof(BITMAP)))){
	if (NULL == (kw->data = (void *) calloc(1, sizeof(kw_add_data_t))))
	{
		_MY_PRINTF(stderr, "no memory for key window add data.\n");
		return -1;
	}

	if (get_keyboard_bitmap(HDC_SCREEN, &(((kw_add_data_t *) (kw->data))->char_key_press), CHAR_KEY_MASK))
	{
		_MY_PRINTF (stderr, "Fail to get key mask bitmap.\n");
		return -1;
	}

	if (get_keyboard_bitmap(HDC_SCREEN, &(((kw_add_data_t *) (kw->data))->func_key_press), FUNC_KEY_MASK))
	{
		_MY_PRINTF (stderr, "Fail to get key mask bitmap.\n");
		return -1;
	}

	kw->update = kw_update;
	//kw->update = NULL;

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

static void destroy_punct_key_window (key_window_t *kw)
{
    if (kw->data)
    {
//        free(kw->data);
    }
}

int init_punct_keypad(const void *handle, HWND hWnd, key_board_t *kb)
{
	int valueCfg = -1;
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

	kb->view_window == NULL;
	kb->stroke_window = NULL;

	SetRectEmpty(&kb->close_rc);
	if(NULL != handleCfg)
	{
		init_punct_key_pads();
	}

	/*initalize for key window.*/
	kb->key_window = (key_window_t *) calloc(1, sizeof(key_window_t));
	if (kb->key_window == NULL)
	{
		_MY_PRINTF("no memory for key window.\n");
		return -1;
	}
	if (0 != init_punct_key_window(hWnd, kb->key_window))
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

	if (get_keyboard_bitmap(HDC_SCREEN, (PBITMAP) (&kb->data), PUNCT9_KBD_BKG0))
	{
		_MY_PRINTF ("Fail to get key mask bitmap.\n");
		return -1;
	}

	kb->update = keyboard_update;

	kb->ime = NULL;

	memset(&(kb->action), 0, sizeof(action_t));

	kb->proceed_msg = symbol_kpd_proc_msg;
	kb->clear = clear_keyboard;

	return 0;
}

void destroy_punct_keypad (key_board_t *kb)
{
	if(NULL == kb)
	{
		return;
	}

    /*stroke window*/
   // destroy_punct_stroke_window (kb->stroke_window);
   // free(kb->stroke_window);

    /*ky window*/
    destroy_punct_key_window (kb->key_window);
    free(kb->key_window);

    /*add data*/
    //free(kb->data);
}

 










