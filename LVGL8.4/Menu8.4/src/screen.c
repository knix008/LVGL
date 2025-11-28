#include "../include/screen.h"
#include "../include/config.h"
#include "../include/style.h"
#include "../include/menu.h"

// ============================================================================
// SCREEN MANAGEMENT
// ============================================================================

void show_screen(int screen_id) {
    for (int i = 0; i <= screen_stack_top; i++) {
        if (screen_stack[i].screen_id == screen_id) {
            screen_stack_top = i;
            lv_scr_load(screen_stack[i].screen);
            return;
        }
    }

    if (screen_id == SCREEN_MENU && (screen_stack_top < 0 || screen_stack[screen_stack_top].screen_id != SCREEN_MENU)) {
        create_menu_screen();
    }
}
