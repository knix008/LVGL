/**
 * FileName: softkeyphone.c
 *
 * Copyright (C) 2014-2015, ZKSoftware Inc.
 *
 * Created: 2014-8-5
 * Author: wilsn
 *
 * Description:
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING 
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION

#include "config.h"
#include "libime/ime.h"
#include "libime/mgpti.h"
#include "softkeyphone.h"
#include "softkeypad/softkeypad.h"
#include "common_animates/common_animates.h"
#include "printlog.h"


#define SFKB_CONFIG_FILE  "etc/sfkb9.cfg"
#define SFKB_NUM 4

static void (*op_cb)(BOOL) = NULL;
static key_board_t* keyboard [SFKB_NUM];
static int m_nImeAllowMode = IME_ALLOWMODE;
static int m_nImeCurMode = IME_DEFAULT;
static void *handle = NULL;
static BOOL gImeIsOpen = FALSE;

static int screen_w = 0;
static int screen_h = 0;
static int skb_win_w = 0;
static int skb_win_h = 0;

static int GetNextMode(int nMode)
{
	int nNewMode = nMode;
	if (nNewMode == m_nImeAllowMode)
	{
		return nNewMode;
	}
	while (1)
	{
		nNewMode = nNewMode << 1;
		if (nNewMode > m_nImeAllowMode)
		{
			nNewMode = 1;
		}
		if ((nNewMode & m_nImeAllowMode) == nNewMode)
		{
			return nNewMode;
		}
		if (nNewMode == nMode)
		{
			return nNewMode;
		}

	}
}

static void ChangeKBDFun(HWND hWnd, SOFTKBD_DATA* pdata, int ime_mode)
{
	int y;
	int mode = ime_mode;
	mode &= m_nImeAllowMode;

	switch (mode)
	{
		case IME_MODE_ALPHABET:
		{
			y = 0;
			break;
		}
		case IME_MODE_PY:
		{
			y = 1;
			break;
		}
		case IME_MODE_NUMBER:
		{
			y = 2;
			break;
		}
		case IME_MODE_SYMBOL:
		{
			y = 3;
			break;
		}
		case IME_MODE_KOREAN:
		{
			y = 5;
			break;
		}
		default:
		{
			ChangeKBDFun(hWnd, pdata, GetNextMode(ime_mode));
			return;
		}

	}
	pdata->current_board_idx = y;
	pdata->keyboard->clear(pdata->keyboard);
	pdata->keyboard = keyboard[pdata->current_board_idx];
	SendMessage(hWnd, MSG_ERASEBKGND, 0, 0L);
	softkey_reset();
	
}

static void send_word(HWND target_hwnd, char *word, int type)
{
	int i = 0;
	int len = strlen(word);

	switch (type)
	{
		case AC_SEND_EN_STRING:
		{
			for (i = 0; i < len; i++)
			{
#if defined(_MGRM_PROCESSES) && (MINIGUI_MAJOR_VERSION > 1) && !defined(_STAND_ALONE)
				Send2ActiveWindow (mgTopmostLayer, MSG_CHAR, word[i], 1);
#elif defined(_MGRM_THREADS) && !defined(_STAND_ALONE)
				PostMessage(target_hwnd, MSG_CHAR, word[i], 0);
#endif
			}

#if defined(_MGRM_PROCESSES) && (MINIGUI_MAJOR_VERSION > 1) && !defined(_STAND_ALONE)
			Send2ActiveWindow (mgTopmostLayer, MSG_IME_CHAR_END, 0, 0);
#elif defined(_MGRM_THREADS) && !defined(_STAND_ALONE)
			PostMessage(target_hwnd, MSG_IME_CHAR_END, 0, 0);
#endif
			break;
		}
		case AC_SEND_CN_STRING:
		{
			WORD wDByte;
			for (i = 0; i < len; i += 2)
			{
				wDByte = MAKEWORD(word[i], word[i + 1]);
#if defined(_MGRM_PROCESSES) && (MINIGUI_MAJOR_VERSION > 1) && !defined(_STAND_ALONE)
				Send2ActiveWindow (mgTopmostLayer, MSG_CHAR, wDByte, 2);
#elif defined(_MGRM_THREADS) && !defined(_STAND_ALONE)
				PostMessage(target_hwnd, MSG_CHAR, wDByte, 0);
#endif
			}

#if defined(_MGRM_PROCESSES) && (MINIGUI_MAJOR_VERSION > 1) && !defined(_STAND_ALONE)
			Send2ActiveWindow (mgTopmostLayer, MSG_IME_CHAR_END, 0, 0);
#elif defined(_MGRM_THREADS) && !defined(_STAND_ALONE)
			PostMessage(target_hwnd, MSG_IME_CHAR_END, 0, 0);
#endif

			break;
		}
	}

	return;

}

static key_board_t* init_keypad_data(HWND hWnd)
{
	/* for english key board */
	if (!(keyboard[0] = (key_board_t*) calloc(1, sizeof(key_board_t))))
	{
		_MY_PRINTF("calloc keyboard data for EN failed\n");
		return NULL;
	}
	if (-1 == init_en_keypad(handle, hWnd, keyboard[0]))
	{
		_MY_PRINTF("error for initalize En-keyboard.\n");
		return NULL;
	}

	/* for py key board */
	if (!(keyboard[1] = (key_board_t*) calloc(1, sizeof(key_board_t))))
	{
		_MY_PRINTF("calloc keyboard data for EN failed\n");
		return NULL;
	}
	if (-1 == init_py_keypad(handle, hWnd, keyboard[1]))
	{
		_MY_PRINTF("error for initalize PinYin-keyboard.\n");
		return NULL;
	}
	/* for num key board */
	if (!(keyboard[2] = (key_board_t*) calloc(1, sizeof(key_board_t))))
	{
		_MY_PRINTF("calloc keyboard data for NUM failed\n");
		return NULL;
	}
	if (-1 == init_num_keypad(handle, hWnd, keyboard[2]))
	{
		_MY_PRINTF("error for initalize num-keyboard.\n");
		return NULL;
	}

	/* for punct key board */
	if (!(keyboard[3] = (key_board_t*) calloc(1, sizeof(key_board_t))))
	{
		_MY_PRINTF("calloc keyboard data for punctuation failed\n");
		return NULL;
	}
	if (-1 == init_punct_keypad(handle, hWnd, keyboard[3]))
	{
		_MY_PRINTF("error for initalize Puctuation-keyboard.\n");
		return NULL;
	}

	//return the default keyboard at beginning.
	return keyboard[0];
}

static destroy_key_win (void)
{
	//FIXME:
	if(NULL != keyboard[0])
    {
    	destroy_en_keypad (keyboard[0]);
    	free(keyboard[0]);
		keyboard[0] = NULL;
	}
	if(NULL != keyboard[1])
    {
		destroy_py_keypad (keyboard[1]);
	    free(keyboard[1]);		
		keyboard[1] = NULL;
	}
	if(NULL != keyboard[2])
    {
		destroy_num_keypad (keyboard[2]);
	    free(keyboard[2]);		
		keyboard[2] = NULL;
	}
	if(NULL != keyboard[3])
    {
	    destroy_punct_keypad (keyboard[3]);
	    free(keyboard[3]);		
		keyboard[3] = NULL;
	}
	
}
 
static void on_imewnd_jmp_finished(ANIMATE_SENCE* as)
{
	if(as != NULL)
	{
		SOFTKBD_DATA* pdata = (SOFTKBD_DATA*)as->param;
		gImeIsOpen = pdata->is_opened;
	}
	else
	{
		gImeIsOpen = FALSE;
	}
	
    if (op_cb)
    {
    	op_cb(gImeIsOpen);
    }

}

static BOOL show_ime_window(HWND hWnd, SOFTKBD_DATA* pdata, BOOL show, BOOL isAnimates, int ime_mode)
{
	int x, y;
	if (!pdata || pdata->is_opened == show)
	{
		return FALSE;
	}

	pdata->is_opened = show;
	ChangeKBDFun(hWnd, pdata, ime_mode);
	if (!show) //hide
	{
		if (pdata && pdata->keyboard && pdata->keyboard->clear)
			pdata->keyboard->clear(pdata->keyboard);
	}

	x = 0;
	y = screen_h - 1;
	if (show)
	{
		SetInterval(1);
		MoveWindow(hWnd, x, y - 1, skb_win_w, skb_win_h, FALSE);
		ShowWindow(hWnd, SW_SHOW);
		RunJumpWindow(hWnd, x, y - 1, x, y - skb_win_h, skb_win_w, skb_win_h, on_imewnd_jmp_finished, pdata);
	}
	else
	{
		if (isAnimates)
		{
			// use animates
			RunJumpWindow(hWnd, x, y - skb_win_h, x, y, skb_win_w, skb_win_h, on_imewnd_jmp_finished, pdata);
		}
		else
		{
			ShowWindow(hWnd, SW_HIDE);
			on_imewnd_jmp_finished(NULL);
		}
	}

	return TRUE;

}


static int DefaultIMEWinProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	SOFTKBD_DATA* pdata = NULL;

	if (message != MSG_CREATE)
	{
		pdata = (SOFTKBD_DATA*) GetWindowAdditionalData(hWnd);
	}

	switch (message)
	{
		case MSG_IME_GETSTATUS:
		{
			if (pdata->is_opened)
			{
				return pdata->is_opened;
			}
			else
			{
				return (int) gImeIsOpen;
			}
			break;
		}
		case MSG_IME_SETSTATUS:
		{
			break;
		}
		case MSG_IME_SETTARGET:
		{
			if ((HWND) wParam != hWnd)
				pdata->target_hwnd = (HWND) wParam;
			return 0;
		}
		case MSG_IME_GETTARGET:
		{
			return (int) pdata->target_hwnd;
		}
		case IME_OPEN:
		{
			BOOL ret;
			
			PrintImeInfo("MSG_IME_OPEN,wParam[%d],lParam[%d]\n", wParam, lParam);
			if (!pdata || pdata->is_opened)
			{
				return FALSE;
			}

			if (wParam >= IME_MODE_ALPHABET && wParam <= IME_ALLOWMODE && (wParam & lParam))
			{
				m_nImeAllowMode = wParam;
				m_nImeCurMode = lParam;
			} else
			{
				m_nImeAllowMode = IME_ALLOWMODE;
				m_nImeCurMode = IME_DEFAULT;
			}

			ret = show_ime_window(hWnd, pdata, TRUE, TRUE, m_nImeCurMode);
			if (ret)
			{
#if defined(_MGRM_PROCESSES) && (MINIGUI_MAJOR_VERSION > 1) && !defined(_STAND_ALONE)
				Send2ActiveWindow (mgTopmostLayer, MSG_IME_SHOW, 0, 0);
#elif defined(_MGRM_THREADS) && !defined(_STAND_ALONE)
				PostMessage(pdata->target_hwnd, MSG_IME_SHOW, 0, 0);
#endif
				InvalidateRect(hWnd, NULL, TRUE);
			}
			break;
		}
		case IME_CLOSE:
		{
			BOOL ret;
#ifdef KBD_TOOLTIP
			HideToolTip((HWND) ((SOFTKBD_DATA *) GetWindowAdditionalData(hWnd))->tooltip_win);
#endif
			reset_mouse_state(hWnd);

			ret = show_ime_window(hWnd, pdata, FALSE, FALSE, m_nImeCurMode);
			PrintImeInfo("MSG_IME_CLOSE,wParam[%d],lParam[%d]\n", wParam, lParam);
			if (ret)
			{
#if defined(_MGRM_PROCESSES) && (MINIGUI_MAJOR_VERSION > 1) && !defined(_STAND_ALONE)
				Send2ActiveWindow (mgTopmostLayer, MSG_IME_HIDE, 0, 0);
#elif defined(_MGRM_THREADS) && !defined(_STAND_ALONE)
				PostMessage(pdata->target_hwnd, MSG_IME_HIDE, 0, 0);
#endif
			}

			break;
		}


	}

	return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

static int SoftKeyPhProc(HWND hWnd, int message, WPARAM wParam, LPARAM lParam)
{
	SOFTKBD_DATA* pdata = NULL;

	if (message != MSG_CREATE)
	{
		pdata = (SOFTKBD_DATA*) GetWindowAdditionalData(hWnd);
	}

	switch (message)
	{
		case MSG_NCCREATE:
			//RegisterIMEWindow(hWnd);
			break;
		case MSG_CREATE:
		{

            printf("[softkeyboard]======\nMSG_CREATE");

			if (!(pdata = (SOFTKBD_DATA*) calloc(1, sizeof(SOFTKBD_DATA))))
			{
				_MY_PRINTF("Fail to calloc SOFTKBD data.\n");
				return -1;
			}
			memset(pdata, 0, sizeof(pdata));
			pdata->current_board_idx = 0;
			pdata->skb_ime = MGIN_SKB9IME;

			if (NULL == (pdata->keyboard = init_keypad_data(hWnd)))
			{
				_MY_PRINTF("Soft Key Window init failed\n");
				return -1;
			}

#ifdef KBD_TOOLTIP
			pdata->tooltip_win = CreateToolTip(hWnd);
#endif
			SetWindowAdditionalData(hWnd, (DWORD) pdata);
			break;
		}
		case MSG_KEYDOWN:
		case MSG_KEYUP:
		{
#if defined(_MGRM_PROCESSES) && (MINIGUI_MAJOR_VERSION > 1) && !defined(_STAND_ALONE)
			Send2ActiveWindow (mgTopmostLayer, message, wParam, lParam);
#elif defined(_MGRM_THREADS) && !defined(_STAND_ALONE)
			PostMessage(pdata->target_hwnd, message, wParam, lParam);
#endif
			return 0;
		}
		case MSG_LBUTTONDOWN:
		case MSG_LBUTTONUP:
		case MSG_MOUSEMOVE:
		case MSG_NCMOUSEMOVE:
		{
			if (!gImeIsOpen)
			{
				return 0;
			}

#ifdef KBD_TOOLTIP 
			if (MSG_LBUTTONUP == message)
			{
				HideToolTip((HWND) ((SOFTKBD_DATA *) GetWindowAdditionalData(hWnd))->tooltip_win);
			}
#endif
			if (MSG_LBUTTONDOWN == message || MSG_LBUTTONUP == message)
				reset_mouse_state(hWnd);

			switch (pdata->keyboard->proceed_msg(pdata->keyboard, hWnd, message, wParam, lParam))
			{
				case AC_CHANGE_KBD:
				{
					POINT p;
					md_key_t* key;
					int ime_mode;
					p.x = LOSWORD(lParam);
					p.y = HISWORD(lParam);
					key = pdata->keyboard->key_window->get_key(pdata->keyboard->key_window, p);
					ime_mode = key->scan_code - SCANCODE_USER;
					ChangeKBDFun(hWnd, pdata, ime_mode);
					return 0;
				}
				case AC_SEND_EN_STRING:
				{
					send_word(pdata->target_hwnd, pdata->keyboard->action.str, AC_SEND_EN_STRING);
					break;
				}
				case AC_SEND_CN_STRING:
				{
					send_word(pdata->target_hwnd, pdata->keyboard->action.str, AC_SEND_CN_STRING);
					break;
				}
				case AC_SEND_MSG:
				{

#if defined(_MGRM_PROCESSES) && (MINIGUI_MAJOR_VERSION > 1) && !defined(_STAND_ALONE)
					Send2ActiveWindow (mgTopmostLayer,
							pdata->keyboard->action.message,
							pdata->keyboard->action.wParam,
							pdata->keyboard->action.lParam);
#elif defined(_MGRM_THREADS) && !defined(_STAND_ALONE)
					PostMessage(pdata->target_hwnd,
							pdata->keyboard->action.message,
							pdata->keyboard->action.wParam,
							pdata->keyboard->action.lParam);
#endif
					return 0;
				}
				default:
				{
					if (MSG_LBUTTONDOWN == message)
					{
#if defined(_MGRM_PROCESSES) && (MINIGUI_MAJOR_VERSION > 1) && !defined(_STAND_ALONE)
						Send2ActiveWindow (mgTopmostLayer, MSG_CHAR, 0, 0);
#elif defined(_MGRM_THREADS) && !defined(_STAND_ALONE)
						PostMessage(pdata->target_hwnd, MSG_CHAR, 0, 0);
#endif

					}
				}

			}

			break;
		}
		case MSG_PAINT:
		{
			HDC hdc = BeginPaint(hWnd);
			do
			{
				view_window_t* view_window = pdata->keyboard->view_window;
				view_window_t* select_window = pdata->keyboard->select_window;
				stroke_window_t* stroke_window = pdata->keyboard->stroke_window;

				if (view_window == NULL || stroke_window == NULL || select_window == NULL)
				{
					break;
				}
				view_window->style |= VW_DRAW_ELMTS;
				view_window->style &= ~VW_EL_PRESSED;
				view_window->update(view_window, hWnd, NULL);
				select_window->style |= VW_DRAW_ELMTS;
				select_window->style &= ~VW_EL_PRESSED;
				select_window->update(select_window, hWnd, NULL);
				stroke_window->update(stroke_window, hWnd);
			
			} while (FALSE);

			EndPaint(hWnd, hdc);
			return 0;
		}
		case MSG_ERASEBKGND:
		{
			if (pdata && pdata->keyboard && pdata->keyboard->update)
			{
				pdata->keyboard->update(pdata->keyboard, hWnd, wParam, (RECT*) lParam);
			}
			return 0;
		}
		case MSG_CLOSE:
		{
			destroy_key_win();
#ifdef KBD_TOOLTIP
			DestroyMainWindow(pdata->tooltip_win);
#endif
			free(pdata);
			DestroyMainWindow(hWnd);
			PostQuitMessage(hWnd);

			return 0;
		}
	}
	return DefaultIMEWinProc(hWnd, message, wParam, lParam);
}

static void init_createinfo(PMAINWINCREATE pCreateInfo)
{
    pCreateInfo->dwStyle = WS_ABSSCRPOS | WS_VISIBLE;
    pCreateInfo->dwExStyle = WS_EX_TOPMOST;
    pCreateInfo->spCaption = "Soft Key Window" ;
    pCreateInfo->hMenu = 0;
    pCreateInfo->hCursor = GetSystemCursor (0);
    pCreateInfo->hIcon = 0;
    pCreateInfo->MainWindowProc = SoftKeyPhProc;
    pCreateInfo->lx = 0; 
    pCreateInfo->ty = screen_h;
    pCreateInfo->rx = skb_win_w;
    pCreateInfo->by = screen_h;
    pCreateInfo->iBkColor = COLOR_lightwhite; 
    pCreateInfo->dwAddData = 0;

}

static HWND create_ime_ph(HWND hosting)
{
    MAINWINCREATE CreateInfo;
    HWND hMainWnd;
    int valueCfg = -1;
	handle = InitMigConfig(getSoftkeyboardPublicPath(SFKB_CONFIG_FILE));
	if (handle == NULL)
	{
//		RECT rect;
//		printf("\033[4;32m env init error:%d \033[0m\n",FindMigCfgIntValue(handle,"SKB_WIN_H"));
//		printf("\033[4;32m env init error:%d \033[0m\n",FindMigCfgRectValue(handle,"RECT_EN_KEY_1"));
		printf("Softkeyboard init no such file and use defaule config FILE[%s],FUNCTION[%s],LINE[%d], filePath[%s] \n", __FILE__, __FUNCTION__, __LINE__,getSoftkeyboardPublicPath(SFKB_CONFIG_FILE));
	}

	if((NULL != handle) && (valueCfg = FindMigCfgIntValue(handle, "SCREEN_H")) >= 0)
	{
		screen_h = valueCfg;
	}
	else
	{
		screen_h = SCREEN_DEFAULT_H;
	}

	if((NULL != handle) && (valueCfg = FindMigCfgIntValue(handle, "SKB_WIN_H")) >= 0)
	{
		skb_win_h = valueCfg;
	}
	else
	{
		skb_win_h = SKB_WIN_DEFAULT_H;
	}

	if((NULL != handle) && (valueCfg = FindMigCfgIntValue(handle, "SKB_WIN_W")) >= 0)
	{
		skb_win_w = valueCfg;
	}
	else
	{
		skb_win_w = SKB_WIN_DEFAULT_W;
	}

    init_createinfo(&CreateInfo);
    CreateInfo.hHosting = hosting;

    hMainWnd = CreateMainWindow(&CreateInfo);
    if (hMainWnd == HWND_INVALID)
    {
    	return HWND_INVALID;
    }
    ReleaseMigCfg(handle);
    return hMainWnd;
}

#ifdef _MGRM_PROCESSES
HWND CreateSoftKeyPhone(void (*cb)(BOOL IsShown))
{
	op_cb = cb; 
	return create_ime_ph(HWND_DESKTOP); 
}

#else

typedef struct ime_info {
    sem_t wait;
    HWND hwnd;
} IME_INFO;

static void* start_ime(void* data)
{
	MSG Msg;
	IME_INFO* ime_info = (IME_INFO*) data;
	HWND ime_hwnd;

	ime_hwnd = ime_info->hwnd = create_ime_ph(HWND_DESKTOP);
	if (ime_hwnd == HWND_INVALID)
		return NULL;

	sem_post(&ime_info->wait);

	while (GetMessage(&Msg, ime_hwnd))
	{
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	MainWindowThreadCleanup(ime_hwnd);

	return NULL;
}

static pthread_t imethread;

/* the argument of 'hosting' is ignored. */
HWND CreateSoftKeyPhone(void (*cb)(BOOL IsShown))
{
	op_cb = cb; 
    IME_INFO ime_info;
    pthread_attr_t new_attr;

    sem_init(&ime_info.wait, 0, 0);

    pthread_attr_init(&new_attr);
    pthread_attr_setdetachstate(&new_attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&imethread, &new_attr, start_ime, &ime_info);
    pthread_attr_destroy(&new_attr);

    sem_wait(&ime_info.wait);
    sem_destroy(&ime_info.wait);

    return ime_info.hwnd;
}

#endif


