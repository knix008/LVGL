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
- MiniGUI development libraries
- FreeType, PNG, JPEG libraries

### Build Commands

```bash
# Check dependencies
make check-deps

# Setup MiniGUI (if not already built)
make setup-minigui

# Build the application
make

# Build and run Korean IME tests
make test_korean

# Run all tests
./run_test.sh
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

The project includes comprehensive test suites for the Korean IME:

### Running Tests

```bash
# Run all tests
./run_test.sh

# Run simple test (quick verification)
./test_korean_simple

# Run comprehensive test (all cases)
./test_korean_input
```

### Test Coverage

The test suite includes:
- **Single chosung + jungsung**: Basic syllable composition
- **Double chosung**: Tensed consonants (ㄲ, ㄸ, ㅃ, ㅆ, ㅉ)
- **Compound jungsung**: Complex vowels (ㅘ, ㅙ, ㅚ, ㅟ, etc.)
- **Jongsung (final consonants)**: Single and compound endings
- **Korean words**: Common words (안녕, 한국, 컴퓨터, 사랑, 친구, etc.)
- **Korean phrases**: 
  - Formal greetings: 안녕하세요
  - Polite expressions: 감사합니다, 죄송합니다
  - Daily conversation: 사랑합니다, 값이얼마예요, 오래간만입니다

Total: 67 test cases with 100% pass rate

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

## Compilation Flags

- `-DNATIVE`: Native Linux build
- `-D_STAND_ALONE`: Standalone application
- `-DKBD_TOOLTIP`: Enable keyboard tooltips

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

