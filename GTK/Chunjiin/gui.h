#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>
#include "chunjiin.h"

typedef struct {
    GtkWidget *window;
    GtkWidget *text_view;
    GtkTextBuffer *text_buffer;
    GtkWidget *buttons[12];
    GtkWidget *mode_button;
    ChunjiinState state;
} AppWidgets;

// GUI 초기화 및 실행
void create_main_window(GtkApplication *app);

#endif // GUI_H
