# LVGL Face Recognition Application

A LVGL-based GUI application with embedded web server, database encryption, and comprehensive multimedia capabilities.

## Features

- **LVGL GUI**: Rich graphical user interface with multiple tabs
- **Database Operations**: MariaDB database connectivity and operations
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

**Note**: All other dependencies (FreeType2, SDL2, OpenSSL, MariaDB, LVGL) are automatically built from source.

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

- Database Tab (with MariaDB operations)
- Settings Tab
- Info Tab
- Calendar Tab
- Clock Tab
- Video Tab
- Korean Input Tabs (Korean, ChunJiIn, QWERTY, Number)

## Database Operations

### Features

- **MariaDB Integration**: Full database connectivity with MariaDB server
- **Client-Server Architecture**: Connect to remote or local MariaDB servers
- **OpenSSL Support**: Secure connections with SSL/TLS encryption
- **MySQL Compatibility**: Compatible with MySQL servers and APIs
- **Database Operations**: Create, read, update, delete database operations

### Database Information

- **Client Library**: MariaDB Connector/C 3.3.9
- **Server Compatibility**: MariaDB 10.x, MySQL 5.7+
- **Connection**: TCP/IP with SSL/TLS support
- **Authentication**: Standard MySQL authentication methods

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

- **Database Tab**: MariaDB database operations
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
- **MariaDB**: Database client library
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
7. **Database connection fails**: Ensure MariaDB server is running and accessible

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
- MariaDB Connector/C (LGPL License)
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

### Version 2.0 - Database Operations
- ✅ Added MariaDB database connectivity
- ✅ Real-time database operations
- ✅ Secure connections with SSL/TLS support
- ✅ Database operations demonstration and testing
- ✅ Enhanced security with TLS 1.3 support

### Version 1.0 - Core Features
- ✅ LVGL GUI with multiple tabs
- ✅ Embedded web server with WebSocket support
- ✅ Database integration with MariaDB connectivity
- ✅ Video playback with FFmpeg
- ✅ Korean input methods (ChunJiIn, QWERTY)
- ✅ Calendar and clock functionality
- ✅ TLS 1.3 support for secure connections
