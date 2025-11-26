#include <gtk/gtk.h>
#include <string.h>
#include <locale.h>
#include "qwerty.h"

// Application state
typedef struct {
    GtkWidget *window;
    GtkWidget *text_view;
    GtkTextBuffer *text_buffer;
    GtkWidget *status_label;
    GtkWidget *shift_buttons[2];  // Left and right shift
    GtkWidget *caps_button;
    GtkWidget *lang_button;
    QwertyState qwerty;
} AppState;

static AppState app_state = {NULL, NULL, NULL, NULL, {NULL, NULL}, NULL, NULL, {LANG_ENGLISH, 0, 0, {0, 0, 0, 0}}};

// Global storage for key buttons to update labels
static GtkWidget *key_buttons[50];
static KeyMap *key_button_maps[50];
static int num_key_buttons = 0;

// Forward declarations
static void update_status();
static void update_button_labels();

// Update status label
static void update_status() {
    gchar *status_text = g_strdup_printf(
        "Mode: %s | Shift: %s | Caps: %s",
        app_state.qwerty.current_language == LANG_ENGLISH ? "English" : "한국어",
        app_state.qwerty.shift_pressed ? "ON" : "OFF",
        app_state.qwerty.caps_lock ? "ON" : "OFF"
    );
    gtk_label_set_text(GTK_LABEL(app_state.status_label), status_text);
    g_free(status_text);
}

// Delete last character
static void delete_last_char() {
    GtkTextIter start, end;
    gtk_text_buffer_get_iter_at_mark(
        app_state.text_buffer,
        &end,
        gtk_text_buffer_get_insert(app_state.text_buffer)
    );
    start = end;
    if (gtk_text_iter_backward_char(&start)) {
        gtk_text_buffer_delete(app_state.text_buffer, &start, &end);
    }
}

// Insert text at cursor position
static void insert_text(const char *text) {
    GtkTextIter iter;
    gtk_text_buffer_get_iter_at_mark(
        app_state.text_buffer,
        &iter,
        gtk_text_buffer_get_insert(app_state.text_buffer)
    );
    gtk_text_buffer_insert(app_state.text_buffer, &iter, text, -1);
}

// Button click callback
static void on_key_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;  // Unused parameter
    KeyMap *key_map = (KeyMap *)data;
    const char *text = qwerty_get_key_char(&app_state.qwerty, key_map);

    if (app_state.qwerty.current_language == LANG_KOREAN) {
        char output[21] = {0};  // Enough for 2 Korean syllables
        int delete_prev = 0;
        qwerty_process_korean_char(&app_state.qwerty, text, output, &delete_prev);

        if (delete_prev) {
            delete_last_char();
        }
        insert_text(output);
    } else {
        insert_text(text);
        qwerty_reset_composition(&app_state.qwerty);
    }
}

// Backspace callback
static void on_backspace_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;  // Unused parameter
    (void)data;    // Unused parameter
    delete_last_char();
    qwerty_reset_composition(&app_state.qwerty);
}

// Space callback
static void on_space_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;  // Unused parameter
    (void)data;    // Unused parameter
    insert_text(" ");
    qwerty_reset_composition(&app_state.qwerty);
}

// Enter callback
static void on_enter_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;  // Unused parameter
    (void)data;    // Unused parameter
    insert_text("\n");
    qwerty_reset_composition(&app_state.qwerty);
}

// Tab callback
static void on_tab_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;  // Unused parameter
    (void)data;    // Unused parameter
    insert_text("\t");
    qwerty_reset_composition(&app_state.qwerty);
}

// Shift toggle callback
static void on_shift_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;  // Unused parameter
    (void)data;    // Unused parameter
    app_state.qwerty.shift_pressed = !app_state.qwerty.shift_pressed;
    update_status();
    update_button_labels();
}

// Caps lock callback
static void on_caps_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;  // Unused parameter
    (void)data;    // Unused parameter
    app_state.qwerty.caps_lock = !app_state.qwerty.caps_lock;
    update_status();
    update_button_labels();
}

// Language switch callback
static void on_lang_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;  // Unused parameter
    (void)data;    // Unused parameter
    app_state.qwerty.current_language = (app_state.qwerty.current_language == LANG_ENGLISH)
                                  ? LANG_KOREAN : LANG_ENGLISH;
    qwerty_reset_composition(&app_state.qwerty);
    update_status();
    update_button_labels();
}

// Clear text callback
static void on_clear_clicked(GtkWidget *widget, gpointer data) {
    (void)widget;  // Unused parameter
    (void)data;    // Unused parameter
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(app_state.text_buffer, &start, &end);
    gtk_text_buffer_delete(app_state.text_buffer, &start, &end);
    qwerty_reset_composition(&app_state.qwerty);
}

// Create a keyboard button
static GtkWidget* create_key_button(const char *label, GCallback callback, gpointer data, int width) {
    GtkWidget *button = gtk_button_new_with_label(label);
    gtk_widget_set_size_request(button, width, 50);
    g_signal_connect(button, "clicked", callback, data);

    // Set monospace font for better key display using CSS
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "button { font-family: monospace; font-weight: bold; font-size: 11pt; }",
        -1, NULL);
    gtk_style_context_add_provider(
        gtk_widget_get_style_context(button),
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );
    g_object_unref(provider);

    return button;
}

// Update all button labels based on current state
static void update_button_labels() {
    for (int i = 0; i < num_key_buttons; i++) {
        const char *label = qwerty_get_key_char(&app_state.qwerty, key_button_maps[i]);
        gtk_button_set_label(GTK_BUTTON(key_buttons[i]), label);
    }

    // Update shift button appearance
    for (int i = 0; i < 2; i++) {
        if (app_state.shift_buttons[i]) {
            if (app_state.qwerty.shift_pressed) {
                gtk_widget_set_name(app_state.shift_buttons[i], "active-modifier");
            } else {
                gtk_widget_set_name(app_state.shift_buttons[i], "");
            }
        }
    }

    // Update caps button appearance
    if (app_state.caps_button) {
        if (app_state.qwerty.caps_lock) {
            gtk_widget_set_name(app_state.caps_button, "active-modifier");
        } else {
            gtk_widget_set_name(app_state.caps_button, "");
        }
    }
}

// Create the GUI
static void activate(GtkApplication *app, gpointer user_data) {
    (void)user_data;  // Unused parameter
    GtkWidget *window;
    GtkWidget *main_box;
    GtkWidget *text_scroll;
    GtkWidget *keyboard_box;
    GtkWidget *row_box;
    GtkWidget *button;

    // Initialize qwerty state
    qwerty_init(&app_state.qwerty);

    // Create main window
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Korean/English QWERTY Keypad");
    gtk_window_set_default_size(GTK_WINDOW(window), 1000, 600);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);
    app_state.window = window;

    // CSS for styling
    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(css_provider,
        "#active-modifier { background: #4CAF50; color: white; }"
        "button { margin: 2px; }"
        "textview { font-family: Sans; font-size: 14pt; }"
        , -1, NULL);
    gtk_style_context_add_provider_for_screen(
        gdk_screen_get_default(),
        GTK_STYLE_PROVIDER(css_provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

    // Main vertical box
    main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), main_box);

    // Status label
    app_state.status_label = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(main_box), app_state.status_label, FALSE, FALSE, 5);
    update_status();

    // Text view with scrolling
    text_scroll = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(text_scroll),
                                    GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(text_scroll, -1, 150);

    app_state.text_view = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(app_state.text_view), GTK_WRAP_WORD_CHAR);
    app_state.text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(app_state.text_view));

    gtk_container_add(GTK_CONTAINER(text_scroll), app_state.text_view);
    gtk_box_pack_start(GTK_BOX(main_box), text_scroll, TRUE, TRUE, 5);

    // Keyboard layout
    keyboard_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_box_pack_start(GTK_BOX(main_box), keyboard_box, FALSE, FALSE, 5);

    // Row 0: Numbers and symbols
    row_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_widget_set_halign(row_box, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(keyboard_box), row_box, FALSE, FALSE, 2);

    for (int i = 0; i < 13; i++) {
        key_buttons[num_key_buttons] = create_key_button(
            qwerty_get_key_char(&app_state.qwerty, &key_maps[i]),
            G_CALLBACK(on_key_clicked),
            &key_maps[i],
            50
        );
        key_button_maps[num_key_buttons] = &key_maps[i];
        gtk_box_pack_start(GTK_BOX(row_box), key_buttons[num_key_buttons], FALSE, FALSE, 0);
        num_key_buttons++;
    }
    button = create_key_button("Backspace", G_CALLBACK(on_backspace_clicked), NULL, 100);
    gtk_box_pack_start(GTK_BOX(row_box), button, FALSE, FALSE, 0);

    // Row 1: QWERTY
    row_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_widget_set_halign(row_box, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(keyboard_box), row_box, FALSE, FALSE, 2);

    button = create_key_button("Tab", G_CALLBACK(on_tab_clicked), NULL, 75);
    gtk_box_pack_start(GTK_BOX(row_box), button, FALSE, FALSE, 0);

    for (int i = 13; i < 26; i++) {
        key_buttons[num_key_buttons] = create_key_button(
            qwerty_get_key_char(&app_state.qwerty, &key_maps[i]),
            G_CALLBACK(on_key_clicked),
            &key_maps[i],
            50
        );
        key_button_maps[num_key_buttons] = &key_maps[i];
        gtk_box_pack_start(GTK_BOX(row_box), key_buttons[num_key_buttons], FALSE, FALSE, 0);
        num_key_buttons++;
    }

    // Row 2: ASDF
    row_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_widget_set_halign(row_box, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(keyboard_box), row_box, FALSE, FALSE, 2);

    app_state.caps_button = create_key_button("Caps", G_CALLBACK(on_caps_clicked), NULL, 90);
    gtk_box_pack_start(GTK_BOX(row_box), app_state.caps_button, FALSE, FALSE, 0);

    for (int i = 26; i < 37; i++) {
        key_buttons[num_key_buttons] = create_key_button(
            qwerty_get_key_char(&app_state.qwerty, &key_maps[i]),
            G_CALLBACK(on_key_clicked),
            &key_maps[i],
            50
        );
        key_button_maps[num_key_buttons] = &key_maps[i];
        gtk_box_pack_start(GTK_BOX(row_box), key_buttons[num_key_buttons], FALSE, FALSE, 0);
        num_key_buttons++;
    }

    button = create_key_button("Enter", G_CALLBACK(on_enter_clicked), NULL, 85);
    gtk_box_pack_start(GTK_BOX(row_box), button, FALSE, FALSE, 0);

    // Row 3: ZXCV
    row_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_widget_set_halign(row_box, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(keyboard_box), row_box, FALSE, FALSE, 2);

    app_state.shift_buttons[0] = create_key_button("Shift", G_CALLBACK(on_shift_clicked), NULL, 110);
    gtk_box_pack_start(GTK_BOX(row_box), app_state.shift_buttons[0], FALSE, FALSE, 0);

    for (int i = 37; i < 47; i++) {
        key_buttons[num_key_buttons] = create_key_button(
            qwerty_get_key_char(&app_state.qwerty, &key_maps[i]),
            G_CALLBACK(on_key_clicked),
            &key_maps[i],
            50
        );
        key_button_maps[num_key_buttons] = &key_maps[i];
        gtk_box_pack_start(GTK_BOX(row_box), key_buttons[num_key_buttons], FALSE, FALSE, 0);
        num_key_buttons++;
    }

    app_state.shift_buttons[1] = create_key_button("Shift", G_CALLBACK(on_shift_clicked), NULL, 110);
    gtk_box_pack_start(GTK_BOX(row_box), app_state.shift_buttons[1], FALSE, FALSE, 0);

    // Row 4: Space bar and controls
    row_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_widget_set_halign(row_box, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(keyboard_box), row_box, FALSE, FALSE, 2);

    app_state.lang_button = create_key_button("한/영", G_CALLBACK(on_lang_clicked), NULL, 80);
    gtk_box_pack_start(GTK_BOX(row_box), app_state.lang_button, FALSE, FALSE, 0);

    button = create_key_button("Space", G_CALLBACK(on_space_clicked), NULL, 450);
    gtk_box_pack_start(GTK_BOX(row_box), button, FALSE, FALSE, 0);

    button = create_key_button("Clear", G_CALLBACK(on_clear_clicked), NULL, 80);
    gtk_box_pack_start(GTK_BOX(row_box), button, FALSE, FALSE, 0);

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    setlocale(LC_ALL, "");

    GtkApplication *app;
    int status;

    app = gtk_application_new("com.qwerty.keypad", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
