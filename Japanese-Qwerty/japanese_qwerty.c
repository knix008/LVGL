/*
 * japanese_qwerty.c
 * Japanese Input Method Editor - Implementation
 */

#include "japanese_qwerty.h"
#include <string.h>
#include <stdio.h>

// Romaji to Kana conversion table (sorted by length for longest match first)
static const RomajiMap romaji_table[] = {
    // 3-character combinations
    {"kya", "きゃ", "キャ"},
    {"kyu", "きゅ", "キュ"},
    {"kyo", "きょ", "キョ"},
    {"sha", "しゃ", "シャ"},
    {"shu", "しゅ", "シュ"},
    {"sho", "しょ", "ショ"},
    {"cha", "ちゃ", "チャ"},
    {"chu", "ちゅ", "チュ"},
    {"cho", "ちょ", "チョ"},
    {"nya", "にゃ", "ニャ"},
    {"nyu", "にゅ", "ニュ"},
    {"nyo", "にょ", "ニョ"},
    {"hya", "ひゃ", "ヒャ"},
    {"hyu", "ひゅ", "ヒュ"},
    {"hyo", "ひょ", "ヒョ"},
    {"mya", "みゃ", "ミャ"},
    {"myu", "みゅ", "ミュ"},
    {"myo", "みょ", "ミョ"},
    {"rya", "りゃ", "リャ"},
    {"ryu", "りゅ", "リュ"},
    {"ryo", "りょ", "リョ"},
    {"gya", "ぎゃ", "ギャ"},
    {"gyu", "ぎゅ", "ギュ"},
    {"gyo", "ぎょ", "ギョ"},
    {"zya", "じゃ", "ジャ"},
    {"jya", "じゃ", "ジャ"},
    {"zyu", "じゅ", "ジュ"},
    {"jyu", "じゅ", "ジュ"},
    {"zyo", "じょ", "ジョ"},
    {"jyo", "じょ", "ジョ"},
    {"bya", "びゃ", "ビャ"},
    {"byu", "びゅ", "ビュ"},
    {"byo", "びょ", "ビョ"},
    {"pya", "ぴゃ", "ピャ"},
    {"pyu", "ぴゅ", "ピュ"},
    {"pyo", "ぴょ", "ピョ"},
    {"tsu", "つ", "ツ"},
    {"shi", "し", "シ"},
    {"chi", "ち", "チ"},
    {"thi", "てぃ", "ティ"},
    {"dhi", "でぃ", "ディ"},
    
    // 2-character combinations
    {"ka", "か", "カ"},
    {"ki", "き", "キ"},
    {"ku", "く", "ク"},
    {"ke", "け", "ケ"},
    {"ko", "こ", "コ"},
    {"sa", "さ", "サ"},
    {"si", "し", "シ"},
    {"su", "す", "ス"},
    {"se", "せ", "セ"},
    {"so", "そ", "ソ"},
    {"ta", "た", "タ"},
    {"ti", "ち", "チ"},
    {"tu", "つ", "ツ"},
    {"te", "て", "テ"},
    {"to", "と", "ト"},
    {"na", "な", "ナ"},
    {"ni", "に", "ニ"},
    {"nu", "ぬ", "ヌ"},
    {"ne", "ね", "ネ"},
    {"no", "の", "ノ"},
    {"ha", "は", "ハ"},
    {"hi", "ひ", "ヒ"},
    {"hu", "ふ", "フ"},
    {"fu", "ふ", "フ"},
    {"he", "へ", "ヘ"},
    {"ho", "ほ", "ホ"},
    {"ma", "ま", "マ"},
    {"mi", "み", "ミ"},
    {"mu", "む", "ム"},
    {"me", "め", "メ"},
    {"mo", "も", "モ"},
    {"ya", "や", "ヤ"},
    {"yu", "ゆ", "ユ"},
    {"yo", "よ", "ヨ"},
    {"ra", "ら", "ラ"},
    {"ri", "り", "リ"},
    {"ru", "る", "ル"},
    {"re", "れ", "レ"},
    {"ro", "ろ", "ロ"},
    {"wa", "わ", "ワ"},
    {"wi", "うぃ", "ウィ"},
    {"we", "うぇ", "ウェ"},
    {"wo", "を", "ヲ"},
    {"ga", "が", "ガ"},
    {"gi", "ぎ", "ギ"},
    {"gu", "ぐ", "グ"},
    {"ge", "げ", "ゲ"},
    {"go", "ご", "ゴ"},
    {"za", "ざ", "ザ"},
    {"zi", "じ", "ジ"},
    {"zu", "ず", "ズ"},
    {"ze", "ぜ", "ゼ"},
    {"zo", "ぞ", "ゾ"},
    {"ja", "じゃ", "ジャ"},
    {"ji", "じ", "ジ"},
    {"ju", "じゅ", "ジュ"},
    {"je", "じぇ", "ジェ"},
    {"jo", "じょ", "ジョ"},
    {"da", "だ", "ダ"},
    {"di", "ぢ", "ヂ"},
    {"du", "づ", "ヅ"},
    {"de", "で", "デ"},
    {"do", "ど", "ド"},
    {"ba", "ば", "バ"},
    {"bi", "び", "ビ"},
    {"bu", "ぶ", "ブ"},
    {"be", "べ", "ベ"},
    {"bo", "ぼ", "ボ"},
    {"pa", "ぱ", "パ"},
    {"pi", "ぴ", "ピ"},
    {"pu", "ぷ", "プ"},
    {"pe", "ぺ", "ペ"},
    {"po", "ぽ", "ポ"},
    {"nn", "ん", "ン"},
    
    // Single vowels
    {"a", "あ", "ア"},
    {"i", "い", "イ"},
    {"u", "う", "ウ"},
    {"e", "え", "エ"},
    {"o", "お", "オ"},
    {"n", "ん", "ン"},
    
    // End marker
    {NULL, NULL, NULL}
};

void ime_init(IMEState *state) {
    memset(state, 0, sizeof(IMEState));
    state->mode = MODE_HIRAGANA;
}

void ime_set_mode(IMEState *state, InputMode mode) {
    state->mode = mode;
    // Re-convert buffer with new mode
    if (state->buffer_pos > 0) {
        convert_buffer_to_kana(state);
    }
}

const char *ime_get_mode_string(IMEState *state) {
    switch (state->mode) {
        case MODE_HIRAGANA:
            return "ひらがな";
        case MODE_KATAKANA:
            return "カタカナ";
        case MODE_ENGLISH:
            return "English";
        default:
            return "Unknown";
    }
}

const RomajiMap *find_romaji_match(const char *str, int len) {
    // Try matching from longest to shortest
    for (int try_len = (len > 3 ? 3 : len); try_len > 0; try_len--) {
        for (int i = 0; romaji_table[i].romaji != NULL; i++) {
            if (strlen(romaji_table[i].romaji) == (size_t)try_len &&
                strncmp(str, romaji_table[i].romaji, try_len) == 0) {
                return &romaji_table[i];
            }
        }
    }
    return NULL;
}

void convert_buffer_to_kana(IMEState *state) {
    char temp_output[512] = {0};
    int out_pos = 0;
    int i = 0;
    
    if (state->mode == MODE_ENGLISH) {
        // In English mode, just copy buffer as-is
        strncpy(temp_output, state->buffer, sizeof(temp_output) - 1);
        out_pos = state->buffer_pos;
    } else {
        // Convert romaji to kana
        while (i < state->buffer_pos) {
            // Check for small tsu (double consonants)
            if (i + 1 < state->buffer_pos &&
                state->buffer[i] == state->buffer[i + 1] &&
                state->buffer[i] != 'n' && state->buffer[i] != 'a' &&
                state->buffer[i] != 'i' && state->buffer[i] != 'u' &&
                state->buffer[i] != 'e' && state->buffer[i] != 'o') {
                
                if (state->mode == MODE_HIRAGANA) {
                    strcpy(temp_output + out_pos, "っ");
                    out_pos += strlen("っ");
                } else {
                    strcpy(temp_output + out_pos, "ッ");
                    out_pos += strlen("ッ");
                }
                i++;
                continue;
            }
            
            // Try to match romaji
            const RomajiMap *match = find_romaji_match(state->buffer + i, state->buffer_pos - i);
            
            if (match) {
                const char *kana = (state->mode == MODE_HIRAGANA) ? match->hiragana : match->katakana;
                strcpy(temp_output + out_pos, kana);
                out_pos += strlen(kana);
                i += strlen(match->romaji);
            } else {
                // No match, keep the character as-is
                temp_output[out_pos++] = state->buffer[i];
                i++;
            }
        }
    }
    
    temp_output[out_pos] = '\0';
    strncpy(state->output, temp_output, sizeof(state->output) - 1);
    state->output[sizeof(state->output) - 1] = '\0';  // Ensure null termination
    state->output_pos = out_pos;
}

void ime_process_char(IMEState *state, char c) {
    // Only accept lowercase letters and hyphen
    if ((c >= 'a' && c <= 'z') || c == '-') {
        if (state->buffer_pos < (int)sizeof(state->buffer) - 1) {
            if (c == '-') {
                // Add prolonged sound mark
                if (state->mode == MODE_HIRAGANA) {
                    strcat(state->display, "ー");
                    state->display_pos += strlen("ー");
                } else if (state->mode == MODE_KATAKANA) {
                    strcat(state->display, "ー");
                    state->display_pos += strlen("ー");
                } else {
                    state->display[state->display_pos++] = '-';
                }
            } else {
                state->buffer[state->buffer_pos++] = c;
                state->buffer[state->buffer_pos] = '\0';
                convert_buffer_to_kana(state);
            }
        }
    }
}

void ime_process_backspace(IMEState *state) {
    if (state->buffer_pos > 0) {
        state->buffer_pos--;
        state->buffer[state->buffer_pos] = '\0';
        convert_buffer_to_kana(state);
    } else if (state->display_pos > 0) {
        // Remove last character from display (handle UTF-8)
        int pos = state->display_pos - 1;
        // Skip UTF-8 continuation bytes
        while (pos > 0 && (state->display[pos] & 0xC0) == 0x80) {
            pos--;
        }
        state->display[pos] = '\0';
        state->display_pos = pos;
    }
}

void ime_process_enter(IMEState *state) {
    // Commit current buffer to display
    if (state->output_pos > 0) {
        int remaining = (int)sizeof(state->display) - state->display_pos - 1;
        if (remaining > 0) {
            strncat(state->display, state->output, remaining);
            state->display_pos = strlen(state->display);
        }
        state->buffer_pos = 0;
        state->output_pos = 0;
        state->buffer[0] = '\0';
        state->output[0] = '\0';
    }
    // Add newline
    if (state->display_pos < (int)sizeof(state->display) - 1) {
        state->display[state->display_pos++] = '\n';
        state->display[state->display_pos] = '\0';
    }
}

void ime_process_space(IMEState *state) {
    // Commit current buffer to display
    if (state->output_pos > 0) {
        int remaining = (int)sizeof(state->display) - state->display_pos - 1;
        if (remaining > 0) {
            strncat(state->display, state->output, remaining);
            state->display_pos = strlen(state->display);
        }
        state->buffer_pos = 0;
        state->output_pos = 0;
        state->buffer[0] = '\0';
        state->output[0] = '\0';
    }
    // Add space
    if (state->display_pos < (int)sizeof(state->display) - 1) {
        state->display[state->display_pos++] = ' ';
        state->display[state->display_pos] = '\0';
    }
}

void ime_clear(IMEState *state) {
    state->buffer_pos = 0;
    state->output_pos = 0;
    state->display_pos = 0;
    state->buffer[0] = '\0';
    state->output[0] = '\0';
    state->display[0] = '\0';
}

const char *ime_get_display_text(IMEState *state) {
    static char full_text[1536];  // Increased size to accommodate display + output
    int written = snprintf(full_text, sizeof(full_text), "%s%s", state->display, state->output);
    if (written >= (int)sizeof(full_text)) {
        full_text[sizeof(full_text) - 1] = '\0';  // Ensure null termination
    }
    return full_text;
}

