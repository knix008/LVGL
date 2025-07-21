# Korean IME (Input Method Editor)

A C-based Korean Input Method Editor that provides real-time Hangul character composition with support for initial, medial, and final consonants.

## Features

- Real-time Korean character composition
- Support for initial, medial, and final consonants
- Backspace, space, and Enter key support
- Multi-character input
- Shift key support for double consonants
- QWERTY keyboard mapping
- Wide character support for proper Hangul display

## Project Structure

```
KoreanIME/
├── korean.h          # Header file with declarations and function prototypes
├── korean.c          # Main implementation with interactive mode
├── korean_lib.c      # Library implementation (without main function)
├── example.c         # Example usage of the header file
├── Makefile          # Build configuration
└── README.md         # This file
```

## Building

### Main Korean IME
```bash
make
./korean_ime
```

### Example Program
```bash
make example
./example
```

### Clean Build
```bash
make clean
make
```

## Using the Header File

The `korean.h` header file provides the following:

### Constants
- `MAX_KEY_LEN`: Maximum key length (3)
- `MAX_OUTPUT_LEN`: Maximum output buffer length (256)

### Data Structures
```c
typedef struct {
    const char *key;
    const char *jamo;
} KeyMap;
```

### Key Mappings
- `cho_keymap[]`: Initial consonant mappings
- `jung_keymap[]`: Medial vowel mappings  
- `jong_keymap[]`: Final consonant mappings

### Korean Character Lists
- `chosung_list[]`: List of initial consonants
- `jungsung_list[]`: List of medial vowels
- `jongsung_list[]`: List of final consonants

### Function Prototypes
- `int get_index(const char *jamo, const char *list[], int size)`: Get index of jamo in list
- `const char* get_jamo_buffer(const char *buffer, KeyMap *map, int size)`: Get jamo for key
- `void set_raw_mode(int enable)`: Enable/disable raw terminal mode
- `void print_buffers(char *input_buf, wchar_t *output_buf)`: Print input and output buffers

## Example Usage

```c
#include "korean.h"
#include <stdio.h>

int main() {
    // Get jamo for a key
    const char* jamo = get_jamo_buffer("r", cho_keymap, 19);
    if (jamo) {
        printf("Jamo for 'r': %s\n", jamo);
    }
    
    // Get index in character list
    int idx = get_index("ㄱ", chosung_list, 19);
    printf("Index of 'ㄱ': %d\n", idx);
    
    return 0;
}
```

## Keyboard Mapping

The IME uses QWERTY keyboard mapping with the following conventions:

- Lowercase letters for single consonants
- Uppercase letters for double consonants
- Vowels mapped to specific keys
- Shift key support for differentiating single/double consonants

## License

This project is only for Intellivix. 