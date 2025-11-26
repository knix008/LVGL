#include "gui_utils.h"
#include <glib.h>
#include <fontconfig/fontconfig.h>
#include <wchar.h>

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

// Get button text based on input mode
const wchar_t* get_button_text(InputMode mode, int button_num) {
    static const wchar_t *hangul_texts[] = {
        L"ㅇㅁ", L"ㅣ", L"·", L"ㅡ", L"ㄱㅋ",
        L"ㄴㄹ", L"ㄷㅌ", L"ㅂㅍ", L"ㅅㅎ", L"ㅈㅊ",
        L"Space", L"Del"
    };
    static const wchar_t *upper_eng_texts[] = {
        L"@?!", L"ABC", L"DEF", L"GHI", L"JKL",
        L"MNO", L"PQR", L"STU", L"VWX", L"YZ.",
        L"Space", L"Del"
    };
    static const wchar_t *lower_eng_texts[] = {
        L"@?!", L"abc", L"def", L"ghi", L"jkl",
        L"mno", L"pqr", L"stu", L"vwx", L"yz.",
        L"Space", L"Del"
    };
    static const wchar_t *number_texts[] = {
        L"0", L"1", L"2", L"3", L"4",
        L"5", L"6", L"7", L"8", L"9",
        L"Space", L"Del"
    };
    static const wchar_t *special_texts[] = {
        L"~`^", L"!@#", L"$%&", L"*()=", L"+{}",
        L"[]=", L"<>|", L"-_", L":;", L"\"'/",
        L"Space", L"Del"
    };

    if (button_num < 0 || button_num > 11) return L"";

    switch (mode) {
        case MODE_HANGUL: return hangul_texts[button_num];
        case MODE_UPPER_ENGLISH: return upper_eng_texts[button_num];
        case MODE_ENGLISH: return lower_eng_texts[button_num];
        case MODE_NUMBER: return number_texts[button_num];
        case MODE_SPECIAL: return special_texts[button_num];
        default: return L"";
    }
}
