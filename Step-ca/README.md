# Step-CA HTTPS Server/Client Project

A complete implementation of secure HTTPS server and client using Mongoose library with mutual TLS (mTLS) authentication, with certificates issued by step-ca.

## Project Structure

```
Step-ca/
├── Makefile              # Build and install step-ca
├── .gitignore            # Git ignore rules
├── README.md             # This file
├── bin/                  # Installed step-ca binaries
│   ├── step-ca
│   └── step
├── Server/               # HTTPS Server implementation
│   ├── server.c
│   ├── Makefile
│   ├── README.md
│   ├── build/
│   └── certs/
└── Client/               # HTTPS Client implementation
    ├── client.c
    ├── Makefile
    ├── README.md
    ├── build/
    └── certs/
```

## Features

- **Step-CA**: Private Certificate Authority for issuing certificates
- **HTTPS Server**: Secure web server with TLS support
- **HTTPS Client**: Client with certificate-based authentication
- **Mutual TLS (mTLS)**: Both server and client authenticate each other
- **Mongoose**: Lightweight embedded web server/client library
- **C/C++**: Native implementation for performance

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

### 1. Install step-ca

```bash
# Build and install step-ca and step CLI
make install
```

This will:
- Check/install Go if needed
- Clone and build step-ca
- Clone and build step CLI
- Install binaries to `./bin/`

### 2. Initialize step-ca

```bash
# Initialize the Certificate Authority
./bin/step ca init

# Follow the prompts:
# - Choose a name for your CA (e.g., "My Local CA")
# - Set DNS names (e.g., localhost)
# - Set your email address
# - Choose a password for your CA
```

### 3. Start step-ca

In a dedicated terminal:
```bash
./bin/step-ca ~/.step/config/ca.json
```

Leave this running - it's your Certificate Authority server.

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

### Root Directory
- `make all` - Build step-ca and step CLI
- `make install` - Install binaries to ./bin/
- `make clean` - Remove build artifacts
- `make help` - Show help information

### Server Directory
- `make all` - Build server
- `make run` - Run server
- `make setup-certs` - Show certificate setup instructions
- `make run-cert-setup` - Generate certificates
- `make clean` - Remove build artifacts

### Client Directory
- `make all` - Build client
- `make test` - Run automated tests
- `make setup-certs` - Show certificate setup instructions
- `make run-cert-setup` - Generate certificates
- `make clean` - Remove build artifacts

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

**Error: Compilation failed**
- Ensure build-essential is installed
- Check that OpenSSL dev libraries are installed

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
