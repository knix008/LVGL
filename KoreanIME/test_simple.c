#include <stdio.h>
#include <wchar.h>
#include <locale.h>
#include <string.h>
#include "korean.h"

int main() {
    setlocale(LC_ALL, "");
    
    const char* input = "eklr";
    wchar_t output[256];
    size_t input_len = strlen(input);
    
    compose_korean_characters(input, input_len, output);
    
    printf("Input: [%s]\n", input);
    printf("Output: [%ls]\n", output);
    printf("Expected: [다ㅣㄱ]\n");
    printf("Match: %s\n", wcscmp(output, L"다ㅣㄱ") == 0 ? "✓ YES" : "✗ NO");
    
    return 0;
} 