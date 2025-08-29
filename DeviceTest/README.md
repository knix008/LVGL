# Device Test Program

A comprehensive device testing suite for embedded systems and Linux devices, supporting camera, network, serial, Wiegand, LCD, CPU, and eMMC testing.

## Features

- **Camera Testing**: Test camera initialization, capture, resolution, FPS, and capabilities with V4L2 support
- **Network Testing**: Test network interface initialization, connectivity, speed, and packet loss
- **Serial (UART) Testing**: Test serial communication, loopback, speed, and error handling
- **Wiegand Testing**: Test Wiegand protocol communication, data reading, and parity validation
- **LCD Testing**: Test LCD interfaces (SPI, I2C, Parallel, MIPI DSI, HDMI, VGA, DVI, DisplayPort)
- **CPU Testing**: Test CPU architecture, cores, frequency, cache, and performance metrics
- **eMMC Testing**: Test embedded MultiMediaCard storage capacity, read/write speed, and health
- **Interactive Mode**: Real-time testing with user commands
- **Automated Test Suite**: Run all tests automatically with comprehensive summary
- **Performance Scoring**: Each test provides a score out of 100
- **Capability Discovery**: Automatically discover device capabilities and supported features
- **Comprehensive Reporting**: Detailed test results with recommendations

## Prerequisites

- Linux system
- OpenCV 4.x development libraries
- CMake 3.10 or higher
- pkg-config
- Network interface access
- Serial port access (for UART testing)
- GPIO access (for Wiegand testing)
- Display access (for LCD testing)

### Install Dependencies (Ubuntu/Debian)

```bash
sudo apt update
sudo apt install build-essential cmake pkg-config
sudo apt install libopencv-dev
sudo apt install libv4l-dev
```

## Building

1. Clone or download the project
2. Run the build script:

```bash
./build.sh
```

The executable will be created at `build/bin/DeviceTest`.

## Usage

### Basic Usage

```bash
# Show help
./build/bin/DeviceTest -h

# Run automated test suite (all tests)
./build/bin/DeviceTest -t auto

# Run automated test suite with specific devices
./build/bin/DeviceTest -t auto -n eth0 -s /dev/ttyUSB0 -w 17 18

# Run all camera tests
./build/bin/DeviceTest -d camera -t all

# Run all network tests on specific interface
./build/bin/DeviceTest -d network -n eth0 -t all

# Run all serial tests
./build/bin/DeviceTest -d serial -s /dev/ttyUSB0 -b 115200 -t all

# Run all Wiegand tests
./build/bin/DeviceTest -d wiegand -w 17 18 -t all

# Run all LCD tests
./build/bin/DeviceTest -d lcd -t all

# Run all CPU tests
./build/bin/DeviceTest -d cpu -t all

# Run all eMMC tests
./build/bin/DeviceTest -d emmc -p /dev/mmcblk0 -t all

# Start interactive camera mode
./build/bin/DeviceTest -i
```

### Command Line Options

- `-h`: Show help message
- `-d <device>`: Device type (`camera`, `network`, `serial`, `wiegand`, `lcd`, `cpu`, `emmc`, `auto`)
- `-c <index>`: Camera index (default: 0)
- `-n <interface>`: Network interface name (e.g., `eth0`, `wlan0`)
- `-s <device>`: Serial device path (e.g., `/dev/ttyUSB0`)
- `-b <baud_rate>`: Serial baud rate (default: 115200)
- `-w <data0> <data1>`: Wiegand data pins (e.g., 17 18)
- `-p <path>`: Device path for specific tests (e.g., `/dev/mmcblk0` for eMMC)
- `-t <test>`: Run specific test (see test types below)
- `-i`: Start interactive mode

### Test Types

#### Camera Tests
- `init`: Test camera initialization
- `capture`: Test camera capture
- `resolution`: Test camera resolution
- `fps`: Test camera FPS
- `capabilities`: Test camera capabilities
- `supported_capabilities`: Test only supported camera capabilities
- `all`: Run all camera tests

#### Network Tests
- `init`: Test network initialization
- `connectivity`: Test network connectivity
- `speed`: Test network speed
- `packet_loss`: Test packet loss
- `all`: Run all network tests

#### Serial Tests
- `init`: Test serial initialization
- `comm`: Test serial communication
- `loopback`: Test serial loopback
- `speed`: Test serial speed
- `error`: Test serial error handling
- `config`: Test serial configuration
- `all`: Run all serial tests

#### Wiegand Tests
- `init`: Test Wiegand initialization
- `reading`: Test Wiegand data reading
- `protocols`: Test Wiegand protocols
- `parity`: Test Wiegand parity validation
- `speed`: Test Wiegand transmission speed
- `error`: Test Wiegand error handling
- `all`: Run all Wiegand tests

#### LCD Tests
- `connection`: Test LCD connection
- `init`: Test LCD initialization
- `resolution`: Test LCD resolution
- `capabilities`: Test LCD capabilities
- `all`: Run all LCD tests

#### CPU Tests
- `architecture`: Test CPU architecture
- `cores`: Test CPU cores
- `frequency`: Test CPU frequency
- `capabilities`: Test CPU capabilities
- `all`: Run all CPU tests

#### eMMC Tests
- `detection`: Test eMMC detection
- `capacity`: Test eMMC capacity
- `capabilities`: Test eMMC capabilities
- `all`: Run all eMMC tests

#### Special Tests
- `auto`: Run automated test suite (all device tests)

## Automated Test Suite

The automated test suite (`-t auto`) runs all available tests and provides a comprehensive summary:

### Features
- **Complete Coverage**: Tests all device functionality (camera, network, serial, Wiegand, LCD, CPU, eMMC)
- **Performance Scoring**: Each test scored out of 100
- **Summary Report**: Overall results with pass/fail statistics
- **Recommendations**: Actionable advice based on test results
- **Final Verdict**: Overall device assessment

### Example Output

```
=== Automated Device Test Suite ===
Starting comprehensive device testing...

=== Running All Camera Tests ===
✓ Camera Initialization: PASS (85.0/100)
✓ Camera Capture: PASS (90.0/100)
✓ Camera Resolution: PASS (75.0/100)
✓ Camera FPS: PASS (80.0/100)
✓ Camera Capabilities: PASS (82.5/100)

=== Running All Network Tests ===
✓ Network Initialization: PASS (40.0/100)
✓ Network Connectivity: PASS (66.7/100)
✗ Network Speed: FAIL (30.0/100)
✗ Network Packet Loss: FAIL (20.0/100)

=== Running All Serial Tests ===
✓ Serial Initialization: PASS (100.0/100)
✓ Serial Communication: PASS (85.0/100)

=== Running All Wiegand Tests ===
✓ Wiegand Initialization: PASS (100.0/100)
✓ Wiegand Reading: PASS (90.0/100)

=== Running All LCD Tests ===
✓ LCD Connection: PASS (100.0/100)
✓ LCD Capabilities: PASS (85.0/100)

=== Running All CPU Tests ===
✓ CPU Architecture: PASS (100.0/100)
✓ CPU Cores: PASS (80.0/100)
✓ CPU Frequency: PASS (100.0/100)
✓ CPU Capabilities: PASS (85.5/100)

=== Running All eMMC Tests ===
✓ eMMC Detection: PASS (100.0/100)
✓ eMMC Capacity: PASS (90.0/100)
✓ eMMC Capabilities: PASS (85.0/100)

=== TEST SUMMARY ===
Camera Tests: 5/5 passed, Average Score: 82.5/100
Network Tests: 2/4 passed, Average Score: 39.2/100
Serial Tests: 2/2 passed, Average Score: 92.5/100
Wiegand Tests: 2/2 passed, Average Score: 95.0/100
LCD Tests: 2/2 passed, Average Score: 92.5/100
CPU Tests: 4/4 passed, Average Score: 91.4/100
eMMC Tests: 3/3 passed, Average Score: 91.7/100

=== OVERALL RESULTS ===
Total Tests: 24
Passed: 20
Failed: 4
Success Rate: 83.3%
Overall Score: 82.1/100

=== FINAL VERDICT ===
✅ MOST TESTS PASSED! Device is working well with minor issues.

=== RECOMMENDATIONS ===
- Network issues detected. Check network configuration and connectivity.
- Network performance is poor. Check network infrastructure.
```

## Interactive Mode

Start interactive mode with `-i` flag:

### Camera Interactive Mode

```bash
./build/bin/DeviceTest -i
```

Available commands:
- `start`: Start camera capture
- `stop`: Stop camera capture
- `capture`: Capture a single frame
- `save <filename>`: Save captured frame to file
- `info`: Show camera information
- `resolutions`: Show supported resolutions
- `capabilities`: Show camera capabilities
- `quit`: Exit interactive mode

### Network Interactive Mode

```bash
./build/bin/DeviceTest -d network -n eth0 -i
```

Available commands:
- `info`: Show network interface information
- `connectivity`: Test network connectivity
- `speed`: Test network speed
- `packet_loss`: Test packet loss
- `quit`: Exit interactive mode

## Device Testing Details

### Camera Testing
- **V4L2 Integration**: Direct V4L2 API usage for reliable camera control
- **Capability Discovery**: Automatically discovers supported resolutions and formats
- **Real Frame Capture**: Tests actual frame capture at different resolutions
- **Performance Metrics**: Measures capture speed and frame rate

### Network Testing
- **Interface Detection**: Automatically detects available network interfaces
- **Connectivity Testing**: Pings remote hosts to verify connectivity
- **Speed Testing**: Measures network throughput
- **Packet Loss Testing**: Monitors network reliability

### Serial (UART) Testing
- **Device Detection**: Tests serial device accessibility
- **Communication Testing**: Tests data transmission and reception
- **Loopback Testing**: Tests internal serial communication
- **Error Handling**: Tests various error conditions

### Wiegand Testing
- **Protocol Support**: Tests 26-bit and 37-bit Wiegand protocols
- **Data Reading**: Tests card data reading functionality
- **Parity Validation**: Tests data integrity checks
- **Speed Testing**: Measures transmission speed

### LCD Testing
- **Interface Detection**: Supports multiple LCD interfaces (SPI, I2C, HDMI, etc.)
- **Connection Testing**: Tests LCD device connectivity
- **Resolution Testing**: Tests display resolution capabilities
- **Capability Testing**: Tests color depth, refresh rate, and other features

### CPU Testing
- **Architecture Detection**: Identifies CPU architecture (x86, ARM, etc.)
- **Core Testing**: Tests CPU cores and threads
- **Performance Testing**: Measures CPU performance metrics
- **Feature Detection**: Tests CPU instruction sets (SSE, AVX, NEON)

### eMMC Testing
- **Device Detection**: Tests eMMC device accessibility
- **Capacity Testing**: Measures storage capacity
- **Performance Testing**: Tests read/write speeds
- **Health Monitoring**: Tests device health and temperature

## Project Structure

```
DeviceTest/
├── CMakeLists.txt          # Build configuration
├── build.sh               # Build script
├── .gitignore             # Git ignore file
├── README.md              # This file
├── include/
│   ├── common.h           # Common structures and definitions
│   ├── camera.h           # Camera-specific declarations
│   ├── network.h          # Network-specific declarations
│   ├── serial.h           # Serial-specific declarations
│   ├── wiegand.h          # Wiegand-specific declarations
│   ├── lcd.h              # LCD-specific declarations
│   ├── cpu.h              # CPU-specific declarations
│   └── emmc.h             # eMMC-specific declarations
└── src/
    ├── main.c             # Main program entry point
    ├── camera.cpp         # Camera testing implementation
    ├── network.cpp        # Network testing implementation
    ├── serial.cpp         # Serial testing implementation
    ├── wiegand.cpp        # Wiegand testing implementation
    ├── lcd.cpp            # LCD testing implementation
    ├── cpu.cpp            # CPU testing implementation
    └── emmc.cpp           # eMMC testing implementation
```

## Header Organization

The project uses a modular header structure:

- **`common.h`**: Contains shared structures like `test_result_t` and `test_summary_t`
- **`camera.h`**: Contains camera-specific structures and function declarations
- **`network.h`**: Contains network-specific structures and function declarations
- **`serial.h`**: Contains serial-specific structures and function declarations
- **`wiegand.h`**: Contains Wiegand-specific structures and function declarations
- **`lcd.h`**: Contains LCD-specific structures and function declarations
- **`cpu.h`**: Contains CPU-specific structures and function declarations
- **`emmc.h`**: Contains eMMC-specific structures and function declarations

This modular approach provides:
- Clear separation of concerns
- Easy maintenance and extension
- Reduced compilation dependencies
- Better code organization

## Performance Scoring

Each test provides a performance score out of 100:

- **90-100**: Excellent performance
- **80-89**: Good performance
- **70-79**: Acceptable performance
- **60-69**: Below average performance
- **Below 60**: Poor performance

## Troubleshooting

### Camera Issues
- Ensure camera is not being used by another application
- Check camera permissions (`/dev/video*`)
- Verify OpenCV and V4L2 installation
- Check camera drivers

### Network Issues
- Ensure network interface exists
- Check network permissions
- Verify network configuration
- Test with different interfaces

### Serial Issues
- Ensure serial device exists and is accessible
- Check serial port permissions
- Verify baud rate settings
- Test with different serial devices

### Wiegand Issues
- Ensure GPIO pins are accessible
- Check GPIO permissions
- Verify pin configuration
- Test with different pin combinations

### LCD Issues
- Ensure display interface is supported
- Check display permissions
- Verify display configuration
- Test with different interfaces

### CPU Issues
- Ensure system information is accessible
- Check CPU monitoring permissions
- Verify CPU configuration
- Test with different performance settings

### eMMC Issues
- Ensure eMMC device exists and is accessible
- Check storage permissions
- Verify device configuration
- Test with different device paths

### Build Issues
- Ensure all dependencies are installed
- Check CMake version (3.10+ required)
- Verify OpenCV and V4L2 installation
- Check compiler compatibility

## License

This project is provided as-is for educational and testing purposes.
