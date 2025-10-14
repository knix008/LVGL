# Chunjiin Korean Input Method - MiniGUI

A Korean input method application using the Chunjiin (천지인) input system, built with MiniGUI for embedded and desktop Linux systems.

## Overview

This application provides a graphical Korean input method using the Chunjiin system, which allows typing Korean characters using a 12-button keypad layout. The application is built with MiniGUI to provide:

- **Lightweight GUI framework** suitable for embedded systems and desktops
- **Real-time Korean character composition** using the Chunjiin input method
- **Multiline text editing** with automatic word wrapping
- **Korean font rendering** with TrueType fonts (NanumGothic)
- **Multiple display backends**: X11, DRM, and framebuffer support
- **Multiple input modes**: 한글 (Hangul), 영문 (English), 숫자 (Numbers), 특수문자 (Special)
- **Non-resizable, fixed-size window** (280x400 pixels)
- **No window minimize/maximize** for consistent UI

## Features

### Input Features
- ✅ **Chunjiin Korean input method** with real-time character composition
- ✅ **12-button keypad layout** with dynamically labeled buttons
- ✅ **Multiple input modes**:
  - 한글 (Hangul) - Korean characters using Chunjiin
  - 영문 (English) - Lowercase letters
  - 영문 대문자 (Uppercase) - Uppercase letters
  - 숫자 (Numbers) - Digits 0-9
  - 특수문자 (Special) - Symbols and punctuation
- ✅ **Smart character composition** with vowel and consonant combining
- ✅ **Button labels update** based on current input mode

### UI Features
- ✅ **Multiline edit control** with automatic text wrapping
- ✅ **Korean font rendering** using NanumGothic TrueType fonts
- ✅ **Fixed-size window** (280x400) - non-resizable, non-minimizable
- ✅ **Wide text display area** (260px width)
- ✅ **Real-time character updates** as you type
- ✅ **Mode, Clear, and Enter buttons** for input control
- ✅ **Message box display** for input result confirmation

## Quick Start

### Option 1: Automated Installation (Recommended)

```bash
# Install all dependencies and MiniGUI
./install_minigui.sh

# Build the application
./build.sh

# Run the application
./chunjiin
```

### Option 2: Using Makefile

```bash
# Install dependencies first
./install_minigui.sh

# Build with make
make

# Run the application
make run
```

## Installation

### System Requirements

The `install_minigui.sh` script will automatically install all required dependencies:

#### Automatically Installed Components:
1. **Build Tools**: gcc, make, cmake, autotools
2. **X11 Libraries**: libx11-dev, libxext-dev, libxft-dev
3. **Graphics Libraries**:
   - FreeType2 (font rendering)
   - libpng, libjpeg (image support)
   - libdrm (Direct Rendering Manager)
   - libpciaccess (PCI access)
4. **Input Libraries**: libinput-dev, libudev-dev
5. **Text Rendering**: HarfBuzz (complex scripts), ICU (internationalization)
6. **Korean Fonts**: Nanum fonts, Noto CJK fonts
7. **MiniGUI Library**: libMiniGUI 5.0.12 (compiled from source)
8. **mGUtils Library**: mGUtils 1.2.6 (additional UI controls)

### Supported Linux Distributions

- **Debian/Ubuntu** (using apt-get)
- **RHEL/CentOS/Fedora** (using yum)
- **Arch Linux** (using pacman)

## Configuration

### MiniGUI Configuration

The `MiniGUI.cfg` file is automatically installed to:
- User config: `~/.MiniGUI.cfg` (highest priority)
- System config: `/usr/local/etc/MiniGUI.cfg`

Key configuration settings:
```ini
[system]
gal_engine=pc_xvfb          # X11 backend for desktop
ial_engine=pc_xvfb          # X11 input handling
defaultmode=800x600-16bpp   # Display resolution

[drm]
# For embedded systems with direct hardware access
defaultmode=1024x768-32bpp
pixelformat=XR24
device=/dev/dri/card0
```

### Display Backends

- **X11 Backend** (default): `gal_engine=pc_xvfb` - For desktop development
- **DRM Backend**: `gal_engine=drm` - For embedded systems
- **Framebuffer Backend**: `gal_engine=fbcon` - For Linux framebuffer

## Application Window

The application creates a fixed-size window with the following characteristics:

- **Window Size**: 280×400 pixels (fixed, non-resizable)
- **Text Area**: 260×100 pixels with automatic word wrapping
- **Window Style**:
  - Non-resizable (no `WS_THICKFRAME`)
  - Non-minimizable (no `WS_MINIMIZEBOX`)
  - Non-maximizable (no `WS_MAXIMIZEBOX`)
  - Title bar and close button only

## Button Layout

The application uses a 12-button keypad with dynamic labels:

### Hangul Mode (한글):
```
┌─────────┬─────────┬─────────┐
│  천(1)  │  지(2)  │  인(3)  │  <- Vowel strokes
├─────────┼─────────┼─────────┤
│  ㄱ(4)  │  ㄴ(5)  │  ㄷ(6)  │
├─────────┼─────────┼─────────┤
│  ㅂ(7)  │  ㅅ(8)  │  ㅈ(9)  │
├─────────┼─────────┼─────────┤
│ 공백(10)│ ㅇㅁ(0) │ 삭제(11)│
└─────────┴─────────┴─────────┘

Control Buttons:
[  Mode  ] [ Clear  ] [ Enter  ]
```

### English Mode (영문):
```
┌─────────┬─────────┬─────────┐
│    ·    │  abc(2) │  def(3) │
├─────────┼─────────┼─────────┤
│  ghi(4) │  jkl(5) │  mno(6) │
├─────────┼─────────┼─────────┤
│ pqrs(7) │  tuv(8) │ wxyz(9) │
├─────────┼─────────┼─────────┤
│  SPC(10)│    ·    │  DEL(11)│
└─────────┴─────────┴─────────┘
```

### Number Mode (숫자):
```
┌─────────┬─────────┬─────────┐
│    1    │    2    │    3    │
├─────────┼─────────┼─────────┤
│    4    │    5    │    6    │
├─────────┼─────────┼─────────┤
│    7    │    8    │    9    │
├─────────┼─────────┼─────────┤
│  SPC(10)│    0    │  DEL(11)│
└─────────┴─────────┴─────────┘
```

## How It Works

### Chunjiin Input Method

The Chunjiin (천지인, "Heaven-Earth-Human") system represents Korean vowels using three basic strokes:
- **천 (ㅣ)**: Vertical line (Heaven)
- **지 (·)**: Dot (Earth)
- **인 (ㅡ)**: Horizontal line (Human)

These combine to form all Korean vowels:
- `ㅣ` + `·` = `ㅏ`
- `ㅡ` + `·` = `ㅗ`
- `ㅣ` + `·` + `·` = `ㅑ`

Consonants are arranged on buttons 0, 4-9 with multiple consonants per button.

### Multiline Text Control

The application uses `CTRL_MEDIT` (MiniGUI's multiline edit control) with:
- **`ES_READONLY`**: Read-only for display purposes
- **`ES_AUTOWRAP`**: Automatic word wrapping at control boundaries
- **260px width**: Fits the 280px window with 10px margins

Text automatically wraps when it reaches the edge of the text area.

## Project Structure

```
Chunjiin/
├── main.c                   # Main application with MiniGUI UI
├── chunjiin.c               # Core Chunjiin input logic
├── chunjiin_hangul.c        # Hangul character composition
├── chunjiin.h               # Header file with type definitions
├── MiniGUI.cfg              # MiniGUI runtime configuration
├── Makefile                 # Build system
├── install_minigui.sh       # Complete installation script
├── build.sh                 # Build script with multiple options
├── README.md                # This file
└── assets/                  # TrueType font files
    ├── NanumGothic-Regular.ttf
    ├── NanumGothic-Bold.ttf
    └── NanumGothic-ExtraBold.ttf
```

## Build System

### Using build.sh

The `build.sh` script provides multiple options:

```bash
# Clean build (release mode)
./build.sh --clean

# Debug build
./build.sh --debug

# Build and run
./build.sh --run

# Verbose output
./build.sh --verbose

# Show help
./build.sh --help
```

The script automatically:
- Generates `build/main_minigui.c` from embedded template
- Compiles all source files with proper flags
- Links against MiniGUI libraries
- Creates the `build/chunjiin` executable

### Using Makefile

```bash
# Build (default)
make

# Clean build
make clean && make

# Debug build
make debug

# Release build
make release

# Build and run
make run

# Show build info
make info

# Install system-wide
sudo make install

# Uninstall
sudo make uninstall
```

## Troubleshooting

### Installation Issues

1. **install_minigui.sh fails:**
   ```bash
   # Check system requirements
   df -h  # Need at least 500MB free space

   # Check package manager
   apt-get --version  # For Debian/Ubuntu
   yum --version      # For RHEL/CentOS
   ```

2. **MiniGUI compilation fails:**
   - Check if all dependencies are installed
   - Review `build.log` in the project directory
   - Ensure you have internet connection for downloads

### Build Issues

1. **Compilation errors:**
   ```bash
   # Verify MiniGUI installation
   ldconfig -p | grep minigui
   pkg-config --modversion minigui

   # Check header files
   ls /usr/local/include/minigui/
   ```

2. **Missing libraries:**
   ```bash
   # Re-run dependency installation
   ./install_minigui.sh

   # Update library cache
   sudo ldconfig
   ```

3. **Build script errors:**
   - Check syntax: `bash -n ./build.sh`
   - Use verbose mode: `./build.sh --verbose`
   - Check build log: `cat build.log`

### Runtime Issues

1. **Application won't start:**
   ```bash
   # Check X11 display
   echo $DISPLAY
   xdpyinfo  # Should show display info

   # Verify MiniGUI config
   cat ~/.MiniGUI.cfg

   # Check library loading
   ldd ./chunjiin
   ```

2. **Korean text not displaying:**
   - Verify font files: `ls assets/*.ttf`
   - Check font loading in output
   - Ensure FreeType support: `pkg-config --cflags freetype2`

3. **Window size issues:**
   - The window is intentionally fixed at 280x400 pixels
   - Check if window manager is overriding settings
   - Try different MiniGUI configuration

4. **Permission errors:**
   ```bash
   # For framebuffer access
   sudo chmod 666 /dev/fb0

   # For input devices
   sudo usermod -a -G input $USER
   newgrp input
   ```

## Technical Details

### Architecture

| Component | Technology |
|-----------|------------|
| GUI Framework | MiniGUI 5.0.12 |
| Display Driver | X11/DRM/Framebuffer |
| Font Rendering | FreeType 2 (via MiniGUI) |
| Text Control | CTRL_MEDIT (multiline edit) |
| Input Method | Chunjiin (천지인) |
| Character Encoding | UTF-8 / Wide characters (wchar_t) |
| Window Style | Fixed size, non-resizable |

### Unicode Ranges Used

- **ASCII**: U+0020-007F (Basic Latin)
- **Hangul Compatibility Jamo**: U+3130-318F (standalone consonants/vowels)
- **Hangul Syllables**: U+AC00-D7AF (11,172 precomposed syllables)
- **Chunjiin Markers**: U+00B7 (·), U+2025 (‥)

### Performance

- **Memory Usage**: 2-8MB RAM (depending on fonts)
- **CPU Requirements**: ARM Cortex-A series or x86/x64
- **Startup Time**: ~0.2 seconds
- **Binary Size**: ~200KB (excluding libraries)
- **Window Size**: 280×400 pixels (fixed)

## Development

### Code Organization

- **main.c**: MiniGUI UI, window creation, event handlers, font management
- **chunjiin.c**: Core input processing, mode switching, button text mapping
- **chunjiin_hangul.c**: Korean character composition algorithms
- **chunjiin.h**: Type definitions, function declarations, constants

### Key Functions

- `init_korean_font()`: Loads Korean TrueType fonts
- `update_display()`: Updates text area with current input
- `update_button_labels()`: Updates button labels based on mode
- `chunjiin_process_input()`: Processes keypad button presses
- `get_button_text()`: Returns appropriate label for button in current mode

### Build Flags

```bash
# Compiler flags (default)
CFLAGS = -Wall -Wextra -O2 -std=c99

# Debug flags
CFLAGS = -Wall -Wextra -g -O0 -DDEBUG -std=c99

# Libraries
LDFLAGS = -lminigui_ths -lpciaccess -ldrm -ljpeg -lpng
          -lz -linput -lharfbuzz -lfreetype -ludev
          -lm -lpthread
```

## Contributing

Contributions are welcome! Please ensure:
1. Code compiles without warnings
2. Follows existing code style
3. Tests on multiple platforms if possible
4. Updates documentation as needed

## References

- **MiniGUI Official Site**: http://www.minigui.org
- **MiniGUI Documentation**: http://www.minigui.org/documentation
- **MiniGUI GitHub**: https://github.com/VincentWei/minigui
- **FreeType Documentation**: https://freetype.org/freetype2/docs/
- **NanumGothic Font**: https://hangeul.naver.com/font
- **Chunjiin Input Method**: https://en.wikipedia.org/wiki/Cheonjiin
- **Korean Unicode**: https://unicode.org/charts/PDF/UAC00.pdf

## License

MIT License

Copyright (c) 2025

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
