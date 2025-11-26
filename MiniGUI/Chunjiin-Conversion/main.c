/*
 * Chunjiin Korean Input Method - MiniGUI Version
 * Main application entry point
 */

#include <stdio.h>
#include <stdlib.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "input/chunjiin.h"
#include "input/input.h"
#include "gui/gui.h"

int MiniGUIMain(int argc, const char* argv[]) {
    (void)argc;
    (void)argv;

    MSG Msg;
    HWND hMainWnd;

    // Initialize input state
    ChunjiinState chunjiin_state;
    input_init(&chunjiin_state);

    // Create main window
    hMainWnd = gui_create_main_window(&chunjiin_state);

    if (hMainWnd == HWND_INVALID) {
        return -1;
    }

    // Main message loop
    while (GetMessage(&Msg, hMainWnd)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
    }

    MainWindowThreadCleanup(hMainWnd);
    return 0;
}

#ifdef _MGRM_THREADS
#include <minigui/dti.c>
#endif
