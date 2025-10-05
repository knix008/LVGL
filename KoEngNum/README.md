# Korean English Number Input System

A comprehensive multi-mode input system for Korean, English, and Number entry built with LVGL (Light and Versatile Graphics Library) and SDL2. This project includes two implementations:

1. **chunjiin_kbd/** - Compact phone-style keyboard (400x640)
2. **Source/** - Full-featured tabbed interface (320x640)

## Projects Overview

### 1. Chunjiin Keyboard (chunjiin_kbd/)

A compact, mobile-style keyboard optimized for phone-like input experience.

**Features:**
- **Three Input Modes**: Korean (Chunjiin), English (T9), Number pad
- **Compact Design**: 400x640 window optimized for vertical display
- **Korean Font Support**: NanumGothic font for all button labels and text
- **T9 English Input**: Character cycling with uppercase/lowercase toggle
- **Visual Feedback**: Color-coded buttons (blue default, orange for Shift)
- **Popup Results**: Enter button shows input result in popup dialog
- **Character Replacement**: Smart T9 input replaces characters on repeated button press

**Input Modes:**
- **Korean Mode**: Chunjiin (천지인) method with real-time composition
  - Vowels: ㅣ, ㆍ, ㅡ (Heaven, Earth, Human)
  - Consonants: ㄱㅋ, ㄴㄹ, ㄷㅌ, ㅂㅍ, ㅅㅎ, ㅈㅊ, ㅇㅁ
  - Controls: Space, Enter, Backspace (←)

- **English Mode**: T9 phone-style input
  - Letter groups: .,!?, abc, def, ghi, jkl, mno, pqrs, tuv, wxyz
  - Shift key toggles uppercase/lowercase (visual feedback)
  - Character cycling: Click same button to cycle (a→b→c→a)
  - Character replacement instead of addition

- **Number Mode**: Standard numeric keypad (0-9)
  - Simple digit input
  - Space, 0, Backspace on row 4
  - Enter on row 5

**Quick Start:**
```bash
cd chunjiin_kbd
./build.sh
./run.sh
```

**Project Structure:**
```
chunjiin_kbd/
├── src/
│   ├── main.c              # Application entry point
│   ├── keyboard_ui.c       # Main keyboard UI and mode switching
│   ├── chunjiin_input.c    # Korean Chunjiin input logic (822 lines)
│   ├── english_input.c     # T9 English input with shift
│   └── number_input.c      # Number pad input
├── include/
│   ├── keyboard_ui.h       # Keyboard API and mode definitions
│   ├── chunjiin_input.h    # Chunjiin input API
│   ├── english_input.h     # English input API
│   └── number_input.h      # Number input API
├── fonts/
│   └── NanumGothic.ttf     # Korean font
├── build.sh                # Build script
├── run.sh                  # Run script
└── CMakeLists.txt          # Build configuration
```

### 2. Tabbed Interface (Source/)

Full-featured interface with tabbed mode switching and enhanced UI.

**Features:**
- **Portrait Display Optimized**: 320 x 640 window size for mobile-like experience
- **Mode Switching**: Easy toggle between input modes via top-right button
- **Unified Korean Font Support**: Consistent NanumGothic font across all modes
- **Shift Functionality**: Dynamic uppercase/lowercase in English mode
- **Visual Feedback**: Button labels change based on input state
- **Popup Results**: Display input results with styled dialogs
- **FreeType Integration**: High-quality Korean font rendering
- **Memory Optimized**: Efficient font caching and resource management

**Quick Start:**
```bash
cd Source
./build.sh
./run.sh
```

**Project Structure:**
```
Source/
├── assets/               # Font files (NanumGothic TrueType fonts)
├── src/
│   ├── main.c            # Application entry point
│   ├── ui_components.c   # Main UI, mode switching, font management
│   ├── chunjiin_input.c  # ChunJiIn input logic
│   ├── tab_chunjiin.c    # Korean input UI
│   ├── tab_english.c     # English T9 input UI
│   └── tab_number.c      # Number pad UI
├── include/              # Header files
├── build.sh              # Build script
├── run.sh                # Run script
└── CMakeLists.txt        # CMake configuration
```

## Requirements

- **Operating System**: Linux (Ubuntu/Debian recommended)
- **Dependencies**:
  - CMake (>= 3.10)
  - SDL2
  - FreeType2
  - GCC/Clang compiler
  - pkg-config

## Installation

### 1. Install Dependencies

On Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install build-essential cmake pkg-config libsdl2-dev libfreetype-dev
```

### 2. Build a Project

**Chunjiin Keyboard:**
```bash
cd chunjiin_kbd
./build.sh
```

**Tabbed Interface:**
```bash
cd Source
./build.sh
```

**Build Script Options:**
```bash
./build.sh          # Normal build
./build.sh clean    # Clean all build artifacts and executables
./build.sh help     # Show available commands
```

## Usage

### Running the Applications

**Chunjiin Keyboard:**
```bash
cd chunjiin_kbd
./run.sh
```

**Tabbed Interface:**
```bash
cd Source
./run.sh
```

Or run directly:
```bash
cd chunjiin_kbd/build && ./chunjiin_keyboard
# or
cd Source/build && ./main
```

## Key Features Comparison

| Feature | Chunjiin Keyboard | Tabbed Interface |
|---------|------------------|------------------|
| Window Size | 400x640 | 320x640 |
| UI Style | Button matrix | Tabbed layout |
| Mode Switching | Integrated | Top-right button |
| Korean Font | Yes (buttons + text) | Yes (unified) |
| T9 English | Yes (with replacement) | Yes (basic) |
| Shift Visual | Yes (orange color) | Yes (green color) |
| Popup Results | Yes | Yes |
| Modular Design | Separate input modules | Tab-based modules |

## Korean Input Method

Both implementations use the **Chunjiin (천지인)** input method:

**Basic Vowel Elements:**
- **ㅣ** (vertical bar) - "천" (Heaven)
- **ㆍ** (dot) - "지" (Earth)
- **ㅡ** (horizontal bar) - "인" (Human)

**Consonant Groups:**
- **ㄱㅋ** (G/K) - ㄱ → ㅋ → ㄲ (repeated presses)
- **ㄴㄹ** (N/L) - ㄴ → ㄹ
- **ㄷㅌ** (D/T) - ㄷ → ㅌ → ㄸ
- **ㅂㅍ** (B/P) - ㅂ → ㅍ → ㅃ
- **ㅅㅎ** (S/H) - ㅅ → ㅎ → ㅆ
- **ㅈㅊ** (J/C) - ㅈ → ㅊ → ㅉ
- **ㅇㅁ** (O/M) - ㅇ → ㅁ

**Features:**
- **Real-time Composition**: Characters form as you type
- **Syllable Building**: Automatically combines consonants and vowels
- **Backspace Support**: Removes last input element
- **Unicode Display**: Proper Korean character rendering

## Troubleshooting

### Font Not Loading

If Korean characters don't display:
- Check that font file exists (`fonts/NanumGothic.ttf` or `assets/NanumGothic-Regular.ttf`)
- The application falls back to built-in CJK fonts automatically
- Verify `LV_FONT_SOURCE_HAN_SANS_SC_16_CJK` is enabled in `lvgl/lv_conf.h`

### SDL2 Errors

If SDL2 window doesn't appear:
```bash
# Reinstall SDL2
sudo apt-get install --reinstall libsdl2-dev
```

### Build Errors

If you encounter build errors:
```bash
# Clean build using build script
./build.sh clean

# Reinstall dependencies
sudo apt-get install build-essential cmake pkg-config libsdl2-dev libfreetype-dev

# Rebuild
./build.sh
```

**Alternative manual clean:**
```bash
# Manual clean (if build script fails)
rm -rf build
```

## Development

### Adding New Features

Both projects share similar architecture and can be extended:

1. **Create Input Module**: Add new input logic in `src/your_input.c`
2. **Update UI**: Modify keyboard UI or add new tab
3. **Font Support**: Use centralized font management functions
4. **Build Configuration**: Update `CMakeLists.txt`

### Font Integration

Both projects use NanumGothic font:

**Chunjiin Keyboard:**
- Font loaded via FreeType in `keyboard_ui.c`
- Applied to buttons, mode label, and text area
- Path: `../fonts/NanumGothic.ttf`

**Tabbed Interface:**
- Centralized font loading in `ui_components.c`
- Access via `get_korean_font()` functions
- Path: `../assets/NanumGothic-Regular.ttf`

## Technical Details

### Chunjiin Input Processing

The Chunjiin implementation uses a state machine:
- **STATE_START**: Initial state, waiting for first input
- **STATE_CHOSEONG**: Processing initial consonant
- **STATE_JUNGSEONG**: Processing vowel
- **STATE_JONGSEONG**: Processing final consonant

**Unicode Composition:**
- Base: 0xAC00 (가)
- Formula: base + (choseong × 588) + (jungseong × 28) + jongseong
- Result: Proper Korean syllable characters

### T9 English Input

Character cycling with smart replacement:
- Track last pressed key and timestamp
- Cycle through character group: a→b→c→a
- Replace last character if same key within 2 seconds
- Add new character if different key or timeout
- Uppercase conversion when Shift is active

## License

This project uses LVGL which is licensed under the MIT License.

## Credits

- **LVGL**: https://lvgl.io/
- **Font**: NanumGothic (Naver Corporation)
- **ChunJiIn Input Method**: Traditional Korean input system

## Version History

### Chunjiin Keyboard (chunjiin_kbd/)

- **v1.0** - Initial Release
  - Compact button matrix design (400x640)
  - Three input modes: Korean, English, Number
  - Korean font integration for all UI elements
  - T9 English with character replacement
  - Shift visual feedback (orange highlight)
  - Enter button popup dialog
  - Modular input processing (separate files)

### Tabbed Interface (Source/)

- **v2.0** - Major Feature Update
  - Enhanced English Mode with shift functionality
  - Font Integration: Unified Korean font support
  - Visual Improvements: Consistent button positioning
  - Symbol Support: Proper backspace arrow (←) display
  - UI Polish: Green shift button when active
  - Documentation: Comprehensive font integration docs

- **v1.0** - Initial Release
  - Korean ChunJiIn input method
  - English T9 input (basic)
  - Number pad functionality
  - 320x640 portrait mode optimization
  - Basic mode switching system
