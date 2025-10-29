/*
** $Id: common.c 1053 2008-08-31 14:15:34Z houhuihua $
**
** common.c: This file include common functions for soft keyboard. 
**
** Copyright (C) 2009 ~ 2014 wilsn.
** Copyright (C) 2003 ~ 2008 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** Create date: 2008/07/13
**
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <ctype.h>
#include "common.h"
#include "printlog.h"

#ifdef KBD_TOOLTIP
#include "tooltip.h"
#endif

#define MAX_PAGE_AIZE 	250

static int index_curr = 0;
static int index_next = 0;
static int stack[MAX_PAGE_AIZE] = {0};
static int sp = 0;
static ime_callback ime_method = NULL;
static char stroke[SW_STR_LEN] = {0};
static int case_mode = PTI_CASE_abc; 

static vw_element_t* _e_down = NULL;
static vw_element_t* _s_down = NULL;
static md_key_t* _key_down = NULL;
static BOOL close_key ;
static BITMAP res_bmp[RES_BMP_SIZE];

int en_kb_Index = 0;

void reset_mouse_state(HWND hwnd)
{
	if (_key_down != NULL)
	{
		if ((_key_down->style & KEY_PAD_PRESSED) && _key_down->scan_code != SCANCODE_LEFTSHIFT)
		{
			if (_key_down->style & KEY_PAD_DRAWED)
			{
				_key_down->style &= ~KEY_PAD_DRAWED;
				SetAutoRepeatMessage(0, 0, 0, 0);
			}
			_key_down->style &= ~KEY_PAD_PRESSED;

			if (hwnd != HWND_INVALID)
			{
				_key_down->update(_key_down, hwnd);

			}
		}
		_key_down = NULL;
	}

}

void softkey_reset(void)
{
	index_curr = 0;
	index_next = 0;
	sp = 0;
	memset(stack, 0, MAX_PAGE_AIZE);
	ime_method = NULL;
	memset(stroke, 0, SW_STR_LEN);
	case_mode = PTI_CASE_abc;
	en_kb_Index = 0;

	_e_down = NULL;
	_s_down = NULL;
	reset_mouse_state(HWND_INVALID);
}

static int getMaxStrLen(view_window_t* view_window, int flag)
{
	int maxStrLen = view_window->max_str_len;
	if ((flag != EN) || (case_mode != PTI_CASE_ABC))
	{
		goto END;
	}

	if (LCD_SIZE_480X272 == getLCDSize())
	{
		maxStrLen = 28;
	}
	else if (LCD_SIZE_320X240== getLCDSize())
	{
		maxStrLen = 18;
	}
	else 
	{
		maxStrLen = 18;
	}
END:
	return maxStrLen;
}

static BOOL try_next(ime_callback func, const char* strokes, int len, int index, int mode)
{
	static char buffer[VW_BUFFER_LEN];
    if (func)
	{
		int r;
		r=func(strokes, buffer, len, index, mode);
		return (BOOL)(r != 0 && r != -1);
	}
	return FALSE;

}

static void append(char* str, char c, int len, int flag)
{
	if(str == NULL || len <= 0)
		return;

    if (strlen(str) >= len)
        return;
	
    while(*str)
	{
		str++;
	}
	
	*(str++) = c;
	*str = 0;
}

static void cut(char* str, int flag)
{
	char* orig = str;

	if (str == NULL)
	{
		return;
	}
	while (*str)
	{
		str++;
	}
	if (orig == str)
	{
		return;
	}
	*(str - 1) = 0;
}

void strcpylower(char* buf, const char* str)
{
	if (str == NULL || buf == NULL)
	{
		return;
	}

	while (*str)
	{
		if (*str >= 'A' && *str <= 'Z')
		{
			*(buf++) = *(str++) + 32;
		}
		else
		{
			*(buf++) = *(str++);
		}
	}

	*buf = 0;
}

BOOL hit_rect(RECT rect, POINT p)
//inline BOOL hit_rect(RECT rect, POINT p)
{
	if (p.x >= rect.left && p.x <= rect.right && p.y >= rect.top && p.y <= rect.bottom)
	{
		return TRUE;
	}

	return FALSE;
}

//键盘更新
void keyboard_update(key_board_t *kb, HWND hWnd, WPARAM wParam, RECT* rect)
{
    HDC hdc; 

    if (kb->data) 
	{
#if 0
        hdc = GetDC (hWnd);
		if(rect != NULL) {
            RECT rcTemp = *rect;
            ScreenToClient(hWnd, &rcTemp.left, &rcTemp.top);                 
            ScreenToClient(hWnd, &rcTemp.right, &rcTemp.bottom);
			SelectClipRect(hdc, &rcTemp);
		}
        FillBoxWithBitmap(hdc, 0, 0, SKB_WIN_W, SKB_WIN_H, (PBITMAP)(kb->data));
        ReleaseDC(hdc);
#else
        BOOL fGetDC = FALSE;
        hdc = (HDC)wParam;
        if (!hdc)
		{
            hdc = GetDC(hWnd);
            fGetDC = TRUE;
        }
		if (rect != NULL) 
		{
            RECT rcTemp = *rect;
            ScreenToClient(hWnd, &rcTemp.left, &rcTemp.top);                 
            ScreenToClient(hWnd, &rcTemp.right, &rcTemp.bottom);
			SelectClipRect(hdc, &rcTemp);
		}
        FillBoxWithBitmap(hdc, 0, 0, kb->skb_win_w, kb->skb_win_h, (PBITMAP)(kb->data));

		FillBoxWithBitmap (hdc, kb->close_rc.left, kb->close_rc.top,
			  		 RECTW(kb->close_rc), RECTH(kb->close_rc), kb->close_normal);
		if (fGetDC)
    	{
    		ReleaseDC(hdc);
		}
#endif
    }
}

void close_update (key_board_t *kb, HWND hWnd,BOOL pressed)
{
	HDC hdc = GetDC(hWnd);
	SetBkMode (hdc, BM_TRANSPARENT);
    EraseBbGround(hWnd, &kb->close_rc);

	if(pressed && !close_key) 
	{	
        if(FillBoxWithBitmap (hdc, kb->close_rc.left, kb->close_rc.top,
                RECTW(kb->close_rc), RECTH(kb->close_rc), kb->close_press))
        {
			close_key = TRUE;
		}
        	
	}
    else if(!pressed && close_key)
	{
        if(FillBoxWithBitmap (hdc, kb->close_rc.left, kb->close_rc.top,
                RECTW(kb->close_rc), RECTH(kb->close_rc), kb->close_normal))
        {
			close_key = FALSE;

		}

	}
    ReleaseDC(hdc);
}

//候选词显示区域处理
void vw_proceed_hit(HWND hwnd, view_window_t* view_window, stroke_window_t* stroke_window, action_t* action,
		BOOL pressed, POINT p, int flag, md_ime_t* ime)
{
	static char buf[VW_ELMT_LEN];

	action->operation = AC_NULL; 

	if(hit_rect(view_window->key_pg_down, p)) 
	{
		if(!(view_window->style & VW_SHOW_PD))
		{
			return;
		}
		if(pressed) 
		{
			view_window->style |= VW_DRAW_ELMTS;
			view_window->style |= VW_PD_PRESSED;
			view_window->update (view_window, hwnd, NULL);
			return;
		} 

		view_window->style &= ~VW_PD_PRESSED;
		view_window->style |= VW_SHOW_PU;

		//get next page
		if (sp >= 0 && sp < MAX_PAGE_AIZE)
		{
			stack[sp++] = index_curr;
		}
		index_curr = index_next;
		
        if (ime_method)
		{
			index_next = ime_method(stroke, view_window->buffer,
                    getMaxStrLen(view_window,flag), index_curr, case_mode);
        }
		view_window->set_elements(view_window, hwnd);
		view_window->style |= VW_DRAW_ELMTS;
		if (!try_next(ime_method, stroke, getMaxStrLen(view_window,flag), index_next, case_mode)) 
		{
			view_window->style &= ~VW_SHOW_PD;
		}
		else 
		{
			view_window->style |= VW_SHOW_PD;
		}

		view_window->update (view_window, hwnd, NULL);
		view_window->style &= ~VW_DRAW_ELMTS;
		return;
	} 

	if(hit_rect(view_window->key_pg_up, p)) 
	{
		if(!(view_window->style & VW_SHOW_PU))
		{
			return;
		}
		if(pressed) 
		{
			view_window->style |= VW_DRAW_ELMTS;
			view_window->style |= VW_PU_PRESSED;
			view_window->update (view_window, hwnd, NULL);
			return;
		} 

		view_window->style &= ~VW_PU_PRESSED;
		view_window->style |= VW_SHOW_PD;
		//get prev page
		sp--;
        if (sp >= 0) 
		{ 
            index_curr = stack[sp];
            if (ime_method)
        	{
        		index_next = ime_method(stroke, view_window->buffer,
                        getMaxStrLen(view_window,flag), index_curr, case_mode);
            }
			view_window->set_elements(view_window, hwnd);
            view_window->style |= VW_DRAW_ELMTS;
            if (sp == 0)
            {
            	view_window->style &= ~VW_SHOW_PU;
            }
        }
		else 
		{
            sp = 0;
            view_window->style &= ~VW_SHOW_PU;
        }
		view_window->update (view_window, hwnd, NULL);
		view_window->style &= ~VW_DRAW_ELMTS;
		return;
	} 
		
	{
		vw_element_t* e = view_window->get_element (view_window, p);
		if(e == NULL)
		{
			return;
		}
		view_window->style |= VW_DRAW_ELMTS;

		if(pressed) 
		{
			view_window->style |= VW_EL_PRESSED;
			view_window->update (view_window, hwnd, e);
			return;
		}
			
		sp = 0;
		index_curr = 0;
		index_next = 0;

		view_window->style &= ~VW_EL_PRESSED;
		view_window->update (view_window, hwnd, e);

		if(flag == CN) 
		{
			memset (buf, 0, VW_ELMT_LEN);
			strcpy(buf, e->string);
		
			action->operation = AC_SEND_CN_STRING; 
			action->str = buf; 

			memset(stroke_window->str, 0, SW_STR_LEN);
			memset(stroke, 0, SW_STR_LEN);
			stroke_window->update(stroke_window, hwnd);
			strncpy(stroke, e->string + e->len - 2, 2);
			ime_method = ime->predict_word;

			index_next = ime_method(stroke, view_window->buffer,
					getMaxStrLen(view_window,flag), index_curr, case_mode);

			if(view_window->set_elements) 
			{
				view_window->set_elements (view_window, hwnd);
			}
			view_window->style = 0;
			view_window->style |= VW_DRAW_ELMTS;

			if (!try_next(ime_method, stroke, getMaxStrLen(view_window,flag), index_next, case_mode)) 
			{
				view_window->style &= ~VW_SHOW_PD;
			}
			else 
			{
				view_window->style |= VW_SHOW_PD;
				sp ++;
			}

			view_window->update (view_window, hwnd, NULL);
			view_window->style &= ~VW_DRAW_ELMTS;

		}
		else if (flag == EN) 
		{
			memset (buf, 0, VW_ELMT_LEN);
			strcpy(buf, e->string);
			action->operation = AC_SEND_EN_STRING; 
			action->str = buf; 

			view_window->style &= ~VW_SHOW_PU;
			view_window->style &= ~VW_SHOW_PD;
			view_window->style |= VW_DRAW_ELMTS;
			view_window->clear_elements(view_window);
			view_window->update (view_window, hwnd, e);

			memset(stroke_window->str, 0, SW_STR_LEN);
			memset(stroke, 0, SW_STR_LEN);
			stroke_window->update(stroke_window, hwnd);
		}
	}
}

void proceed_move(HWND hwnd, key_board_t* key_board, BOOL pressed, POINT p, int flag)
{
	static md_key_t* key = NULL;
	static vw_element_t* e = NULL;
	static vw_element_t* s = NULL;

    key_board->action.operation = AC_NULL; 

    if (!pressed)
	{
		goto RETURN;
	}
	
	if((key_board->view_window->style & VW_SHOW_PD) && !PtInRect(&key_board->view_window->key_pg_down, p.x, p.y) )
	{
		key_board->view_window->style |= VW_DRAW_ELMTS;
		key_board->view_window->style &= ~VW_PD_PRESSED;
		key_board->view_window->update (key_board->view_window, hwnd, NULL);
	}
	
	if((key_board->view_window->style & VW_SHOW_PU) && !PtInRect(&key_board->view_window->key_pg_up, p.x, p.y))
	{
		key_board->view_window->style |= VW_DRAW_ELMTS;
		key_board->view_window->style &= ~VW_PU_PRESSED;
		key_board->view_window->update (key_board->view_window, hwnd, NULL);
	}

	if (!close_key && hit_rect(key_board->close_rc, p)) 
	{
        close_update(key_board,hwnd,TRUE);
    }
	else if (close_key && !hit_rect(key_board->close_rc, p)) 
	{
		close_update(key_board,hwnd,FALSE);
	}

	if (PtInRect(&key_board->key_window->bound, p.x, p.y)) 
	{
		key = key_board->key_window->get_key(key_board->key_window, p);
        if(key && key->style & KEY_PAD_FUNC)
        {
        	return ;
        }
		if (_key_down != key) 
		{
			if (_key_down != NULL && _key_down->scan_code != SCANCODE_LEFTSHIFT) 
			{
               if(_key_down->style & KEY_PAD_DRAWED) 
			   {
                   _key_down->style &= ~KEY_PAD_DRAWED;
                   SetAutoRepeatMessage (0, 0, 0, 0);
               }
				_key_down->style &= ~KEY_PAD_PRESSED;
				_key_down->update(_key_down, hwnd);
			}

			if (key != NULL) 
			{
				key->style |= KEY_PAD_PRESSED;
				key->update(key, hwnd);
			}
            _key_down = key;
		}
		return;
	}

	if (PtInRect(&key_board->view_window->bound, p.x, p.y)) 
	{
		RECT rect;
		rect.left = key_board->view_window->key_pg_up.right;
		rect.top = key_board->view_window->bound.top;
		rect.right = key_board->view_window->key_pg_down.left;
		rect.bottom = key_board->view_window->bound.bottom;
		
		if (PtInRect(&rect, p.x, p.y)) 
		{
			e = key_board->view_window->get_element(key_board->view_window, p);
			if (_e_down != e) 
			{
				if (_e_down != NULL) 
				{
					key_board->view_window->style |= VW_DRAW_ELMTS;
					key_board->view_window->style &= ~VW_PD_PRESSED;
					key_board->view_window->update(key_board->view_window, hwnd, NULL);
				}

				if (e != NULL) 
				{
					vw_proceed_hit(hwnd, key_board->view_window, key_board->stroke_window,
                            &key_board->action, TRUE, p, flag, key_board->ime);
				}
				_e_down = e;
			}
		}
		else if(PtInRect(&key_board->view_window->key_pg_down, p.x, p.y) || PtInRect(&key_board->view_window->key_pg_up, p.x, p.y))
		{
			if(!(key_board->view_window->style & VW_SHOW_PD) && PtInRect(&key_board->view_window->key_pg_down, p.x, p.y) )
			{
				key_board->view_window->style |= VW_DRAW_ELMTS;
				key_board->view_window->style |= VW_PD_PRESSED;
				key_board->view_window->update (key_board->view_window, hwnd, NULL);
			}
			else if(!(key_board->view_window->style & VW_SHOW_PU) && PtInRect(&key_board->view_window->key_pg_up, p.x, p.y))
			{
				key_board->view_window->style |= VW_DRAW_ELMTS;
				key_board->view_window->style |= VW_PU_PRESSED;
				key_board->view_window->update (key_board->view_window, hwnd, NULL);
			}
			vw_proceed_hit(hwnd, key_board->view_window, key_board->stroke_window,
                            &key_board->action, TRUE, p, flag, key_board->ime);
		}

		return;
	}
	else if (key_board->select_window != NULL && PtInRect(&key_board->select_window->bound, p.x, p.y)) 
	{

		s = key_board->select_window->get_element(key_board->select_window, p);
		if (_s_down != s) 
		{
			if ((_s_down != NULL) && (EN == flag) )
			{
				key_board->select_window->style |= VW_DRAW_ELMTS;
				key_board->select_window->style &= ~VW_PD_PRESSED;
				key_board->select_window->update(key_board->select_window, hwnd, NULL);
			}

			if (s != NULL) 
			{
				slw_proceed_hit(hwnd, key_board->view_window,key_board->select_window,
					key_board->stroke_window, &key_board->action, TRUE, p, flag, key_board->ime);
			}
			_s_down = s;
		}
	
		return;
	}

		
RETURN:

	if (!hit_rect(key_board->close_rc, p) && close_key) 
	{
		close_update(key_board,hwnd,FALSE);
	}

	if (_e_down != NULL) 
	{
		key_board->view_window->style |= VW_DRAW_ELMTS;
		key_board->view_window->style &= ~VW_PD_PRESSED;
		key_board->view_window->update(key_board->view_window, hwnd, NULL);
		_e_down = NULL;
	}

	if (_s_down != NULL && key_board->select_window != NULL) 
	{
		key_board->select_window->style |= VW_DRAW_ELMTS;
		key_board->select_window->style &= ~VW_PD_PRESSED;
		key_board->select_window->update(key_board->select_window, hwnd, NULL);
		_s_down = NULL;
	}

	if (_key_down != NULL) 
	{
		_key_down->style &= ~KEY_PAD_PRESSED;
		_key_down->update(_key_down, hwnd);
		_key_down = NULL;
	}
}

void kw_proceed_hit(HWND hwnd, view_window_t* view_window, stroke_window_t* stroke_window, md_key_t* key,
		action_t* action, BOOL pressed, POINT p, int flag, md_ime_t* ime, WPARAM wParam, LPARAM lParam)
{
	if (pressed)
	{
		_key_down = key;
	}
	else
	{
		_key_down = NULL;
	}

	if (key->scan_code == SCANCODE_LEFTSHIFT)
	{
		action->operation = AC_NULL;
		if (!pressed)
		{
			return;
		}
		if (!(key->style & KEY_PAD_PRESSED))
		{
			case_mode = PTI_CASE_ABC;
			key->style |= KEY_PAD_PRESSED;
			key->update(key, hwnd);
		}
		else
		{
			case_mode = PTI_CASE_abc;
			key->style &= ~KEY_PAD_PRESSED;
			key->update(key, hwnd);

		}
		memset(stroke_window->str, 0, sizeof(stroke_window->str));//切换大小写英文输入法后清空stroke框的内容
		return;
	}

	//软键盘删除键处理
	if (key->scan_code == SCANCODE_BACKSPACE)
	{
		if (pressed)
		{
			action->operation = AC_NULL;
			key->style |= KEY_PAD_PRESSED;

			if (!(key->style & KEY_PAD_DRAWED))
			{
				key->update(key, hwnd);
				key->style |= KEY_PAD_DRAWED;
				SetAutoRepeatMessage(hwnd, MSG_LBUTTONDOWN, wParam, lParam);
			}
			return;
		} //end of pressed
		else
		{
			action->operation = AC_NULL;
			key->style &= ~KEY_PAD_PRESSED;

			if (key->style & KEY_PAD_DRAWED)
			{
				key->style &= ~KEY_PAD_DRAWED;
			}

			key->update(key, hwnd);

			SetAutoRepeatMessage(0, 0, 0, 0);

			if (strlen(stroke_window->str) > 1)
			{
				action->operation = AC_NULL;
				//TODO
				// 1. delete a char from stroke_window->str
				cut(stroke_window->str, flag);

				// 2. update stroke_window
				stroke_window->update(stroke_window, hwnd);

				// 3. look up words;
				strcpylower(stroke, stroke_window->str);

				if (flag == EN)
				{
					strcat(stroke, "*");
				}
				sp = 0;
				index_curr = 0;

				if (ime_method)
				{
					index_next = ime_method(stroke, view_window->buffer, getMaxStrLen(view_window, flag), index_curr, case_mode);
				}
				view_window->style = 0;
				if (view_window->set_elements)
				{
					view_window->set_elements(view_window, hwnd);
				}
				view_window->style |= VW_DRAW_ELMTS;

				if (!try_next(ime_method, stroke, getMaxStrLen(view_window, flag), index_next, case_mode))
				{
					view_window->style &= ~VW_SHOW_PD;
				} 
				else
				{
					view_window->style |= VW_SHOW_PD;
					sp++;
				}

				// 4. update view_window
				view_window->update(view_window, hwnd, NULL);
				view_window->style &= ~VW_DRAW_ELMTS;
				// 5. return
				return;
			}
			else if (strlen(stroke_window->str) == 1)
			{
				// 1. delete a char from stroke_window->str
				cut(stroke_window->str, flag);

				// 2. update stroke_window
				stroke_window->update(stroke_window, hwnd);

				view_window->buffer[0] = '\0';
				view_window->style = 0;
				if (view_window->set_elements)
				{
					view_window->set_elements(view_window, hwnd);
				}

				// 4. update view_window
				view_window->update(view_window, hwnd, NULL);
				view_window->style &= ~VW_DRAW_ELMTS;
			}
			else
			{
				//clear view window
				if (view_window->buffer[0])
				{
					view_window->buffer[0] = '\0';
					view_window->style = 0;
					if (view_window->set_elements)
					{
						view_window->set_elements(view_window, hwnd);
					}
					view_window->update(view_window, hwnd, NULL);
				}
				else
				{
					//send SCANCODE_BACKSPACE to application window
					action->operation = AC_SEND_MSG;
					action->message = MSG_KEYDOWN;
					action->wParam = SCANCODE_BACKSPACE;
					action->lParam = 0;
				}
			}
		}
		return;
	}

	if (pressed)
	{
		action->operation = AC_NULL;
		key->style |= KEY_PAD_PRESSED;
		key->update(key, hwnd);
		return;
	}

	if (key->scan_code == SCANCODE_SPACE)
	{
		static char buf[SW_STR_LEN] = { 0 };

		key->style &= ~KEY_PAD_PRESSED;
		key->update(key, hwnd);

		if (strlen(stroke_window->str) > 0)
		{
			if (view_window->element_num != 0)
			{
				strcpy(buf, view_window->elements[0].string);
			}
			else
			{
				strcpy(buf, stroke_window->str);
			}
			view_window->style = VW_DRAW_ELMTS;
			view_window->clear_elements(view_window);
			view_window->update(view_window, hwnd, NULL);

			memset(stroke_window->str, 0, SW_STR_LEN);
			memset(stroke, 0, SW_STR_LEN);
			stroke_window->update(stroke_window, hwnd);

			if (flag == EN)
			{
				action->operation = AC_SEND_EN_STRING;
			}
			else if (flag == CN)
			{
				action->operation = AC_SEND_CN_STRING;
			}
			action->str = buf;
			return;
		}
		else
		{
			//else send SCANCODE_SPACE to application window
			action->operation = AC_SEND_MSG;
			action->message = MSG_KEYDOWN;
			action->wParam = SCANCODE_SPACE;
			action->lParam = 0;
		}
		return;
	}

	if (key->scan_code == SCANCODE_ENTER)
	{
		static char buf[SW_STR_LEN];
		// send stroke_window->str to application
		key->style &= ~KEY_PAD_PRESSED;
		key->update(key, hwnd);
		//		if(flag == EN && strlen(stroke_window->str) > 0) {
		if (stroke_window != NULL && strlen(stroke_window->str) > 0)
		{
			view_window->style = VW_DRAW_ELMTS;
			view_window->clear_elements(view_window);
			view_window->update(view_window, hwnd, NULL);

			strcpy(buf, stroke_window->str);
			memset(stroke_window->str, 0, SW_STR_LEN);
			memset(stroke, 0, SW_STR_LEN);
			stroke_window->update(stroke_window, hwnd);

			action->operation = AC_SEND_EN_STRING;
			action->str = buf;
		}
		else
		{
			action->operation = AC_SEND_MSG;
			action->message = MSG_KEYDOWN;
			action->wParam = SCANCODE_ENTER;
			action->lParam = 0;
		}
		return;
	}

	/*Switch ime status*/
	if (key->scan_code >= SCANCODE_TOEN && key->scan_code <= SCANCODE_TOOP)
	{
		key->style &= ~KEY_PAD_PRESSED;
		key->update(key, hwnd);
		action->operation = AC_CHANGE_KBD;
		return;
	}

	if (key->style & KEY_PAD_CHAR)
	{
		char ch;
		action->operation = AC_NULL;

		if (ime_method == ime->predict_word || ime_method == NULL)
		{
			memset(stroke_window->str, 0, SW_STR_LEN);
			ime_method = ime->translate_word;
		}

		key->style &= ~KEY_PAD_PRESSED;
		key->update(key, hwnd);
		if (case_mode == PTI_CASE_ABC)
		{
			ch = toupper(key->key_char);
		} 
		else
		{
			ch = key->key_char;
		}
		append(stroke_window->str, ch, SW_STR_LEN - 1, flag);
		stroke_window->update(stroke_window, hwnd);
		strcpylower(stroke, stroke_window->str);
		if (flag == EN)
		{
			strcat(stroke, "*");
		}
		// look up dict and update view window

		sp = 0;
		index_curr = 0;
		view_window->buffer[0] = '\0';

		if (ime_method)
		{
			index_next = ime_method(stroke, view_window->buffer, getMaxStrLen(view_window, flag), index_curr, case_mode);
		}
		view_window->style = 0;
		view_window->style |= VW_DRAW_ELMTS;

		if (strlen(view_window->buffer) >= 0)
		{
			view_window->set_elements(view_window, hwnd);
			if (index_next != -1)
			{
				if (!try_next(ime_method, stroke, getMaxStrLen(view_window, flag), index_next, case_mode))
				{
					view_window->style &= ~VW_SHOW_PD;
				}
				else
				{
					view_window->style |= VW_SHOW_PD;
				}
			}
		}
		else
		{
			view_window->clear_elements(view_window);
		}
		view_window->update(view_window, hwnd, NULL);
		view_window->style &= ~VW_DRAW_ELMTS;

		return;
	}
}

void slw_proceed_hit(HWND hwnd, view_window_t* view_window, view_window_t* select_window,
		stroke_window_t* stroke_window, action_t* action, BOOL pressed, POINT p, int flag, md_ime_t* ime)
{
	char ch;
	vw_element_t* e = select_window->get_element(select_window, p);
	if (e == NULL)
	{
		return;
	}

	select_window->style |= VW_DRAW_ELMTS;
	
	if(EN == flag)
	{
		if (pressed)
		{
			select_window->style |= VW_EL_PRESSED;
			select_window->update(select_window, hwnd, e);
			return;
		}
		ch = e->string[0];
		select_window->style &= ~VW_EL_PRESSED;
		select_window->clear_elements(select_window);
		select_window->update(select_window, hwnd, e);

		action->operation = AC_NULL;

		if (ime_method == ime->predict_word || ime_method == NULL)
		{
			memset(stroke_window->str, 0, SW_STR_LEN);
			ime_method = ime->translate_word;
		}

		if (case_mode == PTI_CASE_ABC)
		{
			ch = toupper(ch);
		}

		append(stroke_window->str, ch, SW_STR_LEN - 1, flag);
		stroke_window->update(stroke_window, hwnd);

		strcpylower(stroke, stroke_window->str);
		if (flag == EN)
		{
			strcat(stroke, "*");
		}
		
	}
	else if(CN == flag)
	{
		if (pressed)
		{
			select_window->style |= VW_EL_PRESSED;
			select_window->update(select_window, hwnd, e);
		}
		action->operation = AC_NULL;

		if (ime_method == ime->predict_word || ime_method == NULL)
		{
			memset(stroke_window->str, 0, SW_STR_LEN);
			ime_method = ime->translate_word;
		}

		strcpy(stroke_window->str,e->string);
		stroke_window->update(stroke_window, hwnd);
		strcpylower(stroke, stroke_window->str);

	}
	// look up dict and update view window
	sp = 0;
	index_curr = 0;
	index_next = ime_method(stroke, view_window->buffer, getMaxStrLen(view_window, flag), index_curr, case_mode);
	view_window->style = 0;
	view_window->style |= VW_DRAW_ELMTS;
	if (index_next != -1)
	{
		view_window->set_elements(view_window, hwnd);
	
		if (!try_next(ime_method, stroke, getMaxStrLen(view_window, flag), index_next, case_mode))
		{
			view_window->style &= ~VW_SHOW_PD;
		}
		else
		{
			view_window->style |= VW_SHOW_PD;
		}
	}
	else
	{
		view_window->clear_elements(view_window);
	}
	
	view_window->update(view_window, hwnd, NULL);
	view_window->style &= ~VW_DRAW_ELMTS;
	
	return;
}

void kp_proceed_hit(HWND hwnd, view_window_t* view_window, view_window_t* select_window, stroke_window_t* stroke_window, md_key_t* key, action_t* action, BOOL pressed, POINT p, int flag,
		md_ime_t* ime, WPARAM wParam, LPARAM lParam)
{

	if (pressed)
	{
		_key_down = key;
	}
	else
	{
		_key_down = NULL;
	}
	if(EN == flag)
	{
		select_window->clear_elements(select_window);
		select_window->style &= ~VW_DRAW_ELMTS;		
		select_window->update(select_window, hwnd, NULL);
	}
	
	if (key->scan_code == SCANCODE_ESCAPE)
	{
		action->operation = AC_NULL;
		if (pressed)
		{
			key->style |= KEY_PAD_PRESSED;
			key->update(key, hwnd);
			return;
		}
		key->style &= ~KEY_PAD_PRESSED;
		key->update(key, hwnd);
		SendMessage(hwnd, IME_CLOSE, 0, 0);
		return;
	}
	if (key->scan_code == SCANCODE_CURSORBLOCKDOWN)
	{
		action->operation = AC_NULL;
		if (pressed)
		{
			key->style |= KEY_PAD_PRESSED;
			key->update(key, hwnd);

			view_window->style |= VW_DRAW_ELMTS;
			view_window->style |= VW_PD_PRESSED;
			view_window->update(view_window, hwnd, NULL);
			return;
		}
		else if (!(view_window->style & VW_SHOW_PD) && (strlen(stroke_window->str) != 0))
		{
			return;
		}
		key->style &= ~KEY_PAD_PRESSED;
		key->update(key, hwnd);

		if (strlen(stroke_window->str) == 0)
		{
			action->operation = AC_SEND_MSG;
			action->message = MSG_KEYDOWN;
			action->wParam = SCANCODE_CURSORBLOCKDOWN;
			action->lParam = 0;
			return;
		}

		view_window->style &= ~VW_PD_PRESSED;
		view_window->style |= VW_SHOW_PU;

		//get next page
		if (sp >= 0 && sp < MAX_PAGE_AIZE)
		{
			stack[sp++] = index_curr;
		}
		index_curr = index_next;

		if (ime_method)
		{
			index_next = ime_method(stroke, view_window->buffer, getMaxStrLen(view_window, flag), index_curr, case_mode);
		}
		view_window->set_elements(view_window, hwnd);
		view_window->style |= VW_DRAW_ELMTS;
		if (!try_next(ime_method, stroke, getMaxStrLen(view_window, flag), index_next, case_mode))
		{
			view_window->style &= ~VW_SHOW_PD;
		}
		else
		{
			view_window->style |= VW_SHOW_PD;
		}

		view_window->update(view_window, hwnd, NULL);
		view_window->style &= ~VW_DRAW_ELMTS;
		return;

	}

	if (key->scan_code == SCANCODE_CURSORBLOCKUP)
	{
		action->operation = AC_NULL;
		if (pressed)
		{
			key->style |= KEY_PAD_PRESSED;
			key->update(key, hwnd);
			view_window->style |= VW_DRAW_ELMTS;
			view_window->style |= VW_PU_PRESSED;
			view_window->update(view_window, hwnd, NULL);
			return;
		}

		key->style &= ~KEY_PAD_PRESSED;
		key->update(key, hwnd);

		if (strlen(stroke_window->str) == 0)
		{
			action->operation = AC_SEND_MSG;
			action->message = MSG_KEYDOWN;
			action->wParam = SCANCODE_CURSORBLOCKUP;
			action->lParam = 0;
			return;
		}

		view_window->style &= ~VW_PU_PRESSED;
		view_window->style |= VW_SHOW_PD;
		//get prev page
		sp--;
		if (sp >= 0)
		{
			index_curr = stack[sp];
			if (ime_method)
			{
				index_next = ime_method(stroke, view_window->buffer, getMaxStrLen(view_window, flag), index_curr, case_mode);
			}
			view_window->set_elements(view_window, hwnd);
			view_window->style |= VW_DRAW_ELMTS;
			if (sp == 0)
			{
				view_window->style &= ~VW_SHOW_PU;
			}
		}
		else
		{
			sp = 0;
			view_window->style &= ~VW_SHOW_PU;
		}

		view_window->update(view_window, hwnd, NULL);
		view_window->style &= ~VW_DRAW_ELMTS;
		return;

	}

	if (key->scan_code == SCANCODE_LEFTSHIFT)
	{
		action->operation = AC_NULL;
		if (!pressed)
		{
			return;
		}

		if (!(key->style & KEY_PAD_PRESSED))
		{
			case_mode = PTI_CASE_ABC;
			key->style |= KEY_PAD_PRESSED;
			key->update(key, hwnd);
		}
		else
		{
			case_mode = PTI_CASE_abc;
			key->style &= ~KEY_PAD_PRESSED;
			key->update(key, hwnd);

		}
		return;
	}

	if (key->scan_code == SCANCODE_BACKSPACE)
	{
		if (pressed)
		{
			action->operation = AC_NULL;
			key->style |= KEY_PAD_PRESSED;

			if (!(key->style & KEY_PAD_DRAWED))
			{
				key->update(key, hwnd);
				key->style |= KEY_PAD_DRAWED;
				SetAutoRepeatMessage(hwnd, MSG_LBUTTONDOWN, wParam, lParam);
			}

		} //end of pressed
		else
		{
			action->operation = AC_NULL;
			key->style &= ~KEY_PAD_PRESSED;
			if (key->style & KEY_PAD_DRAWED)
			{
				key->style &= ~KEY_PAD_DRAWED;
			}
			key->update(key, hwnd);
			SetAutoRepeatMessage(0, 0, 0, 0);

			if (strlen(stroke_window->str) > 0)
			{
				action->operation = AC_NULL;
				if(CN == flag)
				{
					St_T9key * T9key = (St_T9key*)select_window->data;
					cut(T9key->szpy, flag);
					T9key->key = '0';
					select_window->set_elements(select_window, hwnd);
					select_window->style &= ~VW_DRAW_ELMTS;
					select_window->update(select_window, hwnd, NULL);
					if(strlen(select_window->buffer) > 0)
					{
						return;
					}
					else
					{
						printf("---------\n");
						stroke_window->str[0] = '\0';
					}
					
				}
				//TODO
				// 1. delete a char from stroke_window->str
				cut(stroke_window->str, flag);
				// 2. update stroke_window
				stroke_window->update(stroke_window, hwnd);
				// 3. look up words;
				strcpylower(stroke, stroke_window->str);
				if (flag == EN)
				{
					strcat(stroke, "*");
				}
				sp = 0;
				index_curr = 0;
				if (ime_method)
				{
					index_next = ime_method(stroke, view_window->buffer, getMaxStrLen(view_window, flag), index_curr, case_mode);
				}
				view_window->style = 0;
				if (view_window->set_elements)
				{
					view_window->set_elements(view_window, hwnd);
				}
				view_window->style |= VW_DRAW_ELMTS;

				if (!try_next(ime_method, stroke, getMaxStrLen(view_window, flag), index_next, case_mode))
				{
					view_window->style &= ~VW_SHOW_PD;
				} 
				else
				{
					view_window->style |= VW_SHOW_PD;
					sp++;
				}

				// 4. update view_window
				view_window->update(view_window, hwnd, NULL);
				view_window->style &= ~VW_DRAW_ELMTS;
				// 5. return
				return;
			}
			else
			{
				if (select_window->buffer[0])
				{
					select_window->buffer[0] = '\0';
					select_window->style = 0;
					if (select_window->clear_elements)
					{
						select_window->clear_elements(select_window);
					}
					select_window->update(select_window, hwnd, NULL);
				}
				
				//clear view window
				if (view_window->buffer[0])
				{
					view_window->buffer[0] = '\0';
					view_window->style = 0;
					if (view_window->set_elements)
					{
						view_window->set_elements(view_window, hwnd);
					}
					view_window->update(view_window, hwnd, NULL);
				}
				else
				{
					//send SCANCODE_BACKSPACE to application window
					action->operation = AC_SEND_MSG;
					action->message = MSG_KEYDOWN;
					action->wParam = SCANCODE_BACKSPACE;
					action->lParam = 0;

				}
			}
		}

		return;
	}

	if (pressed)
	{
		action->operation = AC_NULL;
		key->style |= KEY_PAD_PRESSED;
		key->update(key, hwnd);
		return;
	}

	if (EN == flag && key->scan_code == SCANCODE_SPACE)
	{
		static char buf[SW_STR_LEN] = { 0 };

		key->style &= ~KEY_PAD_PRESSED;
		key->update(key, hwnd);

		if (strlen(stroke_window->str) > 0)
		{
			if (view_window->element_num != 0)
			{
				strcpy(buf, view_window->elements[0].string);
			}
			else
			{
				strcpy(buf, stroke_window->str);
			}

			view_window->style = VW_DRAW_ELMTS;
			view_window->clear_elements(view_window);
			view_window->update(view_window, hwnd, NULL);

			memset(stroke_window->str, 0, SW_STR_LEN);
			memset(stroke, 0, SW_STR_LEN);
			stroke_window->update(stroke_window, hwnd);

			if (flag == EN)
			{
				action->operation = AC_SEND_EN_STRING;
			}
			else if (flag == CN)
			{
				action->operation = AC_SEND_CN_STRING;
			}
			action->str = buf;
			return;
		} 
		else
		{
			//else send SCANCODE_SPACE to application window
			action->operation = AC_SEND_MSG;
			action->message = MSG_KEYDOWN;
			action->wParam = SCANCODE_SPACE;
			action->lParam = 0;
		}
		return;
	}

	if (key->scan_code == SCANCODE_ENTER)
	{
		static char buf[SW_STR_LEN];
		// send stroke_window->str to application
		key->style &= ~KEY_PAD_PRESSED;
		key->update(key, hwnd);
		//		if(flag == EN && strlen(stroke_window->str) > 0) {
		if (EN == flag &&  strlen(stroke_window->str) > 0)
		{
			view_window->style = VW_DRAW_ELMTS;
			view_window->clear_elements(view_window);
			view_window->update(view_window, hwnd, NULL);

			strcpy(buf, stroke_window->str);
			memset(stroke_window->str, 0, SW_STR_LEN);
			memset(stroke, 0, SW_STR_LEN);
			stroke_window->update(stroke_window, hwnd);

			action->operation = AC_SEND_EN_STRING;
			action->str = buf;

		}
		else
		{
			action->operation = AC_SEND_MSG;
			action->message = MSG_KEYDOWN;
			action->wParam = SCANCODE_ENTER;
			action->lParam = 0;
		}

		return;
	}

	/*Switch ime status*/
	if (key->scan_code >= SCANCODE_TOEN && key->scan_code <= SCANCODE_TOOP)
	{
		key->style &= ~KEY_PAD_PRESSED;
		key->update(key, hwnd);
		action->operation = AC_CHANGE_KBD;
		return;
	}

	if (key->style & KEY_PAD_CHAR)
	{
		St_T9key * T9key = (St_T9key*)select_window->data;
		action->operation = AC_NULL;
		key->style &= ~KEY_PAD_PRESSED;
		key->update(key, hwnd);
		select_window->style = 0;
		select_window->style |= VW_DRAW_ELMTS;
		T9key->key= key->key_char;
		T9key->case_mode = case_mode;
		T9key->element_index = 0;
		select_window->set_elements(select_window, hwnd);
		//slw_set_spacing(select_window, hwnd);
		select_window->update(select_window, hwnd, NULL);

	}

}

static void symbol_kw_proceed_hit(HWND hwnd, view_window_t* view_window, stroke_window_t* stroke_window, md_key_t* key,
		action_t* action, BOOL pressed, POINT p, int flag, md_ime_t* ime,WPARAM wParam, LPARAM lParam)
{

	if (pressed)
	{
		_key_down = key;
	}
	else
	{
		_key_down = NULL;
	}

	if (pressed)
	{
		action->operation = AC_NULL;
		key->style |= KEY_PAD_PRESSED;
		key->update(key, hwnd);

	}
	else
	{
		key->style &= ~KEY_PAD_PRESSED;
		key->update(key, hwnd);

		if (key->scan_code >= SCANCODE_TOEN && key->scan_code <= SCANCODE_TOOP)
		{
			action->operation = AC_CHANGE_KBD;
			return;
		}

		if (key->style & KEY_PAD_CHAR)
		{
			action->operation = AC_SEND_MSG;
			action->message = MSG_CHAR;
			action->wParam = key->key_char;
			action->lParam = 0;
		}
		else if (key->style & KEY_PAD_FUNC)
		{
			switch (key->scan_code)
			{
				case SCANCODE_TONUM: /* key "123" switch kbd*/
				{
					action->operation = AC_CHANGE_KBD;
					break;
				}
				case SCANCODE_TOOP:
				{
					action->operation = AC_CHANGE_KBD;
					break;
				}
				case SCANCODE_TOEN:
				{
					action->operation = AC_CHANGE_KBD;
					break;
				}
				case SCANCODE_TOPY:
				{
					action->operation = AC_CHANGE_KBD;
					break;
				}
				case SCANCODE_ENTER:
				{
					static char buf[SW_STR_LEN];
					key->style &= ~KEY_PAD_PRESSED;
					key->update(key, hwnd);
					if (stroke_window != NULL && strlen(stroke_window->str) > 0)
					{
						strcpy(buf, stroke_window->str);
						memset(stroke_window->str, 0, SW_STR_LEN);
						memset(stroke, 0, SW_STR_LEN);
						stroke_window->update(stroke_window, hwnd);
						action->operation = AC_SEND_EN_STRING;
						action->str = buf;
					}
					else
					{
						action->operation = AC_SEND_MSG;
						action->message = MSG_KEYDOWN;
						action->wParam = SCANCODE_ENTER;
						action->lParam = 0;
					}
					break;
				}
				case SCANCODE_BACKSPACE:
				{
					action->operation = AC_SEND_MSG;
					action->message = MSG_KEYDOWN;
					action->wParam = SCANCODE_BACKSPACE;
					action->lParam = 0;
					if (key->style & KEY_PAD_DRAWED)
						key->style &= ~KEY_PAD_DRAWED;
					key->update(key, hwnd);
					SetAutoRepeatMessage(0, 0, 0, 0);
					break;
				}
				case SCANCODE_ESCAPE:
				{
					SendMessage(hwnd, IME_CLOSE, 0, 0);
					action->operation = AC_NULL;
					break;
				}
				case SCANCODE_SPACE:
				{
					action->operation = AC_SEND_MSG;
					action->message = MSG_KEYDOWN;
					action->wParam = SCANCODE_SPACE;
					action->lParam = 0;
					break;
				}
				case SCANCODE_CURSORBLOCKUP:
				{
					action->operation = AC_SEND_MSG;
					action->message = MSG_KEYDOWN;
					action->wParam = SCANCODE_CURSORBLOCKUP;
					action->lParam = 0;
					break;
				}
				case SCANCODE_CURSORBLOCKDOWN:
				{
					action->operation = AC_SEND_MSG;
					action->message = MSG_KEYDOWN;
					action->wParam = SCANCODE_CURSORBLOCKDOWN;
					action->lParam = 0;
					break;
				}

			}
		}
	}
	return;
}

int symbol_proc_msg(key_board_t* key_board, HWND hwnd, int message, WPARAM wParam, LPARAM lParam)
{
	static md_key_t *key_down;
	static char lbuttondown = 0;
	static POINT p;
	md_key_t *key;

	switch (message)
	{
		case MSG_LBUTTONDOWN: /* highlight key */
		{
			p.x = LOSWORD(lParam);
			p.y = HISWORD(lParam);
			lbuttondown = 1;
			key_board->action.operation = AC_NULL;

			if (hit_rect(key_board->close_rc, p))
			{
				key_board->action.operation = AC_NULL;
				close_update(key_board, hwnd, TRUE);
				return AC_NULL;
			}

			if (hit_rect(key_board->key_window->bound, p))
			{
				key = key_board->key_window->get_key(key_board->key_window, p);
				if (key == NULL)
				{
					key_board->action.operation = AC_NULL;
					break;
				}

				symbol_kw_proceed_hit(hwnd, key_board->view_window, key_board->stroke_window, key, &key_board->action, TRUE, p, EN, key_board->ime, wParam, lParam);
				key_down = key;
				break;
			}
			break;
		}
		case MSG_LBUTTONUP: /* send key */
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
			else if (close_key)
			{
				close_update(key_board, hwnd, FALSE);
			}

			if (hit_rect(key_board->key_window->bound, p))
			{
				key = key_board->key_window->get_key(key_board->key_window, p);
				if (key == NULL)
				{
					reset_mouse_state(hwnd);
					key_board->action.operation = AC_NULL;
					break;
				}

				symbol_kw_proceed_hit(hwnd, key_board->view_window, key_board->stroke_window, key, &key_board->action, FALSE, p, EN, key_board->ime, wParam, lParam);

				key_down = NULL;
				break;
			}
			else
			{
				reset_mouse_state(hwnd);
			}
			break;
		}
		case MSG_MOUSEMOVE:
		{
			p.x = LOSWORD(lParam);
			p.y = HISWORD(lParam);

			if (lbuttondown != 0 && close_key && !hit_rect(key_board->close_rc, p))
			{
				close_update(key_board, hwnd, FALSE);
			}
			else if (!close_key && hit_rect(key_board->close_rc, p))
			{
				close_update(key_board, hwnd, TRUE);
			}
			else if (close_key && !hit_rect(key_board->close_rc, p))
			{
				close_update(key_board, hwnd, FALSE);
			}

			if ((lbuttondown != 0) && (hit_rect(key_board->key_window->bound, p)))
			{
				key = key_board->key_window->get_key(key_board->key_window, p);

				if (key != key_down)
				{
					if (key_down != NULL)
					{
						if (key_down->style & KEY_PAD_DRAWED)
						{
							key_down->style &= ~KEY_PAD_DRAWED;
							SetAutoRepeatMessage(0, 0, 0, 0);
						}
						key_down->style &= ~KEY_PAD_PRESSED;
						key_down->update(key_down, hwnd);
						key_board->action.operation = AC_NULL;
					}
					if (key != NULL)
					{
						symbol_kw_proceed_hit(hwnd, key_board->view_window, key_board->stroke_window, key, &key_board->action, TRUE, p, EN, key_board->ime, wParam, lParam);
					}
					else
					{
						reset_mouse_state(hwnd);
					}
					key_down = key;
				}
				else
				{
					key_board->action.operation = AC_NULL;
				}
				break;
			}
			else
			{
				reset_mouse_state(hwnd);
				key_board->action.operation = AC_NULL;
				break;
			}
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

/*************************************************/
/**  interfaces for processing input strings  **/
/*************************************************/
int get_words_num(const char* buff)
{
	int n = 0;
	char *p = NULL;

	p = strchr(buff, ' ');
	while (p != NULL)
	{
		n++;
		p++;
		p = strchr(p, ' ');
	}

	return n;
}

//static char* get_word(const char* buff, int index, char *word)
char* get_word(const char* buff, int index, vw_element_t* e)
{
    char *p = NULL;
    int n = 0;
	char* word = e->string;

    if (strlen(buff) <= 0)
    {
    	return NULL;
    }

    p = strchr (buff, ' ');

    if (index == 0) 
	{
        strncpy(word, buff, p - buff);
        word[p-buff] = '\0';
		e->index = (char*)buff;
        return word;
    }

    while (p != NULL) 
	{
        n++;
        if (n == index) 
		{
            char *t;
            p++;
            t = strchr(p, ' ');

            if (t == NULL) 
			{
                strcpy (word, p);
				e->index = p;
            }
            else 
			{
                strncpy (word, p, t-p);
                word[t-p] = '\0';
				e->index = p;
            }

            return word;
        }
        p++;
        p = strchr(p, ' ');
    }
    return NULL;
}

void EraseBbGround(HWND hWnd, RECT* prc)
{
#if 1
    RECT rc = *prc;
    ClientToScreen (hWnd, &rc.left,  &rc.top);
    ClientToScreen (hWnd, &rc.right, &rc.bottom);
    SendAsyncMessage (hWnd, MSG_ERASEBKGND, 0,
            (LPARAM)&rc);
#else
    InvalidateRect(hWnd, prc, TRUE);
#endif
}

static BOOL get_substr_pos_ex (HDC hdc, const char* str, int index, vw_element_t* e, 
		RECT *rc, int* len, RECT* off)
{
	  SIZE textsize, cur_size, blank_size;
	  char* substr = e->string;
	
	  *len = strlen(substr);
	  GetTextExtent(hdc, substr, strlen(substr), &cur_size);
	  GetTextExtent(hdc, str, (e->index - str), &textsize);
	
	  if (LCD_SIZE_320X240 == getLCDSize())
	  {
		  GetTextExtent(hdc, "   ", 3, &blank_size);
	  }
	  else if (LCD_SIZE_480X272 == getLCDSize())
	  {
		  GetTextExtent(hdc, "	   ", 5, &blank_size);
	  }
	  else 
	  {
		  GetTextExtent(hdc, "	", 2, &blank_size);
	  }
	  
	  SetRect(rc, textsize.cx + off->left + blank_size.cx*index, off->top + 1, 
			  off->left + textsize.cx + cur_size.cx + blank_size.cx + 2 + blank_size.cx*index, 
			  off->bottom);
	  
	  return TRUE;

}

/*
static BOOL get_substr_pos_ex (HDC hdc, const char* str, int index, vw_element_t* e, 
		RECT *rc, int* len, RECT* off)
{
    SIZE textsize, cur_size, blank_size;
	char* substr = e->string;

	*len = strlen(substr);

    GetTextExtent(hdc, substr, strlen(substr), &cur_size);
    GetTextExtent(hdc, str, (e->index - str), &textsize);

    if (LCD_SIZE_320X240 == getLCDSize())
    {
		GetTextExtent(hdc, "     ", 5, &blank_size);
	}
	else if (LCD_SIZE_480X272 == getLCDSize())
	{
		GetTextExtent(hdc, "     ", 5, &blank_size);
	}
	else if (LCD_SIZE_240X320 == getLCDSize())
	{
		GetTextExtent(hdc, "  ", 2, &blank_size);
	}
    SetRect(rc, textsize.cx + off->left + blank_size.cx*index, off->top + 1, 
            off->left + textsize.cx + cur_size.cx + blank_size.cx + 2 + blank_size.cx*index, 
			off->bottom);
	
    return TRUE;
}
*/
void vw_set_elements (struct _view_window_t* view_window, HWND hwnd)
{
    int i;
    HDC hdc = GetDC(hwnd);
    PLOGFONT oldfont;
	int interval , textLen,winWidth;
	SIZE textsize ,blank_size;
	RECT temRc;
	RECT *rc = NULL;
	RECT r = view_window->bound;
	r.left = view_window->key_pg_up.right;
	r.right = view_window->key_pg_down.left;
	view_window->element_num = get_words_num (view_window->buffer);
	oldfont = SelectFont(hdc, view_window->view_font);
	winWidth = RECTW(r);

	if(view_window->element_num == 0)
	{
		view_window->clear_elements(view_window);
		goto END;
	}
	
	GetTextExtent(hdc, " ", 1, &blank_size);
	GetTextExtent(hdc, view_window->buffer, strlen(view_window->buffer), &textsize);
	textLen = textsize.cx - blank_size.cx*(view_window->element_num);
	interval = (winWidth - textLen)/(view_window->element_num + 1);
	SetRect(&temRc,0,0,r.left + interval/2,0);
	
    for (i = 0; i < view_window->element_num; i++ )
    {
		SIZE textsize, cur_size, blank_size;
		char* substr = NULL;
		const char* str = NULL;
		vw_element_t* e = NULL; 
		RECT* off = &r;
		
		get_word (view_window->buffer, i, &view_window->elements[i]);
		str = view_window->buffer;
		e = &view_window->elements[i]; 
		rc = &view_window->elements[i].bound;
		substr = e->string;
		view_window->elements[i].len = strlen(substr);
		
		GetTextExtent(hdc, substr, strlen(substr), &cur_size);
		
		SetRect(&temRc,
				temRc.right,
				off->top + 1,
				interval+temRc.right+cur_size.cx,
				off->bottom);
		CopyRect(rc,&temRc);
		
    }

END:
	SelectFont(hdc, oldfont);
    ReleaseDC (hdc);
    return;
}

void vw_clear_elements (struct _view_window_t* view_window)
{
    memset (view_window->elements, 0, sizeof(view_window->elements));
    view_window->element_num = 0;
    return;
}

vw_element_t* vw_get_element (struct _view_window_t* view_window, POINT p)
{
    int i;
    
    for (i = 0; i<view_window->element_num; i++)
    {
    	if (p.x >= view_window->elements[i].bound.left &&
            p.x <= view_window->elements[i].bound.right &&
            p.y >= view_window->elements[i].bound.top &&
            p.y <= view_window->elements[i].bound.bottom)
    	{
    		return &view_window->elements[i];
    	}
    }

    return NULL;
}
static void debug_key_rect ( HWND hWnd, key_window_t *kw)
{
    int i;
    HDC hdc = GetDC(hWnd);
    md_key_t* keys = kw->key;

    printf("caona -debug rect--kw->key_num=%d----\n", kw->key_num);
	SetPenWidth(hdc, 4);
	SetPenCapStyle(hdc, PT_CAP_BUTT);
	SetPenColor(hdc, PIXEL_red);

 //  	printf("caona bound left =%d, tip=%d, right=%d, bottom =%d---\n", kw->bound.left,  kw->bound.top,  kw->bound.right,  kw->bound.bottom);
 //   Rectangle(hdc, kw->bound.left, kw->bound.top, kw->bound.right, kw->bound.bottom);


    for(i = 0; i < kw->key_num; i++)
    {
    	printf("caona left =%d, tip=%d, right=%d, bottom =%d---\n", keys[i].bound.left, keys[i].bound.top, keys[i].bound.right, keys[i].bound.bottom);
    	Rectangle(hdc, keys[i].bound.left, keys[i].bound.top, keys[i].bound.right, keys[i].bound.bottom);
     }

    printf("caona -debug rect---end---\n");
    ReleaseDC(hdc);
}

void share_key_update(md_key_t *mk, HWND hWnd)
{
    HWND ttw;
    HDC hdc ;
    PBITMAP pbmp ;
    int x, y;
    char key_ch;
	SOFTKBD_DATA * pdata = NULL;

    if(mk->data == NULL)
    {
    	return;
    }
	
#ifdef KBD_TOOLTIP
	pdata = (SOFTKBD_DATA *)GetWindowAdditionalData(hWnd);
	
#if 0
    debug_key_rect(hWnd, pdata->keyboard->key_window);
#endif

    ttw = (HWND)pdata->tooltip_win;
    if (mk->style & KEY_PAD_PRESSED)
    {
        if ((mk->style & KEY_PAD_CHAR) && (pdata->skb_ime != MGIN_SKB9IME || pdata->current_board_idx > 1))
        {
			x = mk->bound.left + RECTW(mk->bound)/2 - tooltip_w/2;
			y = mk->bound.top - tooltip_h;
            if (islower(mk->key_char) && case_mode == PTI_CASE_ABC)
            {
            	key_ch = toupper(mk->key_char);
            }
            else
            {
            	key_ch = mk->key_char;
            }
            ClientToScreen (hWnd, &x, &y);

            ShowToolTip (ttw, x , y, key_ch);
        }
        else
		{
            hdc = GetDC(hWnd);
            pbmp = (PBITMAP)mk->data;
			if(mk->bound.bottom > 208)//解决最底一行电容屏坐标跳变
            {
            	FillBoxWithBitmap (hdc, mk->bound.left, mk->bound.top, 
                    RECTW(mk->bound), RECTH(mk->bound)-5, pbmp);
			}
			else
			{
            	FillBoxWithBitmap (hdc, mk->bound.left, mk->bound.top, 
                    RECTW(mk->bound), RECTH(mk->bound), pbmp);
			}
            ReleaseDC(hdc);
        }
    }
    else
    {
        HideToolTip (ttw);
        EraseBbGround(hWnd, &mk->bound);
    }
#else
    if (mk->style & KEY_PAD_PRESSED){
        hdc = GetDC(hWnd);
        pbmp = (PBITMAP)mk->data;
        FillBoxWithBitmap (hdc, mk->bound.left, mk->bound.top, 
                RECTW(mk->bound), RECTH(mk->bound), pbmp);
        ReleaseDC(hdc);
    } else {
        EraseBbGround(hWnd, &mk->bound);
    }
#endif
    return;
}

void clear_keyboard(key_board_t *kb)
{
	if(kb == NULL)
		return ;

	memset(&kb->action, 0 , sizeof(action_t));	

	if(kb->key_window)
	{
		int i;
		md_key_t* key;
		key = kb->key_window->key;
		for(i=0; i<kb->key_window->key_num; i++)
		{
			key[i].style &= (~(KEY_PAD_PRESSED|KEY_PAD_DRAWED));
		}
	}

	if (kb->view_window)
	{
		kb->view_window->buffer[0] = '\0';
		kb->view_window->style = 0; //clear style
		if (kb->view_window->clear_elements)
		{
			kb->view_window->clear_elements(kb->view_window);
		}
	}

	if (kb->select_window)
	{
		kb->select_window->buffer[0] = '\0';
		kb->select_window->style = 0; //clear style
		if (kb->select_window->clear_elements)
		{	
			kb->select_window->clear_elements(kb->select_window);
		}
	}

	if (kb->stroke_window)
	{
		kb->stroke_window->str[0] = '\0';
	}
}

char *getSoftkeyboardPublicPath(const char *fileName)
{
	static char publicBmpPathBuf[160] = {0};
	char envPathBuf[128] = {0};
	char *bmpFolderName = "commonres/softkeyboard/";

	memset(publicBmpPathBuf, 0, sizeof(publicBmpPathBuf));

	if(getenv("USERDATAPATH"))
	{
		snprintf(envPathBuf, sizeof(envPathBuf), "%s%s",
				getenv("USERDATAPATH"), bmpFolderName);
	}
	else
	{
		snprintf(envPathBuf, sizeof(envPathBuf), "%s",
				"/mnt/mtdblock/commonres");
	}

	sprintf(publicBmpPathBuf, "%s/%s", envPathBuf, fileName);

	return (char*)publicBmpPathBuf;
}

extern int gScreen_w;
extern int gScreen_h;

int getLCDSize(void)
{
	int lcdType = 0;

	if(gScreen_w == 320 && gScreen_h == 240)
	{
		lcdType = LCD_SIZE_320X240;
	}
	else if(gScreen_w == 400 && gScreen_h == 240)
	{
		lcdType = LCD_SIZE_400X240;
	}
	else if(gScreen_w == 240 && gScreen_h == 320)
	{
		lcdType = LCD_SIZE_240X320;
	}
	else if(gScreen_w == 480 && gScreen_h == 272)
	{
		lcdType = LCD_SIZE_480X272;
	}
	else if(gScreen_w == 480 && gScreen_h == 800)
	{
		lcdType = LCD_SIZE_480X800;
	}
	else if(gScreen_w == 600 && gScreen_h == 1024)
	{
		lcdType = LCD_SIZE_600X1024;
	}
	else if(gScreen_w == 800 && gScreen_h == 1280)
	{
		lcdType = LCD_SIZE_800X1280;
	}
	else if(gScreen_w == 720 && gScreen_h == 1280)
	{
		lcdType = LCD_SIZE_720X1280;
	}
	else if(gScreen_w == 1280 && gScreen_h == 720)
	{
		lcdType = LCD_SIZE_1280X720;
	}
	else if(gScreen_w == 800 && gScreen_h == 480)
	{
		lcdType = LCD_SIZE_800X480;
	}


	return lcdType;
}

static void InitResBmp()
{
	int i;
	for(i=0;i<RES_BMP_SIZE;i++)
	{	
		res_bmp[i].bmBits  = NULL;
	}

}

int load_keyboard_bitmap(int ime)
{
	int err = 0;
	InitResBmp();
	if (MGIN_SKB26IME == ime)
	{
		if(LoadBitmap(HDC_SCREEN, &res_bmp[EN26_KBD_BKG], getSoftkeyboardPublicPath("en26_kbd_bkg.png")))
		{
			err++;		
			printf("Fail to load en26_kbd_bkg bitmap.\n");
		}
		if(LoadBitmap(HDC_SCREEN, &res_bmp[EN26_KBD_BKG1], getSoftkeyboardPublicPath("en26_kbd_bkg1.png")))
		{
			err++;
			printf("Fail to load en26_kbd_bkg bitmap.\n");
		}
		if(LoadBitmap(HDC_SCREEN, &res_bmp[NUM26_KBD_BKG], getSoftkeyboardPublicPath("num26_kbd_bkg.png")))
		{
			err++;		
			printf("Fail to load num26_kbd_bkg bitmap.\n");
		}	
		if(LoadBitmap(HDC_SCREEN, &res_bmp[PINYIN26_KBD_BKG], getSoftkeyboardPublicPath("pinyin26_kbd_bkg.png")))
		{
			err++;
			printf("Fail to load pinyin26_kbd_bkg bitmap.\n");
		}
		if(LoadBitmap(HDC_SCREEN, &res_bmp[PUNCT26_KBD_BKG], getSoftkeyboardPublicPath("punct26_kbd_bkg.png")))
		{
			err++;
			printf("Fail to load punct26_kbd_bkg bitmap.\n");
		}
		if(LoadBitmap(HDC_SCREEN, &res_bmp[KOREAN26_KBD_BKG], getSoftkeyboardPublicPath("korean26_kbd_bkg.png")))
		{
			err++;
			printf("Fail to load korean26_kbd_bkg bitmap.\n");
		}
		if(LoadBitmap (HDC_SCREEN, &res_bmp[CLOSE_PRESSED], getSoftkeyboardPublicPath("close_pressed.png")))
		{
			err++;
			printf("Fail to load close_pressed bitmap.\n");
		}
		if(LoadBitmap (HDC_SCREEN, &res_bmp[CLOSE_NORMAL], getSoftkeyboardPublicPath("close_normal.png")))
		{
			err++;
			printf("Fail to load close_normal bitmap.\n");
		}
		
	}
	else if(MGIN_SKB9IME == ime)
	{
		if(LoadBitmap(HDC_SCREEN, &res_bmp[EN9_KBD_BKG], getSoftkeyboardPublicPath("en9_kbd_bkg.png")))
		{
			err++;		
			printf("Fail to load en9_kbd_bkg bitmap.\n");
		}
	
		if(LoadBitmap(HDC_SCREEN, &res_bmp[PINYIN9_KBD_BKG], getSoftkeyboardPublicPath("pinyin9_kbd_bkg.png")))
		{
			err++;
			printf("Fail to load pinyin9_kbd_bkg bitmap.\n");
		}
		if(LoadBitmap(HDC_SCREEN, &res_bmp[PUNCT9_KBD_BKG0], getSoftkeyboardPublicPath("punct9_kbd_bkg0.png")))
		{
			err++;
			printf("Fail to load punct9_kbd_bkg0 bitmap.\n");
		}
		if(LoadBitmap(HDC_SCREEN, &res_bmp[PUNCT9_KBD_BKG1], getSoftkeyboardPublicPath("punct9_kbd_bkg1.png")))
		{
			err++; 
			printf("Fail to load punct9_kbd_bkg1 bitmap.\n");
		}
		if(LoadBitmap(HDC_SCREEN, &res_bmp[PUNCT9_KBD_BKG2], getSoftkeyboardPublicPath("punct9_kbd_bkg2.png")))
		{
			err++;
			printf("Fail to load punct9_kbd_bkg2 bitmap.\n");
		}
		if(LoadBitmap(HDC_SCREEN, &res_bmp[PUNCT9_KBD_BKG3], getSoftkeyboardPublicPath("punct9_kbd_bkg3.png")))
		{
			err++;
			printf("Fail to load punct9_kbd_bkg3 bitmap.\n");
		}

	}
	
	if(LoadBitmap(HDC_SCREEN, &res_bmp[NUM9_KBD_BKG], getSoftkeyboardPublicPath("num9_kbd_bkg.png")))
	{
		err++;		
		printf("Fail to load num9_kbd_bkg bitmap.\n");
	}

	if(LoadBitmap (HDC_SCREEN, &res_bmp[CHAR_KEY_MASK], getSoftkeyboardPublicPath("char_key_mask.png")))
	{
		err++;
		printf("Fail to load char_key_mask bitmap.\n");
	}
	if(LoadBitmap (HDC_SCREEN, &res_bmp[FUNC_KEY_MASK], getSoftkeyboardPublicPath("char_key_mask.png")))
	{
		err++;
		printf("Fail to load func_key_mask bitmap.\n");
	} 																		 
	if(LoadBitmap (HDC_SCREEN, &res_bmp[LEFT_ARROW_ENABLE], getSoftkeyboardPublicPath("left_arrow_enable.png")))
	{
		err++;
		printf("Fail to load left_arrow_enbale bitmap.\n");
	}
	if(LoadBitmap (HDC_SCREEN, &res_bmp[LEFT_ARROW_DISABLE], getSoftkeyboardPublicPath("left_arrow_disable.png")))
	{
		err++;
		printf("Fail to load left_arrow_disable bitmap.\n");
	}
	if(LoadBitmap (HDC_SCREEN, &res_bmp[RIGHT_ARROW_ENABLE], getSoftkeyboardPublicPath("right_arrow_enable.png")))
	{
		err++;
		printf("Fail to load right_arrow_enable bitmap.\n");
	}
	if(LoadBitmap (HDC_SCREEN, &res_bmp[RIGHT_ARROW_DISABLE], getSoftkeyboardPublicPath("right_arrow_disable.png")))
	{
		err++;
		printf("Fail to load right_arrow_disable bitmap.\n");
	}

	if(err)
	{
		printf("Fail to load softkeyboard bitmap.=%d\n",err);
	}
 
	return err;
}

void release_keyboard_bitmap ()
{
	int i;
	for(i=0;i<RES_BMP_SIZE;i++)
	{	
		if(NULL != res_bmp[i].bmBits)
		{
			UnloadBitmap (&res_bmp[i]);
			res_bmp[i].bmBits = NULL;
		}
	}
	
	return;
}

int get_keyboard_bitmap (HDC hdc, PBITMAP* pbmp, int id)
{
    if (id  < 0 || id >= RES_BMP_SIZE) 
    {
    	return -1;
    }

	*pbmp = &res_bmp[id];

    return 0;
}
