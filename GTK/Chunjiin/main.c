/*
* Chunjiin Korean Input Method - GTK GUI
* Main application file
*/

#include <gtk/gtk.h>
#include <locale.h>
#include <glib.h>
#include <fontconfig/fontconfig.h>
#include "chunjiin.h"

typedef struct {
    GtkWidget *window;
    GtkWidget *text_view;
    GtkTextBuffer *text_buffer;
    GtkWidget *buttons[12];
    GtkWidget *mode_button;
    ChunjiinState state;
} AppWidgets;

// 한글 폰트 로드 함수 - FcConfig를 사용하여 폰트 등록
void load_korean_font(void) {
    // 현재 실행 파일의 경로 가져오기
    gchar *exe_path = g_file_read_link("/proc/self/exe", NULL);
    gchar *exe_dir = NULL;
    if (exe_path) {
        exe_dir = g_path_get_dirname(exe_path);
        g_free(exe_path);
    } else {
        exe_dir = g_get_current_dir();
    }

    // 폰트 파일 경로 생성
    gchar *font_path = g_build_filename(exe_dir, "Font", "NanumGothicCoding.ttf", NULL);
    gchar *font_path_bold = g_build_filename(exe_dir, "Font", "NanumGothicCoding-Bold.ttf", NULL);

    // fontconfig를 사용하여 폰트 추가
    FcConfig *config = FcConfigGetCurrent();
    FcConfigAppFontAddFile(config, (const FcChar8 *)font_path);
    FcConfigAppFontAddFile(config, (const FcChar8 *)font_path_bold);

    // 메모리 해제
    g_free(font_path);
    g_free(font_path_bold);
    g_free(exe_dir);
}


// 버튼 레이블용 간단한 폰트 설정 (GTK 기본 메커니즘 사용)
void apply_button_font(GtkWidget *button, const gchar *font_name, gint font_size) {
    gchar *css = g_strdup_printf(
        "label { font-family: '%s'; font-size: %dpt; }",
        font_name, font_size);

    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, css, -1, NULL);

    GtkStyleContext *context = gtk_widget_get_style_context(button);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    g_object_unref(provider);
    g_free(css);
}

// wchar_t 버퍼를 UTF-8 문자열로 변환하는 헬퍼 함수
// Linux에서 wchar_t는 UTF-32 (4바이트)이므로 직접 변환 필요
gchar* wchar_to_utf8(const wchar_t *wstr, size_t max_len) {
    if (wstr == NULL) {
        return g_strdup("");
    }

    GString *str = g_string_new(NULL);
    for (size_t i = 0; i < max_len && wstr[i] != 0; i++) {
        gunichar uc = (gunichar)wstr[i];
        gchar buf[7];
        gint written = g_unichar_to_utf8(uc, buf);
        if (written > 0) {
            g_string_append_len(str, buf, written);
        }
    }
    return g_string_free(str, FALSE);
}

// 버튼 클릭 핸들러
void on_button_clicked(GtkWidget *widget, gpointer data) {
    AppWidgets *app = (AppWidgets *)data;
    int button_num = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(widget), "button_num"));

    // 입력 처리
    chunjiin_process_input(&app->state, button_num);

    // 텍스트 뷰 업데이트
    gchar *utf8_text = wchar_to_utf8(app->state.text_buffer, MAX_TEXT_LEN);
    gtk_text_buffer_set_text(app->text_buffer, utf8_text, -1);
    g_free(utf8_text);

    // 커서를 끝으로 이동
    GtkTextIter end;
    gtk_text_buffer_get_end_iter(app->text_buffer, &end);
    gtk_text_buffer_place_cursor(app->text_buffer, &end);
}

// 모드 변경 버튼 핸들러
void on_mode_button_clicked(GtkWidget *widget __attribute__((unused)), gpointer data) {
    AppWidgets *app = (AppWidgets *)data;

    change_mode(&app->state);

    // 버튼 텍스트 업데이트
    for (int i = 0; i < 12; i++) {
        if (app->buttons[i] == NULL) continue;
        const wchar_t *wtext = get_button_text(app->state.now_mode, i);
        gchar *utf8_text = wchar_to_utf8(wtext, 20);
        gtk_button_set_label(GTK_BUTTON(app->buttons[i]), utf8_text);

        // 폰트 재적용 (레이블이 새로 생성될 수 있으므로)
        apply_button_font(app->buttons[i], "NanumGothicCoding", 20);

        g_free(utf8_text);
    }
}

// 텍스트 지우기 버튼
void on_clear_clicked(GtkWidget *widget __attribute__((unused)), gpointer data) {
    AppWidgets *app = (AppWidgets *)data;
    
    // 현재 모드 저장
    InputMode current_mode = app->state.now_mode;
    
    // 텍스트 지우기 (모드는 보존)
    chunjiin_init(&app->state);
    app->state.now_mode = current_mode;  // 모드 복원
    gtk_text_buffer_set_text(app->text_buffer, "", -1);
}

// 엔터 버튼 핸들러 - 결과 팝업 표시 후 텍스트 지우기
void on_enter_clicked(GtkWidget *widget __attribute__((unused)), gpointer data) {
    AppWidgets *app = (AppWidgets *)data;

    // 현재 모드 저장
    InputMode current_mode = app->state.now_mode;

    // 현재 텍스트 가져오기
    GtkTextIter start, end;
    gtk_text_buffer_get_bounds(app->text_buffer, &start, &end);
    gchar *text = gtk_text_buffer_get_text(app->text_buffer, &start, &end, FALSE);

    // 팝업 다이얼로그 생성
    GtkWidget *dialog = gtk_dialog_new_with_buttons("입력 결과",
                                                   GTK_WINDOW(app->window),
                                                   GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   "확인", GTK_RESPONSE_OK,
                                                   NULL);

    // 다이얼로그 크기 설정
    gtk_window_set_default_size(GTK_WINDOW(dialog), 400, 200);

    // 메시지 레이블 생성
    GtkWidget *content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *label = gtk_label_new(text);
    gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_selectable(GTK_LABEL(label), TRUE);  // 텍스트 선택 가능

    // 레이블에 CSS 클래스 적용
    GtkStyleContext *context = gtk_widget_get_style_context(label);
    gtk_style_context_add_class(context, "dialog-label");

    // 여백 설정
    gtk_widget_set_margin_start(label, 20);
    gtk_widget_set_margin_end(label, 20);
    gtk_widget_set_margin_top(label, 20);
    gtk_widget_set_margin_bottom(label, 20);

    gtk_container_add(GTK_CONTAINER(content_area), label);

    // 다이얼로그 표시
    gtk_widget_show_all(dialog);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    // 메모리 해제
    g_free(text);

    // 텍스트 지우기 (모드는 보존)
    chunjiin_init(&app->state);
    app->state.now_mode = current_mode;  // 모드 복원
    gtk_text_buffer_set_text(app->text_buffer, "", -1);
}

// 저장 버튼 핸들러
void on_save_clicked(GtkWidget *widget __attribute__((unused)), gpointer data) {
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

// 로드 버튼 핸들러
void on_load_clicked(GtkWidget *widget __attribute__((unused)), gpointer data) {
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

            // 텍스트 뷰 업데이트
            gchar *utf8_text = wchar_to_utf8(app->state.text_buffer, MAX_TEXT_LEN);
            gtk_text_buffer_set_text(app->text_buffer, utf8_text, -1);
            g_free(utf8_text);

            // 커서를 끝으로 이동
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

void activate(GtkApplication *app_gtk, gpointer user_data __attribute__((unused))) {
    // 한글 폰트 로드
    load_korean_font();

    AppWidgets *app = g_new0(AppWidgets, 1);
    chunjiin_init(&app->state);

    // 메인 윈도우 생성
    app->window = gtk_application_window_new(app_gtk);
    gtk_window_set_title(GTK_WINDOW(app->window), "천지인 한글 입력기");
    gtk_window_set_default_size(GTK_WINDOW(app->window), 320, 640);
    gtk_window_set_resizable(GTK_WINDOW(app->window), FALSE);

    // 메인 박스
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_start(main_box, 10);
    gtk_widget_set_margin_end(main_box, 10);
    gtk_widget_set_margin_top(main_box, 10);
    gtk_widget_set_margin_bottom(main_box, 10);
    gtk_container_add(GTK_CONTAINER(app->window), main_box);

    // 제목 레이블
    GtkWidget *title_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title_label),
        "<span font='20' weight='bold'>천지인 한글 입력기</span>");
    gtk_box_pack_start(GTK_BOX(main_box), title_label, FALSE, FALSE, 0);

    // 텍스트 뷰 (스크롤 가능)
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(scrolled_window, -1, 150);

    app->text_view = gtk_text_view_new();
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(app->text_view), GTK_WRAP_WORD_CHAR);
    app->text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(app->text_view));

    // CSS를 사용하여 텍스트 뷰 폰트 설정
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

    // 버튼 그리드 (4x3)
    GtkWidget *button_grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(button_grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(button_grid), 5);
    gtk_widget_set_hexpand(button_grid, TRUE);
    gtk_widget_set_vexpand(button_grid, TRUE);
    gtk_box_pack_start(GTK_BOX(main_box), button_grid, TRUE, TRUE, 0);

    // 버튼 배치: 각 행에 3개씩
    // 1행: 천(1), 지(2), 인(3)
    // 2행: ㄱ(4), ㄴ(5), ㄷ(6)
    // 3행: ㅂ(7), ㅅ(8), ㅈ(9)
    // 4행: 공백(10), ㅇㅁ(0), 삭제(11)
    // 5행: 전체지우기 (3칸 너비)
    int positions[12][2] = {
        {1, 3}, // 0: 넷째 행 중앙 (ㅇㅁ)
        {0, 0}, {1, 0}, {2, 0}, // 1-3: 첫째 행 (천, 지, 인)
        {0, 1}, {1, 1}, {2, 1}, // 4-6: 둘째 행 (ㄱ, ㄴ, ㄷ)
        {0, 2}, {1, 2}, {2, 2}, // 7-9: 셋째 행 (ㅂ, ㅅ, ㅈ)
        {0, 3}, {2, 3}  // 10-11: 넷째 행 (Space, Del)
    };

    for (int i = 0; i < 12; i++) {
        const wchar_t *wtext = get_button_text(app->state.now_mode, i);
        gchar *utf8_text = wchar_to_utf8(wtext, 20);

        app->buttons[i] = gtk_button_new_with_label(utf8_text ? utf8_text : "");
        gtk_widget_set_size_request(app->buttons[i], 100, 80);

        // CSS를 사용한 폰트 적용 (버튼 레이블)
        apply_button_font(app->buttons[i], "NanumGothicCoding", 20);

        // 버튼에 번호 저장
        g_object_set_data(G_OBJECT(app->buttons[i]), "button_num", GINT_TO_POINTER(i));
        g_signal_connect(app->buttons[i], "clicked", G_CALLBACK(on_button_clicked), app);

        gtk_grid_attach(GTK_GRID(button_grid), app->buttons[i],
                       positions[i][0], positions[i][1], 1, 1);

        if (utf8_text) g_free(utf8_text);
    }

    // 5번째 행에 3개의 버튼 추가: 모드, 지우기, 엔터

    // 모드 변경 버튼
    app->mode_button = gtk_button_new_with_label("모드");
    gtk_widget_set_size_request(app->mode_button, 100, 80);
    apply_button_font(app->mode_button, "NanumGothicCoding", 20);
    g_signal_connect(app->mode_button, "clicked", G_CALLBACK(on_mode_button_clicked), app);
    gtk_grid_attach(GTK_GRID(button_grid), app->mode_button, 0, 4, 1, 1);

    // 지우기 버튼
    GtkWidget *clear_button = gtk_button_new_with_label("지우기");
    gtk_widget_set_size_request(clear_button, 100, 80);
    apply_button_font(clear_button, "NanumGothicCoding", 20);
    g_signal_connect(clear_button, "clicked", G_CALLBACK(on_clear_clicked), app);
    gtk_grid_attach(GTK_GRID(button_grid), clear_button, 1, 4, 1, 1);

    // 엔터 버튼
    GtkWidget *enter_button = gtk_button_new_with_label("엔터");
    gtk_widget_set_size_request(enter_button, 100, 80);
    apply_button_font(enter_button, "NanumGothicCoding", 20);
    g_signal_connect(enter_button, "clicked", G_CALLBACK(on_enter_clicked), app);
    gtk_grid_attach(GTK_GRID(button_grid), enter_button, 2, 4, 1, 1);

    // 6번째 행에 3개의 버튼 추가: 저장, 로드, (빈칸 또는 추가 버튼)

    // 저장 버튼
    GtkWidget *save_button = gtk_button_new_with_label("저장");
    gtk_widget_set_size_request(save_button, 100, 80);
    apply_button_font(save_button, "NanumGothicCoding", 20);
    g_signal_connect(save_button, "clicked", G_CALLBACK(on_save_clicked), app);
    gtk_grid_attach(GTK_GRID(button_grid), save_button, 0, 5, 1, 1);

    // 로드 버튼
    GtkWidget *load_button = gtk_button_new_with_label("로드");
    gtk_widget_set_size_request(load_button, 100, 80);
    apply_button_font(load_button, "NanumGothicCoding", 20);
    g_signal_connect(load_button, "clicked", G_CALLBACK(on_load_clicked), app);
    gtk_grid_attach(GTK_GRID(button_grid), load_button, 1, 5, 1, 1);

    // 정보 레이블
    GtkWidget *info_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(info_label),
        "<span size='small'>천지인 한글 입력 방식 - MIT License</span>");
    gtk_label_set_justify(GTK_LABEL(info_label), GTK_JUSTIFY_CENTER);
    gtk_box_pack_start(GTK_BOX(main_box), info_label, FALSE, FALSE, 0);

    gtk_widget_show_all(app->window);
}

int main(int argc, char **argv) {
    // 로케일 설정
    setlocale(LC_ALL, "");

    GtkApplication *app = gtk_application_new("com.personal.chunjiin", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
