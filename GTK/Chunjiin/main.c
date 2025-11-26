/*
 * Chunjiin Korean Input Method - Main Entry Point
 */

#include <gtk/gtk.h>
#include <locale.h>
#include "gui.h"

// Activate callback for GTK application
static void activate(GtkApplication *app, gpointer user_data __attribute__((unused))) {
    create_main_window(app);
}

int main(int argc, char **argv) {
    // Set locale
    setlocale(LC_ALL, "");

    GtkApplication *app = gtk_application_new("com.personal.chunjiin", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
