#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

// External declarations
extern wchar_t g_output_buffer[1024];
extern void initialize(void);
extern void process_input(char key);
extern void chunjiin_enter_key_handler(void);
extern void reset_current_syllable(void);
extern int wchar_to_utf8(wchar_t wc, char *utf8_buffer, size_t buffer_size);

void clear_output() {
    g_output_buffer[0] = L'\0';
    reset_current_syllable();
}

void print_current_state() {
    printf("Current output buffer: [");
    for (size_t i = 0; i < wcslen(g_output_buffer); i++) {
        char utf8_char[8];
        int bytes = wchar_to_utf8(g_output_buffer[i], utf8_char, sizeof(utf8_char));
        if (bytes > 0) {
            printf("%s", utf8_char);
        }
    }
    printf("]\n");
}

int main() {
    printf("=== TESTING JONGSEONG + DOT BEHAVIOR ===\n");
    printf("Testing: 각 + dot should show 각 + ㆍ instead of 가 + ㆍ\n");
    initialize();
    
    // Test: gia + g + a (ㄱ + ㅣ + dot + ㄱ + dot)
    printf("\nTest: gia + g + a (ㄱ + ㅣ + dot + ㄱ + dot)\n");
    clear_output();
    
    printf("Step 1: g (ㄱ)\n");
    process_input('g');
    print_current_state();
    
    printf("Step 2: i (ㅣ)\n");
    process_input('i');
    print_current_state();
    
    printf("Step 3: a (dot)\n");
    process_input('a');
    print_current_state();
    
    printf("Step 4: g (ㄱ as jongseong)\n");
    process_input('g');
    print_current_state();
    
    printf("Step 5: a (dot after jongseong)\n");
    process_input('a');
    print_current_state();
    
    printf("Expected: Should see 각 + ㆍ, not 가 + ㆍ\n");
    printf("Final result:\n");
    
    chunjiin_enter_key_handler();
    
    return 0;
} 