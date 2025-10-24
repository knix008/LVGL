# LVGL Image Button Demo Application

A demonstration application showcasing LVGL image buttons with different image formats (PNG, JPG, GIF, BMP). This project demonstrates how to create interactive image buttons using the LVGL graphics library.

## Features

- **Multiple Image Formats**: Demonstrates PNG, JPG, GIF, and BMP image formats
- **Interactive Buttons**: Click buttons to see format information
- **Modern GUI**: Clean, responsive interface with SDL backend
- **Cross-platform**: Works on Linux with SDL2
- **Educational**: Shows best practices for LVGL image button implementation

## Image Formats Demonstrated

| Format | Description | Features |
|--------|-------------|----------|
| **PNG** | Portable Network Graphics | Transparency support, lossless compression |
| **JPG** | JPEG | Lossy compression, small file sizes |
| **GIF** | Graphics Interchange Format | Animation support, limited colors |
| **BMP** | Bitmap | Uncompressed, simple format |

## Project Structure

```
ImageButton/
├── assets/
│   └── images/           # Sample images for demonstration
│       ├── button_png.png
│       ├── button_jpg.jpg
│       ├── button_gif.gif
│       └── button_bmp.bmp
├── lvgl/                 # LVGL library (created by setup)
├── lv_conf.h            # LVGL configuration
├── main.c               # Application entry point
├── image_button_app.c   # GUI implementation
├── image_button_app.h   # Header file
├── Makefile            # Simple build system
├── setup.sh            # LVGL setup script
└── README.md           # This file
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

## Customization

### Adding New Image Formats
1. Add image files to `assets/images/`
2. Update the `buttons` array in `image_button_app.c`
3. Rebuild the application

### Modifying Button Layout
Edit the button creation code in `image_button_app_init()` function:
- Change button positions
- Modify button sizes
- Adjust spacing and layout

### Styling
Modify the style properties in the source code:
- Colors: `lv_color_hex()`
- Fonts: `lv_font_montserrat_*`
- Padding: `lv_obj_set_style_pad_*()`

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
