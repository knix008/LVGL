/**
 * FileName: num_keypad.c
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

static md_key_t num_key_pads[] = {
    { {4,72,59,114}		,'1',    SCANCODE_1,     				KEY_PAD_CHAR, NULL, share_key_update},
    { {63,72,118,114}	,'2', 	SCANCODE_2,         			KEY_PAD_CHAR, NULL, share_key_update},
    { {122,72,177,114}	,'3', 	SCANCODE_3,         			KEY_PAD_CHAR, NULL, share_key_update},
    { {181,72,236,114}	,' ',    SCANCODE_BACKSPACE,     		KEY_PAD_FUNC, NULL, share_key_update},
    { {4,119,59,161}	,'4', 	SCANCODE_4,         			KEY_PAD_CHAR, NULL, share_key_update},
    { {63,119,118,161}	,'5', 	SCANCODE_5,         			KEY_PAD_CHAR, NULL, share_key_update},
    { {122,119,177,161} ,'6', 	SCANCODE_6,         			KEY_PAD_CHAR, NULL, share_key_update},
    { {181,119,236,161} ,' ',    SCANCODE_CURSORBLOCKUP,        	KEY_PAD_FUNC, NULL, share_key_update},
    { {4,166,59,208}	,'7',	SCANCODE_7,         			KEY_PAD_CHAR, NULL, share_key_update},
    { {63,166,118,208}	,'8', 	SCANCODE_8,         			KEY_PAD_CHAR, NULL, share_key_update},
    { {122,166,177,208} ,'9',	SCANCODE_9,         			KEY_PAD_CHAR, NULL, share_key_update},
    { {181,166,236,208} ,' ',    SCANCODE_CURSORBLOCKDOWN,      	KEY_PAD_FUNC, NULL, share_key_update},
    { {4,213,59,260}	,' ',    SCANCODE_ESCAPE,        		KEY_PAD_FUNC, NULL, share_key_update},
    { {63,213,118,260}	,'0',    SCANCODE_SPACE,         		KEY_PAD_CHAR, NULL, share_key_update},
    { {122,213,177,260} ,' ',    SCANCODE_TOOP,          		KEY_PAD_FUNC, NULL, share_key_update},
    { {181,213,236,260} ,' ',    SCANCODE_ENTER,         		KEY_PAD_FUNC, NULL, share_key_update}

};

static void init_num_key_pads()
{
	int i,count;
	count = TABLESIZE(num_key_pads);
	for(i=0;i<count;i++)
	{
	   char key[20] = {0};
	   PRECT pRect = NULL;
	   sprintf(key, "RECT_EN_9_KEY_%d", i + 1);
	   if ((NULL != handleCfg) && (pRect = (PRECT) FindMigCfgRectValue(handleCfg, key)) != NULL)
	   {
		   CopyRect(&num_key_pads[i].bound, pRect);
	   }
	}

}

static md_key_t* get_num_key (key_window_t *kw, POINT point)
{
    int i;
    md_key_t* keys = kw->key;

    for(i = 0; i < kw->key_num; i++)
    {
        if (PtInRect( &keys[i].bound, point.x, point.y))
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
    printf("==== update key window ====\n");
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
		old_tecolor = SetTextColor (hdc, PIXEL_black);
		old_font = SelectFont(hdc, sw->stroke_font);
		DrawText (hdc, sw->str, -1, &(sw->bound), DT_LEFT);
		SetTextColor(hdc, old_tecolor);
		ReleaseDC(hdc);
	}
}

static int init_num_stroke_window(HWND hWnd, stroke_window_t *sw)
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

static void destroy_num_stroke_window (stroke_window_t *sw)
{
	if (sw->stroke_font)
	{
		DestroyLogFont (sw->stroke_font);
	}
	return;
}

/* return 0 on succes , less than 0 on error */
static int init_num_key_window(HWND hWnd, key_window_t *kw)
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

	kw->key = num_key_pads;
	kw->key_num = TABLESIZE(num_key_pads);

	kw->get_key = get_num_key;

	//if (NULL == (kw->data = (void *)calloc (1, sizeof(BITMAP)))){
	if (NULL == (kw->data = (void *) calloc(1, sizeof(kw_add_data_t))))
	{
		_MY_PRINTF("no memory for key window add data.\n");
		return -1;
	}

	if (get_keyboard_bitmap(HDC_SCREEN, &(((kw_add_data_t *) (kw->data))->char_key_press), CHAR_KEY_MASK))
	{
		_MY_PRINTF ("Fail to get key mask bitmap.\n");
		return -1;
	}

	if (get_keyboard_bitmap(HDC_SCREEN, &(((kw_add_data_t *) (kw->data))->func_key_press), FUNC_KEY_MASK))
	{
		_MY_PRINTF( "Fail to get key mask bitmap.\n");
		return -1;
	}

	//kw->update = kw_update;
	kw->update = NULL;

	/*use the shared bitmap for press key painting.*/
	key = kw->key;
	for (i = 0; i < kw->key_num; i++)
	{
		//  key->data = kw->data;
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

static void destroy_num_key_window (key_window_t *kw)
{
    if (kw->data)
    {
//        free(kw->data);
    }
}

int init_num_keypad(const void *handle, HWND hWnd, key_board_t *kb)
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
		init_num_key_pads();
	}

	/*initalize for key window.*/
	kb->key_window = (key_window_t *) calloc(1, sizeof(key_window_t));
	if (kb->key_window == NULL)
	{
		_MY_PRINTF("no memory for key window.\n");
		return -1;
	}
	if (0 != init_num_key_window(hWnd, kb->key_window))
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

	if (get_keyboard_bitmap(HDC_SCREEN, (PBITMAP) (&kb->data), NUM9_KBD_BKG))
	{
		_MY_PRINTF("Fail to get key mask bitmap.\n");
		return -1;
	}

	kb->update = keyboard_update;
	kb->ime = NULL;
	memset(&(kb->action), 0, sizeof(action_t));
	kb->proceed_msg = symbol_proc_msg;
	kb->clear = clear_keyboard;

	return 0;
}

void destroy_num_keypad (key_board_t *kb)
{
	if(NULL == kb)
	{
		return;
	}

	/*stroke window*/
	if(kb->stroke_window != NULL)
	{
		destroy_num_stroke_window(kb->stroke_window);
		free(kb->stroke_window);
	}
	/*ky window*/
	if(kb->key_window != NULL)
	{
		destroy_num_key_window(kb->key_window);
		free(kb->key_window);
	}

    /*add data*/
   // free(kb->data);

}


 









