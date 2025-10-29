/**
 * FileName: resource_keypad.c
 *
 * Copyright (C) 2014-2015, ZKSoftware Inc.
 *
 * Created: 2014-8-5
 * Author: wilsn
 *
 * Description:
 */

/*
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#include "softkeypad.h"

#ifdef SOFTKBD_320_240
#include "320-240/en_kbd_bkg.c"
#include "320-240/num_kbd_bkg.c"
#include "320-240/punct_kbd_bkg.c"
#include "320-240/pinyin_kbd_bkg.c"
#include "320-240/char_key_mask.c"
#include "320-240/func_key_mask.c"
#include "320-240/left_arrow_enable.c"
#include "320-240/left_arrow_disable.c"
#include "320-240/right_arrow_enable.c"
#include "320-240/right_arrow_disable.c"
#ifdef KBD_TOOLTIP
#include "320-240/tooltip_bk.c"
#include "320-240/tooltip_mask.c"
#include "320-240/bitmapfont.c"
#endif
#elif defined (SOFTKBD_480_272)
#include "480-272/en_kbd_bkg.c"
#include "480-272/num_kbd_bkg.c"
#include "480-272/punct_kbd_bkg.c"
#include "480-272/pinyin_kbd_bkg.c"
#include "480-272/char_key_mask.c"
#include "480-272/func_key_mask.c"
#include "480-272/left_arrow_enable.c"
#include "480-272/left_arrow_disable.c"
#include "480-272/right_arrow_enable.c"
#include "480-272/right_arrow_disable.c"
#ifdef KBD_TOOLTIP
#include "480-272/tooltip_bk.c"
#include "480-272/tooltip_mask.c"
#include "480-272/bitmapfont.c"
#endif
#elif defined (SOFTKBD_800_480)
#include "800-480/en_kbd_bkg.c"
#include "800-480/num_kbd_bkg.c"
#include "800-480/punct_kbd_bkg.c"
#include "800-480/pinyin_kbd_bkg.c"
#include "800-480/char_key_mask.c"
#include "800-480/func_key_mask.c"
#include "800-480/left_arrow_enable.c"
#include "800-480/left_arrow_disable.c"
#include "800-480/right_arrow_enable.c"
#include "800-480/right_arrow_disable.c"
#ifdef KBD_TOOLTIP
#include "800-480/tooltip_bk.c"
#include "800-480/tooltip_mask.c"
#include "800-480/bitmapfont.c"
#endif

#elif defined (SOFTKBD_240_320)
#include "240-320/en_kbd_bkg.c"
#include "240-320/num_kbd_bkg.c"
#include "240-320/punct_kbd_bkg.c"
#include "240-320/pinyin_kbd_bkg.c"
#include "240-320/char_key_mask.c"
#include "240-320/func_key_mask.c"
#include "240-320/left_arrow_enable.c"
#include "240-320/left_arrow_disable.c"
#include "240-320/right_arrow_enable.c"
#include "240-320/right_arrow_disable.c"
#ifdef KBD_TOOLTIP
#include "240-320/tooltip_bk.c"
#include "240-320/tooltip_mask.c"
#include "240-320/bitmapfont.c"
#endif
#endif 


typedef struct _RESDATA
{
    const char* file;
    unsigned char* data;
    unsigned int data_size;

} RESDATA;

static BITMAP punct_kpd_bmp[4];
static BITMAP res_bmp[20];

static const char* get_file_suffix (const char *file)
{
    const char* ext;

    if (file == NULL)
        return NULL;

    ext = strrchr (file, '.');
    if (ext)
        return ext + 1;

    return NULL;
}



int get_kpd_bitmap (HDC hdc, PBITMAP* pbmp, int id)
{

    if (id  < 0 || id > TABLESIZE(res_bmp)) 
        return -1;
	if(id < CHAR_KEY_MASK)
	{
		*pbmp = &res_bmp[id];
	}
	else
	{
		//memcpy(* pbmp,&res_bmp[id],sizeof(BITMAP));
	}

    return 0;
}

static char *GetPublicBmpPath(const char *fileName)
{
	static char publicBmpPathBuf[160] = {0};
	char envPathBuf[128] = {0};
	
#ifdef SOFTKBD_320_240 
		char *bmpFolderName = "commonres/softkeyboard/320x240";
#elif defined (SOFTKBD_480_272)   
		char *bmpFolderName = "commonres/softkeyboard/480x272";
#elif defined (SOFTKBD_240_320)   
		char *bmpFolderName = "commonres/softkeyboard/240x320";
#elif defined (SOFTKBD_800_480)   
		char *bmpFolderName = "commonres/softkeyboard/800x480";
#endif

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

int load_kpd_bitmap(void)
{
	int err = 0;
	
	if(LoadBitmap(HDC_SCREEN, &res_bmp[EN9_KBD_BKG], GetPublicBmpPath("en9_kbd_bkg.png")))
	{
		err++;
	}
	if(LoadBitmap(HDC_SCREEN, &res_bmp[NUM9_KBD_BKG], GetPublicBmpPath("num9_kbd_bkg.png")))
	{
		err++;
	}	
	if(LoadBitmap(HDC_SCREEN, &res_bmp[PINYIN9_KBD_BKG], GetPublicBmpPath("pinyin9_kbd_bkg.png")))
	{
		err++;
	}
	if(LoadBitmap(HDC_SCREEN, &res_bmp[PUNCT9_KBD_BKG0], GetPublicBmpPath("punct9_kbd_bkg0.png")))
	{
		err++;
	}
	if(LoadBitmap(HDC_SCREEN, &res_bmp[PUNCT9_KBD_BKG1], GetPublicBmpPath("punct9_kbd_bkg1.png")))
	{
		err++; 
	}
	if(LoadBitmap(HDC_SCREEN, &res_bmp[PUNCT9_KBD_BKG2], GetPublicBmpPath("punct9_kbd_bkg2.png")))
	{
		err++;
	}
	if(LoadBitmap(HDC_SCREEN, &res_bmp[PUNCT9_KBD_BKG3], GetPublicBmpPath("punct9_kbd_bkg3.png")))
	{
		err++;
	}

	if(LoadBitmap (HDC_SCREEN, &res_bmp[CHAR_KEY_MASK], GetPublicBmpPath("char_key_mask.png")))
	{
		err++;
		printf("Fail to load char_key_mask bitmap.=%d\n");
	}
	if(LoadBitmap (HDC_SCREEN, &res_bmp[FUNC_KEY_MASK], GetPublicBmpPath("func_key_mask.png")))
	{
		err++;
		printf("Fail to load func_key_mask bitmap.=%d\n");
	}																		 
	if(LoadBitmap (HDC_SCREEN, &res_bmp[LEFT_ARROW_ENABLE], GetPublicBmpPath("left_arrow_enable.png")))
	{
		err++;
		printf("Fail to load left_arrow_enbale bitmap.=%d\n");
	}
	if(LoadBitmap (HDC_SCREEN, &res_bmp[LEFT_ARROW_DISABLE], GetPublicBmpPath("left_arrow_disable.png")))
	{
		err++;
		printf("Fail to load left_arrow_disable bitmap.=%d\n");
	}
	if(LoadBitmap (HDC_SCREEN, &res_bmp[RIGHT_ARROW_ENABLE], GetPublicBmpPath("right_arrow_enable.png")))
	{
		err++;
		printf("Fail to load right_arrow_enable bitmap.=%d\n");
	}
	if(LoadBitmap (HDC_SCREEN, &res_bmp[RIGHT_ARROW_DISABLE], GetPublicBmpPath("right_arrow_disable.png")))
	{
		err++;
		printf("Fail to load right_arrow_disable bitmap.=%d\n");
	}
	if(LoadBitmap (HDC_SCREEN, &res_bmp[TTW_BK_BMP], GetPublicBmpPath("tooltip_bk.png")))
	{
		err++;
		printf("Fail to load tooltip_bk bitmap.=%d\n");
	}
	if(LoadBitmap (HDC_SCREEN, &res_bmp[TTW_MASK_BMP], GetPublicBmpPath("tooltip_bk.png")))
	{
		err++;
		//printf("Fail to load*** tooltip_mask ***bitmap.=%s\n",GetPublicBmpPath("tooltip_mask.png"));
	}	
	if(LoadBitmap (HDC_SCREEN, &res_bmp[TTW_FONT_BMP], GetPublicBmpPath("bitmap_font.png")))
	{
		err++;
		printf("Fail to load bitmap_font bitmap.=%d\n");
	}
	
	if(err)
	{
		printf("Fail to load softkeyboard bitmap.=%d\n",err);
	}

	return err;
}


void release_kpd_bitmap (PBITMAP pbmp)
{
    UnloadBitmap (pbmp);

    return;
}
*/


