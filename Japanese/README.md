# Japanese Input Application

A modern Japanese character input application built with LVGL, supporting multiple input methods including Hiragana, Katakana, English, Numbers, and Special Characters. Features a mobile phone-style keypad interface with flick input method for intuitive character selection.

## Features

### 🎯 Core Functionality
- **Multi-mode Input**: Support for Hiragana, Katakana, English (lowercase/uppercase), Numbers, and Special Characters
- **Flick Input Method**: Modern smartphone-style character selection interface
- **Mobile Keypad Layout**: Traditional 12-button mobile phone keypad design
- **UTF-8 Support**: Full Unicode character support for Japanese text
- **Real-time Text Display**: Live preview of input text

### 📱 User Interface
- **Window Size**: 320×640 pixels (mobile phone aspect ratio)
- **Responsive Design**: Flex-based layout without scroll bars
- **Japanese Fonts**: TrueType font support with fallback to built-in fonts
- **Modern Styling**: Clean, professional appearance with rounded buttons
- **Color-coded Elements**: Intuitive visual feedback

### 🎨 Input Methods
- **Japanese Mode**: Unified mode with Hiragana/Katakana toggle via Shift button
  - **Hiragana**: ひらがな (あいうえお, かきくけこ, etc.) - Shift OFF
  - **Katakana**: カタカナ (アイウエオ, カキクケコ, etc.) - Shift ON
- **Alphabet Mode**: English letters with case toggle (abc/ABC via Shift)
- **Number Mode**: Digits (1, 2, 3, etc.) with disabled Shift button
- **Symbol Mode**: Japanese punctuation and symbols with dual character sets
  - **Normal Symbols**: ！？、。・：；〜－＝＋×÷％［］<>/\{}
  - **Shifted Symbols**: ！？、．・：；〜－＝＋×÷％［］<>/\{}
  - **Special Characters**: Includes `{}`, `<>/`, `\` and other programming symbols

## Architecture

### 📁 Project Structure
```
Japanese/
├── main.c                    # Main application entry point (orchestrator)
├── lvgl_init.h               # LVGL initialization header
├── lvgl_init.c               # LVGL initialization implementation
├── gui_app.h                 # GUI application header
├── gui_app.c                 # GUI application implementation
├── japanese_input.h          # Japanese input logic header
├── japanese_input.c          # Japanese input logic implementation
├── assets/
│   └── NotoSansCJK.ttc      # Japanese font file
├── tests/                    # Test suite
│   ├── test_core.c          # Core logic tests
│   ├── test_gui.c           # GUI function tests
│   ├── Makefile             # Test build system
│   └── run_tests.sh         # Test runner script
├── run_tests.sh             # Main test runner
├── Makefile                 # Main build system
├── .gitignore               # Git ignore rules
└── README.md                # This file
```

### 🔧 Technical Stack
- **Language**: C (C99 standard)
- **GUI Framework**: LVGL (Light and Versatile Graphics Library)
- **Font Rendering**: FreeType
- **Display**: SDL2
- **Build System**: GNU Make
- **Testing**: Custom test framework

### 🏗️ Modular Architecture Benefits
- **Separation of Concerns**: Each module has a single, well-defined responsibility
- **Maintainability**: Changes to one module don't affect others
- **Reusability**: Modules can be reused in different contexts
- **Testability**: Each module can be tested independently
- **Scalability**: Easy to add new features or modify existing ones
- **Code Clarity**: Clear interfaces between modules make the codebase easier to understand

### 🔌 Module Interfaces
- **`lvgl_init`**: Provides LVGL system initialization, display management, and main loop control
- **`gui_app`**: Handles user interface creation, event processing, and application state management
- **`japanese_input`**: Contains input logic, character mappings, and text processing functions
- **`main`**: Simple orchestrator that coordinates module initialization and startup

## Installation

### Prerequisites
- GCC compiler with C99 support
- LVGL library
- FreeType library
- SDL2 library
- Make build tool

### Build Dependencies (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install build-essential libfreetype6-dev libsdl2-dev pkg-config
```

### Build Dependencies (CentOS/RHEL/Fedora)
```bash
sudo yum install gcc make freetype-devel SDL2-devel pkgconfig
# or for newer versions:
sudo dnf install gcc make freetype-devel SDL2-devel pkgconfig
```

### Compilation
```bash
# Clone or download the project
cd Japanese

# Build the application
make clean
make

# Run the application
./japanese_input
```

## Usage

### 🎮 Basic Operation
1. **Launch Application**: Run `./japanese_input`
2. **Select Input Mode**: Click the Mode button to cycle through input methods
3. **Input Characters**: Click any keypad button to show flick selection
4. **Select Character**: Click on desired character from the flick popup
5. **Text Management**: Use Space, Backspace, Clear, and Enter buttons as needed

### 📝 Input Modes
- **日本語**: Japanese mode (Hiragana/Katakana via Shift toggle)
- **ABC**: English alphabet (lowercase/uppercase via Shift toggle)
- **123**: Numbers and digits (Shift button disabled)
- **記号**: Japanese symbols and punctuation (dual character sets via Shift)

### 🔄 Flick Input Method
1. Click any keypad button
2. A popup window appears with all available characters for that button
3. Click on the desired character to input it
4. The popup automatically closes after selection
5. **Single Character Shortcut**: If only one character is available, it's input directly without showing the popup

### 🔄 Shift Button Functionality
- **Japanese Mode**: Toggles between Hiragana (OFF) and Katakana (ON)
- **Alphabet Mode**: Toggles between lowercase (OFF) and uppercase (ON)
- **Number Mode**: Disabled (gray color, non-clickable)
- **Symbol Mode**: Toggles between normal and shifted symbol sets
- **Visual Feedback**: Green (inactive) / Orange (active) color coding
- **State Persistence**: Shift state is preserved when switching between modes (except Number mode)

### 📱 Keypad Layout
```
Row 1: [あ] [か] [さ]
Row 2: [た] [な] [は]
Row 3: [ま] [や] [ら]
Row 4: [Shift] [わ] [ん]
Row 5: [Space] [Clear] [Enter] [Backspace]
```

**Button Functions:**
- **Character Buttons (0-11)**: Input characters based on current mode
- **Shift Button**: Toggle character sets (Hiragana/Katakana, uppercase/lowercase, symbol sets)
- **Space Button**: Insert space character
- **Clear Button**: Clear all text in the input area
- **Enter Button**: Show input result and clear text area
- **Backspace Button**: Delete the last character
- **Mode Button**: Cycle through input modes (shows next mode)

## Testing

### 🧪 Running Tests
```bash
# Run all tests
./run_tests.sh

# Run tests from tests directory
cd tests
./run_tests.sh

# Run individual test suites
make test-core    # Core logic tests only
make test-gui     # GUI function tests only
make all          # All tests
```

### 📊 Test Coverage
- **Core Logic Tests**: 56 test cases
- **GUI Function Tests**: 212 test cases
- **Total Coverage**: 268 test cases
- **Success Rate**: 100% (all tests pass)

### 🔍 Test Categories
- Initialization and state management
- Mode switching functionality
- Character mapping verification
- Flick input processing
- Shift button functionality
- UTF-8 conversion
- Edge cases and error handling
- GUI consistency checks
- Button label verification
- Symbol character availability

## Development

### 🏗️ Build System
The project uses a simple Makefile-based build system:

```bash
# Clean build artifacts
make clean

# Build application
make

# Build with debug symbols
make CFLAGS="-g -O0"
```

### 📝 Code Structure
- **Modular Architecture**: Clean separation of concerns across multiple modules
- **LVGL Initialization Module**: Handles LVGL system setup, SDL configuration, and display management
- **GUI Application Module**: Manages user interface creation, event handling, and application logic
- **Japanese Input Module**: Contains core input logic, character mappings, and text processing
- **Main Orchestrator**: Simple entry point that coordinates module initialization
- **Testable Code**: All functions are designed for easy testing with comprehensive test coverage

### 🔧 Customization
- **Button Layout**: Modify button positions in `gui_app.c`
- **Character Mappings**: Update character arrays in `japanese_input.c`
- **Symbol Characters**: Add/modify symbols in `symbol_chars` and `symbol_chars_shifted` arrays
- **Styling**: Adjust LVGL styles in `gui_app.c`
- **Display Settings**: Modify window size and properties in `lvgl_init.c`
- **Fonts**: Replace `assets/NotoSansCJK.ttc` with preferred Japanese font
- **Mode Button Behavior**: The mode button shows the next mode instead of current mode

## Contributing

### 🚀 Development Setup
1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Run the test suite: `./run_tests.sh`
5. Ensure all tests pass
6. Submit a pull request

### 📋 Code Style
- Use C99 standard
- Follow consistent indentation (4 spaces)
- Add comments for complex logic
- Maintain clear module boundaries and interfaces
- Write tests for new functionality
- Keep modules focused on single responsibilities

## License

This project is open source. Please check the license file for specific terms.

## Acknowledgments

- **LVGL**: For providing the excellent GUI framework
- **FreeType**: For TrueType font rendering support
- **SDL2**: For cross-platform display and input handling
- **Noto Fonts**: For the Japanese font resources

## Troubleshooting

### Common Issues

**Font Loading Issues**
- Ensure `assets/NotoSansCJK.ttc` exists
- Check FreeType library installation
- Verify font file permissions

**Compilation Errors**
- Install required dependencies
- Check LVGL library path
- Verify SDL2 development packages

**Display Issues**
- Check SDL2 installation
- Verify display drivers
- Try different SDL video drivers

**Character Display Problems**
- Ensure UTF-8 locale is set
- Check font file integrity
- Verify wide character support

**Shift Button Issues**
- Check if Shift button is disabled in Number mode (this is expected behavior)
- Verify Shift state persistence when switching modes
- Ensure proper color feedback (Green = inactive, Orange = active)

**Mode Button Display**
- Mode button shows the next mode, not the current mode
- This is the intended behavior for better user experience

### Getting Help
- Check the test output for functionality verification
- Review the code comments for implementation details
- Ensure all dependencies are properly installed
- Verify system locale settings for Japanese text

## Recent Updates

### ✨ Latest Features (v2.0)
- **Unified Japanese Mode**: Consolidated Hiragana and Katakana into a single Japanese mode with Shift toggle
- **Enhanced Shift Button**: Context-aware functionality with visual feedback and state persistence
- **Extended Symbol Support**: Added missing symbols `{}`, `<>/`, `\` to symbol input mode
- **Improved Mode Button**: Now shows next mode instead of current mode for better UX
- **Smart Flick Input**: Direct character input for single-character buttons (no popup needed)
- **Button Label Optimization**: Alphabet and symbol modes show all available characters on button labels
- **Enter Button Enhancement**: Clears text area after displaying input result
- **Number Mode Optimization**: Shift button automatically disabled in number mode
- **Position Improvements**: Swapped Enter and Space button positions for better usability

### 🔧 Technical Improvements
- **Modular Architecture**: Clean separation of LVGL initialization, GUI application, and input logic
- **Comprehensive Testing**: 268 test cases covering all functionality
- **Memory Management**: Proper cleanup and resource management
- **UTF-8 Support**: Full Unicode character handling
- **Cross-platform Compatibility**: Works on Linux with SDL2 and LVGL

## Future Enhancements

### 🎯 Planned Features
- [ ] Voice input support
- [ ] Additional input methods (Romaji, etc.)
- [ ] Customizable keypad layouts
- [ ] Text prediction and auto-completion
- [ ] Multi-language support
- [ ] Theme customization
- [ ] Keyboard shortcuts
- [ ] Text export/import functionality

### 🔧 Technical Improvements
- [ ] Performance optimization
- [ ] Memory usage reduction
- [ ] Enhanced error handling
- [ ] Configuration file support
- [ ] Plugin architecture
- [ ] Cross-platform improvements