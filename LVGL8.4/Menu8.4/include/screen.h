#ifndef SCREEN_H
#define SCREEN_H

#include "types.h"

// ============================================================================
// SCREEN MANAGEMENT API
// ============================================================================

extern AppState app_state;
extern ScreenState screen_stack[];
extern int screen_stack_top;

void show_screen(int screen_id);
void create_menu_screen(void);

#endif
