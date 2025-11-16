# Step-CA Manager - GUI Management Application

A graphical user interface for managing your Step-CA Certificate Authority.

## Features

- **CA Status Monitoring**: Real-time monitoring of step-ca service status
- **Certificate Management**:
  - Request new certificates
  - View certificate details
  - Renew certificates
  - Revoke certificates
- **Provisioner Management**: View and manage CA provisioners
- **CA Information**: Display CA fingerprint, root certificate, and configuration
- **System Integration**: Start/stop step-ca service

## Prerequisites

- Python 3.8+
- GTK 3.0+
- Step-CA and Step CLI installed

### Install Dependencies

Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install python3 python3-pip python3-gi python3-gi-cairo gir1.2-gtk-3.0
pip3 install -r requirements.txt
```

## Quick Start

**Recommended Method** (with automatic dependency checking):
```bash
# Check all dependencies are installed
make check-deps

# Install Python dependencies (first time only)
make install-deps

# Run the application
make run

# Optional: Create desktop launcher
make install
```

**Alternative Method** (direct):
```bash
# Install Python dependencies
pip3 install -r requirements.txt

# Run the GUI application
python3 src/main.py
```

**Note**: The Manager automatically detects step and step-ca binaries in the parent directory!

## Usage

### Starting the Application

```bash
cd Manager
python3 src/main.py
```

### Features Overview

1. **Dashboard Tab**:
   - View CA status
   - Display CA fingerprint
   - Show CA information

2. **Certificates Tab**:
   - Request new certificates
   - List existing certificates
   - View certificate details
   - Renew/revoke certificates

3. **Provisioners Tab**:
   - View configured provisioners
   - Add/remove provisioners

4. **Settings Tab**:
   - Configure step-ca path
   - Set CA URL
   - Manage preferences

## Architecture

```
Manager/
├── src/
│   ├── main.py              # Application entry point
│   ├── gui/
│   │   ├── main_window.py   # Main window and layout
│   │   ├── dashboard.py     # Dashboard tab
│   │   ├── certificates.py  # Certificate management tab
│   │   ├── provisioners.py  # Provisioner management tab
│   │   └── settings.py      # Settings tab
│   └── core/
│       ├── stepca.py        # Step-CA integration
│       └── utils.py         # Utility functions
├── assets/                  # Icons and images
├── certs/                   # Certificate storage
├── requirements.txt         # Python dependencies
├── Makefile                 # Build and run commands
└── README.md               # This file
```

## Development

```bash
# Run in development mode with debug output
make dev

# Clean generated files
make clean
```

## Makefile Targets

All available commands:

- `make run` - Run the application (checks dependencies first)
- `make dev` - Run in development mode with GTK debugging
- `make check-deps` - Check if all dependencies are installed
- `make install-deps` - Install Python dependencies
- `make install` - Install desktop launcher
- `make uninstall` - Remove desktop launcher
- `make clean` - Clean generated files
- `make lint` - Run code linter (requires pylint)
- `make format` - Format code (requires black)
- `make help` - Show help message

## Troubleshooting

**Error: ModuleNotFoundError: No module named 'core'**
- **Fixed!** The core module is now included
- If you still see this, the module may be missing. Run `make check-deps` to verify

**Error: KeyError: 'step' or 'step_ca'**
- **Fixed!** The utility functions now return the correct dictionary keys
- Update to the latest version

**Error: TypeError: Argument 1 does not allow None as a value**
- **Fixed!** All GTK widgets now handle None values properly
- Update to the latest version

**Error: GTK not found**
```bash
sudo apt-get install python3-gi python3-gi-cairo gir1.2-gtk-3.0
```

**Error: python-dateutil not found**
```bash
make install-deps
# Or manually: pip3 install python-dateutil
```

**Error: Step-CA not found**
- Build step-ca in the parent directory: `cd .. && make build-step-ca`
- Or ensure step-ca is installed and in PATH
- Configure step-ca path in Settings tab

**Error: Cannot connect to CA**
- Ensure step-ca is running
- Check CA URL in Settings tab
- Verify CA is accessible: `step ca health --ca-url https://localhost:9000`

## Recent Fixes

### Version 1.1 (2025-11-16)

✅ **Core Module Created**
- Added complete `core/stepca.py` with StepCA integration class
- Added `core/utils.py` with utility functions for path detection
- Added `core/__init__.py` for proper module initialization

✅ **GUI Compatibility**
- Fixed dictionary key naming (`step` vs `step_path`)
- Added None value handling for all GTK widgets
- Added missing StepCA methods for GUI compatibility

✅ **Makefile Improvements**
- Added automatic PATH configuration for step binaries
- Enhanced dependency checking with detailed status
- Added safety checks for core module existence

✅ **Error Handling**
- All import errors resolved
- Proper fallbacks for missing configuration
- Clear error messages with actionable solutions

## License

This project is provided as-is for educational and development purposes.
