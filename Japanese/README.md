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
- **Hiragana Mode**: ひらがな (あいうえお, かきくけこ, etc.)
- **Katakana Mode**: カタカナ (アイウエオ, カキクケコ, etc.)
- **Alphabet Mode**: English letters (abc, def, ghi, etc.)
- **Number Mode**: Digits (1, 2, 3, etc.)
- **Symbol Mode**: Japanese punctuation and symbols (！？、。, etc.)

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
- **ひらがな**: Japanese hiragana characters
- **カタカナ**: Japanese katakana characters  
- **ABC**: English alphabet
- **123**: Numbers and digits
- **記号**: Japanese symbols and punctuation

### 🔄 Flick Input Method
1. Click any keypad button
2. A popup window appears with all available characters for that button
3. Click on the desired character to input it
4. The popup automatically closes after selection

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
- **Core Logic Tests**: 63 test cases
- **GUI Function Tests**: 264 test cases
- **Total Coverage**: 327 test cases
- **Success Rate**: 100% (all tests pass)

### 🔍 Test Categories
- Initialization and state management
- Mode switching functionality
- Character mapping verification
- Flick input processing
- UTF-8 conversion
- Edge cases and error handling
- GUI consistency checks

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
- **Styling**: Adjust LVGL styles in `gui_app.c`
- **Display Settings**: Modify window size and properties in `lvgl_init.c`
- **Fonts**: Replace `assets/NotoSansCJK.ttc` with preferred Japanese font

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

### Getting Help
- Check the test output for functionality verification
- Review the code comments for implementation details
- Ensure all dependencies are properly installed
- Verify system locale settings for Japanese text

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