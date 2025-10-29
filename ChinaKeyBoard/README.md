# Korean Keypad MiniGUI Application

A complete Korean keyboard input system with MiniGUI integration, supporting Korean character composition, diphthongs, and visual keyboard interface.

## Features

✅ **Korean Character Composition** - Complete Hangul syllable formation  
✅ **Diphthong Support** - Double vowel combinations (ㅗ+ㅣ=ㅚ, ㅜ+ㅣ=ㅟ, etc.)  
✅ **Shift Functionality** - Double consonants (ㅂ→ㅃ, ㅈ→ㅉ, ㄷ→ㄸ, etc.)  
✅ **Korean QWERTY Layout** - Standard Korean keyboard mapping  
✅ **MiniGUI Integration** - Full GUI application with visual keyboard  
✅ **Input Method Engine** - Advanced Korean IME with composition logic  

## Applications

### Main Applications
- **`simple_korean_keypad`** - Main Korean keypad GUI application
- **`korean_keypad_app`** - Alternative Korean keypad with enhanced features

### Korean QWERTY Layout
```
First row:  ㅂㅈㄷㄱㅅㅛㅕㅑㅐㅔ  (qwertyuiop)
Second row: ㅁㄴㅇㄹㅎㅗㅓㅏㅣ  (asdfghjkl)  
Third row:   ㅋㅌㅊㅍㅠㅜㅡ    (zxcvbnm)
```

### Diphthong Combinations
- ㅗ + ㅏ = ㅘ (wa) - h + k
- ㅗ + ㅐ = ㅙ (wae) - h + K  
- ㅗ + ㅣ = ㅚ (oe) - h + l
- ㅜ + ㅓ = ㅝ (wo) - n + O
- ㅜ + ㅔ = ㅞ (we) - n + j
- ㅜ + ㅣ = ㅟ (wi) - n + l
- ㅡ + ㅣ = ㅢ (ui) - m + l

## Usage

### Run Korean Keypad Application
```bash
./run_korean_keypad.sh
```

### Manual Execution
```bash
export LD_LIBRARY_PATH=./install/lib:$LD_LIBRARY_PATH
./simple_korean_keypad
```

## Project Structure

```
ChinaKeyBoard/
├── ime/                    # Korean IME implementation
│   ├── libime/            # IME core logic
│   │   ├── ime_korean.c   # Korean composition engine
│   │   └── ime_korean.h   # Korean IME headers
│   ├── softkeyboard/      # Keyboard layouts
│   └── softkeypad/        # Keypad implementations
├── animate/               # Animation libraries
├── install/               # MiniGUI installation
├── simple_korean_keypad   # Main executable
├── korean_keypad_app.c    # Alternative GUI app
└── run_korean_keypad.sh   # Launcher script
```

## Building

The project uses the existing Makefile system:
```bash
make clean
make
```

## Korean Character Examples

- **Basic syllables:** r + k = 가 (ga), g + k + s = 한 (han)
- **Diphthongs:** h + l = ㅚ (oe), n + l = ㅟ (wi), m + l = ㅢ (ui)
- **Double consonants:** Q + k = 빠 (ppa), R + k = 까 (kka)

## Technical Details

- **Input Method Engine:** Complete Korean composition logic
- **UTF-8 Encoding:** Proper Korean character encoding
- **MiniGUI Integration:** Native GUI framework support
- **State Management:** Advanced composition state handling
- **Visual Feedback:** Real-time character display

## Requirements

- MiniGUI 5.0.16 (included in install/)
- Linux with X11 support
- GCC compiler

---

**Korean Keypad MiniGUI Application** - Complete Korean input system with advanced composition features! 🇰🇷
