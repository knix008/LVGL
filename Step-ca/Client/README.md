# HTTPS Client with step-ca Certificates

This is a secure HTTPS client implementation using Mongoose with mutual TLS authentication using certificates issued by step-ca.

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

### 1. Build the client
```bash
make all
```

### 2. Ensure step-ca is running

If not already running, start step-ca in a separate terminal:
```bash
cd ..
./bin/step-ca ~/.step/config/ca.json
```

### 3. Generate certificates

```bash
make run-cert-setup
```

This will:
- Download the root CA certificate
- Request a client certificate from step-ca

### 4. Run the client

Basic GET request:
```bash
./build/client -u https://localhost:8443/
```

Or use the test target:
```bash
make test
```

## Usage Examples

### GET requests

```bash
# GET root endpoint
./build/client -u https://localhost:8443/

# GET status endpoint
./build/client -u https://localhost:8443/api/status

# GET info endpoint
./build/client -u https://localhost:8443/api/info
```

### POST requests

```bash
# POST JSON data
./build/client -u https://localhost:8443/api/data \
    -m POST \
    -d '{"name":"test","value":123}'
```

### PUT requests

```bash
# PUT request
./build/client -u https://localhost:8443/api/update \
    -m PUT \
    -d '{"id":1,"status":"updated"}'
```

### DELETE requests

```bash
# DELETE request
./build/client -u https://localhost:8443/api/item/123 \
    -m DELETE
```

## Command Line Options

```bash
./build/client [options]

Options:
  -u <url>      Server URL (default: https://localhost:8443/)
  -m <method>   HTTP method: GET, POST, PUT, DELETE (default: GET)
  -d <data>     POST/PUT data (JSON format)
  -c <cert>     Client certificate file (default: certs/client.crt)
  -k <key>      Client key file (default: certs/client.key)
  -r <ca>       Root CA certificate (default: certs/root_ca.crt)
  -t <timeout>  Request timeout in ms (default: 10000)
  -v <level>    Debug level 0-4 (default: 2)
  -h            Show help
```

## Makefile Targets

- `make all` - Download Mongoose and build client
- `make mongoose` - Download Mongoose library
- `make setup-certs` - Show certificate setup instructions
- `make run-cert-setup` - Automatically generate certificates (requires step-ca running)
- `make test` - Run test requests to the server
- `make test-url URL=<url>` - Test specific URL
- `make test-post URL=<url> DATA=<json>` - Test POST request
- `make clean` - Remove build artifacts
- `make distclean` - Remove build artifacts and certificates

## Testing

### Run all tests
```bash
make test
```

This will test:
1. GET / (root endpoint)
2. GET /api/status
3. GET /api/info

### Test specific URL
```bash
make test-url URL=https://localhost:8443/api/status
```

### Test POST request
```bash
make test-post URL=https://localhost:8443/api/data DATA='{"test":"value"}'
```

## Certificate Management

### View certificate information
```bash
../bin/step certificate inspect certs/client.crt
```

### Renew certificate
```bash
../bin/step ca renew certs/client.crt certs/client.key
```

### Request new certificate
```bash
make run-cert-setup
```

## Project Structure

```
Client/
├── client.c          # Main client implementation
├── Makefile          # Build configuration
├── README.md         # This file
├── mongoose.c        # Mongoose library (downloaded)
├── mongoose.h        # Mongoose header (downloaded)
├── build/            # Compiled binaries
│   └── client        # Client executable
└── certs/            # TLS certificates
    ├── root_ca.crt   # Root CA certificate
    ├── client.crt    # Client certificate
    └── client.key    # Client private key
```

## Integration with Server

To test the full setup:

1. **Terminal 1** - Start step-ca:
   ```bash
   cd /home/shkwon/Projects/LVGL/Step-ca
   ./bin/step-ca ~/.step/config/ca.json
   ```

2. **Terminal 2** - Start the server:
   ```bash
   cd /home/shkwon/Projects/LVGL/Step-ca/Server
   make run
   ```

3. **Terminal 3** - Run the client:
   ```bash
   cd /home/shkwon/Projects/LVGL/Step-ca/Client
   make test
   ```

## Security Features

- **Mutual TLS (mTLS)**: Both client and server authenticate each other
- **Certificate-based authentication**: Client uses certificate issued by step-ca
- **Encrypted communication**: All traffic encrypted with TLS 1.2/1.3
- **CA verification**: Server certificate verified against root CA

## Troubleshooting

### Connection refused
- Ensure the server is running on the specified URL
- Check firewall settings

### Certificate verification failed
- Ensure step-ca is running
- Verify certificates exist in `certs/` directory
- Check certificate expiration: `../bin/step certificate inspect certs/client.crt`
- Ensure root CA certificate matches the server's CA

### Timeout errors
- Increase timeout with `-t <milliseconds>`
- Check network connectivity
- Verify server is responding

### OpenSSL errors
- Ensure OpenSSL development libraries are installed
- Check that `libssl-dev` or `openssl-devel` package is installed

## Advanced Usage

### Custom certificate locations
```bash
./build/client \
    -u https://localhost:8443/api/status \
    -c /path/to/client.crt \
    -k /path/to/client.key \
    -r /path/to/root_ca.crt
```

### Verbose debugging
```bash
./build/client -u https://localhost:8443/ -v 4
```

### Different server
```bash
./build/client -u https://myserver.example.com:8443/api/data
```
