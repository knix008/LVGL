# LVGL WebServer Application

A comprehensive multimedia application built with LVGL (Light and Versatile Graphics Library) that combines a modern GUI with an embedded web server backend. The application provides a tabbed interface for video playback, Korean input methods, calendar, clock, and database management, along with a lightweight HTTP server for remote access.

## Features

### GUI Components
- **LVGL GUI**: Rich graphical user interface with multiple tabs
- **Tab Menu Interface**: Easy navigation between different features
- **Video Playback**: FFmpeg-based video player with codec support
- **Audio Support**: Audio streaming and playback capabilities
- **Calendar Widget**: Interactive date selection and calendar view
- **Clock Display**: Real-time clock display
- **Korean Input Methods**: Multiple Korean keyboard layouts
  - ChunJiIn Input Method
  - QWERTY Korean Layout
- **Database Management**: SQLCipher encrypted database viewer

### Backend Features
- **Embedded Web Server**: Mongoose-based HTTP server (port 8080)
- **Web Interface**: Browser-accessible control panel
- **Database Integration**: SQLCipher with OpenSSL encryption
- **TLS 1.3 Support**: Secure HTTPS communication (port 8443)
- **SSL/TLS Certificates**: Auto-generated self-signed certificates
- **Remote Control**: Web-based interface for LVGL application control

## Quick Start

### Prerequisites

The build system automatically detects your CPU architecture (x86-64, ARM64, ARM 32-bit) and verifies all dependencies.

**Supported Architectures:**
- ✓ x86-64 (Intel/AMD) - Desktop, Laptop, Server
- ✓ ARM64 (aarch64) - Raspberry Pi 4/5, Apple Silicon, ARM64 boards
- ✓ ARM 32-bit - Older Raspberry Pi, ARM32 boards

### Automated Setup (Recommended)

The easiest way to build is using the automated setup script:

```bash
cd Source
./build_setup.sh
cd build
make
./main
```

**What `build_setup.sh` does:**
1. ✓ Detects your system architecture
2. ✓ Checks for required dependencies
3. ✓ Offers to install missing packages
4. ✓ Runs CMake configuration
5. ✓ Shows build instructions

### Manual Installation

If you prefer manual setup:

```bash
cd Source

# Install dependencies (Ubuntu/Debian)
sudo apt-get install -y \
    cmake make build-essential \
    libsdl2-dev libfreetype6-dev libssl-dev \
    libavformat-dev libavcodec-dev libavutil-dev libswscale-dev \
    pkg-config

# Build
mkdir -p build && cd build
cmake ..
make
./main
```

The application will start with:
- LVGL GUI window (800x600)
- Embedded web server on port 8080
- HTTPS server on port 8443 (if TLS is enabled)

## Usage

### Running the Application

From the `Source/build` directory:

```bash
# Run the application
./main

# The application will:
# 1. Create an 800x600 SDL window
# 2. Initialize LVGL and load all tabs
# 3. Start the web server on http://localhost:8080
# 4. Display "Press Ctrl+C to exit"
```

### Web Interface

Once the application is running, you can access:

- **HTTP Interface**: http://localhost:8080
- **HTTPS Interface**: https://localhost:8443 (if TLS enabled)
- **API Status**: http://localhost:8080/api/status
- **UI State**: http://localhost:8080/api/ui/state
- **WebSocket**: ws://localhost:8080/ws

### GUI Features

The LVGL GUI provides the following tabs:

- **Database Tab**: SQLCipher database operations
- **Settings Tab**: Application configuration
- **Info Tab**: System information and status
- **Number Tab**: Numeric input interface
- **Korean Tab**: Korean text input
- **ChunJiIn Tab**: ChunJiIn input method
- **QWERTY Tab**: QWERTY Korean input
- **Calendar Tab**: Date and calendar functionality
- **Clock Tab**: Time display and management
- **Video Tab**: Video playback with codec support

### TLS 1.3 Security

The application supports TLS 1.3 for secure HTTPS communication:

#### Features
- **TLS 1.3 Protocol**: Latest security standard with enhanced performance
- **Self-Signed Certificates**: Automatically generated for development/testing
- **Secure WebSocket**: WSS support for real-time encrypted communication
- **Certificate Management**: Automatic certificate generation and loading

#### Certificate Details
- **Certificate Location**: `certs/` directory (created automatically)
- **Certificate Type**: Self-signed X.509 certificates
- **Key Size**: 2048-bit RSA
- **Validity**: 365 days from generation
- **Subject**: `CN=localhost, O=LVGL WebServer, C=US`

#### Security Considerations
- Self-signed certificates are suitable for development and internal use
- For production deployment, replace with certificates from a trusted CA
- TLS 1.3 provides forward secrecy and improved security over TLS 1.2
- All HTTPS traffic is encrypted end-to-end

#### Testing HTTPS
```bash
# Test HTTPS endpoints (ignore certificate warnings for self-signed certs)
curl -k https://localhost:8443/api/status
curl -k https://localhost:8443/

# Test secure WebSocket
wscat -c wss://localhost:8443/ws --no-check
```

### Testing the Web Server

```bash
# Test web server functionality
./test_web.sh
```

## Project Structure

```
WebServer/
├── README.md                   # Project documentation
├── .gitignore                  # Git ignore rules
├── certs/                      # TLS certificates (auto-generated)
│   ├── server.crt              # Server certificate
│   ├── server.key              # Private key
│   └── ca.crt                  # CA certificate
└── Source/
    ├── CMakeLists.txt          # CMake build configuration
    ├── lv_conf.h               # LVGL configuration file
    ├── .gitignore              # Source-level git ignore
    ├── README.md               # Source-level documentation
    ├── include/                # Header files
    │   ├── web_server.h
    │   ├── ui_components.h
    │   └── ...
    ├── src/                    # Application source code
    │   ├── main.c              # Application entry point
    │   ├── web_server.c        # HTTP server implementation
    │   ├── web_control.c       # Web API handlers
    │   ├── ui_components.c     # LVGL GUI components
    │   ├── tab_*.c             # Individual tab implementations
    │   ├── korean_input.c      # Korean input handling
    │   ├── chunjiin_input.c    # ChunJiIn input method
    │   ├── qwerty_korean.c     # QWERTY Korean layout
    │   ├── calendar.c          # Calendar widget
    │   ├── clock.c             # Clock display
    │   ├── audio.c             # Audio playback
    │   ├── video.c             # Video playback
    │   └── mongoose.c          # HTTP server library
    ├── html/                   # Web interface files
    │   ├── index.html          # Main web page
    │   ├── styles.css          # CSS styling
    │   └── script.js           # JavaScript functionality
    ├── lvgl/                   # LVGL library
    ├── sqlcipher/              # SQLCipher encrypted database library
    ├── onnxruntime/            # ONNX Runtime for machine learning
    └── build/                  # Build output directory (created by CMake)
        ├── main                # Main executable
        ├── lib/                # Compiled libraries
        └── CMakeFiles/         # CMake generated files
```

## API Endpoints

### HTTP API

- `GET /` - Web interface
- `GET /api/status` - Server status (includes TLS information)
- `GET /api/ui/state` - Current UI state

### HTTPS API

- `GET /` - Secure web interface
- `GET /api/status` - Secure server status
- `GET /api/ui/state` - Secure UI state

### WebSocket

- `ws://localhost:8080/ws` - Real-time communication
- `wss://localhost:8443/ws` - Secure real-time communication

#### WebSocket Commands

Send JSON messages to control the LVGL application:

```json
{
  "type": "tab",
  "value": "db"
}
```

Available tab values: `db`, `settings`, `info`, `calendar`, `clock`, `video`

## LVGL Features

- **Database Tab**: SQLCipher database operations
- **Settings Tab**: Application configuration
- **Info Tab**: System information and status
- **Number Tab**: Numeric input interface
- **Korean Tab**: Korean text input
- **ChunJiIn Tab**: ChunJiIn input method
- **QWERTY Tab**: QWERTY Korean input
- **Calendar Tab**: Date and calendar functionality
- **Clock Tab**: Time display and management
- **Video Tab**: Video playback with controls

## Development

### Building from Source

#### Automated Build (Recommended)

```bash
cd Source
./build_setup.sh      # Automatically detect architecture and check dependencies
cd build
make
```

#### Manual Build

```bash
cd Source
mkdir -p build && cd build
cmake ..
make
```

**For detailed build instructions, see [Source/BUILD.md](Source/BUILD.md)**

#### Build Specific Targets

```bash
make main              # Build main application only
make test_chunjiin     # Build ChunJiIn input test
make calendar_test     # Build calendar test
make clock_test        # Build clock test
make qwerty_korean_test # Build QWERTY Korean test
make video_test        # Build video playback test
```

### Running Individual Tests

```bash
# From Source/build directory
./test_chunjiin        # Test ChunJiIn input method
./calendar_test        # Test calendar widget
./clock_test           # Test clock display
./qwerty_korean_test   # Test QWERTY Korean layout
./video_test           # Test video playback
```

### Code Organization

- **UI Components**: Individual `tab_*.c` files for each GUI feature
- **Input Handling**: `korean_input.c`, `chunjiin_input.c`, `qwerty_korean.c`
- **Web Server**: `web_server.c` for HTTP server, `web_control.c` for API endpoints
- **Hardware Support**: `audio.c`, `video.c` for multimedia features
- **Database**: SQLite integration via `sqlite_test.c`

## Troubleshooting

### Common Build Issues

1. **CMake configuration fails**
   - Ensure CMake 3.10+ is installed: `cmake --version`
   - Check that all required packages are installed
   - Delete `CMakeCache.txt` and try again

2. **Missing dependency errors**
   - Install all required development packages
   - Check `pkg-config` can find libraries: `pkg-config --list-all | grep freetype`

3. **Linker errors (undefined references)**
   - Ensure FFmpeg libraries are installed
   - Check OpenSSL is properly linked: `pkg-config --cflags --libs openssl`

### Common Runtime Issues

1. **GUI window not appearing**
   - Check SDL2 is properly installed
   - Verify X11/Wayland display is available
   - Check console output for initialization errors

2. **Web server not responding**
   - Verify port 8080 is not in use: `lsof -i :8080`
   - Check firewall settings if accessing remotely

3. **Audio/Video playback fails**
   - Ensure FFmpeg is installed with codec support
   - Check file format is supported by FFmpeg
   - Review console output for codec errors

4. **Korean input not working**
   - Verify locale settings support Korean: `locale -a | grep ko`
   - Check font files are available for Korean characters

5. **Database operations fail**
   - Ensure SQLCipher library is properly compiled
   - Check database file permissions
   - Verify OpenSSL is available for encryption

### Build Configuration

To rebuild with different options:

```bash
# Clean build
cd Source/build
rm -rf *
cmake ..
make

# Build with verbose output
make VERBOSE=1

# Build specific target with verbose output
make VERBOSE=1 main
```

## Known Limitations

- **Vector Graphics**: ThorVG support is disabled (missing config.h)
- **SVG Support**: Disabled due to vector graphics dependency
- **Display Resolution**: Fixed at 800x600 (configured in main.c)
- **Network**: Single-threaded web server (adequate for development)

## Recent Changes

### Build Fixes (Latest)
- Disabled ThorVG compilation (missing dependencies)
- Disabled SVG support (depends on vector graphics)
- Removed svg_example.c from build
- Moved lv_conf.h to Source directory for easier configuration
- Updated CMakeLists.txt to reference new config location

### Configuration Updates
- Vector graphics disabled in lv_conf.h
- ThorVG support disabled (INTERNAL and EXTERNAL)
- SVG support disabled
- Optimized LVGL configuration for web server use

## Dependencies Summary

| Library | Version | License | Purpose |
|---------|---------|---------|---------|
| SDL2 | 2.x | zlib | Display and input handling |
| FreeType2 | 2.x | FreeType | Font rendering |
| OpenSSL | 1.1+ | Apache 2.0 | TLS/SSL, encryption |
| FFmpeg | 4.x+ | LGPL/GPL | Video and audio codecs |
| LVGL | 9.x | MIT | GUI framework |
| SQLCipher | 4.x | BSD | Encrypted database |
| Mongoose | 7.x | GPLv2 | HTTP server |

## Performance Notes

- Main loop refresh rate: ~200 Hz (5ms sleep interval)
- Web server max connections: Limited by system resources
- Database operations: Synchronous (blocking)
- Video decoding: Hardware acceleration not enabled (CPU-based)

## Future Enhancements

- [ ] Enable ThorVG vector graphics
- [ ] Add SVG rendering support
- [ ] Implement async database operations
- [ ] Add hardware video decoding (VAAPI/NVENC)
- [ ] WebSocket server improvements
- [ ] Mobile app for remote control
- [ ] Cloud synchronization support

## License

This project uses various open-source libraries:
- **LVGL**: MIT License
- **Mongoose**: GPLv2 with commercial license option
- **SQLCipher**: BSD License
- **SDL2**: zlib License
- **FFmpeg**: LGPL/GPL
- **FreeType**: FreeType License
- **OpenSSL**: Apache 2.0 License

Ensure compliance with all library licenses when using or distributing this project.

## Support & Documentation

- [LVGL Documentation](https://docs.lvgl.io)
- [FFmpeg Documentation](https://ffmpeg.org/documentation.html)
- [Mongoose Documentation](https://mongoose.ws)
- [SQLCipher Documentation](https://www.zetetic.net/sqlcipher/)
- [OpenSSL Documentation](https://www.openssl.org/docs/)

## Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

For bug reports, please include:
- Steps to reproduce
- Expected behavior
- Actual behavior
- System information (OS, compiler, library versions)
