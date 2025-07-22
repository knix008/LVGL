# Korean IME API Reference

## Table of Contents
1. [Overview](#overview)
2. [Constants](#constants)
3. [Data Structures](#data-structures)
4. [Core Functions](#core-functions)
5. [Input Handling Functions](#input-handling-functions)
6. [Character Mapping Functions](#character-mapping-functions)
7. [Composition Functions](#composition-functions)
8. [Utility Functions](#utility-functions)
9. [Terminal Interface Functions](#terminal-interface-functions)
10. [External Variables](#external-variables)
11. [Error Handling](#error-handling)
12. [Usage Examples](#usage-examples)

## Overview

The Korean IME API provides a complete interface for real-time Korean character composition. The API is designed to be modular, with clear separation between input handling, character mapping, composition logic, and terminal interface.

## Constants

### Buffer and Key Limits
```c
#define MAX_KEY_LEN 3        // Maximum length of key sequences
#define MAX_OUTPUT_LEN 256   // Maximum length of output buffer
```

### Unicode Composition
```c
#define HANGUL_BASE 0xAC00   // Unicode base for Korean syllables
#define CHOSUNG_COUNT 19     // Number of initial consonants
#define JUNGSEONG_COUNT 21   // Number of medial vowels
#define JONGSEONG_COUNT 28   // Number of final consonants
```

## Data Structures

### KeyMap Structure
```c
typedef struct {
    const char *key;    // Input key sequence (e.g., "ek", "fr")
    const char *jamo;   // Corresponding Korean jamo (e.g., "다", "ㄺ")
} KeyMap;
```

**Fields:**
- `key`: String representing the input key combination
- `jamo`: String representing the corresponding Korean character

**Usage:**
```c
KeyMap mapping = {"ek", "다"};
printf("Key: %s -> Jamo: %s\n", mapping.key, mapping.jamo);
```

## Core Functions

### Initialization and Cleanup

#### `void init_korean_ime(void)`
Initializes the Korean IME system.

**Description:** Sets up the Korean locale, enables raw terminal mode, and displays the welcome message.

**Parameters:** None

**Returns:** void

**Side Effects:**
- Sets locale to `ko_KR.UTF-8`
- Enables raw terminal mode
- Displays welcome message

**Example:**
```c
int main() {
    init_korean_ime();
    // ... main program logic
    cleanup_korean_ime();
    return 0;
}
```

#### `void cleanup_korean_ime(void)`
Cleans up the Korean IME system.

**Description:** Restores terminal settings and frees allocated resources.

**Parameters:** None

**Returns:** void

**Side Effects:**
- Restores terminal to normal mode
- Cleans up any allocated resources

### Main Processing

#### `void process_input(char* input_buffer, size_t* input_len, wchar_t* output_buffer, int ch)`
Main input processing function that routes input to appropriate handlers.

**Description:** Analyzes the input character and delegates to the appropriate handler function.

**Parameters:**
- `input_buffer`: Character buffer for raw input (char array)
- `input_len`: Pointer to current length of input buffer (size_t*)
- `output_buffer`: Wide character buffer for composed output (wchar_t array)
- `ch`: Input character code (int)

**Returns:** void

**Behavior:**
- Routes backspace (0x7F) to `handle_backspace()`
- Routes Enter (\n/\r) to `handle_enter()`
- Routes space to `handle_space()`
- Routes other characters to `handle_character()`
- Calls `print_buffers()` after processing

**Example:**
```c
char input_buf[MAX_OUTPUT_LEN] = "";
wchar_t output_buf[MAX_OUTPUT_LEN] = L"";
size_t input_len = 0;

int ch = getchar();
process_input(input_buf, &input_len, output_buf, ch);
```

## Input Handling Functions

### `void handle_backspace(char* input_buffer, size_t* input_len, wchar_t* output_buffer)`
Handles backspace key presses with proper character decomposition.

**Description:** Removes the last character from the input buffer and re-composes the remaining characters.

**Parameters:**
- `input_buffer`: Input character buffer (char*)
- `input_len`: Pointer to current input length (size_t*)
- `output_buffer`: Output wide character buffer (wchar_t*)

**Returns:** void

**Behavior:**
- Decrements input length if buffer is not empty
- Null-terminates the input buffer
- Clears output buffer
- Calls `compose_korean_characters()` to re-compose

**Example:**
```c
char input[] = "ekfr";
size_t len = 4;
wchar_t output[MAX_OUTPUT_LEN];
handle_backspace(input, &len, output);
// input becomes "ekf", len becomes 3
```

### `void handle_enter(char* input_buffer, size_t* input_len, wchar_t* output_buffer)`
Handles Enter key presses.

**Description:** Completes the current line and starts a new input line.

**Parameters:**
- `input_buffer`: Input character buffer (char*)
- `input_len`: Pointer to current input length (size_t*)
- `output_buffer`: Output wide character buffer (wchar_t*)

**Returns:** void

**Behavior:**
- Prints newline character
- Clears all buffers
- Resets input length to 0
- Displays empty buffer status

**Example:**
```c
char input[] = "ekfr";
size_t len = 4;
wchar_t output[MAX_OUTPUT_LEN];
handle_enter(input, &len, output);
// All buffers cleared, new line started
```

### `void handle_space(char* input_buffer, size_t* input_len, wchar_t* output_buffer)`
Handles space character input.

**Description:** Adds a space character to the output buffer.

**Parameters:**
- `input_buffer`: Input character buffer (char*)
- `input_len`: Pointer to current input length (size_t*)
- `output_buffer`: Output wide character buffer (wchar_t*)

**Returns:** void

**Behavior:**
- Adds space to input buffer if space available
- Adds space to output buffer
- Null-terminates both buffers

**Example:**
```c
char input[] = "ek";
size_t len = 2;
wchar_t output[MAX_OUTPUT_LEN];
handle_space(input, &len, output);
// input becomes "ek ", output contains space
```

### `void handle_character(char* input_buffer, size_t* input_len, wchar_t* output_buffer, int ch)`
Handles regular character input.

**Description:** Validates and processes regular character input.

**Parameters:**
- `input_buffer`: Input character buffer (char*)
- `input_len`: Pointer to current input length (size_t*)
- `output_buffer`: Output wide character buffer (wchar_t*)
- `ch`: Input character code (int)

**Returns:** void

**Behavior:**
- Validates character using `is_mappable_character()`
- Adds character to input buffer if valid
- Calls `compose_korean_characters()` to update output

**Example:**
```c
char input[] = "ek";
size_t len = 2;
wchar_t output[MAX_OUTPUT_LEN];
handle_character(input, &len, output, 'r');
// input becomes "ekr", output updated with composition
```

## Character Mapping Functions

### `const char* get_jamo_buffer(const char *buffer, KeyMap *map, int size)`
Looks up a key mapping in a KeyMap array.

**Description:** Searches for a key sequence in the provided KeyMap array and returns the corresponding jamo.

**Parameters:**
- `buffer`: Key sequence to search for (const char*)
- `map`: Array of KeyMap structures (KeyMap*)
- `size`: Number of elements in the map array (int)

**Returns:** 
- `const char*`: Corresponding jamo string if found
- `NULL`: If key sequence not found

**Example:**
```c
const char* jamo = get_jamo_buffer("ek", cho_keymap, sizeof(cho_keymap)/sizeof(KeyMap));
if (jamo) {
    printf("Found jamo: %s\n", jamo);
}
```

### `int get_index(const char *jamo, const char *list[], int size)`
Finds the index of a jamo in a character list.

**Description:** Searches for a jamo string in the provided list and returns its index.

**Parameters:**
- `jamo`: Jamo string to search for (const char*)
- `list`: Array of jamo strings (const char*[])
- `size`: Number of elements in the list (int)

**Returns:**
- `int`: Index of jamo if found (0-based)
- `-1`: If jamo not found

**Example:**
```c
int idx = get_index("ㄷ", chosung_list, 19);
if (idx >= 0) {
    printf("Choseong index: %d\n", idx);
}
```

### `int is_mappable_character(char ch)`
Checks if a character has a valid Korean mapping.

**Description:** Determines whether a character can be mapped to Korean jamos.

**Parameters:**
- `ch`: Character to check (char)

**Returns:**
- `1`: Character is mappable
- `0`: Character is not mappable

**Behavior:**
- Always returns 1 for space character
- Checks against choseong, jungseong, and jongseong mappings

**Example:**
```c
if (is_mappable_character('e')) {
    printf("Character 'e' is mappable\n");
}
```

## Composition Functions

### `void compose_korean_characters(const char* input_buffer, size_t input_len, wchar_t* output_buffer)`
Core Korean character composition engine.

**Description:** Converts input key sequences into composed Korean syllables using Unicode composition.

**Parameters:**
- `input_buffer`: Raw input character sequence (const char*)
- `input_len`: Length of input sequence (size_t)
- `output_buffer`: Output buffer for composed characters (wchar_t*)

**Returns:** void

**Algorithm:**
1. **Progressive Parsing**: Process input buffer character by character
2. **Pattern Matching**: Look for choseong → jungseong → jongseong patterns
3. **Compound Detection**: Check for 2-character compound sequences first
4. **Syllable Formation**: Combine valid patterns into complete syllables
5. **Individual Display**: Show incomplete jamos as individual characters

**Unicode Formula:**
```c
Syllable = 0xAC00 + (chosung_index × 21 × 28) + (jungseong_index × 28) + jongseong_index
```

**Edge Cases Handled:**
- Compound jongseong splitting when followed by jungseong
- Invalid combinations
- Incomplete sequences
- Non-Korean characters

**Example:**
```c
char input[] = "ekfr";
wchar_t output[MAX_OUTPUT_LEN];
compose_korean_characters(input, 4, output);
// output contains "닭" (ㄷ + ㅏ + ㄺ)
```

## Utility Functions

### `void print_buffers(char *input_buf, wchar_t *output_buf)`
Displays current input and output buffers.

**Description:** Clears the current line and displays the real-time status of input and output buffers.

**Parameters:**
- `input_buf`: Input buffer to display (char*)
- `output_buf`: Output buffer to display (wchar_t*)

**Returns:** void

**Behavior:**
- Clears current line using `\r\033[K`
- Shows input buffer in square brackets
- Shows output buffer in square brackets
- Flushes stdout

**Example:**
```c
char input[] = "ekfr";
wchar_t output[] = L"닭";
print_buffers(input, output);
// Displays: Input: [ekfr] | Output: [닭]
```

## Terminal Interface Functions

### `void set_raw_mode(int enable)`
Controls terminal raw mode.

**Description:** Enables or disables raw terminal mode for character-by-character input processing.

**Parameters:**
- `enable`: 1 to enable raw mode, 0 to disable (int)

**Returns:** void

**Behavior:**
- **Enable (1)**: Disables ICANON and ECHO flags
- **Disable (0)**: Restores original terminal settings

**Side Effects:**
- Modifies terminal attributes using `tcsetattr()`
- Stores original settings for restoration

**Example:**
```c
set_raw_mode(1);  // Enable raw mode
// ... process input character by character
set_raw_mode(0);  // Restore normal mode
```

## External Variables

### Key Mapping Arrays
```c
extern KeyMap cho_keymap[];    // Choseong (initial consonants) mappings
extern KeyMap jung_keymap[];   // Jungseong (medial vowels) mappings
extern KeyMap jong_keymap[];   // Jongseong (final consonants) mappings
```

### Character Lists
```c
extern const char* chosung_list[19];   // Array of choseong characters
extern const char* jungsung_list[21];  // Array of jungseong characters
extern const char* jongsung_list[28];  // Array of jongseong characters
```

## Error Handling

### Return Values
- **Index functions**: Return -1 for "not found"
- **Pointer functions**: Return NULL for "not found"
- **Boolean functions**: Return 1 for true, 0 for false
- **Void functions**: No return value, errors handled through side effects

### Common Error Conditions
1. **Invalid character input**: Ignored silently
2. **Buffer overflow**: Prevented by MAX_OUTPUT_LEN limit
3. **Invalid Unicode composition**: Graceful fallback to individual jamos
4. **Terminal setup failure**: May cause input processing issues

### Error Prevention
- Buffer bounds checking in all functions
- Character validation before processing
- Graceful handling of invalid combinations
- Proper cleanup in error conditions

## Usage Examples

### Basic Usage Pattern
```c
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
```

### Custom Input Processing
```c
void custom_input_handler(const char* input) {
    wchar_t output[MAX_OUTPUT_LEN];
    size_t len = strlen(input);
    
    compose_korean_characters(input, len, output);
    printf("Input: %s -> Output: %ls\n", input, output);
}

// Usage
custom_input_handler("ekfr");  // Output: 닭
custom_input_handler("dhk");   // Output: 와
```

### Character Mapping Lookup
```c
void print_mapping(const char* key) {
    const char* cho_jamo = get_jamo_buffer(key, cho_keymap, sizeof(cho_keymap)/sizeof(KeyMap));
    const char* jung_jamo = get_jamo_buffer(key, jung_keymap, sizeof(jung_keymap)/sizeof(KeyMap));
    const char* jong_jamo = get_jamo_buffer(key, jong_keymap, sizeof(jong_keymap)/sizeof(KeyMap));
    
    if (cho_jamo) printf("Choseong: %s -> %s\n", key, cho_jamo);
    if (jung_jamo) printf("Jungseong: %s -> %s\n", key, jung_jamo);
    if (jong_jamo) printf("Jongseong: %s -> %s\n", key, jong_jamo);
}
```

### Buffer Management
```c
void reset_buffers(char* input_buf, size_t* input_len, wchar_t* output_buf) {
    input_buf[0] = '\0';
    output_buf[0] = L'\0';
    *input_len = 0;
    print_buffers(input_buf, output_buf);
}
```

---

**Version**: 1.0  
**Last Updated**: 2024  
**Compatibility**: C99 standard  
**Dependencies**: Standard C library, termios.h, wchar.h, locale.h 