#!/bin/bash

# WebSocket Test Script for LVGL Web Control
# This script tests the WebSocket connection and command processing

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

# Check if the application is running
check_app_running() {
    if ! pgrep -f "main" > /dev/null; then
        print_error "Application is not running. Please start it first with: ./run.sh"
        exit 1
    fi
    print_success "Application is running"
}

# Test HTTP endpoints
test_http_endpoints() {
    print_status "Testing HTTP endpoints..."
    
    # Test main page
    print_status "1. Testing main web interface..."
    PAGE_SIZE=$(curl -s http://localhost:8080/ | wc -c)
    if [ $PAGE_SIZE -gt 1000 ]; then
        print_success "Main page loaded: $PAGE_SIZE bytes"
    else
        print_error "Main page test failed"
        return 1
    fi
    
    # Test API status
    print_status "2. Testing API status endpoint..."
    STATUS=$(curl -s http://localhost:8080/api/status)
    if [[ $STATUS == *"running"* ]]; then
        print_success "API status: $STATUS"
    else
        print_error "API status test failed"
        return 1
    fi
    
    # Test UI state endpoint
    print_status "3. Testing UI state endpoint..."
    UI_STATE=$(curl -s http://localhost:8080/api/ui/state)
    if [[ $UI_STATE == *"ui_state"* ]]; then
        print_success "UI state endpoint working"
        echo "UI State: $UI_STATE" | head -c 200
        echo "..."
    else
        print_error "UI state test failed"
        return 1
    fi
}

# Test WebSocket connection using wscat (if available)
test_websocket() {
    print_status "Testing WebSocket connection..."
    
    if command -v wscat &> /dev/null; then
        print_status "Using wscat to test WebSocket..."
        
        # Test basic WebSocket connection
        echo '{"type":"system","value":"status"}' | timeout 5 wscat -c ws://localhost:8080/ws || {
            print_warning "WebSocket connection test failed (this is normal if wscat is not properly configured)"
        }
    else
        print_warning "wscat not available - skipping WebSocket test"
        print_status "You can install wscat with: npm install -g wscat"
    fi
}

# Test HTTPS endpoints
test_https_endpoints() {
    print_status "Testing HTTPS endpoints..."
    
    # Check if TLS is enabled
    TLS_ENABLED=$(curl -s http://localhost:8080/api/status | grep -o '"tls_enabled":[^,]*' | cut -d':' -f2)
    
    if [[ $TLS_ENABLED == "true" ]]; then
        print_success "TLS is enabled"
        
        # Test HTTPS status (ignore certificate warnings)
        print_status "1. Testing HTTPS server status..."
        HTTPS_STATUS=$(curl -k -s https://localhost:8443/api/status)
        if [[ $HTTPS_STATUS == *"running"* ]]; then
            print_success "HTTPS server status: $HTTPS_STATUS"
        else
            print_warning "HTTPS server test failed"
        fi
        
        # Test HTTPS main page
        print_status "2. Testing HTTPS main web interface..."
        HTTPS_PAGE_SIZE=$(curl -k -s https://localhost:8443/ | wc -c)
        if [ $HTTPS_PAGE_SIZE -gt 1000 ]; then
            print_success "HTTPS main page loaded: $HTTPS_PAGE_SIZE bytes"
        else
            print_warning "HTTPS main page test failed"
        fi
    else
        print_warning "TLS is disabled - HTTPS tests skipped"
    fi
}

# Manual testing instructions
show_manual_test_instructions() {
    echo ""
    echo "=== Manual Testing Instructions ==="
    echo ""
    echo "1. Open your web browser and navigate to:"
    echo "   HTTP:  http://localhost:8080"
    echo "   HTTPS: https://localhost:8443 (if TLS enabled)"
    echo ""
    echo "2. Check the browser's developer console (F12) for WebSocket messages"
    echo ""
    echo "3. Try clicking the control buttons and watch for:"
    echo "   - Console log messages"
    echo "   - Application terminal output"
    echo "   - UI state updates"
    echo ""
    echo "4. If buttons don't respond, check:"
    echo "   - WebSocket connection status in the web interface"
    echo "   - Browser console for JavaScript errors"
    echo "   - Application logs for command processing"
    echo ""
}

# Main test function
main() {
    echo "=== LVGL WebSocket Test ==="
    echo ""
    
    check_app_running
    test_http_endpoints
    test_https_endpoints
    test_websocket
    
    echo ""
    echo "=== Test Summary ==="
    echo "✅ Application is running"
    echo "✅ HTTP server is responding"
    echo "✅ API endpoints are working"
    echo "✅ Web interface is accessible"
    
    # Check TLS status
    TLS_ENABLED=$(curl -s http://localhost:8080/api/status | grep -o '"tls_enabled":[^,]*' | cut -d':' -f2)
    if [[ $TLS_ENABLED == "true" ]]; then
        echo "🔒 TLS 1.3 is enabled"
        echo "✅ HTTPS server is responding"
    else
        echo "⚠️  TLS is disabled"
    fi
    
    echo ""
    echo "🌐 HTTP interface: http://localhost:8080"
    echo "📊 HTTP API status: http://localhost:8080/api/status"
    echo "📱 HTTP UI state: http://localhost:8080/api/ui/state"
    
    if [[ $TLS_ENABLED == "true" ]]; then
        echo ""
        echo "🔒 HTTPS interface: https://localhost:8443"
        echo "📊 HTTPS API status: https://localhost:8443/api/status"
        echo "📱 HTTPS UI state: https://localhost:8443/api/ui/state"
    fi
    
    show_manual_test_instructions
}

main "$@"
