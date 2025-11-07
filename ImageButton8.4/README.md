# LVGL 8.4 Button Demo with Korean Font Support

A simple but feature-rich button demo application built with **LVGL 8.4** and **SDL2**, featuring Korean language support using TrueType fonts.

## Overview

This project demonstrates:
- LVGL 8.4 GUI framework integration
- SDL2-based rendering and input handling
- FreeType font loading for Korean text support
- Interactive button widgets with visual feedback
- Toggle buttons and disabled button states

## Features

### UI Components
- **3 Interactive Buttons with Static Images**: Orange buttons (280x70px) with visual effects and click feedback
  - Button 1: PNG image (button_png.png) with green border flash effect
  - Button 2: BMP image (button_bmp.bmp) with green border flash effect
  - Button 3: JPG image (button_jpg.jpg) with green border flash effect
- **1 GIF Image Button**: Orange button with animated GIF displaying visual feedback on click
  - GIF 버튼: Displays animated GIF with green border flash effect
  - Smooth color animation when clicked
- **1 Toggle Button**: Gray button that toggles between states with animated color transitions
  - Gray (OFF) to Blue (ON) with smooth color animation
  - Proper event handling and state tracking
- **1 Disabled Button**: Non-interactive button to show disabled state
- **Korean UI Text**: All text displayed in Korean using NanumGothicCoding font
- **Title and Info Label**: Provides user guidance in Korean
- **Display Resolution**: 320x640 portrait orientation

### Technical Features
- **Multi-Format Image Support**: PNG, BMP, JPG, and GIF images
- **Image Decoders**: LibPNG for PNG, native BMP decoder, TJPGD for JPG, LVGL GIF decoder
- **Animated GIF Support**: LVGL 8.4 built-in GIF widget for frame-by-frame animation
- **Flex Layout**: Buttons use flex layout for proper image and label alignment
- **Double Buffering**: Smooth rendering with LVGL's display buffer
- **Mouse Input**: Real-time mouse click detection and handling
- **FreeType Integration**: Dynamic TrueType font loading at runtime
- **Event Handling**: Proper LVGL event callbacks for button interactions
- **SDL2 Backend**: Cross-platform display and input
- **Visual Effects**: Safe and reliable visual feedback animations
  - Green border flash effect on image buttons (200ms duration)
  - Green border flash effect on GIF button with color animation
  - Smooth color transitions for toggle buttons
  - Timer-based animation cleanup to prevent visual artifacts

## Project Structure

```
ImageButton8.4/
├── main.c                          # Main application source code
├── Makefile                        # Build configuration
├── README.md                       # This file
├── .gitignore                      # Git ignore rules
├── lv_conf.h                       # LVGL configuration
├── setup.sh                        # Setup script for dependencies
├── lvgl/                           # LVGL library (v8.4)
│   ├── src/                        # LVGL source code
│   ├── lib/                        # Compiled LVGL library (liblvgl.a)
│   └── ...
└── assets/
    ├── fonts/
    │   ├── NanumGothicCoding.ttf   # Korean font (regular)
    │   └── NanumGothicCoding-Bold.ttf # Korean font (bold)
    └── images/
        ├── button_png.png          # PNG image (32x32)
        ├── button_bmp.bmp          # BMP image (32x32)
        ├── button_jpg.jpg          # JPG image (32x32)
        └── button_gif.gif          # Animated GIF (multiple frames)
```

## Prerequisites

### System Requirements
- **Linux** (Ubuntu 18.04 or later recommended)
- **GCC** compiler
- **SDL2** development libraries
- **FreeType2** development libraries
- **libpng** development libraries (for PNG image support)
- **Git** (for cloning LVGL)

### Install Dependencies

```bash
sudo apt-get update
sudo apt-get install -y build-essential libsdl2-dev libfreetype6-dev libpng-dev git
```

## Building

### 1. Initial Setup

Run the setup script to prepare the LVGL library:

```bash
./setup.sh
```

This script will:
- Check system dependencies
- Clone LVGL v8.4 if not already present
- Build the LVGL static library

### 2. Build the Application

```bash
make
```

Or clean and rebuild:

```bash
make clean && make
```

### 3. Run the Application

```bash
./button_demo
```

Or use the Makefile target:

```bash
make run
```

## Usage

### Interaction
- **Click Image Buttons (PNG/BMP/JPG)**:
  - Orange buttons change to green with smooth color animation
  - Green border flashes around the image for 200ms
  - Images remain visible and stable throughout the animation
- **Click GIF Button**:
  - Orange button changes color with animation on click
  - Green border flashes around the GIF for 200ms
  - GIF animation continues playing during the effect
  - Visual feedback similar to image buttons
- **Click Toggle Button**:
  - Toggle button changes color between gray (OFF) and blue (ON)
  - Smooth animated color transitions
  - Proper state tracking and feedback
- **Disabled Button**: The disabled button cannot be clicked
- **Console Output**: Button events are printed to console (e.g., "Button clicked!", "Toggle button is ON", "GIF button clicked!")

### Exit
- Close the window, or
- Press ESC key

## Configuration

### Display Resolution
Edit these defines in `main.c` to change the window size:

```c
#define DISP_HOR_RES 320  // Horizontal resolution (portrait)
#define DISP_VER_RES 640  // Vertical resolution (portrait)
```

### Button Layout
Button positions and sizes are defined in `create_buttons()`:

```c
lv_obj_set_pos(btn1, 20, 40);       // X, Y position
lv_obj_set_size(btn1, 280, 70);     // Width, Height
```

Current layout (portrait 320x640):
- Button 1 (PNG): (20, 40) - 280x70px
- Button 2 (BMP): (20, 125) - 280x70px
- Button 3 (JPG): (20, 210) - 280x70px
- GIF Button: (20, 295) - 280x70px
- Toggle Button 1: (20, 380) - 280x70px
- Disabled Button: (20, 465) - 280x70px

### Font Sizes
Modify the font weight in `init_korean_fonts()` function:

```c
info_20.weight = 20;  // Title font size
info_16.weight = 16;  // Button/text font size
```

### Image Display
Images are loaded from the `assets/images/` directory using the POSIX filesystem driver:

```c
lv_img_set_src(img1, "A:assets/images/button_png.png");
```

Images are:
- 32x32 pixels each
- Stored in POSIX filesystem with "A:" prefix (LVGL filesystem driver letter)
- Automatically sized and displayed by LVGL image widget

### Colors
Button and text colors are defined using hex values in `create_buttons()`:

```c
lv_color_hex(0xFF9800)  // Orange (buttons)
lv_color_hex(0x2196F3)  // Blue (toggle ON)
lv_color_hex(0x757575)  // Gray (toggle OFF)
lv_color_hex(0x4CAF50)  // Green (click feedback)
```

## File Descriptions

### main.c
Main application file containing:
- SDL2 window and renderer setup
- LVGL initialization and display driver
- Input device (mouse) driver
- FreeType font initialization
- Button UI creation
- Event handlers for button interactions

### Makefile
Build configuration that:
- Compiles main.c with LVGL
- Links SDL2 and FreeType libraries
- Generates the `button` executable
- Provides `clean`, `distclean`, and `run` targets

### lv_conf.h
LVGL configuration file (for v9.4-dev with modifications for v8.4 compatibility)

### setup.sh
Initialization script that:
- Checks for required system packages
- Clones LVGL v8.4 repository
- Builds LVGL static library from source

## Troubleshooting

### "Failed to create SDL window"
- Ensure SDL2 development libraries are installed
- Check that your system supports graphics output

### "Failed to initialize FreeType"
- Verify FreeType2 libraries are installed: `libfreetype6-dev`
- Check that font files exist in `assets/fonts/`

### "Fonts not displaying properly"
- Ensure locale is set to UTF-8: `export LANG=en_US.UTF-8`
- Check that NanumGothicCoding.ttf files are in `assets/fonts/`

### "Images not displaying on buttons"
- Verify image files exist: `assets/images/button_png.png`, `button_bmp.bmp`, `button_jpg.jpg`
- Ensure libpng is installed for PNG support: `sudo apt-get install libpng-dev`
- If adding PNG support, rebuild LVGL: `bash setup.sh` then `make clean && make`
- Check that images are 32x32 pixels

### "PNG images specifically not displaying"
- Run `bash setup.sh` to rebuild LVGL with PNG decoder enabled
- Verify `LV_USE_LIBPNG 1` is set in `lv_conf.h`
- Ensure libpng16 development library is installed

### "Button clicks not registering"
- Verify SDL2 was built with mouse support
- Check that the window is focused

## Development

### Adding More Buttons
1. Create button object in `create_buttons()`
2. Assign event handler using `lv_obj_add_event_cb()`
3. Set style properties with `lv_obj_set_style_*()` functions

### Modifying Button Events
Edit the event handlers:
- `button_event_handler()` - for regular button clicks
- `toggle_button_event_handler()` - for toggle button state changes

### Using Different Fonts
1. Place font file in `assets/fonts/`
2. Update font path in `init_korean_fonts()`:
   ```c
   info_20.name = "assets/fonts/YourFont.ttf";
   ```

### Adding Static Images to Buttons
1. Place image file in `assets/images/`
2. Create an image widget and set its source:
   ```c
   lv_obj_t *img = lv_img_create(btn);
   lv_obj_set_flex_grow(img, 0);
   lv_img_set_src(img, "A:assets/images/your_image.png");
   ```
3. Supported formats: PNG, BMP, JPG
4. For PNG support, ensure `LV_USE_LIBPNG 1` in `lv_conf.h` and rebuild with `bash setup.sh`

### Adding Animated GIF to Buttons
1. Place GIF file in `assets/images/`
2. Create a GIF widget and set its source:
   ```c
   lv_obj_t *gif = lv_gif_create(btn);
   lv_obj_set_flex_grow(gif, 0);
   lv_gif_set_src(gif, "A:assets/images/your_image.gif");
   ```
3. LVGL 8.4 automatically handles GIF frame animations
4. GIF animation runs continuously until the object is deleted
5. Ensure `LV_USE_GIF 1` in `lv_conf.h` (enabled by default in LVGL 8.4)

## Performance Notes

- **Frame Rate**: SDL_DELAY(5ms) provides ~200 FPS target
- **Double Buffering**: Reduces flickering for smoother animations
- **FreeType Caching**: Font glyphs are cached in memory for faster rendering
- **Image Rendering**: Images are decoded once and cached by LVGL
- **Flex Layout**: Efficient layout calculation for button content alignment

## Image Assets

The project includes four sample images for demonstration:
- **button_png.png** - PNG image (32x32px, 8-bit RGB)
- **button_bmp.bmp** - BMP image (32x32px, 24-bit)
- **button_jpg.jpg** - JPEG image (32x32px, baseline)
- **button_gif.gif** - Animated GIF (32x32px, multiple frames)

All images are 32x32 pixels for consistency. The GIF file contains multiple frames that animate continuously. Replace these files with your own images as needed (maintaining the same filenames and 32x32px dimensions for best results).

## License

This project uses:
- **LVGL** - https://github.com/lvgl/lvgl (MIT License)
- **SDL2** - https://www.libsdl.org/ (Zlib License)
- **FreeType** - https://www.freetype.org/ (FreeType License)
- **libpng** - http://libpng.org (PNG License)
- **NanumGothic** - Korean font by NAVER

## References

- LVGL Documentation: https://docs.lvgl.io/8.4/
- SDL2 Documentation: https://wiki.libsdl.org/
- FreeType Documentation: https://www.freetype.org/freetype2/docs/

## Recent Updates

### v1.3.0 - GIF Animation Support
- **Added animated GIF button** - supports multiple frame animations with visual effects
- **GIF button event handler** - dedicated handler for GIF widget detection and effect application
- **Visual effects on GIF** - green border flash effect (200ms) with smooth color animation
- **Layout optimization** - GIF button positioned alongside image buttons
- **Reduced toggle buttons** - now featuring 1 toggle button with proper state management

### v1.2.0 - Visual Effects & Animation
- **Fixed image disappearing issue** - images now stay visible when clicked
- **Added green border flash effect** - 200ms duration for image buttons
- **Improved toggle button animations** - smooth color transitions
- **Safe animation implementation** - timer-based cleanup prevents visual artifacts
- **Simplified visual effects** - removed problematic transform animations
- **Enhanced user feedback** - reliable visual responses to button interactions

### v1.1.0 - Image Support
- Added support for PNG, BMP, and JPG image formats
- Implemented multi-format image decoders (LibPNG, TJPGD)
- Fixed image display issues with proper LVGL 8.4 API usage
- Increased button sizes to 70x280px for better usability
- Updated to portrait 320x640 display resolution
- Added POSIX filesystem driver for image loading

### v1.0.0 - Initial Release
- LVGL 8.4 integration with SDL2
- Korean font support with FreeType
- Interactive buttons with event handlers
- Toggle and disabled button states

## Author

Created with LVGL 8.4, SDL2, and FreeType support for Korean language GUI applications with image support.

## See Also

- Related project: `../Chunjiin/` - Korean input method example
- Related project: `../ImageButton/` - LVGL 9.x image button demo (newer version)
- LVGL Examples: `lvgl/examples/`
- LVGL Documentation: https://docs.lvgl.io/8.4/

---

For questions or issues, refer to the LVGL documentation or modify the source code in `main.c`.
