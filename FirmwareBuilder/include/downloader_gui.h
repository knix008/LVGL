/*
 * downloader_gui.h - GTK GUI for Firmware Downloader
 *
 * Provides GTK widgets and functions for downloading, verifying,
 * decrypting, decompressing, and validating firmware images.
 */

#ifndef DOWNLOADER_GUI_H
#define DOWNLOADER_GUI_H

#include <gtk/gtk.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Create and return a downloader GUI tab for a notebook widget
 *
 * @param notebook The GtkNotebook to add the tab to
 * @param window The main application window (for dialogs)
 */
void create_downloader_tab(GtkWidget *notebook, GtkWidget *window);

/**
 * Initialize the downloader GUI (called during application startup)
 */
void downloader_gui_init(void);

/**
 * Cleanup the downloader GUI (called during application shutdown)
 */
void downloader_gui_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif /* DOWNLOADER_GUI_H */
