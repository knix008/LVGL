
/*
 * wangjian<wangjian@minigui.org>
 * 2008-7-11
*/

#include "softkeyboard.h"

#ifdef KBD_TOOLTIP
#include "tooltip.h"
#endif

static void* handleCfg = NULL;

#define  KEY_MOVE_X	6
#define  KEY_MOVE_Y	10

#if SOFTKBD_800_1280
static md_key_t punct_key_pads[] = {
    /***** 1, 2, 3, 4, 5, 6, 7, 8, 9 ,0 *****/
    { {0,KEY_ROW1_TY-KEY_MOVE_Y,73+KEY_MOVE_X,KEY_ROW1_BY+KEY_MOVE_Y}	, '[', SCANCODE_1,         KEY_PAD_CHAR, NULL, share_key_update},
    { {86-KEY_MOVE_X,KEY_ROW1_TY-KEY_MOVE_Y,153+KEY_MOVE_X,KEY_ROW1_BY+KEY_MOVE_Y}	, ']', SCANCODE_2,         KEY_PAD_CHAR, NULL, share_key_update},
    { {166-KEY_MOVE_X,KEY_ROW1_TY-KEY_MOVE_Y,233+KEY_MOVE_X,KEY_ROW1_BY+KEY_MOVE_Y}	, '{', SCANCODE_3,         KEY_PAD_CHAR, NULL, share_key_update},
    { {246-KEY_MOVE_X,KEY_ROW1_TY-KEY_MOVE_Y,313+KEY_MOVE_X,KEY_ROW1_BY+KEY_MOVE_Y}	, '}', SCANCODE_4,         KEY_PAD_CHAR, NULL, share_key_update},
    { {326-KEY_MOVE_X,KEY_ROW1_TY-KEY_MOVE_Y,393+KEY_MOVE_X,KEY_ROW1_BY+KEY_MOVE_Y}	, '#', SCANCODE_5,         KEY_PAD_CHAR, NULL, share_key_update},
    { {406-KEY_MOVE_X,KEY_ROW1_TY-KEY_MOVE_Y,473+KEY_MOVE_X,KEY_ROW1_BY+KEY_MOVE_Y}	, '%', SCANCODE_6,         KEY_PAD_CHAR, NULL, share_key_update},
    { {486-KEY_MOVE_X,KEY_ROW1_TY-KEY_MOVE_Y,553+KEY_MOVE_X,KEY_ROW1_BY+KEY_MOVE_Y}	, '^', SCANCODE_7,         KEY_PAD_CHAR, NULL, share_key_update},
    { {566-KEY_MOVE_X,KEY_ROW1_TY-KEY_MOVE_Y,633+KEY_MOVE_X,KEY_ROW1_BY+KEY_MOVE_Y}	, '*', SCANCODE_8,         KEY_PAD_CHAR, NULL, share_key_update},
    { {646-KEY_MOVE_X,KEY_ROW1_TY-KEY_MOVE_Y,713+KEY_MOVE_X,KEY_ROW1_BY+KEY_MOVE_Y}	, '+', SCANCODE_9,         KEY_PAD_CHAR, NULL, share_key_update},
    { {726-KEY_MOVE_X,KEY_ROW1_TY-KEY_MOVE_Y,800,KEY_ROW1_BY+KEY_MOVE_Y}	, '=', SCANCODE_EQUAL,     KEY_PAD_CHAR, NULL, share_key_update},
    { {0,KEY_ROW2_TY-KEY_MOVE_Y,73+KEY_MOVE_X,KEY_ROW2_BY+KEY_MOVE_Y}	, '_', SCANCODE_MINUS,     KEY_PAD_CHAR, NULL, share_key_update},
    { {86-KEY_MOVE_X,KEY_ROW2_TY-KEY_MOVE_Y,153+KEY_MOVE_X,KEY_ROW2_BY+KEY_MOVE_Y}	, '\\', SCANCODE_BACKSLASH,KEY_PAD_CHAR, NULL, share_key_update},
    { {166-KEY_MOVE_X,KEY_ROW2_TY-KEY_MOVE_Y,233+KEY_MOVE_X,KEY_ROW2_BY+KEY_MOVE_Y}	, '|', SCANCODE_SEMICOLON, KEY_PAD_CHAR, NULL, share_key_update},
    { {246-KEY_MOVE_X,KEY_ROW2_TY-KEY_MOVE_Y,313+KEY_MOVE_X,KEY_ROW2_BY+KEY_MOVE_Y}	, '~', SCANCODE_SEMICOLON, KEY_PAD_CHAR, NULL, share_key_update},
    { {326-KEY_MOVE_X,KEY_ROW2_TY-KEY_MOVE_Y,393+KEY_MOVE_X,KEY_ROW2_BY+KEY_MOVE_Y}	, '<', SCANCODE_LESS,      KEY_PAD_CHAR, NULL, share_key_update},
    { {406-KEY_MOVE_X,KEY_ROW2_TY-KEY_MOVE_Y,473+KEY_MOVE_X,KEY_ROW2_BY+KEY_MOVE_Y}	, '>', SCANCODE_0,         KEY_PAD_CHAR, NULL, share_key_update},
    { {486-KEY_MOVE_X,KEY_ROW2_TY-KEY_MOVE_Y,553+KEY_MOVE_X,KEY_ROW2_BY+KEY_MOVE_Y}	, '$', SCANCODE_4,         KEY_PAD_CHAR, NULL, share_key_update},
    { {566-KEY_MOVE_X,KEY_ROW2_TY-KEY_MOVE_Y,633+KEY_MOVE_X,KEY_ROW2_BY+KEY_MOVE_Y}	, '&', SCANCODE_7,         KEY_PAD_CHAR, NULL, share_key_update},
    { {646-KEY_MOVE_X,KEY_ROW2_TY-KEY_MOVE_Y,713+KEY_MOVE_X,KEY_ROW2_BY+KEY_MOVE_Y}	, '@', SCANCODE_2,         KEY_PAD_CHAR, NULL, share_key_update},
    { {726-KEY_MOVE_X,KEY_ROW2_TY-KEY_MOVE_Y,800+KEY_MOVE_X,KEY_ROW2_BY+KEY_MOVE_Y}	, '"', SCANCODE_APOSTROPHE,KEY_PAD_CHAR, NULL, share_key_update},
    { {0,KEY_ROW3_TY-KEY_MOVE_Y,106+KEY_MOVE_X,KEY_ROW3_BY+KEY_MOVE_Y}	, ' ', SCANCODE_TOSYMBOL,      KEY_PAD_FUNC, NULL, share_key_update},
    { {118-KEY_MOVE_X,KEY_ROW3_TY-KEY_MOVE_Y,218+KEY_MOVE_X,KEY_ROW3_BY+KEY_MOVE_Y}	, '.', SCANCODE_PERIOD,    KEY_PAD_CHAR, NULL, share_key_update},
    { {230-KEY_MOVE_X,KEY_ROW3_TY-KEY_MOVE_Y,330+KEY_MOVE_X,KEY_ROW3_BY+KEY_MOVE_Y}	, ',', SCANCODE_COMMA,     KEY_PAD_CHAR, NULL, share_key_update},
    { {343-KEY_MOVE_X,KEY_ROW3_TY-KEY_MOVE_Y,443+KEY_MOVE_X,KEY_ROW3_BY+KEY_MOVE_Y}	, '?', SCANCODE_SLASH,     KEY_PAD_CHAR, NULL, share_key_update},
    { {456-KEY_MOVE_X,KEY_ROW3_TY-KEY_MOVE_Y,556+KEY_MOVE_X,KEY_ROW3_BY+KEY_MOVE_Y}	, '!', SCANCODE_1,         KEY_PAD_CHAR, NULL, share_key_update},
    { {569-KEY_MOVE_X,KEY_ROW3_TY-KEY_MOVE_Y,669+KEY_MOVE_X,KEY_ROW3_BY+KEY_MOVE_Y}	, '\'', SCANCODE_APOSTROPHE,KEY_PAD_CHAR, NULL, share_key_update},
    { {692-KEY_MOVE_X,KEY_ROW3_TY-KEY_MOVE_Y,800+KEY_MOVE_X,KEY_ROW3_BY+KEY_MOVE_Y}	, ' ', SCANCODE_BACKSPACE, KEY_PAD_FUNC, NULL, share_key_update},
    { {0,KEY_ROW4_TY-KEY_MOVE_Y,106+KEY_MOVE_X,KEY_ROW4_BY+KEY_MOVE_Y}	, ' ', SCANCODE_TOEN,      KEY_PAD_FUNC, NULL, share_key_update},
    { {119-KEY_MOVE_X,KEY_ROW4_TY-KEY_MOVE_Y,219+KEY_MOVE_X,KEY_ROW4_BY+KEY_MOVE_Y}	, ' ', SCANCODE_TOPY,      KEY_PAD_FUNC, NULL, share_key_update},
    { {230-KEY_MOVE_X,KEY_ROW4_TY-KEY_MOVE_Y,594+KEY_MOVE_X,KEY_ROW4_BY+KEY_MOVE_Y}	, ' ', SCANCODE_SPACE,     KEY_PAD_CHAR, NULL, share_key_update},
    { {607-KEY_MOVE_X,KEY_ROW4_TY-KEY_MOVE_Y,800+KEY_MOVE_X,KEY_ROW4_BY+KEY_MOVE_Y}	, ' ', SCANCODE_ENTER,     KEY_PAD_FUNC, NULL, share_key_update},
};
#else
static md_key_t punct_key_pads[] = {
    /***** 1, 2, 3, 4, 5, 6, 7, 8, 9 ,0 *****/ 
    { {2,63,46,96}		, '[', SCANCODE_1,         KEY_PAD_CHAR, NULL, share_key_update},
    { {50,63,94,96}		, ']', SCANCODE_2,         KEY_PAD_CHAR, NULL, share_key_update},
    { {97,63,142,96}	, '{', SCANCODE_3,         KEY_PAD_CHAR, NULL, share_key_update},
    { {146,63,190,96}	, '}', SCANCODE_4,         KEY_PAD_CHAR, NULL, share_key_update},
    { {194,63,238,96}	, '#', SCANCODE_5,         KEY_PAD_CHAR, NULL, share_key_update},
    { {242,63,286,96}	, '%', SCANCODE_6,         KEY_PAD_CHAR, NULL, share_key_update},
    { {289,63,334,96}	, '^', SCANCODE_7,         KEY_PAD_CHAR, NULL, share_key_update},
    { {338,63,382,96}	, '*', SCANCODE_8,         KEY_PAD_CHAR, NULL, share_key_update},
    { {386,63,430,96}	, '+', SCANCODE_9,         KEY_PAD_CHAR, NULL, share_key_update},
    { {434,63,478,96}	, '=', SCANCODE_EQUAL,     KEY_PAD_CHAR, NULL, share_key_update},
    { {2,100,46,133}	, '_', SCANCODE_MINUS,     KEY_PAD_CHAR, NULL, share_key_update},
    { {50,100,94,133}	, '\\', SCANCODE_BACKSLASH,KEY_PAD_CHAR, NULL, share_key_update},
    { {97,100,142,133}	, '|', SCANCODE_SEMICOLON, KEY_PAD_CHAR, NULL, share_key_update},
    { {146,100,190,133}	, '~', SCANCODE_SEMICOLON, KEY_PAD_CHAR, NULL, share_key_update},
    { {194,100,238,133}	, '<', SCANCODE_LESS,      KEY_PAD_CHAR, NULL, share_key_update},
    { {242,100,286,133}	, '>', SCANCODE_0,         KEY_PAD_CHAR, NULL, share_key_update},
    { {289,100,334,133}	, '$', SCANCODE_4,         KEY_PAD_CHAR, NULL, share_key_update},
    { {338,100,382,133}	, '&', SCANCODE_7,         KEY_PAD_CHAR, NULL, share_key_update},
    { {386,100,430,133}	, '@', SCANCODE_2,         KEY_PAD_CHAR, NULL, share_key_update},
    { {434,100,478,133}	, '"', SCANCODE_APOSTROPHE,KEY_PAD_CHAR, NULL, share_key_update},
    { {2,137,70,170}	, ' ', SCANCODE_TOSYMBOL,      KEY_PAD_FUNC, NULL, share_key_update},
    { {74,137,137,170}	, '.', SCANCODE_PERIOD,    KEY_PAD_CHAR, NULL, share_key_update},
    { {141,137,204,170}	, ',', SCANCODE_COMMA,     KEY_PAD_CHAR, NULL, share_key_update},
    { {207,137,271,170}	, '?', SCANCODE_SLASH,     KEY_PAD_CHAR, NULL, share_key_update},
    { {275,137,338,170}	, '!', SCANCODE_1,         KEY_PAD_CHAR, NULL, share_key_update},
    { {342,137,405,170}	, '\'', SCANCODE_APOSTROPHE,KEY_PAD_CHAR, NULL, share_key_update},
    { {410,137,478,170}	, ' ', SCANCODE_BACKSPACE, KEY_PAD_FUNC, NULL, share_key_update},
    { {2,175,70,213}	, ' ', SCANCODE_TOEN,      KEY_PAD_FUNC, NULL, share_key_update},
    { {74,175,166,213}	, ' ', SCANCODE_TOPY,      KEY_PAD_FUNC, NULL, share_key_update},
    { {170,175,358,213}	, ' ', SCANCODE_SPACE,     KEY_PAD_CHAR, NULL, share_key_update},
    { {362,175,478,213}	, ' ', SCANCODE_ENTER,     KEY_PAD_FUNC, NULL, share_key_update},
};
#endif

static void init_punct_key_pads()
{
	int i,count;
	count = TABLESIZE(punct_key_pads);
	for(i=0;i<count;i++)
	{
		PRECT pRect = NULL;
	    char key[20] = {0};
	    sprintf(key,"RECT_NUM_26_KEY_%d",i+1);
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

static int init_punct_stroke_window (HWND hWnd, stroke_window_t *sw)
{
	PRECT pRect = NULL;
	if((NULL != handleCfg) && (pRect = (PRECT) FindMigCfgRectValue(handleCfg, "RECT_SKB_26_SW")) != NULL)
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
static int init_punct_key_window (HWND hWnd, key_window_t *kw)
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

	//kw->update = kw_update;
	kw->update = NULL;

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

int init_punct_keyboard (const void* handle,HWND hWnd, key_board_t *kb)
{
	PRECT pRect = NULL;
	int valueCfg = -1;
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
		init_punct_key_pads();
	}

	kb->view_window == NULL;
	kb->stroke_window = NULL;

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

	if (get_keyboard_bitmap(HDC_SCREEN, (PBITMAP) (&kb->data), PUNCT26_KBD_BKG))
	{
		_MY_PRINTF ("Fail to get key mask bitmap.\n");
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

	kb->update = keyboard_update;

	kb->ime = NULL;

	memset(&(kb->action), 0, sizeof(action_t));

	kb->proceed_msg = symbol_proc_msg;
	kb->clear = clear_keyboard;

	return 0;
}

void destroy_punct_keyboard (key_board_t *kb)
{
	if(NULL == kb)
	{
		return;
	}

    /*stroke window*/
    destroy_punct_stroke_window (kb->stroke_window);
    free(kb->stroke_window);

    /*ky window*/
    destroy_punct_key_window (kb->key_window);
    free(kb->key_window);

    /*add data*/
    //free(kb->data);

}












