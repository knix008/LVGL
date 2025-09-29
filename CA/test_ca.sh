#!/bin/bash

# Certificate Authority Test Script

set -e

echo "Testing Certificate Authority Server..."

# Check if server is running
if ! pgrep -f "ca_server" > /dev/null; then
    echo "Error: CA server is not running"
    echo "Please start the server first: ./build/ca_server"
    exit 1
fi

echo "CA server is running, starting tests..."

# Test 1: Create a certificate
echo "Test 1: Creating a test certificate..."
curl -X POST http://localhost:8080/api/certificates \
  -H "Content-Type: application/json" \
  -d '{
    "common_name": "test.example.com",
    "email": "test@example.com",
    "organization": "Test Corp",
    "country": "KR",
    "state": "Seoul",
    "city": "Seoul",
    "key_size": 2048,
    "validity_days": 365
  }' > /tmp/cert_response.json

if [ $? -eq 0 ]; then
    echo "✓ Certificate creation test passed"
    cat /tmp/cert_response.json
else
    echo "✗ Certificate creation test failed"
    exit 1
fi

# Test 2: List certificates
echo -e "\nTest 2: Listing certificates..."
curl -s http://localhost:8080/api/certificates > /tmp/cert_list.json

if [ $? -eq 0 ]; then
    echo "✓ Certificate listing test passed"
    echo "Found certificates:"
    
    # Use jq if available, otherwise use python
    if command -v jq >/dev/null 2>&1; then
        echo "Using jq for JSON processing:"
        cat /tmp/cert_list.json | jq '.'
        echo ""
        echo "Certificate count: $(cat /tmp/cert_list.json | jq '.certificates | length')"
    else
        echo "Using python for JSON processing (jq not available):"
        cat /tmp/cert_list.json | python3 -m json.tool
        echo ""
        echo "Certificate count: $(cat /tmp/cert_list.json | grep -o '\"serial_number\"' | wc -l)"
    fi
else
    echo "✗ Certificate listing test failed"
    exit 1
fi

# Test 3: Download CA certificate
echo -e "\nTest 3: Downloading CA certificate..."
curl -s http://localhost:8080/api/ca > /tmp/ca.crt

if [ $? -eq 0 ] && [ -s /tmp/ca.crt ]; then
    echo "✓ CA certificate download test passed"
    echo "CA certificate saved to /tmp/ca.crt"
else
    echo "✗ CA certificate download test failed"
    exit 1
fi

# Test 4: Download CRL
echo -e "\nTest 4: Downloading CRL..."
curl -s http://localhost:8080/api/crl > /tmp/ca.crl

if [ $? -eq 0 ] && [ -s /tmp/ca.crl ]; then
    echo "✓ CRL download test passed"
    echo "CRL saved to /tmp/ca.crl"
else
    echo "✗ CRL download test failed"
    exit 1
fi

# Test 5: Test web interface
echo -e "\nTest 5: Testing web interface..."
if curl -s http://localhost:8080/ | grep -q "Certificate Authority"; then
    echo "✓ Web interface test passed"
else
    echo "✗ Web interface test failed"
    exit 1
fi

echo -e "\nAll tests passed! ✓"
echo "Test files created:"
echo "  - /tmp/cert_response.json (certificate creation response)"
echo "  - /tmp/cert_list.json (certificate list)"
echo "  - /tmp/ca.crt (CA certificate)"
echo "  - /tmp/ca.crl (Certificate Revocation List)"

echo -e "\nFile structure:"
echo "  - certs/ directory: CA files (ca.crt, ca.key, ca.db) and client certificates/keys"
echo "  - config/ directory: Configuration files"

echo -e "\nYou can now:"
echo "1. Open http://localhost:8080 in your browser to use the web interface"
echo "2. Use the Python client example: python3 examples/client_example.py list"
echo "3. Check the generated files in the certs/ directory"
echo "4. View CA files in the certs/ directory"
