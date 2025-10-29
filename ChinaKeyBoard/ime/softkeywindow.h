#ifndef __SOFTKEYWINDOW_H__
#define __SOFTKEYWINDOW_H__

#define  MSG_IME_SHOW           MSG_USER+799
#define  MSG_IME_HIDE           MSG_USER+800
#define  MSG_IME_CHAR_END       MSG_USER+801
#define  MSG_IME_LANGUAGE       MSG_USER+803



HWND CreateSoftKeypad (int sfkbType,void (*cb)(BOOL SoftKeyWindowIsShown));
void HideSoftKeyWindow(HWND hWnd);
#endif
