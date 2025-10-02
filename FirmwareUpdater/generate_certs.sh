#!/bin/bash

# Script to generate self-signed TLS certificates for device simulator

CERTS_DIR="certs"
DAYS_VALID=365

# Create certs directory if it doesn't exist
mkdir -p "$CERTS_DIR"

echo "Generating self-signed TLS certificate for device simulator..."

# Generate private key
openssl genrsa -out "$CERTS_DIR/device.key" 2048

# Generate certificate signing request (CSR)
openssl req -new -key "$CERTS_DIR/device.key" -out "$CERTS_DIR/device.csr" \
    -subj "/C=US/ST=State/L=City/O=Organization/OU=DeviceSimulator/CN=localhost"

# Generate self-signed certificate
openssl x509 -req -days $DAYS_VALID \
    -in "$CERTS_DIR/device.csr" \
    -signkey "$CERTS_DIR/device.key" \
    -out "$CERTS_DIR/device.crt"

# Remove CSR (no longer needed)
rm "$CERTS_DIR/device.csr"

echo "Certificate generation complete!"
echo "Certificate: $CERTS_DIR/device.crt"
echo "Private Key: $CERTS_DIR/device.key"
echo "Valid for: $DAYS_VALID days"

# Set appropriate permissions
chmod 600 "$CERTS_DIR/device.key"
chmod 644 "$CERTS_DIR/device.crt"

echo ""
echo "Note: This is a self-signed certificate for testing purposes only."
echo "For production use, obtain a certificate from a trusted Certificate Authority."
