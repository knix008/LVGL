#!/bin/bash

# Certificate Authority Server - Run Script
# This script starts the CA server with proper setup and error handling

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_status() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Function to check if server is already running
check_server_running() {
    if pgrep -f "ca_server" > /dev/null; then
        print_warning "CA server is already running!"
        echo "Running processes:"
        pgrep -f "ca_server" | xargs ps -p
        echo ""
        echo "To stop the existing server, run:"
        echo "  pkill -f ca_server"
        echo ""
        read -p "Do you want to stop the existing server and start a new one? (y/N): " -n 1 -r
        echo
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            print_status "Stopping existing CA server..."
            pkill -f ca_server
            sleep 2
        else
            print_status "Exiting without starting new server."
            exit 0
        fi
    fi
}

# Function to check if build exists
check_build() {
    if [ ! -f "./build/ca_server" ]; then
        print_error "CA server executable not found!"
        echo "Please build the project first:"
        echo "  ./build.sh"
        exit 1
    fi
}

# Function to check if config exists
check_config() {
    if [ ! -f "./config/ca.conf" ]; then
        print_error "Configuration file not found!"
        echo "Please ensure config/ca.conf exists."
        exit 1
    fi
}

# Function to create directories if they don't exist
create_directories() {
    print_status "Creating necessary directories..."
    mkdir -p certs keys
    print_success "Directories created successfully"
}

# Function to start the server
start_server() {
    print_status "Starting Certificate Authority Server..."
    echo ""
    
    # Start server in background
    ./build/ca_server &
    SERVER_PID=$!
    
    # Wait a moment for server to start
    sleep 2
    
    # Check if server is running
    if kill -0 $SERVER_PID 2>/dev/null; then
        print_success "CA server started successfully!"
        echo ""
        echo "Server Information:"
        echo "  PID: $SERVER_PID"
        echo "  Web Interface: http://localhost:8080"
        echo "  API Base URL: http://localhost:8080/api"
        echo ""
        echo "Available Endpoints:"
        echo "  GET  /api/certificates     - List all certificates"
        echo "  GET  /api/certificates/{id} - Get certificate details"
        echo "  POST /api/certificates     - Create new certificate"
        echo "  DELETE /api/certificates/{id} - Revoke certificate"
        echo "  GET  /api/crl              - Get Certificate Revocation List"
        echo "  GET  /api/ca               - Get CA certificate"
        echo ""
        echo "To stop the server:"
        echo "  pkill -f ca_server"
        echo "  or"
        echo "  kill $SERVER_PID"
        echo ""
        echo "Press Ctrl+C to stop this script (server will continue running)"
        
        # Wait for user interrupt
        trap 'print_status "Script stopped. Server continues running in background."; exit 0' INT
        while true; do
            sleep 1
        done
    else
        print_error "Failed to start CA server!"
        exit 1
    fi
}

# Function to show help
show_help() {
    echo "Certificate Authority Server - Run Script"
    echo ""
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  -h, --help     Show this help message"
    echo "  -f, --foreground  Run server in foreground (not background)"
    echo "  -c, --check    Check if server is running and exit"
    echo "  -s, --stop     Stop running server and exit"
    echo ""
    echo "Examples:"
    echo "  $0                    # Start server in background"
    echo "  $0 --foreground      # Start server in foreground"
    echo "  $0 --check           # Check server status"
    echo "  $0 --stop            # Stop running server"
}

# Function to stop server
stop_server() {
    if pgrep -f "ca_server" > /dev/null; then
        print_status "Stopping CA server..."
        pkill -f ca_server
        sleep 2
        if pgrep -f "ca_server" > /dev/null; then
            print_warning "Server still running, force stopping..."
            pkill -9 -f ca_server
        fi
        print_success "CA server stopped successfully!"
    else
        print_warning "No CA server is currently running."
    fi
}

# Function to check server status
check_status() {
    if pgrep -f "ca_server" > /dev/null; then
        print_success "CA server is running!"
        echo "Running processes:"
        pgrep -f "ca_server" | xargs ps -p
        echo ""
        echo "Web Interface: http://localhost:8080"
        echo "API Base URL: http://localhost:8080/api"
    else
        print_warning "CA server is not running."
    fi
}

# Parse command line arguments
FOREGROUND=false

while [[ $# -gt 0 ]]; do
    case $1 in
        -h|--help)
            show_help
            exit 0
            ;;
        -f|--foreground)
            FOREGROUND=true
            shift
            ;;
        -c|--check)
            check_status
            exit 0
            ;;
        -s|--stop)
            stop_server
            exit 0
            ;;
        *)
            print_error "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
done

# Main execution
echo "Certificate Authority Server - Run Script"
echo "========================================"
echo ""

# Check if server is already running
check_server_running

# Check if build exists
check_build

# Check if config exists
check_config

# Create directories
create_directories

# Start server
if [ "$FOREGROUND" = true ]; then
    print_status "Starting CA server in foreground..."
    ./build/ca_server
else
    start_server
fi
