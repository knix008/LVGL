#ifndef FILE_H
#define FILE_H

#include <wchar.h>
#include <stddef.h>

// 파일 저장/로드 함수
int save_text_to_file(const wchar_t *text, const char *filename);
int load_text_from_file(wchar_t *text, size_t max_len, const char *filename);

#endif // FILE_H
