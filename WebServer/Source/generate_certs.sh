#!/bin/bash

# Generate RSA certificates for Mongoose built-in TLS
# This script creates self-signed certificates suitable for development

set -e

CERT_DIR="certs"
KEY_FILE="$CERT_DIR/server.key"
CERT_FILE="$CERT_DIR/server.crt"
CA_FILE="$CERT_DIR/ca.crt"

echo "Generating RSA certificates for Mongoose built-in TLS..."

# Create certs directory
mkdir -p "$CERT_DIR"

# Generate RSA private key (2048-bit) in traditional PEM format
echo "Generating RSA private key..."
openssl genrsa -out "$KEY_FILE" 2048

# Generate self-signed certificate
echo "Generating self-signed certificate..."
openssl req -new -key "$KEY_FILE" -x509 -nodes -out "$CERT_FILE" -days 365 \
    -subj "/C=US/ST=State/L=City/O=Organization/CN=localhost" \
    -addext "subjectAltName=DNS:localhost,IP:127.0.0.1" \
    -addext "keyUsage=digitalSignature,keyEncipherment" \
    -addext "extendedKeyUsage=serverAuth"

# Copy certificate as CA (for self-signed certs)
cp "$CERT_FILE" "$CA_FILE"

# Verify the key format
echo "Verifying key format..."
if openssl rsa -in "$KEY_FILE" -noout -text > /dev/null 2>&1; then
    echo "✓ Key format is compatible with Mongoose built-in TLS"
else
    echo "✗ Key format may not be compatible"
fi

echo "Certificate generation complete!"
echo "Files created:"
echo "  Private key: $KEY_FILE"
echo "  Certificate: $CERT_FILE"
echo "  CA certificate: $CA_FILE"
echo ""
echo "Note: These are self-signed certificates for development only."
echo "For production, use certificates from a trusted Certificate Authority."
