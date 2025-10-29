/**
 * FileName: mgime.c
 *
 * Copyright (C) 2014-2015, ZKSoftware Inc.
 *
 * Created: 2014-8-5
 * Author: wilsn
 *
 * Description:
 */

#include <minigui/common.h> 
#include <minigui/minigui.h> 
#include <minigui/gdi.h> 
#include <minigui/window.h> 
#include <minigui/control.h> 
#include <minigui/endianrw.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "softkeywindow.h"
#include "softkeyphone.h"
#include "common.h"
#include "printlog.h"

#define MAX_IME_NAME    20

#define IME_OPEN			(SCANCODE_USER + 0x0153)
#define IME_CLOSE			(SCANCODE_USER + 0x0154)

#ifndef _STAND_ALONE
#define IME_REQID    (MAX_SYS_REQID + 9)
#endif
#define IME_HIDE      		1
#define IME_SHOW      		2
#define IME_GETSTATE      	3
#define IME_SETTARGET      	4
#define MGIN_SKB26IME      	300
#define MGIN_SKB9IME		301
typedef struct _REQUEST_DATA{
	HWND sfkb;
	int iMsg;
	WPARAM wParam; 
	LPARAM lParam;
} REQUEST_DATA;


/** IME Window Information */
typedef struct _IME_WIN {
    /** Handle of IME Window */
    HWND ime_hwnd;
    /** Name of IME Window */
    char ime_name[MAX_IME_NAME];
	
	BOOL is_reg;
} IME_WIN;


/** IME Window Container Information */
typedef struct _IME_CONTAINER {
    /** The IME container window */
    HWND cur_hwnd;
    /** The max number of IME Windows that the container contains */
    int max_ime_windows;
    /** The current IME window ID */
    int cur_wins;
    /** IME Window Information */
    IME_WIN* ime_wins;
} IME_CONTAINER;

enum _IME_WINDOW_STATUS {
    /**
     * IME window added successfully.
     */
    IME_WINDOW_SUCCESS,
    /**
     * the IME window that gonna add is invalid
     * or the container hasn't been created yet.
     */
    IME_WINDOW_INVALID,
    /**
     * the IME window has already added before. 
     */
    IME_WINDOW_EXISTED,
    /**
     * the IME window container is full. 
     */
    IME_WINDOW_FULL,
};

static IME_CONTAINER* ime_cont = NULL;
static GHANDLE ime_handle;
static HWND gCurHwnd = HWND_INVALID;

static GHANDLE mgiCreateIMEContainer (int max_ime_wins)
{
    int i;

    if (ime_cont != NULL)
    {
        fprintf (stderr, "The IME container already existed!\n");
        return 0;
    }
    ime_cont = malloc (sizeof(IME_CONTAINER));

    ime_cont->cur_hwnd = HWND_INVALID;
    ime_cont->max_ime_windows = max_ime_wins;
    ime_cont->cur_wins = 0;
    ime_cont->ime_wins = malloc (sizeof(IME_WIN)*max_ime_wins);

    for (i = 0; i < max_ime_wins; i++)
    {
    	ime_cont->ime_wins[i].ime_hwnd = HWND_INVALID;
		ime_cont->ime_wins[i].ime_name[0] = '\0';
		ime_cont->ime_wins[i].is_reg = FALSE;
	}
    return (GHANDLE)ime_cont;
}

static int mgiAddIMEWindow (GHANDLE ime, HWND ime_hwnd, const char* ime_name)
{
    IME_CONTAINER* ime_cont = (IME_CONTAINER*)ime;
    int i;
    if (ime_cont == NULL || !IsWindow(ime_hwnd))
        return IME_WINDOW_INVALID;
    if (ime_cont->cur_wins == ime_cont->max_ime_windows)
        return IME_WINDOW_FULL;

    for (i = 0; i < ime_cont->max_ime_windows; i++)
    {
        if (!strcmp (ime_cont->ime_wins[i].ime_name, ime_name))
            return IME_WINDOW_EXISTED;
    }

    for (i = 0; i < ime_cont->max_ime_windows; i++)
    {
        if (ime_cont->ime_wins[i].ime_hwnd == HWND_INVALID)
        {
            ime_cont->ime_wins[i].ime_hwnd = ime_hwnd;
            strncpy (ime_cont->ime_wins[i].ime_name, ime_name, MAX_IME_NAME);
            ime_cont->cur_wins ++;
            return IME_WINDOW_SUCCESS;
        }
    }

    return IME_WINDOW_FULL;
}

static BOOL mgiRemoveIMEWindow (GHANDLE ime, const char* ime_name)
{
    IME_CONTAINER* ime_cont = (IME_CONTAINER*)ime;
    int i;
    if (ime_cont == NULL || ime_cont->cur_wins == 0)
        return FALSE;

	ime_cont->cur_hwnd = HWND_INVALID;
    for (i = 0; i < ime_cont->max_ime_windows; i++)
    {
        if (!strcmp(ime_cont->ime_wins[i].ime_name, ime_name))
        {
            ime_cont->ime_wins[i].ime_hwnd = HWND_INVALID;
            ime_cont->ime_wins[i].ime_name[0] = '\0';
			ime_cont->ime_wins[i].is_reg= FALSE;
            return TRUE;
        }
    }
	
    return FALSE;
}

static HWND mgiSetActiveIMEWindow (GHANDLE ime, const char* ime_name)
{
    IME_CONTAINER* ime_cont = (IME_CONTAINER*)ime;
    int i, found = -1;

    if (ime_cont == NULL || ime_cont->cur_wins == 0)
        return HWND_INVALID;

    for (i = 0; i < ime_cont->max_ime_windows; i++)
    {
        if (found < 0 && !strcmp(ime_cont->ime_wins[i].ime_name, ime_name))
        {
            found = i;
        } 
		else if(IsWindow(ime_cont->ime_wins[i].ime_hwnd))
		{
			if(ime_cont->ime_wins[i].is_reg)
            {
            	UnregisterIMEWindow (ime_cont->ime_wins[i].ime_hwnd);
				ime_cont->ime_wins[i].is_reg = FALSE;
			}
			
            ShowWindow (ime_cont->ime_wins[i].ime_hwnd, SW_HIDE);
        }
    }

    if (found >= 0 && IsWindow(ime_cont->ime_wins[found].ime_hwnd))
    {
        int ret_value;		
		HWND ime_hwnd = HWND_INVALID;
		BOOL is_reg = ime_cont->ime_wins[found].is_reg;
		ime_hwnd = ime_cont->ime_wins[found].ime_hwnd;
		if(ime_cont->cur_hwnd != ime_hwnd)
        {
        	if(!is_reg)
            {
        		ret_value = RegisterIMEWindow (ime_hwnd);
				if(ERR_OK == ret_value)
				{
					ime_cont->ime_wins[found].is_reg = TRUE;
				}
        	}
			ime_cont->cur_hwnd = ime_hwnd;
		}
        return ime_hwnd;
    }
    return HWND_INVALID;
}

static BOOL mgiDestroyIMEContainer (GHANDLE ime)
{
    IME_CONTAINER* ime_cont = (IME_CONTAINER*)ime;
    int i;

    if (ime_cont == NULL)
        return FALSE;

    ime_cont->cur_hwnd = HWND_INVALID;

    for (i = 0; i < ime_cont->max_ime_windows; i++)
    {

		if (IsWindow(ime_cont->ime_wins[i].ime_hwnd ))
	    {		
			BOOL is_reg = ime_cont->ime_wins[i].is_reg;
	    	if(is_reg)
	    	{
				UnregisterIMEWindow (ime_cont->ime_wins[i].ime_hwnd);
			}
	    	DestroyMainWindow (ime_cont->ime_wins[i].ime_hwnd);
		}
    }
    
    free (ime_cont->ime_wins);
    free (ime_cont);
    ime_cont = NULL;

    return TRUE;
}
 
static int imeHandler (int cli, int clifd, void* buff, size_t len)
{
	REQUEST_DATA *data = (REQUEST_DATA*)buff;
	IME_CONTAINER* ime_cont = (IME_CONTAINER*)ime_handle;
	int ret = 0;
	HWND mSoftKeypad = ime_cont->cur_hwnd;

    printf("[softkeyboard]======\nimeHandler");
	if(sizeof(REQUEST_DATA) != len || buff == NULL)
	{
		goto ERR;
	}

	switch(data->iMsg)
	{
		case IME_SHOW:
		{
			HWND hskb = HWND_INVALID;
			if(MGIN_SKB26IME == data->sfkb)
			{
				hskb = mgiSetActiveIMEWindow (ime_handle, "skb26");
			}
			else if(MGIN_SKB9IME == data->sfkb)
			{
				hskb = mgiSetActiveIMEWindow (ime_handle, "skb9");
			}
			
			ret = SendMessage(hskb, IME_OPEN, data->wParam, data->lParam);
			ret = (ret==0?TRUE:FALSE);

		}break;
		case IME_HIDE:
		{
			ret = SendMessage(ime_cont->cur_hwnd, IME_CLOSE, data->wParam, data->lParam);
			ret = (ret==0?TRUE:FALSE);

		}break;
		case IME_GETSTATE:
		{
			ret = SendMessage(ime_cont->cur_hwnd, MSG_IME_GETSTATUS, data->wParam, data->lParam);

		}break;
		case IME_SETTARGET:
		{
			HWND ime_hwnd;
			int i;
			for (i = 0; i < ime_cont->max_ime_windows; i++)
			{	
				ime_hwnd = ime_cont->ime_wins[i].ime_hwnd ;
				if(ime_hwnd == HWND_INVALID)
				{
					continue;
				}
			
				ret = SendMessage(ime_hwnd, MSG_IME_SETTARGET, data->wParam, data->lParam);
			}

		}break;
		defaule:
			;
			
	}
	
ERR:	
#ifndef _STAND_ALONE
	return ServerSendReply (clifd, &ret, sizeof(int));
#else
	return ret;
#endif
	
}

static int ImeHandler (int cli, int clifd, void* buff, size_t len)
{
	REQUEST_DATA *data = (REQUEST_DATA*)buff;
	int ret = 0;

	if(sizeof(REQUEST_DATA) != len || buff == NULL || !IsWindow(gCurHwnd))
	{
		goto ERR;
	}

	switch(data->iMsg)
	{
		case IME_SHOW:
		{
			ret = SendMessage(gCurHwnd, IME_OPEN, data->wParam, data->lParam);
			ret = (ret==0?TRUE:FALSE);

		}break;
		case IME_HIDE:
		{
			ret = SendMessage(gCurHwnd, IME_CLOSE, data->wParam, data->lParam);
			ret = (ret==0?TRUE:FALSE);

		}break;
		case IME_GETSTATE:
		{
			ret = SendMessage(gCurHwnd, MSG_IME_GETSTATUS, data->wParam, data->lParam);

		}break;
		case IME_SETTARGET:
		{
			ret = SendMessage(gCurHwnd, MSG_IME_SETTARGET, data->wParam, data->lParam);

		}break;
		defaule:
			;
			
	}
	
ERR:	
#ifndef _STAND_ALONE
	return ServerSendReply (clifd, &ret, sizeof(int));
#else
	return ret;
#endif
	
}

static void proceed_sfkbd(BOOL skbd_is_shown)
{
	if(skbd_is_shown) {
#ifdef ENABLE_MLSSHELL
		MLSKeyboardShow(FALSE);
		MLSKeyboardSetActive(FALSE);
#endif
	} else {
#ifdef ENABLE_MLSSHELL
		MLSKeyboardSetActive(TRUE);
#endif
	}
}

int CreateSoftKeypadIme (void)
{
	HWND sk_ime_hwnd=HWND_INVALID, ph_ime_hwnd=HWND_INVALID;
#ifndef _STAND_ALONE
	SetImeLogTypesByParams("EWID");
#endif
	SetImeDynamicName("softkeyboard");
	printf("\033[4;32m =========== CreateSoftKeypadIme =========== \033[0m\n");
//	PrintImeInfo("start....");
	//if(LCD_SIZE_480X272 == getLCDSize())
	{
	  //  ime_handle = mgiCreateIMEContainer (2);
		//load_keyboard_bitmap(MGIN_SKB26IME);
	    gCurHwnd = CreateSoftKeypad(MGIN_SKB9IME, proceed_sfkbd);
	  //  ph_ime_hwnd = CreateSoftKeyPhone(proceed_sfkbd);
	 //   mgiAddIMEWindow (ime_handle, sk_ime_hwnd, "skb26");
	//	mgiAddIMEWindow (ime_handle, ph_ime_hwnd, "skb9");
		
	}
	/*
	else if(LCD_SIZE_240X320== getLCDSize())
	{
		int err = 0;
	    //ime_handle = mgiCreateIMEContainer (1);
		err = load_keyboard_bitmap(MGIN_SKB9IME);
	    gCurHwnd = CreateSoftKeypad(MGIN_SKB9IME,proceed_sfkbd);
		//mgiAddIMEWindow (ime_handle, ph_ime_hwnd, "skb9");
		printf("\033[4;32m LCD_SIZE_240X320 err=%d, gCurHwnd=%d\033[0m\n", err, gCurHwnd);
		
	}
	else if(LCD_SIZE_320X240== getLCDSize())
	{
		int err = 0;
	    //ime_handle = mgiCreateIMEContainer (1);
		err = load_keyboard_bitmap(MGIN_SKB9IME);
	    gCurHwnd = CreateSoftKeypad(MGIN_SKB9IME,proceed_sfkbd);
		//mgiAddIMEWindow (ime_handle, ph_ime_hwnd, "skb9");	
		
		printf("\033[4;32m LCD_SIZE_320X240 err=%d, gCurHwnd=%d\033[0m\n", err, gCurHwnd);
	}
	*/
#ifndef _STAND_ALONE
	if(IsWindow(gCurHwnd) && !RegisterRequestHandler (IME_REQID, ImeHandler)) 
	{
		DestroySoftKeypadIme();
		fprintf (stderr, "Can not register sfkbd handler.\n");
		printf("\033[4;31m libsoftkeywin.so Create SoftKeyboardIme error: Can not register sfkbd handler \033[0m\n");
		return -1;
	}
#endif

//	PrintImeInfo("end....");
	return 0;
}

void DestroySoftKeypadIme(void)
{
	if(IsWindow(gCurHwnd))
	{
		DestroyMainWindow (gCurHwnd);
	}
 	//mgiDestroyIMEContainer(ime_handle);
	release_keyboard_bitmap();
}

void HideSoftKeypadIme(void)
{
	HideSoftKeyWindow(gCurHwnd);
}

