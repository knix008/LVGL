#!/bin/bash

# Test script for TLS functionality
# This script tests both HTTP and HTTPS endpoints

set -e

echo "Testing WebServer TLS functionality..."
echo "======================================"

# Start the server in the background
echo "Starting WebServer..."
cd build
./main &
SERVER_PID=$!

# Wait for server to start
sleep 3

echo ""
echo "Testing HTTP endpoint (port 8080)..."
if curl -s http://localhost:8080 > /dev/null; then
    echo "✓ HTTP server is running"
else
    echo "✗ HTTP server failed"
fi

echo ""
echo "Testing HTTPS endpoint (port 8443)..."
if curl -k -s https://localhost:8443 > /dev/null; then
    echo "✓ HTTPS server is running with TLS"
else
    echo "✗ HTTPS server failed"
fi

echo ""
echo "Testing TLS certificate..."
if echo | openssl s_client -connect localhost:8443 -servername localhost 2>/dev/null | grep -q "subject="; then
    echo "✓ TLS certificate is valid"
else
    echo "✗ TLS certificate validation failed"
fi

echo ""
echo "Testing WebSocket over HTTPS..."
if curl -k -i -N -H "Connection: Upgrade" -H "Upgrade: websocket" -H "Sec-WebSocket-Version: 13" -H "Sec-WebSocket-Key: x3JJHMbDL1EzLkh9GBhXDw==" https://localhost:8443 2>/dev/null | grep -q "101"; then
    echo "✓ WebSocket over HTTPS is working"
else
    echo "✗ WebSocket over HTTPS failed"
fi

echo ""
echo "Server information:"
echo "HTTP:  http://localhost:8080"
echo "HTTPS: https://localhost:8443"
echo ""
echo "Note: For HTTPS, you may need to accept the self-signed certificate in your browser"
echo "      or use curl with -k flag to skip certificate verification"

# Stop the server
echo ""
echo "Stopping WebServer..."
kill $SERVER_PID 2>/dev/null || true
wait $SERVER_PID 2>/dev/null || true

echo "Test completed!"
