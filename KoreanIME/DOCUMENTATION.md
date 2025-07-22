# Korean IME Documentation

## Table of Contents
1. [Overview](#overview)
2. [Architecture](#architecture)
3. [API Reference](#api-reference)
4. [Data Structures](#data-structures)
5. [Character Mapping System](#character-mapping-system)
6. [Korean Character Composition](#korean-character-composition)
7. [Input Processing](#input-processing)
8. [Building and Installation](#building-and-installation)
9. [Usage Examples](#usage-examples)
10. [Testing](#testing)
11. [Troubleshooting](#troubleshooting)
12. [Contributing](#contributing)

## Overview

The Korean IME (Input Method Editor) is a C-based real-time Korean character composition system that converts individual jamos (consonants and vowels) into complete Korean syllables. It supports compound consonants, compound vowels, and complex character combinations with proper Unicode composition.

### Key Features
- **Real-time composition**: Characters are composed as you type
- **Compound support**: Handles all Korean compound consonants and vowels
- **Backspace handling**: Proper character decomposition when deleting
- **Multi-character input**: Support for multiple syllables
- **Individual jamo display**: Shows incomplete characters as individual jamos
- **Comprehensive testing**: Extensive test suite for validation

## Architecture

The program follows a modular architecture with clear separation of concerns:

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│   Main Loop     │───▶│  Input Handler  │───▶│  Composition    │
│   (main.c)      │    │                 │    │   Engine        │
└─────────────────┘    └─────────────────┘    └─────────────────┘
                                │                       │
                                ▼                       ▼
                       ┌─────────────────┐    ┌─────────────────┐
                       │  Key Mapping    │    │  Unicode Output │
                       │   System        │    │   Generation    │
                       └─────────────────┘    └─────────────────┘
```

### Core Components

1. **Input Processing Layer** (`process_input`, `handle_*` functions)
2. **Character Mapping Layer** (`KeyMap` structures and lookup functions)
3. **Composition Engine** (`compose_korean_characters`)
4. **Terminal Interface** (`set_raw_mode`, `print_buffers`)

## API Reference

### Core Functions

#### `void init_korean_ime(void)`
Initializes the Korean IME system.
- Sets Korean locale (`ko_KR.UTF-8`)
- Enables raw terminal mode
- Displays welcome message

#### `void cleanup_korean_ime(void)`
Cleans up the IME system.
- Restores terminal settings
- Frees allocated resources

#### `void process_input(char* input_buffer, size_t* input_len, wchar_t* output_buffer, int ch)`
Main input processing function.
- **Parameters:**
  - `input_buffer`: Character buffer for raw input
  - `input_len`: Current length of input buffer
  - `output_buffer`: Wide character buffer for composed output
  - `ch`: Input character code
- **Behavior:** Routes input to appropriate handler based on character type

#### `void compose_korean_characters(const char* input_buffer, size_t input_len, wchar_t* output_buffer)`
Core Korean character composition engine.
- **Parameters:**
  - `input_buffer`: Raw input character sequence
  - `input_len`: Length of input sequence
  - `output_buffer`: Output buffer for composed characters
- **Algorithm:** Progressive parsing with choseong → jungseong → jongseong pattern matching

### Input Handling Functions

#### `void handle_backspace(char* input_buffer, size_t* input_len, wchar_t* output_buffer)`
Handles backspace key presses.
- Removes last character from input buffer
- Re-composes remaining characters
- Updates output display

#### `void handle_enter(char* input_buffer, size_t* input_len, wchar_t* output_buffer)`
Handles Enter key presses.
- Clears all buffers
- Starts new input line

#### `void handle_space(char* input_buffer, size_t* input_len, wchar_t* output_buffer)`
Handles space character input.
- Adds space to output buffer
- Preserves input for next composition

#### `void handle_character(char* input_buffer, size_t* input_len, wchar_t* output_buffer, int ch)`
Handles regular character input.
- Validates character is mappable
- Adds to input buffer
- Triggers composition

### Utility Functions

#### `int get_index(const char *jamo, const char *list[], int size)`
Finds index of jamo in character list.
- **Returns:** Index if found, -1 if not found

#### `const char* get_jamo_buffer(const char *buffer, KeyMap *map, int size)`
Looks up key mapping in KeyMap array.
- **Returns:** Mapped jamo string or NULL if not found

#### `int is_mappable_character(char ch)`
Checks if character has valid Korean mapping.
- **Returns:** 1 if mappable, 0 if not

#### `void set_raw_mode(int enable)`
Controls terminal raw mode.
- **Parameters:**
  - `enable`: 1 to enable raw mode, 0 to disable

#### `void print_buffers(char *input_buf, wchar_t *output_buf)`
Displays current input and output buffers.
- Clears line and shows real-time status

## Data Structures

### KeyMap Structure
```c
typedef struct {
    const char *key;    // Input key sequence
    const char *jamo;   // Corresponding Korean jamo
} KeyMap;
```

### Character Lists
```c
extern const char* chosung_list[19];   // Initial consonants
extern const char* jungsung_list[21];  // Medial vowels
extern const char* jongsung_list[28];  // Final consonants
```

### Constants
```c
#define MAX_KEY_LEN 3        // Maximum key sequence length
#define MAX_OUTPUT_LEN 256   // Maximum output buffer size
```

## Character Mapping System

### Choseong (Initial Consonants) - 19 characters
```
Basic: ㄱ(r), ㄴ(s), ㄷ(e), ㄹ(f), ㅁ(a), ㅂ(q), ㅅ(t), ㅇ(d), ㅈ(w), ㅊ(c), ㅋ(z), ㅌ(x), ㅍ(v), ㅎ(g)
Double: ㄲ(R), ㄸ(E), ㅃ(Q), ㅆ(T), ㅉ(W)
```

### Jungseong (Medial Vowels) - 21 characters
```
Basic: ㅏ(k), ㅐ(o), ㅑ(i), ㅒ(O), ㅓ(j), ㅔ(p), ㅕ(u), ㅖ(P), ㅗ(h), ㅛ(y), ㅜ(n), ㅠ(b), ㅡ(m), ㅣ(l)
Compound: ㅘ(hk), ㅙ(ho), ㅚ(hl), ㅝ(nj), ㅞ(np), ㅟ(nl)
```

### Jongseong (Final Consonants) - 28 characters
```
Basic: ㄱ(r), ㄴ(s), ㄷ(e), ㄹ(f), ㅁ(a), ㅂ(q), ㅅ(t), ㅇ(d), ㅈ(w), ㅊ(c), ㅋ(z), ㅌ(x), ㅍ(v), ㅎ(g)
Double: ㄲ(R), ㅆ(T)
Compound: ㄳ(rt), ㄵ(sw), ㄶ(sg), ㄺ(fr), ㄻ(fa), ㄼ(fq), ㄽ(ft), ㄾ(fx), ㄿ(fv), ㅀ(fg), ㅄ(qt)
```

## Korean Character Composition

### Unicode Composition Formula
Korean syllables are composed using the Unicode formula:
```
Syllable = 0xAC00 + (chosung_index × 21 × 28) + (jungseong_index × 28) + jongseong_index
```

### Composition Algorithm
1. **Progressive Parsing**: Process input buffer character by character
2. **Pattern Matching**: Look for choseong → jungseong → jongseong patterns
3. **Compound Detection**: Check for 2-character compound sequences first
4. **Syllable Formation**: Combine valid patterns into complete syllables
5. **Individual Display**: Show incomplete jamos as individual characters

### Edge Cases Handled
- **Compound Jongseong Splitting**: When followed by jungseong
- **Invalid Combinations**: Graceful handling of impossible combinations
- **Incomplete Sequences**: Display of partial jamos
- **Non-Korean Characters**: Preservation of non-mappable characters

## Input Processing

### Input Flow
```
Raw Input → Character Validation → Buffer Addition → Composition → Output Display
```

### Special Key Handling
- **Backspace (0x7F)**: Character deletion with re-composition
- **Enter (\n/\r)**: Line completion and buffer clearing
- **Space**: Direct output addition
- **Ctrl+C**: Program termination

### Buffer Management
- **Input Buffer**: Stores raw key sequences
- **Output Buffer**: Stores composed wide characters
- **Dynamic Updates**: Real-time buffer modification

## Building and Installation

### Prerequisites
```bash
# Required packages
sudo apt-get install build-essential
sudo apt-get install locales
sudo locale-gen ko_KR.UTF-8
```

### Build Commands
```bash
# Standard build
make

# Debug build
make debug

# Release build
make release

# Clean build artifacts
make clean
```

### Installation
```bash
# Install to system (requires sudo)
make install

# Uninstall
make uninstall
```

### Available Make Targets
- `all`: Build the Korean IME (default)
- `clean`: Remove build artifacts
- `install`: Install to /usr/local/bin
- `uninstall`: Remove from /usr/local/bin
- `run`: Build and run the program
- `test`: Build and run comprehensive tests
- `test-simple`: Build and run simple tests
- `test-all`: Run all tests
- `debug`: Build with debug symbols
- `release`: Build optimized release version
- `help`: Show help message

## Usage Examples

### Basic Character Composition
```bash
# Start the IME
./korean

# Type: ek
# Output: 다 (ㄷ + ㅏ)

# Type: qk
# Output: 바 (ㅂ + ㅏ)

# Type: ekr
# Output: 닥 (ㄷ + ㅏ + ㄱ)
```

### Compound Character Examples
```bash
# Compound choseong
# Type: Rk
# Output: 까 (ㄲ + ㅏ)

# Compound jungseong
# Type: dhk
# Output: 와 (ㅇ + ㅘ)

# Compound jongseong
# Type: ekfr
# Output: 닭 (ㄷ + ㅏ + ㄺ)
```

### Multi-Syllable Input
```bash
# Type: ekek
# Output: 다다 (ㄷ + ㅏ + ㄷ + ㅏ)

# Type: ekfrk
# Output: 달가 (ㄷ + ㅏ + ㄹ + ㄱ + ㅏ)
```

### Special Operations
```bash
# Backspace: Delete last character
# Space: Add space to output
# Enter: Complete line and start new input
# Ctrl+C: Exit program
```

## Testing

### Running Tests
```bash
# Comprehensive tests
make test

# Simple tests
make test-simple

# All tests
make test-all
```

### Test Coverage
- **Basic Composition**: All single jamo combinations
- **Compound Characters**: All compound choseong, jungseong, jongseong
- **Edge Cases**: Invalid combinations, incomplete sequences
- **Backspace**: Character deletion and re-composition
- **Multi-character**: Multiple syllable sequences

### Test Statistics
- **Total Tests**: 289
- **Passing Tests**: 289 (100%)
- **Failing Tests**: 0 (0%)

## Troubleshooting

### Common Issues

#### Locale Problems
```bash
# Error: Cannot set locale
sudo locale-gen ko_KR.UTF-8
export LC_ALL=ko_KR.UTF-8
```

#### Compilation Errors
```bash
# Missing dependencies
sudo apt-get install build-essential

# Unicode support issues
sudo apt-get install locales
```

#### Runtime Issues
```bash
# Terminal not responding
# Press Ctrl+C to exit

# Characters not displaying correctly
# Ensure terminal supports UTF-8
```

### Debug Mode
```bash
# Build with debug symbols
make debug

# Run with debug output
./korean
```

### Performance Optimization
```bash
# Build optimized version
make release

# Profile performance
gprof ./korean
```

## Contributing

### Development Setup
1. Fork the repository
2. Create feature branch
3. Make changes with proper testing
4. Submit pull request

### Code Style
- Follow C99 standard
- Use meaningful variable names
- Add comments for complex logic
- Maintain consistent indentation

### Testing Guidelines
- Add tests for new features
- Ensure all tests pass
- Test edge cases and error conditions
- Update documentation for API changes

### File Structure
```
KoreanIME/
├── main.c          # Main program entry point
├── korean.c        # Core IME implementation
├── korean.h        # Header file with declarations
├── Makefile        # Build configuration
├── README.md       # Project overview
├── DOCUMENTATION.md # This documentation file
├── test_korean.c   # Comprehensive test suite
└── test_simple.c   # Simple test cases
```

---

**Version**: 1.0  
**Last Updated**: 2024  
**License**: MIT License  
**Author**: Korean IME Development Team 