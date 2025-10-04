# Korean English Number Input System

A multi-mode input system for Korean, English, and Number entry built with LVGL (Light and Versatile Graphics Library) and SDL2.

## Features

- **Three Input Modes:**
  - **Korean (KOR)**: ChunJiIn (천지인) input method for Korean characters
  - **English (ENG)**: T9-style phone keypad for English text entry
  - **Number (123)**: Number pad with numeric input

- **Optimized for Portrait Display**: 320 x 640 window size
- **Mode Switching**: Easy toggle between input modes via top-right button
- **Korean Font Support**: Uses FreeType with fallback to built-in CJK fonts

## Screenshot

The application displays a compact keyboard interface optimized for narrow screens.

## Requirements

- **Operating System**: Linux (Ubuntu/Debian recommended)
- **Dependencies**:
  - CMake (>= 3.10)
  - SDL2
  - FreeType2
  - GCC/Clang compiler
  - pkg-config

## Installation

### 1. Install Dependencies

On Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install build-essential cmake pkg-config libsdl2-dev libfreetype-dev
```

### 2. Build the Project

```bash
cd Source
./build.sh
```

The build script will:
- Install any missing dependencies (with sudo)
- Configure the project with CMake
- Build LVGL library
- Build the main application

## Usage

### Running the Application

```bash
cd Source
./run.sh
```

Or run directly:
```bash
cd Source/build
./main
```

### Input Modes

#### Korean ChunJiIn Mode (KOR)

The ChunJiIn input method uses three basic strokes:
- **|** (ㅣ) - Vertical bar
- **.** (ㆍ) - Dot
- **-** (ㅡ) - Horizontal bar

Plus consonant groups:
- G/K (ㄱ/ㅋ)
- N/L (ㄴ/ㄹ)
- D/T (ㄷ/ㅌ)
- B/P (ㅂ/ㅍ)
- S/H (ㅅ/ㅎ)
- J/C (ㅈ/ㅊ)
- O/M (ㅇ/ㅁ)

Layout:
```
[|]  [.]  [-]
[G/K][N/L][D/T]
[B/P][S/H][J/C]
[Space][O/M][←]
    [Enter]
```

#### English T9 Mode (ENG)

Phone-style T9 input where you click a button multiple times to cycle through letters:

```
[ABC][DEF][GHI]
[JKL][MNO][PQR]
[STU][VWX][YZ]
[Next][Space][←]
[Clear]  [Enter]
```

- Click a button repeatedly to cycle through its letters (e.g., ABC: a→b→c→a)
- Press "Next" to finalize the current character
- Press "Space" to add a space
- Press "←" to delete the last character

#### Number Mode (123)

Standard number pad layout:

```
[1][2][3]
[4][5][6]
[7][8][9]
[Clear][0][←]
   [Enter]
```

### Mode Switching

Click the button in the top-right corner to cycle through modes:
- KOR → ENG → 123 → KOR

## Project Structure

```
Source/
├── assets/               # Font files (NanumGothic TrueType fonts)
├── build/               # Build output directory
├── include/             # Header files
│   ├── chunjiin_input.h
│   ├── font_config.h
│   ├── qwerty_korean.h
│   ├── tab_chunjiin.h
│   ├── tab_english.h
│   ├── tab_number.h
│   └── ui_components.h
├── lvgl/                # LVGL library (submodule)
├── src/                 # Source files
│   ├── chunjiin_input.c  # ChunJiIn input logic
│   ├── main.c            # Application entry point
│   ├── qwerty_korean.c   # Korean QWERTY logic
│   ├── tab_chunjiin.c    # Korean input UI
│   ├── tab_english.c     # English T9 input UI
│   ├── tab_number.c      # Number pad UI
│   └── ui_components.c   # Main UI and mode switching
├── build.sh             # Build script
├── run.sh               # Run script
└── CMakeLists.txt       # CMake configuration
```

## Configuration

### Font Settings

Font configuration is in `include/font_config.h`:

```c
#define FONT_DIR "../assets/"
#define KOREAN_FONT_REGULAR "NanumGothic-Regular.ttf"
#define DEFAULT_FONT_SIZE 24
```

### Window Size

Window size is set in `src/main.c`:

```c
lv_display_t * disp = lv_sdl_window_create(320, 640);
```

## Building from Source

### Manual Build Steps

1. Create build directory:
```bash
mkdir -p build
cd build
```

2. Configure with CMake:
```bash
cmake ..
```

3. Build:
```bash
make -j$(nproc)
```

4. Run:
```bash
./main
```

### Clean Build

To clean and rebuild:
```bash
rm -rf build
./build.sh
```

## Troubleshooting

### Font Not Loading

If Korean characters don't display:
- Check that `assets/NanumGothic-Regular.ttf` exists
- The application falls back to built-in CJK fonts automatically
- Verify `LV_FONT_SOURCE_HAN_SANS_SC_16_CJK` is enabled in `lvgl/lv_conf.h`

### SDL2 Errors

If SDL2 window doesn't appear:
```bash
# Reinstall SDL2
sudo apt-get install --reinstall libsdl2-dev
```

### Build Errors

If you encounter build errors:
```bash
# Clean build
rm -rf build
# Reinstall dependencies
sudo apt-get install build-essential cmake pkg-config libsdl2-dev libfreetype-dev
# Rebuild
./build.sh
```

## Development

### Adding New Input Modes

1. Create header file in `include/tab_yourmode.h`
2. Create implementation in `src/tab_yourmode.c`
3. Add to `CMakeLists.txt` MAIN_SOURCES
4. Update mode enum in `src/ui_components.c`
5. Add case to `switch_input_mode()` function

### Customizing Button Layout

Button positions and sizes are defined in each tab file:
- `src/tab_chunjiin.c` - Korean layout
- `src/tab_english.c` - English T9 layout
- `src/tab_number.c` - Number pad layout

## License

This project uses LVGL which is licensed under the MIT License.

## Credits

- **LVGL**: https://lvgl.io/
- **Font**: NanumGothic (Naver Corporation)
- **ChunJiIn Input Method**: Traditional Korean input system

## Version History

- **v1.0** - Initial release
  - Korean ChunJiIn input
  - English T9 input
  - Number pad
  - 320x640 portrait mode
  - Mode switching
