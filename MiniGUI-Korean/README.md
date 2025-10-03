# MiniGUI Korean Font Display Application

This project demonstrates a MiniGUI application for displaying Korean text using multiple font weights. It showcases Korean character rendering with NanumGothic fonts and provides interactive font switching capabilities.

## Overview

MiniGUI is a lightweight GUI library for embedded systems and Linux desktop applications. This Korean font display project includes:
- Korean text display application (`main.c`)
- Three NanumGothic font weights (Regular, Bold, ExtraBold)
- Build configuration (`Makefile`)
- Run script (`run.sh`)
- MiniGUI configuration (`MiniGUI.cfg`)
- Korean font assets in `assets/fonts/`

## Features

The application provides:
- **Korean Text Display**: Comprehensive Korean character rendering including:
  - Basic greetings and sentences
  - Korean alphabet (consonants and vowels)
  - Complex consonants and vowels
  - Numbers and special characters
  - Real Korean sentences and phrases
- **Multiple Font Weights**: Three NanumGothic font variants:
  - Regular (standard weight)
  - Bold (medium weight)
  - ExtraBold (heavy weight)
- **Interactive Controls**:
  - SPACE key: Cycle through Korean text samples
  - F key: Switch between font weights
  - ESC/Q keys: Exit application
- **Font Information Display**: Shows current font name and index
- **UTF-8 Korean Support**: Proper Korean locale and encoding support

## Korean Text Samples

The application includes diverse Korean text samples:
- **Basic**: "안녕하세요! Hello in Korean"
- **Descriptive**: "한국어 텍스트 표시 프로그램"
- **Technical**: "MiniGUI로 한국어를 표시합니다"
- **Alphabets**: "가나다라마바사아자차카타파하"
- **Consonants**: "ㄱㄴㄷㄹㅁㅂㅅㅇㅈㅊㅋㅌㅍㅎ"
- **Complex Characters**: "ㄲㄸㅃㅆㅉ"
- **Vowels**: "ㅐㅒㅔㅖㅘㅙㅚㅝㅞㅟㅢ"
- **Numbers**: "한글 숫자 테스트: 0123456789"
- **Special Characters**: "한글 특수문자 테스트: !@#$%^&*()"
- **Sentences**: "이것은 한국어 문장입니다."

## Prerequisites

- Linux system (tested on Ubuntu/Debian)
- GCC compiler
- Make build tool
- X11 development libraries (for pc_xvfb engine)
- Korean UTF-8 locale support

## Font Assets

The application uses NanumGothic Korean fonts located in `assets/fonts/`:
- **NanumGothic-Regular.ttf**: Standard weight Korean font
- **NanumGothic-Bold.ttf**: Bold weight Korean font  
- **NanumGothic-ExtraBold.ttf**: Extra bold weight Korean font

These fonts are automatically copied to `install/share/fonts/` during the build process for MiniGUI to access.

## Installation

### 1. Build MiniGUI

The project includes a local MiniGUI installation. MiniGUI is built and installed locally in the `install/` directory.

```bash
# Build the Korean display application
make
```

This will:
- Copy Korean fonts to the MiniGUI font directory
- Compile the Korean display application
- Create the `korean_display` executable

### 2. Build the Application

```bash
# Build the Korean display application
make
```

This will create the `korean_display` executable.

## Running the Application

### Using the Run Script (Recommended)

```bash
# Make the run script executable
chmod +x run.sh

# Run the Korean display application
./run.sh
```

The run script automatically:
- Sets up the library path
- Configures MiniGUI environment variables
- Uses the pc_xvfb graphics engine
- Runs the Korean display application

### Manual Execution

If you prefer to run manually:

```bash
# Set environment variables
export LD_LIBRARY_PATH="$(pwd)/install/lib:$LD_LIBRARY_PATH"
export MG_RUNTIME_MODE="standalone"
export MG_GAL_ENGINE="pc_xvfb"
export MG_IAL_ENGINE="pc_xvfb"

# Run the Korean display application
./korean_display
```

## Usage

Once the application is running, you can interact with it using the following controls:

- **SPACE**: Cycle through different Korean text samples
- **F**: Switch between font weights (Regular → Bold → Extra Bold → Regular)
- **ESC** or **Q**: Exit the application

The application displays various Korean text samples including:
- Common Korean phrases and greetings
- Korean proverbs and sayings
- Korean alphabet (Hangul) characters
- Mixed Korean-English text
- Technical terms in Korean

## Configuration

The `MiniGUI.cfg` file contains MiniGUI configuration settings:

- **Graphics Engine**: pc_xvfb (PC Virtual FrameBuffer)
- **Input Engine**: pc_xvfb
- **Display Mode**: 800x600-16bpp
- **Korean UTF-8 locale support**
- **TrueType font loading for NanumGothic fonts**
- **Font size set to 16pt for all font weights**
- **Font files loaded from `install/share/fonts/` directory**
- **Runtime Mode**: standalone

## File Structure

```
MiniGUI-Korean/
├── main.c              # Korean font display application source code
├── Makefile            # Build configuration
├── run.sh              # Run script with environment setup
├── build.sh            # Build script for MiniGUI and application
├── MiniGUI.cfg         # MiniGUI configuration with Korean font support
├── korean_display      # Compiled executable
├── install/            # Local MiniGUI installation directory
│   ├── lib/           # MiniGUI libraries
│   └── share/fonts/   # Korean font files location
├── assets/            # Application assets
│   └── fonts/         # Source Korean font files
│       ├── NanumGothic-Regular.ttf
│       ├── NanumGothic-Bold.ttf
│       └── NanumGothic-ExtraBold.ttf
└── README.md          # This file
```

## Application Features

The Korean display application (`main.c`) demonstrates:

- **Korean UTF-8 text rendering** with proper charset handling
- **Multiple TrueType font loading** (Regular, Bold, Extra Bold)
- **Interactive font switching** between different weights
- **Korean text sample cycling** with various content types
- **Keyboard event handling** for user interaction
- **Proper font resource management** and cleanup

### Key Functions:

- `on_button1_clicked()`: Shows a message box when "Click Me" is pressed
- `on_button2_clicked()`: Exits the application when "Exit" is pressed
- `main()`: Initializes MiniGUI, creates the window, and runs the event loop

- **load_korean_fonts()**: Loads three Korean font weights from TrueType files
- **MainWinProc()**: Main window procedure handling paint and keyboard events
- **korean_texts[]**: Array of Korean text samples for display
- **Font switching logic**: Cycles through font weights when 'F' key is pressed
- **Text cycling logic**: Displays different Korean samples when SPACE is pressed

## Troubleshooting

### Common Issues:

1. **Font loading errors**:
   - Ensure Korean font files are in `install/share/fonts/` directory
   - Check that font file names match exactly in `MiniGUI.cfg`
   - Verify Korean UTF-8 locale support is available on system

2. **Korean text display issues**:
   - Confirm UTF-8 encoding is properly configured
   - Check that LC_ALL or LANG environment variables support Korean
   - Verify Korean fonts contain the required character glyphs

3. **Library not found errors**:
   - Ensure the library path points to `install/lib` directory
   - Check that MiniGUI was built and installed properly in local directory

4. **Graphics engine errors**:
   - The pc_xvfb engine requires X11 development libraries
   - If X11 is not available, you can use the dummy engine for testing

5. **Build errors**:
   - Make sure all dependencies are installed (gcc, make, X11-dev)
   - Check that MiniGUI headers are accessible during compilation

### Alternative Graphics Engines:

If pc_xvfb doesn't work, you can try:

- **dummy**: For testing without display (modify `run.sh` and `MiniGUI.cfg`)
- **drm**: For direct framebuffer access (requires proper permissions)
- **fbcon**: For Linux framebuffer console (requires framebuffer device)

## Development

To modify the Korean display application:

1. **Add new Korean text samples**: Edit the `korean_texts[]` array in `main.c`
2. **Change font sizes**: Modify the font size parameter in `load_korean_fonts()`
3. **Add new fonts**: Copy font files to `assets/fonts/` and update `MiniGUI.cfg`
4. **Customize controls**: Modify keyboard event handling in `MainWinProc()`
5. **Rebuild**: Use `make` to compile changes
6. **Test**: Run with `./run.sh` to verify functionality

### Adding Custom Korean Fonts:

1. Place `.ttf` font files in `assets/fonts/` directory
2. Update the `[truetypefonts]` section in `MiniGUI.cfg`
3. Modify the font loading code in `load_korean_fonts()` function
4. Update the font switching logic if adding more than three fonts

## MiniGUI Resources

## References

- [MiniGUI Official Documentation](http://www.minigui.com/)
- [MiniGUI Programming Guide](http://www.minigui.com/doc/)
- [MiniGUI API Reference](http://www.minigui.com/doc/)
- [Korean UTF-8 Character Encoding](https://en.wikipedia.org/wiki/UTF-8)
- [NanumGothic Font Family](https://hangeul.naver.com/2017/nanum)
- [TrueType Font Loading in MiniGUI](http://www.minigui.com/doc/)

## License

This project is for educational purposes. MiniGUI has its own licensing terms - please refer to the MiniGUI documentation for details.

The NanumGothic fonts are distributed under the SIL Open Font License.

## System Information

- **MiniGUI Version**: 4.0
- **Target Platform**: Linux with X11 support
- **Font Support**: TrueType (TTF) Korean fonts
- **Character Encoding**: UTF-8
- **Locale Support**: Korean (ko_KR.UTF-8)