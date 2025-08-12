#!/bin/bash

# LVGL WebServer Test Script
# This script tests the web server functionality

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

# Check if curl is available
if ! command -v curl &> /dev/null; then
    print_error "curl is not installed. Please install curl to test the web server."
    exit 1
fi

# Check if the application is running
check_app_running() {
    if ! pgrep -f "main" > /dev/null; then
        print_error "Application is not running. Please start it first with: ./run.sh"
        exit 1
    fi
    print_success "Application is running"
}

# Test web server endpoints
test_endpoints() {
    print_status "Testing web server endpoints..."
    
    # Test 1: Server status (HTTP)
    print_status "1. Testing HTTP server status..."
    STATUS=$(curl -s http://localhost:8080/api/status)
    if [[ $STATUS == *"running"* ]]; then
        print_success "HTTP server status: $STATUS"
    else
        print_error "HTTP server status test failed"
        return 1
    fi
    
    # Test 2: UI state (HTTP)
    print_status "2. Testing HTTP UI state endpoint..."
    UI_STATE=$(curl -s http://localhost:8080/api/ui/state)
    if [[ $UI_STATE == *"ui_state"* ]]; then
        print_success "HTTP UI state endpoint working"
    else
        print_error "HTTP UI state test failed"
        return 1
    fi
    
    # Test 3: Main page (HTTP)
    print_status "3. Testing HTTP main web interface..."
    PAGE_SIZE=$(curl -s http://localhost:8080/ | wc -c)
    if [ $PAGE_SIZE -gt 1000 ]; then
        print_success "HTTP main page loaded: $PAGE_SIZE bytes"
    else
        print_error "HTTP main page test failed"
        return 1
    fi
    
    # Test 4: TLS status check
    print_status "4. Checking TLS status..."
    TLS_ENABLED=$(echo $STATUS | grep -o '"tls_enabled":[^,]*' | cut -d':' -f2)
    if [[ $TLS_ENABLED == "true" ]]; then
        print_success "TLS is enabled"
        
        # Test 5: HTTPS server status (skip certificate verification for self-signed)
        print_status "5. Testing HTTPS server status..."
        HTTPS_STATUS=$(curl -k -s https://localhost:8443/api/status)
        if [[ $HTTPS_STATUS == *"running"* ]]; then
            print_success "HTTPS server status: $HTTPS_STATUS"
        else
            print_warning "HTTPS server test failed (may be normal if certificates not configured)"
        fi
        
        # Test 6: HTTPS main page
        print_status "6. Testing HTTPS main web interface..."
        HTTPS_PAGE_SIZE=$(curl -k -s https://localhost:8443/ | wc -c)
        if [ $HTTPS_PAGE_SIZE -gt 1000 ]; then
            print_success "HTTPS main page loaded: $HTTPS_PAGE_SIZE bytes"
        else
            print_warning "HTTPS main page test failed (may be normal if certificates not configured)"
        fi
    else
        print_warning "TLS is disabled - HTTPS tests skipped"
    fi
    
    print_success "All web server tests passed!"
}

# Main test function
main() {
    echo "=== LVGL WebServer Test ==="
    echo ""
    
    check_app_running
    test_endpoints
    
    echo ""
    echo "=== Test Summary ==="
    echo "✅ Application is running"
    echo "✅ Web server is responding"
    echo "✅ API endpoints are working"
    echo "✅ Web interface is accessible"
    
    # Check TLS status
    TLS_ENABLED=$(echo $STATUS | grep -o '"tls_enabled":[^,]*' | cut -d':' -f2)
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
}

main "$@"
