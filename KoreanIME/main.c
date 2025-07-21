#include "korean.h"

int main() {
    char input_buffer[MAX_OUTPUT_LEN] = "";
    wchar_t output_buffer[MAX_OUTPUT_LEN] = L"";
    size_t input_len = 0;

    init_korean_ime();

    int ch;
    while ((ch = getchar()) != EOF) {
        process_input(input_buffer, &input_len, output_buffer, ch);
    }

    cleanup_korean_ime();
    return 0;
} 