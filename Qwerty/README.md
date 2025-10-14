# Korean/English/Number QWERTY Keypad

A GTK-based graphical QWERTY keyboard application in C that supports Korean (Hangul), English, numbers, and special characters.

## Features

- **Visual keyboard interface**: Interactive GUI with clickable keys
- **Multi-language support**: Switch between English and Korean layouts
- **Complete QWERTY layout**: All standard keys including:
  - Letters (a-z, A-Z)
  - Numbers (0-9)
  - Special characters (!, @, #, $, %, ^, &, *, etc.)
  - Special keys (Shift, Caps Lock, Tab, Enter, Backspace, Space)
- **Visual feedback**: Active modifiers (Shift, Caps Lock) are highlighted in green
- **Real-time updates**: Button labels change dynamically when switching languages
- **Korean Hangul characters**: Standard 2-Set Korean keyboard layout (두벌식)
- **Hangul composition**: Automatic combining of Korean jamos into complete syllables (ㄱ + ㅏ → 가)
- **Text display area**: Shows typed text with scrolling support

## Keyboard Layout

### Row 0 (Numbers and Symbols)
```
` 1 2 3 4 5 6 7 8 9 0 - = [BKSP]
~ ! @ # $ % ^ & * ( ) _ +
```

### Row 1 (QWERTY)
```
[TAB] q w e r t y u i o p [ ] \
      Q W E R T Y U I O P { } |
      ㅂ ㅈ ㄷ ㄱ ㅅ ㅛ ㅕ ㅑ ㅐ ㅔ
      ㅃ ㅉ ㄸ ㄲ ㅆ ㅛ ㅕ ㅑ ㅒ ㅖ
```

### Row 2 (ASDF)
```
[CAPS] a s d f g h j k l ; ' [ENTER]
       A S D F G H J K L : "
       ㅁ ㄴ ㅇ ㄹ ㅎ ㅗ ㅓ ㅏ ㅣ
```

### Row 3 (ZXCV)
```
[SHIFT] z x c v b n m , . / [SHIFT]
        Z X C V B N M < > ?
        ㅋ ㅌ ㅊ ㅍ ㅠ ㅜ ㅡ
```

### Row 4 (Control)
```
[한/영] [SPACE] [CLEAR]
```

## Project Structure

```
Qwerty/
├── qwerty.h           # Core keyboard logic header
├── qwerty.c           # Core keyboard logic implementation
├── main.c             # GTK GUI implementation
├── Makefile           # Build configuration
├── README.md          # Documentation
└── .gitignore         # Git ignore rules
```

### File Organization

- **qwerty.h / qwerty.c**: Core keyboard logic
  - Hangul composition algorithm
  - Key mappings (English/Korean)
  - State management
  - Character processing

- **main.c**: GTK GUI
  - Window and widget creation
  - Event handlers
  - User interface logic

## Requirements

- GCC compiler
- GTK+ 3.0 development libraries

### Installing GTK+ 3.0

**Ubuntu/Debian:**
```bash
sudo apt-get install libgtk-3-dev
```

**Fedora/RHEL:**
```bash
sudo dnf install gtk3-devel
```

**Arch Linux:**
```bash
sudo pacman -S gtk3
```

## Installation

### Build the application
```bash
make
```

### Manual compilation
```bash
gcc -Wall -Wextra -std=c99 $(pkg-config --cflags gtk+-3.0) -o qwerty_gtk main_gtk.c $(pkg-config --libs gtk+-3.0)
```

## Usage

Run the application:
```bash
./qwerty_gtk
```

Or use Make:
```bash
make run
```

### How to Use

1. **Type characters**: Click on any key button to add it to the text area
2. **Switch language**: Click the "한/영" button to switch between English and Korean
3. **Use modifiers**:
   - Click "Shift" to toggle shift on/off (stays active until clicked again)
   - Click "Caps" to toggle caps lock on/off (stays active until clicked again)
   - Active modifiers are highlighted in green
4. **Special keys**:
   - **Backspace** - Delete last character
   - **Enter** - Insert newline
   - **Tab** - Insert tab
   - **Space** - Insert space
   - **Clear** - Clear all text
5. **Close the window** to exit the application

## Korean Keyboard Layout

The Korean layout follows the standard 2-Set (두벌식) keyboard layout used in South Korea:
- **Consonants (자음)** on the left side: ㅂ ㅈ ㄷ ㄱ ㅅ ㅁ ㄴ ㅇ ㄹ ㅎ ㅋ ㅌ ㅊ ㅍ
- **Vowels (모음)** on the right side: ㅛ ㅕ ㅑ ㅐ ㅔ ㅗ ㅓ ㅏ ㅣ ㅠ ㅜ ㅡ
- **Double consonants** accessible via Shift: ㅃ ㅉ ㄸ ㄲ ㅆ
- **Compound vowels** via Shift: ㅒ ㅖ

### Hangul Composition

The application automatically combines Korean jamos (자모) into complete syllables:

**Example 1: 한글 (Hangeul)**
- Press ㅎ → shows: ㅎ
- Press ㅏ → combines to: 하
- Press ㄴ → adds final consonant: 한
- Press ㄱ → starts new syllable: 한ㄱ
- Press ㅡ → combines: 한그
- Press ㄹ → adds final: 한글

**Example 2: Three-part syllables**
- ㄱ + ㅏ → 가
- ㄱ + ㅏ + ㅁ → 감
- ㄱ + ㅏ + ㅁ + ㅅ → 감 + ㅅ (starts new syllable)

The composition automatically handles:
- Initial consonants (초성)
- Vowels (중성)
- Final consonants (종성)
- Syllable boundaries and transitions

## Limitations

- Requires UTF-8 locale support for Korean characters
- Advanced Korean input features (like double vowel composition) may have edge cases

## Future Enhancements
- Add physical keyboard input support (type with your keyboard, not just clicking)
- Add arrow keys and function keys (F1-F12)
- Implement clipboard operations (copy/paste)
- Add support for additional special keys (Delete, Home, End, Page Up/Down)
- Implement auto-completion or suggestions
- Add themes/skins (dark mode, custom colors)
- Add sound effects for key presses

## License

This project is provided as-is for educational purposes.
