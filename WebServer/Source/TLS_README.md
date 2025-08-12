# TLS Configuration for WebServer

This project uses Mongoose's built-in TLS 1.3 implementation for secure HTTPS connections.

## Overview

The WebServer supports both HTTP and HTTPS connections:
- **HTTP**: Port 8080 (default)
- **HTTPS**: Port 8443 (default)

## TLS Backend

This project uses **Mongoose's built-in TLS** (`MG_TLS_BUILTIN`), which provides:
- TLS 1.3 support
- No external dependencies (no OpenSSL required)
- Smaller footprint
- Better integration with Mongoose

## Certificate Management

### Development Certificates

For development, the server automatically generates self-signed ECC certificates if they don't exist:

```bash
# Manual certificate generation
cd Source
./generate_certs.sh
```

This creates:
- `certs/server.key` - ECC private key (prime256v1 curve)
- `certs/server.crt` - Self-signed certificate
- `certs/ca.crt` - CA certificate (same as server cert for self-signed)

### Production Certificates

For production use, replace the self-signed certificates with certificates from a trusted Certificate Authority (e.g., Let's Encrypt).

## Configuration

### TLS Settings

The TLS configuration is defined in `include/tls_config.h`:

```c
#define MG_TLS MG_TLS_BUILTIN        // Use built-in TLS
#define TLS_PORT 8443                // HTTPS port
#define HTTP_PORT 8080               // HTTP port
#define TLS_CERT_FILE "certs/server.crt"
#define TLS_KEY_FILE "certs/server.key"
```

### Building with TLS

The project is configured to build with built-in TLS support:

```bash
cd Source
mkdir build && cd build
cmake ..
make
```

## Usage

### Starting the Server

The server automatically starts both HTTP and HTTPS listeners:

```bash
# From Source/build directory
./main
```

### Accessing the Web Interface

- **HTTP**: http://localhost:8080
- **HTTPS**: https://localhost:8443

### Browser Security Warning

When using self-signed certificates, browsers will show a security warning. You can:
1. Click "Advanced" and "Proceed to localhost"
2. Add the certificate to your browser's trusted certificates
3. Use `curl` with `-k` flag for testing: `curl -k https://localhost:8443`

## TLS Features

### Supported Protocols
- TLS 1.3 (primary)
- TLS 1.2 (fallback)

### Cipher Suites
- TLS_AES_128_GCM_SHA256
- TLS_AES_256_GCM_SHA384
- TLS_CHACHA20_POLY1305_SHA256

### Key Exchange
- X25519 (preferred)
- P-256
- P-384

## Troubleshooting

### Certificate Issues

If you encounter certificate errors:

1. **Regenerate certificates**:
   ```bash
   cd Source
   rm -rf certs
   ./generate_certs.sh
   ```

2. **Check certificate validity**:
   ```bash
   openssl x509 -in certs/server.crt -text -noout
   ```

3. **Verify key matches certificate**:
   ```bash
   openssl x509 -noout -modulus -in certs/server.crt | openssl md5
   openssl rsa -noout -modulus -in certs/server.key | openssl md5
   ```

### Build Issues

If you encounter build errors:

1. **Clean build**:
   ```bash
   cd Source/build
   make clean-all
   cmake ..
   make
   ```

2. **Check TLS configuration**:
   - Ensure `MG_TLS=MG_TLS_BUILTIN` is set
   - Verify no OpenSSL dependencies remain

### Runtime Issues

If the HTTPS server fails to start:

1. **Check port availability**:
   ```bash
   netstat -tlnp | grep :8443
   ```

2. **Check certificate permissions**:
   ```bash
   ls -la certs/
   ```

3. **Enable debug output**:
   - Add `#define MG_ENABLE_LOG 1` to see detailed TLS handshake logs

## Security Considerations

### Development
- Self-signed certificates are suitable for development only
- No certificate validation is performed
- Traffic is encrypted but not authenticated

### Production
- Use certificates from a trusted CA
- Enable certificate validation
- Consider implementing certificate pinning
- Regularly update certificates

## References

- [Mongoose TLS Documentation](https://mongoose.ws/documentation/tutorials/tls/)
- [TLS 1.3 RFC 8446](https://tools.ietf.org/html/rfc8446)
- [Let's Encrypt](https://letsencrypt.org/) - Free SSL certificates
