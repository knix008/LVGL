# ChunJiIn Input System - API Documentation

## Overview

The ChunJiIn Input System provides a complete Korean input method implementation based on the ChunJiIn (천지인) principle. This document describes the API, data structures, and usage patterns for integrating the system into applications.

**Version**: 2.0.0  
**Test Status**: 558 test cases, 100% pass rate  
**License**: MIT

## Table of Contents

1. [Core Data Structures](#core-data-structures)
2. [Core Functions](#core-functions)
3. [Syllable Processing Functions](#syllable-processing-functions)
4. [Utility Functions](#utility-functions)
5. [Global Variables](#global-variables)
6. [Usage Examples](#usage-examples)
7. [Error Handling](#error-handling)
8. [Performance Considerations](#performance-considerations)

## Core Data Structures

### SyllableState Enum

```c
typedef enum {
    STATE_START,          // Initial state - no input yet
    STATE_CHOSEONG,       // Initial consonant entered
    STATE_JUNGSEONG,      // Vowel entered
    STATE_JONGSEONG       // Final consonant entered
} SyllableState;
```

**Description**: Tracks the current state of syllable formation.

### CurrentSyllable Structure

```c
typedef struct {
    int cho;              // Choseong (initial consonant) index
    int jung;             // Jungseong (vowel) index  
    int jong;             // Jongseong (final consonant) index
    SyllableState state;  // Current combination state
    int temp_vowel;       // Temporary vowel for combination (100=ㆍ, 200=ㆍㆍ)
    int temp_consonant;   // Temporary consonant for cycling
} CurrentSyllable;
```

**Description**: Contains the current syllable components and state information for real-time syllable formation.

**Fields**:
- `cho`: Choseong index (0-18, corresponding to Korean consonants)
- `jung`: Jungseong index (0-20, corresponding to Korean vowels)
- `jong`: Jongseong index (-1 to 27, -1 means no final consonant)
- `state`: Current syllable formation state
- `temp_vowel`: Temporary vowel state (0=none, 100=ㆍ, 200=ㆍㆍ)
- `temp_consonant`: Temporary consonant for cycling logic

## Core Functions

### initialize()

```c
void initialize(void);
```

**Description**: Initializes the ChunJiIn input system.

**Parameters**: None

**Returns**: void

**Usage**:
```c
#include "chunjiin_input.h"

int main() {
    initialize();
    // System is now ready for input
    return 0;
}
```

### process_input()

```c
void process_input(char key);
```

**Description**: Main input processing function that handles all key inputs and manages syllable formation.

**Parameters**:
- `key`: The input key character

**Returns**: void

**Key Mapping**:
- `'g'`: ㄱ (choseong) / ㄱ→ㅋ→ㄲ (jongseong cycling)
- `'n'`: ㄴ (choseong) / ㄴ→ㄹ (jongseong cycling)
- `'d'`: ㄷ (choseong) / ㄷ→ㅌ (jongseong cycling)
- `'b'`: ㅂ (choseong) / ㅂ→ㅍ (jongseong cycling)
- `'s'`: ㅅ (choseong) / ㅅ→ㅎ→ㅆ (jongseong cycling)
- `'j'`: ㅈ (choseong) / ㅈ→ㅊ (jongseong cycling)
- `'m'`: ㅇ (choseong) / ㅇ→ㅁ (jongseong cycling)
- `'a'`: ㆍ (dot) - cycles between single and double dots
- `'e'`: ㅡ (horizontal line)
- `'i'`: ㅣ (vertical line)
- `'\r'` or `'\n'`: Enter key - finalize current syllable
- `'\b'`: Backspace - delete last input
- `' '`: Space - clear all input

**Usage**:
```c
// Process user input
process_input('g');  // ㄱ
process_input('i');  // ㅣ
process_input('a');  // ㆍ
// Results in: 가 (ㄱ + ㅏ)
```

### chunjiin_get_current_text()

```c
void chunjiin_get_current_text(wchar_t * buffer);
```

**Description**: Retrieves the current text being displayed, including any incomplete syllables being composed.

**Parameters**:
- `buffer`: Output buffer to store the current text (must be large enough for 1024 wide characters)

**Returns**: void

**Usage**:
```c
wchar_t buffer[1024];
chunjiin_get_current_text(buffer);
// buffer now contains the current display text
```

### chunjiin_enter_key_handler()

```c
void chunjiin_enter_key_handler(void);
```

**Description**: Finalizes the current syllable and adds it to the output buffer.

**Parameters**: None

**Returns**: void

**Usage**:
```c
// After processing input, finalize the syllable
chunjiin_enter_key_handler();
```

## Syllable Processing Functions

### handle_consonant()

```c
void handle_consonant(int key_code);
```

**Description**: Processes consonant key inputs, including cycling through related consonants and compound consonant formation.

**Parameters**:
- `key_code`: The consonant key code (1=g, 2=n, 3=d, 4=b, 5=s, 6=j, 7=m)

**Returns**: void

**Usage**:
```c
handle_consonant(1);  // Process 'g' key (ㄱ)
```

### handle_vowel()

```c
void handle_vowel(int key_code);
```

**Description**: Processes vowel key inputs, including complex vowel combinations following ChunJiIn rules and dot cycling.

**Parameters**:
- `key_code`: The vowel key code (1=ㆍ, 2=ㅡ, 3=ㅣ)

**Returns**: void

**Usage**:
```c
handle_vowel(1);  // Process dot (ㆍ)
handle_vowel(2);  // Process ㅡ
handle_vowel(3);  // Process ㅣ
```

### handle_special()

```c
void handle_special(char key);
```

**Description**: Processes special keys like Enter, Backspace, Space, etc.

**Parameters**:
- `key`: The special key character

**Returns**: void

**Usage**:
```c
handle_special('\r');  // Enter key
handle_special('\b');  // Backspace
handle_special(' ');   // Space
```

### finalize_syllable()

```c
void finalize_syllable(void);
```

**Description**: Completes the current syllable formation and adds it to the output buffer. Handles dot display and state transitions.

**Parameters**: None

**Returns**: void

**Usage**:
```c
// Complete the current syllable
finalize_syllable();
```

### reset_current_syllable()

```c
void reset_current_syllable(void);
```

**Description**: Clears the current syllable state and prepares for a new syllable input.

**Parameters**: None

**Returns**: void

**Usage**:
```c
// Reset for new input
reset_current_syllable();
```

## Utility Functions

### get_composing_char()

```c
wchar_t get_composing_char(void);
```

**Description**: Returns the character currently being composed for display. Handles special cases like dot display and incomplete syllables.

**Parameters**: None

**Returns**: Wide character representing the current composition

**Usage**:
```c
wchar_t composing = get_composing_char();
// Use composing character for display
```

### combine_syllable()

```c
wchar_t combine_syllable(int cho, int jung, int jong);
```

**Description**: Combines choseong, jungseong, and jongseong indices into a complete Korean syllable Unicode character.

**Parameters**:
- `cho`: Choseong index (0-18)
- `jung`: Jungseong index (0-20)
- `jong`: Jongseong index (-1 to 27, -1 means no final consonant)

**Returns**: Complete Korean syllable as wide character

**Usage**:
```c
wchar_t syllable = combine_syllable(0, 0, 1);  // 가 + ㄱ = 각
```

### wchar_to_utf8()

```c
int wchar_to_utf8(wchar_t wc, char *utf8_buffer, size_t buffer_size);
```

**Description**: Converts a wide character to UTF-8 encoding with bounds checking.

**Parameters**:
- `wc`: Wide character to convert
- `utf8_buffer`: Output buffer for UTF-8 bytes
- `buffer_size`: Size of the output buffer

**Returns**: Number of bytes written to the buffer

**Usage**:
```c
wchar_t korean_char = L'가';
char utf8_buffer[8];
int bytes = wchar_to_utf8(korean_char, utf8_buffer, sizeof(utf8_buffer));
// utf8_buffer now contains UTF-8 encoded Korean character
```

## Global Variables

### g_output_buffer

```c
extern wchar_t g_output_buffer[1024];
```

**Description**: Global output buffer that stores the final Korean text output in wide character format.

**Size**: 1024 wide characters (sufficient for most Korean text input)

**Usage**:
```c
// Access the output buffer directly
wchar_t* output = g_output_buffer;
// Process the output text
```

### g_current_syllable

```c
extern CurrentSyllable g_current_syllable;
```

**Description**: Global variable containing the current syllable state and components.

**Usage**:
```c
// Access current syllable state
if (g_current_syllable.state == STATE_JONGSEONG) {
    // Handle jongseong state
}
```

## Usage Examples

### Basic Syllable Formation

```c
#include "chunjiin_input.h"

void form_basic_syllable() {
    initialize();
    
    // Form 가 (ㄱ + ㅏ)
    process_input('g');  // ㄱ
    process_input('i');  // ㅣ
    process_input('a');  // ㆍ (forms ㅏ)
    
    // Get the result
    wchar_t buffer[1024];
    chunjiin_get_current_text(buffer);
    // buffer contains "가"
}
```

### Complex Vowel Combinations

```c
void form_complex_vowel() {
    initialize();
    
    // Form 교 (ㄱ + ㅛ)
    process_input('g');  // ㄱ
    process_input('a');  // ㆍ
    process_input('a');  // ㆍㆍ
    process_input('e');  // ㅡ (forms ㅛ)
    
    // Get the result
    wchar_t buffer[1024];
    chunjiin_get_current_text(buffer);
    // buffer contains "교"
}
```

### Jongseong + Dot Combinations

```c
void form_jongseong_dot() {
    initialize();
    
    // Form 각 + dot + dot + vowel
    process_input('g');  // ㄱ
    process_input('i');  // ㅣ
    process_input('a');  // ㆍ
    process_input('g');  // ㄱ (forms 각)
    process_input('a');  // ㆍ (각 + dot)
    process_input('a');  // ㆍ (각 + dot + dot)
    process_input('i');  // ㅣ (decomposes to 가겨)
    
    // Get the result
    wchar_t buffer[1024];
    chunjiin_get_current_text(buffer);
    // buffer contains "가겨"
}
```

### Consonant Cycling

```c
void demonstrate_cycling() {
    initialize();
    
    // Form 가
    process_input('g');  // ㄱ
    process_input('i');  // ㅣ
    process_input('a');  // ㆍ
    
    // Cycle through jongseong consonants
    process_input('g');  // ㄱ (각)
    process_input('g');  // ㅋ (갚)
    process_input('g');  // ㄲ (갘)
    process_input('g');  // ㄱ (같)
    
    // Get the result
    wchar_t buffer[1024];
    chunjiin_get_current_text(buffer);
    // buffer contains "같"
}
```

### Complete Word Formation

```c
void form_complete_word() {
    initialize();
    
    // Form "안녕하세요"
    // 안
    process_input('m');  // ㅇ
    process_input('i');  // ㅣ
    process_input('a');  // ㆍ
    process_input('n');  // ㄴ
    chunjiin_enter_key_handler();
    
    // 녕
    process_input('n');  // ㄴ
    process_input('a');  // ㆍ
    process_input('a');  // ㆍㆍ
    process_input('e');  // ㅡ
    chunjiin_enter_key_handler();
    
    // 하
    process_input('s');  // ㅅ
    process_input('i');  // ㅣ
    process_input('a');  // ㆍ
    chunjiin_enter_key_handler();
    
    // 세
    process_input('s');  // ㅅ
    process_input('a');  // ㆍ
    process_input('i');  // ㅣ
    chunjiin_enter_key_handler();
    
    // 요
    process_input('m');  // ㅇ
    process_input('a');  // ㆍ
    process_input('a');  // ㆍㆍ
    process_input('e');  // ㅡ
    chunjiin_enter_key_handler();
    
    // Get the complete word
    wchar_t buffer[1024];
    chunjiin_get_current_text(buffer);
    // buffer contains "안녕하세요"
}
```

## Error Handling

### Buffer Overflow Protection

The system includes built-in buffer overflow protection:

```c
// Safe string operations
int wchar_to_utf8(wchar_t wc, char *utf8_buffer, size_t buffer_size) {
    if (buffer_size < 1) {
        return 0;  // Buffer too small
    }
    // Safe conversion with bounds checking
}
```

### Invalid Input Handling

```c
void handle_invalid_input() {
    initialize();
    
    // Invalid input is gracefully handled
    process_input('x');  // Invalid key - ignored
    process_input('g');  // Valid key - processed normally
}
```

### State Recovery

```c
void recover_from_error() {
    // Reset the system if needed
    reset_current_syllable();
    
    // Or reinitialize completely
    initialize();
}
```

## Performance Considerations

### Memory Usage

- **Static allocation**: All buffers are statically allocated
- **No dynamic memory**: No malloc/free operations
- **Predictable memory usage**: Fixed memory footprint

### Processing Speed

- **O(1) operations**: Most operations are constant time
- **No complex algorithms**: Simple state machine logic
- **Efficient Unicode handling**: Optimized character conversion

### Buffer Sizes

- **Output buffer**: 1024 wide characters (2KB)
- **UTF-8 buffer**: 8 bytes per function call
- **Temporary buffers**: Minimal stack usage

### Thread Safety

**Note**: The current implementation is not thread-safe. For multi-threaded applications:

1. Use mutex locks around API calls
2. Consider creating thread-local instances
3. Implement proper synchronization

```c
// Example with mutex (pseudo-code)
pthread_mutex_t chunjiin_mutex = PTHREAD_MUTEX_INITIALIZER;

void thread_safe_input(char key) {
    pthread_mutex_lock(&chunjiin_mutex);
    process_input(key);
    pthread_mutex_unlock(&chunjiin_mutex);
}
```

## Integration Guidelines

### Initialization

```c
// Always initialize before use
initialize();

// Check for successful initialization
if (g_current_syllable.state != STATE_START) {
    // Handle initialization error
}
```

### Input Processing

```c
// Process input in a loop
char input;
while ((input = get_input()) != EOF) {
    process_input(input);
    
    // Update display
    wchar_t buffer[1024];
    chunjiin_get_current_text(buffer);
    update_display(buffer);
}
```

### Cleanup

```c
// No explicit cleanup needed - static allocation
// Just reset if needed
reset_current_syllable();
```

## Testing

The system includes comprehensive testing capabilities:

```c
// Run the test suite
gcc -o test_chunjiin_input src/test_chunjiin_input.c src/chunjiin_input.c -Iinclude
./test_chunjiin_input
```

**Test Coverage**: 558 test cases covering all functionality

**Test Categories**:
- Basic syllable formation
- Vowel combinations
- Consonant cycling
- Jongseong combinations
- Dot display and cycling
- Edge cases and error handling

---

**For more information, see the main README.md and HowTo.md files.** 