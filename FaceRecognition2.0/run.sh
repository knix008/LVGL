#!/bin/bash

# LVGL Face Recognition Application Runner
# This script runs the already built LVGL application

set -e  # Exit on any error

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

# Function to check if application is built
check_application() {
    if [ ! -f "Source/build/main" ]; then
        print_error "Application not found. Please run './build.sh' first to build the application."
        exit 1
    fi
    print_success "Application found"
}

# Function to run the application
run_application() {
    print_status "Starting LVGL application with web server..."
    
    cd Source/build
    
    print_success "Application starting..."
    print_status "Web interface will be available at: http://localhost:8080"
    print_status "HTTPS interface will be available at: https://localhost:8443"
    print_status "Press Ctrl+C to stop the application"
    echo ""
    
    # Run the application
    ./main
}

# Function to show help
show_help() {
    echo "LVGL Face Recognition Application Runner"
    echo ""
    echo "Usage: $0 [OPTION]"
    echo ""
    echo "Options:"
    echo "  run, -r, --run      Run the application (default)"
    echo "  help, -h, --help    Show this help message"
    echo ""
    echo "Examples:"
    echo "  $0                 # Run the application"
    echo "  $0 run            # Run the application"
    echo ""
    echo "Note: Make sure to run './build.sh' first to build the application"
    echo ""
    echo "Features:"
    echo "  - Database encryption with SQLCipher"
    echo "  - Web interface with HTTPS support"
    echo "  - Korean input methods"
    echo "  - Video playback with FFmpeg"
    echo ""
}

# Main script logic
main() {
    case "${1:-run}" in
        "run"|"-r"|"--run")
            check_application
            run_application
            ;;
        "help"|"-h"|"--help")
            show_help
            ;;
        *)
            print_error "Unknown option: $1"
            show_help
            exit 1
            ;;
    esac
}

# Run main function with all arguments
main "$@"
