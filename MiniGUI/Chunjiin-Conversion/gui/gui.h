#ifndef GUI_H
#define GUI_H

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/window.h>

#include "../input/chunjiin.h"

// GUI initialization
int gui_init_fonts(void);
void gui_cleanup_fonts(void);

// Create and manage main window
HWND gui_create_main_window(ChunjiinState *state);

// Update display functions
void gui_update_display(ChunjiinState *state);
void gui_update_button_labels(ChunjiinState *state);

// Event handlers
void gui_handle_key_press(ChunjiinState *state, int key);
void gui_handle_mode_change(ChunjiinState *state);
void gui_handle_punctuation(ChunjiinState *state);
void gui_handle_enter(ChunjiinState *state, HWND hWnd);
void gui_handle_save(ChunjiinState *state, HWND hWnd);
void gui_handle_load(ChunjiinState *state, HWND hWnd);
void gui_handle_save_dialog(ChunjiinState *state, HWND hWnd);
void gui_handle_load_dialog(ChunjiinState *state, HWND hWnd);

#endif // GUI_H
