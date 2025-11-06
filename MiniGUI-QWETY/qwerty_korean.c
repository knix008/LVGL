
// 한글 입력기 (C IME) 전체 코드: 실시간 조합, 복합 초/중/종성, 백스페이스, 공백, 다중 글자 지원
#include "qwerty_korean.h"
#include <stdlib.h>

KeyMap cho_keymap[] = {
    {"R", "ㄲ"}, {"r", "ㄱ"}, {"S", "ㄴ"}, {"s", "ㄴ"}, {"E", "ㄸ"},
    {"e", "ㄷ"},  {"F", "ㄹ"}, {"f", "ㄹ"}, {"A", "ㅁ"}, {"a", "ㅁ"}, {"Q", "ㅃ"}, 
    {"q", "ㅂ"},  {"T", "ㅆ"}, {"t", "ㅅ"}, {"D", "ㅇ"}, {"d", "ㅇ"},
    {"W", "ㅉ"}, {"w", "ㅈ"}, {"C", "ㅊ"}, {"c", "ㅊ"}, {"Z", "ㅋ"}, {"z", "ㅋ"},
    {"X", "ㅌ"}, {"x", "ㅌ"}, {"V", "ㅍ"}, {"v", "ㅍ"}, {"G", "ㅎ"}, {"g", "ㅎ"}
};

KeyMap jung_keymap[] = {
    {"hk", "ㅘ"}, {"ho", "ㅙ"}, {"hl", "ㅚ"}, {"np", "ㅞ"},
    {"nj", "ㅝ"}, {"np", "ㅞ"}, {"nj", "ㅝ"}, {"nl", "ㅟ"}, 
    {"y", "ㅛ"}, {"n", "ㅜ"}, {"k", "ㅏ"}, {"o", "ㅐ"}, {"i", "ㅑ"}, 
    {"O", "ㅒ"}, {"j", "ㅓ"}, {"p", "ㅔ"}, {"u", "ㅕ"}, 
    {"P", "ㅖ"}, {"h", "ㅗ"}, {"y", "ㅛ"}, {"n", "ㅜ"}, 
    {"b", "ㅠ"}, {"l", "ㅣ"}, {"m", "ㅡ"}  
};

KeyMap jong_keymap[] = {
    {"", ""}, {"rt", "ㄳ"}, {"sw", "ㄵ"}, {"sg", "ㄶ"}, {"fr", "ㄺ"},
    {"fa", "ㄻ"}, {"fq", "ㄼ"}, {"ft", "ㄽ"}, {"fx", "ㄾ"}, {"fv", "ㄿ"},
    {"fg", "ㅀ"}, {"qt", "ㅄ"}, {"R", "ㄲ"}, {"T", "ㅆ"},
    {"r", "ㄱ"}, {"s", "ㄴ"}, {"e", "ㄷ"}, {"f", "ㄹ"}, {"a", "ㅁ"},
    {"q", "ㅂ"}, {"t", "ㅅ"}, {"d", "ㅇ"}, {"w", "ㅈ"}, {"c", "ㅊ"},
    {"z", "ㅋ"}, {"x", "ㅌ"}, {"v", "ㅍ"}, {"g", "ㅎ"}
};

const char* chosung_list[19] = {
    "ㄱ","ㄲ","ㄴ","ㄷ","ㄸ","ㄹ","ㅁ","ㅂ","ㅃ","ㅅ",
    "ㅆ","ㅇ","ㅈ","ㅉ","ㅊ","ㅋ","ㅌ","ㅍ","ㅎ"
};
const char* jungsung_list[21] = {
    "ㅏ","ㅐ","ㅑ","ㅒ","ㅓ","ㅔ","ㅕ","ㅖ","ㅗ","ㅘ",
    "ㅙ","ㅚ","ㅛ","ㅜ","ㅝ","ㅞ","ㅟ","ㅠ","ㅡ","ㅢ","ㅣ"
};
const char* jongsung_list[28] = {
    "","ㄱ","ㄲ","ㄳ","ㄴ","ㄵ","ㄶ","ㄷ","ㄹ","ㄺ",
    "ㄻ","ㄼ","ㄽ","ㄾ","ㄿ","ㅀ","ㅁ","ㅂ","ㅄ","ㅅ",
    "ㅆ","ㅇ","ㅈ","ㅊ","ㅋ","ㅌ","ㅍ","ㅎ"
};

// Direct UTF-8 to Unicode conversion without setlocale
static wchar_t utf8_to_unicode(const char* utf8_str) {
    if (utf8_str == NULL || strlen(utf8_str) == 0) return 0;
    
    unsigned char* bytes = (unsigned char*)utf8_str;
    wchar_t result = 0;
    
    if (bytes[0] < 0x80) {
        // 1-byte UTF-8 (ASCII)
        result = bytes[0];
    } else if ((bytes[0] & 0xE0) == 0xC0) {
        // 2-byte UTF-8
        result = ((bytes[0] & 0x1F) << 6) | (bytes[1] & 0x3F);
    } else if ((bytes[0] & 0xF0) == 0xE0) {
        // 3-byte UTF-8 (Korean characters)
        result = ((bytes[0] & 0x0F) << 12) | ((bytes[1] & 0x3F) << 6) | (bytes[2] & 0x3F);
    } else if ((bytes[0] & 0xF8) == 0xF0) {
        // 4-byte UTF-8
        result = ((bytes[0] & 0x07) << 18) | ((bytes[1] & 0x3F) << 12) | 
                 ((bytes[2] & 0x3F) << 6) | (bytes[3] & 0x3F);
    }
    
    return result;
}

// Function to convert Unicode to UTF-8 (public function)
void unicode_to_utf8(wchar_t* wstr, char* utf8_str, size_t utf8_size) {
    if (wstr == NULL || utf8_str == NULL || utf8_size == 0) return;
    
    size_t utf8_len = 0;
    size_t wstr_len = wcslen(wstr);
    
    for (size_t i = 0; i < wstr_len && utf8_len < utf8_size - 4; i++) {
        wchar_t wc = wstr[i];
        
        // Convert Unicode to UTF-8
        if (wc < 0x80) {
            utf8_str[utf8_len++] = (char)wc;
        } else if (wc < 0x800) {
            utf8_str[utf8_len++] = 0xC0 | (wc >> 6);
            utf8_str[utf8_len++] = 0x80 | (wc & 0x3F);
        } else if (wc < 0x10000) {
            utf8_str[utf8_len++] = 0xE0 | (wc >> 12);
            utf8_str[utf8_len++] = 0x80 | ((wc >> 6) & 0x3F);
            utf8_str[utf8_len++] = 0x80 | (wc & 0x3F);
        } else {
            utf8_str[utf8_len++] = 0xF0 | (wc >> 18);
            utf8_str[utf8_len++] = 0x80 | ((wc >> 12) & 0x3F);
            utf8_str[utf8_len++] = 0x80 | ((wc >> 6) & 0x3F);
            utf8_str[utf8_len++] = 0x80 | (wc & 0x3F);
        }
    }
    
    utf8_str[utf8_len] = '\0';
}

void qwerty_korean_init(void) {
    // No locale setup needed - using direct UTF-8 conversion
    printf("Korean input system initialized (no locale required)\n");
}

void qwerty_korean_cleanup(void) {
    // No cleanup needed
    printf("Korean input system cleaned up\n");
}

int qwerty_get_index(const char *jamo, const char *list[], int size) {
    for (int i = 0; i < size; i++) {
        if (strcmp(jamo, list[i]) == 0) {
            return i;
        }
    }
    return -1;
}

const char* qwerty_get_jamo_buffer(const char *buffer, KeyMap *map, int size) {
    for (int i = 0; i < size; i++) {
        if (strcmp(buffer, map[i].key) == 0) return map[i].jamo;
    }
    return NULL;
}

void qwerty_print_buffers(char *input_buf, wchar_t *output_buf) {
    printf("\r\033[K");
    printf("Input: [%s] | Output: [", input_buf);
    if (wcslen(output_buf) > 0) {
        // Convert wide characters to UTF-8 for console display
        char utf8_output[1024] = {0};
        size_t utf8_len = 0;
        
        for (size_t i = 0; i < wcslen(output_buf) && utf8_len < sizeof(utf8_output) - 4; i++) {
            wchar_t wc = output_buf[i];
            
            // Convert Unicode to UTF-8
            if (wc < 0x80) {
                utf8_output[utf8_len++] = (char)wc;
            } else if (wc < 0x800) {
                utf8_output[utf8_len++] = 0xC0 | (wc >> 6);
                utf8_output[utf8_len++] = 0x80 | (wc & 0x3F);
            } else if (wc < 0x10000) {
                utf8_output[utf8_len++] = 0xE0 | (wc >> 12);
                utf8_output[utf8_len++] = 0x80 | ((wc >> 6) & 0x3F);
                utf8_output[utf8_len++] = 0x80 | (wc & 0x3F);
            } else {
                utf8_output[utf8_len++] = 0xF0 | (wc >> 18);
                utf8_output[utf8_len++] = 0x80 | ((wc >> 12) & 0x3F);
                utf8_output[utf8_len++] = 0x80 | ((wc >> 6) & 0x3F);
                utf8_output[utf8_len++] = 0x80 | (wc & 0x3F);
            }
        }
        
        // Print UTF-8 string, with fallback to hex if needed
        printf("%s", utf8_output);
        
        // Alternative: If UTF-8 doesn't display properly, you can uncomment this:
        // printf("(Unicode: ");
        // for (size_t i = 0; i < wcslen(output_buf); i++) {
        //     printf("U+%04X ", (unsigned int)output_buf[i]);
        // }
        // printf(")");
    }
    printf("]");
    fflush(stdout);
}

// Check if a character has a valid mapping in any of the key maps
int qwerty_is_mappable_character(char ch) {
    // Space is always mappable
    if (ch == ' ') {
        return 1;
    }
    
    char single_pattern[4] = {ch, '\0'};
    
    // Check if it's a choseong
    if (qwerty_get_jamo_buffer(single_pattern, cho_keymap, sizeof(cho_keymap)/sizeof(KeyMap))) {
        return 1;
    }
    
    // Check if it's a jungseong
    if (qwerty_get_jamo_buffer(single_pattern, jung_keymap, sizeof(jung_keymap)/sizeof(KeyMap))) {
        return 1;
    }
    
    // Check if it's a jongseong
    if (qwerty_get_jamo_buffer(single_pattern, jong_keymap, sizeof(jong_keymap)/sizeof(KeyMap))) {
        return 1;
    }
    
    return 0;
}

// Input handling functions
void qwerty_handle_backspace(char* input_buffer, size_t* input_len, wchar_t* output_buffer) {
    if (*input_len > 0) {
        input_buffer[--(*input_len)] = '\0';
        input_buffer[*input_len] = '\0';
        
        // Clear output buffer
        output_buffer[0] = L'\0';
        
        // Call the main Korean character composition logic
        qwerty_compose_korean_characters(input_buffer, *input_len, output_buffer);
    }
}

void qwerty_handle_enter(char* input_buffer, size_t* input_len, wchar_t* output_buffer) {
    printf("\n");
    // Clear all buffers
    input_buffer[0] = '\0';
    output_buffer[0] = L'\0';
    *input_len = 0;
    printf("Output: []\n");
    fflush(stdout);
}

void qwerty_handle_space(char* input_buffer, size_t* input_len, wchar_t* output_buffer) {
    printf("qwerty_handle_space: input_len before = %zu\n", *input_len);

    if (*input_len < MAX_OUTPUT_LEN - 1) {
        input_buffer[(*input_len)++] = ' ';
        input_buffer[*input_len] = '\0';
    }

    // Recompose Korean characters to get correct output
    qwerty_compose_korean_characters(input_buffer, *input_len, output_buffer);

    // Find the end of output buffer and add space
    size_t output_len = wcslen(output_buffer);
    printf("qwerty_handle_space: output_len before space = %zu\n", output_len);

    if (output_len < MAX_OUTPUT_LEN - 1) {
        output_buffer[output_len] = L' ';
        output_buffer[output_len + 1] = L'\0';
        printf("qwerty_handle_space: Added space at position %zu\n", output_len);
    }

    printf("qwerty_handle_space: output_len after space = %zu\n", wcslen(output_buffer));
}

void qwerty_handle_character(char* input_buffer, size_t* input_len, wchar_t* output_buffer, int ch) {
    // Check if character is mappable before adding to input buffer
    if (!qwerty_is_mappable_character((char)ch)) {
        // Ignore unmappable characters
        return;
    }
    // Add character to input buffer
    if (*input_len < MAX_OUTPUT_LEN - 1) {
        input_buffer[(*input_len)++] = (char)ch;
        input_buffer[*input_len] = '\0';
    }

    // Call the main Korean character composition logic
    qwerty_compose_korean_characters(input_buffer, *input_len, output_buffer);
}


void qwerty_process_input(char* input_buffer, size_t* input_len, wchar_t* output_buffer, int ch) {
    if (ch == 0x7f) {  // 백스페이스
        qwerty_handle_backspace(input_buffer, input_len, output_buffer);
    } else if (ch == '\n' || ch == '\r') {  // Enter key
        qwerty_handle_enter(input_buffer, input_len, output_buffer);
    } else if (ch == ' ') {  // 공백
        qwerty_handle_space(input_buffer, input_len, output_buffer);
    } else {
        qwerty_handle_character(input_buffer, input_len, output_buffer, ch);
    }
    // print the buffer status
    qwerty_print_buffers(input_buffer, output_buffer);
}

// Main Korean character composition logic
void qwerty_compose_korean_characters(const char* input_buffer, size_t input_len, wchar_t* output_buffer) {
    wchar_t temp_output[MAX_OUTPUT_LEN];
    temp_output[0] = L'\0';
    size_t temp_len = 0;
    
    // Process input buffer progressively to show intermediate states
    size_t i = 0;
    while (i < input_len && temp_len < MAX_OUTPUT_LEN - 1) {
        char current_char = input_buffer[i];

        // Handle space character - add it directly to output
        if (current_char == ' ') {
            temp_output[temp_len++] = L' ';
            temp_output[temp_len] = L'\0';
            i++;
            continue;
        }

        // Check if current character is a choseong
        char single_pattern[4] = {current_char, '\0'};
        const char* cho_jamo = qwerty_get_jamo_buffer(single_pattern, cho_keymap, sizeof(cho_keymap)/sizeof(KeyMap));
        
        if (cho_jamo) {
            // Found a choseong, check for jungseong next
            const char* jung_jamo = NULL;
            int jung_advance = 0;
            
            // Check for compound jungseong first (2 characters)
            if (i + 2 < input_len) {
                char jung2_pattern[4] = {input_buffer[i+1], input_buffer[i+2], '\0'};
                jung_jamo = qwerty_get_jamo_buffer(jung2_pattern, jung_keymap, sizeof(jung_keymap)/sizeof(KeyMap));
                if (jung_jamo) {
                    jung_advance = 2;
                }
            }
            
            // If no compound jungseong, check for single jungseong
            if (!jung_jamo && i + 1 < input_len) {
                char next_char = input_buffer[i + 1];
                char next_pattern[4] = {next_char, '\0'};
                jung_jamo = qwerty_get_jamo_buffer(next_pattern, jung_keymap, sizeof(jung_keymap)/sizeof(KeyMap));
                if (jung_jamo) {
                    jung_advance = 1;
                }
            }
            
            if (jung_jamo) {
                // Found choseong + jungseong, check for jongseong
                int cho_idx = qwerty_get_index(cho_jamo, chosung_list, 19);
                int jung_idx = qwerty_get_index(jung_jamo, jungsung_list, 21);
                
                if (cho_idx >= 0 && jung_idx >= 0) {
                    // Check for compound jongseong first (2 characters)
                    int jong_idx = 0;
                    int jong_advance = 0;
                    
                    if (i + 2 + jung_advance < input_len) {
                        char jong2_pattern[4] = {input_buffer[i+1+jung_advance], input_buffer[i+2+jung_advance], '\0'};
                        const char* jong2_jamo = qwerty_get_jamo_buffer(jong2_pattern, jong_keymap, sizeof(jong_keymap)/sizeof(KeyMap));
                        if (jong2_jamo && strlen(jong2_jamo) > 0) {
                            int idx = qwerty_get_index(jong2_jamo, jongsung_list, 28);
                            if (idx > 0) { 
                                jong_idx = idx; 
                                jong_advance = 2;
                            }
                        }
                    }
                    
                    // If no compound jongseong, check for single jongseong
                    if (jong_idx == 0 && i + 1 + jung_advance < input_len) {
                        char jong1_pattern[4] = {input_buffer[i+1+jung_advance], '\0'};
                        const char* jong1_jamo = qwerty_get_jamo_buffer(jong1_pattern, jong_keymap, sizeof(jong_keymap)/sizeof(KeyMap));
                        if (jong1_jamo && strlen(jong1_jamo) > 0) {
                            int idx = qwerty_get_index(jong1_jamo, jongsung_list, 28);
                            if (idx > 0) { 
                                jong_idx = idx; 
                                jong_advance = 1;
                            }
                        }
                    }
                    
                    // Check if there's a jungseong after the jongseong
                    if (jong_advance > 0 && i + 1 + jung_advance + jong_advance < input_len) {
                        char after_jong_char = input_buffer[i + 1 + jung_advance + jong_advance];
                        char after_jong_pattern[4] = {after_jong_char, '\0'};
                        const char* after_jung_jamo = qwerty_get_jamo_buffer(after_jong_pattern, jung_keymap, sizeof(jung_keymap)/sizeof(KeyMap));
                        
                        if (after_jung_jamo) {
                            // There's a jungseong after compound jongseong
                            // Split the compound jongseong: first char becomes jongseong, second becomes choseong
                            if (jong_advance == 2) {
                                // For compound jongseong, get the first character
                                char first_jong_char = input_buffer[i+1+jung_advance];
                                char first_jong_pattern[4] = {first_jong_char, '\0'};
                                const char* first_jong_jamo = qwerty_get_jamo_buffer(first_jong_pattern, jong_keymap, sizeof(jong_keymap)/sizeof(KeyMap));
                                if (first_jong_jamo && strlen(first_jong_jamo) > 0) {
                                    int idx = qwerty_get_index(first_jong_jamo, jongsung_list, 28);
                                    if (idx > 0) { 
                                        jong_idx = idx; 
                                        jong_advance = 1; // Only advance by 1, second char will be processed next
                                    }
                                }
                            } else {
                                // For single jongseong, don't use it
                                jong_idx = 0;
                                jong_advance = 0;
                            }
                        }
                        // If there's a choseong after jongseong, don't split the compound jongseong
                        // The compound jongseong stays intact and the choseong starts a new syllable
                    }
                    
                    // Compose syllable
                    wchar_t syll = 0xAC00 + (cho_idx * 21 * 28) + (jung_idx * 28) + jong_idx;
                    temp_output[temp_len++] = syll;
                    i += 1 + jung_advance + jong_advance;
                    
                    continue;
                }
            }
            
            // No jungseong found, display individual choseong
            wchar_t wc = utf8_to_unicode(cho_jamo);
            if (wc != 0) {
                temp_output[temp_len++] = wc;
            }
            i++;
        } else {
            // Check if it's a jungseong
            const char* jung_jamo = qwerty_get_jamo_buffer(single_pattern, jung_keymap, sizeof(jung_keymap)/sizeof(KeyMap));
            if (jung_jamo) {
                // Display individual jungseong
                wchar_t wc = utf8_to_unicode(jung_jamo);
                if (wc != 0) {
                    temp_output[temp_len++] = wc;
                }
                i++;
            } else {
                // Check if it's a jongseong
                const char* jong_jamo = qwerty_get_jamo_buffer(single_pattern, jong_keymap, sizeof(jong_keymap)/sizeof(KeyMap));
                if (jong_jamo && strlen(jong_jamo) > 0) {
                    // Display individual jongseong
                    wchar_t wc = utf8_to_unicode(jong_jamo);
                    if (wc != 0) {
                        temp_output[temp_len++] = wc;
                    }
                } else {
                    // Not a Korean character, keep it in the input buffer
                    // and start a new Korean syllable after it
                    // For now, we'll just advance past it
                }
                i++;
            }
        }
    }
    
    temp_output[temp_len] = L'\0';
    
    // Update output buffer
    wcscpy(output_buffer, temp_output);
}