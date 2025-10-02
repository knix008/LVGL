# Device Simulator with TLS 1.3

A C++ device simulator that exposes an HTTPS API for firmware updates using TLS 1.3.

## Features

- **TLS 1.3 Only**: Secure communication using the latest TLS protocol
- **HTTPS API**: RESTful endpoints for firmware updates and device status
- **Multipart File Upload**: Direct firmware file upload with metadata
- **Firmware Storage**: Automatically saves uploaded firmware files to disk
- **JSON Support**: Request/response handling with JSON format
- **Large Firmware Support**: Handles firmware files up to 2GB
- **Progress Logging**: Real-time progress tracking for large firmware transfers
- **Size Validation**: Automatic rejection of firmware exceeding size limits
- **Binary File Support**: Correctly handles binary firmware files without corruption

## API Endpoints

### POST /api/v1.0/updatefirmware
Update device firmware with version and URL, or upload firmware file directly.

**Maximum firmware size: 2GB**

#### Method 1: JSON Request (URL only)
**Request:**
```json
{
    "version": "2.0.0",
    "firmware_url": "https://example.com/firmware.bin"
}
```

#### Method 2: Multipart File Upload (Firmware file + metadata)
**Request:**
```
Content-Type: multipart/form-data; boundary=FormBoundary...

--FormBoundary...
Content-Disposition: form-data; name="metadata"
Content-Type: application/json

{
    "version": "2.0.0",
    "firmware_url": "file://firmware.bin",
    "filename": "firmware.bin"
}

--FormBoundary...
Content-Disposition: form-data; name="firmware"; filename="firmware.bin"
Content-Type: application/octet-stream

[binary firmware data]
--FormBoundary...--
```

**Success Response (200 OK):**
```json
{
    "status": "success",
    "message": "Firmware update completed successfully",
    "device_id": "DEVICE-SIM-001",
    "previous_version": "1.0.0",
    "new_version": "2.0.0",
    "update_timestamp": "2025-10-02T10:30:45",
    "saved_file": "received_firmwares/firmware_2.0.0_2025-10-02T15-29-34_firmware.bin",
    "firmware_size": 107527151
}
```

**Error Response - Size Limit Exceeded (413 Payload Too Large):**
```json
{
    "status": "error",
    "message": "Firmware size exceeds maximum allowed size of 2GB",
    "max_size_bytes": 2147483648,
    "requested_size_bytes": 2500000000
}
```

### GET /api/v1.0/status
Get current device status and update history.

**Response:**
```json
{
    "device_id": "DEVICE-SIM-001",
    "current_firmware_version": "2.0.0",
    "status": "idle",
    "last_update": "2025-10-02T10:30:45",
    "update_history": [...]
}
```

### GET /api/v1.0/health
Health check endpoint.

**Response:**
```json
{
    "status": "healthy",
    "device_id": "DEVICE-SIM-001",
    "timestamp": "2025-10-02T10:30:45"
}
```

## Prerequisites

- C++17 compatible compiler (g++ or clang++)
- CMake 3.10 or higher
- OpenSSL 1.1.1 or higher (with TLS 1.3 support)
- jsoncpp library
- pkg-config

## Quick Start

**The easiest way to get started:**

```bash
# 1. Install dependencies (Ubuntu/Debian/macOS)
./install_dependencies.sh

# 2. Build everything
./build.sh

# 3. Run the device simulator
./run.sh
```

**Then in another terminal:**

```bash
# Upload firmware
cd build
./firmware_uploader --version 2.0.0 --url https://example.com/firmware.bin

# Or run automated tests
./test_upload.sh
```

---

### Detailed Setup

<details>
<summary>Click to expand manual setup steps</summary>

#### 1. Install Dependencies

Use the automated installation script (supports Ubuntu/Debian, CentOS/RHEL/Fedora, and macOS):

```bash
./install_dependencies.sh
```

Or install manually for your platform:

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake libssl-dev libjsoncpp-dev pkg-config
```

**CentOS/RHEL/Fedora:**
```bash
sudo dnf groupinstall -y "Development Tools"
sudo dnf install -y cmake openssl-devel jsoncpp-devel pkgconfig
```

**macOS:**
```bash
brew install cmake openssl jsoncpp pkg-config
```

#### 2. Build the Project

**Using build script (recommended):**
```bash
./build.sh              # Build the project
./build.sh clean        # Clean everything
./build.sh rebuild      # Clean and rebuild
./build.sh help         # Show help
```

**Or manually:**
```bash
./generate_certs.sh  # Generate TLS certificates
mkdir build
cd build
cmake ..
make
```

This builds both:
- `device_simulator` - The device simulator server
- `firmware_uploader` - The firmware upload client

#### 3. Run the Simulator

**Using run script (recommended):**
```bash
./run.sh
```

**Or manually:**
```bash
cd build
./device_simulator --cert ../certs/device.crt --key ../certs/device.key
```

#### 4. Upload Firmware

In another terminal, use the firmware uploader:

```bash
cd build
./firmware_uploader --version 2.0.0 --url https://example.com/firmware.bin
```

Or run the automated test script:

```bash
cd build
./test_upload.sh
```

</details>

## Device Simulator Usage

### Using run.sh (Recommended)

```bash
# Start simulator
./run.sh
./run.sh start

# Stop simulator
./run.sh stop

# Check status
./run.sh status

# Restart simulator
./run.sh restart

# Custom configuration
./run.sh start --device-id DEVICE-001 --port 9443 --version 1.0.0

# Show help
./run.sh help
```

The `run.sh` script provides:
- **start** - Start the device simulator (default)
- **stop** - Stop the running simulator gracefully
- **status** - Check if simulator is running (shows PID and port)
- **restart** - Stop and restart the simulator
- Automatic certificate generation if needed
- Port conflict detection
- Proper path setup

### Manual Usage

```bash
cd build
./device_simulator --cert ../certs/device.crt --key ../certs/device.key

# Stop manually
pkill device_simulator
```

### Command Line Options

```bash
./run.sh [command] [options]

Commands:
  start             Start the device simulator (default)
  stop              Stop the running device simulator
  status            Check if simulator is running
  restart           Restart the device simulator
  help              Show help message

Options (for start/restart):
  --device-id ID    Device ID (default: DEVICE-SIM-001)
  --port PORT       Port to listen on (default: 8443)
  --version VER     Initial firmware version (default: 1.0.0)
```

## Firmware Uploader Usage

### Command Line Options

```bash
./firmware_uploader [options]

Options:
  --host HOST           Target host (default: localhost)
  --port PORT           Target port (default: 8443)
  --file FILE           Firmware file to upload
  --version VERSION     Firmware version
  --url URL             Firmware URL (alternative to --file)
  --status              Get device status
  --verify-ssl          Verify SSL certificate (default: false)
  --help                Show help message
```

### Examples

**Upload firmware via URL:**
```bash
./firmware_uploader --version 2.0.0 --url https://example.com/firmware.bin
```

**Upload firmware file:**
```bash
./firmware_uploader --file firmware.bin --version 2.0.0
```

**Upload to custom host/port:**
```bash
./firmware_uploader --host 192.168.1.100 --port 9443 --file fw.bin --version 3.0.0
```

**Get device status:**
```bash
./firmware_uploader --status
```

### Automated Testing

Run the automated test script to test various firmware sizes:

```bash
# From project root or build directory
cd build
./test_upload.sh              # Basic tests (1KB, 1MB)
./test_upload.sh --full       # Full tests (includes 10MB)
./test_upload.sh --full --large  # All tests (includes 100MB)
```

The test script will:
1. Check device status
2. Upload firmware via URL (JSON mode)
3. Upload firmware files of various sizes
4. Display final device status
5. Clean up test files automatically

**Test output includes:**
- Progress bars for large file uploads (>1MB)
- MD5 checksum verification
- Firmware storage confirmation
- Upload success/failure status

## Testing the API

### Using the Firmware Uploader (Recommended)

The included `firmware_uploader` tool is the easiest way to test:

```bash
# Upload firmware
./firmware_uploader --version 2.0.0 --url https://example.com/fw.bin

# Check status
./firmware_uploader --status
```

### Using curl

1. **Health Check**:
   ```bash
   curl -k https://localhost:8443/api/v1.0/health
   ```

2. **Get Status**:
   ```bash
   curl -k https://localhost:8443/api/v1.0/status
   ```

3. **Update Firmware**:
   ```bash
   curl -k -X POST https://localhost:8443/api/v1.0/updatefirmware \
     -H "Content-Type: application/json" \
     -d '{
       "version": "2.0.0",
       "firmware_url": "https://example.com/firmware_v2.bin"
     }'
   ```

### Using Python

```python
import requests
import json

# Disable SSL warnings for self-signed cert
requests.packages.urllib3.disable_warnings()

url = "https://localhost:8443/api/v1.0/updatefirmware"
data = {
    "version": "2.0.0",
    "firmware_url": "https://example.com/firmware_v2.bin"
}

response = requests.post(url, json=data, verify=False)
print(json.dumps(response.json(), indent=2))
```

## Firmware Upload & Storage

### Upload Methods

The device simulator supports two upload methods:

1. **JSON Request (URL only)**: Send firmware URL in JSON payload
2. **Multipart File Upload**: Upload firmware binary directly with metadata

### Firmware Storage

When firmware is uploaded via multipart file upload:
- Firmware files are automatically saved to `received_firmwares/` directory
- Filename format: `firmware_<version>_<timestamp>_<original_filename>`
- Example: `firmware_2.0.0_2025-10-02T15-43-24_Firmware.bin`
- Files are saved with exact binary integrity (MD5 checksum verified)
- The directory is created automatically if it doesn't exist
- Location: Project root directory (`received_firmwares/`)

**Verify uploaded firmware integrity:**
```bash
# Compare MD5 checksums of original and uploaded firmware
md5sum original_firmware.bin received_firmwares/firmware_*.bin

# Example output (checksums should match):
# b50662fd7e58a33a00f1bcdbed24105d  Firmware.bin
# b50662fd7e58a33a00f1bcdbed24105d  received_firmwares/firmware_2.0.0_2025-10-02T15-43-24_Firmware.bin
```

### Size Limits

- **Maximum firmware size**: 2GB (2,147,483,648 bytes)
- Firmware transfers exceeding this limit will be rejected with HTTP 413 (Payload Too Large)
- Progress is logged every 1MB for large firmware transfers (>1MB)
- The simulator handles streaming reception of large files efficiently

## TLS Configuration

The simulator is configured to use **TLS 1.3 only** for maximum security:

- Minimum TLS version: 1.3
- Maximum TLS version: 1.3
- Self-signed certificates for testing (use proper CA certificates in production)

## Security Notes

⚠️ **Important**: The included certificate generation script creates self-signed certificates for **testing purposes only**.

For production deployments:
- Obtain certificates from a trusted Certificate Authority (CA)
- Use proper certificate validation
- Implement client certificate authentication if needed
- Consider using mutual TLS (mTLS) for enhanced security

## Project Structure

```
FirmwareUpdater/
├── device_simulator.cpp      # Device simulator server (HTTPS API)
├── firmware_uploader.cpp     # Firmware upload client
├── CMakeLists.txt           # CMake build configuration
├── build.sh                 # Build script (generates certs, builds project)
├── run.sh                   # Run script for device simulator
├── install_dependencies.sh  # Automated dependency installer
├── generate_certs.sh        # Certificate generation script
├── test_upload.sh           # Automated test script
├── README.md               # This file
├── .gitignore              # Git ignore patterns
├── build/                   # Build directory (after building)
│   ├── device_simulator     # Compiled server
│   ├── firmware_uploader    # Compiled client
│   └── test_upload.sh       # Test script (copied)
├── certs/                   # Generated certificates (after running generate_certs.sh)
│   ├── device.crt          # SSL certificate
│   └── device.key          # Private key
└── received_firmwares/      # Uploaded firmware storage (auto-created)
    └── firmware_*.bin       # Saved firmware files with version and timestamp
```

## Troubleshooting

### OpenSSL Version Issues

Ensure you have OpenSSL 1.1.1 or higher with TLS 1.3 support:

```bash
openssl version
```

### Certificate Errors

If you get certificate errors, regenerate them:

```bash
./generate_certs.sh
```

### Port Already in Use

If port 8443 is already in use, specify a different port:

```bash
./device_simulator --port 9443
```

## License

This is a testing/simulation tool for firmware update development.
