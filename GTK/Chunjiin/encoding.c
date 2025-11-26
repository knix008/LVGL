#include "encoding.h"

// wchar_t 버퍼를 UTF-8 문자열로 변환하는 함수
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
