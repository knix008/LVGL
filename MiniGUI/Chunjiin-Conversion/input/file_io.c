/*
 * File I/O Module for Chunjiin Korean Input Method
 * Handles saving and loading input text to/from files
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <stdint.h>
#include "file_io.h"

// Helper: Convert wchar_t to UTF-8
static size_t wchar_to_utf8_buffer(const wchar_t *wstr, char *utf8_str, size_t max_len) {
    if (wstr == NULL || utf8_str == NULL) return 0;

    size_t i = 0, j = 0;
    while (wstr[i] != 0 && j < max_len - 4) {
        uint32_t uc = (uint32_t)wstr[i];

        if (uc < 0x80) {
            utf8_str[j++] = (char)uc;
        } else if (uc < 0x800) {
            utf8_str[j++] = (char)(0xC0 | (uc >> 6));
            utf8_str[j++] = (char)(0x80 | (uc & 0x3F));
        } else if (uc < 0x10000) {
            utf8_str[j++] = (char)(0xE0 | (uc >> 12));
            utf8_str[j++] = (char)(0x80 | ((uc >> 6) & 0x3F));
            utf8_str[j++] = (char)(0x80 | (uc & 0x3F));
        } else {
            utf8_str[j++] = (char)(0xF0 | (uc >> 18));
            utf8_str[j++] = (char)(0x80 | ((uc >> 12) & 0x3F));
            utf8_str[j++] = (char)(0x80 | ((uc >> 6) & 0x3F));
            utf8_str[j++] = (char)(0x80 | (uc & 0x3F));
        }
        i++;
    }
    utf8_str[j] = '\0';
    return j;
}

// Helper: Convert UTF-8 to wchar_t
static size_t utf8_to_wchar_buffer(const char *utf8_str, wchar_t *wstr, size_t max_len) {
    if (utf8_str == NULL || wstr == NULL) return 0;

    size_t i = 0, j = 0;
    while (utf8_str[i] != '\0' && j < max_len - 1) {
        unsigned char c = (unsigned char)utf8_str[i];

        if (c < 0x80) {
            wstr[j++] = (wchar_t)c;
            i++;
        } else if ((c & 0xE0) == 0xC0 && utf8_str[i + 1] != '\0') {
            uint32_t codepoint = ((uint32_t)(c & 0x1F) << 6) |
                                 ((uint32_t)(utf8_str[i + 1] & 0x3F));
            wstr[j++] = (wchar_t)codepoint;
            i += 2;
        } else if ((c & 0xF0) == 0xE0 && utf8_str[i + 2] != '\0') {
            uint32_t codepoint = ((uint32_t)(c & 0x0F) << 12) |
                                 ((uint32_t)(utf8_str[i + 1] & 0x3F) << 6) |
                                 ((uint32_t)(utf8_str[i + 2] & 0x3F));
            wstr[j++] = (wchar_t)codepoint;
            i += 3;
        } else if ((c & 0xF8) == 0xF0 && utf8_str[i + 3] != '\0') {
            uint32_t codepoint = ((uint32_t)(c & 0x07) << 18) |
                                 ((uint32_t)(utf8_str[i + 1] & 0x3F) << 12) |
                                 ((uint32_t)(utf8_str[i + 2] & 0x3F) << 6) |
                                 ((uint32_t)(utf8_str[i + 3] & 0x3F));
            if (codepoint <= 0xFFFF) {
                wstr[j++] = (wchar_t)codepoint;
            }
            i += 4;
        } else {
            i++;
        }
    }
    wstr[j] = '\0';
    return j;
}

// Save input text to file
int file_save_input(const ChunjiinState *state, const char *filename) {
    if (!state || !filename) {
        printf("file_save_input: Invalid parameters\n");
        return 0;
    }

    FILE *file = fopen(filename, "w");
    if (!file) {
        printf("file_save_input: Cannot open file %s\n", filename);
        return 0;
    }

    // Convert wchar_t text_buffer to UTF-8
    char utf8_buffer[MAX_TEXT_LEN * 4] = {0};
    wchar_to_utf8_buffer(state->text_buffer, utf8_buffer, sizeof(utf8_buffer));

    // Write UTF-8 text to file
    size_t written = fwrite(utf8_buffer, 1, strlen(utf8_buffer), file);
    fclose(file);

    printf("file_save_input: Saved %zu bytes to %s\n", written, filename);
    return (written > 0) ? 1 : 0;
}

// Load input text from file
int file_load_input(ChunjiinState *state, const char *filename) {
    if (!state || !filename) {
        printf("file_load_input: Invalid parameters\n");
        return 0;
    }

    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("file_load_input: Cannot open file %s\n", filename);
        return 0;
    }

    // Read file content as UTF-8
    char utf8_buffer[MAX_TEXT_LEN * 4] = {0};
    size_t read = fread(utf8_buffer, 1, sizeof(utf8_buffer) - 1, file);
    fclose(file);

    if (read == 0) {
        printf("file_load_input: File is empty\n");
        wmemset(state->text_buffer, 0, MAX_TEXT_LEN);
        state->cursor_pos = 0;
        return 1;
    }

    utf8_buffer[read] = '\0';

    // Clear the buffer first
    wmemset(state->text_buffer, 0, MAX_TEXT_LEN);

    // Convert UTF-8 to wchar_t
    utf8_to_wchar_buffer(utf8_buffer, state->text_buffer, MAX_TEXT_LEN);
    state->cursor_pos = wcslen(state->text_buffer);

    printf("file_load_input: Loaded %zu bytes from %s, cursor_pos=%d\n", read, filename, state->cursor_pos);
    return 1;
}
