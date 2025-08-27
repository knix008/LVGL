# LVGL Face Recognition Application

A LVGL-based GUI application with embedded web server, database encryption, and comprehensive multimedia capabilities.

## Features

- **LVGL GUI**: Rich graphical user interface with multiple tabs
- **Database Encryption**: SQLCipher encrypted database with secure storage
- **Embedded Web Server**: Mongoose-based HTTP/HTTPS server with WebSocket support
- **Video Support**: FFmpeg-based video playback
- **Korean Input**: ChunJiIn and QWERTY Korean input methods
- **Calendar & Clock**: Date/time functionality
- **Remote Control**: Web-based interface for controlling the LVGL application
- **TLS Support**: Secure HTTPS connections with TLS 1.3
- **Static Library Build**: All dependencies built as static libraries for portability

## Quick Start

### Prerequisites

- CMake (3.10 or higher)
- Make
- FFmpeg libraries (system libraries)
- curl (for testing)

**Note**: All other dependencies (FreeType2, SDL2, OpenSSL, SQLCipher, LVGL) are automatically built from source.

### Installation

1. Clone the repository
2. Navigate to the project directory
3. Run the application:

```bash
./run.sh
```

This will:
- Check system dependencies
- Download and build all required libraries from source
- Build the application
- Start the LVGL GUI with embedded web server

## Usage

### Running the Application

```bash
# Build and run (default)
./run.sh

# Build only
./run.sh build

# Build libraries only
./run.sh libs

# Clean build artifacts
./run.sh clean

# Show help
./run.sh help
```

### Main Application

```bash
cd Source/build
./main
```

- Starts the LVGL GUI with all tabs
- Web interface available at http://localhost:8080
- HTTPS interface available at https://localhost:8443
- Automatic HTML file copying during build process

### Web Interface

Once the application is running, you can access:

- **HTTP Interface**: http://localhost:8080
- **HTTPS Interface**: https://localhost:8443 (TLS 1.3)
- **API Status**: http://localhost:8080/api/status
- **UI State**: http://localhost:8080/api/ui/state
- **WebSocket**: ws://localhost:8080/ws
- **Secure WebSocket**: wss://localhost:8443/ws

#### Web Interface Features

The web interface provides complete control over the LVGL application:

- **Tab Navigation**: Switch between all GUI tabs
- **Video Controls**: Play, pause, stop, next, previous, volume control
- **Real-time Status**: View current application state and status
- **WebSocket Communication**: Real-time bidirectional communication

#### Available Tab Controls

- Database Tab (with SQLCipher encryption)
- Settings Tab
- Info Tab
- Calendar Tab
- Clock Tab
- Video Tab
- Korean Input Tabs (Korean, ChunJiIn, QWERTY, Number)

## Database Encryption

### Features

- **SQLCipher Integration**: Full database encryption with AES-256
- **Secure Storage**: All data encrypted at rest
- **OpenSSL Backend**: Uses OpenSSL for cryptographic operations
- **Automatic Key Management**: Encryption keys handled securely
- **Database Operations**: Create, read, update, delete encrypted data

### Database Information

- **Encryption**: AES-256-CBC
- **Key Derivation**: PBKDF2 with 256,000 iterations
- **Page Size**: 4096 bytes
- **Cipher Version**: SQLCipher 4.5.4

## Project Structure

```
FaceRecognition2.0/
├── run.sh              # Main runner script
├── test_web.sh         # Web server test script
├── README.md           # This file
├── certs/              # TLS certificates (auto-generated)
│   ├── server.crt      # Server certificate
│   ├── server.key      # Private key
│   └── ca.crt          # CA certificate
└── Source/
    ├── include/        # Header files
    │   ├── web_server.h
    │   ├── mongoose.h
    │   ├── web_control.h
    │   └── tls_config.h
    ├── src/           # Source files
    │   ├── main.c                    # LVGL GUI application
    │   ├── sqlite_test.c             # Database encryption demo
    │   ├── web_server.c              # Web server implementation
    │   ├── web_control.c             # Web control handlers
    │   ├── mongoose.c
    │   └── ...
    ├── html/          # Web interface files
    │   ├── index.html                # Main web interface
    │   ├── script.js                 # JavaScript functionality
    │   └── styles.css                # CSS styling
    ├── assets/        # Media files
    ├── lvgl/          # LVGL library
    ├── lib/           # Built libraries and headers
    │   ├── lib/       # Static libraries
    │   └── include/   # Header files
    ├── build_*.sh     # Individual library build scripts
    ├── build_all_libs.sh # Master library build script
    └── build/         # Build artifacts (auto-copied HTML files)
```

## LVGL Features

- **Database Tab**: SQLCipher encrypted database operations
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

### Building

```bash
cd Source
mkdir -p build
cd build
cmake ..
make
```

This builds the main LVGL GUI application with all features.

**Note**: The build process automatically copies HTML files from `Source/html` to `Source/build/html` for the web server.

### Running

```bash
cd Source/build
./main
```

### Testing

```bash
# Test web server
./test_web.sh

# Test individual components
cd Source
./build_chunjiin_test.sh
./build_calendar_test.sh
./build_clock_test.sh

# Test web interface
curl http://localhost:8080
```

## Library Build System

### Automatic Library Building

The project includes a comprehensive library build system that:

- **Downloads source code** for all dependencies
- **Builds static libraries** for maximum portability
- **Cleans up source directories** after successful builds to save disk space
- **Supports individual library builds** for development

### Available Libraries

- **FreeType2**: Font rendering library
- **SDL2**: Graphics and input library
- **OpenSSL**: Cryptography library
- **SQLCipher**: Encrypted database library
- **LVGL**: Graphics library (built from existing source)

### Build Scripts

```bash
# Build all libraries
./Source/build_all_libs.sh all

# Build individual libraries
./Source/build_freetype.sh
./Source/build_sdl2.sh
./Source/build_openssl.sh
./Source/build_sqlcipher.sh
./Source/build_lvgl.sh
```

## Troubleshooting

### Common Issues

1. **Build fails**: Check that all system dependencies are installed
2. **Web server not accessible**: Ensure port 8080 is not in use
3. **LVGL GUI not appearing**: Check SDL2 installation and display settings
4. **HTTPS not working**: Ensure OpenSSL is installed and port 8443 is available
5. **Certificate warnings**: Self-signed certificates will show browser warnings (normal for development)
6. **TLS handshake fails**: Check that OpenSSL libraries are properly linked
7. **Database encryption fails**: Ensure SQLCipher was built with OpenSSL support

### Dependencies

Install required packages on Ubuntu/Debian:

```bash
sudo apt-get install cmake make pkg-config libavformat-dev libavcodec-dev libavutil-dev libswscale-dev libswresample-dev
```

### Web Interface Issues

1. **Tab switching not working**: Check WebSocket connection and browser console for errors
2. **HTML files not updated**: The build process automatically copies HTML files, but you may need to rebuild

## License

This project uses various open-source libraries:
- LVGL (MIT License)
- Mongoose (GPL v2)
- SQLCipher (BSD License)
- SDL2 (zlib License)
- FFmpeg (LGPL/GPL)
- OpenSSL (Apache License)
- FreeType2 (FreeType License)

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## Recent Updates

### Version 2.2 - Library Build System & Cleanup
- ✅ Implemented automatic library building from source
- ✅ Added source cleanup after successful builds to save disk space
- ✅ Removed webcam IPC app and OpenCV dependencies
- ✅ Simplified project structure and dependencies
- ✅ Enhanced build scripts with download and cleanup functionality
- ✅ Improved CMakeLists.txt for better library linking
- ✅ Added comprehensive library build documentation

### Version 2.1 - Web Interface & Performance Improvements
- ✅ Added comprehensive web interface for tab navigation
- ✅ Implemented automatic HTML file copying during build process
- ✅ Enhanced web server command parsing and error handling
- ✅ Added comprehensive web interface testing and validation

### Version 2.0 - Database Encryption
- ✅ Added SQLCipher database encryption
- ✅ Real-time encrypted database operations
- ✅ Secure key management and storage
- ✅ Database encryption demonstration and testing
- ✅ Enhanced security with TLS 1.3 support

### Version 1.0 - Core Features
- ✅ LVGL GUI with multiple tabs
- ✅ Embedded web server with WebSocket support
- ✅ Database integration with SQLCipher encryption
- ✅ Video playback with FFmpeg
- ✅ Korean input methods (ChunJiIn, QWERTY)
- ✅ Calendar and clock functionality
- ✅ TLS 1.3 support for secure connections
