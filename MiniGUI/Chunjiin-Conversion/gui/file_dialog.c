/*
 * File Dialog Module for Chunjiin Korean Input Method
 * Follows the pattern from Qwerty-Conversion
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include <minigui/common.h>
#include <minigui/minigui.h>
#include <minigui/gdi.h>
#include <minigui/window.h>
#include <minigui/control.h>

#include "file_dialog.h"
#include "../input/file_io.h"

#define SAVE_DIR "/tmp/chunjiin_input"
#define MAX_FILENAME 256
#define DIR_PERMISSIONS 0755
#define FILE_EXTENSION ".txt"
#define MSG_BUFFER_SIZE 768
#define FILEPATH_BUFFER_SIZE 512

// Dialog dimensions and positions
#define DIALOG_X 100
#define DIALOG_Y 100
#define DIALOG_SAVE_WIDTH 300
#define DIALOG_SAVE_HEIGHT 140
#define DIALOG_LOAD_WIDTH 300
#define DIALOG_LOAD_HEIGHT 240

// Control IDs
#define CTRL_ID_LABEL 1000
#define CTRL_ID_EDIT 1001
#define CTRL_ID_LISTBOX 1002

// Control dimensions and positions for Save dialog
#define SAVE_LABEL_X 10
#define SAVE_LABEL_Y 15
#define SAVE_LABEL_W 280
#define SAVE_LABEL_H 20
#define SAVE_EDIT_X 10
#define SAVE_EDIT_Y 38
#define SAVE_EDIT_W 280
#define SAVE_EDIT_H 25
#define SAVE_BTN_Y 80
#define SAVE_OK_X 80
#define SAVE_CANCEL_X 160
#define SAVE_BTN_W 60
#define SAVE_BTN_H 25

// Control dimensions and positions for Load dialog
#define LOAD_LABEL_X 10
#define LOAD_LABEL_Y 15
#define LOAD_LABEL_W 280
#define LOAD_LABEL_H 20
#define LOAD_LISTBOX_X 10
#define LOAD_LISTBOX_Y 38
#define LOAD_LISTBOX_W 280
#define LOAD_LISTBOX_H 120
#define LOAD_BTN_Y 170
#define LOAD_OK_X 80
#define LOAD_CANCEL_X 160
#define LOAD_BTN_W 60
#define LOAD_BTN_H 25

// Misc constants
#define NUM_CONTROLS 4
#define FIRST_ITEM 0

// Helper function to ensure save directory exists
static void ensure_save_dir(void) {
    struct stat st = {0};
    if (stat(SAVE_DIR, &st) == -1) {
        if (mkdir(SAVE_DIR, DIR_PERMISSIONS) == 0) {
            printf("Created directory: %s\n", SAVE_DIR);
        }
    }
}

// Helper function to list all saved files
static char** get_saved_files(int *count) {
    DIR *dir;
    struct dirent *entry;
    char **files = NULL;
    int file_count = 0;

    ensure_save_dir();

    dir = opendir(SAVE_DIR);
    if (!dir) {
        printf("Error: Cannot open directory %s\n", SAVE_DIR);
        *count = 0;
        return NULL;
    }

    // Count files first
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG && strstr(entry->d_name, FILE_EXTENSION) != NULL) {
            printf("Found file: %s\n", entry->d_name);
            file_count++;
        }
    }

    printf("Total files found: %d\n", file_count);

    if (file_count == 0) {
        closedir(dir);
        *count = 0;
        return NULL;
    }

    // Allocate and fill array
    files = (char**)malloc(file_count * sizeof(char*));
    rewinddir(dir);

    file_count = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG && strstr(entry->d_name, FILE_EXTENSION) != NULL) {
            files[file_count] = (char*)malloc(strlen(entry->d_name) + 1);
            strcpy(files[file_count], entry->d_name);
            file_count++;
        }
    }

    closedir(dir);
    *count = file_count;
    return files;
}

// Helper function to free file list
static void free_file_list(char **files, int count) {
    if (files) {
        for (int i = 0; i < count; i++) {
            free(files[i]);
        }
        free(files);
    }
}

// Global for passing filename between dialog and procedure
static char g_selected_filename[MAX_FILENAME] = {0};

// Dialog procedure for saving
static LRESULT SaveDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hInput = HWND_INVALID;

    switch (message) {
        case MSG_INITDIALOG: {
            hInput = GetDlgItem(hDlg, CTRL_ID_EDIT);
            SetFocus(hInput);
            return TRUE;
        }
        case MSG_COMMAND:
            if (wParam == IDOK) {
                char filename[MAX_FILENAME] = {0};
                GetWindowText(hInput, filename, sizeof(filename) - 1);

                if (strlen(filename) == 0) {
                    MessageBox(hDlg, "Please enter a filename", "Error", MB_OK | MB_ICONEXCLAMATION);
                    return TRUE;
                }

                // Ensure .txt extension
                if (!strstr(filename, FILE_EXTENSION)) {
                    strcat(filename, FILE_EXTENSION);
                }

                strcpy(g_selected_filename, filename);
                printf("Save filename: %s\n", g_selected_filename);
                EndDialog(hDlg, IDOK);
            } else if (wParam == IDCANCEL) {
                EndDialog(hDlg, IDCANCEL);
            }
            return TRUE;
    }
    return DefaultDialogProc(hDlg, message, wParam, lParam);
}

// Dialog procedure for loading
static LRESULT LoadDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hListBox = HWND_INVALID;
    static char **files = NULL;
    static int file_count = 0;

    switch (message) {
        case MSG_INITDIALOG: {
            hListBox = GetDlgItem(hDlg, CTRL_ID_LISTBOX);

            // Get list of files
            files = get_saved_files(&file_count);

            if (file_count == 0) {
                SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)"No saved files");
                EnableWindow(GetDlgItem(hDlg, IDOK), FALSE);
                return TRUE;
            }

            // Add files to listbox
            for (int i = 0; i < file_count; i++) {
                printf("Adding file to listbox: %s\n", files[i]);
                SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)files[i]);
            }

            // Select first item
            SendMessage(hListBox, LB_SETCURSEL, FIRST_ITEM, 0);
            SetFocus(hListBox);
            return TRUE;
        }
        case MSG_COMMAND:
            if (wParam == IDOK) {
                int selected = SendMessage(hListBox, LB_GETCURSEL, 0, 0);
                if (selected >= 0 && selected < file_count && files != NULL) {
                    strcpy(g_selected_filename, files[selected]);
                    printf("Load filename: %s\n", g_selected_filename);
                    free_file_list(files, file_count);
                    EndDialog(hDlg, IDOK);
                } else {
                    free_file_list(files, file_count);
                    EndDialog(hDlg, IDCANCEL);
                }
            } else if (wParam == IDCANCEL) {
                free_file_list(files, file_count);
                EndDialog(hDlg, IDCANCEL);
            }
            return TRUE;
    }
    return DefaultDialogProc(hDlg, message, wParam, lParam);
}

// Open save dialog
int gui_dialog_save(ChunjiinState *state, HWND hParent) {
    if (!state) return 0;

    char filepath[FILEPATH_BUFFER_SIZE];
    memset(g_selected_filename, 0, sizeof(g_selected_filename));

    DLGTEMPLATE dlg_template = {
        WS_BORDER | WS_CAPTION,
        WS_EX_NONE,
        DIALOG_X, DIALOG_Y, DIALOG_SAVE_WIDTH, DIALOG_SAVE_HEIGHT,
        "Save Input",
        0, 0,
        NUM_CONTROLS,
        NULL,
        0
    };

    CTRLDATA CtrlData[] = {
        {
            CTRL_STATIC,
            WS_VISIBLE | SS_LEFT,
            SAVE_LABEL_X, SAVE_LABEL_Y, SAVE_LABEL_W, SAVE_LABEL_H,
            CTRL_ID_LABEL,
            "Enter filename (without .txt):",
            0,
            WS_EX_NONE
        },
        {
            CTRL_EDIT,
            WS_VISIBLE | WS_TABSTOP | WS_BORDER,
            SAVE_EDIT_X, SAVE_EDIT_Y, SAVE_EDIT_W, SAVE_EDIT_H,
            CTRL_ID_EDIT,
            "",
            0,
            WS_EX_NONE
        },
        {
            CTRL_BUTTON,
            WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
            SAVE_OK_X, SAVE_BTN_Y, SAVE_BTN_W, SAVE_BTN_H,
            IDOK,
            "OK",
            0,
            WS_EX_NONE
        },
        {
            CTRL_BUTTON,
            WS_VISIBLE | WS_TABSTOP,
            SAVE_CANCEL_X, SAVE_BTN_Y, SAVE_BTN_W, SAVE_BTN_H,
            IDCANCEL,
            "Cancel",
            0,
            WS_EX_NONE
        }
    };

    dlg_template.controlnr = NUM_CONTROLS;
    dlg_template.controls = CtrlData;

    int ret = DialogBoxIndirectParam(&dlg_template, hParent, SaveDialogProc, 0);

    if (ret == IDOK && strlen(g_selected_filename) > 0) {
        ensure_save_dir();
        snprintf(filepath, sizeof(filepath), "%s/%s", SAVE_DIR, g_selected_filename);

        printf("Saving to: %s\n", filepath);

        if (file_save_input(state, filepath)) {
            char msg[MSG_BUFFER_SIZE];
            snprintf(msg, sizeof(msg), "Saved to:\n%s", filepath);
            MessageBox(hParent, msg, "Save Complete", MB_OK | MB_ICONINFORMATION);
            return 1;
        } else {
            MessageBox(hParent, "Save failed", "Error", MB_OK | MB_ICONEXCLAMATION);
            return 0;
        }
    }

    return 0;
}

// Open load dialog
int gui_dialog_load(ChunjiinState *state, HWND hParent) {
    if (!state) return 0;

    char filepath[FILEPATH_BUFFER_SIZE];
    memset(g_selected_filename, 0, sizeof(g_selected_filename));

    DLGTEMPLATE dlg_template = {
        WS_BORDER | WS_CAPTION,
        WS_EX_NONE,
        DIALOG_X, DIALOG_Y, DIALOG_LOAD_WIDTH, DIALOG_LOAD_HEIGHT,
        "Load Input",
        0, 0,
        NUM_CONTROLS,
        NULL,
        0
    };

    CTRLDATA CtrlData[] = {
        {
            CTRL_STATIC,
            WS_VISIBLE | SS_LEFT,
            LOAD_LABEL_X, LOAD_LABEL_Y, LOAD_LABEL_W, LOAD_LABEL_H,
            CTRL_ID_LABEL,
            "Select a file to load:",
            0,
            WS_EX_NONE
        },
        {
            CTRL_LISTBOX,
            WS_VISIBLE | WS_TABSTOP | WS_BORDER,
            LOAD_LISTBOX_X, LOAD_LISTBOX_Y, LOAD_LISTBOX_W, LOAD_LISTBOX_H,
            CTRL_ID_LISTBOX,
            "",
            0,
            WS_EX_NONE
        },
        {
            CTRL_BUTTON,
            WS_VISIBLE | WS_TABSTOP | BS_DEFPUSHBUTTON,
            LOAD_OK_X, LOAD_BTN_Y, LOAD_BTN_W, LOAD_BTN_H,
            IDOK,
            "OK",
            0,
            WS_EX_NONE
        },
        {
            CTRL_BUTTON,
            WS_VISIBLE | WS_TABSTOP,
            LOAD_CANCEL_X, LOAD_BTN_Y, LOAD_BTN_W, LOAD_BTN_H,
            IDCANCEL,
            "Cancel",
            0,
            WS_EX_NONE
        }
    };

    dlg_template.controlnr = NUM_CONTROLS;
    dlg_template.controls = CtrlData;

    int ret = DialogBoxIndirectParam(&dlg_template, hParent, LoadDialogProc, 0);

    if (ret == IDOK && strlen(g_selected_filename) > 0) {
        snprintf(filepath, sizeof(filepath), "%s/%s", SAVE_DIR, g_selected_filename);

        printf("Loading from: %s\n", filepath);

        if (file_load_input(state, filepath)) {
            char msg[MSG_BUFFER_SIZE];
            snprintf(msg, sizeof(msg), "Loaded from:\n%s", filepath);
            MessageBox(hParent, msg, "Load Complete", MB_OK | MB_ICONINFORMATION);
            return 1;
        } else {
            MessageBox(hParent, "Load failed", "Error", MB_OK | MB_ICONEXCLAMATION);
            return 0;
        }
    }

    return 0;
}
