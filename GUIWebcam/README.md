# LVGL WebServer Application

A LVGL-based GUI application with an embedded web server for remote control and monitoring.

## Features

- **LVGL GUI**: Rich graphical user interface with multiple tabs
- **Embedded Web Server**: Mongoose-based HTTP server with WebSocket support
- **Database Integration**: SQLCipher encrypted database
- **Video Support**: FFmpeg-based video playback
- **Korean Input**: ChunJiIn and QWERTY Korean input methods
- **Calendar & Clock**: Date/time functionality
- **Remote Control**: Web-based interface for controlling the LVGL application

## Quick Start

### Prerequisites

- CMake (3.10 or higher)
- Make
- FreeType2 (`libfreetype6-dev`)
- SDL2 (`libsdl2-dev`)
- FFmpeg libraries
- OpenSSL (`libssl-dev`) - for TLS 1.3 support
- curl (for testing)

### Installation

1. Clone the repository
2. Navigate to the WebServer directory
3. Run the application:

```bash
./run.sh
```

This will:
- Check dependencies
- Build the application
- Start the LVGL GUI with embedded web server

## Usage

### Running the Application

```bash
# Build and run (default)
./run.sh

# Build only
./run.sh build

# Clean build artifacts
./run.sh clean

# Show help
./run.sh help
```

### Web Interface

Once the application is running, you can access:

- **HTTP Interface**: http://localhost:8080
- **HTTPS Interface**: https://localhost:8443 (TLS 1.3)
- **API Status**: http://localhost:8080/api/status
- **UI State**: http://localhost:8080/api/ui/state
- **WebSocket**: ws://localhost:8080/ws
- **Secure WebSocket**: wss://localhost:8443/ws

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
    │   └── tls_config.h # TLS configuration
    ├── src/           # Source files
    │   ├── main.c
    │   ├── web_server.c
    │   ├── mongoose.c
    │   └── ...
    ├── assets/        # Media files
    ├── lvgl/          # LVGL library
    ├── sqlcipher/     # SQLCipher library
    └── build/         # Build artifacts
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

### Building

```bash
cd Source
mkdir -p build
cd build
cmake ..
make main
```

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
```

## Troubleshooting

### Common Issues

1. **Build fails**: Check that all dependencies are installed
2. **Web server not accessible**: Ensure port 8080 is not in use
3. **LVGL GUI not appearing**: Check SDL2 installation and display settings
4. **HTTPS not working**: Ensure OpenSSL is installed and port 8443 is available
5. **Certificate warnings**: Self-signed certificates will show browser warnings (normal for development)
6. **TLS handshake fails**: Check that OpenSSL libraries are properly linked

### Dependencies

Install required packages on Ubuntu/Debian:

```bash
sudo apt-get install cmake make libfreetype6-dev libsdl2-dev libavformat-dev libavcodec-dev libavutil-dev libswscale-dev libssl-dev
```

## License

This project uses various open-source libraries:
- LVGL (MIT License)
- Mongoose (GPL v2)
- SQLCipher (BSD License)
- SDL2 (zlib License)
- FFmpeg (LGPL/GPL)

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request
