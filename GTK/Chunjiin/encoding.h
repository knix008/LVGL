#ifndef ENCODING_H
#define ENCODING_H

#include <wchar.h>
#include <glib.h>

// 문자 인코딩 변환 함수
gchar* wchar_to_utf8(const wchar_t *wstr, size_t max_len);

#endif // ENCODING_H
