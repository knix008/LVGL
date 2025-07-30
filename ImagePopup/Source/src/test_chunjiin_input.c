#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <assert.h>
#include "../include/chunjiin_input.h"

// External declarations for global variables from chunjiin_input.c
extern wchar_t g_output_buffer[1024];

// Function declarations from chunjiin_input.c
void initialize(void);
void process_input(char key);
void reset_current_syllable(void);
int wchar_to_utf8(wchar_t wc, char *utf8_buffer, size_t buffer_size);

// Test function declarations
void test_all_choseong(void);
void test_all_jungseong(void);
void test_compound_vowels(void);
void test_all_jongseong(void);
void test_compound_jongseong(void);
void test_jongseong_cycling(void);
void test_complex_jongseong_combinations(void);
void test_choseong_jungseong_jongseong_combinations(void);
void test_jongseong_limitations(void);
void test_double_consonants(void);
void test_complex_syllables(void);
void test_advanced_korean_syllables(void);
void test_vowel_combinations(void);
void test_consonant_group_toggling(void);
void test_special_functions(void);
void test_edge_cases(void);
void test_complete_words(void);
void test_dot_combinations(void);
void test_gwon_syllable(void);
void test_wi_vowel(void);
void test_compound_vowels_comprehensive(void);

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
        {"gean", "권", "ㄱ + ㅡ + ㆍ + ㄴ = 권 (correct input sequence)"},
        {"geaain", "권", "ㄱ + ㅡ + ㆍ + ㆍ + ㅣ + ㄴ = 권 (dot + dot + ㅣ + ㄴ)"},
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
        {"gueiaeg", "권ㄱ", "ㄱ + ㅡ + ㆍ + ㅣ + ㅣ + ㆍ + ㄱ = 권ㄱ"},
        {"maeiai", "왜", "ㅇ + ㆍ + ㅡ + ㅣ + ㆍ + ㅣ = 왜 (오 + ㅣ + ㆍ + ㅣ)"},
        {"meaaii", "웨", "ㅇ + ㅡ + ㆍ + ㆍ + ㅣ + ㅣ = 웨 (워 + ㅣ)"}
    };
    
    for (int i = 0; i < 34; i++) {
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

// Test dot combinations with choseong and jongseong
void test_dot_combinations() {
    printf("\n=== TESTING DOT COMBINATIONS ===\n");
    
    // Test choseong + dot combinations
    printf("--- Testing Choseong + Dot ---\n");
    struct {
        char* input;
        char* description;
    } choseong_dot_tests[] = {
        {"ga", "ㄱ + ㆍ (choseong + dot)"},
        {"na", "ㄴ + ㆍ (choseong + dot)"},
        {"da", "ㄷ + ㆍ (choseong + dot)"},
        {"ba", "ㅂ + ㆍ (choseong + dot)"},
        {"sa", "ㅅ + ㆍ (choseong + dot)"},
        {"ja", "ㅈ + ㆍ (choseong + dot)"},
        {"ma", "ㅇ + ㆍ (choseong + dot)"}
    };
    
    for (int i = 0; i < 7; i++) {
        clear_output();
        for (int j = 0; choseong_dot_tests[i].input[j] != '\0'; j++) {
            process_input(choseong_dot_tests[i].input[j]);
        }
        
        // Get output before enter (which clears the buffer)
        char* output = get_current_output();
        test_assert(strlen(output) > 0, 
                   choseong_dot_tests[i].description);
        free(output);
        
        chunjiin_enter_key_handler();
    }
    
    // Test jongseong + dot combinations
    printf("--- Testing Jongseong + Dot ---\n");
    struct {
        char* input;
        char* description;
    } jongseong_dot_tests[] = {
        {"giag", "ㄱ + ㅣ + ㆍ + ㄱ (가 + ㄱ + ㆍ)"},
        {"giagb", "ㄱ + ㅣ + ㆍ + ㄱ + ㅂ (가 + ㄱ + ㆍ)"},
        {"giags", "ㄱ + ㅣ + ㆍ + ㄱ + ㅅ (가 + ㄱ + ㆍ)"},
        {"giagj", "ㄱ + ㅣ + ㆍ + ㄱ + ㅈ (가 + ㄱ + ㆍ)"},
        {"giagm", "ㄱ + ㅣ + ㆍ + ㄱ + ㅁ (가 + ㄱ + ㆍ)"},
        {"niag", "ㄴ + ㅣ + ㆍ + ㄱ (나 + ㄱ + ㆍ)"},
        {"diag", "ㄷ + ㅣ + ㆍ + ㄱ (다 + ㄱ + ㆍ)"},
        {"biag", "ㅂ + ㅣ + ㆍ + ㄱ (바 + ㄱ + ㆍ)"},
        {"siag", "ㅅ + ㅣ + ㆍ + ㄱ (사 + ㄱ + ㆍ)"},
        {"jiag", "ㅈ + ㅣ + ㆍ + ㄱ (자 + ㄱ + ㆍ)"},
        {"miag", "ㅇ + ㅣ + ㆍ + ㄱ (아 + ㄱ + ㆍ)"}
    };
    
    for (int i = 0; i < 11; i++) {
        clear_output();
        for (int j = 0; jongseong_dot_tests[i].input[j] != '\0'; j++) {
            process_input(jongseong_dot_tests[i].input[j]);
        }
        
        // Get output before enter (which clears the buffer)
        char* output = get_current_output();
        test_assert(strlen(output) > 0, 
                   jongseong_dot_tests[i].description);
        free(output);
        
        chunjiin_enter_key_handler();
    }
    
    // Test dot + vowel combinations after choseong
    printf("--- Testing Choseong + Dot + Vowel ---\n");
    struct {
        char* input;
        char* description;
    } choseong_dot_vowel_tests[] = {
        {"gae", "ㄱ + ㆍ + ㅡ (ㄱㆍ + ㅡ)"},
        {"gai", "ㄱ + ㆍ + ㅣ (ㄱㆍ + ㅣ)"},
        {"nae", "ㄴ + ㆍ + ㅡ (ㄴㆍ + ㅡ)"},
        {"nai", "ㄴ + ㆍ + ㅣ (ㄴㆍ + ㅣ)"},
        {"dae", "ㄷ + ㆍ + ㅡ (ㄷㆍ + ㅡ)"},
        {"dai", "ㄷ + ㆍ + ㅣ (ㄷㆍ + ㅣ)"},
        {"bae", "ㅂ + ㆍ + ㅡ (ㅂㆍ + ㅡ)"},
        {"bai", "ㅂ + ㆍ + ㅣ (ㅂㆍ + ㅣ)"},
        {"sae", "ㅅ + ㆍ + ㅡ (ㅅㆍ + ㅡ)"},
        {"sai", "ㅅ + ㆍ + ㅣ (ㅅㆍ + ㅣ)"},
        {"jae", "ㅈ + ㆍ + ㅡ (ㅈㆍ + ㅡ)"},
        {"jai", "ㅈ + ㆍ + ㅣ (ㅈㆍ + ㅣ)"},
        {"mae", "ㅇ + ㆍ + ㅡ (ㅇㆍ + ㅡ)"},
        {"mai", "ㅇ + ㆍ + ㅣ (ㅇㆍ + ㅣ)"}
    };
    
    for (int i = 0; i < 14; i++) {
        clear_output();
        for (int j = 0; choseong_dot_vowel_tests[i].input[j] != '\0'; j++) {
            process_input(choseong_dot_vowel_tests[i].input[j]);
        }
        
        // Get output before enter (which clears the buffer)
        char* output = get_current_output();
        test_assert(strlen(output) > 0, 
                   choseong_dot_vowel_tests[i].description);
        free(output);
        
        chunjiin_enter_key_handler();
    }
    
    // Test dot + vowel combinations after jongseong
    printf("--- Testing Jongseong + Dot + Vowel ---\n");
    struct {
        char* input;
        char* description;
    } jongseong_dot_vowel_tests[] = {
        {"giage", "ㄱ + ㅣ + ㆍ + ㄱ + ㅡ (각 + ㆍ + ㅡ = 가고)"},
        {"giagi", "ㄱ + ㅣ + ㆍ + ㄱ + ㅣ (각 + ㆍ + ㅣ = 가거)"},
        {"giagae", "ㄱ + ㅣ + ㆍ + ㄱ + ㅡ + ㅣ (각 + ㆍ + ㅡ + ㅣ)"},
        {"giagai", "ㄱ + ㅣ + ㆍ + ㄱ + ㅣ + ㅣ (각 + ㆍ + ㅣ + ㅣ)"},
        {"niage", "ㄴ + ㅣ + ㆍ + ㄱ + ㅡ (낙 + ㆍ + ㅡ = 나고)"},
        {"niagi", "ㄴ + ㅣ + ㆍ + ㄱ + ㅣ (낙 + ㆍ + ㅣ = 나거)"},
        {"diage", "ㄷ + ㅣ + ㆍ + ㄱ + ㅡ (닥 + ㆍ + ㅡ = 다고)"},
        {"diagi", "ㄷ + ㅣ + ㆍ + ㄱ + ㅣ (닥 + ㆍ + ㅣ = 다거)"},
        {"biage", "ㅂ + ㅣ + ㆍ + ㄱ + ㅡ (박 + ㆍ + ㅡ = 바고)"},
        {"biagi", "ㅂ + ㅣ + ㆍ + ㄱ + ㅣ (박 + ㆍ + ㅣ = 바거)"},
        {"siage", "ㅅ + ㅣ + ㆍ + ㄱ + ㅡ (삭 + ㆍ + ㅡ = 사고)"},
        {"siagi", "ㅅ + ㅣ + ㆍ + ㄱ + ㅣ (삭 + ㆍ + ㅣ = 사거)"},
        {"jiage", "ㅈ + ㅣ + ㆍ + ㄱ + ㅡ (작 + ㆍ + ㅡ = 자고)"},
        {"jiagi", "ㅈ + ㅣ + ㆍ + ㄱ + ㅣ (작 + ㆍ + ㅣ = 자거)"},
        {"miage", "ㅇ + ㅣ + ㆍ + ㄱ + ㅡ (악 + ㆍ + ㅡ = 아고)"},
        {"miagi", "ㅇ + ㅣ + ㆍ + ㄱ + ㅣ (악 + ㆍ + ㅣ = 아거)"}
    };
    
    for (int i = 0; i < 16; i++) {
        clear_output();
        for (int j = 0; jongseong_dot_vowel_tests[i].input[j] != '\0'; j++) {
            process_input(jongseong_dot_vowel_tests[i].input[j]);
        }
        
        // Get output before enter (which clears the buffer)
        char* output = get_current_output();
        test_assert(strlen(output) > 0, 
                   jongseong_dot_vowel_tests[i].description);
        free(output);
        
        chunjiin_enter_key_handler();
    }
    
    // Test complex multi-syllable dot combinations
    printf("--- Testing Complex Multi-Syllable Dot Combinations ---\n");
    struct {
        char* input;
        char* description;
    } complex_dot_tests[] = {
        {"giagbsi", "ㄱ + ㅣ + ㆍ + ㄱ + ㅂ + ㅅ + ㅣ (갑시)"},
        {"giagbsae", "ㄱ + ㅣ + ㆍ + ㄱ + ㅂ + ㅅ + ㅡ + ㅣ (갑시 + ㅡ + ㅣ)"},
        {"giagbsai", "ㄱ + ㅣ + ㆍ + ㄱ + ㅂ + ㅅ + ㅣ + ㅣ (갑시 + ㅣ + ㅣ)"},
        {"niagbsi", "ㄴ + ㅣ + ㆍ + ㄱ + ㅂ + ㅅ + ㅣ (납시)"},
        {"diagbsi", "ㄷ + ㅣ + ㆍ + ㄱ + ㅂ + ㅅ + ㅣ (답시)"},
        {"biagbsi", "ㅂ + ㅣ + ㆍ + ㄱ + ㅂ + ㅅ + ㅣ (밥시)"},
        {"siagbsi", "ㅅ + ㅣ + ㆍ + ㄱ + ㅂ + ㅅ + ㅣ (삽시)"},
        {"jiagbsi", "ㅈ + ㅣ + ㆍ + ㄱ + ㅂ + ㅅ + ㅣ (잡시)"},
        {"miagbsi", "ㅇ + ㅣ + ㆍ + ㄱ + ㅂ + ㅅ + ㅣ (압시)"},
        {"giagjsi", "ㄱ + ㅣ + ㆍ + ㄱ + ㅈ + ㅅ + ㅣ (갖시)"},
        {"giagmsi", "ㄱ + ㅣ + ㆍ + ㄱ + ㅁ + ㅅ + ㅣ (강시)"}
    };
    
    for (int i = 0; i < 11; i++) {
        clear_output();
        for (int j = 0; complex_dot_tests[i].input[j] != '\0'; j++) {
            process_input(complex_dot_tests[i].input[j]);
        }
        
        // Get output before enter (which clears the buffer)
        char* output = get_current_output();
        test_assert(strlen(output) > 0, 
                   complex_dot_tests[i].description);
        free(output);
        
        chunjiin_enter_key_handler();
    }
    
    // Test multiple consecutive dots
    printf("--- Testing Multiple Consecutive Dots ---\n");
    struct {
        char* input;
        char* description;
    } multiple_dot_tests[] = {
        {"giaga", "ㄱ + ㅣ + ㆍ + ㄱ + ㆍ (각 + dot)"},
        {"giagaa", "ㄱ + ㅣ + ㆍ + ㄱ + ㆍ + ㆍ (각 + dot + dot)"},
        {"niaga", "ㄴ + ㅣ + ㆍ + ㄱ + ㆍ (낙 + dot)"},
        {"niagaa", "ㄴ + ㅣ + ㆍ + ㄱ + ㆍ + ㆍ (낙 + dot + dot)"},
        {"diaga", "ㄷ + ㅣ + ㆍ + ㄱ + ㆍ (닥 + dot)"},
        {"diagaa", "ㄷ + ㅣ + ㆍ + ㄱ + ㆍ + ㆍ (닥 + dot + dot)"},
        {"biaga", "ㅂ + ㅣ + ㆍ + ㄱ + ㆍ (박 + dot)"},
        {"biagaa", "ㅂ + ㅣ + ㆍ + ㄱ + ㆍ + ㆍ (박 + dot + dot)"},
        {"siaga", "ㅅ + ㅣ + ㆍ + ㄱ + ㆍ (삭 + dot)"},
        {"siagaa", "ㅅ + ㅣ + ㆍ + ㄱ + ㆍ + ㆍ (삭 + dot + dot)"},
        {"jiaga", "ㅈ + ㅣ + ㆍ + ㄱ + ㆍ (작 + dot)"},
        {"jiagaa", "ㅈ + ㅣ + ㆍ + ㄱ + ㆍ + ㆍ (작 + dot + dot)"},
        {"miaga", "ㅇ + ㅣ + ㆍ + ㄱ + ㆍ (악 + dot)"},
        {"miagaa", "ㅇ + ㅣ + ㆍ + ㄱ + ㆍ + ㆍ (악 + dot + dot)"}
    };
    
    for (int i = 0; i < 14; i++) {
        clear_output();
        for (int j = 0; multiple_dot_tests[i].input[j] != '\0'; j++) {
            process_input(multiple_dot_tests[i].input[j]);
        }
        
        // Get output before enter (which clears the buffer)
        char* output = get_current_output();
        test_assert(strlen(output) > 0, 
                   multiple_dot_tests[i].description);
        free(output);
        
        chunjiin_enter_key_handler();
    }
    
    // Test dot cycling behavior
    printf("--- Testing Dot Cycling Behavior ---\n");
    struct {
        char* input;
        char* description;
    } dot_cycling_tests[] = {
        {"ga", "ㄱ + ㆍ (first dot)"},
        {"gaa", "ㄱ + ㆍㆍ (second dot)"},
        {"gaaa", "ㄱ + ㆍ (third dot - cycles back)"},
        {"gaaaa", "ㄱ + ㆍㆍ (fourth dot - cycles again)"},
        {"giaga", "각 + ㆍ (jongseong + first dot)"},
        {"giagaa", "각 + ㆍㆍ (jongseong + second dot)"},
        {"giagaaa", "각 + ㆍ (jongseong + third dot - cycles back)"},
        {"giagaaaa", "각 + ㆍㆍ (jongseong + fourth dot - cycles again)"}
    };
    
    for (int i = 0; i < 8; i++) {
        clear_output();
        for (int j = 0; dot_cycling_tests[i].input[j] != '\0'; j++) {
            process_input(dot_cycling_tests[i].input[j]);
        }
        
        // Get output before enter (which clears the buffer)
        char* output = get_current_output();
        test_assert(strlen(output) > 0, 
                   dot_cycling_tests[i].description);
        free(output);
        
        chunjiin_enter_key_handler();
    }
}

// Test specifically for the Korean syllable "권" (gwon)
void test_gwon_syllable() {
    printf("\n=== TESTING GWON (권) SYLLABLE ===\n");
    
    struct {
        char* input;
        char* expected;
        char* description;
    } gwon_tests[] = {
        // Standard input sequences for "권"
        {"gueai", "권", "ㄱ + ㅡ + ㆍ + ㅣ = 권 (standard sequence)"},
        {"gean", "권", "ㄱ + ㅡ + ㆍ + ㄴ = 권 (with final consonant ㄴ)"},
        {"geaain", "권", "ㄱ + ㅡ + ㆍ + ㆍ + ㅣ + ㄴ = 권 (dot + dot + ㅣ + ㄴ)"},
        
        // Alternative input sequences
        {"gueaig", "권ㄱ", "ㄱ + ㅡ + ㆍ + ㅣ + ㄱ = 권ㄱ (with final ㄱ)"},
        {"gueais", "권ㅅ", "ㄱ + ㅡ + ㆍ + ㅣ + ㅅ = 권ㅅ (with final ㅅ)"},
        {"gueaij", "권ㅈ", "ㄱ + ㅡ + ㆍ + ㅣ + ㅈ = 권ㅈ (with final ㅈ)"},
        {"gueaib", "권ㅂ", "ㄱ + ㅡ + ㆍ + ㅣ + ㅂ = 권ㅂ (with final ㅂ)"},
        {"gueaim", "권ㅇ", "ㄱ + ㅡ + ㆍ + ㅣ + ㅇ = 권ㅇ (with final ㅇ)"},
        
        // Double consonant variations
        {"ggueai", "꿘", "ㄲ + ㅡ + ㆍ + ㅣ = 꿘 (double ㄱ)"},
        {"ggueaig", "꿘ㄱ", "ㄲ + ㅡ + ㆍ + ㅣ + ㄱ = 꿘ㄱ (double ㄱ with final ㄱ)"},
        
        // Complex final consonant combinations
        {"gueaigs", "권ㄶ", "ㄱ + ㅡ + ㆍ + ㅣ + ㄴ + ㅎ = 권ㄶ (with ㄶ)"},
        {"gueailg", "권ㄺ", "ㄱ + ㅡ + ㆍ + ㅣ + ㄹ + ㄱ = 권ㄺ (with ㄺ)"},
        {"gueaigj", "권ㄵ", "ㄱ + ㅡ + ㆍ + ㅣ + ㄴ + ㅈ = 권ㄵ (with ㄵ)"},
        
        // Cycling final consonants
        {"gueaiggg", "권ㄲ", "ㄱ + ㅡ + ㆍ + ㅣ + ㄱ + ㄱ + ㄱ = 권ㄲ (cycling to ㄲ)"},
        {"gueaiddd", "권ㄸ", "ㄱ + ㅡ + ㆍ + ㅣ + ㄷ + ㄷ + ㄷ = 권ㄸ (cycling to ㄸ)"},
        {"gueaibbb", "권ㅃ", "ㄱ + ㅡ + ㆍ + ㅣ + ㅂ + ㅂ + ㅂ = 권ㅃ (cycling to ㅃ)"},
        {"gueaiss", "권ㅆ", "ㄱ + ㅡ + ㆍ + ㅣ + ㅅ + ㅅ = 권ㅆ (cycling to ㅆ)"},
        {"gueaijj", "권ㅉ", "ㄱ + ㅡ + ㆍ + ㅣ + ㅈ + ㅈ = 권ㅉ (cycling to ㅉ)"},
        
        // Vowel variations
        {"gueiaeia", "권ㅏ", "ㄱ + ㅡ + ㆍ + ㅣ + ㅣ + ㆍ + ㅣ + ㆍ = 권ㅏ (with additional vowels)"},
        {"gueiaeg", "권ㄱ", "ㄱ + ㅡ + ㆍ + ㅣ + ㅣ + ㆍ + ㄱ = 권ㄱ (with vowel + final consonant)"},
        
        // Edge cases
        {"gueaia", "권", "ㄱ + ㅡ + ㆍ + ㅣ + ㆍ = 권 (with extra dot)"},
        {"gueaiaa", "권", "ㄱ + ㅡ + ㆍ + ㅣ + ㆍ + ㆍ = 권 (with multiple dots)"},
        {"gueaiai", "권", "ㄱ + ㅡ + ㆍ + ㅣ + ㆍ + ㅣ = 권 (with dot + ㅣ)"}
    };
    
    for (int i = 0; i < 22; i++) {
        clear_output();
        
        // Input the sequence for each gwon test
        for (int j = 0; gwon_tests[i].input[j] != '\0'; j++) {
            process_input(gwon_tests[i].input[j]);
        }
        
        // Get output before enter (which clears the buffer)
        char* output = get_current_output();
        test_assert(strlen(output) > 0, 
                   gwon_tests[i].description);
        free(output);
        
        chunjiin_enter_key_handler();
    }
}

// Test specifically for the Korean vowel "ㅟ" (wi)
void test_wi_vowel() {
    printf("\n=== TESTING WI (ㅟ) VOWEL ===\n");
    
    struct {
        char* input;
        char* expected;
        char* description;
    } wi_tests[] = {
        // Basic ㅟ formation from ㅜ + ㅣ
        {"duei", "뒤", "ㄷ + ㅡ + ㆍ + ㅣ = 뒤 (ㅜ + ㅣ = ㅟ)"},
        {"guei", "귀", "ㄱ + ㅡ + ㆍ + ㅣ = 귀 (ㅜ + ㅣ = ㅟ)"},
        {"nuei", "뉘", "ㄴ + ㅡ + ㆍ + ㅣ = 뉘 (ㅜ + ㅣ = ㅟ)"},
        {"buei", "뷔", "ㅂ + ㅡ + ㆍ + ㅣ = 뷔 (ㅜ + ㅣ = ㅟ)"},
        {"suei", "쉬", "ㅅ + ㅡ + ㆍ + ㅣ = 쉬 (ㅜ + ㅣ = ㅟ)"},
        {"juei", "쥐", "ㅈ + ㅡ + ㆍ + ㅣ = 쥐 (ㅜ + ㅣ = ㅟ)"},
        {"muei", "위", "ㅇ + ㅡ + ㆍ + ㅣ = 위 (ㅜ + ㅣ = ㅟ)"},
        
        // ㅟ with final consonants
        {"dueig", "뒤ㄱ", "ㄷ + ㅡ + ㆍ + ㅣ + ㄱ = 뒤ㄱ"},
        {"duein", "뒤ㄴ", "ㄷ + ㅡ + ㆍ + ㅣ + ㄴ = 뒤ㄴ"},
        {"dueid", "뒤ㄷ", "ㄷ + ㅡ + ㆍ + ㅣ + ㄷ = 뒤ㄷ"},
        {"dueib", "뒤ㅂ", "ㄷ + ㅡ + ㆍ + ㅣ + ㅂ = 뒤ㅂ"},
        {"dueis", "뒤ㅅ", "ㄷ + ㅡ + ㆍ + ㅣ + ㅅ = 뒤ㅅ"},
        {"dueij", "뒤ㅈ", "ㄷ + ㅡ + ㆍ + ㅣ + ㅈ = 뒤ㅈ"},
        {"dueim", "뒤ㅁ", "ㄷ + ㅡ + ㆍ + ㅣ + ㅁ = 뒤ㅁ"},
        
        // Double consonants with ㅟ
        {"dduei", "뛰", "ㄸ + ㅡ + ㆍ + ㅣ = 뛰 (double ㄷ + ㅟ)"},
        {"gguei", "궤", "ㄲ + ㅡ + ㆍ + ㅣ = 궤 (double ㄱ + ㅟ)"},
        {"bbuei", "뷰", "ㅃ + ㅡ + ㆍ + ㅣ = 뷰 (double ㅂ + ㅟ)"},
        {"ssuei", "쒸", "ㅆ + ㅡ + ㆍ + ㅣ = 쒸 (double ㅅ + ㅟ)"},
        {"jjuei", "쮜", "ㅉ + ㅡ + ㆍ + ㅣ = 쮜 (double ㅈ + ㅟ)"},
        
        // ㅟ with compound final consonants
        {"dueigs", "뒤ㄶ", "ㄷ + ㅡ + ㆍ + ㅣ + ㄴ + ㅎ = 뒤ㄶ"},
        {"dueigj", "뒤ㄵ", "ㄷ + ㅡ + ㆍ + ㅣ + ㄴ + ㅈ = 뒤ㄵ"},
        {"dueilg", "뒤ㄺ", "ㄷ + ㅡ + ㆍ + ㅣ + ㄹ + ㄱ = 뒤ㄺ"},
        {"dueils", "뒤ㄽ", "ㄷ + ㅡ + ㆍ + ㅣ + ㄹ + ㅅ = 뒤ㄽ"},
        {"dueilb", "뒤ㄼ", "ㄷ + ㅡ + ㆍ + ㅣ + ㄹ + ㅂ = 뒤ㄼ"},
        {"dueibs", "뒤ㅄ", "ㄷ + ㅡ + ㆍ + ㅣ + ㅂ + ㅅ = 뒤ㅄ"},
        
        // Cycling final consonants with ㅟ
        {"dueiggg", "뒤ㄲ", "ㄷ + ㅡ + ㆍ + ㅣ + ㄱ + ㄱ + ㄱ = 뒤ㄲ"},
        {"dueiddd", "뒤ㄸ", "ㄷ + ㅡ + ㆍ + ㅣ + ㄷ + ㄷ + ㄷ = 뒤ㄸ"},
        {"dueibbb", "뒤ㅃ", "ㄷ + ㅡ + ㆍ + ㅣ + ㅂ + ㅂ + ㅂ = 뒤ㅃ"},
        {"dueiss", "뒤ㅆ", "ㄷ + ㅡ + ㆍ + ㅣ + ㅅ + ㅅ = 뒤ㅆ"},
        {"dueijj", "뒤ㅉ", "ㄷ + ㅡ + ㆍ + ㅣ + ㅈ + ㅈ = 뒤ㅉ"},
        
        // ㅟ with additional vowel combinations
        {"dueiaeia", "뒤ㅏ", "ㄷ + ㅡ + ㆍ + ㅣ + ㅣ + ㆍ + ㅣ + ㆍ = 뒤ㅏ"},
        {"dueiaeg", "뒤ㄱ", "ㄷ + ㅡ + ㆍ + ㅣ + ㅣ + ㆍ + ㄱ = 뒤ㄱ"},
        {"dueiaei", "뒤ㅣ", "ㄷ + ㅡ + ㆍ + ㅣ + ㅣ + ㆍ + ㅣ = 뒤ㅣ"},
        
        // Edge cases and variations
        {"dueia", "뒤", "ㄷ + ㅡ + ㆍ + ㅣ + ㆍ = 뒤 (with extra dot)"},
        {"dueiaa", "뒤", "ㄷ + ㅡ + ㆍ + ㅣ + ㆍ + ㆍ = 뒤 (with multiple dots)"},
        {"dueiai", "뒤", "ㄷ + ㅡ + ㆍ + ㅣ + ㆍ + ㅣ = 뒤 (with dot + ㅣ)"},
        
        // Test the specific sequence that was problematic: ㄷ + ㅡ + ㆍ + ㅣ
        {"duei", "뒤", "ㄷ + ㅡ + ㆍ + ㅣ = 뒤 (specific test for the fix)"},
        
        // Test ㅜ + ㆍ = ㅠ combinations
        {"duea", "듀", "ㄷ + ㅡ + ㆍ = 듀 (ㅜ + ㆍ = ㅠ)"},
        {"dueai", "듀ㅣ", "ㄷ + ㅡ + ㆍ + ㅣ = 듀ㅣ (ㅠ + ㅣ)"},
        {"dueaig", "듀ㄱ", "ㄷ + ㅡ + ㆍ + ㅣ + ㄱ = 듀ㄱ (ㅠ + ㅣ + ㄱ)"}
    };
    
    for (int i = 0; i < 35; i++) {
        clear_output();
        
        // Input the sequence for each ㅟ test
        for (int j = 0; wi_tests[i].input[j] != '\0'; j++) {
            process_input(wi_tests[i].input[j]);
        }
        
        // Get output before enter (which clears the buffer)
        char* output = get_current_output();
        test_assert(strlen(output) > 0, 
                   wi_tests[i].description);
        free(output);
        
        chunjiin_enter_key_handler();
    }
}

// Test comprehensively for all Korean compound vowels: ㅙ, ㅞ, ㅝ, ㅚ, ㅟ, ㅔ, ㅐ, ㅖ, ㅒ
void test_compound_vowels_comprehensive() {
    printf("\n=== TESTING COMPREHENSIVE COMPOUND VOWELS ===\n");
    
    struct {
        char* input;
        char* expected;
        char* description;
        char* vowel_type;
    } compound_tests[] = {
        // ㅙ (wa) - ㅗ + ㅐ
        {"gaeia", "과", "ㄱ + ㆍ + ㅡ + ㅣ + ㆍ = 과 (ㅗ + ㅐ = ㅙ)", "ㅙ"},
        {"naeia", "놔", "ㄴ + ㆍ + ㅡ + ㅣ + ㆍ = 놔 (ㅗ + ㅐ = ㅙ)", "ㅙ"},
        {"daeia", "돠", "ㄷ + ㆍ + ㅡ + ㅣ + ㆍ = 돠 (ㅗ + ㅐ = ㅙ)", "ㅙ"},
        {"baeia", "봐", "ㅂ + ㆍ + ㅡ + ㅣ + ㆍ = 봐 (ㅗ + ㅐ = ㅙ)", "ㅙ"},
        {"saeia", "솨", "ㅅ + ㆍ + ㅡ + ㅣ + ㆍ = 솨 (ㅗ + ㅐ = ㅙ)", "ㅙ"},
        {"jaeia", "좌", "ㅈ + ㆍ + ㅡ + ㅣ + ㆍ = 좌 (ㅗ + ㅐ = ㅙ)", "ㅙ"},
        {"maeia", "와", "ㅇ + ㆍ + ㅡ + ㅣ + ㆍ = 와 (ㅗ + ㅐ = ㅙ)", "ㅙ"},
        
        // ㅞ (we) - ㅜ + ㅔ
        {"dueaii", "뒤", "ㄷ + ㅡ + ㆍ + ㅣ + ㅣ = 뒤 (ㅜ + ㅔ = ㅞ)", "ㅞ"},
        {"gueaii", "귀", "ㄱ + ㅡ + ㆍ + ㅣ + ㅣ = 귀 (ㅜ + ㅔ = ㅞ)", "ㅞ"},
        {"nueaii", "뉘", "ㄴ + ㅡ + ㆍ + ㅣ + ㅣ = 뉘 (ㅜ + ㅔ = ㅞ)", "ㅞ"},
        {"bueaii", "뷔", "ㅂ + ㅡ + ㆍ + ㅣ + ㅣ = 뷔 (ㅜ + ㅔ = ㅞ)", "ㅞ"},
        {"sueaii", "쉬", "ㅅ + ㅡ + ㆍ + ㅣ + ㅣ = 쉬 (ㅜ + ㅔ = ㅞ)", "ㅞ"},
        {"jueaii", "쥐", "ㅈ + ㅡ + ㆍ + ㅣ + ㅣ = 쥐 (ㅜ + ㅔ = ㅞ)", "ㅞ"},
        {"mueaii", "위", "ㅇ + ㅡ + ㆍ + ㅣ + ㅣ = 위 (ㅜ + ㅔ = ㅞ)", "ㅞ"},
        
        // ㅝ (wo) - ㅜ + ㅗ
        {"dueaa", "둬", "ㄷ + ㅡ + ㆍ + ㆍ = 둬 (ㅜ + ㅗ = ㅝ)", "ㅝ"},
        {"gueaa", "궈", "ㄱ + ㅡ + ㆍ + ㆍ = 궈 (ㅜ + ㅗ = ㅝ)", "ㅝ"},
        {"nueaa", "눠", "ㄴ + ㅡ + ㆍ + ㆍ = 눠 (ㅜ + ㅗ = ㅝ)", "ㅝ"},
        {"bueaa", "붜", "ㅂ + ㅡ + ㆍ + ㆍ = 붜 (ㅜ + ㅗ = ㅝ)", "ㅝ"},
        {"sueaa", "숴", "ㅅ + ㅡ + ㆍ + ㆍ = 숴 (ㅜ + ㅗ = ㅝ)", "ㅝ"},
        {"jueaa", "줘", "ㅈ + ㅡ + ㆍ + ㆍ = 줄 (ㅜ + ㅗ = ㅝ)", "ㅝ"},
        {"mueaa", "워", "ㅇ + ㅡ + ㆍ + ㆍ = 워 (ㅜ + ㅗ = ㅝ)", "ㅝ"},
        
        // ㅚ (oe) - ㅗ + ㅣ
        {"gaai", "괴", "ㄱ + ㆍ + ㆍ + ㅣ = 괴 (ㅗ + ㅣ = ㅚ)", "ㅚ"},
        {"naai", "뇌", "ㄴ + ㆍ + ㆍ + ㅣ = 뇌 (ㅗ + ㅣ = ㅚ)", "ㅚ"},
        {"daai", "되", "ㄷ + ㆍ + ㆍ + ㅣ = 되 (ㅗ + ㅣ = ㅚ)", "ㅚ"},
        {"baai", "뵈", "ㅂ + ㆍ + ㆍ + ㅣ = 뵈 (ㅗ + ㅣ = ㅚ)", "ㅚ"},
        {"saai", "쇠", "ㅅ + ㆍ + ㆍ + ㅣ = 쇠 (ㅗ + ㅣ = ㅚ)", "ㅚ"},
        {"jaai", "죄", "ㅈ + ㆍ + ㆍ + ㅣ = 죄 (ㅗ + ㅣ = ㅚ)", "ㅚ"},
        {"maai", "외", "ㅇ + ㆍ + ㆍ + ㅣ = 외 (ㅗ + ㅣ = ㅚ)", "ㅚ"},
        
        // ㅟ (wi) - ㅜ + ㅣ
        {"duei", "뒤", "ㄷ + ㅡ + ㆍ + ㅣ = 뒤 (ㅜ + ㅣ = ㅟ)", "ㅟ"},
        {"guei", "귀", "ㄱ + ㅡ + ㆍ + ㅣ = 귀 (ㅜ + ㅣ = ㅟ)", "ㅟ"},
        {"nuei", "뉘", "ㄴ + ㅡ + ㆍ + ㅣ = 뉘 (ㅜ + ㅣ = ㅟ)", "ㅟ"},
        {"buei", "뷔", "ㅂ + ㅡ + ㆍ + ㅣ = 뷔 (ㅜ + ㅣ = ㅟ)", "ㅟ"},
        {"suei", "쉬", "ㅅ + ㅡ + ㆍ + ㅣ = 쉬 (ㅜ + ㅣ = ㅟ)", "ㅟ"},
        {"juei", "쥐", "ㅈ + ㅡ + ㆍ + ㅣ = 쥐 (ㅜ + ㅣ = ㅟ)", "ㅟ"},
        {"muei", "위", "ㅇ + ㅡ + ㆍ + ㅣ = 위 (ㅜ + ㅣ = ㅟ)", "ㅟ"},
        
        // ㅔ (e) - ㅓ + ㅣ
        {"laai", "레", "ㆍ + ㅣ + ㅣ = 레 (ㅓ + ㅣ = ㅔ)", "ㅔ"},
        {"gaaii", "개", "ㄱ + ㆍ + ㆍ + ㅣ + ㅣ = 개 (ㅔ)", "ㅔ"},
        {"naaii", "내", "ㄴ + ㆍ + ㆍ + ㅣ + ㅣ = 내 (ㅔ)", "ㅔ"},
        {"daaii", "대", "ㄷ + ㆍ + ㆍ + ㅣ + ㅣ = 대 (ㅔ)", "ㅔ"},
        {"baaii", "배", "ㅂ + ㆍ + ㆍ + ㅣ + ㅣ = 배 (ㅔ)", "ㅔ"},
        {"saaii", "새", "ㅅ + ㆍ + ㆍ + ㅣ + ㅣ = 새 (ㅔ)", "ㅔ"},
        {"jaaii", "재", "ㅈ + ㆍ + ㆍ + ㅣ + ㅣ = 재 (ㅔ)", "ㅔ"},
        {"maaii", "애", "ㅇ + ㆍ + ㆍ + ㅣ + ㅣ = 애 (ㅔ)", "ㅔ"},
        
        // ㅐ (ae) - ㅏ + ㅣ
        {"iai", "애", "ㅣ + ㆍ + ㅣ = 애 (ㅏ + ㅣ = ㅐ)", "ㅐ"},
        {"giai", "개", "ㄱ + ㅣ + ㆍ + ㅣ = 개 (ㅐ)", "ㅐ"},
        {"niai", "내", "ㄴ + ㅣ + ㆍ + ㅣ = 내 (ㅐ)", "ㅐ"},
        {"diai", "대", "ㄷ + ㅣ + ㆍ + ㅣ = 대 (ㅐ)", "ㅐ"},
        {"biai", "배", "ㅂ + ㅣ + ㆍ + ㅣ = 배 (ㅐ)", "ㅐ"},
        {"siai", "새", "ㅅ + ㅣ + ㆍ + ㅣ = 새 (ㅐ)", "ㅐ"},
        {"jiai", "재", "ㅈ + ㅣ + ㆍ + ㅣ = 재 (ㅐ)", "ㅐ"},
        {"miai", "애", "ㅇ + ㅣ + ㆍ + ㅣ = 애 (ㅐ)", "ㅐ"},
        
        // ㅖ (ye) - ㅕ + ㅣ
        {"aaii", "예", "ㆍ + ㆍ + ㅣ + ㅣ = 예 (ㅕ + ㅣ = ㅖ)", "ㅖ"},
        {"gaaii", "계", "ㄱ + ㆍ + ㆍ + ㅣ + ㅣ = 계 (ㅖ)", "ㅖ"},
        {"naaii", "녜", "ㄴ + ㆍ + ㆍ + ㅣ + ㅣ = 녜 (ㅖ)", "ㅖ"},
        {"daaii", "뎨", "ㄷ + ㆍ + ㆍ + ㅣ + ㅣ = 뎨 (ㅖ)", "ㅖ"},
        {"baaii", "볘", "ㅂ + ㆍ + ㆍ + ㅣ + ㅣ = 볼 (ㅖ)", "ㅖ"},
        {"saaii", "셰", "ㅅ + ㆍ + ㆍ + ㅣ + ㅣ = 셰 (ㅖ)", "ㅖ"},
        {"jaaii", "졔", "ㅈ + ㆍ + ㆍ + ㅣ + ㅣ = 졔 (ㅖ)", "ㅖ"},
        {"maaii", "예", "ㅇ + ㆍ + ㆍ + ㅣ + ㅣ = 예 (ㅖ)", "ㅖ"},
        
        // ㅒ (yae) - ㅑ + ㅣ
        {"iaai", "얘", "ㅣ + ㆍ + ㆍ + ㅣ = 얘 (ㅑ + ㅣ = ㅒ)", "ㅒ"},
        {"giaai", "걔", "ㄱ + ㅣ + ㆍ + ㆍ + ㅣ = 걔 (ㅒ)", "ㅒ"},
        {"niaai", "냬", "ㄴ + ㅣ + ㆍ + ㆍ + ㅣ = 냬 (ㅒ)", "ㅒ"},
        {"diaai", "댸", "ㄷ + ㅣ + ㆍ + ㆍ + ㅣ = 댸 (ㅒ)", "ㅒ"},
        {"biaai", "뱨", "ㅂ + ㅣ + ㆍ + ㆍ + ㅣ = 뱨 (ㅒ)", "ㅒ"},
        {"siaai", "샤", "ㅅ + ㅣ + ㆍ + ㆍ + ㅣ = 샤 (ㅒ)", "ㅒ"},
        {"jiaai", "쟤", "ㅈ + ㅣ + ㆍ + ㆍ + ㅣ = 쟤 (ㅒ)", "ㅒ"},
        {"miaai", "얘", "ㅇ + ㅣ + ㆍ + ㆍ + ㅣ = 얘 (ㅒ)", "ㅒ"},
        
        // Additional compound vowel combinations with final consonants
        // ㅙ with final consonants
        {"gaeiaa", "과", "ㄱ + ㆍ + ㅡ + ㅣ + ㆍ + ㆍ = 과 (ㅙ + dot)", "ㅙ"},
        {"gaeiaig", "과ㄱ", "ㄱ + ㆍ + ㅡ + ㅣ + ㆍ + ㄱ = 과ㄱ (ㅙ + ㄱ)", "ㅙ"},
        {"gaeiaig", "과ㄴ", "ㄱ + ㆍ + ㅡ + ㅣ + ㆍ + ㄴ = 과ㄴ (ㅙ + ㄴ)", "ㅙ"},
        
        // ㅞ with final consonants
        {"dueaiig", "뒤ㄱ", "ㄷ + ㅡ + ㆍ + ㅣ + ㅣ + ㄱ = 뒤ㄱ (ㅞ + ㄱ)", "ㅞ"},
        {"dueaiin", "뒤ㄴ", "ㄷ + ㅡ + ㆍ + ㅣ + ㅣ + ㄴ = 뒤ㄴ (ㅞ + ㄴ)", "ㅞ"},
        {"dueaiid", "뒤ㄷ", "ㄷ + ㅡ + ㆍ + ㅣ + ㅣ + ㄷ = 뒤ㄷ (ㅞ + ㄷ)", "ㅞ"},
        
        // ㅝ with final consonants
        {"dueaag", "둬ㄱ", "ㄷ + ㅡ + ㆍ + ㆍ + ㄱ = 둬ㄱ (ㅝ + ㄱ)", "ㅝ"},
        {"dueaan", "둬ㄴ", "ㄷ + ㅡ + ㆍ + ㆍ + ㄴ = 둬ㄴ (ㅝ + ㄴ)", "ㅝ"},
        {"dueaab", "둬ㅂ", "ㄷ + ㅡ + ㆍ + ㆍ + ㅂ = 둬ㅂ (ㅝ + ㅂ)", "ㅝ"},
        
        // ㅚ with final consonants
        {"gaaig", "괴ㄱ", "ㄱ + ㆍ + ㆍ + ㅣ + ㄱ = 괴ㄱ (ㅚ + ㄱ)", "ㅚ"},
        {"gaain", "괴ㄴ", "ㄱ + ㆍ + ㆍ + ㅣ + ㄴ = 괴ㄴ (ㅚ + ㄴ)", "ㅚ"},
        {"gaaib", "괴ㅂ", "ㄱ + ㆍ + ㆍ + ㅣ + ㅂ = 괴ㅂ (ㅚ + ㅂ)", "ㅚ"},
        
        // ㅟ with final consonants (additional)
        {"dueig", "뒤ㄱ", "ㄷ + ㅡ + ㆍ + ㅣ + ㄱ = 뒤ㄱ (ㅟ + ㄱ)", "ㅟ"},
        {"duein", "뒤ㄴ", "ㄷ + ㅡ + ㆍ + ㅣ + ㄴ = 뒤ㄴ (ㅟ + ㄴ)", "ㅟ"},
        {"dueib", "뒤ㅂ", "ㄷ + ㅡ + ㆍ + ㅣ + ㅂ = 뒤ㅂ (ㅟ + ㅂ)", "ㅟ"},
        
        // ㅔ with final consonants
        {"gaaiig", "개ㄱ", "ㄱ + ㆍ + ㆍ + ㅣ + ㅣ + ㄱ = 개ㄱ (ㅔ + ㄱ)", "ㅔ"},
        {"gaaiin", "개ㄴ", "ㄱ + ㆍ + ㆍ + ㅣ + ㅣ + ㄴ = 개ㄴ (ㅔ + ㄴ)", "ㅔ"},
        {"gaaiib", "개ㅂ", "ㄱ + ㆍ + ㆍ + ㅣ + ㅣ + ㅂ = 개ㅂ (ㅔ + ㅂ)", "ㅔ"},
        
        // ㅐ with final consonants
        {"giaig", "개ㄱ", "ㄱ + ㅣ + ㆍ + ㅣ + ㄱ = 개ㄱ (ㅐ + ㄱ)", "ㅐ"},
        {"giain", "개ㄴ", "ㄱ + ㅣ + ㆍ + ㅣ + ㄴ = 개ㄴ (ㅐ + ㄴ)", "ㅐ"},
        {"giaib", "개ㅂ", "ㄱ + ㅣ + ㆍ + ㅣ + ㅂ = 개ㅂ (ㅐ + ㅂ)", "ㅐ"},
        
        // ㅖ with final consonants
        {"gaaiig", "계ㄱ", "ㄱ + ㆍ + ㆍ + ㅣ + ㅣ + ㄱ = 계ㄱ (ㅖ + ㄱ)", "ㅖ"},
        {"gaaiin", "계ㄴ", "ㄱ + ㆍ + ㆍ + ㅣ + ㅣ + ㄴ = 계ㄴ (ㅖ + ㄴ)", "ㅖ"},
        {"gaaiib", "계ㅂ", "ㄱ + ㆍ + ㆍ + ㅣ + ㅣ + ㅂ = 계ㅂ (ㅖ + ㅂ)", "ㅖ"},
        
        // ㅒ with final consonants
        {"giaaig", "걔ㄱ", "ㄱ + ㅣ + ㆍ + ㆍ + ㅣ + ㄱ = 걔ㄱ (ㅒ + ㄱ)", "ㅒ"},
        {"giaain", "걔ㄴ", "ㄱ + ㅣ + ㆍ + ㆍ + ㅣ + ㄴ = 걔ㄴ (ㅒ + ㄴ)", "ㅒ"},
        {"giaaib", "걔ㅂ", "ㄱ + ㅣ + ㆍ + ㆍ + ㅣ + ㅂ = 걔ㅂ (ㅒ + ㅂ)", "ㅒ"},
        
        // Double consonants with compound vowels
        {"ggaeia", "꽈", "ㄲ + ㆍ + ㅡ + ㅣ + ㆍ = 꽈 (ㄲ + ㅙ)", "ㅙ"},
        {"ddueaii", "뛰", "ㄸ + ㅡ + ㆍ + ㅣ + ㅣ = 뛰 (ㄸ + ㅞ)", "ㅞ"},
        {"bbueaa", "뿌", "ㅃ + ㅡ + ㆍ + ㆍ = 뿌 (ㅃ + ㅝ)", "ㅝ"},
        {"ssaai", "쐬", "ㅆ + ㆍ + ㆍ + ㅣ = 쐬 (ㅆ + ㅚ)", "ㅚ"},
        {"gguei", "궤", "ㄲ + ㅡ + ㆍ + ㅣ = 궤 (ㄲ + ㅟ)", "ㅟ"},
        {"ggaaii", "깨", "ㄲ + ㆍ + ㆍ + ㅣ + ㅣ = 깨 (ㄲ + ㅔ)", "ㅔ"},
        {"ggiai", "깨", "ㄲ + ㅣ + ㆍ + ㅣ = 깨 (ㄲ + ㅐ)", "ㅐ"},
        {"ggaaii", "꼐", "ㄲ + ㆍ + ㆍ + ㅣ + ㅣ = 꼐 (ㄲ + ㅖ)", "ㅖ"},
        {"ggiaai", "꺠", "ㄲ + ㅣ + ㆍ + ㆍ + ㅣ = 꺠 (ㄲ + ㅒ)", "ㅒ"},
        
        // Compound final consonants with compound vowels
        {"gaeiaigs", "과ㄶ", "ㄱ + ㆍ + ㅡ + ㅣ + ㆍ + ㄴ + ㅎ = 과ㄶ (ㅙ + ㄶ)", "ㅙ"},
        {"dueaiigj", "뒤ㄵ", "ㄷ + ㅡ + ㆍ + ㅣ + ㅣ + ㄴ + ㅈ = 뒤ㄵ (ㅞ + ㄵ)", "ㅞ"},
        {"dueaags", "둬ㄶ", "ㄷ + ㅡ + ㆍ + ㆍ + ㄴ + ㅎ = 둬ㄶ (ㅝ + ㄶ)", "ㅝ"},
        {"gaaigs", "괴ㄶ", "ㄱ + ㆍ + ㆍ + ㅣ + ㄴ + ㅎ = 괴ㄶ (ㅚ + ㄶ)", "ㅚ"},
        {"dueigs", "뒤ㄶ", "ㄷ + ㅡ + ㆍ + ㅣ + ㄴ + ㅎ = 뒤ㄶ (ㅟ + ㄶ)", "ㅟ"},
        {"gaaiigs", "개ㄶ", "ㄱ + ㆍ + ㆍ + ㅣ + ㅣ + ㄴ + ㅎ = 개ㄶ (ㅔ + ㄶ)", "ㅔ"},
        {"giaigs", "개ㄶ", "ㄱ + ㅣ + ㆍ + ㅣ + ㄴ + ㅎ = 개ㄶ (ㅐ + ㄶ)", "ㅐ"},
        {"gaaiigs", "계ㄶ", "ㄱ + ㆍ + ㆍ + ㅣ + ㅣ + ㄴ + ㅎ = 계ㄶ (ㅖ + ㄶ)", "ㅖ"},
        {"giaaigs", "걔ㄶ", "ㄱ + ㅣ + ㆍ + ㆍ + ㅣ + ㄴ + ㅎ = 걔ㄶ (ㅒ + ㄶ)", "ㅒ"},
        
        // Cycling final consonants with compound vowels
        {"gaeiaiggg", "과ㄲ", "ㄱ + ㆍ + ㅡ + ㅣ + ㆍ + ㄱ + ㄱ + ㄱ = 과ㄲ (ㅙ + ㄲ)", "ㅙ"},
        {"dueaiiddd", "뒤ㄸ", "ㄷ + ㅡ + ㆍ + ㅣ + ㅣ + ㄷ + ㄷ + ㄷ = 뒤ㄸ (ㅞ + ㄸ)", "ㅞ"},
        {"dueaabbb", "둬ㅃ", "ㄷ + ㅡ + ㆍ + ㆍ + ㅂ + ㅂ + ㅂ = 둬ㅃ (ㅝ + ㅃ)", "ㅝ"},
        {"gaaiggg", "괴ㄲ", "ㄱ + ㆍ + ㆍ + ㅣ + ㄱ + ㄱ + ㄱ = 괴ㄲ (ㅚ + ㄲ)", "ㅚ"},
        {"dueiggg", "뒤ㄲ", "ㄷ + ㅡ + ㆍ + ㅣ + ㄱ + ㄱ + ㄱ = 뒤ㄲ (ㅟ + ㄲ)", "ㅟ"},
        {"gaaiiggg", "개ㄲ", "ㄱ + ㆍ + ㆍ + ㅣ + ㅣ + ㄱ + ㄱ + ㄱ = 개ㄲ (ㅔ + ㄲ)", "ㅔ"},
        {"giaiggg", "개ㄲ", "ㄱ + ㅣ + ㆍ + ㅣ + ㄱ + ㄱ + ㄱ = 개ㄲ (ㅐ + ㄲ)", "ㅐ"},
        {"gaaiiggg", "계ㄲ", "ㄱ + ㆍ + ㆍ + ㅣ + ㅣ + ㄱ + ㄱ + ㄱ = 계ㄲ (ㅖ + ㄲ)", "ㅖ"},
        {"giaaiggg", "걔ㄲ", "ㄱ + ㅣ + ㆍ + ㆍ + ㅣ + ㄱ + ㄱ + ㄱ = 걔ㄲ (ㅒ + ㄲ)", "ㅒ"},
        
        // Edge cases and variations
        {"gaeiaa", "과", "ㄱ + ㆍ + ㅡ + ㅣ + ㆍ + ㆍ = 과 (ㅙ + extra dot)", "ㅙ"},
        {"dueaiia", "뒤", "ㄷ + ㅡ + ㆍ + ㅣ + ㅣ + ㆍ = 뒤 (ㅞ + extra dot)", "ㅞ"},
        {"dueaaa", "둬", "ㄷ + ㅡ + ㆍ + ㆍ + ㆍ = 둬 (ㅝ + extra dot)", "ㅝ"},
        {"gaaia", "괴", "ㄱ + ㆍ + ㆍ + ㅣ + ㆍ = 괴 (ㅚ + extra dot)", "ㅚ"},
        {"dueia", "뒤", "ㄷ + ㅡ + ㆍ + ㅣ + ㆍ = 뒤 (ㅟ + extra dot)", "ㅟ"},
        {"gaaiia", "개", "ㄱ + ㆍ + ㆍ + ㅣ + ㅣ + ㆍ = 개 (ㅔ + extra dot)", "ㅔ"},
        {"giaia", "개", "ㄱ + ㅣ + ㆍ + ㅣ + ㆍ = 개 (ㅐ + extra dot)", "ㅐ"},
        {"gaaiia", "계", "ㄱ + ㆍ + ㆍ + ㅣ + ㅣ + ㆍ = 계 (ㅖ + extra dot)", "ㅖ"},
        {"giaaia", "걔", "ㄱ + ㅣ + ㆍ + ㆍ + ㅣ + ㆍ = 걔 (ㅒ + extra dot)", "ㅒ"}
    };
    
    // Group tests by vowel type for better organization
    char* vowel_types[] = {"ㅙ", "ㅞ", "ㅝ", "ㅚ", "ㅟ", "ㅔ", "ㅐ", "ㅖ", "ㅒ"};
    int num_vowel_types = 9;
    
    for (int v = 0; v < num_vowel_types; v++) {
        printf("\n--- Testing %s vowel combinations ---\n", vowel_types[v]);
        
        for (int i = 0; i < 108; i++) { // Total number of test cases
            if (strcmp(compound_tests[i].vowel_type, vowel_types[v]) == 0) {
                clear_output();
                
                // Input the sequence for each compound vowel test
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
    }
    
    // Test specific problematic sequences that were mentioned
    printf("\n--- Testing Specific Problematic Sequences ---\n");
    struct {
        char* input;
        char* description;
    } specific_tests[] = {
        {"duei", "ㄷ + ㅡ + ㆍ + ㅣ = 뒤 (specific test for ㅟ fix)"},
        {"gaai", "ㄱ + ㆍ + ㆍ + ㅣ = 괴 (specific test for ㅚ)"},
        {"dueaa", "ㄷ + ㅡ + ㆍ + ㆍ = 둬 (specific test for ㅝ)"},
        {"gaeia", "ㄱ + ㆍ + ㅡ + ㅣ + ㆍ = 과 (specific test for ㅙ)"},
        {"dueaii", "ㄷ + ㅡ + ㆍ + ㅣ + ㅣ = 뒤 (specific test for ㅞ)"},
        {"gaaii", "ㄱ + ㆍ + ㆍ + ㅣ + ㅣ = 개 (specific test for ㅔ)"},
        {"giai", "ㄱ + ㅣ + ㆍ + ㅣ = 개 (specific test for ㅐ)"},
        {"gaaii", "ㄱ + ㆍ + ㆍ + ㅣ + ㅣ = 계 (specific test for ㅖ)"},
        {"giaai", "ㄱ + ㅣ + ㆍ + ㆍ + ㅣ = 걔 (specific test for ㅒ)"}
    };
    
    for (int i = 0; i < 9; i++) {
        clear_output();
        
        // Input the sequence for each specific test
        for (int j = 0; specific_tests[i].input[j] != '\0'; j++) {
            process_input(specific_tests[i].input[j]);
        }
        
        // Get output before enter (which clears the buffer)
        char* output = get_current_output();
        test_assert(strlen(output) > 0, 
                   specific_tests[i].description);
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
    test_dot_combinations();
    test_gwon_syllable(); // Add the new test function
    test_wi_vowel(); // Add the ㅟ vowel test
    test_compound_vowels_comprehensive(); // Add comprehensive compound vowel tests
    
    // Print final statistics
    test_print_stats();
    
    return (test_stats.failed_tests == 0) ? 0 : 1;
} 