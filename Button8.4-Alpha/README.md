# Button8.4-Alpha - LVGL Number Input System

A number input system built with LVGL 8.4 and SDL2, featuring Korean font support, PNG image display, and an interactive button matrix interface.

## Features

- **Interactive Number Input**: 12-button keypad matrix with numeric buttons (0-9), clear, backspace, and enter functions
- **Korean Font Support**: Full Korean text rendering using FreeType and NanumGothicCoding-Bold font
- **PNG Image Display**: Dialog box displays a custom 80x80 PNG image from the buttons directory
- **SDL2 Graphics Backend**: Cross-platform rendering using SDL2
- **Responsive UI**: Real-time display updates with formatted input display
- **Dark Theme Dialog**: Semi-transparent dark dialog with shadows for visual hierarchy

## Project Structure

```
Button8.4-Alpha/
├── main.c                          # Main application code
├── lv_conf.h                       # LVGL configuration file
├── Makefile                        # Build configuration
├── setup.sh                        # Setup script for dependencies
├── lvgl/                           # LVGL library (v8.4)
├── fonts/                          # Korean font files
│   ├── NanumGothicCoding-Bold.ttf
│   └── NanumGothicCoding.ttf
├── buttons/                        # Button images
│   ├── Button-Info-icon-resized.png (80x80 pixels)
│   ├── Button-Info-icon-original.png
│   └── Button-Info-icon-resized.png
└── assets/                         # Asset files
    └── images/
        └── button_png.png
```

## System Requirements

### Dependencies
- **Build Tools**: GCC, Make, Git
- **SDL2**: SDL2 development libraries (`libsdl2-dev`)
- **FreeType**: Font rendering (`libfreetype6-dev`)
- **libpng**: PNG image support (`libpng16-dev`)

### Display Configuration
- **Resolution**: 320x640 pixels
- **Color Depth**: 32-bit ARGB8888
- **Double Buffering**: Enabled for smooth rendering

## Installation

### 1. Install Dependencies

Run the setup script:
```bash
bash setup.sh
```

Or manually install required packages:
```bash
sudo apt-get update
sudo apt-get install -y \
  build-essential \
  libsdl2-dev \
  libfreetype6-dev \
  libpng16-dev \
  git
```

### 2. Verify Font Files

Ensure the following font files exist:
- `fonts/NanumGothicCoding-Bold.ttf`
- `fonts/NanumGothicCoding.ttf`

### 3. Build the Project

```bash
make clean
make
```

## Usage

### Run the Application

```bash
./test
```

### Keyboard Controls

| Button | Function |
|--------|----------|
| 0-9 | Input numeric digits |
| 지우기 | Clear input buffer |
| ← | Backspace/delete last digit |
| 입력 | Submit/Enter (shows dialog) |
| 닫기 | Close dialog |

## Build Configuration

### Makefile Targets

```bash
make                    # Build the application (default)
make clean             # Remove build artifacts
make distclean         # Remove all artifacts including LVGL library
make run               # Build and run the application
make debug             # Build with debug symbols
make release           # Build optimized release version
make install           # Install executable to /usr/local/bin
make help              # Show available targets
```

### Compiler Flags

- **Standard**: C99
- **Optimization**: -O2 (default), -O3 (release)
- **Debug**: -g (enabled by default)
- **Warnings**: -Wall -Wextra

## LVGL Configuration

Key LVGL settings in `lv_conf.h`:

### Color Depth
- **LV_COLOR_DEPTH**: 32-bit ARGB8888

### Image Decoder Support
- **LV_USE_PNG**: 1 (PNG decoder enabled)
- **LV_USE_LODEPNG**: 1 (Pure C PNG decoder)
- **LV_USE_LIBPNG**: 1 (External libpng library)

### Font System
- **LV_USE_FREETYPE**: 1 (TrueType font support)

## Font Configuration

Three Korean font sizes are loaded:
- **Size 16**: Dialog text labels
- **Size 20**: Display area and buttons
- **Size 24**: Alternative large font (currently unused)

### Font File Location
```c
"fonts/NanumGothicCoding-Bold.ttf"
```

## PNG Image Handling

### Image Specifications
- **File**: `buttons/Button-Info-icon-resized.png`
- **Dimensions**: 80x80 pixels
- **Format**: PNG with transparency (RGBA)
- **Display Path**: `"A:buttons/Button-Info-icon-resized.png"`

### Image in Dialog
The PNG image is displayed at the top of the dialog box:
- **Size**: 80x80 pixels
- **Position**: Top-center with 10px margin
- **Supports**: Full transparency and alpha blending

## UI Components

### Main Window
- **Display Container**: Shows current input with size 20 Korean font
- **Keypad Matrix**: 4x4 button grid with custom colors
  - Default: Dodger blue (#1E90FF)
  - Pressed/Focused: Light orange (#FFB366)

### Dialog Box
- **Background**: Dark gray with 60% opacity
- **Border**: 2px dark gray border with 10px radius
- **Shadow**: 20px shadow for depth effect
- **Contents**:
  - 80x80 PNG image at top
  - "숫자 입력" (Number Input) title
  - Input number display
  - "닫기" (Close) button

## Building LVGL Library

The LVGL static library (`lvgl/lib/liblvgl.a`) is built with:
- All core LVGL functionality
- PNG decoder support (LODEPNG + libpng)
- FreeType font support
- SDL2 backend support

### Library Files Generated
- Object files: `lvgl/build/*.o` (239 files)
- Static library: `lvgl/lib/liblvgl.a` (1.7MB)

## Display Driver

### SDL2 Backend Implementation
- **Display Flush Callback**: Renders LVGL draw buffer to SDL2 texture
- **Input Callback**: Handles mouse input from SDL2
- **Pixel Format**: ARGB8888
- **Texture Access**: Streaming mode for performance

## Troubleshooting

### Issue: PNG image not displaying
**Solution**: Ensure `lv_png_init()` is called after `lv_init()` and PNG decoders are enabled in `lv_conf.h`

### Issue: Korean font not loading
**Solution**: Verify font files exist in `fonts/` directory and FreeType is properly initialized

### Issue: Build errors with libpng
**Solution**: Ensure libpng development libraries are installed (`libpng16-dev`)

### Issue: SDL2 not found
**Solution**: Install SDL2 development packages: `sudo apt-get install libsdl2-dev`

## Development Notes

### Font Loading Process
1. FreeType library initialization
2. Load three font sizes (16, 20, 24) from TTF file
3. Apply fonts to appropriate UI components
4. Check for failures and fall back gracefully

### Image Decoding Process
1. `lv_png_init()` registers PNG decoder
2. LVGL uses LODEPNG or libpng based on configuration
3. Images loaded via `lv_img_set_src()` with `A:` prefix
4. `A:` prefix maps to the application working directory

### Input Processing Flow
1. Button press detected in button matrix
2. Button callback (`button_matrix_cb`) invoked
3. Input buffer updated
4. Display label refreshed
5. On Enter: Dialog created and displayed

## Performance

- **Compilation Time**: ~5 seconds
- **Executable Size**: 684KB
- **LVGL Library Size**: 1.7MB
- **Runtime Memory**: Efficient double-buffering implementation

## License

This project uses LVGL 8.4 (MIT License) and SDL2 (Zlib License).

## References

- [LVGL Documentation](https://docs.lvgl.io/8.4/)
- [SDL2 Documentation](https://wiki.libsdl.org/)
- [FreeType Documentation](https://www.freetype.org/freetype2/docs/)

## Version

- **Project**: Button8.4-Alpha
- **LVGL Version**: 8.4 (release/v8.4)
- **Build Date**: 2025-11-25
- **Platform**: Linux

---

Last Updated: 2025-11-25
