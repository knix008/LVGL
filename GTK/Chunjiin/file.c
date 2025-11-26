#include "file.h"
#include "chunjiin.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int save_text_to_file(const wchar_t *text, const char *filename) {
    if (text == NULL || filename == NULL) return -1;

    FILE *file = fopen(filename, "w");
    if (file == NULL) return -1;

    // wchar_t를 UTF-8로 변환하여 파일에 저장
    for (int i = 0; i < MAX_TEXT_LEN && text[i] != 0; i++) {
        char utf8_char[5] = {0};
        wchar_t wc = text[i];

        if (wc < 0x80) {
            utf8_char[0] = (char)wc;
        } else if (wc < 0x800) {
            utf8_char[0] = 0xC0 | (wc >> 6);
            utf8_char[1] = 0x80 | (wc & 0x3F);
        } else if (wc < 0x10000) {
            utf8_char[0] = 0xE0 | (wc >> 12);
            utf8_char[1] = 0x80 | ((wc >> 6) & 0x3F);
            utf8_char[2] = 0x80 | (wc & 0x3F);
        } else {
            utf8_char[0] = 0xF0 | (wc >> 18);
            utf8_char[1] = 0x80 | ((wc >> 12) & 0x3F);
            utf8_char[2] = 0x80 | ((wc >> 6) & 0x3F);
            utf8_char[3] = 0x80 | (wc & 0x3F);
        }

        fputs(utf8_char, file);
    }

    fclose(file);
    return 0;
}

int load_text_from_file(wchar_t *text, size_t max_len, const char *filename) {
    if (text == NULL || filename == NULL) return -1;

    FILE *file = fopen(filename, "r");
    if (file == NULL) return -1;

    memset(text, 0, max_len * sizeof(wchar_t));
    int pos = 0;

    // UTF-8에서 wchar_t로 변환하여 읽기
    int byte;
    while ((byte = fgetc(file)) != EOF && pos < (int)max_len - 1) {
        wchar_t wc = 0;

        if ((byte & 0x80) == 0) {
            // 1바이트 문자
            wc = byte;
        } else if ((byte & 0xE0) == 0xC0) {
            // 2바이트 문자
            wc = (byte & 0x1F) << 6;
            byte = fgetc(file);
            if ((byte & 0xC0) == 0x80) {
                wc |= (byte & 0x3F);
            } else {
                fclose(file);
                return -1;
            }
        } else if ((byte & 0xF0) == 0xE0) {
            // 3바이트 문자
            wc = (byte & 0x0F) << 12;
            byte = fgetc(file);
            if ((byte & 0xC0) == 0x80) {
                wc |= (byte & 0x3F) << 6;
                byte = fgetc(file);
                if ((byte & 0xC0) == 0x80) {
                    wc |= (byte & 0x3F);
                } else {
                    fclose(file);
                    return -1;
                }
            } else {
                fclose(file);
                return -1;
            }
        } else if ((byte & 0xF8) == 0xF0) {
            // 4바이트 문자
            wc = (byte & 0x07) << 18;
            byte = fgetc(file);
            if ((byte & 0xC0) == 0x80) {
                wc |= (byte & 0x3F) << 12;
                byte = fgetc(file);
                if ((byte & 0xC0) == 0x80) {
                    wc |= (byte & 0x3F) << 6;
                    byte = fgetc(file);
                    if ((byte & 0xC0) == 0x80) {
                        wc |= (byte & 0x3F);
                    } else {
                        fclose(file);
                        return -1;
                    }
                } else {
                    fclose(file);
                    return -1;
                }
            } else {
                fclose(file);
                return -1;
            }
        }

        text[pos++] = wc;
    }

    fclose(file);
    return 0;
}
