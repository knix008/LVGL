# HTTPS Server with step-ca Certificates

This is a secure HTTPS server implementation using Mongoose and certificates issued by step-ca.

## Prerequisites

- GCC compiler
- OpenSSL development libraries
- step-ca installed (run `make install` in parent directory)

### Install OpenSSL development libraries

On Ubuntu/Debian:
```bash
sudo apt-get install libssl-dev
```

On Fedora/RHEL:
```bash
sudo yum install openssl-devel
```

## Quick Start

### 1. Build the server
```bash
make all
```

### 2. Setup step-ca (first time only)

Initialize step-ca:
```bash
cd ..
./bin/step ca init
```

Follow the prompts to configure your CA.

### 3. Start step-ca

In a separate terminal:
```bash
cd ..
./bin/step-ca ~/.step/config/ca.json
```

### 4. Generate certificates

Back in the Server directory:
```bash
make run-cert-setup
```

This will:
- Download the root CA certificate
- Request a server certificate for localhost

### 5. Run the server

```bash
make run
```

The server will start on https://localhost:8443

## Testing the Server

### Using curl
```bash
# Test with the CA certificate
curl --cacert certs/root_ca.crt https://localhost:8443

# Test status endpoint
curl --cacert certs/root_ca.crt https://localhost:8443/api/status

# Test info endpoint
curl --cacert certs/root_ca.crt https://localhost:8443/api/info
```

### Using a web browser

1. Import the root CA certificate (`certs/root_ca.crt`) into your browser's trusted certificates
2. Navigate to https://localhost:8443

## Available Endpoints

- `/` - Home page with server information
- `/api/status` - JSON status response
- `/api/info` - JSON server information

## Command Line Options

```bash
./build/server [options]
  -a <addr>  Listening address (default: https://0.0.0.0:8443)
  -c <cert>  TLS certificate file (default: certs/server.crt)
  -k <key>   TLS key file (default: certs/server.key)
  -r <ca>    Root CA certificate (default: certs/root_ca.crt)
  -v <level> Debug level 0-4 (default: 2)
```

## Certificate Renewal

step-ca certificates have a limited lifetime. To renew:

```bash
../bin/step ca renew certs/server.crt certs/server.key
```

Or request a new certificate:
```bash
make run-cert-setup
```

## Makefile Targets

- `make all` - Download Mongoose and build server
- `make mongoose` - Download Mongoose library
- `make setup-certs` - Show certificate setup instructions
- `make run-cert-setup` - Automatically generate certificates (requires step-ca running)
- `make run` - Run the server
- `make clean` - Remove build artifacts
- `make distclean` - Remove build artifacts and certificates

## Project Structure

```
Server/
├── server.c           # Main server implementation
├── Makefile          # Build configuration
├── README.md         # This file
├── mongoose.c        # Mongoose library (downloaded)
├── mongoose.h        # Mongoose header (downloaded)
├── build/            # Compiled binaries
│   └── server        # Server executable
└── certs/            # TLS certificates
    ├── root_ca.crt   # Root CA certificate
    ├── server.crt    # Server certificate
    └── server.key    # Server private key
```

## Security Notes

- The server uses TLS 1.2/1.3 for secure connections
- Certificates are issued by your local step-ca instance
- Private keys are stored locally and should be kept secure
- The default configuration listens on all interfaces (0.0.0.0) - adjust for production use

## Troubleshooting

### "Failed to create listener" error
- Check if port 8443 is already in use
- Try using a different port with `-a https://0.0.0.0:9443`

### Certificate errors
- Ensure step-ca is running
- Verify certificates exist in `certs/` directory
- Check certificate expiration: `../bin/step certificate inspect certs/server.crt`

### OpenSSL errors
- Ensure OpenSSL development libraries are installed
- Check that `libssl-dev` or `openssl-devel` package is installed
