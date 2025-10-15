# Korean QWERTY Virtual Keyboard

A full-featured Korean QWERTY virtual keyboard application built with MiniGUI framework, supporting both English and Korean input with proper character composition.

## Features

### Core Functionality
- **Full QWERTY Layout**: Complete 104-key keyboard with all standard keys
- **Korean Input Support**: Native Korean character composition with compound vowels
- **Dual Language Mode**: Seamless switching between English and Korean input
- **Visual Feedback**: Clear shift state indication and language mode display
- **Text Display**: Multi-line text area with word wrapping and scroll support

### Korean Character Support
- **Compound Vowels**: Proper composition of complex vowels (ㅜ+ㅓ=ㅝ, ㅗ+ㅏ=ㅘ, ㅗ+ㅣ=ㅚ)
- **Complete Syllables**: Full Korean syllable formation (초성+중성+종성)
- **Advanced Combinations**: Complex character sequences like ㄱ+ㅜ+ㅓ+ㄴ → 권
- **Font Optimization**: Uses NanumGothic fonts with proper Korean character rendering

### User Interface
- **Dual Shift Buttons**: Both left and right shift keys for enhanced usability
- **Centered Layout**: Mathematically centered button positioning for optimal ergonomics
- **Real-time Updates**: Immediate visual feedback for all key presses
- **Language Indicator**: Clear display of current input mode (English/Korean)

## Technical Specifications

### System Requirements
- **OS**: Linux (Ubuntu 20.04+ recommended)
- **GUI Framework**: MiniGUI 5.0+ with standalone mode
- **Display**: X11 with Xvfb support for virtual framebuffer
- **Dependencies**: FreeType2, HarfBuzz, libpng, libjpeg

### Build Dependencies
```bash
# Ubuntu/Debian
sudo apt-get install build-essential git gcc g++ make cmake pkg-config \
    libjpeg-dev libpng-dev libfreetype6-dev libinput-dev libdrm-dev \
    libx11-dev libxext-dev libxrender-dev libxrandr-dev libxinerama-dev \
    libxi-dev libxcursor-dev libxfixes-dev libharfbuzz-dev libpixman-1-dev \
    libwebp-dev libudev-dev libpciaccess-dev xvfb

# Red Hat/CentOS/Fedora
sudo dnf groupinstall "Development Tools"
sudo dnf install git gcc gcc-c++ cmake pkgconfig gtk2-devel \
    libjpeg-turbo-devel libpng-devel freetype-devel libinput-devel \
    libdrm-devel libX11-devel libXext-devel libXrender-devel \
    harfbuzz-devel pixman-devel libwebp-devel systemd-devel \
    libpciaccess-devel xorg-x11-server-Xvfb
```

## Installation & Usage

### Quick Start
```bash
# Clone or download the project
cd /path/to/korean-qwerty

# Build MiniGUI and the application (first time only)
./build.sh

# Run the application
./run.sh
```

### Development Workflow
```bash
# For development (rebuild just the application)
make clean && make

# Run the application
./run.sh

# Check MiniGUI installation
make check-minigui
```

### Build System Overview
- **`build.sh`**: Downloads, compiles, and installs MiniGUI locally + builds application
- **`run.sh`**: Sets up environment and runs the Korean QWERTY application  
- **`Makefile`**: Handles application compilation and Korean font installation

## Project Structure

```
.
├── README.md                 # This documentation file
├── main.c                    # Main application and UI logic
├── qwerty.c                  # Korean character composition engine
├── qwerty.h                  # Header definitions and structures
├── MiniGUI.cfg              # MiniGUI configuration file
├── build.sh                 # MiniGUI setup and build script
├── run.sh                   # Application runner script
├── Makefile                 # Build configuration
├── assets/                  # Korean font files
│   ├── NanumGothic-Bold.ttf
│   └── NanumGothic-Regular.ttf
├── install/                 # Local MiniGUI installation (auto-created)
└── minigui/                 # MiniGUI source code (auto-downloaded)
```

## Usage Instructions

### Starting the Application
1. Run `./build.sh` for first-time setup
2. Execute `./run.sh` to start the virtual keyboard
3. The application window will display the full QWERTY layout

### Input Methods
- **Click Keys**: Click any key button to input characters
- **Shift Keys**: Use either left or right shift for uppercase/symbols
- **Language Toggle**: Korean mode is activated by default with shift
- **Text Area**: View composed text in the multi-line text box at the top

### Korean Input
1. **Enable Korean Mode**: Press shift to activate Korean input
2. **Compose Characters**: Type Korean letters to form syllables
3. **Compound Vowels**: Combine vowels (ㅜ+ㅓ) to create compound forms (ㅝ)
4. **Complete Words**: Form complete Korean words with proper syllable composition

## Configuration

### MiniGUI Configuration (`MiniGUI.cfg`)
```ini
[system]
# PC Virtual FrameBuffer Graphics Abstract Layer
gal_engine=pc_xvfb
defaultmode=800x600-16bpp

# Input Abstract Layer  
ial_engine=pc_xvfb

[pc_xvfb]
defaultmode=800x600-16bpp

[systemfont]
font_number=4
font0=ttf-NanumGothic-Bold-rrncnn-0-0-UTF-8
font1=ttf-NanumGothic-Regular-rrncnn-0-0-UTF-8
font2=*-fixed-rrncnn-*-16-UTF-8
font3=*-Terminal-rrncnn-*-8-UTF-8
default=0
wchar_def=1
fixed=2
caption=0
menu=0
control=0
```

### Font Management
- **Primary**: NanumGothic-Bold (preferred for better character coverage)
- **Secondary**: NanumGothic-Regular (fallback option)
- **System Fallback**: Fixed-width system fonts for basic display
- **Automatic**: Font selection optimized for Korean character rendering

## Troubleshooting

### Common Issues

#### Application Won't Start
```bash
# Check if MiniGUI is built
make check-minigui

# Rebuild if necessary
./build.sh

# Verify executable exists
ls -la korean_input
```

#### Font Display Problems
```bash
# Check if fonts are installed
ls -la ./install/share/fonts/

# Reinstall fonts
make copy-fonts

# Verify font configuration in MiniGUI.cfg
```

#### Build Errors
```bash
# Install missing dependencies
sudo apt-get install build-essential cmake pkg-config

# Clean and rebuild
make clean && ./build.sh
```

### Known Problems

#### Character Display Issues
- **Backtick Character**: Due to font limitations, backtick (`) is displayed as apostrophe (') 
- **Text Wrapping**: Very long continuous text without spaces may not wrap properly in the text display area
- **Font Warnings**: Cosmetic charset warnings may appear but don't affect functionality

#### Memory Management
- **Minor Memory Leaks**: Minimal MiniGUI internal memory blocks may remain (2 blocks typical)
- **Font Cache**: FreeType2 cache optimization may show harmless warnings
- **PNG Warnings**: libpng sRGB profile warnings are cosmetic and don't affect operation

### Performance Notes
- **Startup Time**: Initial launch may take 2-3 seconds for font loading
- **Memory Usage**: Typical usage ~15-20MB RAM
- **Font Rendering**: Korean character composition optimized for speed

## Development

### Code Architecture
- **main.c**: UI framework, window management, event handling, font loading
- **qwerty.c**: Korean composition logic, character state management
- **qwerty.h**: Data structures, constants, function prototypes

### Key Components
```c
// Main structures
typedef struct {
    wchar_t consonant;      // Current consonant (초성)
    wchar_t vowel;          // Current vowel (중성)  
    wchar_t final_consonant; // Final consonant (종성)
    InputMode mode;         // English/Korean mode
    BOOL shift_pressed;     // Shift state
} QwertyState;

// Core functions
void process_korean_key(const char* key_text);
wchar_t combine_vowels(wchar_t base, wchar_t add);
wchar_t form_korean_syllable(wchar_t cho, wchar_t jung, wchar_t jong);
```

### Adding Features
1. **New Keys**: Add entries to button arrays in main.c
2. **Korean Logic**: Extend composition rules in qwerty.c  
3. **UI Elements**: Modify window creation in KoreanInputWinProc
4. **Configuration**: Update MiniGUI.cfg for system changes

## Testing

### Functional Testing
```bash
# Build and run
./build.sh && ./run.sh

# Test basic functionality
1. Click English letters → verify display
2. Press shift → verify Korean mode activation  
3. Type Korean sequences → verify proper composition
4. Test compound vowels → verify ㅜ+ㅓ=ㅝ combinations
```

### Composition Testing
- **Simple**: ㄱ + ㅏ → 가
- **Complex**: ㄱ + ㅜ + ㅓ + ㄴ → 권  
- **Compound**: ㅗ + ㅏ → ㅘ, ㅜ + ㅓ → ㅝ
- **Extended**: Full words with multiple syllables

## License

This project uses MiniGUI framework which follows its own licensing terms. Please refer to the MiniGUI license for usage restrictions and requirements.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly with Korean input
5. Submit a pull request with detailed description

## Support

For issues related to:
- **Korean Composition**: Check qwerty.c logic and character mappings
- **UI Problems**: Review main.c window management and event handling  
- **Build Issues**: Verify dependencies and MiniGUI installation
- **Font Problems**: Check assets/ directory and MiniGUI.cfg configuration

---

**Version**: 1.0  
**Last Updated**: October 2025  
**Compatible**: MiniGUI 5.0+, Linux/X11 systems