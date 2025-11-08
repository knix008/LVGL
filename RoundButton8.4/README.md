# Round Button Demo - LVGL 8.4

A demonstration application showcasing round buttons with Korean text using LVGL 8.4 and SDL2.

## Features

- **Round Buttons**: Four beautifully styled circular buttons with gradients and shadows
- **Korean Font Support**: Uses FreeType to render Korean text with NanumGothicCoding font
- **Modern UI**: Gradient backgrounds, smooth shadows, and clean design
- **No Shrink Effect**: Buttons maintain their size when pressed
- **Window Size**: 320x640 pixels, optimized for mobile-like display

## Button Labels

- **재생** (Play) - Blue button
- **시작** (Start) - Green button
- **일시정지** (Pause) - Orange button
- **정지** (Stop) - Red button

## Requirements

### System Dependencies

- **SDL2**: Graphics and window management
- **FreeType**: TrueType font rendering
- **GCC**: C compiler
- **pkg-config**: For managing library compile/link flags

### Installation (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install -y libsdl2-dev libfreetype6-dev build-essential git
```

## Project Structure

```
RoundButton8.4/
├── main.c              # Main application code
├── lv_conf.h           # LVGL configuration
├── Makefile            # Build configuration
├── setup.sh            # Setup script for LVGL
├── assets/             # Font and resource files
│   └── NanumGothicCoding.ttf
├── lvgl/               # LVGL library (cloned)
└── button              # Compiled executable
```

## Building

### First Time Setup

Run the setup script to install dependencies and clone LVGL:

```bash
chmod +x setup.sh
./setup.sh
```

### Compile

```bash
make
```

Or with parallel compilation:

```bash
make -j4
```

### Clean Build

```bash
make clean
make
```

## Running

```bash
./button
```

Or build and run in one command:

```bash
make run
```

## Makefile Targets

- `make` or `make all` - Build the application
- `make clean` - Remove all build files
- `make run` - Build and run the application
- `make rebuild` - Clean and rebuild
- `make help` - Show available targets

## Configuration

### Window Size

Edit `main.c` to change window dimensions:

```c
#define WINDOW_WIDTH  320
#define WINDOW_HEIGHT 640
```

### Button Size

Modify button size in the `create_round_buttons()` function:

```c
lv_obj_set_size(btn1, 120, 120);  // Change to desired size
```

### Font Size

Change Korean font size in `hal_init()`:

```c
korean_font = lv_freetype_font_create("assets/NanumGothicCoding.ttf",
                                      LV_FREETYPE_FONT_RENDER_MODE_BITMAP,
                                      20,  // Change font size here
                                      LV_FREETYPE_FONT_STYLE_NORMAL);
```

## Technical Details

### LVGL Configuration

- Color depth: 32-bit (XRGB8888)
- Memory: 256KB allocated
- FreeType support: Enabled
- SDL2 backend: Enabled

### Button Styling

Each button features:
- Circular shape (`LV_RADIUS_CIRCLE`)
- Vertical gradient background
- Drop shadow (15px width, 5px offset)
- No scale transform on press (maintains size)
- Custom color schemes per button type

## Troubleshooting

### Font Not Loading

If you see "Warning: Failed to load Korean font":

1. Check that `assets/NanumGothicCoding.ttf` exists
2. Verify file permissions
3. Ensure FreeType is properly installed

### Window Doesn't Close

The application checks for window closure automatically. If it doesn't exit:
- Try pressing Ctrl+C in the terminal
- Check that SDL2 is properly configured

### Build Errors

If compilation fails:
- Ensure all dependencies are installed: `pkg-config --libs sdl2 freetype2`
- Run `make clean` before rebuilding
- Check that LVGL is cloned: `ls lvgl/`

## License

This project uses LVGL which is licensed under the MIT license.

## Credits

- **LVGL**: Light and Versatile Graphics Library
- **Font**: Nanum Gothic Coding (Open Font License)
- **Graphics Backend**: SDL2
- **Font Rendering**: FreeType
