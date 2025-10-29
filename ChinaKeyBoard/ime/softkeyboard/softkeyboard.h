/*
** $Id: softkeyboard.h 991 2008-08-31 02:02:40Z houhuihua $
**
** softkeyboard.h: soft keyboard head file. 
**
** Copyright (C) 2003 ~ 2008 Feynman Software.
** Copyright (C) 1999 ~ 2002 Wei Yongming.
**
** Create date: 2008/07/15
**
*/


#ifndef __SOFTKEYBOARD_H__
#define __SOFTKEYBOARD_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "../../config.h"
#include "../common.h"

#define _MY_PRINTF(fmt...) fprintf (stderr, fmt)

#define __FILL_DIRECT__			1

#define SCREEN_DEFAULT_W 	800
#define SCREEN_DEFAULT_H 	1280

#define SKB_WIN_DEFAULT_W	  	800
#define SKB_WIN_DEFAULT_H  		580

#define SKB_CLOSE_DEFAULT_L 	690
#define SKB_CLOSE_DEFAULT_T 	10
#define SKB_CLOSE_DEFAULT_R 	790
#define SKB_CLOSE_DEFAULT_B 	70

#define SKB_VW_DEFAULT_L 	10
#define SKB_VW_DEFAULT_T 	10
#define SKB_VW_DEFAULT_R 	680
#define SKB_VW_DEFAULT_B 	70

#define SKB_VW_PU_DEFAULT_L 	50
#define SKB_VW_PU_DEFAULT_T 	10
#define SKB_VW_PU_DEFAULT_R 	100
#define SKB_VW_PU_DEFAULT_B 	70

#define SKB_VW_PD_DEFAULT_L 	630
#define SKB_VW_PD_DEFAULT_T 	10
#define SKB_VW_PD_DEFAULT_R 	680
#define SKB_VW_PD_DEFAULT_B 	70

#define SKB_SW_DEFAULT_L 	200
#define SKB_SW_DEFAULT_T 	90
#define SKB_SW_DEFAULT_R 	600
#define SKB_SW_DEFAULT_B 	140

#define SKB_KW_DEFAULT_L 	0
#define SKB_KW_DEFAULT_T 	181
#define SKB_KW_DEFAULT_R 	800
#define SKB_KW_DEFAULT_B 	580

#define KEY_ROW1_TY	141
#define KEY_ROW1_BY	233
#define KEY_ROW2_TY	256
#define KEY_ROW2_BY	348
#define KEY_ROW3_TY	371
#define KEY_ROW3_BY	463
#define KEY_ROW4_TY	482
#define KEY_ROW4_BY	574

int init_en_keyboard (const void* handle,HWND hWnd, key_board_t *kb);
void destroy_en_keyboard (key_board_t *kb);
int init_py_keyboard (const void* handle,HWND hWnd, key_board_t *kb);
void destroy_py_keyboard (key_board_t *kb);
int init_num_keyboard (const void* handle,HWND hWnd, key_board_t *kb);
void destroy_num_keyboard (key_board_t *kb);
int init_punct_keyboard (const void* handle,HWND hWnd, key_board_t *kb);
void destroy_punct_keyboard (key_board_t *kb);
int init_korean_keyboard (const void* handle,HWND hWnd, key_board_t *kb);
void destroy_korean_keyboard (key_board_t *kb);

#endif
