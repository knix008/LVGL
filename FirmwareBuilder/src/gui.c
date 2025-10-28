#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "firmware_builder.h"
#include "compressor.h"
#include "crc.h"
#include "hasher.h"

typedef struct {
    GtkWidget *window;
    /* Build tab widgets */
    GtkWidget *source_entry;
    GtkWidget *output_entry;
    GtkWidget *compress_check;
    GtkWidget *compress_level_spin;
    GtkWidget *crc_check;
    GtkWidget *encrypt_check;
    GtkWidget *password_entry;
    GtkWidget *hash_check;
    /* Extract tab widgets */
    GtkWidget *extract_file_entry;
    GtkWidget *extract_output_entry;
    GtkWidget *extract_password_entry;
    /* Output and status */
    GtkWidget *output_text;
    GtkWidget *status_bar;
    GtkTextBuffer *text_buffer;
    guint status_context;
} FirmwareBuilderGUI;

static FirmwareBuilderGUI gui = {0};

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

static void on_browse_source(GtkButton *button, gpointer data)
{
    (void)button;
    (void)data;

    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Select Source Directory",
        GTK_WINDOW(gui.window),
        GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
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
        "Select Output Directory",
        GTK_WINDOW(gui.window),
        GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Open", GTK_RESPONSE_ACCEPT,
        NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        gtk_entry_set_text(GTK_ENTRY(gui.output_entry), filename);
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

static void on_browse_extract_file(GtkButton *button, gpointer data)
{
    (void)button;
    (void)data;

    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Select Firmware File to Extract",
        GTK_WINDOW(gui.window),
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Open", GTK_RESPONSE_ACCEPT,
        NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        gtk_entry_set_text(GTK_ENTRY(gui.extract_file_entry), filename);
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

static void on_browse_extract_output(GtkButton *button, gpointer data)
{
    (void)button;
    (void)data;

    GtkWidget *dialog = gtk_file_chooser_dialog_new(
        "Select Output Directory for Extraction",
        GTK_WINDOW(gui.window),
        GTK_FILE_CHOOSER_ACTION_CREATE_FOLDER,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Open", GTK_RESPONSE_ACCEPT,
        NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        gtk_entry_set_text(GTK_ENTRY(gui.extract_output_entry), filename);
        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

static void on_build_clicked(GtkButton *button, gpointer data)
{
    (void)button;
    (void)data;

    const char *source = gtk_entry_get_text(GTK_ENTRY(gui.source_entry));
    const char *output = gtk_entry_get_text(GTK_ENTRY(gui.output_entry));

    if (!source || strlen(source) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(
            GTK_WINDOW(gui.window),
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Error");
        gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog),
            "Please select a source directory");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    /* Get options from GUI */
    int compress = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gui.compress_check));
    int compress_level = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(gui.compress_level_spin));
    int add_crc = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gui.crc_check));
    int encrypt = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gui.encrypt_check));
    int hash = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gui.hash_check));
    const char *password = gtk_entry_get_text(GTK_ENTRY(gui.password_entry));

    /* Clear output */
    gtk_text_buffer_set_text(gui.text_buffer, "", -1);
    append_output("🔨 Starting build process...");
    append_output("");

    /* Create build options */
    fw_build_options_t opts = {
        .compress = compress,
        .compression_level = compress_level,
        .add_crc = add_crc,
        .encrypt = encrypt,
        .encryption_password = (char *)password,
        .generate_hash = hash
    };

    /* Create firmware builder */
    fw_package_t *pkg = fw_builder_create(source, output, opts);
    if (!pkg) {
        append_output("❌ ERROR: Failed to create firmware builder");
        gtk_statusbar_push(GTK_STATUSBAR(gui.status_bar), gui.status_context, "Build failed");
        return;
    }

    /* Build firmware */
    if (fw_builder_build(pkg) != 0) {
        append_output("❌ ERROR: Build failed");
        gtk_statusbar_push(GTK_STATUSBAR(gui.status_bar), gui.status_context, "Build failed");
        fw_builder_free(pkg);
        return;
    }

    append_output("");
    append_output("✅ Build completed successfully!");
    append_output("");
    append_output("📦 Output: Build artifacts created in");
    append_output(output);

    fw_builder_free(pkg);
    gtk_statusbar_push(GTK_STATUSBAR(gui.status_bar), gui.status_context, "Build completed successfully");
}

static void on_extract_clicked(GtkButton *button, gpointer data)
{
    (void)button;
    (void)data;

    const char *file = gtk_entry_get_text(GTK_ENTRY(gui.extract_file_entry));
    const char *output = gtk_entry_get_text(GTK_ENTRY(gui.extract_output_entry));
    const char *password = gtk_entry_get_text(GTK_ENTRY(gui.extract_password_entry));
    (void)password;

    if (!file || strlen(file) == 0) {
        GtkWidget *dialog = gtk_message_dialog_new(
            GTK_WINDOW(gui.window),
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Error");
        gtk_message_dialog_format_secondary_text(GTK_MESSAGE_DIALOG(dialog),
            "Please select a firmware file to extract");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    /* Clear output */
    gtk_text_buffer_set_text(gui.text_buffer, "", -1);
    append_output("🔓 Starting extraction process...");
    append_output("");

    /* For future implementation: extract_firmware(file, output, password) */
    append_output("📂 Source file: " );
    append_output(file);
    append_output("");
    append_output("📁 Output directory: " );
    append_output(output ? output : "./extracted");
    append_output("");
    append_output("⚠️  NOTE: Extraction feature coming soon");
    append_output("This feature allows you to extract and verify firmware packages");

    gtk_statusbar_push(GTK_STATUSBAR(gui.status_bar), gui.status_context, "Extraction feature not yet implemented");
}

static void on_compression_toggled(GtkToggleButton *button, gpointer data)
{
    (void)data;
    gboolean active = gtk_toggle_button_get_active(button);
    gtk_widget_set_sensitive(gui.compress_level_spin, active);
}

static void on_encryption_toggled(GtkToggleButton *button, gpointer data)
{
    (void)data;
    gboolean active = gtk_toggle_button_get_active(button);
    gtk_widget_set_sensitive(gui.password_entry, active);
}

static void create_build_tab(GtkWidget *notebook)
{
    GtkWidget *build_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(build_box), 10);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), build_box, gtk_label_new("Build"));

    /* Source selection */
    GtkWidget *source_frame = gtk_frame_new("Source Directory");
    GtkWidget *source_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gui.source_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(gui.source_entry), "Select source directory...");
    gtk_box_pack_start(GTK_BOX(source_hbox), gui.source_entry, TRUE, TRUE, 0);

    GtkWidget *browse_source_btn = gtk_button_new_with_label("Browse");
    g_signal_connect(browse_source_btn, "clicked", G_CALLBACK(on_browse_source), NULL);
    gtk_box_pack_start(GTK_BOX(source_hbox), browse_source_btn, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(source_frame), source_hbox);
    gtk_box_pack_start(GTK_BOX(build_box), source_frame, FALSE, FALSE, 0);

    /* Output selection */
    GtkWidget *output_frame = gtk_frame_new("Output Directory");
    GtkWidget *output_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gui.output_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(gui.output_entry), "Select output directory...");
    gtk_entry_set_text(GTK_ENTRY(gui.output_entry), "./firmware-build");
    gtk_box_pack_start(GTK_BOX(output_hbox), gui.output_entry, TRUE, TRUE, 0);

    GtkWidget *browse_output_btn = gtk_button_new_with_label("Browse");
    g_signal_connect(browse_output_btn, "clicked", G_CALLBACK(on_browse_output), NULL);
    gtk_box_pack_start(GTK_BOX(output_hbox), browse_output_btn, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(output_frame), output_hbox);
    gtk_box_pack_start(GTK_BOX(build_box), output_frame, FALSE, FALSE, 0);

    /* Options frame */
    GtkWidget *options_frame = gtk_frame_new("Build Options");
    GtkWidget *options_grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(options_grid), 10);
    gtk_grid_set_row_spacing(GTK_GRID(options_grid), 5);
    gtk_container_set_border_width(GTK_CONTAINER(options_grid), 10);

    int row = 0;

    /* Compression */
    gui.compress_check = gtk_check_button_new_with_label("Enable Compression");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui.compress_check), TRUE);
    g_signal_connect(gui.compress_check, "toggled", G_CALLBACK(on_compression_toggled), NULL);
    gtk_grid_attach(GTK_GRID(options_grid), gui.compress_check, 0, row, 2, 1);
    row++;

    GtkWidget *compress_level_label = gtk_label_new("Compression Level:");
    gtk_grid_attach(GTK_GRID(options_grid), compress_level_label, 0, row, 1, 1);
    gui.compress_level_spin = gtk_spin_button_new_with_range(0, 9, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(gui.compress_level_spin), 6);
    gtk_grid_attach(GTK_GRID(options_grid), gui.compress_level_spin, 1, row, 1, 1);
    row++;

    /* CRC */
    gui.crc_check = gtk_check_button_new_with_label("Add CRC Checksum");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui.crc_check), TRUE);
    gtk_grid_attach(GTK_GRID(options_grid), gui.crc_check, 0, row, 2, 1);
    row++;

    /* Encryption */
    gui.encrypt_check = gtk_check_button_new_with_label("Enable Encryption");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui.encrypt_check), FALSE);
    g_signal_connect(gui.encrypt_check, "toggled", G_CALLBACK(on_encryption_toggled), NULL);
    gtk_grid_attach(GTK_GRID(options_grid), gui.encrypt_check, 0, row, 2, 1);
    row++;

    GtkWidget *password_label = gtk_label_new("Password:");
    gtk_grid_attach(GTK_GRID(options_grid), password_label, 0, row, 1, 1);
    gui.password_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(gui.password_entry), "Enter encryption password");
    gtk_entry_set_visibility(GTK_ENTRY(gui.password_entry), FALSE);
    gtk_widget_set_sensitive(gui.password_entry, FALSE);
    gtk_grid_attach(GTK_GRID(options_grid), gui.password_entry, 1, row, 1, 1);
    row++;

    /* Hash */
    gui.hash_check = gtk_check_button_new_with_label("Generate Hash");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui.hash_check), TRUE);
    gtk_grid_attach(GTK_GRID(options_grid), gui.hash_check, 0, row, 2, 1);
    row++;

    gtk_container_add(GTK_CONTAINER(options_frame), options_grid);
    gtk_box_pack_start(GTK_BOX(build_box), options_frame, FALSE, FALSE, 0);

    /* Output text view */
    GtkWidget *output_text_frame = gtk_frame_new("Build Output");
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    gui.output_text = gtk_text_view_new();
    gui.text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(gui.output_text));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(gui.output_text), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(gui.output_text), TRUE);
    gtk_container_add(GTK_CONTAINER(scrolled), gui.output_text);
    gtk_container_add(GTK_CONTAINER(output_text_frame), scrolled);
    gtk_box_pack_start(GTK_BOX(build_box), output_text_frame, TRUE, TRUE, 0);

    /* Build button */
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_set_homogeneous(GTK_BOX(button_box), FALSE);

    GtkWidget *build_btn = gtk_button_new_with_label("Build Firmware");
    gtk_widget_set_size_request(build_btn, 150, 40);
    g_signal_connect(build_btn, "clicked", G_CALLBACK(on_build_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(button_box), build_btn, FALSE, FALSE, 0);

    gtk_box_pack_end(GTK_BOX(build_box), button_box, FALSE, FALSE, 0);
}

static void create_extract_tab(GtkWidget *notebook)
{
    GtkWidget *extract_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(extract_box), 10);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), extract_box, gtk_label_new("Extract"));

    /* File selection */
    GtkWidget *file_frame = gtk_frame_new("Firmware File");
    GtkWidget *file_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gui.extract_file_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(gui.extract_file_entry), "Select firmware file to extract...");
    gtk_box_pack_start(GTK_BOX(file_hbox), gui.extract_file_entry, TRUE, TRUE, 0);

    GtkWidget *browse_file_btn = gtk_button_new_with_label("Browse");
    g_signal_connect(browse_file_btn, "clicked", G_CALLBACK(on_browse_extract_file), NULL);
    gtk_box_pack_start(GTK_BOX(file_hbox), browse_file_btn, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(file_frame), file_hbox);
    gtk_box_pack_start(GTK_BOX(extract_box), file_frame, FALSE, FALSE, 0);

    /* Output selection */
    GtkWidget *extract_output_frame = gtk_frame_new("Extract To Directory");
    GtkWidget *extract_output_hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gui.extract_output_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(gui.extract_output_entry), "Select output directory...");
    gtk_entry_set_text(GTK_ENTRY(gui.extract_output_entry), "./extracted");
    gtk_box_pack_start(GTK_BOX(extract_output_hbox), gui.extract_output_entry, TRUE, TRUE, 0);

    GtkWidget *browse_extract_output_btn = gtk_button_new_with_label("Browse");
    g_signal_connect(browse_extract_output_btn, "clicked", G_CALLBACK(on_browse_extract_output), NULL);
    gtk_box_pack_start(GTK_BOX(extract_output_hbox), browse_extract_output_btn, FALSE, FALSE, 0);

    gtk_container_add(GTK_CONTAINER(extract_output_frame), extract_output_hbox);
    gtk_box_pack_start(GTK_BOX(extract_box), extract_output_frame, FALSE, FALSE, 0);

    /* Password (if encrypted) */
    GtkWidget *extract_pwd_frame = gtk_frame_new("Encryption Password (if needed)");
    gui.extract_password_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(gui.extract_password_entry), "Enter password if firmware is encrypted");
    gtk_entry_set_visibility(GTK_ENTRY(gui.extract_password_entry), FALSE);
    gtk_container_add(GTK_CONTAINER(extract_pwd_frame), gui.extract_password_entry);
    gtk_box_pack_start(GTK_BOX(extract_box), extract_pwd_frame, FALSE, FALSE, 0);

    /* Output text view */
    GtkWidget *extract_output_text_frame = gtk_frame_new("Extraction Output");
    GtkWidget *extract_scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(extract_scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(extract_box), extract_output_text_frame, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(extract_output_text_frame), extract_scrolled);

    /* Extract button */
    GtkWidget *extract_button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_set_homogeneous(GTK_BOX(extract_button_box), FALSE);

    GtkWidget *extract_btn = gtk_button_new_with_label("Extract Firmware");
    gtk_widget_set_size_request(extract_btn, 150, 40);
    g_signal_connect(extract_btn, "clicked", G_CALLBACK(on_extract_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(extract_button_box), extract_btn, FALSE, FALSE, 0);

    gtk_box_pack_end(GTK_BOX(extract_box), extract_button_box, FALSE, FALSE, 0);
}

static void create_info_tab(GtkWidget *notebook)
{
    GtkWidget *info_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(info_box), 10);
    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), info_box, gtk_label_new("Info"));

    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    GtkWidget *info_text = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(info_text), FALSE);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(info_text), TRUE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(info_text), GTK_WRAP_WORD);

    GtkTextBuffer *info_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(info_text));
    gtk_text_buffer_set_text(info_buffer,
        "Firmware Builder v1.0.0\n"
        "==================================\n\n"
        "A comprehensive firmware building tool with multiple features:\n\n"
        "BUILD TAB:\n"
        "- Select source directory to build from\n"
        "- Choose output directory for firmware\n"
        "- Enable/disable compression (gzip, level 0-9)\n"
        "- Add CRC checksums for integrity verification\n"
        "- Enable AES-256-CBC encryption with passwords\n"
        "- Generate SHA-256/SHA-512/MD5 hashes\n\n"
        "EXTRACT TAB:\n"
        "- Extract previously built firmware packages\n"
        "- Verify integrity with stored checksums\n"
        "- Decrypt encrypted firmware with password\n\n"
        "FEATURES:\n"
        "- Compression: Gzip with configurable levels\n"
        "- Checksum: CRC32, CRC16, CRC8\n"
        "- Encryption: AES-256-CBC with PBKDF2 key derivation\n"
        "- Hashing: SHA-256, SHA-512, MD5\n"
        "- Cross-platform: Linux, macOS, Windows\n\n"
        "USAGE:\n"
        "1. Select source directory containing firmware files\n"
        "2. Choose output location for built firmware\n"
        "3. Configure options (compression, encryption, etc.)\n"
        "4. Click 'Build Firmware' to start\n"
        "5. Monitor progress in the output area\n\n"
        "NOTES:\n"
        "- Encryption requires a password (min 8 characters recommended)\n"
        "- Larger compression levels take more time but achieve better ratio\n"
        "- CRC and hash checksums are always computed for verification\n",
        -1);

    gtk_container_add(GTK_CONTAINER(scrolled), info_text);
    gtk_box_pack_start(GTK_BOX(info_box), scrolled, TRUE, TRUE, 0);
}

static void create_gui(void)
{
    gui.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(gui.window), "Firmware Builder - GTK GUI");
    gtk_window_set_default_size(GTK_WINDOW(gui.window), 800, 700);
    gtk_container_set_border_width(GTK_CONTAINER(gui.window), 10);
    g_signal_connect(gui.window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    /* Main vbox */
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_add(GTK_CONTAINER(gui.window), main_box);

    /* Title */
    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), "<big><b>Firmware Builder</b></big>");
    gtk_box_pack_start(GTK_BOX(main_box), title, FALSE, FALSE, 0);

    /* Subtitle */
    GtkWidget *subtitle = gtk_label_new("Build, compress, encrypt, and verify firmware packages");
    gtk_label_set_markup(GTK_LABEL(subtitle), "<small>Build, compress, encrypt, and verify firmware packages</small>");
    gtk_box_pack_start(GTK_BOX(main_box), subtitle, FALSE, FALSE, 0);

    /* Notebook for tabs */
    GtkWidget *notebook = gtk_notebook_new();
    gtk_box_pack_start(GTK_BOX(main_box), notebook, TRUE, TRUE, 0);

    /* Create tabs */
    create_build_tab(notebook);
    create_extract_tab(notebook);
    create_info_tab(notebook);

    /* Status bar */
    gui.status_bar = gtk_statusbar_new();
    gui.status_context = gtk_statusbar_get_context_id(GTK_STATUSBAR(gui.status_bar), "main");
    gtk_statusbar_push(GTK_STATUSBAR(gui.status_bar), gui.status_context, "Ready");
    gtk_box_pack_end(GTK_BOX(main_box), gui.status_bar, FALSE, FALSE, 0);

    gtk_widget_show_all(gui.window);
}

int main(int argc, char *argv[])
{
    gtk_init(&argc, &argv);

    create_gui();

    gtk_main();

    return 0;
}
