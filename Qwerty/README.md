# MAC Address Hex Input with LVGL

A virtual hex input application for MAC address entry built with LVGL (Light and Versatile Graphics Library), SDL2, and FreeType for TrueType font rendering.

## Features

- **Hex Input Interface**: Virtual keyboard optimized for MAC address input
- **FreeType Font Rendering**: Direct TrueType font rendering at runtime (no conversion needed)
- **Modern UI**: Built with LVGL v9 for smooth, hardware-accelerated graphics
- **Compact Design**: Optimized 640×480 layout
- **Efficient Build System**: LVGL compiled once during setup, fast application builds

## Requirements

- **GCC compiler**
- **Make**
- **SDL2 development libraries** (libsdl2-dev)
- **FreeType development libraries** (libfreetype-dev)
- **LVGL v9.x** (automatically cloned and built by setup script)
- **Git** (for cloning LVGL)

## Quick Start

### 1. Run Setup Script

The setup script will install dependencies and build LVGL:

```bash
./setup.sh
```

This will:
- Check for and install required build tools (gcc, make, pkg-config)
- Install SDL2 and FreeType development libraries if missing
- Clone LVGL v9.2 into the project directory
- Build LVGL as a static library (`lvgl/build/liblvgl.a`)
- Verify lv_conf.h configuration

### 2. Build the Application

```bash
make
```

The build system is optimized for development:
- **Setup phase**: LVGL is compiled once into a static library
- **Application builds**: Only compile application sources (very fast)
- **No recompilation**: LVGL library is reused across builds

### 3. Run the Application

```bash
./hexinput
```

Or:
```bash
make run
```

## Usage

### Hex Input Interface

The application provides a virtual keyboard optimized for MAC address input with a compact 640×480 window layout.

### Controls

- **Hex Keys**: 0-9, A-F for hexadecimal input
- **← (Backspace)**: Delete last character
- **Enter**: Show input result in popup dialog and clear text area
- **Space**: Insert space character for MAC address formatting
- **Clear**: Clear all text
- **Function buttons**: Color-coded for easy identification

### Button Color Scheme

The application uses color-coded buttons for easy identification:

- **🟠 Orange**: Function buttons (Clear)
- **🔵 Blue**: Action buttons (Enter)
- **⚪ Default**: Regular hex input keys (0-9, A-F)

This color scheme helps users quickly identify special function buttons versus regular input keys.

## Project Structure

```
Qwerty/
├── main.c              # Main application with LVGL UI
├── qwerty.c            # Hex input logic
├── qwerty.h            # Header file
├── lv_conf.h           # LVGL configuration (v9.4.0)
├── Makefile            # Build configuration (application only)
├── setup.sh            # Environment setup script (builds LVGL)
├── assets/             # TrueType font files
│   ├── NanumGothicCoding.ttf
│   └── NanumGothicCoding-Bold.ttf
├── lvgl/               # LVGL library (cloned and built by setup.sh)
│   └── build/
│       └── liblvgl.a   # Pre-built LVGL static library
├── LICENSE
└── README.md           # This file
```

## Configuration

### LVGL Configuration (lv_conf.h)

The project includes a pre-configured `lv_conf.h` file with:
- **LVGL version**: v9.4.0-dev
- **Color depth**: 32-bit (XRGB8888)
- **SDL driver**: Enabled (LV_USE_SDL = 1)
- **FreeType support**: Enabled (LV_USE_FREETYPE = 1)
- **Memory pool**: 256KB (for font rendering)

### Display Resolution

Current resolution: **640×480** pixels

To change the display resolution, edit these constants in `main.c`:

```c
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
```

### Font Configuration

The application uses **NanumGothicCoding** as the default font with **FreeType** for direct TrueType rendering:

- **Status Label**: NanumGothicCoding 12px (normal style)
- **Text Area**: NanumGothicCoding 16px (normal style)
- **Keyboard Buttons**: NanumGothicCoding 16px (normal style)

**Key Features:**
- **No font conversion needed!** FreeType renders `.ttf` files at runtime
- **Consistent appearance**: All text uses the same coding font family
- **Normal style**: All fonts use regular weight (not bold) for better readability

## Recent Updates

### Version Improvements

**Latest Updates:**
- **Optimized build system**: LVGL compiled once during setup, fast application builds
- **Color-coded buttons**: Orange for function buttons, blue for action buttons
- **Efficient compilation**: Application builds are very fast (no LVGL recompilation)
- **Simplified setup**: Single setup script handles all dependencies and LVGL building
- **Static library approach**: LVGL built as `liblvgl.a` for reliable linking

**Build System Enhancements:**
- **Setup phase**: LVGL is compiled once into a static library
- **Application builds**: Only compile application sources (very fast)
- **No recompilation**: LVGL library is reused across builds
- **Clean separation**: Setup handles LVGL, Makefile handles application

## Building from Scratch

### Install Dependencies (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install build-essential libsdl2-dev libfreetype-dev pkg-config git
```

### Setup and Build

```bash
./setup.sh  # Installs dependencies, clones and builds LVGL
make        # Builds the application
```

The setup script compiles LVGL once into a static library. Application builds are very fast afterward.

## Troubleshooting

### "LVGL library not found" Error

Run the setup script to build LVGL:
```bash
./setup.sh
```

### "SDL not found" Error

Install SDL2 development libraries:
```bash
sudo apt-get install libsdl2-dev
```

### "FreeType not found" Error

Install FreeType development libraries:
```bash
sudo apt-get install libfreetype-dev
```

### Font Display Issues

The application uses fonts from `assets/` directory. Verify:
1. `assets/NanumGothicCoding.ttf` exists
2. Font files are not corrupted

### Compilation Errors

1. Verify `lv_conf.h` exists in the project directory
2. Check that `lvgl/build/liblvgl.a` exists (run `./setup.sh` if missing)
3. Run `make clean` and then `make`
4. Check compiler flags support C11 standard

### Build System Issues

**If builds are slow:**
- Run `./setup.sh` first to build LVGL library
- Application builds should be very fast after setup
- Use `make clean-lvgl` to rebuild LVGL if needed

**If LVGL library is missing:**
- Run `./setup.sh` to build the LVGL static library
- Check that `lvgl/build/liblvgl.a` exists

## Development

### Button Size Customization

All buttons are **35×39px** (regular keys). To modify:

```c
// In create_key_button():
lv_obj_set_size(btn, width, 39);  // Change height here

// For individual buttons, change width parameter:
create_key_button(row, "A", callback, data, 38);  // Change width
```

### Button Color Customization

To change button colors, modify the styling in `create_gui()`:

```c
// Orange color for function buttons
lv_obj_set_style_bg_color(app_state.clear_button, lv_color_hex(0xFF8C00), 0);

// Blue color for action buttons  
lv_obj_set_style_bg_color(app_state.enter_button, lv_color_hex(0x0000FF), 0);
```

### Font Size Customization

Edit `init_fonts()` function in `main.c`:

```c
app_state.korean_font_20 = lv_freetype_font_create(
    "assets/NanumGothicCoding.ttf",
    LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
    16,  // <- Change size here
    LV_FREETYPE_FONT_STYLE_NORMAL
);
```

### Adding New Fonts

Place your `.ttf` font file in `assets/` directory and load it:

```c
lv_font_t *my_font = lv_freetype_font_create(
    "assets/MyFont.ttf",
    LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
    20,
    LV_FREETYPE_FONT_STYLE_NORMAL
);
```

### Modifying Hex Input Layout

Edit the `key_maps` array in `qwerty.c` to customize the hex input interface:

```c
KeyMap key_maps[16] = {
    {"0", "0"},  // {normal, shift}
    {"1", "1"},
    {"2", "2"},
    // ... hex keys 0-F
};
```

## Technical Details

### Technologies Used

- **LVGL**: v9.2 (Light and Versatile Graphics Library)
- **SDL2**: Display and input handling
- **FreeType**: TrueType font rendering
- **C11**: Programming language standard

### Architecture

```
User Input → LVGL Event → Hex Input Processing → 
  Text Validation → MAC Address Display
```

### Memory Usage

- **LVGL pool**: 256KB (configured in lv_conf.h)
- **FreeType cache**: 256 glyphs (default)
- **Typical runtime**: ~10MB RAM

### Performance

- **Frame rate**: 30 FPS (SDL VSync)
- **Input latency**: < 5ms
- **Font rendering**: Hardware accelerated (SDL2)
- **Build time**: Very fast application builds (LVGL pre-compiled)

## License

See [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please feel free to submit issues or pull requests.

## Credits

- **LVGL**: https://lvgl.io/
- **SDL2**: https://www.libsdl.org/
- **FreeType**: https://www.freetype.org/
- **NanumGothic Font**: Naver Corporation

## References

- [LVGL Documentation](https://docs.lvgl.io/)
- [LVGL v9 Migration Guide](https://docs.lvgl.io/master/CHANGELOG.html)
- [FreeType Documentation](https://www.freetype.org/freetype2/docs/documentation.html)
- [SDL2 Documentation](https://wiki.libsdl.org/)
