# Step-CA HTTPS Server/Client Project

A complete implementation of secure HTTPS server and client using Mongoose library with mutual TLS (mTLS) authentication, with certificates issued by step-ca.

## Project Structure

```
Step-ca/
├── Makefile              # Build and install step-ca (improved, no bootstrap needed!)
├── .gitignore            # Git ignore rules
├── README.md             # This file
├── CHANGELOG.md          # Version history and changes
├── bin/                  # Installed step-ca binaries
│   ├── step-ca
│   └── step
├── build/                # Build directory (auto-created)
│   ├── certificates/     # Step-ca source
│   └── cli/              # Step CLI source
├── Manager/              # Step-CA Manager GUI (GTK)
│   ├── Makefile
│   ├── README.md
│   ├── src/
│   │   ├── main.py
│   │   ├── core/         # Step-CA integration
│   │   └── gui/          # GTK interface
│   └── requirements.txt
├── Server/               # HTTPS Server (C implementation)
│   ├── server.c
│   ├── Makefile
│   ├── README.md
│   ├── build/
│   └── certs/
├── ServerGUI/            # HTTPS Server Control Panel (GTK) (NEW!)
│   ├── Makefile
│   ├── README.md
│   ├── src/
│   │   ├── main.py
│   │   ├── core/         # Server management
│   │   └── gui/          # GTK interface
│   └── requirements.txt
├── Client/               # HTTPS Client (C implementation)
│   ├── client.c
│   ├── Makefile
│   ├── README.md
│   ├── build/
│   └── certs/
└── ClientGUI/            # HTTPS Client GUI (GTK) (NEW!)
    ├── Makefile
    ├── README.md
    ├── src/
    │   ├── main.py
    │   ├── core/         # Client wrapper
    │   └── gui/          # GTK interface
    └── requirements.txt
```

## Features

- **Step-CA**: Private Certificate Authority for issuing certificates
- **GUI Applications**: Three GTK-based graphical interfaces:
  - **Step-CA Manager**: Manage certificates and provisioners
  - **Server Control Panel**: Monitor and control HTTPS server (NEW!)
  - **Client GUI**: Visual HTTPS request builder (NEW!)
- **HTTPS Server**: Secure web server with TLS support (C implementation)
- **HTTPS Client**: Client with certificate-based authentication (C implementation)
- **Mutual TLS (mTLS)**: Both server and client authenticate each other
- **Mongoose**: Lightweight embedded web server/client library
- **C/C++**: Native implementation for performance
- **Python/GTK**: Modern GUI applications for ease of use
- **Improved Build System**: Direct Go builds without bootstrap dependencies

## Prerequisites

### System Requirements
- Linux (tested on Ubuntu/Debian)
- GCC compiler
- Go 1.21+ (for building step-ca)
- OpenSSL development libraries
- wget or curl

### Install Dependencies

Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install build-essential libssl-dev wget git
```

Fedora/RHEL:
```bash
sudo yum install gcc gcc-c++ openssl-devel wget git
```

## Quick Start

### Option 1: Complete GUI Setup (Easiest!)

```bash
# 1. Build step-ca and step CLI (improved build - no GoReleaser needed!)
make all

# 2. Launch the Step-CA Manager
cd Manager
make install-deps  # First time only
make run
```

The Manager GUI provides:
- Visual CA status monitoring
- Certificate management (request, renew, revoke)
- Provisioner management
- Easy configuration

**Additional GUI Applications:**

```bash
# Server Control Panel - Monitor and control the HTTPS server
cd ServerGUI
make install-deps  # First time only
make run

# Client GUI - Visual HTTPS request builder
cd ClientGUI
make install-deps  # First time only
make run
```

### Option 2: Automated Setup (Command Line)

```bash
# 1. Build step-ca and step CLI
make all

# 2. Run the setup script (interactive)
./setup-step-ca.sh
```

The setup script will:
- Initialize your Certificate Authority
- Create a startup script
- Optionally create a systemd service
- Display next steps

### Option 3: Quick Start for Testing

```bash
# 1. Build step-ca
make all

# 2. Quick initialization and start (uses default password: changeme)
./quick-start.sh
```

⚠️ **Warning**: The quick-start script uses a default password and is for development/testing only!

### Option 4: Manual Setup

```bash
# 1. Build and install step-ca and step CLI
make all

# 2. Initialize the Certificate Authority
./step ca init

# Follow the prompts:
# - Choose a name for your CA (e.g., "My Local CA")
# - Set DNS names (e.g., localhost)
# - Set your email address
# - Choose a password for your CA

# 3. Start step-ca
./step-ca ~/.step/config/ca.json
```

Leave this running - it's your Certificate Authority server.

## Helper Scripts

Three helper scripts are provided for managing step-ca:

### setup-step-ca.sh
Interactive setup script for production use:
```bash
./setup-step-ca.sh
```
- Initializes CA with your settings
- Creates startup script
- Optionally creates systemd service
- Secure password handling

### quick-start.sh
Quick setup for development/testing:
```bash
./quick-start.sh
```
- One-command initialization and start
- Uses default password (changeme)
- Perfect for quick testing
- Not for production use

### step-ca-helper.sh
Common operations helper:
```bash
# Check if step-ca is running
./step-ca-helper.sh status

# Get CA fingerprint
./step-ca-helper.sh fingerprint

# Request a new certificate
./step-ca-helper.sh cert myserver.local

# Inspect a certificate
./step-ca-helper.sh inspect server.crt

# Renew a certificate
./step-ca-helper.sh renew server.crt server.key

# List provisioners
./step-ca-helper.sh list

# Show all commands
./step-ca-helper.sh help
```

## Starting step-ca

After setup, you can start step-ca using:

```bash
# Using the generated startup script
./start-step-ca.sh

# Or manually
./step-ca ~/.step/config/ca.json

# If you created a systemd service
sudo systemctl start step-ca
sudo systemctl status step-ca
```

### 4. Build and Run the HTTPS Server

In a new terminal:
```bash
cd Server

# Build the server
make all

# Generate server certificates
make run-cert-setup

# Start the server
make run
```

The server will be running on https://localhost:8443

### 5. Build and Run the HTTPS Client

In another terminal:
```bash
cd Client

# Build the client
make all

# Generate client certificates
make run-cert-setup

# Run tests
make test
```

## Detailed Usage

### Step-CA Management

```bash
# View step-ca version
./bin/step-ca version

# View step CLI version
./bin/step version

# Bootstrap (trust) the CA on your system
./bin/step ca bootstrap --ca-url https://localhost:9000 --fingerprint <fingerprint>

# Get root CA certificate
./bin/step ca root root_ca.crt

# Request a certificate
./bin/step ca certificate <name> cert.crt key.key
```

### Server Usage

```bash
cd Server

# Build
make all

# Setup certificates
make run-cert-setup

# Run server (default: https://0.0.0.0:8443)
make run

# Run with custom settings
./build/server -a https://0.0.0.0:9443 -v 4
```

Available endpoints:
- `GET /` - Home page with server info
- `GET /api/status` - JSON status response
- `GET /api/info` - JSON server information

### Client Usage

```bash
cd Client

# Build
make all

# Setup certificates
make run-cert-setup

# Run automated tests
make test

# Custom GET request
./build/client -u https://localhost:8443/api/status

# POST request
./build/client -u https://localhost:8443/api/data -m POST -d '{"key":"value"}'

# PUT request
./build/client -u https://localhost:8443/api/update -m PUT -d '{"id":1}'

# DELETE request
./build/client -u https://localhost:8443/api/item/123 -m DELETE
```

## Testing the Complete Setup

### Terminal 1: Start step-ca
```bash
./bin/step-ca ~/.step/config/ca.json
```

### Terminal 2: Start Server
```bash
cd Server
make run
```

### Terminal 3: Run Client
```bash
cd Client
make test
```

You should see successful HTTPS requests with mutual TLS authentication!

## Certificate Management

### View Certificate Information
```bash
./bin/step certificate inspect Server/certs/server.crt
./bin/step certificate inspect Client/certs/client.crt
```

### Renew Certificates
```bash
# Renew server certificate
./bin/step ca renew Server/certs/server.crt Server/certs/server.key

# Renew client certificate
./bin/step ca renew Client/certs/client.crt Client/certs/client.key
```

### Check Certificate Expiration
```bash
./bin/step certificate inspect --format=json Server/certs/server.crt | grep -A2 validity
```

## Makefile Targets

### Root Directory (Improved!)
- `make all` - Build step-ca and step CLI (no bootstrap needed!)
- `make build-step-ca` - Build only step-ca
- `make build-step-cli` - Build only step CLI
- `make install` - Install binaries to ./bin/
- `make clean` - Remove build directory and binaries
- `make clean-build` - Remove build directory only
- `make rebuild` - Clean and rebuild everything
- `make test-step-ca` - Test step-ca binary
- `make test-step-cli` - Test step CLI binary
- `make help` - Show help information

**Note**: The build system has been improved to build directly with `go build` - no GoReleaser Pro license needed!

### Manager Directory
- `make run` - Run the Step-CA Manager GUI
- `make check-deps` - Check all dependencies
- `make install-deps` - Install Python dependencies
- `make install` - Install desktop launcher
- `make clean` - Clean generated files
- `make help` - Show help information

### ServerGUI Directory (NEW!)
- `make run` - Run the Server Control Panel GUI
- `make dev` - Run in development mode with debug output
- `make check-deps` - Check all dependencies
- `make install-deps` - Install Python dependencies
- `make build-server` - Build the C server executable
- `make clean` - Clean generated files
- `make help` - Show help information

### ClientGUI Directory (NEW!)
- `make run` - Run the Client GUI
- `make dev` - Run in development mode with debug output
- `make check-deps` - Check all dependencies
- `make install-deps` - Install Python dependencies
- `make build-client` - Build the C client executable
- `make clean` - Clean generated files
- `make help` - Show help information

### Server Directory (C implementation)
- `make all` - Build server
- `make run` - Run server
- `make setup-certs` - Show certificate setup instructions
- `make run-cert-setup` - Generate certificates
- `make clean` - Remove build artifacts

### Client Directory (C implementation)
- `make all` - Build client
- `make test` - Run automated tests
- `make setup-certs` - Show certificate setup instructions
- `make run-cert-setup` - Generate certificates
- `make clean` - Remove build artifacts

## GUI Application Workflows

### Complete GUI Setup

For the best experience, use all three GUI applications together:

**1. Terminal 1 - Start Step-CA Manager:**
```bash
cd Manager
make run
# Initialize and start step-ca through the GUI
```

**2. Terminal 2 - Start Server Control Panel:**
```bash
cd ServerGUI
make run
# Click "Start Server" to launch the HTTPS server
```

**3. Terminal 3 - Start Client GUI:**
```bash
cd ClientGUI
make run
# Make HTTPS requests to the server through the GUI
```

### Example Workflow

1. **Setup CA**: Use the Manager to initialize step-ca
2. **Generate Certificates**: Use Manager or command line to create server/client certificates
3. **Start Server**: Use ServerGUI to start and monitor the HTTPS server
4. **Make Requests**: Use ClientGUI to send HTTPS requests to the server
5. **Monitor**: Watch logs in ServerGUI to see incoming requests

### GUI vs Command Line

| Task | GUI Application | Command Line |
|------|----------------|--------------|
| Manage step-ca | Manager | `./bin/step ca ...` |
| Control server | ServerGUI | `cd Server && make run` |
| Make requests | ClientGUI | `cd Client && ./build/client ...` |
| View certificates | Manager | `./bin/step certificate inspect ...` |
| Monitor server | ServerGUI (live logs) | Manual log checking |

## Security Considerations

1. **Private Keys**: Never commit `.key` files to version control
2. **Certificates**: Store certificates securely
3. **CA Password**: Keep your CA password safe
4. **Production**: For production use, review and harden configurations
5. **Certificate Lifetime**: step-ca issues short-lived certificates by default (24 hours)
6. **Renewal**: Set up automatic certificate renewal for production

## Troubleshooting

### Step-CA Issues

**Error: CA not initialized**
```bash
./bin/step ca init
```

**Error: Connection refused**
- Ensure step-ca is running
- Check the CA URL (default: https://localhost:9000)

### Server Issues

**Error: Port already in use**
- Change the port: `./build/server -a https://0.0.0.0:9443`

**Error: Certificate not found**
- Run `make run-cert-setup` in Server directory

**Error: OpenSSL not found**
- Install: `sudo apt-get install libssl-dev`

### Client Issues

**Error: Connection failed**
- Ensure server is running
- Check the URL and port

**Error: Certificate verification failed**
- Ensure root CA matches between client and server
- Check certificate expiration

### Build Issues

**Error: Go not found**
- Run `make install-go` or install Go manually

**Error: goreleaser.tar.gz not in gzip format**
- This error has been fixed! The new Makefile builds directly with `go build`
- Simply run `make all` or `make rebuild`

**Error: Compilation failed**
- Ensure build-essential is installed
- Check that OpenSSL dev libraries are installed

### Manager Issues

**Error: ModuleNotFoundError: No module named 'core'**
- This has been fixed! The core module is now included
- If you still see this, try: `cd Manager && make check-deps`

**Error: GTK not found**
- Install: `sudo apt-get install python3-gi python3-gi-cairo gir1.2-gtk-3.0`
- Run: `cd Manager && make check-deps` to verify

## Advanced Configuration

### Custom step-ca Configuration

Edit `~/.step/config/ca.json` to customize:
- Port and address
- Certificate lifetime
- Key types and sizes
- Certificate templates

### Server Configuration

Modify `Server/server.c` to:
- Add custom API endpoints
- Implement authentication
- Add logging
- Customize response formats

### Client Configuration

Modify `Client/client.c` to:
- Add custom headers
- Implement retry logic
- Add response parsing
- Handle different content types

## Resources

- [step-ca Documentation](https://smallstep.com/docs/step-ca)
- [Mongoose Documentation](https://mongoose.ws/)
- [TLS Best Practices](https://wiki.mozilla.org/Security/Server_Side_TLS)

## License

This project is provided as-is for educational and development purposes.

## Contributing

Feel free to submit issues and enhancement requests!

## Author

Created for secure local development and testing with step-ca.
