# ChunJiIn Korean Input Method - MiniGUI Version

A comprehensive Korean input method implementation using the ChunJiIn (천지인) system, built with MiniGUI framework for embedded and desktop environments.

## 🌟 Features

### Korean Input System (ChunJiIn)
- **Three fundamental elements**: ㆍ (천/Heaven), ㅡ (지/Earth), ㅣ (인/Human)
- **Complete Hangul composition**: Supports all Korean consonants and vowels
- **Intelligent syllable formation**: Automatic composition of 초성 (initial), 중성 (medial), and 종성 (final) consonants
- **Complex consonants**: Full support for 겹받침 (double final consonants)
- **Real-time composition**: Live preview of syllable formation

### Multi-Language Support
- **Korean (한글)**: Full ChunJiIn input method
- **English**: Upper and lowercase input modes
- **Numbers**: Numeric input mode
- **Special characters**: Special symbol input mode

### User Interface
- **Graphical interface**: Built with MiniGUI for cross-platform compatibility
- **Virtual keyboard**: 12-key layout similar to mobile phone keypads
- **Text display area**: Real-time text composition and editing
- **Mode switching**: Easy switching between input modes
- **Korean font support**: Uses NanumGothic fonts for proper Korean rendering

## 🏗️ Architecture

### Core Components

- **`main.c`**: Main application window and GUI components
- **`chunjiin.c`**: Core ChunJiIn input method logic
- **`chunjiin_hangul.c`**: Hangul character composition and Unicode handling
- **`chunjiin.h`**: Header file with data structures and function declarations

### Key Data Structures

```c
typedef struct {
    HangulState hangul;       // Korean input state
    InputMode now_mode;       // Current input mode
    wchar_t text_buffer[MAX_TEXT_LEN];  // Text buffer
    int cursor_pos;           // Cursor position
} ChunjiinState;

typedef struct {
    wchar_t chosung[16];      // Initial consonants
    wchar_t jungsung[16];     // Medial vowels
    wchar_t jongsung[16];     // Final consonants
    wchar_t jongsung2[16];    // Double final consonants
    int step;                 // Current composition step
    // ... composition flags
} HangulState;
```

## 🛠️ Building and Installation

### Prerequisites

The build script will automatically install required packages on supported Linux distributions:

**Ubuntu/Debian:**
```bash
sudo apt-get install -y git gcc g++ binutils autoconf automake libtool make cmake pkg-config \
    libgtk2.0-dev libjpeg-dev libpng-dev libfreetype6-dev libinput-dev libdrm-dev \
    libsqlite3-dev libxml2-dev libssl-dev libx11-dev libxext-dev libxrender-dev \
    libxrandr-dev libxinerama-dev libxi-dev libxcursor-dev libxfixes-dev \
    libharfbuzz-dev libpixman-1-dev libwebp-dev libudev-dev libpciaccess-dev xvfb
```

**CentOS/RHEL/Fedora:**
```bash
sudo yum groupinstall -y "Development Tools"
sudo yum install -y [corresponding packages...]
```

### Quick Start

1. **Clone and navigate to the project:**
   ```bash
   git clone <your-repo-url>
   cd Chunjiin
   ```

2. **Build everything (first time):**
   ```bash
   ./build.sh
   ```
   This script will:
   - Download and build MiniGUI locally
   - Copy Korean fonts to the proper location
   - Build the ChunJiIn application

3. **Run the application:**
   ```bash
   ./run.sh
   ```

### Development Workflow

After initial setup, you can use standard make commands:

```bash
# Rebuild the application only
make

# Clean build files
make clean

# Check MiniGUI installation
make check-minigui

# Show help
make help
```

## 🎯 Usage

### Input Modes

The application supports five input modes:

1. **한글 (Korean)**: ChunJiIn input method
2. **ABC (Uppercase English)**: Capital letters
3. **abc (Lowercase English)**: Lowercase letters  
4. **123 (Numbers)**: Numeric input
5. **!@# (Special)**: Special characters and symbols

### Key Layout

The virtual keyboard follows a standard 12-key mobile phone layout:

```
1      2      3
abc    def    ghi

4      5      6  
jkl    mno    pqr

7      8      9
stu    vwx    yz

*      0      #
       spc
```

### Korean Input (ChunJiIn System)

The ChunJiIn method uses three basic strokes:
- **ㆍ (dot)**: Represents 천 (Heaven)
- **ㅡ (horizontal line)**: Represents 지 (Earth)  
- **ㅣ (vertical line)**: Represents 인 (Human)

#### Basic Vowels:
- `1` (ㆍ): Creates ㅏ, ㅓ, ㅗ, ㅜ, ㅡ
- `*` (ㆍㆍ): Creates ㅑ, ㅕ, ㅛ, ㅠ
- `0` (ㅣ): Creates ㅣ, ㅓ, ㅏ

#### Consonants:
- `2`: ㄱ → ㅋ → ㄲ
- `3`: ㄴ → ㄹ
- `4`: ㄷ → ㅌ → ㄸ
- `5`: ㅁ → ㅂ → ㅍ → ㅃ
- `6`: ㅅ → ㅈ → ㅊ → ㅆ → ㅉ
- `7`: ㅇ → ㅎ
- And so on...

### Controls

- **Mode Button**: Switch between input modes
- **Clear Button**: Clear the text area
- **Enter Button**: Insert line break
- **Number Keys (0-9, *, #)**: Input characters based on current mode
- **Escape/Q**: Exit the application

## 🔧 Configuration

### Font Configuration

Korean fonts are automatically copied from the `assets/` directory:
- `NanumGothic-Regular.ttf`
- `NanumGothic-Bold.ttf`
- `NanumGothic-ExtraBold.ttf`

### MiniGUI Configuration

The application uses `MiniGUI.cfg` for runtime configuration:
- Standalone mode (no window manager required)
- PC X Virtual FrameBuffer (XVFB) graphics engine
- Default resolution: 800x600-16bpp
- Unicode support enabled

## 🚀 Technical Details

### MiniGUI Integration

- **Graphics Engine**: PC XVFB for virtual display
- **Input Engine**: PC XVFB for keyboard/mouse simulation
- **Font System**: TrueType font support with Unicode
- **Memory Management**: Local installation prevents system conflicts

### Unicode Support

- Full Unicode support for Korean text (UTF-8/UTF-16)
- Proper Hangul syllable composition (U+AC00-U+D7AF)
- Support for all Korean characters including rare combinations

### Cross-Platform Compatibility

- **Linux**: Primary target (Ubuntu, CentOS, Fedora)
- **Embedded Systems**: ARM/MIPS/x86 architectures
- **Virtual Environments**: Works with XVFB for headless systems

## 📁 Project Structure

```
Chunjiin/
├── README.md              # This file
├── build.sh              # Build script (downloads MiniGUI, builds app)
├── run.sh                # Run script (sets environment, runs app)
├── Makefile              # Build configuration
├── MiniGUI.cfg           # MiniGUI runtime configuration
├── main.c                # Main application and GUI
├── chunjiin.c            # Core input method logic
├── chunjiin_hangul.c     # Hangul composition functions
├── chunjiin.h            # Header file
└── assets/               # Korean fonts
    ├── NanumGothic-Regular.ttf
    ├── NanumGothic-Bold.ttf
    └── NanumGothic-ExtraBold.ttf
```

## 🐛 Troubleshooting

### Common Issues

1. **MiniGUI build fails**:
   - Ensure all dependencies are installed
   - Check internet connection for git clone
   - Verify disk space (MiniGUI build requires ~500MB)

2. **Application doesn't start**:
   - Run `./build.sh` first to ensure MiniGUI is built
   - Check `./install/lib/libminigui_sa.so` exists
   - Verify XVFB is installed and working

3. **Korean text not displaying**:
   - Ensure Korean fonts are in `./install/share/fonts/`
   - Check `assets/` directory contains `.ttf` files
   - Verify font permissions are readable

4. **Input not working**:
   - Check MiniGUI configuration in `MiniGUI.cfg`
   - Verify environment variables are set (run via `./run.sh`)
   - Test with different input modes

### Debug Mode

To run with verbose output:
```bash
export MG_DEBUG=1
./run.sh
```

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## 📜 License

This project is licensed under the MIT License - see the LICENSE file for details.

## 🙏 Acknowledgments

- **MiniGUI Team**: For the excellent GUI framework
- **ChunJiIn System**: Traditional Korean input method
- **NanumFont Project**: Korean font family
- **Unicode Consortium**: For Korean character standardization

## 📞 Support

For questions, issues, or contributions:
- Open an issue on GitHub
- Check the troubleshooting section above
- Review MiniGUI documentation for framework-specific issues

---

*Built with ❤️ for preserving and modernizing Korean input methods*