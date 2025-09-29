# Certificate Authority Server

A complete Certificate Authority (CA) server implementation with REST API and web interface for managing digital certificates.

## Features

- **Root CA Generation**: Create and manage your own Certificate Authority
- **Certificate Management**: Issue, revoke, and list digital certificates
- **CRL Support**: Generate and maintain Certificate Revocation Lists
- **REST API**: Full HTTP API for programmatic access
- **Web Interface**: Modern, responsive web UI with real-time validation
- **Database Storage**: SQLite-based certificate storage and tracking
- **Unified Directory**: All certificates and keys in one organized location
- **Modern Cryptography**: OpenSSL 3.0+ compatible with EVP_PKEY API
- **Clean Build**: Zero warnings with professional-grade code quality
- **Automated Setup**: One-command build with dependency installation
- **JSON Processing**: Includes `jq` for advanced JSON manipulation
- **Troubleshooting**: Comprehensive error handling and debug support

## Prerequisites

- CMake 3.10 or higher
- OpenSSL development libraries
- JSON-C library
- SQLite3 development libraries
- pthread library

### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install cmake build-essential libssl-dev libjson-c-dev libsqlite3-dev
```

### CentOS/RHEL
```bash
sudo yum install cmake gcc openssl-devel json-c-devel sqlite3-devel
```

## Building

```bash
# Clone or download the project
cd CA

# Build the project (automatically installs dependencies)
./build.sh

# Clean all generated files (optional)
./build.sh clean

# Install system-wide (optional)
sudo make install
```

### Build Features

- **Zero Warnings**: Clean compilation with no deprecation warnings
- **Modern OpenSSL**: Uses OpenSSL 3.0+ EVP_PKEY API
- **Auto Dependencies**: Automatically installs required packages including `jq`
- **Clean Build**: Professional-grade code quality
- **Cross Platform**: Works on Ubuntu, Debian, CentOS, RHEL
- **JSON Processing**: Includes `jq` for advanced JSON manipulation

### OpenSSL Modernization

This project uses modern OpenSSL 3.0+ APIs for enhanced security and compatibility:

- **EVP_PKEY API**: Replaces deprecated RSA functions
- **Context Management**: Proper EVP_PKEY_CTX usage
- **Memory Safety**: Enhanced cleanup and error handling
- **Future Proof**: Compatible with latest OpenSSL versions

## Usage

### Starting the CA Server

```bash
# Basic usage (from project root)
./build/ca_server

# Custom configuration
./build/ca_server -p 8080 -k config/ca.key -c config/ca.crt -r config/ca.crl -d config/ca.db
```

### Command Line Options

- `-p port`: HTTP server port (default: 8080)
- `-k key_file`: CA private key file (default: config/ca.key)
- `-c cert_file`: CA certificate file (default: config/ca.crt)
- `-r crl_file`: Certificate Revocation List file (default: config/ca.crl)
- `-d db_file`: Database file (default: config/ca.db)
- `-b key_size`: Key size in bits (default: 4096)
- `-y validity_years`: CA validity period in years (default: 10)

### Configuration

The server loads configuration from `config/ca.conf`. Key settings include:

- **Unified Directory Structure**: All certificate-related files are stored in the `certs/` directory
  - CA files: `ca.crt`, `ca.key`, `ca.db`, `ca.crl`
  - Client certificates: `*.crt` files
  - Client private keys: `*.key` files
- **Country Restriction**: Only certificates with country code "KR" (South Korea) are allowed
- **Validity Period**: Maximum certificate validity is 10 years (3650 days)
- **Security**: Modern OpenSSL 3.0+ EVP_PKEY API for enhanced security

### Unified Directory Structure

The CA server uses a unified directory structure for better organization:

```
certs/                          # All certificate files in one location
├── ca.crt                     # CA certificate
├── ca.key                     # CA private key
├── ca.db                      # Certificate database
├── ca.crl                     # Certificate Revocation List
├── client1.example.com.crt   # Client certificate
├── client1.example.com.key   # Client private key
├── client2.example.com.crt   # Another client certificate
└── client2.example.com.key   # Another client private key
```

**Benefits:**
- **Simplified Management**: Certificate and key pairs are co-located
- **Easier Deployment**: All related files in one directory
- **Better Organization**: Logical grouping of certificate materials
- **Simplified Backup**: Single directory to backup for certificates
- **Reduced Complexity**: No need to track files across multiple directories

### Web Interface

Open your browser and navigate to `http://localhost:8080` to access the modern, responsive web interface.

#### **Features:**
- **Create Certificates**: User-friendly form with validation hints
- **Certificate List**: View all issued certificates with status
- **CA Management**: Download CA certificate and CRL
- **Certificate Revocation**: Revoke certificates with one click

#### **Certificate Creation Requirements:**
- **Common Name**: Required, unique identifier for the certificate
- **Email**: Optional, contact email address
- **Organization**: Required, organization name
- **Country**: Must be "KR" (South Korea) - currently the only allowed country
- **State/Province**: Required, state or province name
- **City**: Required, city name
- **Key Size**: 2048 or 4096 bits (minimum 2048)
- **Validity Period**: 1 to 3650 days (maximum 10 years)

#### **Web Interface Benefits:**
- **Intuitive Design**: Modern, responsive interface
- **Real-time Validation**: Immediate feedback on form inputs
- **Error Handling**: Clear error messages and success notifications
- **Certificate Management**: Full lifecycle management through the browser

### REST API

The CA server provides a comprehensive REST API for programmatic access:

#### **Endpoints:**
- `GET /api/certificates` - List all certificates
- `GET /api/certificates/{id}` - Get certificate details
- `POST /api/certificates` - Create new certificate
- `DELETE /api/certificates/{id}` - Revoke certificate
- `GET /api/crl` - Get Certificate Revocation List
- `GET /api/ca` - Get CA certificate

#### **Certificate Creation Example:**
```bash
curl -X POST http://localhost:8080/api/certificates \
  -H "Content-Type: application/json" \
  -d '{
    "common_name": "example.com",
    "email": "admin@example.com",
    "organization": "Example Corp",
    "country": "KR",
    "state": "Seoul",
    "city": "Seoul",
    "key_size": 2048,
    "validity_days": 365
  }'
```

#### **List Certificates Example:**
```bash
curl http://localhost:8080/api/certificates
```

#### **Download CA Certificate:**
```bash
curl http://localhost:8080/api/ca -o ca.crt
```

### Cleaning Generated Files

To remove all automatically generated files (CA certificates, client certificates, build artifacts):

```bash
./build.sh clean
```

This will:
- Stop any running CA server
- Remove CA files from `certs/` directory
- Remove client certificates from `certs/` directory  
- Remove client private keys from `keys/` directory
- Remove build artifacts from `build/` directory
- Remove temporary test files

## API Endpoints

### GET /api/certificates
List all certificates

**Response:**
```json
{
  "certificates": [
    {
      "serial_number": "abc123...",
      "common_name": "example.com",
      "email": "admin@example.com",
      "status": "valid",
      "created_at": 1640995200,
      "expires_at": 1672531200
    }
  ]
}
```

### GET /api/certificates/{serial_number}
Get certificate details

### POST /api/certificates
Create a new certificate

**Request:**
```json
{
  "common_name": "example.com",
  "email": "admin@example.com",
  "organization": "Example Corp",
  "country": "US",
  "state": "California",
  "city": "San Francisco",
  "key_size": 2048,
  "validity_days": 365
}
```

### DELETE /api/certificates/{serial_number}
Revoke a certificate

### GET /api/crl
Download Certificate Revocation List

### GET /api/ca
Download CA certificate

## Certificate Management

### Creating Certificates via API

```bash
# Create a new certificate
curl -X POST http://localhost:8080/api/certificates \
  -H "Content-Type: application/json" \
  -d '{
    "common_name": "example.com",
    "email": "admin@example.com",
    "organization": "Example Corp",
    "country": "US",
    "state": "California",
    "city": "San Francisco",
    "key_size": 2048,
    "validity_days": 365
  }'
```

### Listing Certificates

```bash
# List all certificates
curl http://localhost:8080/api/certificates
```

### Revoking Certificates

```bash
# Revoke a certificate
curl -X DELETE http://localhost:8080/api/certificates/abc123...
```

### Downloading CA Certificate

```bash
# Download CA certificate
curl http://localhost:8080/api/ca -o ca.crt
```

## File Structure

```
CA/
├── src/                    # Source code
│   ├── main.c             # Main application
│   ├── ca_server.c        # Core CA functionality
│   ├── database.c         # Database operations
│   ├── http_server.c      # HTTP server and API
│   └── config_parser.c    # Configuration file parser
├── include/               # Header files
│   ├── ca_server.h       # CA server definitions
│   ├── http_server.h     # HTTP server definitions
│   └── config_parser.h   # Configuration parser definitions
├── web/                   # Web interface
│   └── index.html        # Web UI
├── config/               # CA configuration
│   └── ca.conf          # CA configuration
├── certs/                # CA files and client certificates/keys storage
│   ├── ca.crt           # CA certificate (generated)
│   ├── ca.key           # CA private key (generated)
│   ├── ca.db            # Certificate database (generated)
│   ├── ca.crl           # Certificate Revocation List (generated)
│   ├── *.crt            # Client certificates (generated)
│   └── *.key            # Client private keys (generated)
├── examples/             # Usage examples
│   └── client_example.py # Python client example
├── build/                # Build artifacts
│   └── ca_server        # Executable
├── CMakeLists.txt        # Build configuration
├── build.sh             # Build script
├── test_ca.sh           # Test script
└── README.md            # This file
```

## Security Considerations

1. **Unified Directory Security**: All sensitive files are in the `certs/` directory - protect this entire directory
2. **Private Key Protection**: Store CA and client private keys securely and restrict access
3. **Database Security**: Protect the SQLite database file in `certs/ca.db`
4. **File Permissions**: Ensure proper permissions on sensitive files:
   ```bash
   # CA files
   chmod 600 certs/ca.key
   chmod 644 certs/ca.crt
   chmod 600 certs/ca.db
   
   # Client files (certificates and keys)
   chmod 644 certs/*.crt
   chmod 600 certs/*.key
   ```
5. **Directory Protection**: Secure the entire `certs/` directory:
   ```bash
   chmod 700 certs/
   ```
6. **Modern Cryptography**: Uses OpenSSL 3.0+ EVP_PKEY API for enhanced security
7. **Memory Safety**: Enhanced cleanup and error handling for secure operations
8. **Network Security**: Use HTTPS in production environments
9. **Access Control**: Implement authentication for production use
10. **Backup**: Regularly backup the entire `certs/` directory
11. **Git Security**: The `.gitignore` file excludes sensitive CA files from version control
12. **Code Quality**: Zero warnings build ensures robust implementation

## Troubleshooting

### Common Issues

#### **"Invalid certificate request" Error**
- **Cause**: Incorrect field names or validation failures
- **Solution**: Ensure country is "KR", key size ≥ 2048, validity ≤ 3650 days
- **Web Interface**: Use the form hints and validation messages

#### **"Address already in use" Error**
- **Cause**: Port 8080 is already in use by another process
- **Solution**: 
  ```bash
  # Kill existing CA server
  pkill -f ca_server
  
  # Or use a different port
  ./build/ca_server -p 8081
  ```

#### **"Failed to load CA certificate" Error**
- **Cause**: CA certificate doesn't exist or is corrupted
- **Solution**: Delete `certs/` directory and restart server to regenerate CA

#### **Web Interface Not Loading**
- **Cause**: Server not running or port blocked
- **Solution**: 
  ```bash
  # Check if server is running
  pgrep -f ca_server
  
  # Start server if not running
  ./build/ca_server
  ```

#### **Certificate Creation Fails**
- **Cause**: Validation errors or file permission issues
- **Solution**: 
  - Check country code is "KR"
  - Ensure key size ≥ 2048
  - Verify validity period ≤ 3650 days
  - Check file permissions on `certs/` directory

### Debug Mode

Enable verbose logging by setting environment variables:
```bash
export CA_DEBUG=1
./build/ca_server
```

## Development

### Adding New Features

1. Add new functions to appropriate source files
2. Update header files with new declarations
3. Add API endpoints in `http_server.c`
4. Update web interface if needed
5. Update documentation

### Testing

```bash
# Build with debug information
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# Run tests (add your test cases)
./ca_server
```

## Troubleshooting

### Common Issues

1. **Build Errors**: Ensure all dependencies are installed
2. **Permission Errors**: Check file permissions for certificates and keys
3. **Port Conflicts**: Use a different port if 8080 is occupied
4. **Database Errors**: Check SQLite installation and permissions

### Logs

The server outputs logs to stdout. For production, redirect to a log file:

```bash
./ca_server > ca.log 2>&1
```

## License

This project is open source. Please check the license file for details.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Submit a pull request

## Support

For issues and questions:
1. Check the troubleshooting section
2. Review the API documentation
3. Open an issue on the project repository
