#include "gui.h"
#include "gui_utils.h"
#include "file.h"
#include "chunjiin.h"
#include <glib.h>
#include <wchar.h>

// Event handler - Input button click
static void on_button_clicked(GtkWidget *widget, gpointer data) {
    AppWidgets *app = (AppWidgets *)data;
    int button_num = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "button_num"));

    // Process input
    chunjiin_process_input(&app->state, button_num);

    // Update text view
    gchar *utf8_text = wchar_to_utf8(app->state.text_buffer, MAX_TEXT_LEN);
    gtk_text_buffer_set_text(app->text_buffer, utf8_text, -1);
    g_free(utf8_text);

    // Move cursor to end
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(app->text_buffer, &end);
    gtk_text_buffer_place_cursor(app->text_buffer, &end);
}

// Event handler - Mode button click
static void on_mode_button_clicked(GtkWidget *widget __attribute__((unused)), gpointer data) {
    AppWidgets *app = (AppWidgets *)data;

    change_mode(&app->state);

    // Update button text
    for (int i = 0; i < 12; i++) {
        if (app->buttons[i] == NULL) continue;
        const wchar_t *wtext = get_button_text(app->state.now_mode, i);
        gchar *utf8_text = wchar_to_utf8(wtext, 20);
        gtk_button_set_label(GTK_BUTTON(app->buttons[i]), utf8_text);

        // Reapply font (label may have been recreated)
        apply_button_font(app->buttons[i], "NanumGothicCoding", 20);

        g_free(utf8_text);
    }
}

// Event handler - Clear button click
static void on_clear_clicked(GtkWidget *widget __attribute__((unused)), gpointer data) {
    AppWidgets *app = (AppWidgets *)data;

    // Save current mode
    InputMode current_mode = app->state.now_mode;

    // Clear text (preserve mode)
    chunjiin_init(&app->state);
    app->state.now_mode = current_mode;
    gtk_text_buffer_set_text(app->text_buffer, "", -1);
}

// Event handler - Enter button click
static void on_enter_clicked(GtkWidget *widget __attribute__((unused)), gpointer data) {
    AppWidgets *app = (AppWidgets *)data;

    // Save current mode
    InputMode current_mode = app->state.now_mode;

    // Get current text
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(app->text_buffer, &start, &end);
    gchar *text = gtk_text_buffer_get_text(app->text_buffer, &start, &end, FALSE);

    // Create result dialog
    GtkWidget *dialog = gtk_dialog_new_with_buttons("입력 결과",
                                                   GTK_WINDOW(app->window),
                                                   GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   "확인", GTK_RESPONSE_OK,
                                                   NULL);

    gtk_window_set_default_size(GTK_WINDOW(dialog), 400, 200);

    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *label = gtk_label_new(text);
    gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_selectable(GTK_LABEL(label), TRUE);

    GtkStyleContext *context = gtk_widget_get_style_context(label);
    gtk_style_context_add_class(context, "dialog-label");

    gtk_widget_set_margin_start(label, 20);
    gtk_widget_set_margin_end(label, 20);
    gtk_widget_set_margin_top(label, 20);
    gtk_widget_set_margin_bottom(label, 20);

    gtk_container_add(GTK_CONTAINER(content_area), label);

    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    g_free(text);

    // Clear text (preserve mode)
    chunjiin_init(&app->state);
    app->state.now_mode = current_mode;
    gtk_text_buffer_set_text(app->text_buffer, "", -1);
}

// Event handler - Save button click
static void on_save_clicked(GtkWidget *widget __attribute__((unused)), gpointer data) {
    AppWidgets *app = (AppWidgets *)data;

    GtkWidget *dialog = gtk_file_chooser_dialog_new("파일 저장",
                                                    GTK_WINDOW(app->window),
                                                    GTK_FILE_CHOOSER_ACTION_SAVE,
                                                    "_취소", GTK_RESPONSE_CANCEL,
                                                    "_저장", GTK_RESPONSE_ACCEPT,
                                                    NULL);

    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "chunjiin_text.txt");

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        if (save_text_to_file(app->state.text_buffer, filename) == 0) {
            GtkWidget *info_dialog = gtk_dialog_new_with_buttons("저장 성공",
                                                                 GTK_WINDOW(app->window),
                                                                 GTK_DIALOG_MODAL,
                                                                 "확인", GTK_RESPONSE_OK,
                                                                 NULL);
            GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(info_dialog));
            GtkWidget *label = gtk_label_new("파일이 저장되었습니다.");
            gtk_container_add(GTK_CONTAINER(content), label);
            gtk_widget_show_all(info_dialog);
            gtk_dialog_run(GTK_DIALOG(info_dialog));
            gtk_widget_destroy(info_dialog);
        } else {
            GtkWidget *error_dialog = gtk_dialog_new_with_buttons("저장 실패",
                                                                  GTK_WINDOW(app->window),
                                                                  GTK_DIALOG_MODAL,
                                                                  "확인", GTK_RESPONSE_OK,
                                                                  NULL);
            GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(error_dialog));
            GtkWidget *label = gtk_label_new("파일 저장에 실패했습니다.");
            gtk_container_add(GTK_CONTAINER(content), label);
            gtk_widget_show_all(error_dialog);
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
        }

        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

// Event handler - Load button click
static void on_load_clicked(GtkWidget *widget __attribute__((unused)), gpointer data) {
    AppWidgets *app = (AppWidgets *)data;

    GtkWidget *dialog = gtk_file_chooser_dialog_new("파일 열기",
                                                    GTK_WINDOW(app->window),
                                                    GTK_FILE_CHOOSER_ACTION_OPEN,
                                                    "_취소", GTK_RESPONSE_CANCEL,
                                                    "_열기", GTK_RESPONSE_ACCEPT,
                                                    NULL);

    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

        if (load_text_from_file(app->state.text_buffer, MAX_TEXT_LEN, filename) == 0) {
            app->state.cursor_pos = wcslen(app->state.text_buffer);

            // Update text view
            gchar *utf8_text = wchar_to_utf8(app->state.text_buffer, MAX_TEXT_LEN);
            gtk_text_buffer_set_text(app->text_buffer, utf8_text, -1);
            g_free(utf8_text);

            // Move cursor to end
            GtkTextIter end;
            gtk_text_buffer_get_end_iter(app->text_buffer, &end);
            gtk_text_buffer_place_cursor(app->text_buffer, &end);

            GtkWidget *info_dialog = gtk_dialog_new_with_buttons("로드 성공",
                                                                 GTK_WINDOW(app->window),
                                                                 GTK_DIALOG_MODAL,
                                                                 "확인", GTK_RESPONSE_OK,
                                                                 NULL);
            GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(info_dialog));
            GtkWidget *label = gtk_label_new("파일이 로드되었습니다.");
            gtk_container_add(GTK_CONTAINER(content), label);
            gtk_widget_show_all(info_dialog);
            gtk_dialog_run(GTK_DIALOG(info_dialog));
            gtk_widget_destroy(info_dialog);
        } else {
            GtkWidget *error_dialog = gtk_dialog_new_with_buttons("로드 실패",
                                                                  GTK_WINDOW(app->window),
                                                                  GTK_DIALOG_MODAL,
                                                                  "확인", GTK_RESPONSE_OK,
                                                                  NULL);
            GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(error_dialog));
            GtkWidget *label = gtk_label_new("파일 로드에 실패했습니다.");
            gtk_container_add(GTK_CONTAINER(content), label);
            gtk_widget_show_all(error_dialog);
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
        }

        g_free(filename);
    }

    gtk_widget_destroy(dialog);
}

// Create main window and GUI
void create_main_window(GtkApplication *app_gtk) {
    // Load Korean font
    load_korean_font();

    AppWidgets *app = g_new0(AppWidgets, 1);
    chunjiin_init(&app->state);

    // Create main window
    app->window = gtk_application_window_new(app_gtk);
    gtk_window_set_title(GTK_WINDOW(app->window), "천지인 한글 입력기");
    gtk_window_set_default_size(GTK_WINDOW(app->window), 320, 640);
    gtk_window_set_resizable(GTK_WINDOW(app->window), FALSE);

    // Main box
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_start(main_box, 10);
    gtk_widget_set_margin_end(main_box, 10);
    gtk_widget_set_margin_top(main_box, 10);
    gtk_widget_set_margin_bottom(main_box, 10);
    gtk_container_add(GTK_CONTAINER(app->window), main_box);

    // Title label
    GtkWidget *title_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title_label),
        "<span font='20' weight='bold'>천지인 한글 입력기</span>");
    gtk_box_pack_start(GTK_BOX(main_box), title_label, FALSE, FALSE, 0);

    // Text view (scrollable)
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scrolled_window, -1, 150);

    app->text_view = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(app->text_view), GTK_WRAP_WORD_CHAR);
    app->text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(app->text_view));

    // CSS for text view font
    GtkCssProvider *text_view_css = gtk_css_provider_new();
    gtk_css_provider_load_from_data(text_view_css,
        "textview { font-family: 'NanumGothicCoding'; font-size: 20px; }",
        -1, NULL);
    GtkStyleContext *text_view_context = gtk_widget_get_style_context(app->text_view);
    gtk_style_context_add_provider(text_view_context, GTK_STYLE_PROVIDER(text_view_css),
                                   GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(text_view_css);

    gtk_container_add(GTK_CONTAINER(scrolled_window), app->text_view);
    gtk_box_pack_start(GTK_BOX(main_box), scrolled_window, TRUE, TRUE, 0);

    // Button grid (4x3)
    GtkWidget *button_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(button_grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(button_grid), 5);
    gtk_widget_set_hexpand(button_grid, TRUE);
    gtk_widget_set_vexpand(button_grid, TRUE);
    gtk_box_pack_start(GTK_BOX(main_box), button_grid, TRUE, TRUE, 0);

    // Button positions
    int positions[12][2] = {
        {1, 3}, // 0: Center (ㅇㅁ)
        {0, 0}, {1, 0}, {2, 0}, // 1-3: Row 1 (ㅣ, ·, ㅡ)
        {0, 1}, {1, 1}, {2, 1}, // 4-6: Row 2 (ㄱㅋ, ㄴㄹ, ㄷㅌ)
        {0, 2}, {1, 2}, {2, 2}, // 7-9: Row 3 (ㅂㅍ, ㅅㅎ, ㅈㅊ)
        {0, 3}, {2, 3}  // 10-11: Row 4 (Space, Del)
    };

    // Create input buttons (0-11)
    for (int i = 0; i < 12; i++) {
        const wchar_t *wtext = get_button_text(app->state.now_mode, i);
        gchar *utf8_text = wchar_to_utf8(wtext, 20);

        app->buttons[i] = gtk_button_new_with_label(utf8_text ? utf8_text : "");
        gtk_widget_set_size_request(app->buttons[i], 100, 80);

        apply_button_font(app->buttons[i], "NanumGothicCoding", 20);

        g_object_set_data(G_OBJECT(app->buttons[i]), "button_num", GINT_TO_POINTER(i));
        g_signal_connect(app->buttons[i], "clicked", G_CALLBACK(on_button_clicked), app);

        gtk_grid_attach(GTK_GRID(button_grid), app->buttons[i],
                       positions[i][0], positions[i][1], 1, 1);

        if (utf8_text) g_free(utf8_text);
    }

    // Mode button (Row 5, Col 0)
    app->mode_button = gtk_button_new_with_label("모드");
    gtk_widget_set_size_request(app->mode_button, 100, 80);
    apply_button_font(app->mode_button, "NanumGothicCoding", 20);
    g_signal_connect(app->mode_button, "clicked", G_CALLBACK(on_mode_button_clicked), app);
    gtk_grid_attach(GTK_GRID(button_grid), app->mode_button, 0, 4, 1, 1);

    // Clear button (Row 5, Col 1)
    GtkWidget *clear_button = gtk_button_new_with_label("지우기");
    gtk_widget_set_size_request(clear_button, 100, 80);
    apply_button_font(clear_button, "NanumGothicCoding", 20);
    g_signal_connect(clear_button, "clicked", G_CALLBACK(on_clear_clicked), app);
    gtk_grid_attach(GTK_GRID(button_grid), clear_button, 1, 4, 1, 1);

    // Enter button (Row 5, Col 2)
    GtkWidget *enter_button = gtk_button_new_with_label("엔터");
    gtk_widget_set_size_request(enter_button, 100, 80);
    apply_button_font(enter_button, "NanumGothicCoding", 20);
    g_signal_connect(enter_button, "clicked", G_CALLBACK(on_enter_clicked), app);
    gtk_grid_attach(GTK_GRID(button_grid), enter_button, 2, 4, 1, 1);

    // Save button (Row 6, Col 0)
    GtkWidget *save_button = gtk_button_new_with_label("저장");
    gtk_widget_set_size_request(save_button, 100, 80);
    apply_button_font(save_button, "NanumGothicCoding", 20);
    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_clicked), app);
    gtk_grid_attach(GTK_GRID(button_grid), save_button, 0, 5, 1, 1);

    // Load button (Row 6, Col 1)
    GtkWidget *load_button = gtk_button_new_with_label("로드");
    gtk_widget_set_size_request(load_button, 100, 80);
    apply_button_font(load_button, "NanumGothicCoding", 20);
    g_signal_connect(load_button, "clicked", G_CALLBACK(on_load_clicked), app);
    gtk_grid_attach(GTK_GRID(button_grid), load_button, 1, 5, 1, 1);

    // Info label
    GtkWidget *info_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(info_label),
        "<span size='small'>천지인 한글 입력 방식 - MIT License</span>");
    gtk_label_set_justify(GTK_LABEL(info_label), GTK_JUSTIFY_CENTER);
    gtk_box_pack_start(GTK_BOX(main_box), info_label, FALSE, FALSE, 0);

    gtk_widget_show_all(app->window);
}
