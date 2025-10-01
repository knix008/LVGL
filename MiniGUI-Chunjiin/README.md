# ChunJiIn Korean Input System

A complete Korean input system based on the ChunJiIn (천지인) principle, implemented with MiniGUI for embedded systems.

## ✨ Korean Character Display

This application **fully displays Korean characters** in the MiniGUI interface:

- **Button labels**: ㅣ, ㆍ, ㅡ, ㄱ,ㅋ, ㄴ,ㄹ, ㄷ,ㅌ, ㅂ,ㅍ, ㅅ,ㅎ, ㅈ,ㅊ, ㅇ,ㅁ
- **Text input**: Real-time Korean text composition in the textbox
- **TrueType fonts**: NanumGothic fonts (Regular, Bold, ExtraBold) included
- **Proper encoding**: UTF-8 throughout the application

**Quick Test:**
```bash
./test_korean_display.sh
```

This will start the application and show font loading status. You should see:
```
✓ Loaded NanumGothic TrueType font (size 20) for Korean characters
```

## Overview

The ChunJiIn input system is based on the traditional Korean philosophical concept of "천지인" (Heaven, Earth, Human), using three fundamental elements to create all Korean characters:

- **ㆍ (천)** - Sky/Heaven (dot)
- **ㅡ (지)** - Earth (horizontal line)  
- **ㅣ (인)** - Human (vertical line)

## Features

### Core ChunJiIn System
- **Complete Korean syllable formation** (choseong, jungseong, jongseong)
- **Advanced vowel combinations** following ChunJiIn rules
- **Consonant cycling and toggling** for related sounds
- **Dot (ㆍ) display and cycling** functionality
- **Full Unicode support** with memory-safe operations
- **Comprehensive state machine** architecture

### Application
- **ChunJiIn Keypad Application** - Full MiniGUI interface with ChunJiIn keyboard

## File Structure

```
├── chunjiin_input.c          # Core ChunJiIn input system implementation
├── chunjiin_input.h          # ChunJiIn input system header
├── chunjiin_app.c            # Main ChunJiIn MiniGUI application (integrated UI)
├── run.sh                    # Run script for ChunJiIn application
├── Makefile                  # Build configuration
├── MiniGUI.cfg               # MiniGUI configuration
└── assets/                   # Korean fonts
    ├── NanumGothic-Regular.ttf
    ├── NanumGothic-Bold.ttf
    └── NanumGothic-ExtraBold.ttf
```

## Building

### Prerequisites
- MiniGUI development environment with FreeType support
- Korean fonts (NanumGothic included in assets/)
- GCC compiler with C99 support
- System packages: libjpeg, libpng, libfreetype, libharfbuzz

### First-Time Setup

If MiniGUI is not installed, use the build script to download and build MiniGUI locally:

```bash
./build.sh
```

This will:
- Download MiniGUI source from GitHub
- Build MiniGUI in standalone mode with Korean support
- Install MiniGUI locally in `./install/`
- Copy Korean fonts to the MiniGUI font directory
- Build the ChunJiIn application

### Build Commands

```bash
# Build ChunJiIn application (after MiniGUI is installed)
make all

# Clean build files
make clean

# Show help
make help

# Check if MiniGUI is installed
make check-minigui
```

## Usage

### ChunJiIn Input System

The ChunJiIn system uses a simplified keyboard layout with three fundamental elements:

#### Fundamental Elements
- **a** - ㆍ (천) - Sky/Heaven
- **e** - ㅡ (지) - Earth  
- **i** - ㅣ (인) - Human

#### Consonant Groups
- **g** - ㄱ, ㅋ, ㄲ (cycle with repeated presses)
- **n** - ㄴ, ㄹ (cycle with repeated presses)
- **d** - ㄷ, ㅌ, ㄸ (cycle with repeated presses)
- **b** - ㅂ, ㅍ, ㅃ (cycle with repeated presses)
- **s** - ㅅ, ㅎ, ㅆ (cycle with repeated presses)
- **j** - ㅈ, ㅊ, ㅉ (cycle with repeated presses)
- **m** - ㅇ, ㅁ (cycle with repeated presses)

#### Special Keys
- **Space** - Finalize current syllable
- **<** - Backspace (cancel composition or delete character)
- **.** - Enter (complete input and clear)

### Running Applications

#### ChunJiIn Keypad Application

**Standard Run:**
```bash
./run.sh
```

**Direct Run (if environment is already configured):**
```bash
export LD_LIBRARY_PATH="./install/lib:$LD_LIBRARY_PATH"
export MG_CONFIG_FILE="./MiniGUI.cfg"
./chunjiin_app
```

**Test Korean Character Display:**
```bash
./test_korean_display.sh
```

#### Application Features
- Full MiniGUI interface with ChunJiIn keyboard
- 5-row layout with ChunJiIn fundamental elements
- **Korean character display** on all buttons (ㅣ, ㆍ, ㅡ, ㄱ,ㅋ, etc.)
- **Real-time Korean text input** in the textbox
- Visual feedback for input states
- TrueType Korean font support (NanumGothic)
- Press Escape or Q to quit
- Press F1 for help

## ChunJiIn Input Examples

### Basic Character Formation

```
Input: g + a + i
Output: 가 (ㄱ + ㅏ = 가)

Input: n + e + i  
Output: 느 (ㄴ + ㅡ = 느)

Input: m + i
Output: 미 (ㅁ + ㅣ = 미)
```

### Consonant Cycling

```
Input: g (first press)
Output: ㄱ

Input: g (second press)  
Output: ㅋ

Input: g (third press)
Output: ㄲ

Input: g (fourth press)
Output: ㄱ (cycles back)
```

### Vowel Combinations

```
Input: a + e
Output: ㅗ (ㆍ + ㅡ = ㅗ)

Input: a + i
Output: ㅓ (ㆍ + ㅣ = ㅓ)

Input: a + a + e
Output: ㅛ (ㆍㆍ + ㅡ = ㅛ)
```

### Complex Syllables

```
Input: g + a + i + g
Output: 각 (ㄱ + ㅏ + ㄱ = 각)

Input: n + e + i + n
Output: 능 (ㄴ + ㅡ + ㄴ = 능)
```

## 5-Row Center-Aligned Keyboard Layout

The ChunJiIn keyboard follows a 5-row center-aligned layout optimized for Korean input:

```
┌─────────────────────────────────────────────────────────┐
│                    ChunJiIn Korean Input System        │
│                                                         │
│  [Text Display Area]                                    │
│                                                         │
│  Row 1: ChunJiIn Fundamental Elements (Center-Aligned)   │
│        ┌─────────┐ ┌─────────┐ ┌─────────┐               │
│        │   인    │ │   천    │ │   지    │               │
│        │  (ㅣ)   │ │  (ㆍ)   │ │  (ㅡ)   │               │
│        └─────────┘ └─────────┘ └─────────┘               │
│                                                         │
│  Row 2: Basic Consonants (Center-Aligned)              │
│        ┌─────────┐ ┌─────────┐ ┌─────────┐               │
│        │ ㄱ,ㅋ   │ │ ㄴ,ㄹ   │ │ ㄷ,ㅌ   │               │
│        └─────────┘ └─────────┘ └─────────┘               │
│                                                         │
│  Row 3: Additional Consonants (Center-Aligned)         │
│        ┌─────────┐ ┌─────────┐ ┌─────────┐               │
│        │ ㅂ,ㅍ   │ │ ㅅ,ㅎ   │ │ ㅈ,ㅊ   │               │
│        └─────────┘ └─────────┘ └─────────┘               │
│                                                         │
│  Row 4: Special Keys (Center-Aligned)                  │
│        ┌─────────┐ ┌─────────┐ ┌─────────┐               │
│        │  Space  │ │ ㅇ,ㅁ   │ │  Back  │               │
│        └─────────┘ └─────────┘ └─────────┘               │
│                                                         │
│  Row 5: Control Keys (Center-Aligned)                  │
│              ┌─────────┐ ┌─────────┐                   │
│              │  Enter  │ │  Clear  │                   │
│              └─────────┘ └─────────┘                   │
└─────────────────────────────────────────────────────────┘
```

### Layout Features
- **Center-aligned rows** for balanced appearance
- **Reduced button spacing** (10px) for compact layout
- **X-offset positioning** for fine-tuned alignment
- **Consistent button sizing** across all rows
- **Optimized spacing** between rows
- **Dynamic positioning** with automatic center calculation
- **Responsive layout** that adapts to window size

### Button Functions

#### Row 1: ChunJiIn Fundamental Elements
- **인 (ㅣ)** - Human element (vertical line)
- **천 (ㆍ)** - Sky/Heaven element (dot)
- **지 (ㅡ)** - Earth element (horizontal line)

#### Row 2: Basic Consonants
- **ㄱ,ㅋ** - G group (ㄱ, ㅋ, ㄲ - cycle with repeated presses)
- **ㄴ,ㄹ** - N group (ㄴ, ㄹ - cycle with repeated presses)
- **ㄷ,ㅌ** - D group (ㄷ, ㅌ, ㄸ - cycle with repeated presses)

#### Row 3: Additional Consonants
- **ㅂ,ㅍ** - B group (ㅂ, ㅍ, ㅃ - cycle with repeated presses)
- **ㅅ,ㅎ** - S group (ㅅ, ㅎ, ㅆ - cycle with repeated presses)
- **ㅈ,ㅊ** - J group (ㅈ, ㅊ, ㅉ - cycle with repeated presses)

#### Row 4: Special Keys
- **Space** - Finalize current syllable
- **ㅇ,ㅁ** - M group (ㅇ, ㅁ - cycle with repeated presses)
- **Back** - Backspace (cancel composition or delete character)

#### Row 5: Control Keys
- **Enter** - Complete input and clear
- **Clear** - Clear all input

## Technical Details

### State Machine
The ChunJiIn system uses a sophisticated state machine to track syllable formation:

- **STATE_START** - Initial state, no input
- **STATE_CHOSEONG** - Initial consonant entered
- **STATE_JUNGSEONG** - Vowel entered  
- **STATE_JONGSEONG** - Final consonant entered

### Memory Management
- Wide character buffers for Unicode support
- Bounds checking for all buffer operations
- Safe UTF-8 conversion functions
- Automatic cleanup on exit

### Font Support
- **NanumGothic TrueType fonts** for Korean character display
  - NanumGothic-Regular.ttf (primary)
  - NanumGothic-Bold.ttf (fallback)
  - NanumGothic-ExtraBold.ttf (fallback)
- **Automatic font loading** with multiple fallback methods
- **UTF-8 character encoding** throughout
- **MiniGUI TrueType font configuration** in MiniGUI.cfg
- Fonts loaded at size 20px for optimal readability

## Troubleshooting

### Common Issues

#### 1. Korean characters not displaying in MiniGUI windows

**Symptoms:** Buttons and textbox show boxes, question marks, or empty spaces instead of Korean characters.

**Solutions:**

a. **Check MiniGUI.cfg TrueType font configuration:**
   ```bash
   # Verify [truetypefonts] section in MiniGUI.cfg
   cat MiniGUI.cfg | grep -A 10 "truetypefonts"
   ```
   Should show:
   ```ini
   [truetypefonts]
   font_number=3
   name0=ttf-NanumGothic-rrncnn-0-0-UTF-8
   fontfile0=./install/share/fonts/NanumGothic-Regular.ttf
   ```

b. **Verify font files exist:**
   ```bash
   ls -lh ./install/share/fonts/
   ```
   Should show NanumGothic-Regular.ttf, NanumGothic-Bold.ttf, and NanumGothic-ExtraBold.ttf

c. **Check font loading messages:**
   Run `./test_korean_display.sh` and look for:
   ```
   ✓ Loaded NanumGothic TrueType font (size 20) for Korean characters
   ```

d. **Verify MiniGUI FreeType support:**
   ```bash
   ldd ./install/lib/libminigui_sa.so | grep freetype
   ```
   Should show libfreetype linked

e. **Rebuild if necessary:**
   ```bash
   make clean && make
   ```

**Note:** The Korean characters ARE in the source code ([chunjiin_app.c](chunjiin_app.c)). If your editor cannot display them, that's an editor font issue, not an application issue. The MiniGUI application will display them correctly when running.

#### 2. MiniGUI fails to initialize

**Error:** `make_devfont: invalid font type`

**Solution:** Check that font names in MiniGUI.cfg use the correct format:
- Correct: `ttf-NanumGothic-rrncnn-0-0-UTF-8`
- Incorrect: `NanumGothic-Regular`

#### 3. Application won't start - library not found

**Error:** `error while loading shared libraries: libminigui_sa.so`

**Solution:**
```bash
export LD_LIBRARY_PATH="./install/lib:$LD_LIBRARY_PATH"
```
Or use `./run.sh` which sets this automatically.

#### 4. Input not working

**Symptoms:** Clicking buttons doesn't produce text

**Solutions:**
- Check MiniGUI installation is complete
- Verify keyboard focus is on the application window
- Check terminal for error messages

#### 5. Build errors

**Error:** `cannot find -lminigui_sa`

**Solution:** MiniGUI needs to be built first:
```bash
./build.sh
```

**Error:** Compiler errors about missing headers

**Solution:** Ensure all prerequisites are installed (see Building section)

### Debug Information

Watch for font loading messages when starting the application:
```bash
./chunjiin_app 2>&1 | grep -i font
```

Check MiniGUI configuration:
```bash
echo "MG_CONFIG_FILE: $MG_CONFIG_FILE"
cat $MG_CONFIG_FILE | grep -A 10 "truetypefonts"
```

Verify font file permissions:
```bash
ls -lh ./install/share/fonts/*.ttf
```

## License

This project is part of the MiniGUI-Chunjiin Korean input system implementation.

## Contributing

When contributing to the ChunJiIn system:

1. Follow the existing code style
2. Add tests for new functionality
3. Update documentation
4. Ensure Korean character support
5. Test with various input combinations

## References

- ChunJiIn (천지인) Korean input method
- Korean Unicode standard
- MiniGUI documentation
- Korean language processing