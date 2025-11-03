# LVGL 8.4 Circle Button Demo with Korean Font Support

A feature-rich button demo application built with **LVGL 8.4** and **SDL2**, featuring circular bitmap buttons with press effects and Korean language support using TrueType fonts.

## Overview

This project demonstrates:
- LVGL 8.4 GUI framework integration
- SDL2-based rendering and input handling
- FreeType font loading for Korean text support
- Circular bitmap buttons with visual press effects
- Interactive button widgets with image display
- Multi-format image support (PNG, BMP, JPG)

## Features

### UI Components
- **3 Interactive Buttons with Images**: Orange buttons (280x70px) with images and Korean labels
  - Button 1: PNG image (button_png.png) - "PNG 버튼"
  - Button 2: BMP image (button_bmp.bmp) - "BMP 버튼"
  - Button 3: JPG image (button_jpg.jpg) - "JPG 버튼"
- **3 Circular Bitmap Buttons**: Circular image-based buttons with press effects
  - Red circle button (circle_red.bmp)
  - Green circle button (circle_green.bmp)
  - Blue circle button (circle_blue.bmp)
- **Korean UI Text**: All text displayed in Korean using NanumGothicCoding font
- **Title Label**: "LVGL 8.4 버튼 데모"
- **Display Resolution**: 320x640 portrait orientation
- **Light Gray Background**: 0xF5F5F5 for a clean, modern look

### Circular Button Features
- **Dual-State Images**: Each button has normal and pressed state images
  - Normal state: 60x60 pixel circular images
  - Pressed state: 63x63 pixel circular images (5% larger)
- **Center-Aligned Zoom**: Buttons expand from center when pressed
- **Smooth Transitions**: Instant image swap with position adjustment
- **Visual Feedback**: Clear press indication without excessive animation
- **Transparent Corners**: Properly masked circular images with light gray background

### Technical Features
- **Multi-Format Image Support**: PNG, BMP, and JPG images
- **Image Decoders**: LibPNG for PNG, native BMP decoder, SJPG for JPG
- **Flex Layout**: Buttons use flex layout for proper image and label alignment
- **Double Buffering**: Smooth rendering with LVGL's display buffer
- **Mouse Input**: Real-time mouse click detection and handling
- **FreeType Integration**: Dynamic TrueType font loading at runtime
- **Event Handling**: Proper LVGL event callbacks for button interactions (PRESSED, RELEASED, CLICKED)
- **SDL2 Backend**: Cross-platform display and input
- **Python Image Generation**: Automated circular bitmap creation script

## Project Structure

```
CircleButton/
├── main.c                          # Main application source code
├── Makefile                        # Build configuration
├── README.md                       # This file
├── .gitignore                      # Git ignore rules
├── lv_conf.h                       # LVGL configuration
├── setup.sh                        # Setup script for dependencies
├── create_circle_bitmaps.py        # Python script to generate circular images
├── lvgl/                           # LVGL library (v8.4) - cloned by setup.sh
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
        ├── circle_red.bmp          # Red circle normal (60x60)
        ├── circle_red_pressed.bmp  # Red circle pressed (63x63)
        ├── circle_green.bmp        # Green circle normal (60x60)
        ├── circle_green_pressed.bmp # Green circle pressed (63x63)
        ├── circle_blue.bmp         # Blue circle normal (60x60)
        ├── circle_blue_pressed.bmp # Blue circle pressed (63x63)
        └── *.png                   # PNG versions (with transparency)
```

## Prerequisites

### System Requirements
- **Linux** (Ubuntu 18.04 or later recommended)
- **GCC** compiler
- **SDL2** development libraries
- **FreeType2** development libraries
- **libpng** development libraries (for PNG image support)
- **Python 3** with Pillow (PIL) for image generation
- **Git** (for cloning LVGL)

### Install Dependencies

```bash
sudo apt-get update
sudo apt-get install -y build-essential libsdl2-dev libfreetype6-dev libpng-dev git python3 python3-pip
pip3 install Pillow
```

## Building

### 1. Generate Circular Images

First, generate the circular bitmap images:

```bash
python3 create_circle_bitmaps.py
```

This will create:
- 6 BMP files (normal + pressed states for 3 colors)
- 6 PNG files (with transparency)

### 2. Initial Setup

Run the setup script to prepare the LVGL library:

```bash
./setup.sh
```

This script will:
- Check system dependencies
- Verify Korean font files exist
- Clone LVGL v8.4 if not already present
- Build the LVGL static library

### 3. Build the Application

```bash
make
```

Or clean and rebuild:

```bash
make clean && make
```

### 4. Run the Application

```bash
./button_demo
```

Or use the Makefile target:

```bash
make run
```

## Usage

### Interaction
- **Click Regular Buttons**:
  - Orange buttons with images and Korean labels
  - Console output: "Button clicked!"
- **Click Circle Buttons**:
  - Circular buttons expand slightly when pressed (60px → 63px)
  - Center-aligned zoom effect
  - Console output: "Circle button clicked!"
- **Long Press**: Hold any button for long press detection
  - Console output: "Button long pressed!"

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
Button positions are defined in `create_buttons()`:

```c
// Regular buttons
lv_obj_set_pos(btn1, 20, 100);      // Button 1 at Y=100
lv_obj_set_pos(btn2, 20, 185);      // Button 2 at Y=185
lv_obj_set_pos(btn3, 20, 270);      // Button 3 at Y=270

// Circle buttons (equally spaced)
lv_obj_set_pos(circle_btn1, 30, 450);   // Red at X=30
lv_obj_set_pos(circle_btn2, 130, 450);  // Green at X=130
lv_obj_set_pos(circle_btn3, 230, 450);  // Blue at X=230
```

### Circular Button Customization

To modify circular button appearance, edit `create_circle_bitmaps.py`:

```python
# Normal and pressed sizes
size_normal = 60   # Normal state diameter
size_pressed = 63  # Pressed state diameter (5% larger)

# Colors (RGB tuples)
(233, 30, 99)   # Red/Pink - 0xE91E63
(76, 175, 80)   # Green - 0x4CAF50
(33, 150, 243)  # Blue - 0x2196F3

# Border
(51, 51, 51)    # Border color - 0x333333
border_width = 2  # Border width in pixels
```

After modifying, regenerate images:

```bash
python3 create_circle_bitmaps.py
```

### Font Sizes
Modify the font weight in `init_korean_fonts()` function:

```c
info_20.weight = 20;  // Title font size
info_16.weight = 16;  // Button/text font size
```

### Colors
Button and text colors are defined using hex values:

```c
lv_color_hex(0xF5F5F5)  // Light gray (background)
lv_color_hex(0xFF9800)  // Orange (regular buttons)
lv_color_hex(0x333333)  // Dark gray (borders)
lv_color_hex(0xFFFFFF)  // White (text on buttons)
lv_color_hex(0x000000)  // Black (title text)
```

## File Descriptions

### main.c
Main application file containing:
- SDL2 window and renderer setup
- LVGL initialization and display driver
- Input device (mouse) driver
- FreeType font initialization
- Button UI creation with regular and circular buttons
- Event handlers for button interactions (PRESSED, RELEASED, CLICKED)
- Image swapping logic for circular buttons

### create_circle_bitmaps.py
Python script to generate circular bitmap images:
- Creates both PNG (with transparency) and BMP (with light gray background)
- Generates normal and pressed state images
- Applies circular mask for perfect circles
- Draws borders and fills with specified colors

### Makefile
Build configuration that:
- Compiles main.c with LVGL
- Links SDL2, FreeType, and libpng libraries
- Generates the `button_demo` executable
- Provides `clean`, `distclean`, and `run` targets

### lv_conf.h
LVGL configuration file enabling:
- FreeType font support
- PNG, BMP, and JPG decoders
- Flex layout system
- Display and input drivers

### setup.sh
Initialization script that:
- Checks for required system packages
- Verifies Korean font files
- Clones LVGL v8.4 repository
- Builds LVGL static library from source

## Troubleshooting

### "Failed to create SDL window"
- Ensure SDL2 development libraries are installed
- Check that your system supports graphics output

### "Failed to initialize FreeType"
- Verify FreeType2 libraries are installed: `libfreetype6-dev`
- Check that font files exist in `assets/fonts/`

### "Font file not found"
- Ensure `NanumGothicCoding.ttf` and `NanumGothicCoding-Bold.ttf` are in `assets/fonts/`
- The setup script checks for these files automatically

### "Circle button images not displaying"
- Run the image generation script: `python3 create_circle_bitmaps.py`
- Check that all 6 BMP files exist in `assets/images/`
- Verify light gray background matches screen (0xF5F5F5)

### "Python Pillow not installed"
- Install Pillow: `pip3 install Pillow`
- Or system package: `sudo apt-get install python3-pil`

### "Circle buttons not pressing correctly"
- Ensure both normal and pressed images exist
- Check that event handler is registered with `LV_EVENT_ALL`
- Verify position offset calculation (-1 for press, +1 for release)

## Development

### Adding More Circular Buttons
1. Generate new circular images with `create_circle_bitmaps.py`
2. Add button creation in `create_buttons()`:
   ```c
   lv_obj_t *circle_btn = lv_img_create(scr);
   lv_img_set_src(circle_btn, "A:assets/images/your_circle.bmp");
   lv_obj_set_pos(circle_btn, x, y);
   lv_obj_add_flag(circle_btn, LV_OBJ_FLAG_CLICKABLE);
   lv_obj_add_event_cb(circle_btn, circle_button_event_handler, LV_EVENT_ALL, NULL);
   ```
3. Update event handler to recognize new button images

### Modifying Button Events
Edit the event handlers:
- `button_event_handler()` - for regular button clicks
- `circle_button_event_handler()` - for circular button press/release/click

### Creating Custom Circular Images
Modify `create_circle_bitmaps.py`:
1. Adjust `size_normal` and `size_pressed` for different sizes
2. Change color tuples for different colors
3. Modify border width and color
4. Run script to generate new images

## Performance Notes

- **Frame Rate**: SDL_DELAY(5ms) provides ~200 FPS target
- **Double Buffering**: Reduces flickering for smoother animations
- **FreeType Caching**: Font glyphs are cached in memory
- **Image Caching**: LVGL caches decoded images
- **Instant Press Effect**: No animation delays, immediate visual feedback
- **Minimal Image Swapping**: Only swaps images, no complex transformations

## Image Assets

### Regular Button Images
- **button_png.png** - PNG image (32x32px, 8-bit RGB)
- **button_bmp.bmp** - BMP image (32x32px, 24-bit)
- **button_jpg.jpg** - JPEG image (32x32px, baseline)

### Circular Button Images
Generated by `create_circle_bitmaps.py`:
- **Normal state**: 60x60 pixels
  - circle_red.bmp / .png
  - circle_green.bmp / .png
  - circle_blue.bmp / .png
- **Pressed state**: 63x63 pixels (5% larger)
  - circle_red_pressed.bmp / .png
  - circle_green_pressed.bmp / .png
  - circle_blue_pressed.bmp / .png

All circular images feature:
- Perfect circular shape with transparent corners (PNG) or light gray background (BMP)
- 2-pixel dark gray border (0x333333)
- Solid color fills matching Material Design palette

## License

This project uses:
- **LVGL** - https://github.com/lvgl/lvgl (MIT License)
- **SDL2** - https://www.libsdl.org/ (Zlib License)
- **FreeType** - https://www.freetype.org/ (FreeType License)
- **libpng** - http://libpng.org (PNG License)
- **NanumGothic** - Korean font by NAVER
- **Python Pillow** - https://python-pillow.org/ (PIL License)

## References

- LVGL Documentation: https://docs.lvgl.io/8.4/
- SDL2 Documentation: https://wiki.libsdl.org/
- FreeType Documentation: https://www.freetype.org/freetype2/docs/
- Pillow Documentation: https://pillow.readthedocs.io/

## Recent Updates

### v2.0.0 - Circular Bitmap Buttons
- **Added circular bitmap buttons** - Three circular image-based buttons (red, green, blue)
- **Dual-state images** - Normal (60x60) and pressed (63x63) states for each button
- **Center-aligned press effect** - Buttons expand from center when pressed
- **Python image generator** - Automated script to create circular BMP/PNG images
- **Position-aware press handling** - Maintains button center during size change
- **Removed excessive animations** - Clean, simple press feedback
- **Updated layout** - Regular buttons at top, circular buttons at bottom
- **Light gray background** - Modern, clean look (0xF5F5F5)

### v1.2.0 - Visual Effects Cleanup
- Fixed image disappearing issue
- Simplified visual effects
- Enhanced user feedback

### v1.1.0 - Image Support
- Added PNG, BMP, and JPG support
- Implemented multi-format decoders
- Portrait display orientation

### v1.0.0 - Initial Release
- LVGL 8.4 integration
- Korean font support
- Basic button interactions

## Author

Created with LVGL 8.4, SDL2, FreeType, and Python Pillow for demonstrating circular bitmap buttons with press effects in Korean language GUI applications.

## See Also

- LVGL Examples: `lvgl/examples/`
- LVGL Documentation: https://docs.lvgl.io/8.4/
- Material Design Colors: https://materialui.co/colors

---

For questions or issues, refer to the LVGL documentation or modify the source code in `main.c`.
