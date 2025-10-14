# Chunjiin Korean Input Method - LVGL

A modern Korean input method application using the Chunjiin (천지인) input system, built with LVGL and SDL2.

## Overview

This application provides a graphical Korean input method using the Chunjiin system, which allows typing Korean characters using a 3x4 numeric keypad layout. The application has been built with LVGL (Light and Versatile Graphics Library) to provide:
- Excellent embedded system support
- Real-time Korean character composition
- Display of incomplete Hangul characters (partial jamos) as you type
- Beautiful Korean font rendering using FreeType with NanumGothic fonts
- SDL2-based desktop simulation
- Multiple input modes: 한글 (Hangul), 영문 (English), 숫자 (Numbers), 특수문자 (Special characters)

## Prerequisites

### System Requirements

1. **SDL2 Development Libraries**
   ```bash
   sudo apt-get update
   sudo apt-get install libsdl2-dev
   ```

2. **FreeType Development Libraries** (for TrueType font rendering)
   ```bash
   sudo apt-get install libfreetype6-dev
   ```

3. **Build Tools**
   ```bash
   sudo apt-get install build-essential git
   ```

4. **NanumGothic Font Files**
   - The required fonts are already included in the `assets/` directory
   - No additional font conversion tools are needed!

## Quick Start

### Option 1: Automated Setup (Recommended)

Run the provided setup script:

```bash
./setup.sh
```

This will automatically:
- Check for required system packages
- Install missing dependencies (with your permission)
- Clone LVGL v9.2
- Build the application
- Optionally run it

### Option 2: Manual Setup

1. **Clone LVGL**
   ```bash
   git clone --depth 1 --branch release/v9.2 https://github.com/lvgl/lvgl.git
   ```

2. **Build the Application**
   ```bash
   make
   ```

3. **Run the Application**
   ```bash
   ./chunjiin
   ```

## Configuration

The `lv_conf.h` file is pre-configured with:
- ✓ SDL2 driver enabled for desktop simulation
- ✓ FreeType library enabled for TrueType font rendering
- ✓ Memory settings optimized for Korean fonts (256KB heap)
- ✓ All necessary widgets enabled (buttons, text area, message box, etc.)

## Font Rendering

The application uses **LVGL's FreeType integration** to render Korean fonts directly from TrueType files:

- **Font Files:** NanumGothic-Regular.ttf (included in `assets/`)
- **Font Sizes:** 12px, 14px, 16px, 20px (loaded at runtime)
- **Character Support:**
  - Complete Hangul Syllables (AC00-D7AF)
  - Hangul Compatibility Jamo (3130-318F) for displaying incomplete characters
  - ASCII characters (0020-007F)
  - Korean punctuation and symbols

No pre-conversion needed - fonts are loaded directly at runtime!

## Project Structure

```
Chunjiin/
├── main.c                    # LVGL GUI with SDL2
├── chunjiin.c               # Core Chunjiin input logic
├── chunjiin_hangul.c        # Hangul character composition
├── chunjiin.h               # Header file with type definitions
├── lv_conf.h                # LVGL configuration (FreeType enabled)
├── Makefile                 # Build system with LVGL integration
├── setup.sh                 # Automated setup script
├── README.md                # This file
├── assets/                  # TrueType font files
│   ├── NanumGothic-Regular.ttf
│   ├── NanumGothic-Bold.ttf
│   └── NanumGothic-ExtraBold.ttf
└── lvgl/                    # LVGL library (cloned during setup)
```

## How It Works

### Chunjiin Input Method

The Chunjiin (천지인, "Sky-Earth-Human") system represents Korean vowels using three basic strokes:
- **천 (ㅣ)**: Vertical line (Heaven)
- **지 (·)**: Dot (Earth) 
- **인 (ㅡ)**: Horizontal line (Human)

These combine to form all Korean vowels. Consonants are arranged on number keys 0, 4-9.

### Real-time Character Display

The application displays **incomplete Hangul characters** as you type:
- Single consonant: `ㄱ` (compatibility jamo)
- Consonant + vowel: `가` (combined syllable)
- Adding final consonant: `각` (complete syllable)

This is achieved by using **Hangul Compatibility Jamo** (U+3130-U+318F) for standalone display, and **Composed Hangul Syllables** (U+AC00-U+D7AF) for complete characters.

## Button Layout

The application maintains the same 3x5 grid layout as the GTK version:

```
┌─────────┬─────────┬─────────┐
│  천(1)   │  지(2)   │  인(3)   │  Row 0
├─────────┼─────────┼─────────┤
│  ㄱ(4)   │  ㄴ(5)   │  ㄷ(6)   │  Row 1
├─────────┼─────────┼─────────┤
│  ㅂ(7)   │  ㅅ(8)   │  ㅈ(9)   │  Row 2
├─────────┼─────────┼─────────┤
│ Space(10)│ ㅇㅁ(0)  │ Del(11)  │  Row 3
├─────────┼─────────┼─────────┤
│  모드     │  지우기   │  엔터     │  Row 4
└─────────┴─────────┴─────────┘
```

## Features

### Input Features
- ✓ **Chunjiin Korean input method** with real-time character composition
- ✓ **Display incomplete characters** (partial jamos) as you type
- ✓ **Multiple input modes:**
  - 한글 (Hangul) - Korean characters
  - 영문 (English) - Lowercase by default
  - 영문 대문자 (Uppercase English)
  - 숫자 (Numbers) - 0-9
  - 특수문자 (Special characters) - Symbols and punctuation
- ✓ **Smart character composition** with vowel and consonant combining
- ✓ **Delete/backspace** support for step-by-step character decomposition

### UI Features
- ✓ **Beautiful Korean font rendering** using NanumGothic TrueType fonts
- ✓ **Scrollable text area** for long text input
- ✓ **Result popup dialog** when pressing Enter
- ✓ **Mode switching button** to cycle through input modes
- ✓ **Clear button** to reset input
- ✓ **320×640 portrait display** optimized for mobile-style layouts

## Troubleshooting

### Build Errors

1. **Missing LVGL directory:**
   ```bash
   git clone --depth 1 --branch release/v9.2 https://github.com/lvgl/lvgl.git
   ```
   Or run:
   ```bash
   ./setup.sh
   ```

2. **SDL2 not found:**
   ```bash
   sudo apt-get install libsdl2-dev
   pkg-config --cflags --libs sdl2
   ```

3. **FreeType not found:**
   ```bash
   sudo apt-get install libfreetype6-dev
   pkg-config --cflags --libs freetype2
   ```

4. **Compilation errors:** 
   - Make sure `lv_conf.h` is in the project root
   - Verify LVGL was cloned to `lvgl/` directory
   - Check that all `.ttf` font files are in `assets/` directory

### Runtime Issues

1. **Application won't start / Segmentation fault:**
   - Check that font files exist in `assets/` directory
   - Verify FreeType libraries are installed: `ldconfig -p | grep freetype`

2. **Black screen or window doesn't appear:**
   - Check SDL2 installation: `pkg-config --modversion sdl2`
   - Try running with: `SDL_VIDEODRIVER=x11 ./chunjiin`

3. **Missing Korean characters (boxes or blank):**
   - Verify `assets/NanumGothic-Regular.ttf` exists
   - Check FreeType initialization in console output
   - The font includes ranges: U+0020-007F, U+3130-318F, U+AC00-D7AF

4. **Application crashes or memory errors:**
   - Increase `LV_MEM_SIZE` in `lv_conf.h` (currently 256KB)
   - Check available system memory

## Technical Details

### Architecture

| Component | Technology |
|-----------|------------|
| GUI Framework | LVGL 9.2 |
| Display Driver | SDL2 |
| Font Rendering | FreeType 2 (runtime TrueType loading) |
| Layout System | LVGL Grid Layout |
| Input Method | Chunjiin (천지인) |
| Character Encoding | UTF-8 / Wide characters (wchar_t) |

### Unicode Ranges Used

- **ASCII:** U+0020-007F (Basic Latin)
- **Hangul Compatibility Jamo:** U+3130-318F (for standalone display)
- **Hangul Syllables:** U+AC00-D7AF (11,172 precomposed syllables)
- **Chunjiin Dots:** U+00B7 (·), U+2025 (‥)

### Performance

- **Memory Usage:** ~256KB LVGL heap + ~5MB for fonts (loaded by FreeType)
- **Frame Rate:** 30+ FPS (SDL2 refresh rate)
- **Startup Time:** ~0.5 seconds (including font loading)
- **Binary Size:** ~600KB (with LVGL compiled in)

## Development

### Building from Source

```bash
# Clone the repository
git clone https://github.com/yourusername/Chunjiin.git
cd Chunjiin

# Run setup
./setup.sh

# Or manually:
git clone --depth 1 --branch release/v9.2 https://github.com/lvgl/lvgl.git
make
./chunjiin
```

### Code Structure

- `main.c` - LVGL GUI, event handlers, font initialization
- `chunjiin.c` - Core input processing, mode management
- `chunjiin_hangul.c` - Korean character composition algorithms
- `chunjiin.h` - Type definitions and function declarations

## References

- **LVGL Documentation:** https://docs.lvgl.io/
- **SDL2 Documentation:** https://wiki.libsdl.org/
- **FreeType Documentation:** https://freetype.org/freetype2/docs/
- **NanumGothic Font:** https://hangeul.naver.com/font
- **Chunjiin Input Method:** https://en.wikipedia.org/wiki/Cheonjiin

## License

MIT License

Copyright (c) 2025

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
