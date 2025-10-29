/*
**
** softkeypad.h: soft keyphone head file. 
**
** Author: wilsn
**
** Create date: 2014-8-5
**  Description:
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
#include "properties.h"

#define _MY_PRINTF(fmt...) fprintf (stderr, fmt)

#define PUNCT_KBD_NUM 4
#define __FILL_DIRECT__			1

#define SCREEN_DEFAULT_W 	800
#define SCREEN_DEFAULT_H 	1280

#define SKB_WIN_DEFAULT_W	  	800
#define SKB_WIN_DEFAULT_H  		580

#define SKB_VW_DEFAULT_L 	30
#define SKB_VW_DEFAULT_T 	0
#define SKB_VW_DEFAULT_R 	740
#define SKB_VW_DEFAULT_B 	95

#define SKB_VW_PU_DEFAULT_L 	30
#define SKB_VW_PU_DEFAULT_T 	0
#define SKB_VW_PU_DEFAULT_R 	65
#define SKB_VW_PU_DEFAULT_B 	89

#define SKB_VW_PD_DEFAULT_L 	705
#define SKB_VW_PD_DEFAULT_T 	2
#define SKB_VW_PD_DEFAULT_R 	740
#define SKB_VW_PD_DEFAULT_B 	89

#define SKB_SW_DEFAULT_L 	101
#define SKB_SW_DEFAULT_T 	42
#define SKB_SW_DEFAULT_R 	740
#define SKB_SW_DEFAULT_B 	156

#define SKB_SELECT_DEFAULT_L 	4
#define SKB_SELECT_DEFAULT_T 	41
#define SKB_SELECT_DEFAULT_R 	750
#define SKB_SELECT_DEFAULT_B 	151

#define SKB_KW_DEFAULT_L 	0
#define SKB_KW_DEFAULT_T 	62
#define SKB_KW_DEFAULT_R 	740
#define SKB_KW_DEFAULT_B 	580

int init_en_keypad (const void *handle,HWND hWnd, key_board_t *kb);
void destroy_en_keypad (key_board_t *kb);
int init_py_keypad (const void *handle,HWND hWnd, key_board_t *kb);
void destroy_py_keypad(key_board_t *kb);
int init_num_keypad(const void *handle,HWND hWnd, key_board_t *kb);
void destroy_num_keypad (key_board_t *kb);
int init_punct_keypad (const void *handle,HWND hWnd, key_board_t *kb);
void destroy_punct_keypad (key_board_t *kb);


#endif
