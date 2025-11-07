# LVGL Image Button Demo Application

A fully interactive demonstration application showcasing image buttons with multiple image formats (PNG, JPG, GIF, BMP) using LVGL (Light and Versatile Graphics Library). Features clickable buttons with images as labels, visual feedback on interactions, and configurable sizing.

## Features

✨ **Multi-Format Image Support**
- PNG images with transparency support (tested & working)
- JPG/JPEG images with compression (tested & working)
- GIF images with animation support (tested & working)
- BMP bitmap images (tested & working)

🎨 **Interactive Image Buttons**
- Clickable buttons with image labels and text
- Visual feedback on button press/release (image dimming to 70% opacity)
- Displays detailed image information when clicked
- Text labels displayed next to images

⚙️ **Configurable Button & Image Sizing**
- Easy-to-adjust button height (40-80px recommended)
- Automatic image widget sizing based on button height
- Configurable image scale percentage
- Images automatically rescaled via LVGL's image scaling

🎯 **Responsive UI**
- Flex layout for automatic element positioning
- Centered buttons on screen
- Information display panel with clicked button details
- Status label showing last clicked button

## Image Format Details

| Format | File Size | Image Size | Features |
|--------|-----------|-----------|----------|
| **PNG** | ~11.5 KB | 32x32 px | Lossless compression, transparency support |
| **JPG** | ~1.1 KB | 32x32 px | Lossy compression, smallest file size |
| **GIF** | ~1.1 KB | 32x32 px | Animation support, auto-play |
| **BMP** | ~3.2 KB | 32x32 px | Uncompressed, simple format |

## Project Structure

```
ImageButton/
├── assets/
│   ├── images/                    # Image button assets
│   │   ├── button_png.png        # PNG image (32x32)
│   │   ├── button_jpg.jpg        # JPG image (32x32)
│   │   ├── button_gif.gif        # GIF animated image (32x32)
│   │   └── button_bmp.bmp        # BMP image (32x32)
│   └── fonts/
│       └── NanumGothicCoding-Bold.ttf  # Custom font (16px, 12px)
├── lvgl/                          # LVGL library (created by setup.sh)
├── lv_conf.h                     # LVGL configuration
├── main.c                        # Application entry point with SDL initialization
├── image_button_app.c            # Button UI implementation and event handlers
├── image_button_app.h            # Header file with function declarations
├── Makefile                      # Build system
├── setup.sh                      # LVGL setup and build script
└── README.md                     # This documentation
```

## Prerequisites

- **Linux** (Ubuntu/Debian recommended)
- **GCC** compiler
- **SDL2** development libraries
- **FreeType** development libraries
- **Python3** with PIL (for generating sample images)

### Installing Dependencies

On Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install build-essential git libsdl2-dev libfreetype6-dev pkg-config python3-pil
```

## Quick Start

### 1. Setup LVGL
```bash
./setup.sh
```
This will:
- Check system requirements
- Clone and build LVGL library
- Verify all dependencies

### 2. Build the Application
```bash
make
```

### 3. Run the Application
```bash
make run
```
Or:
```bash
./image_button_app
```

## Build System

This project uses a simple Makefile-based build system instead of CMake:

### Available Make Targets

```bash
make              # Build the application (default)
make clean        # Remove build artifacts
make distclean    # Remove all build files including LVGL
make run          # Build and run the application
make install-deps # Install required system dependencies
make help         # Show all available targets
```

### Build Process

The Makefile:
- Compiles main application sources (`main.c`, `image_button_app.c`)
- Compiles SDL driver sources from LVGL
- Links against LVGL library and system libraries (SDL2, FreeType)
- Creates executable: `image_button_app`

## Application Controls

- **Mouse**: Click image buttons to see format information
- **Keyboard**: 
  - `Q` or `Escape`: Quit the application
  - Use mouse to interact with buttons

## Technical Details

### LVGL Configuration
The application uses a custom `lv_conf.h` with the following key features:
- SDL2 backend for desktop development
- Support for PNG, JPG, GIF, and BMP image formats
- FreeType integration for font rendering
- File system support for loading images
- Image button widget enabled

### Image Loading
Images are loaded using LVGL's file system interface:
- Images are stored in `assets/images/`
- File system driver letter: `A:`
- Supported formats: PNG, JPG, GIF, BMP

### Code Structure
- **`main.c`**: Application entry point and SDL integration
- **`image_button_app.c`**: GUI implementation and button handling
- **`image_button_app.h`**: Header file with function declarations

## Configuration & Customization

### Button Sizing Configuration

Edit `image_button_app.c` (lines 14-35) to adjust button and image sizes:

```c
// Easy-to-adjust button size settings
#define BUTTON_WIDTH    180   // Button width in pixels
#define BUTTON_HEIGHT   50    // Button height in pixels

// Image scale percentage (as percentage of the calculated widget size)
#define IMAGE_SCALE_PERCENT   100   // Image as percentage of widget size
```

**Size Examples:**
- `BUTTON_HEIGHT 40` → 32x32 image display area (compact)
- `BUTTON_HEIGHT 50` → 42x42 image display area (current - balanced)
- `BUTTON_HEIGHT 60` → 52x52 image display area (spacious)
- `BUTTON_HEIGHT 80` → 72x72 image display area (large)

**Scale Examples:**
- `50` = images at 50% of widget size (small images, lots of padding)
- `75` = images at 75% of widget size (medium images)
- `100` = images fill entire widget size (full/largest) ← current

### Adding New Image Buttons

1. Prepare images (32x32 pixels recommended)
2. Place in `assets/images/`
3. Update the `buttons[]` array in `image_button_app.c`:

```c
static const button_info_t buttons[] = {
    {"PNG Button", "A:assets/images/button_png.png", "PNG format description", false},
    {"JPG Button", "A:assets/images/button_jpg.jpg", "JPG format description", false},
    {"GIF Button", "A:assets/images/button_gif.gif", "GIF format description", true},
    {"BMP Button", "A:assets/images/button_bmp.bmp", "BMP format description", false},
    // Add new buttons here
    // Set the last parameter to 'true' for GIF buttons, 'false' for static images
};
```

4. Rebuild: `make clean && make`

### Modifying Button Layout

Edit the flex layout settings in `create_image_button()`:
- Change button styling (colors, borders, radius)
- Adjust padding between image and label
- Modify alignment and spacing

### Customizing Image Behavior

Edit event handlers in `image_button_app.c`:
- `button_image_press_handler()` - Controls image opacity on press/release
- `button_click_handler()` - Updates info display on click
- Modify opacity values (currently 70% on press, 100% on release)

## Troubleshooting

### Common Issues

1. **"SDL2 not found"**
   ```bash
   sudo apt-get install libsdl2-dev
   ```

2. **"FreeType not found"**
   ```bash
   sudo apt-get install libfreetype6-dev
   ```

3. **"LVGL library not found"**
   ```bash
   ./setup.sh
   ```

4. **"Images not loading"**
   - Check that image files exist in `assets/images/`
   - Verify file permissions
   - Check LVGL file system configuration

5. **Build errors**
   ```bash
   make clean
   make
   ```

### Debug Mode
Build in debug mode for more detailed error information:
```bash
make clean
CFLAGS="-g -O0" make
```

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## License

This project is for educational purposes and demonstrates LVGL image button usage.

## Resources

- [LVGL Documentation](https://docs.lvgl.io/)
- [LVGL Examples](https://github.com/lvgl/lvgl)
- [SDL2 Documentation](https://wiki.libsdl.org/)
- [Make Documentation](https://www.gnu.org/software/make/manual/)

## .gitignore

A sample `.gitignore` is provided to help keep your repository clean. It ignores:
- Build artifacts (e.g., `build/`, `*.o`, `*.elf`, `*.bin`)
- LVGL library folder (`lvgl/`)
- Application binary (`image_button_app`)
- Common editor files (`.vscode/`, `.idea/`, `*.swp`, `*~`, `.DS_Store`, `Thumbs.db`)

You can customize `.gitignore` as needed for your workflow.
