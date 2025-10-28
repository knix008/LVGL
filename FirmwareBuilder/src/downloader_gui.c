/*
 * downloader_gui.c - GTK GUI for Firmware Downloader
 *
 * Provides a complete graphical interface for downloading, verifying,
 * decrypting, decompressing, and validating firmware images.
 */

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "firmware_downloader.h"
#include "downloader_gui.h"

/* ============================================================================
 * Data Structures
 * ============================================================================ */

/**
 * GUI context for downloader tab
 */
typedef struct {
    GtkWidget *window;

    /* File selection widgets */
    GtkWidget *source_entry;
    GtkWidget *output_entry;

    /* Configuration widgets */
    GtkWidget *encrypted_check;
    GtkWidget *password_entry;
    GtkWidget *compressed_check;
    GtkWidget *hash_algo_combo;
    GtkWidget *expected_hash_entry;
    GtkWidget *expected_crc_entry;

    /* Progress and status widgets */
    GtkWidget *progress_bar;
    GtkWidget *status_label;
    GtkWidget *output_text;
    GtkWidget *download_button;
    GtkWidget *cancel_button;
    GtkTextBuffer *text_buffer;

    /* Download state */
    int is_downloading;
    firmware_downloader_t *downloader;
    pthread_t download_thread;
} DownloaderGUI;

static DownloaderGUI gui = {0};

/* ============================================================================
 * Helper Functions
 * ============================================================================ */

/**
 * Append text to output display
 */
static void append_output(const char *text)
{
    if (!gui.text_buffer) return;

    GtkTextIter end;
    gtk_text_buffer_get_end_iter(gui.text_buffer, &end);
    gtk_text_buffer_insert(gui.text_buffer, &end, text, -1);
    gtk_text_buffer_insert(gui.text_buffer, &end, "\n", -1);

    GtkTextView *view = GTK_TEXT_VIEW(gui.output_text);
    gtk_text_view_scroll_to_iter(view, &end, 0.0, FALSE, 0.0, 0.0);
}

/**
 * Update status label
 */
static void update_status(const char *status)
{
    if (gui.status_label) {
        gtk_label_set_text(GTK_LABEL(gui.status_label), status);
    }
}

/**
 * Update progress bar
 */
static void update_progress(double fraction)
{
    if (gui.progress_bar) {
        if (fraction >= 0.0 && fraction <= 1.0) {
            gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(gui.progress_bar), fraction);
        }
    }
}

/**
 * Enable/disable download controls
 */
static void set_controls_enabled(int enabled)
{
    if (gui.source_entry) gtk_widget_set_sensitive(gui.source_entry, enabled);
    if (gui.output_entry) gtk_widget_set_sensitive(gui.output_entry, enabled);
    if (gui.encrypted_check) gtk_widget_set_sensitive(gui.encrypted_check, enabled);
    if (gui.password_entry) gtk_widget_set_sensitive(gui.password_entry, enabled);
    if (gui.compressed_check) gtk_widget_set_sensitive(gui.compressed_check, enabled);
    if (gui.hash_algo_combo) gtk_widget_set_sensitive(gui.hash_algo_combo, enabled);
    if (gui.expected_hash_entry) gtk_widget_set_sensitive(gui.expected_hash_entry, enabled);
    if (gui.expected_crc_entry) gtk_widget_set_sensitive(gui.expected_crc_entry, enabled);
    if (gui.download_button) gtk_widget_set_sensitive(gui.download_button, enabled);
}

/**
 * Idle callback for updating progress (thread-safe)
 */
typedef struct {
    double fraction;
} ProgressData;

static gboolean update_progress_idle(gpointer data)
{
    ProgressData *pdata = (ProgressData*)data;
    update_progress(pdata->fraction);
    g_free(pdata);
    return FALSE;
}

static gboolean append_output_idle(gpointer data)
{
    append_output((const char*)data);
    g_free(data);
    return FALSE;
}

/**
 * Progress callback for download
 */
static int download_progress_cb(size_t bytes_downloaded, size_t total_bytes, void *user_data)
{
    (void)user_data;

    if (total_bytes > 0) {
        double fraction = (double)bytes_downloaded / total_bytes;
        char status[256];
        snprintf(status, sizeof(status), "Downloaded: %zu / %zu bytes (%.1f%%)",
                 bytes_downloaded, total_bytes, fraction * 100.0);

        ProgressData *pdata = g_malloc(sizeof(ProgressData));
        pdata->fraction = fraction;
        g_idle_add(update_progress_idle, pdata);

        g_idle_add(append_output_idle, g_strdup(status));
    }

    return gui.is_downloading ? 0 : 1;  /* Return 1 to cancel */
}

/**
 * Verification callback
 */
static int verify_callback_cb(download_status_t status, const char *message, void *user_data)
{
    (void)user_data;

    const char *status_str = "Unknown";
    switch (status) {
        case DL_CONNECTING: status_str = "Connecting..."; break;
        case DL_DOWNLOADING: status_str = "Downloading..."; break;
        case DL_VERIFYING: status_str = "Verifying hash..."; break;
        case DL_DECRYPTING: status_str = "Decrypting..."; break;
        case DL_DECOMPRESSING: status_str = "Decompressing..."; break;
        case DL_CRC_CHECK: status_str = "Verifying CRC..."; break;
        case DL_COMPLETE: status_str = "Complete!"; break;
        case DL_ERROR: status_str = "Error"; break;
        default: status_str = "Processing..."; break;
    }

    char output[512];
    snprintf(output, sizeof(output), "[%s] %s", status_str, message);

    g_idle_add(append_output_idle, g_strdup(output));

    return gui.is_downloading ? 0 : 1;
}

/* ============================================================================
 * File Selection Callbacks
 * ============================================================================ */

static void on_browse_source(GtkButton *button, gpointer data)
{
    (void)button;
    (void)data;

    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Select Firmware File",
        GTK_WINDOW(gui.window),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Open", GTK_RESPONSE_ACCEPT,
        NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        gtk_entry_set_text(GTK_ENTRY(gui.source_entry), filename);
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

static void on_browse_output(GtkButton *button, gpointer data)
{
    (void)button;
    (void)data;

    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Save Firmware As",
        GTK_WINDOW(gui.window),
        GTK_FILE_CHOOSER_ACTION_SAVE,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Save", GTK_RESPONSE_ACCEPT,
        NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        gtk_entry_set_text(GTK_ENTRY(gui.output_entry), filename);
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

/**
 * Toggle password entry sensitivity
 */
static void on_encrypted_toggled(GtkToggleButton *button, gpointer data)
{
    (void)data;

    gboolean encrypted = gtk_toggle_button_get_active(button);
    gtk_widget_set_sensitive(gui.password_entry, encrypted);
}

/* ============================================================================
 * Download Thread
 * ============================================================================ */

typedef struct {
    char source_file[512];
    char output_file[512];
    char password[256];
    char expected_hash[256];
    uint32_t expected_crc;
    int is_encrypted;
    int is_compressed;
} ThreadConfig;

/**
 * Download thread function
 */
static void* download_thread_func(void *arg)
{
    ThreadConfig *cfg = (ThreadConfig*)arg;

    append_output("=== Starting Firmware Download ===");

    /* Create downloader if needed */
    if (!gui.downloader) {
        gui.downloader = fw_downloader_create();
        if (!gui.downloader) {
            append_output("Error: Failed to create downloader");
            gui.is_downloading = 0;
            g_free(cfg);
            return NULL;
        }
    }

    /* Run download process */
    int result = fw_downloader_process(
        gui.downloader,
        cfg->source_file,
        cfg->output_file,
        cfg->is_encrypted ? cfg->password : NULL,
        cfg->expected_hash,
        cfg->expected_crc,
        cfg->is_encrypted,
        cfg->is_compressed,
        download_progress_cb,
        verify_callback_cb,
        NULL
    );

    if (result == 0) {
        g_idle_add(append_output_idle, g_strdup("Download completed successfully!"));
        g_idle_add(append_output_idle, g_strdup("=== Download Complete ==="));
    } else {
        const char *error = fw_downloader_get_error(gui.downloader);
        char msg[512];
        snprintf(msg, sizeof(msg), "Download failed: %s",
                 error ? error : "Unknown error");
        g_idle_add(append_output_idle, g_strdup(msg));
    }

    gui.is_downloading = 0;
    set_controls_enabled(1);
    if (gui.cancel_button) gtk_widget_set_sensitive(gui.cancel_button, 0);
    if (gui.download_button) gtk_widget_set_sensitive(gui.download_button, 1);

    g_free(cfg);
    return NULL;
}

/**
 * Download button callback
 */
static void on_download_clicked(GtkButton *button, gpointer data)
{
    (void)button;
    (void)data;

    if (gui.is_downloading) {
        return;
    }

    /* Get configuration from UI */
    const char *source = gtk_entry_get_text(GTK_ENTRY(gui.source_entry));
    const char *output = gtk_entry_get_text(GTK_ENTRY(gui.output_entry));
    const char *password = gtk_entry_get_text(GTK_ENTRY(gui.password_entry));
    const char *hash = gtk_entry_get_text(GTK_ENTRY(gui.expected_hash_entry));
    const char *crc_str = gtk_entry_get_text(GTK_ENTRY(gui.expected_crc_entry));

    gboolean is_encrypted = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gui.encrypted_check));
    gboolean is_compressed = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gui.compressed_check));

    /* Validate inputs */
    if (!source || strlen(source) == 0) {
        append_output("Error: Please select a source firmware file");
        return;
    }

    if (!output || strlen(output) == 0) {
        append_output("Error: Please specify output firmware file");
        return;
    }

    if (!hash || strlen(hash) == 0) {
        append_output("Error: Please enter expected hash");
        return;
    }

    if (is_encrypted && (!password || strlen(password) == 0)) {
        append_output("Error: Please enter decryption password");
        return;
    }

    /* Parse CRC if provided */
    uint32_t expected_crc = 0;
    if (crc_str && strlen(crc_str) > 0) {
        if (sscanf(crc_str, "%u", &expected_crc) != 1) {
            if (sscanf(crc_str, "%x", &expected_crc) != 1) {
                append_output("Error: CRC must be a decimal or hex number");
                return;
            }
        }
    }

    gui.is_downloading = 1;

    /* Clear output */
    gtk_text_buffer_set_text(gui.text_buffer, "", -1);
    update_progress(0.0);
    update_status("Connecting to firmware...");

    /* Disable controls */
    set_controls_enabled(0);
    if (gui.download_button) gtk_widget_set_sensitive(gui.download_button, 0);
    if (gui.cancel_button) gtk_widget_set_sensitive(gui.cancel_button, 1);

    /* Prepare thread configuration */
    ThreadConfig *cfg = g_malloc(sizeof(ThreadConfig));
    strncpy(cfg->source_file, source, sizeof(cfg->source_file) - 1);
    strncpy(cfg->output_file, output, sizeof(cfg->output_file) - 1);
    strncpy(cfg->password, password, sizeof(cfg->password) - 1);
    strncpy(cfg->expected_hash, hash, sizeof(cfg->expected_hash) - 1);
    cfg->expected_crc = expected_crc;
    cfg->is_encrypted = is_encrypted;
    cfg->is_compressed = is_compressed;

    /* Start download thread */
    if (pthread_create(&gui.download_thread, NULL, download_thread_func, cfg) != 0) {
        append_output("Error: Failed to start download thread");
        gui.is_downloading = 0;
        set_controls_enabled(1);
        if (gui.download_button) gtk_widget_set_sensitive(gui.download_button, 1);
        if (gui.cancel_button) gtk_widget_set_sensitive(gui.cancel_button, 0);
        g_free(cfg);
    }
}

/**
 * Cancel button callback
 */
static void on_cancel_clicked(GtkButton *button, gpointer data)
{
    (void)button;
    (void)data;

    gui.is_downloading = 0;
    update_status("Download cancelled");
    append_output("Download cancelled by user");
}

/* ============================================================================
 * Tab Creation
 * ============================================================================ */

void create_downloader_tab(GtkWidget *notebook, GtkWidget *window)
{
    gui.window = window;

    /* Main container */
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(main_box), 10);

    /* === File Selection Section === */
    GtkWidget *file_frame = gtk_frame_new("Firmware File Selection");
    GtkWidget *file_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(file_box), 10);

    /* Source file */
    GtkWidget *source_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *source_label = gtk_label_new("Source Firmware:");
    gtk_label_set_xalign(GTK_LABEL(source_label), 0);
    gtk_label_set_width_chars(GTK_LABEL(source_label), 18);
    gtk_box_pack_start(GTK_BOX(source_hbox), source_label, FALSE, FALSE, 0);

    gui.source_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(gui.source_entry),
                                    "Path to firmware file to download");
    gtk_box_pack_start(GTK_BOX(source_hbox), gui.source_entry, TRUE, TRUE, 0);

    GtkWidget *source_browse = gtk_button_new_with_label("Browse...");
    g_signal_connect(source_browse, "clicked", G_CALLBACK(on_browse_source), NULL);
    gtk_box_pack_start(GTK_BOX(source_hbox), source_browse, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(file_box), source_hbox, FALSE, FALSE, 0);

    /* Output file */
    GtkWidget *output_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *output_label = gtk_label_new("Output Firmware:");
    gtk_label_set_xalign(GTK_LABEL(output_label), 0);
    gtk_label_set_width_chars(GTK_LABEL(output_label), 18);
    gtk_box_pack_start(GTK_BOX(output_hbox), output_label, FALSE, FALSE, 0);

    gui.output_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(gui.output_entry),
                                    "Path to save downloaded firmware");
    gtk_box_pack_start(GTK_BOX(output_hbox), gui.output_entry, TRUE, TRUE, 0);

    GtkWidget *output_browse = gtk_button_new_with_label("Browse...");
    g_signal_connect(output_browse, "clicked", G_CALLBACK(on_browse_output), NULL);
    gtk_box_pack_start(GTK_BOX(output_hbox), output_browse, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(file_box), output_hbox, FALSE, FALSE, 0);
    gtk_container_add(GTK_CONTAINER(file_frame), file_box);
    gtk_box_pack_start(GTK_BOX(main_box), file_frame, FALSE, FALSE, 0);

    /* === Verification Section === */
    GtkWidget *verify_frame = gtk_frame_new("Firmware Verification");
    GtkWidget *verify_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(verify_box), 10);

    /* Hash algorithm and value */
    GtkWidget *hash_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *hash_label = gtk_label_new("Hash Algorithm:");
    gtk_label_set_xalign(GTK_LABEL(hash_label), 0);
    gtk_label_set_width_chars(GTK_LABEL(hash_label), 18);
    gtk_box_pack_start(GTK_BOX(hash_hbox), hash_label, FALSE, FALSE, 0);

    gui.hash_algo_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(gui.hash_algo_combo), "sha256", "SHA-256 (Recommended)");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(gui.hash_algo_combo), "sha512", "SHA-512");
    gtk_combo_box_text_append(GTK_COMBO_BOX_TEXT(gui.hash_algo_combo), "md5", "MD5 (Legacy)");
    gtk_combo_box_set_active(GTK_COMBO_BOX(gui.hash_algo_combo), 0);
    gtk_box_pack_start(GTK_BOX(hash_hbox), gui.hash_algo_combo, FALSE, FALSE, 0);

    GtkWidget *hash_value_label = gtk_label_new("Expected Hash:");
    gtk_box_pack_start(GTK_BOX(hash_hbox), hash_value_label, FALSE, FALSE, 0);

    gui.expected_hash_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(gui.expected_hash_entry),
                                    "SHA-256 hash in hex format");
    gtk_box_pack_start(GTK_BOX(hash_hbox), gui.expected_hash_entry, TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(verify_box), hash_hbox, FALSE, FALSE, 0);

    /* CRC value */
    GtkWidget *crc_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *crc_label = gtk_label_new("Expected CRC32:");
    gtk_label_set_xalign(GTK_LABEL(crc_label), 0);
    gtk_label_set_width_chars(GTK_LABEL(crc_label), 18);
    gtk_box_pack_start(GTK_BOX(crc_hbox), crc_label, FALSE, FALSE, 0);

    gui.expected_crc_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(gui.expected_crc_entry),
                                    "CRC32 value (decimal or hex, optional)");
    gtk_box_pack_start(GTK_BOX(crc_hbox), gui.expected_crc_entry, TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(verify_box), crc_hbox, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(verify_frame), verify_box);
    gtk_box_pack_start(GTK_BOX(main_box), verify_frame, FALSE, FALSE, 0);

    /* === Encryption Section === */
    GtkWidget *encrypt_frame = gtk_frame_new("Encryption & Compression");
    GtkWidget *encrypt_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(encrypt_box), 10);

    /* Encrypted checkbox */
    gui.encrypted_check = gtk_check_button_new_with_label(
        "Firmware is AES-256 encrypted");
    g_signal_connect(gui.encrypted_check, "toggled",
                     G_CALLBACK(on_encrypted_toggled), NULL);
    gtk_box_pack_start(GTK_BOX(encrypt_box), gui.encrypted_check, FALSE, FALSE, 0);

    /* Password entry */
    GtkWidget *password_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *password_label = gtk_label_new("Decryption Password:");
    gtk_label_set_xalign(GTK_LABEL(password_label), 0);
    gtk_label_set_width_chars(GTK_LABEL(password_label), 18);
    gtk_box_pack_start(GTK_BOX(password_hbox), password_label, FALSE, FALSE, 0);

    gui.password_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(gui.password_entry),
                                    "Password for AES-256 decryption");
    gtk_entry_set_visibility(GTK_ENTRY(gui.password_entry), FALSE);
    gtk_widget_set_sensitive(gui.password_entry, FALSE);
    gtk_box_pack_start(GTK_BOX(password_hbox), gui.password_entry, TRUE, TRUE, 0);

    gtk_box_pack_start(GTK_BOX(encrypt_box), password_hbox, FALSE, FALSE, 0);

    /* Compressed checkbox */
    gui.compressed_check = gtk_check_button_new_with_label(
        "Firmware is gzip compressed");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui.compressed_check), TRUE);
    gtk_box_pack_start(GTK_BOX(encrypt_box), gui.compressed_check, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(encrypt_frame), encrypt_box);
    gtk_box_pack_start(GTK_BOX(main_box), encrypt_frame, FALSE, FALSE, 0);

    /* === Progress Section === */
    GtkWidget *progress_frame = gtk_frame_new("Download Progress");
    GtkWidget *progress_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(progress_box), 10);

    gui.status_label = gtk_label_new("Ready");
    gtk_label_set_xalign(GTK_LABEL(gui.status_label), 0);
    gtk_box_pack_start(GTK_BOX(progress_box), gui.status_label, FALSE, FALSE, 0);

    gui.progress_bar = gtk_progress_bar_new();
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(gui.progress_bar), 0.0);
    gtk_box_pack_start(GTK_BOX(progress_box), gui.progress_bar, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(progress_frame), progress_box);
    gtk_box_pack_start(GTK_BOX(main_box), progress_frame, FALSE, FALSE, 0);

    /* === Output Section === */
    GtkWidget *output_frame = gtk_frame_new("Download Log");
    GtkWidget *output_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(output_scroll),
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    gui.output_text = gtk_text_view_new();
    gui.text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gui.output_text));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(gui.output_text), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(gui.output_text), GTK_WRAP_WORD);
    gtk_container_add(GTK_CONTAINER(output_scroll), gui.output_text);

    gtk_container_add(GTK_CONTAINER(output_frame), output_scroll);
    gtk_box_pack_start(GTK_BOX(main_box), output_frame, TRUE, TRUE, 0);

    /* === Action Buttons === */
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_widget_set_halign(button_box, GTK_ALIGN_END);

    gui.download_button = gtk_button_new_with_label("Start Download");
    g_signal_connect(gui.download_button, "clicked", G_CALLBACK(on_download_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), gui.download_button, FALSE, FALSE, 0);

    gui.cancel_button = gtk_button_new_with_label("Cancel");
    g_signal_connect(gui.cancel_button, "clicked", G_CALLBACK(on_cancel_clicked), NULL);
    gtk_widget_set_sensitive(gui.cancel_button, FALSE);
    gtk_box_pack_start(GTK_BOX(button_box), gui.cancel_button, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(main_box), button_box, FALSE, FALSE, 0);

    /* Add tab to notebook */
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), main_box,
                             gtk_label_new("Downloader"));

    gtk_widget_show_all(main_box);
}

void downloader_gui_init(void)
{
    memset(&gui, 0, sizeof(DownloaderGUI));
}

void downloader_gui_cleanup(void)
{
    if (gui.downloader) {
        fw_downloader_free(gui.downloader);
        gui.downloader = NULL;
    }

    if (gui.is_downloading) {
        gui.is_downloading = 0;
        pthread_join(gui.download_thread, NULL);
    }

    memset(&gui, 0, sizeof(DownloaderGUI));
}
