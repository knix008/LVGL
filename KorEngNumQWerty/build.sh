#!/bin/bash

# ==============================================================================
# KorEngNumQWerty Build Script with Local LVGL
# ==============================================================================

set -e  # Exit on error

# Color output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

print_info() {
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

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
SOURCE_DIR="${SCRIPT_DIR}/Source"
BUILD_DIR="${SOURCE_DIR}/build"
LVGL_DIR="${SOURCE_DIR}/lvgl"

print_info "KorEngNumQWerty LVGL Application Build Script"
print_info "=============================================="

# Check if LVGL directory exists
if [ ! -d "${LVGL_DIR}" ]; then
    print_error "LVGL directory not found at ${LVGL_DIR}"
    exit 1
fi

# Check dependencies
print_info "Checking dependencies..."

# Check for required packages (simplified check)
REQUIRED_PACKAGES=(
    "cmake"
    "build-essential" 
    "libsdl2-dev"
    "libfreetype-dev"
    "pkg-config"
    "git"
)

MISSING_PACKAGES=()

for package in "${REQUIRED_PACKAGES[@]}"; do
    if ! dpkg -l "$package" 2>/dev/null | grep -q "^ii"; then
        MISSING_PACKAGES+=("$package")
    fi
done

if [ ${#MISSING_PACKAGES[@]} -ne 0 ]; then
    print_warning "Missing packages: ${MISSING_PACKAGES[*]}"
    print_info "Please install them with: sudo apt-get install ${MISSING_PACKAGES[*]}"
    exit 1
fi

print_success "All dependencies are satisfied"

# Create build directory
print_info "Setting up build directory..."
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}"

# Configure with CMake
print_info "Configuring project with CMake..."
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build the project
print_info "Building project..."
make -j$(nproc)

print_success "Build completed successfully!"
print_info "Executable location: ${BUILD_DIR}/KorEngNumQWerty"
print_info "To run the application: ./run.sh"