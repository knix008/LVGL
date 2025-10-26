# Number Input System with LVGL 8.4

A simple and functional number input system built with LVGL 8.4 for a 320x640 window resolution. This application provides a clean interface for entering numbers with a virtual keypad.

## Features

- **Number Keypad**: 0-9 digits with clear (지우기) and backspace (←) buttons
- **Display**: Shows the entered numbers in real-time with top-left alignment
- **Enter Function**: Shows popup dialog with entered number and clears the text box
- **Custom UI**: Dodger blue buttons with white text that turn light orange when clicked
- **Korean Font Support**: Uses NanumGothicCoding-Bold fonts for Korean text display
- **Responsive**: Real-time text updates as you type

## Screenshots

The application displays:
- A title "숫자 입력 시스템" (Number Input System in Korean)
- A text display area showing entered numbers
- A 4x4 keypad with numbers 0-9, Clear (지우기), Backspace (←), and Enter (입력) buttons

## Requirements

- LVGL 8.4 library
- SDL2 development libraries
- FreeType library
- GCC compiler with C99 support
- Make

## Installation

### Ubuntu/Debian
```bash
sudo apt update
sudo apt install libsdl2-dev libfreetype-dev build-essential
```

### CentOS/RHEL/Fedora
```bash
sudo yum install SDL2-devel freetype-devel gcc make
# or for newer versions:
sudo dnf install SDL2-devel freetype-devel gcc make
```

### macOS
```bash
brew install sdl2 freetype
```

## Building

The project uses a simple Makefile for building:

```bash
# Build the application
make

# Build with debug symbols
make debug

# Build optimized release version
make release

# Clean build artifacts
make clean

# Clean everything including LVGL library
make distclean

# Show available targets
make help
```

## Running

After building, the executable will be created in the project root:

```bash
# Run the application
./number
```

The application will open a 320x640 window with the number input interface.

## Usage

1. **Enter Numbers**: Click on the number buttons (0-9) to input digits
2. **Clear**: Press '지우기' to clear the entire input and reset to "0"
3. **Backspace**: Press '←' to delete the last entered digit
4. **Enter**: Press '입력' to show a popup dialog with the entered number and clear the text box
5. **Exit**: Close the window or press Escape to exit

## Project Structure

```
Button8.4/
├── main.c              # Main application source
├── Makefile           # Build configuration
├── lv_conf.h          # LVGL configuration
├── lvgl/              # LVGL 8.4 library
│   ├── src/           # LVGL source files
│   └── lib/           # Pre-built LVGL library
├── number              # Executable (after build)
├── .gitignore         # Git ignore file
└── README.md          # This file
```

## Technical Details

### Architecture
- **Display**: SDL2 window with LVGL rendering
- **Input**: Mouse/touch input through SDL2
- **Rendering**: Double-buffered LVGL rendering to SDL texture
- **UI**: LVGL button matrix for keypad interface

### Key Components
- **Display Driver**: Custom SDL2 flush callback for rendering
- **Input Driver**: Mouse state reading for button interactions
- **Button Matrix**: LVGL button matrix with custom callback handling
- **Text Display**: LVGL label with left-aligned text
- **Popup Dialog**: Custom dialog with Korean font support for displaying entered numbers
- **Korean Fonts**: FreeType-based Korean font rendering with NanumGothicCoding-Bold

### Configuration
- **Resolution**: 320x640 pixels
- **Color Depth**: 32-bit ARGB8888
- **Buffer Size**: 10% of screen resolution for double buffering
- **Font**: Default LVGL Montserrat font

## Customization

### Window Resolution
To change the resolution, modify these constants in `main.c`:
```c
#define DISP_HOR_RES 320
#define DISP_VER_RES 640
```

### Keypad Layout
The keypad layout is defined in the `btnm_map` array:
```c
static const char *btnm_map[] = {
    "1", "2", "3", "\n",
    "4", "5", "6", "\n", 
    "7", "8", "9", "\n",
    "지우기", "0", "←", "\n",
    "입력", ""
};
```

### Button Styling
The application uses custom button colors:
- **Default**: Dodger blue (#1E90FF) with white text
- **Pressed**: Light orange (#FFB366) with white text
- **Focused**: Dodger blue (#1E90FF) with white text
- **Focused + Pressed**: Light orange (#FFB366) with white text

To customize further:
1. Modify the color values in the `create_number_input_ui()` function
2. Use `lv_obj_set_style_*()` functions for additional styling

### Text Processing
The `button_matrix_cb()` function handles all button interactions. To modify behavior:
1. Edit the callback function in `main.c`
2. Add custom logic for number processing
3. Modify the enter button behavior

## Troubleshooting

### Build Issues
- **Missing SDL2**: Install SDL2 development libraries
- **Missing FreeType**: Install FreeType development libraries
- **LVGL not found**: Ensure `lvgl/lib/liblvgl.a` exists
- **Compilation errors**: Check GCC version (C99 support required)

### Runtime Issues
- **Window doesn't appear**: Check SDL2 installation
- **No input response**: Verify mouse/touch input is working
- **Segmentation fault**: Ensure proper LVGL initialization
- **Text not updating**: Check button callback registration

### Performance
- **Slow rendering**: Reduce buffer size or disable double buffering
- **High CPU usage**: Increase SDL_Delay() value in main loop
- **Memory issues**: Check LVGL memory configuration in `lv_conf.h`

## Development

### Adding Features
1. **New Button Types**: Add to `btnm_map` and handle in `button_matrix_cb()`
2. **Custom Styling**: Use LVGL style functions in `create_number_input_ui()`
3. **Input Validation**: Add validation logic in the callback function
4. **File Operations**: Add file I/O for saving/loading numbers

### Debugging
- **Debug Output**: Uncomment printf statements in callback functions
- **LVGL Logging**: Enable logging in `lv_conf.h`
- **SDL Debug**: Use SDL debug flags for input/display issues

## License

This project uses LVGL which is licensed under the MIT License. See the LVGL license file for details.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## Changelog

### Version 1.0
- Initial release
- Basic number input functionality
- SDL2 + LVGL integration
- Default button styling
- Top-left text alignment

## Support

For issues and questions:
1. Check the troubleshooting section
2. Review the LVGL documentation
3. Check SDL2 documentation for display issues
4. Create an issue in the project repository