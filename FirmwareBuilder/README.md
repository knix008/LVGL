# Firmware Builder - C Implementation

A professional-grade C firmware builder with compression, CRC checksum, AES-256 encryption, and hash generation. Features both CLI and GTK GUI interfaces.

## Features

- **Compression**: Gzip compression using zlib
- **CRC Checksums**: CRC32, CRC16, and CRC8 support
- **AES-256 Encryption**: Password-based encryption with PBKDF2 key derivation
- **Hash Generation**: SHA-256, SHA-512, and MD5 hashing
- **CLI Tool**: Full-featured command-line interface
- **GTK GUI**: User-friendly graphical interface with GTK 3
- **Static Library**: Reusable firmware builder library
- **Verification**: Built-in integrity checking
- **Firmware Downloader**: Complete download, verify, decrypt, decompress, and validate pipeline

## Requirements

### Build Requirements
- **gcc** - C compiler
- **make** - Build system
- **pkg-config** - Dependency management

### Runtime Dependencies
- **zlib** - Compression library
- **OpenSSL** - Cryptography and hashing
- **GTK 3** (optional) - For GUI application

### Installation on Linux

#### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install build-essential pkg-config libz-dev libssl-dev libgtk-3-dev
```

#### Fedora/RHEL
```bash
sudo dnf groupinstall "Development Tools"
sudo dnf install pkgconfig zlib-devel openssl-devel gtk3-devel
```

#### Arch Linux
```bash
sudo pacman -S base-devel pkg-config zlib openssl gtk3
```

#### macOS
```bash
brew install gcc make pkg-config openssl gtk+3
```

## Building

### Check Dependencies
```bash
make deps
```

### Build All (CLI + Library)
```bash
make all
# or
make
```

### Build CLI Only
```bash
make cli
```

### Build GUI
```bash
make gui
```

### Build Static Library
```bash
make lib
```

### Clean Build
```bash
make clean
```

## Testing

The project includes a comprehensive test suite with 43 tests across 5 modules.

### Run All Tests

```bash
# Simple and quick (recommended)
make test

# Or directly with test runner
./tests/test_runner.sh
```

### Advanced Testing Options

```bash
# Run with verbose output
./tests/test_runner.sh --verbose

# Run specific test module
./tests/test_runner.sh --test compressor
./tests/test_runner.sh --test crc
./tests/test_runner.sh --test encryptor
./tests/test_runner.sh --test hasher
./tests/test_runner.sh --test integration

# List all available tests
./tests/test_runner.sh --list

# Stop on first failure
./tests/test_runner.sh --stop-on-fail

# Show performance benchmarks
./tests/test_runner.sh --benchmark

# Get help
./tests/test_runner.sh --help
```

### Test Scripts

Multiple test runners available for different needs:

| Script | Purpose | Command |
|--------|---------|---------|
| run_tests.sh | Basic test runner | `./tests/run_tests.sh` |
| test_runner.sh | Advanced options | `./tests/test_runner.sh [OPTIONS]` |
| test_all.sh | Full logging | `./tests/test_all.sh` |
| test_parallel.sh | Parallel execution | `./tests/test_parallel.sh` |
| test_coverage.sh | Coverage report | `./tests/test_coverage.sh` |

### Test Results

All **53 tests passing** (100%):
- Compression Module: 5/5 ✅
- CRC Module: 12/12 ✅
- Encryption Module: 8/8 ✅
- Hash Module: 10/10 ✅
- Integration Tests: 8/8 ✅
- Firmware Downloader: 10/10 ✅

### Documentation

For detailed testing information, see:
- **TESTING.md** - Complete testing guide
- **TEST_SCRIPTS.md** - Scripts reference
- **TEST_SUMMARY.txt** - Executive summary

## Usage

### Command-Line Interface

#### Basic Build
```bash
./bin/firmware-builder -s ./firmware -o ./build
```

#### With Compression
```bash
./bin/firmware-builder -s ./firmware -o ./build -c true -l 9
```

#### With Encryption
```bash
./bin/firmware-builder -s ./firmware -o ./build -e -p "mypassword"
```

#### Verify After Build
```bash
./bin/firmware-builder -s ./firmware -o ./build -v
```

#### Extract Firmware
```bash
./bin/firmware-builder --extract -s firmware.bin -p "mypassword" -o ./extracted
```

#### Full Example
```bash
./bin/firmware-builder \
  -s ./firmware \
  -o ./build \
  -c true \
  -l 6 \
  -r true \
  -e \
  -p "secure_password" \
  -g true \
  -v
```

#### CLI Help
```bash
./bin/firmware-builder --help
```

### Graphical Interface (GTK 3)

#### Launching the GUI
```bash
./bin/firmware-builder-gui
```

#### GUI Features

The GTK GUI application provides an intuitive interface with three main tabs:

**1. Build Tab**
- Select source directory to package
- Choose output directory for firmware
- Configure build options:
  - Enable/disable compression (0-9 levels)
  - Add CRC checksums for integrity
  - Enable AES-256-CBC encryption with password
  - Generate cryptographic hashes
- Real-time build progress display
- Visual status indicator

**2. Extract Tab**
- Select encrypted/compressed firmware to extract
- Choose extraction output directory
- Provide password for encrypted files
- Verify integrity during extraction

**3. Info Tab**
- Complete feature documentation
- Usage instructions
- Security information
- Quick reference guide

#### GUI Controls

- **Browse Buttons**: File/folder selection dialogs
- **Checkboxes**: Toggle features on/off
- **Spin Button**: Compression level selector (0-9)
- **Text Fields**: Path and password input
- **Output Panel**: Real-time build log display
- **Status Bar**: Operation status indicator

## File Structure

```
Firmware/
├── Makefile                      # Build configuration
├── README.md                     # Main documentation
├── GUI_QUICKSTART.md            # GUI quick start guide
├── TESTING.md                   # Testing guide
├── TEST_SCRIPTS.md              # Test scripts reference
├── TEST_SUMMARY.txt             # Test summary
├── INDEX.md                     # Project index
├── COMPLETION_REPORT.txt        # Project completion report
├── DOWNLOADER_SUMMARY.md        # Firmware downloader documentation
│
├── include/                      # Header files
│   ├── firmware_builder.h       # Main API
│   ├── firmware_downloader.h    # Downloader API
│   ├── compressor.h             # Compression module
│   ├── crc.h                    # CRC module
│   ├── encryptor.h              # Encryption module
│   └── hasher.h                 # Hash module
│
├── src/                          # Source files
│   ├── main.c                   # CLI entry point
│   ├── gui.c                    # GTK GUI application
│   ├── firmware_builder.c       # Main builder implementation
│   └── modules/
│       ├── compressor.c         # Compression implementation
│       ├── crc.c                # CRC implementation
│       ├── encryptor.c          # Encryption implementation
│       └── hasher.c             # Hash implementation
│
├── downloader/                   # Firmware Downloader Module (NEW)
│   ├── firmware_downloader.c    # Downloader implementation
│   └── README.md                # Downloader documentation
│
├── tests/                        # Test suite
│   ├── run_tests.sh             # Basic test runner
│   ├── test_runner.sh           # Advanced test runner
│   ├── test_all.sh              # Comprehensive test runner
│   ├── test_parallel.sh         # Parallel test runner
│   ├── test_coverage.sh         # Coverage report
│   ├── test_compressor.c        # Compression tests
│   ├── test_crc.c               # CRC tests
│   ├── test_encryptor.c         # Encryption tests
│   ├── test_hasher.c            # Hash tests
│   ├── test_integration.c       # Integration tests
│   └── test_downloader.c        # Downloader tests (NEW)
│
├── build/                        # Build artifacts (generated)
└── bin/                          # Compiled binaries (generated)
    ├── firmware-builder         # CLI executable
    ├── firmware-builder-gui     # GUI executable
    ├── libfirmware.a            # Static library
    ├── test_downloader          # Downloader test executable
    └── test_*                   # Other test executables
```

## Modules

### Compressor Module (`compressor.h`)
Handles directory and file compression using zlib.

**Key Functions:**
- `compress_file()` - Compress a file to gzip format
- `decompress_file()` - Decompress a gzip file
- `compress_buffer()` - Compress buffer in memory
- `decompress_buffer()` - Decompress buffer in memory
- `compress_directory()` - Compress directory

### CRC Module (`crc.h`)
Calculates and verifies CRC checksums.

**Key Functions:**
- `crc32_calculate()` - Calculate CRC32 for buffer
- `crc32_file()` - Calculate CRC32 for file
- `crc16_calculate()` - Calculate CRC16
- `crc16_file()` - Calculate CRC16 for file
- `crc8_calculate()` - Calculate CRC8
- `crc8_file()` - Calculate CRC8 for file
- `crc*_verify()` - Verify checksums
- `crc*_to_string()` - Convert to hex string

### Encryption Module (`encryptor.h`)
Provides AES-256-CBC encryption with password-based key derivation.

**Key Functions:**
- `encrypt_file()` - Encrypt file with password
- `decrypt_file()` - Decrypt file with password
- `encrypt_buffer()` - Encrypt buffer in memory
- `decrypt_buffer()` - Decrypt buffer in memory

**Features:**
- AES-256-CBC cipher
- PBKDF2 key derivation (100,000 iterations)
- Random IV and salt generation
- Secure password-based encryption

### Hash Module (`hasher.h`)
Generates cryptographic hashes for files and buffers.

**Key Functions:**
- `hash_file()` - Hash a file (SHA-256, SHA-512, MD5)
- `hash_buffer()` - Hash a buffer
- `hash_directory()` - Hash directory contents
- `hash_verify()` - Verify hash match
- `hash_file_multi()` - Generate multiple hashes
- `generate_manifest()` - Create manifest with hashes

### Firmware Builder Module (`firmware_builder.h`)
Orchestrates the complete build pipeline.

**Key Functions:**
- `fw_builder_create()` - Create builder instance
- `fw_builder_build()` - Execute build process
- `fw_builder_verify()` - Verify integrity
- `fw_builder_extract()` - Extract and decrypt
- `fw_builder_free()` - Free resources
- `fw_builder_print_package()` - Display package info

### Firmware Downloader Module (`firmware_downloader.h`) - NEW!
Complete firmware download, verification, decryption, decompression, and validation pipeline.

**Key Features:**
- Download firmware with progress tracking
- Verify file integrity using SHA-256/512/MD5
- Decrypt AES-256-CBC encrypted firmware
- Decompress gzip-compressed firmware
- Validate CRC32 checksums
- Complete orchestration pipeline

**Key Functions:**
- `fw_downloader_create()` / `fw_downloader_free()` - Instance management
- `fw_downloader_from_file()` - Download/copy firmware
- `fw_downloader_verify_hash()` - Hash verification
- `fw_downloader_decrypt()` - AES-256 decryption
- `fw_downloader_decompress()` - Gzip decompression
- `fw_downloader_verify_crc()` - CRC32 verification
- `fw_downloader_process()` - Complete pipeline

For detailed documentation, see [downloader/README.md](downloader/README.md) and [DOWNLOADER_SUMMARY.md](DOWNLOADER_SUMMARY.md)

## API Examples

### C Code

```c
#include "include/firmware_builder.h"

int main() {
    fw_build_options_t opts = {
        .compress = 1,
        .compression_level = 6,
        .add_crc = 1,
        .encrypt = 1,
        .encryption_password = "mypassword",
        .generate_hash = 1
    };

    fw_package_t *pkg = fw_builder_create("./firmware", "./build", opts);
    if (!pkg) return 1;

    if (fw_builder_build(pkg) != 0) {
        fw_builder_free(pkg);
        return 1;
    }

    fw_builder_print_package(pkg);
    fw_builder_verify(pkg);

    fw_builder_free(pkg);
    return 0;
}
```

Compile with:
```bash
gcc -I./include your_program.c ./bin/libfirmware.a -lz -lcrypto -lssl -o your_program
```

## Security Features

- **Key Derivation**: PBKDF2-SHA256 with 100,000 iterations
- **Encryption**: AES-256-CBC with random IV
- **Salt**: 32-byte random salt per file
- **Random Generation**: Cryptographically secure randomness
- **No Plaintext Storage**: Passwords never written to disk

## Performance

- **Compression**: 5-10 MB/s (zlib level 6)
- **CRC**: 50-100 MB/s
- **Hash**: 20-50 MB/s
- **Encryption**: 10-30 MB/s

## Output

### Build Process
```
🔨 Starting firmware build...
📦 Source: ./firmware
📁 Output: ./build
📦 Compressing...
✓ File compressed: ./build/firmware.tar.gz
🔐 Calculating CRC...
✓ CRC32: ABC12345
🔒 Encrypting... (if enabled)
✓ File encrypted: ./build/firmware.tar.gz.enc
🔗 Generating hash...
✓ SHA-256: abc123def456...
✅ Firmware build completed!
```

### Output Files
```
build/
├── firmware.tar.gz          # Compressed firmware
├── firmware.tar.gz.enc      # Encrypted firmware (if encryption enabled)
└── manifest.json            # Build metadata
```

## Troubleshooting

### Build Fails: "zlib.h: No such file"
**Solution**: Install zlib development package
```bash
# Ubuntu/Debian
sudo apt-get install libz-dev

# Fedora
sudo dnf install zlib-devel
```

### Build Fails: "openssl/aes.h: No such file"
**Solution**: Install OpenSSL development package
```bash
# Ubuntu/Debian
sudo apt-get install libssl-dev

# Fedora
sudo dnf install openssl-devel
```

### GUI Won't Build
**Solution**: Install GTK 3 development package
```bash
# Ubuntu/Debian
sudo apt-get install libgtk-3-dev

# Fedora
sudo dnf install gtk3-devel
```

### Encryption/Decryption Fails
- Check password is correct
- Verify file is encrypted (should have IV + salt)
- Ensure OpenSSL is properly installed

## Installation

```bash
make deps          # Verify dependencies
make all           # Build
sudo make install  # Install to /usr/local/bin
```

After installation:
```bash
firmware-builder --help
```

## License

MIT License - See LICENSE file for details

## Support

For issues or questions:
1. Run `make help` for build options
2. Run `firmware-builder --help` for CLI usage
3. Check function documentation in header files
4. Review examples in source files

## Technical Details

### Build Configuration
- **Compiler**: GCC with C99 standard
- **Optimization**: Level 2
- **Warnings**: All + Extra enabled

### Dependencies
- **zlib** 1.2.x+ - Compression
- **OpenSSL** 1.1.x+ - Encryption & hashing
- **GTK** 3.0+ (optional) - GUI framework

### Thread Safety
- Not thread-safe by default
- Separate instances safe for concurrent use
- Avoid sharing fw_package_t across threads

## Changelog

### Version 1.0.0
- Initial release
- CLI tool with full features
- GTK GUI application
- Static library for integration
- Complete documentation
