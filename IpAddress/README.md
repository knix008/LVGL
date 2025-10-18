# IP Address Input GUI

A modern GUI application built with LVGL for inputting and validating both IPv4 and IPv6 addresses. The application features a 320x640 window with intuitive controls and real-time validation.

## Features

- **Dual Mode Support**: Switch between IPv4 and IPv6 input modes
- **Real-time Validation**: Instant feedback on address validity
- **Custom Keyboards**: Optimized keyboard layouts for each IP version
- **Modern UI**: Clean, responsive interface with LVGL
- **Cross-platform**: Works on Linux, Windows, and embedded systems

## Requirements

- GCC compiler
- LVGL library (included)
- SDL2 (for desktop simulation)
- Make

## Installation

### Install Dependencies (Ubuntu/Debian)

```bash
make install-deps
```

### Build the Application

```bash
make
```

### Run the Application

```bash
# Option 1: Using the run script
./run.sh

# Option 2: Direct execution
./ip_address

# Option 3: Using make
make run
```

**Note:** This is a GUI application that requires a graphical environment (X11 or Wayland). Make sure you have DISPLAY set and are running in a desktop environment.

## Usage

1. **Mode Selection**: Use the toggle switch to switch between IPv4 and IPv6 modes (keyboard layout changes automatically)
2. **Input**: Type an IP address using the on-screen keyboard (always visible)
3. **Validation**: Click "Validate" to check the entered address
4. **Clear**: Click "Clear" to reset the input field

### IPv4 Mode
- Enter addresses in dotted decimal notation (e.g., 192.168.1.1)
- Each octet must be between 0-255
- Example valid addresses: 192.168.1.1, 10.0.0.1, 127.0.0.1

### IPv6 Mode
- Enter addresses in hexadecimal notation
- Supports both full and compressed notation
- Example valid addresses: 2001:db8::1, ::1, fe80::1

## Project Structure

```
IpAddress/
├── main.c                 # Main application entry point
├── ip_address_gui.h       # GUI interface declarations
├── ip_address_gui.c       # GUI implementation
├── ip_address.h           # IP validation logic interface
├── ip_address.c           # IP validation logic implementation
├── Makefile               # Build configuration
├── lv_conf.h              # LVGL configuration
├── lvgl/                  # LVGL library
├── test/                  # Test suite
│   ├── test_ipv4.c        # IPv4 validation tests
│   ├── test_ipv6.c        # IPv6 validation tests
│   ├── test_all.c         # Comprehensive tests
│   ├── Makefile           # Test build configuration
│   ├── run_tests.sh       # Test runner script
│   └── README.md          # Test documentation
└── README.md              # This file
```

## Build Options

- `make` - Standard build
- `make debug` - Debug build with additional flags
- `make release` - Optimized release build
- `make clean` - Remove build files
- `make help` - Show available targets

## Configuration

The application window size and other parameters can be modified in `ip_address.h`:

```c
#define WINDOW_WIDTH  320
#define WINDOW_HEIGHT 640
```

## Validation Logic

### IPv4 Validation
- Checks for exactly 4 octets separated by dots
- Each octet must be a valid number (0-255)
- No leading zeros (except for "0" itself)

### IPv6 Validation
- Supports both full and compressed notation
- Validates hexadecimal groups (0-FFFF)
- Handles double colon (::) compression correctly
- Ensures proper group count and format

## Testing

The project includes a comprehensive test suite with 163+ test cases covering IPv4 and IPv6 validation.

### Running Tests

```bash
# Run all tests
cd test && ./run_tests.sh

# Run specific tests
./run_tests.sh ipv4           # IPv4 tests only
./run_tests.sh ipv6           # IPv6 tests only
./run_tests.sh comprehensive  # Comprehensive tests

# Manual build and run
cd test && make test
```

### Test Coverage

- **IPv4 Tests (41 cases)**: Valid addresses, invalid formats, edge cases, octet parsing
- **IPv6 Tests (51 cases)**: Full/compressed notation, invalid formats, hex parsing
- **Comprehensive Tests (71 cases)**: Combined validation, format functions, edge cases

**Current Status**: ✅ 100% pass rate (163/163 tests passing)

For detailed test documentation, see [test/README.md](test/README.md).
