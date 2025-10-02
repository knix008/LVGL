# Quick Start Guide

## Get Started in 3 Steps

```bash
# 1. Build everything
./build.sh

# 2. Run the device simulator
./run.sh
```

**Then in another terminal:**

```bash
# 3. Upload firmware
cd build
./firmware_uploader --version 2.0.0 --url https://example.com/firmware.bin
```

## Scripts Overview

| Script | Description |
|--------|-------------|
| `./install_dependencies.sh` | Install required packages (Ubuntu/Debian/macOS) |
| `./build.sh` | Build both device_simulator and firmware_uploader |
| `./run.sh` | Run the device simulator with proper configuration |
| `./generate_certs.sh` | Generate TLS 1.3 certificates |
| `./test_upload.sh` | Run automated firmware upload tests |

## Common Commands

**Check device status:**
```bash
cd build
./firmware_uploader --status
```

**Upload firmware file:**
```bash
cd build
./firmware_uploader --file firmware.bin --version 3.0.0
```

**Run automated tests:**
```bash
cd build
./test_upload.sh              # Basic tests (1KB, 1MB)
./test_upload.sh --full       # Add 10MB test
./test_upload.sh --full --large  # Add 100MB test
```

**Custom simulator configuration:**
```bash
./run.sh --device-id DEVICE-001 --port 9443 --version 2.0.0
```

## Endpoints

- **Firmware Update:** `https://localhost:8443/api/v1.0/updatefirmware`
- **Status:** `https://localhost:8443/api/v1.0/status`
- **Health Check:** `https://localhost:8443/api/v1.0/health`

## Stop the Simulator

```bash
pkill device_simulator
# or press Ctrl+C in the terminal running it
```

## Troubleshooting

**Port already in use:**
```bash
./run.sh --port 9443
```

**Rebuild everything:**
```bash
rm -rf build
./build.sh
```

**Clean and rebuild:**
```bash
rm -rf build certs
./build.sh
```
