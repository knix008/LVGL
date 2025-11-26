#ifndef FILE_DIALOG_H
#define FILE_DIALOG_H

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/window.h>

#include "../input/chunjiin.h"

// Open save dialog and save file with user-specified filename
int gui_dialog_save(ChunjiinState *state, HWND hParent);

// Open load dialog and load file with user-specified filename
int gui_dialog_load(ChunjiinState *state, HWND hParent);

#endif // FILE_DIALOG_H
