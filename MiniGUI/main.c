#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>

#define IDC_BUTTON1    101
#define IDC_BUTTON2    102
#define IDC_EDIT       103

static HWND hMainWnd;
static HWND hButton1, hButton2, hEdit;

static LRESULT HelloWinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    
    switch (message) {
        case MSG_CREATE:
            // Create buttons
            hButton1 = CreateWindow("button", "Click Me!",
                WS_VISIBLE | WS_CHILD,
                IDC_BUTTON1,
                50, 50, 100, 30,
                hWnd, 0);
            
            hButton2 = CreateWindow("button", "Exit",
                WS_VISIBLE | WS_CHILD,
                IDC_BUTTON2,
                200, 50, 100, 30,
                hWnd, 0);
            
            // Create edit control
            hEdit = CreateWindow("edit", "Hello MiniGUI!",
                WS_VISIBLE | WS_CHILD | WS_BORDER,
                IDC_EDIT,
                50, 100, 250, 30,
                hWnd, 0);
            
            return 0;
            
        case MSG_COMMAND:
            switch (LOWORD(wParam)) {
                case IDC_BUTTON1:
                    MessageBox(hWnd, "Button 1 clicked!", "Message", MB_OK | MB_ICONINFORMATION);
                    break;
                    
                case IDC_BUTTON2:
                    PostMessage(hWnd, MSG_CLOSE, 0, 0);
                    break;
            }
            return 0;
            
        case MSG_PAINT:
            hdc = BeginPaint(hWnd);
            SetBkMode(hdc, BM_TRANSPARENT);
            TextOut(hdc, 50, 150, "Welcome to MiniGUI Application!");
            EndPaint(hWnd, hdc);
            return 0;
            
        case MSG_CLOSE:
            DestroyMainWindow(hWnd);
            PostQuitMessage(hWnd);
            return 0;
            
        case MSG_DESTROY:
            return 0;
    }
    
    return DefaultMainWinProc(hWnd, message, wParam, lParam);
}

static void InitCreateInfo(MAINWINCREATE* pCreateInfo)
{
    pCreateInfo->dwStyle = WS_VISIBLE | WS_BORDER | WS_CAPTION;
    pCreateInfo->dwExStyle = WS_EX_NONE;
    pCreateInfo->spCaption = "MiniGUI Hello World";
    pCreateInfo->hMenu = 0;
    pCreateInfo->hCursor = GetSystemCursor(0);
    pCreateInfo->hIcon = 0;
    pCreateInfo->MainWindowProc = HelloWinProc;
    pCreateInfo->lx = 0;
    pCreateInfo->ty = 0;
    pCreateInfo->rx = 400;
    pCreateInfo->by = 300;
    pCreateInfo->iBkColor = COLOR_lightwhite;
    pCreateInfo->dwAddData = 0;
    pCreateInfo->hHosting = HWND_DESKTOP;
}

int MiniGUIMain(int argc, const char* argv[])
{
    MAINWINCREATE CreateInfo;
    MSG msg;
    
    // JoinLayer removed for standalone mode
    
    InitCreateInfo(&CreateInfo);
    hMainWnd = CreateMainWindow(&CreateInfo);
    
    if (hMainWnd == HWND_INVALID)
        return -1;
    
    ShowWindow(hMainWnd, SW_SHOWNORMAL);
    
    while (GetMessage(&msg, hMainWnd)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    
    // MainWindowThreadCleanup removed for compatibility
    return 0;
}

#ifndef _MGRM_PROCESSES
#include <minigui/dti.c>
#endif
