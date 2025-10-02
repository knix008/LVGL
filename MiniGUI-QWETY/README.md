# MiniGUI Korean QWERTY Input Application

This project demonstrates a Korean input system using MiniGUI with a QWERTY keyboard layout. The application provides a virtual keyboard for Korean character input with real-time Hangul composition.

## Overview

MiniGUI is a lightweight GUI library for embedded systems and Linux desktop applications. This project includes:
- Korean QWERTY input system with Hangul composition (`main.c`, `qwerty_korean.c`)
- Build configuration (`Makefile`)
- Build script for local MiniGUI installation (`build.sh`)
- Run script (`run.sh`)
- MiniGUI configuration (`MiniGUI.cfg`)
- Korean font support (NanumGothic TrueType fonts)

## Features

The application provides:
- **Virtual QWERTY Keyboard**: On-screen keyboard with Korean character labels
- **Real-time Hangul Composition**: Automatic composition of Korean characters from jamo (초성, 중성, 종성)
- **Shift Key Support**: Toggle for double consonants (ㅃ, ㅉ, ㄸ, ㄲ, ㅆ) and combined vowels (ㅒ, ㅖ)
- **Korean Font Display**: Full Unicode Korean character support using NanumGothic fonts
- **Interactive Input**: Click buttons or use keyboard for input
- **Backspace and Enter**: Standard text editing controls

### Korean Character Layout

The virtual keyboard displays Korean jamo on QWERTY keys:
- **First Row**: ㅂ ㅈ ㄷ ㄱ ㅅ ㅛ ㅕ ㅑ ㅐ ㅔ
- **Second Row**: ㅁ ㄴ ㅇ ㄹ ㅎ ㅗ ㅓ ㅏ ㅣ
- **Third Row**: ㅋ ㅌ ㅊ ㅍ ㅠ ㅜ ㅡ
- **Special Keys**: Shift, Space, Back, Enter

## Prerequisites

- Linux system (tested on Ubuntu/Debian)
- GCC compiler
- Make build tool
- X11 development libraries (for pc_xvfb engine)
- FreeType2 and HarfBuzz for font rendering

## Installation

### 1. Build MiniGUI

The project includes a local MiniGUI build script that downloads, configures, and builds MiniGUI in the local `minigui/` directory.

```bash
# Make the build script executable
chmod +x build.sh

# Run the build (this downloads and builds MiniGUI locally)
./build.sh
```

This will:
- Download MiniGUI source code from GitHub
- Configure and compile MiniGUI
- Install it locally in `./install/`
- Copy Korean fonts to `./install/share/fonts/`

### 2. Build the Application

```bash
# Build the Korean input application
make
```

This will create the `korean_input` executable.

## Running the Application

### Using the Run Script (Recommended)

```bash
# Make the run script executable
chmod +x run.sh

# Run the application
./run.sh
```

The run script automatically:
- Sets up the library path for local MiniGUI
- Runs the Korean input application

### Manual Execution

If you prefer to run manually:

```bash
# Set environment variable
export LD_LIBRARY_PATH="./install/lib:$LD_LIBRARY_PATH"

# Run the application
./korean_input
```

## Configuration

### MiniGUI Configuration

The `MiniGUI.cfg` file contains MiniGUI configuration settings:

- **Graphics Engine**: pc_xvfb (PC Virtual FrameBuffer)
- **Input Engine**: pc_xvfb
- **Display Mode**: 800x600-16bpp
- **Runtime Mode**: standalone
- **Korean Fonts**: TrueType fonts loaded from `./install/share/fonts/`

### Font Configuration

Korean fonts are configured in the `[truetypefonts]` section:
```
[truetypefonts]
font_number=3
name0=ttf-NanumGothic-rrncnn-0-0-UTF-8
fontfile0=./install/share/fonts/NanumGothic-Regular.ttf
name1=ttf-NanumGothic-bold-rrncnn-0-0-UTF-8
fontfile1=./install/share/fonts/NanumGothic-Bold.ttf
name2=ttf-NanumGothic-extrabold-rrncnn-0-0-UTF-8
fontfile2=./install/share/fonts/NanumGothic-ExtraBold.ttf
```

## File Structure

```
MiniGUI-QWETY/
├── main.c                    # Main application and UI
├── qwerty_korean.c           # Korean input engine
├── qwerty_korean.h           # Korean input header
├── Makefile                  # Build configuration
├── build.sh                  # MiniGUI build script
├── run.sh                    # Application run script
├── MiniGUI.cfg               # MiniGUI configuration
├── assets/                   # Korean fonts (NanumGothic)
│   ├── NanumGothic-Regular.ttf
│   ├── NanumGothic-Bold.ttf
│   └── NanumGothic-ExtraBold.ttf
├── minigui/                  # MiniGUI source (created by build.sh)
├── install/                  # Local MiniGUI installation
│   ├── lib/                  # MiniGUI libraries
│   ├── include/              # MiniGUI headers
│   └── share/fonts/          # Korean fonts (copied from assets)
├── korean_input              # Compiled executable
└── README.md                 # This file
```

## Application Code

### Main Application (`main.c`)

Demonstrates:
- Korean font loading with CreateLogFont
- Virtual keyboard UI with 30 buttons
- Text box for Korean output display
- Event handling for button clicks and keyboard input
- Font application to all UI elements

### Korean Input Engine (`qwerty_korean.c`)

Implements:
- QWERTY to Korean jamo mapping
- Real-time Hangul syllable composition
- Unicode handling (UTF-8 ↔ wchar_t conversion)
- Backspace, space, and enter key handling
- Support for compound consonants and vowels

### Key Functions:

- `qwerty_korean_init()`: Initializes the Korean input system
- `qwerty_process_input()`: Processes each input character
- `qwerty_compose_korean_characters()`: Composes Korean syllables from jamo
- `unicode_to_utf8()`: Converts Unicode to UTF-8 for display
- `update_textbox()`: Updates the text display with Korean output
- `update_button_labels()`: Updates button labels based on shift state

## Usage

1. **Click Buttons**: Click Korean character buttons to input jamo
2. **Use Keyboard**: Type on your physical keyboard (Q→ㅂ, W→ㅈ, etc.)
3. **Shift Key**: Toggle shift for double consonants (ㅃ, ㅉ, ㄸ, ㄲ, ㅆ)
4. **Backspace**: Remove the last character
5. **Space**: Add a space
6. **Enter**: Start a new line
7. **ESC**: Exit the application

## Troubleshooting

### Common Issues:

1. **Korean fonts not displaying**:
   - Ensure fonts are copied to `./install/share/fonts/`
   - Run `make` to automatically copy fonts
   - Check that font files exist in `assets/` directory

2. **Library not found errors**:
   - Run `./build.sh` to build MiniGUI locally
   - Ensure `LD_LIBRARY_PATH` includes `./install/lib`

3. **Graphics engine errors**:
   - The pc_xvfb engine requires X11
   - Make sure gvfb is installed: `/usr/local/bin/gvfb`

4. **Build errors**:
   - Install development packages:
     ```bash
     sudo apt-get install gcc make pkg-config libfreetype6-dev \
          libharfbuzz-dev libdrm-dev libinput-dev libjpeg-dev \
          libpng-dev libgtk2.0-dev
     ```

## Korean Input Algorithm

The application uses a stateful Korean composition algorithm:

1. **Consonant Input** (초성): Detects choseong from QWERTY keys
2. **Vowel Input** (중성): Combines with choseong to form syllable base
3. **Final Consonant** (종성): Adds jongseong to complete syllable
4. **Compound Jamo**: Supports double consonants and compound vowels
5. **Unicode Composition**: Converts jamo to complete Hangul syllable (U+AC00 - U+D7A3)

## Development

To modify the application:

1. Edit `main.c` for UI changes
2. Edit `qwerty_korean.c` for input logic changes
3. Rebuild with `make clean && make`
4. Test with `./run.sh`

## MiniGUI Resources

- [MiniGUI Official Website](http://www.minigui.com/)
- [MiniGUI GitHub](https://github.com/VincentWei/minigui)
- [MiniGUI Documentation](http://www.minigui.com/doc/)

## License

This project is for educational purposes. MiniGUI and NanumGothic fonts have their own licensing terms - please refer to their respective documentation for details.

## System Information

- **MiniGUI Version**: 5.0.x
- **Font Support**: TrueType (FreeType2)
- **Character Encoding**: UTF-8
- **Korean Font**: NanumGothic
- **Architecture**: x86_64
- **OS**: Linux
