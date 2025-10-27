# Chunjiin Korean Input Method - LVGL 8.4

A modern Korean input method application using the Chunjiin (천지인) input system, built with LVGL 8.4 and SDL2.

## Overview

This application provides a graphical Korean input method using the Chunjiin system, which allows typing Korean characters using a 3x4 numeric keypad layout. The application has been built with LVGL 8.4 (Light and Versatile Graphics Library) to provide:
- Excellent embedded system support with LVGL 8.4
- Real-time Korean character composition
- Display of incomplete Hangul characters (partial jamos) as you type
- Beautiful Korean font rendering using FreeType with NanumGothic fonts
- SDL2-based desktop simulation with hardware-accelerated rendering
- Multiple input modes: 한글 (Hangul), 영문 (English), 숫자 (Numbers), 특수문자 (Special characters)
- Modern UI with color-coded buttons and optimized typography

## Recent Enhancements (2025)

### LVGL 8.4 Migration
- ✅ **Framework Upgrade**: Migrated from LVGL 9.2 to LVGL 8.4
  - Full SDL2 integration with hardware-accelerated texture rendering
  - Proper display driver and input device initialization
  - Double-buffering for smooth rendering
  - Complete HAL (Hardware Abstraction Layer) implementation
- 🎬 **Real-time Display**: Direct pixel rendering to SDL2 window
  - Area-based updates for efficient rendering
  - lv_tick_inc() for proper timing synchronization

### Typography & Visual Design
- 📝 **Clean Font Styling**: Regular (non-bold) fonts for all UI elements
  - All button labels use regular 14px font
  - Text result area uses regular 16px font
  - Popup dialog titles and messages use regular fonts (20px and 16px)
  - Cleaner, more minimalist appearance
- 🎨 **Color-Coded Buttons**: Enhanced visual hierarchy
  - Orange mode button (0xFF8C00) for input mode switching
  - Green enter button (0x28A745) for primary action confirmation
  - Gray/Blue dual-button popup dialogs for Cancel/Confirm actions
- ⬅️ **Modern Symbols**: Left arrow symbol (←) for backspace

### Enhanced Functionality
- 🔘 **Dual-Button Popup Dialogs**:
  - Cancel button (취소) and confirm button (확인)
  - Better user control over dialog actions
- ⌨️ **Improved Special Characters**:
  - Period (.) available in special character mode
  - Complete symbol set: hyphen (-), underscore (_), period (.)

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
- Clone LVGL v8.4
- Build LVGL static library (one-time process)
- Build the application
- Optionally run it

### Option 2: Manual Setup

1. **Clone LVGL**
   ```bash
   git clone --depth 1 --branch release/v8.4 https://github.com/lvgl/lvgl.git
   ```

2. **Build LVGL Library** (one-time setup)
   ```bash
   # This creates lvgl/lib/liblvgl.a
   ./setup.sh
   ```

3. **Build the Application** (fast subsequent builds)
   ```bash
   make
   ```

4. **Run the Application**
   ```bash
   ./chunjiin
   ```

### Build Process

The project uses an optimized build system:

- **First time setup**: `./setup.sh` (builds LVGL library + application)
- **Subsequent builds**: `make` (very fast, ~0.003 seconds)
- **Clean build**: `make clean-all` (removes all build artifacts)

The LVGL library is built once into `lvgl/lib/liblvgl.a`, making subsequent application builds extremely fast.

## Configuration

The `lv_conf.h` file is pre-configured with:
- ✓ SDL2 driver enabled for desktop simulation
- ✓ FreeType library enabled for TrueType font rendering
- ✓ Memory settings optimized for Korean fonts (256KB heap)
- ✓ All necessary widgets enabled (buttons, text area, message box, etc.)

## Font Rendering

The application uses **LVGL 8.4's FreeType integration** to render Korean fonts directly from TrueType files:

- **Font Files:** NanumGothicCoding.ttf and NanumGothicCoding-Bold.ttf (included in `assets/`)
- **Font Sizes and Styles:**
  - 12px regular - Info labels
  - 14px regular - Button labels and standard UI elements
  - 16px regular - Text result area and popup messages
  - 20px regular - Popup dialog titles
- **Character Support:**
  - Complete Hangul Syllables (AC00-D7AF)
  - Hangul Compatibility Jamo (3130-318F) for displaying incomplete characters
  - ASCII characters (0020-007F)
  - Korean punctuation and symbols
  - Special symbols (←, ·, ‥)

No pre-conversion needed - fonts are loaded directly at runtime via FreeType!

## Project Structure

```
Chunjiin/
├── main.c                    # LVGL GUI with SDL2
├── chunjiin.c               # Core Chunjiin input logic
├── chunjiin_hangul.c        # Hangul character composition
├── chunjiin.h               # Header file with type definitions
├── lv_conf.h                # LVGL configuration (FreeType enabled)
├── Makefile                 # Optimized build system
├── setup.sh                 # Automated setup script
├── README.md                # This file
├── assets/                  # TrueType font files
│   ├── NanumGothic-Regular.ttf
│   ├── NanumGothic-Bold.ttf
│   └── NanumGothic-ExtraBold.ttf
└── lvgl/                    # LVGL library (cloned during setup)
    ├── src/                 # LVGL source code
    ├── build/               # LVGL object files (*.o)
    └── lib/                 # LVGL static library (liblvgl.a)
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

### Popup Dialog Functionality

The application features a robust popup system that displays input results:

- **Safe Implementation**: Uses custom container objects instead of problematic message box APIs
- **Korean Text Support**: Displays Korean text with bold font rendering
- **Dual-Button Interface**:
  - **Cancel Button** (취소): Gray color (0x808080) for secondary action
  - **Confirm Button** (확인): Blue color (0x4A90E2) for primary action
- **Bold Typography**: Title and message text use bold fonts for emphasis
- **Error Handling**: Shows appropriate messages for empty input
- **No Segmentation Faults**: Defensive programming prevents crashes

## Button Layout

The application maintains the same 3x5 grid layout with enhanced visual design:

```
┌─────────┬─────────┬─────────┐
│  천(1)   │  지(2)   │  인(3)   │  Row 0
├─────────┼─────────┼─────────┤
│  ㄱ(4)   │  ㄴ(5)   │  ㄷ(6)   │  Row 1
├─────────┼─────────┼─────────┤
│  ㅂ(7)   │  ㅅ(8)   │  ㅈ(9)   │  Row 2
├─────────┼─────────┼─────────┤
│ Space(10)│ ㅇㅁ(0)  │   ←(11)  │  Row 3
├─────────┼─────────┼─────────┤
│모드(橙色) │  지우기   │ 엔터(绿色)│  Row 4
└─────────┴─────────┴─────────┘
```

**Visual Enhancements:**
- **Mode Button**: Orange color (0xFF8C00) for easy identification
- **Enter Button**: Green color (0x28A745) to indicate primary action
- **Backspace**: Uses left arrow symbol (←) instead of "Del"
- **Bold Fonts**: All button labels use bold fonts for better readability

## Features

### Input Features
- ✓ **Chunjiin Korean input method** with real-time character composition
- ✓ **Display incomplete characters** (partial jamos) as you type
- ✓ **Multiple input modes:**
  - 한글 (Hangul) - Korean characters
  - 영문 (English) - Lowercase by default
  - 영문 대문자 (Uppercase English)
  - 숫자 (Numbers) - 0-9
  - 특수문자 (Special characters) - Enhanced symbol layout with period (.) added to button 7 (-_.)
- ✓ **Smart character composition** with vowel and consonant combining
- ✓ **Delete/backspace** support for step-by-step character decomposition with intuitive left arrow (←) symbol

### UI Features
- ✓ **Beautiful Korean font rendering** using NanumGothicCoding TrueType fonts
- ✓ **Clean, minimalist typography** with regular (non-bold) fonts:
  - Text result area uses regular 16px font
  - All button labels use regular 14px font
  - Popup dialog titles use regular 20px font
  - Popup dialog messages use regular 16px font
- ✓ **Scrollable text area** for long text input
- ✓ **Enhanced popup dialogs** with dual-button interface (Cancel/Confirm)
- ✓ **Color-coded buttons** for intuitive interaction:
  - Orange mode button for input mode switching
  - Green enter button for primary action
  - Gray cancel button / Blue confirm button in dialogs
- ✓ **Modern UI elements**:
  - Left arrow symbol (←) for backspace/delete
  - Clean, readable typography
- ✓ **Mode switching button** to cycle through input modes
- ✓ **Clear button** to reset input
- ✓ **320×640 portrait display** optimized for mobile-style layouts
- ✓ **Robust error handling** with defensive programming

## Troubleshooting

### Build Errors

1. **Missing LVGL library:**
   ```bash
   Error: LVGL library (lvgl/lib/liblvgl.a) not found!
   Please run './setup.sh' first to build LVGL library.
   ```
   **Solution:** Run `./setup.sh` to build the LVGL library first.

2. **Missing LVGL directory:**
   ```bash
   git clone --depth 1 --branch release/v9.2 https://github.com/lvgl/lvgl.git
   ```
   Or run:
   ```bash
   ./setup.sh
   ```

3. **SDL2 not found:**
   ```bash
   sudo apt-get install libsdl2-dev
   pkg-config --cflags --libs sdl2
   ```

4. **FreeType not found:**
   ```bash
   sudo apt-get install libfreetype6-dev
   pkg-config --cflags --libs freetype2
   ```

5. **Compilation errors:** 
   - Make sure `lv_conf.h` is in the project root
   - Verify LVGL was cloned to `lvgl/` directory
   - Check that all `.ttf` font files are in `assets/` directory
   - Run `make clean-all` and `./setup.sh` to rebuild everything

### Runtime Issues

1. **Application won't start / Segmentation fault:**
   - Check that font files exist in `assets/` directory
   - Verify FreeType libraries are installed: `ldconfig -p | grep freetype`
   - The application now uses safe popup implementation to avoid segfaults

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

5. **Popup issues:**
   - The application uses safe popup implementation that avoids segmentation faults
   - Popups auto-dismiss after 3 seconds
   - If popups don't appear, check console output for font loading errors

## Technical Details

### Architecture

| Component | Technology |
|-----------|------------|
| GUI Framework | LVGL 8.4 |
| Display Driver | SDL2 (hardware-accelerated texture rendering) |
| Font Rendering | FreeType 2 (runtime TrueType loading) |
| Layout System | LVGL Grid Layout |
| Input Device | SDL2 mouse/touch input |
| Input Method | Chunjiin (천지인) |
| Character Encoding | UTF-8 / Wide characters (wchar_t) |

### Unicode Ranges Used

- **ASCII:** U+0020-007F (Basic Latin)
- **Hangul Compatibility Jamo:** U+3130-318F (for standalone display)
- **Hangul Syllables:** U+AC00-D7AF (11,172 precomposed syllables)
- **Chunjiin Dots:** U+00B7 (·), U+2025 (‥)

### Performance

- **Memory Usage:** ~256KB LVGL 8.4 heap + ~5MB for fonts (loaded by FreeType)
- **Font Loading:** Runtime loading of 4 font variants (12px, 14px, 16px, 20px regular fonts)
- **Frame Rate:** 60 FPS (with SDL2 hardware-accelerated rendering and vsync)
- **Startup Time:** ~0.5 seconds (including all font loading)
- **Binary Size:** ~547KB (with LVGL 8.4 compiled in)

## Development

### Building from Source

```bash
# Clone the repository
git clone https://github.com/yourusername/Chunjiin.git
cd Chunjiin

# First time setup (builds LVGL library + application)
./setup.sh

# Subsequent builds (very fast)
make

# Clean everything
make clean-all

# Run the application
./chunjiin
```

### Build System

The project uses an optimized two-stage build process:

1. **LVGL Library Build** (one-time, ~30 seconds):
   - Compiles all LVGL sources into `lvgl/build/`
   - Creates static library `lvgl/lib/liblvgl.a`
   - Handled by `setup.sh`

2. **Application Build** (fast, ~0.003 seconds):
   - Compiles only application sources
   - Links against pre-built LVGL library
   - Handled by `make`

### Code Structure

- `main.c` - LVGL GUI, event handlers, font initialization, safe popup implementation
- `chunjiin.c` - Core input processing, mode management
- `chunjiin_hangul.c` - Korean character composition algorithms
- `chunjiin.h` - Type definitions and function declarations
- `setup.sh` - Automated setup script with LVGL library building
- `Makefile` - Optimized build system with library linking

## References

- **LVGL Documentation:** https://docs.lvgl.io/
- **SDL2 Documentation:** https://wiki.libsdl.org/
- **FreeType Documentation:** https://freetype.org/freetype2/docs/
- **NanumGothic Font:** https://hangeul.naver.com/font
- **Chunjiin Input Method:** https://en.wikipedia.org/wiki/Cheonjiin

# Chunjiin Input System

A robust Chunjiin (천지인) input system for Hangul, English, Number, and Special character entry, designed for use with the LVGL GUI framework.

## Features
- Hangul, English, Number, and Special input modes
- Full buffer and cursor boundary protection
- Comprehensive automated test suite
- Fuzz and edge-case testing

## Directory Structure
- `chunjiin.c`, `chunjiin.h`, `chunjiin_hangul.c`: Core Chunjiin logic
- `main.c`: Example or UI integration (if present)
- `test/`: Automated test suite and scripts
- `lvgl/`: LVGL library (submodule or source)

## How to Test
See `test/README.md` for details. To run all tests:

```bash
cd test
./run_test.sh
```

## License
See `lvgl/LICENCE.txt` for licensing information.
