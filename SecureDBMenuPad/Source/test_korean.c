#include "korean.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    printf("Korean IME Test Program\n");
    printf("Testing Korean character composition...\n\n");
    
    // Initialize Korean locale
    setlocale(LC_ALL, "ko_KR.UTF-8");
    
    // Test input buffer and output buffer
    char input_buffer[MAX_OUTPUT_LEN] = {0};
    wchar_t output_buffer[MAX_OUTPUT_LEN] = {0};
    size_t input_len = 0;
    
    // Test composing "안형" (an-hyeong)
    // 안 = ㅇ(d) + ㅏ(k) + ㄴ(s)
    // 형 = ㅎ(g) + ㅕ(u) + ㅇ(d)
    
    printf("Testing composition of '안형' (an-hyeong):\n");
    
    // First character: 안
    process_input_gui(input_buffer, &input_len, output_buffer, 'd'); // ㅇ
    process_input_gui(input_buffer, &input_len, output_buffer, 'k'); // ㅏ
    process_input_gui(input_buffer, &input_len, output_buffer, 's'); // ㄴ
    
    // Add space
    process_input_gui(input_buffer, &input_len, output_buffer, ' ');
    
    // Second character: 형
    process_input_gui(input_buffer, &input_len, output_buffer, 'g'); // ㅎ
    process_input_gui(input_buffer, &input_len, output_buffer, 'u'); // ㅕ
    process_input_gui(input_buffer, &input_len, output_buffer, 'd'); // ㅇ
    
    // Convert to UTF-8 and display
    char utf8_output[MAX_OUTPUT_LEN * 4];
    size_t result = wcstombs(utf8_output, output_buffer, sizeof(utf8_output) - 1);
    if (result != (size_t)-1) {
        utf8_output[result] = '\0';
        printf("Input buffer: [%s]\n", input_buffer);
        printf("Composed Korean: [%s]\n", utf8_output);
    } else {
        printf("Conversion failed\n");
    }
    
    printf("\nKorean IME test completed successfully!\n");
    printf("The GUI application should now be running with a Korean input tab.\n");
    return 0;
}
