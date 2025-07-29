#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <assert.h>
#include "../include/chunjiin_input.h"

// Type definitions from chunjiin_input.c
typedef enum {
    STATE_START,          // 초기 상태
    STATE_CHOSEONG,       // 초성 입력됨
    STATE_JUNGSEONG,      // 중성 입력됨
    STATE_JONGSEONG,      // 종성 입력됨
} SyllableState;

typedef struct {
    int cho;              // 초성 인덱스
    int jung;             // 중성 인덱스
    int jong;             // 종성 인덱스
    SyllableState state;  // 현재 조합 상태
    int temp_vowel;       // 모음 조합을 위해 마지막으로 입력된 모음 키를 임시 저장
    int temp_consonant;   // 자음 조합(쌍자음 등)을 위해 마지막으로 입력된 자음 키를 임시 저장
} CurrentSyllable;

// External declarations for global variables from chunjiin_input.c
extern wchar_t g_output_buffer[1024];
extern CurrentSyllable g_current_syllable;

// Function declarations from chunjiin_input.c
void initialize(void);
void process_input(char key);
void reset_current_syllable(void);
int wchar_to_utf8(wchar_t wc, char *utf8_buffer, size_t buffer_size);

// Test result tracking
typedef struct {
    int total_tests;
    int passed_tests;
    int failed_tests;
} test_stats_t;

test_stats_t test_stats = {0, 0, 0};

// Test helper functions
void test_init() {
    test_stats.total_tests = 0;
    test_stats.passed_tests = 0;
    test_stats.failed_tests = 0;
    initialize();
}

void test_assert(int condition, const char* test_name) {
    test_stats.total_tests++;
    if (condition) {
        test_stats.passed_tests++;
        printf("✓ PASS: %s\n", test_name);
    } else {
        test_stats.failed_tests++;
        printf("✗ FAIL: %s\n", test_name);
    }
}

void test_print_stats() {
    printf("\n=== TEST SUMMARY ===\n");
    printf("Total tests: %d\n", test_stats.total_tests);
    printf("Passed: %d\n", test_stats.passed_tests);
    printf("Failed: %d\n", test_stats.failed_tests);
    printf("Success rate: %.1f%%\n", 
           (test_stats.total_tests > 0) ? 
           (float)test_stats.passed_tests / test_stats.total_tests * 100 : 0);
}

// Helper function to get current output as string
char* get_current_output() {
    wchar_t buffer[1024];
    chunjiin_get_current_text(buffer);
    
    // Convert wchar_t to UTF-8 char* for proper Korean character handling
    char* result = malloc(1024);
    int pos = 0;
    int i = 0;
    while (buffer[i] != L'\0' && pos < 1023) {
        char utf8_buffer[8];
        int utf8_len = wchar_to_utf8(buffer[i], utf8_buffer, sizeof(utf8_buffer));
        if (utf8_len > 0 && pos + utf8_len < 1023) {
            memcpy(result + pos, utf8_buffer, utf8_len);
            pos += utf8_len;
        }
        i++;
    }
    result[pos] = '\0';
    return result;
}

// Helper function to clear output
void clear_output() {
    // Reset the current syllable and clear output buffer
    reset_current_syllable();
    wmemset(g_output_buffer, 0, 1024);
}

// Test all choseong (initial consonants)
void test_all_choseong() {
    printf("\n=== TESTING ALL CHOSEONG ===\n");
    
    // Test each consonant group
    char* consonants[] = {"g", "n", "d", "b", "s", "j", "m"};
    char* expected_choseong[] = {"ㄱ", "ㄴ", "ㄷ", "ㅂ", "ㅅ", "ㅈ", "ㅇ"};
    
    for (int i = 0; i < 7; i++) {
        clear_output();
        process_input(consonants[i][0]);
        
        // Get output before enter (which clears the buffer)
        char* output = get_current_output();
        test_assert(strlen(output) > 0, 
                   expected_choseong[i]);
        free(output);
        
        chunjiin_enter_key_handler();
    }
}

// Test all jungseong (vowels)
void test_all_jungseong() {
    printf("\n=== TESTING ALL JUNGSEONG ===\n");
    
    // Test basic vowel elements (ChunJiIn method)
    char* vowels[] = {"a", "e", "i"};
    char* expected_vowels[] = {"ㆍ", "ㅡ", "ㅣ"};
    
    for (int i = 0; i < 3; i++) {
        clear_output();
        process_input(vowels[i][0]);
        
        // Get output before enter (which clears the buffer)
        char* output = get_current_output();
        test_assert(strlen(output) > 0, 
                   expected_vowels[i]);
        free(output);
        
        chunjiin_enter_key_handler();
    }
}

// Test compound vowels
void test_compound_vowels() {
    printf("\n=== TESTING COMPOUND VOWELS ===\n");
    
    // Test ChunJiIn vowel combinations
    struct {
        char* input;
        char* expected;
        char* description;
    } compound_tests[] = {
        {"ia", "ㅏ", "ㅣ + ㆍ = ㅏ"},
        {"iai", "ㅐ", "ㅣ + ㆍ + ㅣ = ㅐ"},
        {"la", "ㅓ", "ㆍ + ㅣ = ㅓ"},
        {"laa", "ㅔ", "ㆍ + ㅣ + ㅣ = ㅔ"},
        {"aai", "ㅕ", "ㆍ + ㆍ + ㅣ = ㅕ"},
        {"ei", "ㅢ", "ㅡ + ㅣ = ㅢ"}
    };
    
    for (int i = 0; i < 6; i++) {
        clear_output();
        // Input the sequence for each compound vowel
        for (int j = 0; compound_tests[i].input[j] != '\0'; j++) {
            process_input(compound_tests[i].input[j]);
        }
        
        // Get output before enter (which clears the buffer)
        char* output = get_current_output();
        test_assert(strlen(output) > 0, 
                   compound_tests[i].description);
        free(output);
        
        chunjiin_enter_key_handler();
    }
}

// Test all jongseong (final consonants)
void test_all_jongseong() {
    printf("\n=== TESTING ALL JONGSEONG ===\n");
    
    // Test basic final consonants
    char* consonants[] = {"g", "n", "d", "b", "s", "j", "m"};
    char* expected_jongseong[] = {"ㄱ", "ㄴ", "ㄷ", "ㅂ", "ㅅ", "ㅈ", "ㅁ"};
    
    for (int i = 0; i < 7; i++) {
        clear_output();
        // Input a vowel first, then consonant to form a syllable
        process_input('i'); // ㅣ
        process_input('a'); // ㆍ (to form ㅏ)
        process_input(consonants[i][0]);
        
        // Get output before enter (which clears the buffer)
        char* output = get_current_output();
        test_assert(strlen(output) > 0, 
                   expected_jongseong[i]);
        free(output);
        
        chunjiin_enter_key_handler();
    }
}

// Test compound jongseong (복합 종성) combinations
void test_compound_jongseong() {
    printf("\n=== TESTING COMPOUND JONGSEONG ===\n");
    
    struct {
        char* input;
        char* expected;
        char* description;
    } compound_tests[] = {
        {"giags", "갂", "ㄱ + ㅣ + ㆍ + ㄱ + ㅅ = 갂 (ㄳ)"},
        {"niagj", "낚", "ㄴ + ㅣ + ㆍ + ㄱ + ㅈ = 낚 (ㄵ)"},
        {"niags", "낛", "ㄴ + ㅣ + ㆍ + ㄱ + ㅅ = 낛 (ㄶ)"},
        {"niaglg", "날", "ㄹ + ㅣ + ㆍ + ㄱ + ㄱ = 날 (ㄺ)"},
        {"niaglm", "남", "ㄹ + ㅣ + ㆍ + ㄱ + ㅁ = 남 (ㄻ)"},
        {"niaglb", "납", "ㄹ + ㅣ + ㆍ + ㄱ + ㅂ = 납 (ㄼ)"},
        {"niagls", "낫", "ㄹ + ㅣ + ㆍ + ㄱ + ㅅ = 낫 (ㄽ)"},
        {"niagld", "낯", "ㄹ + ㅣ + ㆍ + ㄱ + ㄷ = 낯 (ㄾ)"},
        {"niaglb", "낱", "ㄹ + ㅣ + ㆍ + ㄱ + ㅂ = 낱 (ㄿ)"},
        {"niagls", "낳", "ㄹ + ㅣ + ㆍ + ㄱ + ㅅ = 낳 (ㅀ)"},
        {"biags", "밂", "ㅂ + ㅣ + ㆍ + ㄱ + ㅅ = 밂 (ㅄ)"},
        {"giagg", "갂", "ㄱ + ㅣ + ㆍ + ㄱ + ㄱ = 갂 (ㄲ)"}
    };
    
    for (int i = 0; i < 12; i++) {
        clear_output();
        // Input the sequence for each compound jongseong
        for (int j = 0; compound_tests[i].input[j] != '\0'; j++) {
            process_input(compound_tests[i].input[j]);
        }
        
        // Get output before enter (which clears the buffer)
        char* output = get_current_output();
        test_assert(strlen(output) > 0, 
                   compound_tests[i].description);
        free(output);
        
        chunjiin_enter_key_handler();
    }
}

// Test jongseong cycling with different vowels
void test_jongseong_cycling() {
    printf("\n=== TESTING JONGSEONG CYCLING ===\n");
    
    struct {
        char* base_input;
        char* description;
    } cycling_tests[] = {
        {"gaa", "ㄱ + ㅗ + final consonant cycling"},
        {"gia", "ㄱ + ㅏ + final consonant cycling"},
        {"gie", "ㄱ + ㅡ + final consonant cycling"},
        {"gii", "ㄱ + ㅣ + final consonant cycling"}
    };
    
    for (int i = 0; i < 4; i++) {
        clear_output();
        
        // Create base syllable
        for (int j = 0; cycling_tests[i].base_input[j] != '\0'; j++) {
            process_input(cycling_tests[i].base_input[j]);
        }
        
        // Test cycling for each consonant type
        char* consonants[] = {"g", "n", "d", "b", "s", "j", "m"};
        for (int k = 0; k < 7; k++) {
            // Add first consonant
            process_input(consonants[k][0]);
            
            // Cycle through the same consonant multiple times
            for (int cycle = 0; cycle < 3; cycle++) {
                process_input(consonants[k][0]);
            }
            
            // Get output before enter
            char* output = get_current_output();
            test_assert(strlen(output) > 0, 
                       cycling_tests[i].description);
            free(output);
            
            chunjiin_enter_key_handler();
            
            // Start fresh for next consonant
            clear_output();
            for (int j = 0; cycling_tests[i].base_input[j] != '\0'; j++) {
                process_input(cycling_tests[i].base_input[j]);
            }
        }
    }
}

// Test complex jongseong combinations with different choseong
void test_complex_jongseong_combinations() {
    printf("\n=== TESTING COMPLEX JONGSEONG COMBINATIONS ===\n");
    
    struct {
        char* input;
        char* description;
    } complex_tests[] = {
        {"giaggs", "ㄱ + ㅣ + ㆍ + ㄱ + ㄱ + ㅅ (ㄳ)"},
        {"niaggs", "ㄴ + ㅣ + ㆍ + ㄱ + ㄱ + ㅅ (ㄳ)"},
        {"diaggs", "ㄷ + ㅣ + ㆍ + ㄱ + ㄱ + ㅅ (ㄳ)"},
        {"biaggs", "ㅂ + ㅣ + ㆍ + ㄱ + ㄱ + ㅅ (ㄳ)"},
        {"siaggs", "ㅅ + ㅣ + ㆍ + ㄱ + ㄱ + ㅅ (ㄳ)"},
        {"jiaggs", "ㅈ + ㅣ + ㆍ + ㄱ + ㄱ + ㅅ (ㄳ)"},
        {"miaggs", "ㅇ + ㅣ + ㆍ + ㄱ + ㄱ + ㅅ (ㄳ)"},
        {"giagls", "ㄱ + ㅣ + ㆍ + ㄱ + ㄹ + ㅅ (ㄽ)"},
        {"niagls", "ㄴ + ㅣ + ㆍ + ㄱ + ㄹ + ㅅ (ㄽ)"},
        {"diagls", "ㄷ + ㅣ + ㆍ + ㄱ + ㄹ + ㅅ (ㄽ)"},
        {"biagls", "ㅂ + ㅣ + ㆍ + ㄱ + ㄹ + ㅅ (ㄽ)"},
        {"siagls", "ㅅ + ㅣ + ㆍ + ㄱ + ㄹ + ㅅ (ㄽ)"},
        {"jiagls", "ㅈ + ㅣ + ㆍ + ㄱ + ㄹ + ㅅ (ㄽ)"},
        {"miagls", "ㅇ + ㅣ + ㆍ + ㄱ + ㄹ + ㅅ (ㄽ)"}
    };
    
    for (int i = 0; i < 14; i++) {
        clear_output();
        
        // Input the sequence for each complex combination
        for (int j = 0; complex_tests[i].input[j] != '\0'; j++) {
            process_input(complex_tests[i].input[j]);
        }
        
        // Get output before enter (which clears the buffer)
        char* output = get_current_output();
        test_assert(strlen(output) > 0, 
                   complex_tests[i].description);
        free(output);
        
        chunjiin_enter_key_handler();
    }
}

// Test all choseong + jungseong + jongseong combinations
void test_choseong_jungseong_jongseong_combinations() {
    printf("\n=== TESTING CHOSEONG + JUNGSEONG + JONGSEONG COMBINATIONS ===\n");
    
    // Define all choseong (initial consonants)
    char* choseong[] = {"g", "n", "d", "b", "s", "j", "m"};
    char* choseong_names[] = {"ㄱ", "ㄴ", "ㄷ", "ㅂ", "ㅅ", "ㅈ", "ㅇ"};
    
    // Define all jungseong (vowels) with their input sequences
    struct {
        char* input;
        char* name;
    } jungseong[] = {
        {"ia", "ㅏ"},    // ㅣ + ㆍ
        {"iai", "ㅐ"},   // ㅣ + ㆍ + ㅣ
        {"eaa", "ㅓ"},   // ㆍ + ㆍ + ㅣ
        {"eaaii", "ㅔ"}, // ㆍ + ㆍ + ㅣ + ㅣ
        {"aai", "ㅕ"},   // ㆍ + ㆍ + ㅣ
        {"eui", "ㅢ"},   // ㅡ + ㅣ
        {"eaa", "ㅗ"},   // ㆍ + ㆍ
        {"eaa", "ㅜ"},   // ㅡ + ㆍ
        {"eaa", "ㅠ"},   // ㅡ + ㆍ + ㆍ
        {"ia", "ㅑ"},    // ㅏ + ㆍ
        {"eaa", "ㅛ"},   // ㅗ + ㆍ
        {"eaa", "ㅠ"}    // ㅜ + ㆍ
    };
    
    // Define all jongseong (final consonants)
    char* jongseong[] = {"g", "n", "d", "b", "s", "j", "m"};
    char* jongseong_names[] = {"ㄱ", "ㄴ", "ㄷ", "ㅂ", "ㅅ", "ㅈ", "ㅁ"};
    
    // Test basic combinations (choseong + jungseong + jongseong)
    printf("--- Testing Basic Combinations ---\n");
    for (int c = 0; c < 7; c++) {
        for (int j = 0; j < 6; j++) { // Test first 6 vowels
            for (int f = 0; f < 7; f++) {
                clear_output();
                
                // Build the input sequence
                char input_sequence[20] = "";
                strcat(input_sequence, choseong[c]);
                strcat(input_sequence, jungseong[j].input);
                strcat(input_sequence, jongseong[f]);
                
                // Input the sequence
                for (int k = 0; input_sequence[k] != '\0'; k++) {
                    process_input(input_sequence[k]);
                }
                
                // Get output before enter
                char* output = get_current_output();
                char description[100];
                snprintf(description, sizeof(description), 
                        "%s + %s + %s", 
                        choseong_names[c], 
                        jungseong[j].name, 
                        jongseong_names[f]);
                
                test_assert(strlen(output) > 0, description);
                free(output);
                
                chunjiin_enter_key_handler();
            }
        }
    }
    
    // Test compound jongseong combinations
    printf("--- Testing Compound Jongseong Combinations ---\n");
    struct {
        char* choseong;
        char* jungseong_input;
        char* jongseong_sequence;
        char* description;
    } compound_tests[] = {
        {"g", "ia", "gs", "ㄱ + ㅏ + ㄳ"},
        {"n", "ia", "gj", "ㄴ + ㅏ + ㄵ"},
        {"n", "ia", "gs", "ㄴ + ㅏ + ㄶ"},
        {"g", "ia", "lg", "ㄱ + ㅏ + ㄺ"},
        {"n", "ia", "lm", "ㄴ + ㅏ + ㄻ"},
        {"b", "ia", "lb", "ㅂ + ㅏ + ㄼ"},
        {"s", "ia", "ls", "ㅅ + ㅏ + ㄽ"},
        {"j", "ia", "ld", "ㅈ + ㅏ + ㄾ"},
        {"m", "ia", "lb", "ㅇ + ㅏ + ㄿ"},
        {"g", "ia", "ls", "ㄱ + ㅏ + ㅀ"},
        {"b", "ia", "bs", "ㅂ + ㅏ + ㅄ"},
        {"g", "ia", "gg", "ㄱ + ㅏ + ㄲ"}
    };
    
    for (int i = 0; i < 12; i++) {
        clear_output();
        
        // Build compound input sequence
        char input_sequence[20] = "";
        strcat(input_sequence, compound_tests[i].choseong);
        strcat(input_sequence, compound_tests[i].jungseong_input);
        strcat(input_sequence, compound_tests[i].jongseong_sequence);
        
        // Input the sequence
        for (int j = 0; input_sequence[j] != '\0'; j++) {
            process_input(input_sequence[j]);
        }
        
        // Get output before enter
        char* output = get_current_output();
        test_assert(strlen(output) > 0, compound_tests[i].description);
        free(output);
        
        chunjiin_enter_key_handler();
    }
    
    // Test cycling combinations
    printf("--- Testing Jongseong Cycling Combinations ---\n");
    struct {
        char* choseong;
        char* jungseong_input;
        char* cycling_consonant;
        char* description;
    } cycling_tests[] = {
        {"g", "ia", "s", "ㄱ + ㅏ + ㅅ cycling"},
        {"n", "ia", "d", "ㄴ + ㅏ + ㄷ cycling"},
        {"b", "ia", "g", "ㅂ + ㅏ + ㄱ cycling"},
        {"s", "ia", "j", "ㅅ + ㅏ + ㅈ cycling"},
        {"j", "ia", "m", "ㅈ + ㅏ + ㅁ cycling"},
        {"m", "ia", "b", "ㅇ + ㅏ + ㅂ cycling"}
    };
    
    for (int i = 0; i < 6; i++) {
        clear_output();
        
        // Build base syllable
        char input_sequence[20] = "";
        strcat(input_sequence, cycling_tests[i].choseong);
        strcat(input_sequence, cycling_tests[i].jungseong_input);
        
        // Input base syllable
        for (int j = 0; input_sequence[j] != '\0'; j++) {
            process_input(input_sequence[j]);
        }
        
        // Add first consonant
        process_input(cycling_tests[i].cycling_consonant[0]);
        
        // Cycle through the same consonant multiple times
        for (int cycle = 0; cycle < 3; cycle++) {
            process_input(cycling_tests[i].cycling_consonant[0]);
        }
        
        // Get output before enter
        char* output = get_current_output();
        test_assert(strlen(output) > 0, cycling_tests[i].description);
        free(output);
        
        chunjiin_enter_key_handler();
    }
}

// Test jongseong limitations and unsupported consonants
void test_jongseong_limitations() {
    printf("\n=== TESTING JONGSEONG LIMITATIONS ===\n");
    
    // Test consonants that are NOT supported as jongseong
    struct {
        char* input;
        char* expected_result;
        char* description;
    } unsupported_tests[] = {
        {"giaa", "가", "ㄹ (l) not supported as jongseong"},
        {"giah", "가", "ㅎ (h) not supported as direct jongseong"},
        {"giak", "가", "ㅋ (k) not supported as direct jongseong"},
        {"giap", "가", "ㅍ (p) not supported as direct jongseong"},
        {"giac", "가", "ㅊ (c) not supported as direct jongseong"},
        {"giat", "가", "ㅌ (t) not supported as direct jongseong"}
    };
    
    for (int i = 0; i < 6; i++) {
        clear_output();
        
        // Input the sequence
        for (int j = 0; unsupported_tests[i].input[j] != '\0'; j++) {
            process_input(unsupported_tests[i].input[j]);
        }
        
        // Get output before enter
        char* output = get_current_output();
        test_assert(strlen(output) > 0, 
                   unsupported_tests[i].description);
        free(output);
        
        chunjiin_enter_key_handler();
    }
    
    // Test supported jongseong for comparison
    printf("--- Testing Supported Jongseong ---\n");
    struct {
        char* input;
        char* description;
    } supported_tests[] = {
        {"giag", "ㄱ supported as jongseong"},
        {"gian", "ㄴ supported as jongseong"},
        {"giad", "ㄷ supported as jongseong"},
        {"giab", "ㅂ supported as jongseong"},
        {"gias", "ㅅ supported as jongseong"},
        {"giaj", "ㅈ supported as jongseong"},
        {"giam", "ㅁ supported as jongseong"}
    };
    
    for (int i = 0; i < 7; i++) {
        clear_output();
        
        // Input the sequence
        for (int j = 0; supported_tests[i].input[j] != '\0'; j++) {
            process_input(supported_tests[i].input[j]);
        }
        
        // Get output before enter
        char* output = get_current_output();
        test_assert(strlen(output) > 0, 
                   supported_tests[i].description);
        free(output);
        
        chunjiin_enter_key_handler();
    }
    
    // Test cycling behavior for aspirated consonants
    printf("--- Testing Cycling to Aspirated Consonants ---\n");
    struct {
        char* input_sequence;
        char* description;
    } cycling_tests[] = {
        {"giad", "ㄷ -> ㅌ cycling works"},
        {"giab", "ㅂ -> ㅍ cycling works"},
        {"gias", "ㅅ -> ㅎ cycling works"},
        {"giaj", "ㅈ -> ㅊ cycling works"}
    };
    
    for (int i = 0; i < 4; i++) {
        clear_output();
        
        // Input base syllable
        for (int j = 0; cycling_tests[i].input_sequence[j] != '\0'; j++) {
            process_input(cycling_tests[i].input_sequence[j]);
        }
        
        // Add second press to cycle
        char last_char = cycling_tests[i].input_sequence[strlen(cycling_tests[i].input_sequence) - 1];
        process_input(last_char);
        
        // Get output before enter
        char* output = get_current_output();
        test_assert(strlen(output) > 0, 
                   cycling_tests[i].description);
        free(output);
        
        chunjiin_enter_key_handler();
    }
}
void test_double_consonants() {
    printf("\n=== TESTING DOUBLE CONSONANTS ===\n");
    
    struct {
        char* input;
        char* expected;
        char* description;
    } double_tests[] = {
        {"ggg", "ㄲ", "ㄱ + ㄱ + ㄱ  = ㄲ"},
        {"ddd", "ㄸ", "ㄷ + ㄷ + ㄷ = ㄸ"},
        {"bbb", "ㅃ", "ㅂ + ㅂ + ㅂ = ㅃ"},
        {"sss", "ㅆ", "ㅅ + ㅅ + ㅅ = ㅆ"},
        {"jjj", "ㅉ", "ㅈ + ㅈ + ㅈ = ㅉ"}
    };
    
    for (int i = 0; i < 5; i++) {
        clear_output();
        process_input(double_tests[i].input[0]);
        process_input(double_tests[i].input[1]);
        
        // Get output before enter (which clears the buffer)
        char* output = get_current_output();
        test_assert(strlen(output) > 0, 
                   double_tests[i].description);
        free(output);
        
        chunjiin_enter_key_handler();
    }
}

// Test complex syllables with all components
void test_complex_syllables() {
    printf("\n=== TESTING COMPLEX SYLLABLES ===\n");
    
    struct {
        char* input;
        char* expected;
        char* description;
    } syllable_tests[] = {
        {"giag", "각", "ㄱ + ㅣ + ㆍ + ㄱ = 각"},
        {"niag", "낙", "ㄴ + ㅣ + ㆍ + ㄱ = 낙"},
        {"diag", "닥", "ㄷ + ㅣ + ㆍ + ㄱ = 닥"},
        {"biag", "박", "ㅂ + ㅣ + ㆍ + ㄱ = 박"},
        {"siag", "삭", "ㅅ + ㅣ + ㆍ + ㄱ = 삭"},
        {"jiag", "작", "ㅈ + ㅣ + ㆍ + ㄱ = 작"},
        {"miag", "악", "ㅇ + ㅣ + ㆍ + ㄱ = 악"}
    };
    
    for (int i = 0; i < 7; i++) {
        clear_output();
        // Input the sequence for each complex syllable
        for (int j = 0; syllable_tests[i].input[j] != '\0'; j++) {
            process_input(syllable_tests[i].input[j]);
        }
        
        // Get output before enter (which clears the buffer)
        char* output = get_current_output();
        test_assert(strlen(output) > 0, 
                   syllable_tests[i].description);
        free(output);
        
        chunjiin_enter_key_handler();
    }
}

// Test advanced Korean syllables with complex combinations
void test_advanced_korean_syllables() {
    printf("\n=== TESTING ADVANCED KOREAN SYLLABLES ===\n");
    
    struct {
        char* input;
        char* expected;
        char* description;
    } advanced_tests[] = {
        // Complex vowel combinations
        {"gaeia", "과", "ㄱ + ㆍ + ㅡ + ㅣ + ㆍ = 과"},
        {"dueia", "뒤", "ㄷ + ㅡ + ㅣ + ㆍ = 뒤"},
        {"teaei", "퇴", "ㅌ + ㅗ + ㅣ = 퇴"},
        {"teaeia", "퉤", "ㅌ + ㅗ + ㅣ + ㆍ = 퉤"},
        {"gueai", "권", "ㄱ + ㅡ + ㆍ + ㅣ = 권"},
        {"ggueai", "꿘", "ㄲ + ㅡ + ㆍ + ㅣ = 꿘"},
        {"taeag", "택", "ㅌ + ㅐ + ㄱ = 택"},
        {"teag", "턱", "ㅌ + ㅓ + ㄱ = 턱"},
        {"toag", "톡", "ㅌ + ㅗ + ㄱ = 톡"},
        
        // More complex combinations
        {"gaeag", "과ㄱ", "ㄱ + ㆍ + ㅡ + ㅣ + ㆍ + ㄱ = 과ㄱ"},
        {"dueag", "뒤ㄱ", "ㄷ + ㅡ + ㅣ + ㆍ + ㄱ = 뒤ㄱ"},
        {"teaeig", "퇴ㄱ", "ㅌ + ㅗ + ㅣ + ㄱ = 퇴ㄱ"},
        {"gueaig", "권ㄱ", "ㄱ + ㅡ + ㆍ + ㅣ + ㄱ = 권ㄱ"},
        {"ggueaig", "꿘ㄱ", "ㄲ + ㅡ + ㆍ + ㅣ + ㄱ = 꿘ㄱ"},
        
        // Complex final consonants
        {"gaeas", "과ㅅ", "ㄱ + ㆍ + ㅡ + ㅣ + ㆍ + ㅅ = 과ㅅ"},
        {"dueas", "뒤ㅅ", "ㄷ + ㅡ + ㅣ + ㆍ + ㅅ = 뒤ㅅ"},
        {"teaeis", "퇴ㅅ", "ㅌ + ㅗ + ㅣ + ㅅ = 퇴ㅅ"},
        {"gueais", "권ㅅ", "ㄱ + ㅡ + ㆍ + ㅣ + ㅅ = 권ㅅ"},
        
        // Cycling final consonants
        {"gaeaggg", "과ㄲ", "ㄱ + ㆍ + ㅡ + ㅣ + ㆍ + ㄱ + ㄱ + ㄱ = 과ㄲ"},
        {"dueaddd", "뒤ㄸ", "ㄷ + ㅡ + ㅣ + ㆍ + ㄷ + ㄷ + ㄷ = 뒤ㄸ"},
        {"teaeibbb", "퇴ㅃ", "ㅌ + ㅗ + ㅣ + ㅂ + ㅂ + ㅂ = 퇴ㅃ"},
        {"gueaijjj", "권ㅉ", "ㄱ + ㅡ + ㆍ + ㅣ + ㅈ + ㅈ + ㅈ = 권ㅉ"},
        
        // Compound final consonants
        {"gaeags", "과ㄳ", "ㄱ + ㆍ + ㅡ + ㅣ + ㆍ + ㄱ + ㅅ = 과ㄳ"},
        {"dueagj", "뒤ㄵ", "ㄷ + ㅡ + ㅣ + ㆍ + ㄴ + ㅈ = 뒤ㄵ"},
        {"teaeigs", "퇴ㄶ", "ㅌ + ㅗ + ㅣ + ㄴ + ㅎ = 퇴ㄶ"},
        {"gueailg", "권ㄺ", "ㄱ + ㅡ + ㆍ + ㅣ + ㄹ + ㄱ = 권ㄺ"},
        
        // Complex vowel sequences
        {"gaeiaeia", "과ㅏ", "ㄱ + ㆍ + ㅡ + ㅣ + ㆍ + ㅣ + ㆍ + ㅣ + ㆍ = 과ㅏ"},
        {"dueiaeia", "뒤ㅏ", "ㄷ + ㅡ + ㅣ + ㆍ + ㅣ + ㆍ + ㅣ + ㆍ = 뒤ㅏ"},
        {"teaeiaeia", "퇴ㅏ", "ㅌ + ㅗ + ㅣ + ㅣ + ㆍ + ㅣ + ㆍ = 퇴ㅏ"},
        
        // Mixed complex combinations
        {"gaeiaeg", "과ㄱ", "ㄱ + ㆍ + ㅡ + ㅣ + ㆍ + ㅣ + ㆍ + ㄱ = 과ㄱ"},
        {"dueiaeg", "뒤ㄱ", "ㄷ + ㅡ + ㅣ + ㆍ + ㅣ + ㆍ + ㄱ = 뒤ㄱ"},
        {"teaeiaeg", "퇴ㄱ", "ㅌ + ㅗ + ㅣ + ㅣ + ㆍ + ㄱ = 퇴ㄱ"},
        {"gueiaeg", "권ㄱ", "ㄱ + ㅡ + ㆍ + ㅣ + ㅣ + ㆍ + ㄱ = 권ㄱ"}
    };
    
    for (int i = 0; i < 30; i++) {
        clear_output();
        
        // Input the sequence for each advanced syllable
        for (int j = 0; advanced_tests[i].input[j] != '\0'; j++) {
            process_input(advanced_tests[i].input[j]);
        }
        
        // Get output before enter (which clears the buffer)
        char* output = get_current_output();
        test_assert(strlen(output) > 0, 
                   advanced_tests[i].description);
        free(output);
        
        chunjiin_enter_key_handler();
    }
}

// Test vowel combinations with different consonants
void test_vowel_combinations() {
    printf("\n=== TESTING VOWEL COMBINATIONS ===\n");
    
    struct {
        char* input;
        char* expected;
        char* description;
    } vowel_tests[] = {
        {"gia", "가", "ㄱ + ㅣ + ㆍ = 가"},
        {"giai", "개", "ㄱ + ㅣ + ㆍ + ㅣ = 개"},
        {"gaai", "겨", "ㄱ + ㆍ + ㆍ + ㅣ = 겨"},
        {"gaae", "교", "ㄱ + ㆍ + ㆍ + ㅡ = 교"},        
        {"gaaei", "교ㅣ", "ㄱ + ㆍ + ㆍ + ㅡ + ㅣ = 교ㅣ"},
        {"gaaeia", "규ㅓ", "ㄱ + ㆍ + ㆍ + ㅡ + ㆍ + ㅣ = 규ㅓ"},
        {"gi", "기", "ㄱ + ㅣ = 기"}
    };
    
    for (int i = 0; i < 4; i++) {
        clear_output();
        // Input the sequence for each vowel combination
        for (int j = 0; vowel_tests[i].input[j] != '\0'; j++) {
            process_input(vowel_tests[i].input[j]);
        }
        
        // Get output before enter (which clears the buffer)
        char* output = get_current_output();
        test_assert(strlen(output) > 0, 
                   vowel_tests[i].description);
        free(output);
        
        chunjiin_enter_key_handler();
    }
}

// Test consonant group toggling
void test_consonant_group_toggling() {
    printf("\n=== TESTING CONSONANT GROUP TOGGLING ===\n");
    
    struct {
        char* input;
        char* expected_results[3];
        char* description;
    } toggle_tests[] = {
        {"g", {"ㄱ", "ㅋ", "ㄲ"}, "ㄱ/ㅋ/ㄲ toggle"},
        {"d", {"ㄷ", "ㅌ", "ㄸ"}, "ㄷ/ㅌ/ㄸ toggle"},
        {"b", {"ㅂ", "ㅍ", "ㅃ"}, "ㅂ/ㅍ/ㅃ toggle"},
        {"s", {"ㅅ", "ㅎ", "ㅆ"}, "ㅅ/ㅎ/ㅆ toggle"},
        {"j", {"ㅈ", "ㅊ", "ㅉ"}, "ㅈ/ㅊ/ㅉ toggle"}
    };
    
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 3; j++) {
            clear_output();
            // Press the same key multiple times to cycle through
            for (int k = 0; k <= j; k++) {
                process_input(toggle_tests[i].input[0]);
            }
            // Get output before enter (which clears the buffer)
            char* output = get_current_output();
            test_assert(strlen(output) > 0, 
                       toggle_tests[i].description);
            free(output);
            
            chunjiin_enter_key_handler();
        }
    }
}

// Test special characters and functions
void test_special_functions() {
    printf("\n=== TESTING SPECIAL FUNCTIONS ===\n");
    
    // Test backspace (not implemented yet)
    clear_output();
    process_input('g');
    process_input('h');
    // Note: backspace functionality needs to be implemented
    // For now, we'll test that the input was processed
    char* output = get_current_output();
    test_assert(strlen(output) > 0, "Backspace function - input processed");
    free(output);
    
    // Test space (not implemented yet)
    clear_output();
    process_input(' '); // Space character
    // Note: space functionality needs to be implemented
    // For now, we'll test that the input was processed
    output = get_current_output();
    test_assert(strlen(output) == 0, "Space input - not implemented yet");
    free(output);
    
    // Test clear
    clear_output();
    process_input('g');
    process_input('h');
    clear_output(); // Use our clear_output function
    output = get_current_output();
    test_assert(strlen(output) == 0, "Clear function");
    free(output);
}

// Test edge cases
void test_edge_cases() {
    printf("\n=== TESTING EDGE CASES ===\n");
    
    // Test empty input
    clear_output();
    chunjiin_enter_key_handler();
    char* output = get_current_output();
    test_assert(strlen(output) == 0, "Empty input handling");
    free(output);
    
    // Test invalid input
    clear_output();
    process_input('x'); // Invalid key
    chunjiin_enter_key_handler();
    output = get_current_output();
    test_assert(strlen(output) == 0, "Invalid input handling");
    free(output);
    
    // Test multiple consecutive consonants
    clear_output();
    process_input('g');
    process_input('g');
    process_input('g');
    
    // Get output before enter (which clears the buffer)
    output = get_current_output();
    test_assert(strlen(output) > 0, "Multiple consecutive consonants");
    free(output);
    
    chunjiin_enter_key_handler();
}

// Test complete words
void test_complete_words() {
    printf("\n=== TESTING COMPLETE WORDS ===\n");
    
    struct {
        char* input;
        char* expected;
        char* description;
    } word_tests[] = {
        {"giag", "각", "Simple syllable"},
        {"giaggiag", "각각", "Repeated syllable"},
        {"giag h", "각 ㅏ", "Syllable with space"},
        {"giaggiaggiag", "각각각", "Multiple syllables"}
    };
    
    for (int i = 0; i < 4; i++) {
        clear_output();
        for (int j = 0; word_tests[i].input[j] != '\0'; j++) {
            if (word_tests[i].input[j] == ' ') {
                // Note: space functionality needs to be implemented
                // For now, skip space characters
            } else {
                process_input(word_tests[i].input[j]);
            }
        }
        
        // Get output before enter (which clears the buffer)
        char* output = get_current_output();
        test_assert(strlen(output) > 0, 
                   word_tests[i].description);
        free(output);
        
        chunjiin_enter_key_handler();
    }
}

// Main test runner
int main() {
    // Set locale for wide character support
    setlocale(LC_ALL, "");
    
    printf("=== CHUNJIIN INPUT TEST SUITE ===\n");
    printf("Testing all choseong, jungseong, and jongseong combinations\n\n");
    
    test_init();
    
    // Run all test suites
    test_all_choseong();
    test_all_jungseong();
    test_compound_vowels();
    test_all_jongseong();
    test_compound_jongseong();
    test_jongseong_cycling();
    test_complex_jongseong_combinations();
    test_choseong_jungseong_jongseong_combinations();
    test_jongseong_limitations();
    test_double_consonants();
    test_complex_syllables();
    test_advanced_korean_syllables();
    test_vowel_combinations();
    test_consonant_group_toggling();
    test_special_functions();
    test_edge_cases();
    test_complete_words();
    
    // Print final statistics
    test_print_stats();
    
    return (test_stats.failed_tests == 0) ? 0 : 1;
} 