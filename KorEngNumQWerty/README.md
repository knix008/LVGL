# Korean/English/Number QWERTY Keypad

A comprehensive virtual keyboard application built with LVGL that supports Korean input, English typing, and number/symbol input modes. Features an intuitive QWERTY layout with proper Korean character composition and real-time mode switching.

## 🌟 Features

### ✅ **Multi-Language Input Support**
- **Korean Mode**: Full Korean character composition using QWERTY key mapping
- **English Mode**: Both lowercase and uppercase English input
- **Number Mode**: Complete special character and symbol set
- **Real-time switching**: Seamless mode transitions with visual feedback

### ✅ **Korean Input System**
- **Proper Korean composition**: Real-time hangul character formation
- **Complex syllables**: Support for choseong, jungseong, and jongseong
- **Advanced combinations**: Double consonants and complex vowel combinations
- **Backspace handling**: Intelligent character deletion preserving composition state

### ✅ **Professional UI Design**
- **800x400 window**: Optimized for desktop usage
- **Responsive layout**: Buttons scale properly with window size
- **Visual feedback**: Dynamic shift button colors and states
- **Centered alignment**: Professional keyboard layout with proper spacing

### ✅ **Smart Button Sizing**
- **Mathematical proportions**: Space button = 5 button widths + gaps
- **Enter button**: 2 button widths + gap for consistent sizing
- **10px offset**: All keyboard rows properly aligned
- **Centered 4th row**: Space and Enter buttons horizontally centered

### ✅ **Advanced Features**
- **Shift functionality**: Different behavior per mode (Korean double chars, English case)
- **Mode indicators**: Clear visual indication of current input mode
- **Special characters**: Complete set of programming and mathematical symbols
- **Memory safe**: Proper buffer management and character encoding

## 🎯 Keyboard Layouts

### Korean Mode (QWERTY Layout)
| Row | Keys |
|-----|------|
| **Row 1** | ㅂ ㅈ ㄷ ㄱ ㅅ ㅛ ㅕ ㅑ ㅐ ㅔ |
| **Row 2** | ㅁ ㄴ ㅇ ㄹ ㅎ ㅗ ㅓ ㅏ ㅣ |
| **Row 3** | [Shift] ㅋ ㅌ ㅊ ㅍ ㅠ ㅜ ㅡ [Backspace] |
| **Row 4** | [Space] [Enter] |

**Shift Mode (Double Consonants/Vowels)**:
| Row | Keys |
|-----|------|
| **Row 1** | ㅃ ㅉ ㄸ ㄲ ㅆ ㅛ ㅕ ㅑ ㅒ ㅖ |

### English Mode
| Row | Keys |
|-----|------|
| **Lowercase** | q w e r t y u i o p |
|               | a s d f g h j k l |
|               | [Shift] z x c v b n m [Backspace] |
| **Uppercase** | Q W E R T Y U I O P |
|               | A S D F G H J K L |
|               | [Shift] Z X C V B N M [Backspace] |

### Number/Symbol Mode
| Row | Keys |
|-----|------|
| **Row 1** | 1 2 3 4 5 6 7 8 9 0 |
| **Row 2** | ! @ # $ % ^ & * ( ) |
| **Row 3** | [Shift] + = [ ] { } \| \\ ; : [Backspace] |

## 🔧 Special Functions

| Button | Function | Behavior |
|--------|----------|----------|
| **Mode** | Switch input mode | Korean → English → Number → Korean |
| **Shift** | Mode-specific | Korean: Double chars, English: Case toggle, Number: Disabled |
| **Space** | Insert space | Ends Korean composition, adds space character |
| **Enter** | Line break | Inserts newline character |
| **Backspace** | Delete character | Smart deletion preserving Korean composition |
| **Clear** | Clear all text | Resets entire input buffer |

## 📝 Usage Examples

### Korean Text Input
```
Click "ㅂ" → "ㅏ" → "ㄱ" → 밥 (rice)
Click "ㅇ" → "ㅏ" → "ㄴ" → "ㄴ" → "ㅕ" → "ㅇ" → 안녕 (hello)
Click "ㄱ" → "ㅏ" → "ㅁ" → "ㅅ" → "ㅏ" → "ㅎ" → "ㅏ" → 감사하 (thank)
```

### Mode Switching
```
1. Start in Korean mode
2. Click "Mode" button → Switch to English mode
3. Type English text with proper case using Shift
4. Click "Mode" button → Switch to Number mode
5. Access all special characters and symbols
6. Click "Mode" button → Return to Korean mode
```

### Shift Button Usage
- **Korean Mode**: Click Shift → Orange color → Access ㅃ, ㅉ, ㄸ, ㄲ, ㅆ, ㅒ, ㅖ
- **English Mode**: Click Shift → Orange color → Toggle between abc/ABC
- **Number Mode**: Shift button disabled (grayed out)

## 🚀 Quick Start

### Prerequisites
```bash
# Ubuntu/Debian
sudo apt install build-essential cmake libsdl2-dev libfreetype6-dev

# CentOS/RHEL
sudo yum install gcc-c++ cmake SDL2-devel freetype-devel

# macOS
brew install cmake sdl2 freetype
```

### Build and Run
```bash
# Clone the repository
git clone <repository-url>
cd KorEngNumQWerty

# Build the project
./build.sh

# Run the application
./run.sh

# Or build manually:
cd Source
mkdir -p build && cd build
cmake ..
make -j$(nproc)
./KorEngNumQWerty
```

### Quick Scripts
```bash
# Build only
./build.sh

# Run directly (builds if needed)
./run.sh

# Clean build
rm -rf Source/build
./build.sh
```

## 📁 Project Structure

```
KorEngNumQWerty/
├── README.md                   # This file
├── build.sh                    # Build script
├── run.sh                      # Run script
└── Source/
    ├── CMakeLists.txt          # Build configuration
    ├── include/
    │   ├── font_config.h       # Font configuration
    │   ├── qwerty_korean.h     # Korean input interface
    │   ├── qwerty.h            # Main keyboard interface
    │   ├── ui_callbacks.h      # UI event handlers
    │   └── ui_components.h     # UI component definitions
    ├── src/
    │   ├── main.c              # Application entry point (800x400 window)
    │   ├── qwerty.c            # Main keyboard implementation
    │   ├── qwerty_korean.c     # Korean input system
    │   └── ui_components.c     # UI component implementation
    ├── assets/
    │   ├── NanumGothic-Bold.ttf      # Korean font (Bold)
    │   ├── NanumGothic-ExtraBold.ttf # Korean font (Extra Bold)
    │   └── NanumGothic-Regular.ttf   # Korean font (Regular)
    ├── lvgl/                   # LVGL graphics library
    └── build/                  # Build output directory
        └── KorEngNumQWerty     # Executable
```

## 🔧 Recent Improvements

### ✅ **Window Size Optimization**
- **Updated**: Window size changed from 700x360 to 800x400
- **Benefit**: Better screen utilization and improved button spacing
- **Result**: More professional desktop application appearance

### ✅ **Tab Alignment Enhancement**
- **Fixed**: Tab width aligned to match title bar (800px full width)
- **Improvement**: Edge-to-edge interface with no side margins
- **Result**: Professional, consistent visual layout

### ✅ **Keyboard Layout Improvements**
- **Positioning**: All keyboard rows moved 10px to the right for better balance
- **Second row**: Additional 30px right offset for traditional QWERTY stagger
- **Result**: Improved visual hierarchy and familiar keyboard feel

### ✅ **Number Mode Character Set**
- **Enhanced**: Replaced incomplete character set with comprehensive symbols
- **Added**: Programming symbols (brackets, pipes, backslash)
- **Removed**: Empty button slots for complete professional layout
- **Result**: Full 30-button utilization with useful special characters

### ✅ **Smart Button Sizing System**
- **Space button**: Calculated as 5 × button_width + 4 × gaps = 291px
- **Enter button**: Calculated as 2 × button_width + 1 × gap = 114px
- **Benefit**: Mathematical consistency and automatic scaling
- **Result**: Professional proportions with logical size relationships

### ✅ **4th Row Centering**
- **Alignment**: Space and Enter buttons centered horizontally in 800px window
- **Calculation**: Perfect center positioning with equal side margins
- **Result**: Balanced, symmetrical keyboard layout

### ✅ **Shift Button Visual Feedback**
- **Korean mode**: Orange when active (ㅃㅉ), gray when inactive (ㅂㅈ)
- **English mode**: Orange for uppercase (ABC), gray for lowercase (abc)
- **Number mode**: Disabled/dimmed appearance
- **Enhancement**: Clear text indicators showing current shift state
- **Result**: Intuitive visual feedback for all input modes

### ✅ **Backspace Functionality Fix**
- **Problem**: Backspace was removing all characters and blocking further input
- **Solution**: Fixed Korean composition buffer management and state handling
- **Enhancement**: Improved space handling to properly end Korean composition
- **Result**: Single-character deletion with preserved input capability

## 🧪 Testing and Quality

The Korean input system includes comprehensive functionality testing:

- ✅ Real-time Korean character composition
- ✅ Proper hangul syllable formation (choseong + jungseong + jongseong)
- ✅ Backspace handling preserving composition state
- ✅ Mode switching without data loss
- ✅ Special character input validation
- ✅ Memory safety and buffer management
- ✅ UI responsiveness and visual feedback

**Current Status**: All core functionality tested and working correctly

## 🏗️ Technical Architecture

### LVGL Integration
- **Graphics Library**: LVGL 9.4.0-dev with SDL2 backend
- **Window Management**: SDL2 windowing system (800x400)
- **Font Rendering**: FreeType integration with Korean font support
- **Memory Management**: Optimized for 256KB memory allocation

### Korean Input Engine
- **Input Processing**: Real-time Korean character composition
- **State Management**: Proper handling of Korean syllable building
- **Buffer Management**: Separate input and output buffers for composition
- **Character Mapping**: QWERTY-to-Korean key mapping system

### UI Architecture
- **Component-based**: Modular UI components with clear separation
- **Event-driven**: Responsive button handling and mode switching
- **Layout System**: Mathematical button sizing and positioning
- **Theme Support**: Consistent styling with visual feedback

### Key Components
- `qwerty.c` - Main keyboard implementation and layout
- `qwerty_korean.c` - Korean input processing and composition
- `ui_components.c` - UI component creation and management
- `main.c` - Application entry point and SDL window setup

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Make your changes with proper testing
4. Ensure Korean input functionality remains intact
5. Test all three input modes (Korean/English/Number)
6. Commit your changes (`git commit -m 'Add amazing feature'`)
7. Push to the branch (`git push origin feature/amazing-feature`)
8. Open a Pull Request

## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.

## 🙏 Acknowledgments

- **LVGL Team**: For the excellent embedded graphics library
- **SDL2 Project**: For cross-platform windowing and input handling
- **FreeType Project**: For high-quality font rendering
- **Nanum Gothic Fonts**: For beautiful Korean typography
- **Korean Language Community**: For input method requirements and feedback

## 📞 Support

For issues, questions, or contributions:

1. **Bug Reports**: Open an issue with detailed steps to reproduce
2. **Feature Requests**: Describe the enhancement with use cases
3. **Korean Input Issues**: Provide specific Korean text examples
4. **UI/Layout Issues**: Include screenshots if possible
5. **Build Problems**: Include your system info and error messages

### System Requirements
- **OS**: Linux (primary), Windows (with SDL2), macOS (with homebrew)
- **Memory**: 256MB+ RAM recommended
- **Display**: 800x400 minimum resolution
- **Dependencies**: SDL2, FreeType, CMake, GCC/Clang

---

**Status**: ✅ Production Ready - Multi-language virtual keyboard with professional UI

**Version**: 2.0.0 - Complete QWERTY implementation with Korean/English/Number support 