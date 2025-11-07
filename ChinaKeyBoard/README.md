# Korean QWERTY Keypad Project

A Korean input method editor (IME) implementation with GUI keyboard for Linux using MiniGUI.

## Features

- **Korean QWERTY Input**: Supports Dubeolsik (두벌식) Korean keyboard layout
- **Complete Hangul Support**:
  - Single and double consonants (chosung)
  - Single and compound vowels (jungsung)
  - Single and compound final consonants (jongsung)
- **Real-time Character Composition**: Live preview while typing
- **GUI Keyboard**: Visual on-screen keyboard with Korean character display
- **Physical Keyboard Support**: Works with both on-screen and physical keyboards
- **Left Arrow Key**: Customized to act as backspace for efficient editing

## Project Structure

```
ChinaKeyBoard/
├── ime/                      # Input Method Engine
│   ├── libime/              # Core IME library
│   │   ├── ime_korean.c    # Korean character composition engine
│   │   └── ime_korean.h    # Korean IME header
│   ├── common.c             # Shared IME functionality
│   ├── properties.c         # Configuration handling
│   └── softkeyboard/       # GUI keyboard implementations
│       └── ko_kbd.c        # Korean keyboard layout
├── assets/                   # Fonts
│   └── NanumGothic-*.ttf   # Korean fonts
├── qwerty_korean_keypad.c  # Main application
├── Makefile                 # Build configuration
├── run.sh                   # Application launcher
├── run_test.sh              # Test suite runner
└── test_korean_input.c      # Comprehensive IME tests
```

## Building

### Prerequisites

- GCC compiler
- MiniGUI development libraries (system-wide or local installation)
- FreeType, PNG, JPEG libraries

### Build Commands

```bash
# Build the main application
./build.sh

# Build Korean IME tests
make test_simple      # Build simple test
make test_korean      # Build comprehensive test

# Clean build artifacts
make clean
```

## Running

### Main Application

```bash
./run.sh
```

Or directly:
```bash
./qwerty_korean_keypad
```

### Using the Keyboard

- **Korean Characters**: Click on-screen keys or type QWERTY keys
- **Shift Key**: Toggles between normal and tensed consonants (ㄱ/ㄲ, ㅂ/ㅃ, etc.)
- **Left Arrow**: Acts as backspace
- **Clear Button**: Clears the input buffer

## Keyboard Layout

The application uses Dubeolsik (두벌식) layout mapping:

```
QWERTY      Korean
q → ㅂ      w → ㅈ      e → ㄷ      r → ㄱ      t → ㅅ
y → ㅛ      u → ㅕ      i → ㅑ      o → ㅐ      p → ㅔ

a → ㅁ      s → ㄴ      d → ㅇ      f → ㄹ      g → ㅎ
h → ㅗ      j → ㅓ      k → ㅏ      l → ㅣ

z → ㅋ      x → ㅌ      c → ㅊ      v → ㅍ
b → ㅠ      n → ㅜ      m → ㅡ
```

## Testing

The project includes comprehensive test suites for the Korean IME with automated test execution and reporting.

### Test Setup

The project includes a `Makefile` that:
- Automatically detects system-wide or local MiniGUI installation via `pkg-config`
- Compiles test programs with proper include paths
- Links against MiniGUI libraries for complete IME functionality
- Supports both simple and comprehensive test targets

### Running Tests

```bash
# Run all tests with automated build and reporting
./run_test.sh

# Build and run simple test only (quick verification)
make test_simple
./test_korean_simple

# Build and run comprehensive test (all test cases)
make test_korean
./test_korean_input

# Clean build artifacts
make clean
```

### Test Coverage

The comprehensive test suite includes 67 test cases covering:

**Basic Composition Tests**
- **Single chosung + jungsung**: Basic syllable composition (15 tests)
  - Examples: r→ㄱ, rk→가, sj→너, el→디, etc.
- **Double chosung**: Tensed consonants (5 tests)
  - Examples: Rk→까, Ej→떠, Ql→삐, Th→쏘, Wn→쭈

**Vowel Variations Tests**
- **Compound jungsung**: Complex vowels (6 tests)
  - Examples: rhk→과, sho→놰, ehl→되, etc.

**Final Consonant Tests**
- **Jongsung**: Single final consonants (10 tests)
  - Examples: rkr→각, sjs→넌, fhf→롤, etc.
- **Compound jongsung**: Multiple final consonants (10 tests)
  - Examples: rkrt→갃, sjsw→넍, elsg→딚, etc.

**Complex Composition Tests**
- **Advanced combinations**: Multiple syllables (4 tests)
  - Examples: rkrdk→각아, sjsrkr→넌각, etc.

**Real-World Tests**
- **Korean words**: Common vocabulary (10 tests)
  - 안녕 (hello), 한국어 (Korean language), 테스트 (test), 사랑 (love), etc.
- **Korean phrases**: Complete sentences (6 tests)
  - 안녕하세요 (formal greeting)
  - 감사합니다 (thank you)
  - 죄송합니다 (sorry)
  - 사랑합니다 (I love you)
  - 값이얼마에요 (How much is this?)
  - 오래간만입니다 (Long time no see)

### Test Results

**Status**: ✅ All tests passing
- **Total tests**: 67
- **Passed**: 67
- **Failed**: 0
- **Success rate**: 100%

The test suite validates that the Korean IME correctly handles:
- Unicode Hangul syllable composition
- Complex consonant and vowel combinations
- Final consonant variations and combinations
- Real-world Korean text input and composition

## Key Features Implementation

### Korean Character Composition

The IME handles Hangul syllable composition according to Unicode standard:
- Cho (초성): 19 consonants
- Jung (중성): 21 vowels
- Jong (종성): 28 final consonants (including empty)

Syllable code = 0xAC00 + (cho × 21 × 28) + (jung × 28) + jong

### Input Processing Flow

1. **Keystroke Input**: QWERTY keys typed by user
2. **Key Mapping**: Conversion to jamo (자모) notation
3. **Syllable Detection**: Recognize complete syllable boundaries
4. **Composition**: Combine cho+jung+jong into Unicode syllable
5. **Output**: Display composed Korean text

### GUI Keyboard

- Visual Korean character display on each key
- Color-coded keys (normal, shift, backspace)
- Clickable buttons for touch/mouse input
- Real-time character preview

## Build Configuration

### Makefile Details

The `Makefile` provides an automated build system for Korean IME tests:

**Key Features:**
- **Smart Library Detection**: Uses `pkg-config` to find system MiniGUI installation
- **Fallback Configuration**: Falls back to local `install` directory if system MiniGUI unavailable
- **Test Targets**: Provides both simple and comprehensive test compilation
- **Dependency Management**: Automatically tracks source file dependencies

**Example Usage:**
```bash
# Build all tests
make

# Build specific test
make test_korean_simple

# Rebuild from scratch
make clean && make

# Run with verbose output
make VERBOSE=1
```

### Compilation Flags

Core application flags:
- `-DNATIVE`: Native Linux build
- `-D_STAND_ALONE`: Standalone application
- `-DKBD_TOOLTIP`: Enable keyboard tooltips

Test compilation flags:
- `-Wall -Wextra`: Enable all warnings
- `-g`: Include debug symbols
- `-I./ime/libime`: Include Korean IME headers
- `-I./ime`: Include IME framework headers

## File Formats

- **Input**: QWERTY keystrokes
- **Internal**: UTF-8 encoded Korean characters
- **Display**: Wide characters (wchar_t) for composition

## Development

### Code Style

- C programming language
- Unix-style indentation (spaces)
- Function naming: `snake_case`
- Comments: Korean and English mixed

### Adding New Features

1. Core functionality in `ime/libime/ime_korean.c`
2. GUI updates in `ime/softkeyboard/ko_kbd.c`
3. Test cases in `test_korean_input.c`
4. Build configuration in `Makefile`

## Known Limitations

- Some complex syllable combinations may not compose perfectly
- Space handling needs refinement
- Long sentence input may have performance impact

## License

Copyright (C) 2024 Korean IME Implementation

## Contact

For issues, questions, or contributions, please refer to the project repository.

## Acknowledgments

- Based on MiniGUI framework
- Uses Dubeolsik standard layout
- Korean font: NanumGothic

