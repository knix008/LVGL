#!/bin/bash

echo "=== LVGL Web Server Test ==="
echo "Testing web server functionality..."

# Test 1: Check if server is running
echo "1. Testing server status..."
STATUS=$(curl -s http://localhost:8080/api/status)
echo "Status: $STATUS"

# Test 2: Get UI state
echo -e "\n2. Testing UI state endpoint..."
UI_STATE=$(curl -s http://localhost:8080/api/ui/state)
echo "UI State: $UI_STATE"

# Test 3: Test main page
echo -e "\n3. Testing main web interface..."
PAGE_SIZE=$(curl -s http://localhost:8080/ | wc -c)
echo "Main page size: $PAGE_SIZE bytes"

# Test 4: Test WebSocket connection (basic)
echo -e "\n4. Testing WebSocket endpoint..."
WS_RESPONSE=$(curl -s -H "Connection: Upgrade" -H "Upgrade: websocket" -H "Sec-WebSocket-Key: test" -H "Sec-WebSocket-Version: 13" http://localhost:8080/ws)
echo "WebSocket response: $WS_RESPONSE"

echo -e "\n=== Test Complete ==="
echo "Web interface available at: http://localhost:8080"
echo "API endpoints:"
echo "  - GET /api/status - Server status"
echo "  - GET /api/ui/state - UI state"
echo "  - GET / - Web interface"
echo "  - WebSocket /ws - Real-time communication"
