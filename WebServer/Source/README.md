# LVGL WebServer Project

A comprehensive multimedia application built with LVGL (Light and Versatile Graphics Library) that combines a GUI with a web server backend. The application provides a tabbed interface for various features including video playback, Korean input methods, calendar, clock, and database management.

## Features

### GUI Components (LVGL-based)
- **Tab Menu Interface**: Multi-tab UI for easy navigation between different features
- **Video Playback**: Video player with FFmpeg support
- **Audio Playback**: Audio streaming and playback capabilities
- **Calendar Widget**: Interactive calendar with date selection
- **Clock Display**: Real-time clock display
- **Korean Input Methods**: Multiple Korean keyboard layouts
  - Chunjiin Input Method
  - Qwerty Korean Layout
- **Database Management**: SQLite database viewer and manager
- **Settings Tab**: Application configuration interface

### Backend Features
- **Web Server**: Lightweight HTTP server built with Mongoose
- **RESTful API**: Web interface at `http://localhost:8080`
- **SQLCipher**: Encrypted SQLite database support
- **SSL/TLS Support**: OpenSSL integration for secure communications
- **FreeType Font Support**: Advanced font rendering for multilingual text

## System Requirements

### Dependencies
- SDL2 (Simple DirectMedia Layer)
- FreeType2
- OpenSSL
- FFmpeg (libavformat, libavcodec, libavutil, libswscale)
- PkgConfig
- CMake 3.10 or higher

### Build Environment
- C99 compatible compiler (GCC, Clang, etc.)
- C++14 compatible compiler
- Linux/Unix-like operating system

## Installation

### Install Dependencies (Ubuntu/Debian)
```bash
sudo apt-get install -y \
    libsdl2-dev \
    libfreetype6-dev \
    libssl-dev \
    libavformat-dev \
    libavcodec-dev \
    libavutil-dev \
    libswscale-dev \
    pkg-config \
    cmake \
    build-essential
```

## Building

### 1. Create and Enter Build Directory
```bash
cd /path/to/WebServer/Source
mkdir -p build
cd build
```

### 2. Configure with CMake
```bash
cmake ..
```

### 3. Build the Project
```bash
make
```

The executable will be created at `Source/build/main`.

### 4. Optional: Build Test Executables
Test executables are automatically built alongside the main application:
- `test_chunjiin` - Test Chunjiin input method
- `calendar_test` - Test calendar widget
- `clock_test` - Test clock display
- `qwerty_korean_test` - Test Qwerty Korean layout
- `video_test` - Test video playback

## Running the Application

### Start the Application
```bash
./main
```

The application will:
1. Initialize an 800x600 SDL window with the title "Video Input Demo"
2. Load and initialize all subsystems (FreeType, FFmpeg, Web Server)
3. Display the tabbed GUI interface
4. Start the web server on `http://localhost:8080`
5. Accept keyboard, mouse, and mousewheel input

### Accessing the Web Interface
Open your web browser and navigate to:
```
http://localhost:8080
```

### Exiting the Application
Press `Ctrl+C` in the terminal to gracefully shut down the application.

## Project Structure

```
Source/
├── CMakeLists.txt              # CMake build configuration
├── lv_conf.h                   # LVGL configuration
├── .gitignore                  # Git ignore rules
├── src/                        # Application source code
│   ├── main.c                  # Main application entry point
│   ├── ui_components.c         # GUI component definitions
│   ├── web_server.c            # Web server implementation
│   ├── web_control.c           # Web API handlers
│   ├── tab_*.c                 # Individual tab implementations
│   ├── korean_input.c          # Korean input handling
│   ├── chunjiin_input.c        # Chunjiin input method
│   ├── qwerty_korean.c         # Qwerty Korean layout
│   ├── calendar.c              # Calendar widget
│   ├── clock.c                 # Clock display
│   ├── audio.c                 # Audio playback
│   ├── video.c                 # Video playback
│   ├── mongoose.c              # HTTP server library
│   └── sqlite_test.c           # SQLite/SQLCipher examples
├── html/                       # Web interface files
│   ├── index.html              # Main web page
│   ├── styles.css              # CSS styling
│   └── script.js               # JavaScript functionality
├── include/                    # Header files
├── lvgl/                       # LVGL library (submodule or directory)
├── sqlcipher/                  # SQLCipher library
└── onnxruntime/                # ONNX Runtime for ML support
```

## Configuration

### LVGL Configuration
LVGL settings are configured in `lv_conf.h` located in the `Source` directory. Key settings include:
- Color depth: 32-bit (XRGB8888)
- Display resolution: Configured via SDL window size
- Disabled features: Vector Graphics (ThorVG), SVG support
- Enabled libraries: FreeType, FFmpeg

### Web Server Configuration
The web server runs on port 8080 by default. Modify `web_server.c` to change the port or other settings.

### Database Configuration
SQLite databases are encrypted using SQLCipher with OpenSSL. Database files are typically stored in the application's data directory.

## Build Notes

### ThorVG Support
ThorVG (vector graphics library) support has been disabled in this build due to missing configuration dependencies. To re-enable it, create a `config.h` file in the ThorVG library directory and ensure all ThorVG dependencies are present.

### SVG Support
SVG vector graphics support is currently disabled. To enable it, ensure LV_USE_VECTOR_GRAPHIC and LV_USE_SVG are set to 1 in `lv_conf.h` and the SVG example file is included in the build.

## Performance Optimization

The main application loop uses a 5ms sleep interval for a refresh rate of ~200 Hz. Adjust the `usleep(5000)` value in `main.c` to optimize performance based on your system's capabilities.

## Development

### Adding New Features

1. **New Tab**: Create a new `tab_*.c` file and add it to `CMakeLists.txt`
2. **New Input Method**: Add to the input handling section in `main.c`
3. **New API Endpoint**: Modify `web_control.c` to add new routes

### Code Organization
- Keep UI logic in separate `tab_*.c` files
- Web API handlers go in `web_control.c`
- Utility functions in `src/` with corresponding headers in `include/`

## Troubleshooting

### Build Errors
- **SDL2 not found**: Install SDL2-devel: `sudo apt-get install libsdl2-dev`
- **FreeType not found**: Install FreeType-devel: `sudo apt-get install libfreetype6-dev`
- **FFmpeg not found**: Install FFmpeg development packages

### Runtime Issues
- **Window not appearing**: Ensure SDL2 is properly installed and X11/Wayland is available
- **Web server not responding**: Check if port 8080 is already in use
- **Font rendering issues**: Verify FreeType initialization in the console output

## Git Configuration

The `.gitignore` file is configured to exclude:
- Build artifacts (`build/`, `*.o`, `*.a`, `*.so`)
- CMake generated files
- IDE configuration files (`.vscode/`, `.idea/`)
- Test scripts
- Data directories (`models/`, `certs/`)

## License

This project uses several open-source libraries:
- **LVGL**: MIT License
- **SQLCipher**: BSD License (with OpenSSL integration)
- **Mongoose**: Dual licensing (GPLv2 and Commercial)
- **FreeType**: FreeType License
- **FFmpeg**: LGPL License

Ensure compliance with all library licenses when using or distributing this project.

## Support

For issues or questions:
1. Check the console output for error messages
2. Review the LVGL documentation at https://docs.lvgl.io
3. Check FFmpeg documentation for video/audio codec issues
4. Review Mongoose documentation for web server configuration

## Future Enhancements

- [ ] Enable ThorVG vector graphics support
- [ ] Add more Korean input methods
- [ ] Implement WebSocket support for real-time updates
- [ ] Add machine learning inference (ONNX Runtime)
- [ ] Improve video codec support
- [ ] Add internationalization (i18n) support
