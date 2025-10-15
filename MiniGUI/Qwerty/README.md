# Korean QWERTY Input System

A comprehensive Korean virtual keyboard implementation using MiniGUI framework with full QWERTY layout and advanced Hangul composition support.

## Features

### 🔤 **Complete QWERTY Layout**
- Full 4-row keyboard layout with proper key spacing
- Dual Shift keys (Left and Right) in Row 3
- All standard punctuation and special characters
- Backspace and Enter functionality

### 🇰🇷 **Advanced Korean Input**
- **Hangul Composition**: Real-time Korean character assembly
- **Compound Vowels**: Support for complex vowels (ㅜ+ㅓ=ㅝ, ㅗ+ㅏ=ㅘ, etc.)
- **Complete Jamo Set**: All Korean consonants and vowels
- **Proper Syllable Formation**: Automatic cho-jung-jong composition

### 🎨 **User Interface**
- **Centered Layout**: Mathematically calculated button positioning
- **Visual Feedback**: Shift button state indication
- **Mode Switching**: Korean/English toggle with visual indicators
- **Real-time Display**: Live text composition preview

### 🛠 **Technical Features**
- **Font Support**: NanumGothic Bold/ExtraBold for proper character rendering
- **Unicode Support**: Full UTF-8 text handling
- **Memory Management**: Proper resource cleanup
- **Error Handling**: Graceful font fallback system

## Project Structure

```
├── main.c              # Main application and UI logic
├── qwerty-korean.c     # Korean input processing and character composition
├── qwerty.h           # Header file with data structures and function declarations
├── Makefile           # Build configuration
├── MiniGUI.cfg        # MiniGUI runtime configuration
├── install_minigui.sh # MiniGUI installation script
└── assets/            # Font files
    ├── NanumGothic-Bold.ttf
    ├── NanumGothic-ExtraBold.ttf
    └── NanumGothic-Regular.ttf
```

## Building and Running

### Prerequisites
- MiniGUI 5.0+ with threading support
- GCC compiler
- Linux environment (tested on Ubuntu 24.04)
- Korean font support

### Installation
1. **Install MiniGUI** (if not already installed):
   ```bash
   ./install_minigui.sh
   ```

2. **Build the application**:
   ```bash
   make clean && make
   ```

3. **Run the Korean QWERTY input system**:
   ```bash
   ./qwerty
   ```

## Usage

### Basic Operation
- **Korean/English Toggle**: Click the "한글"/"Eng" button or use mode switching
- **Shift Keys**: Press either Left or Right Shift for capitalization/special characters
- **Text Input**: Click keys to compose text in the display area
- **Backspace**: Delete characters with the ← button
- **Enter**: Complete input and show result dialog

### Korean Input Examples
- **Simple Characters**: ㄱ + ㅏ = 가
- **Complex Syllables**: ㄱ + ㅜ + ㅓ + ㄴ = 권 (compound vowel ㅜ+ㅓ=ㅝ)
- **Final Consonants**: ㅎ + ㅏ + ㄴ = 한

### Keyboard Layout
```
Row 0: ` 1 2 3 4 5 6 7 8 9 0 - = [Backspace]
Row 1:   Q W E R T Y U I O P [ ] \
Row 2:    A S D F G H J K L ; '
Row 3: [Shift] Z X C V B N M , . / [Shift]
Row 4:      [한글/Eng] [Space] [Enter]
```

## Technical Implementation

### Korean Character Composition
The system implements proper Hangul composition following Unicode standards:
- **Initial Consonants (초성)**: 19 consonants
- **Vowels (중성)**: 21 vowels including compound forms
- **Final Consonants (종성)**: 28 consonant combinations

### Font Handling
- **Primary**: NanumGothic-Bold (optimal character coverage)
- **Fallback**: NanumGothic-ExtraBold → NanumGothic-Regular
- **Character Issues**: Resolved zero-width backtick in Regular font

### Memory Management
- Proper MiniGUI resource cleanup
- Font destruction on application exit
- Window and button resource management

## Configuration

### MiniGUI Configuration (MiniGUI.cfg)
```ini
[system]
gal_engine=pc_xvfb
defaultmode=800x600-16bpp

[pc_xvfb]
window_caption=Korean QWERTY Input
bg_color=0xC0C0C0
```

### Compile Options
- **Threading**: `-lminigui_ths` for thread-safe operation
- **Font Support**: FreeType and HarfBuzz integration
- **Image Support**: PNG and JPEG support

## Troubleshooting

### Common Issues
1. **Font Errors**: `Invalid charset name` warnings are normal and don't affect functionality
2. **Display Issues**: Ensure proper Korean locale support (`en_US.UTF-8`)
3. **Build Errors**: Verify MiniGUI development headers are installed

### Debug Output
The application provides verbose logging for:
- Font loading status
- Korean character composition
- Shift key state changes
- Mode switching events

## Known Limitations
- Font cache warnings for some Korean characters (cosmetic only)
- Requires X11 environment for pc_xvfb GAL engine
- NanumGothic-Regular has zero-width backtick character

## Development Notes

### Key Features Implemented
- ✅ Dual Shift key support in Row 3
- ✅ Compound vowel composition (권, 봐, etc.)
- ✅ Centered keyboard layout
- ✅ Font fallback system for character coverage
- ✅ Real-time Korean text composition

### Architecture
- **Modular Design**: Separate Korean processing from UI logic
- **State Management**: Comprehensive keyboard and composition state
- **Event Handling**: Proper MiniGUI message processing
- **Resource Management**: Clean initialization and cleanup

## License
This project is part of the MiniGUI ecosystem and follows standard open-source practices.

## Contributing
Contributions welcome for:
- Additional language support
- UI improvements
- Performance optimizations
- Bug fixes and testing

---
**Author**: Korean QWERTY Input System Development Team  
**Framework**: MiniGUI 5.0+  
**Platform**: Linux (Ubuntu 24.04+ recommended)  
**Last Updated**: October 2025