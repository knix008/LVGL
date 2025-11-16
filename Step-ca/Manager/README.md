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

```bash
# Install Python dependencies
pip3 install -r requirements.txt

# Run the GUI application
python3 src/main.py
```

Or use the Makefile:
```bash
# Install dependencies
make install-deps

# Run the application
make run

# Create desktop launcher
make install
```

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

## Troubleshooting

**Error: GTK not found**
```bash
sudo apt-get install python3-gi python3-gi-cairo gir1.2-gtk-3.0
```

**Error: Step-CA not found**
- Ensure step-ca is installed and in PATH
- Configure step-ca path in Settings tab

**Error: Cannot connect to CA**
- Ensure step-ca is running
- Check CA URL in Settings tab

## License

This project is provided as-is for educational and development purposes.
