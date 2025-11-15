#!/bin/bash

# Quick start script for step-ca
# Minimal setup with default values for quick testing

set -e

GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

# Check if already initialized
if [ -f "$HOME/.step/config/ca.json" ]; then
    print_warn "step-ca is already initialized"
    print_info "Starting step-ca..."
    exec ./step-ca ~/.step/config/ca.json
    exit 0
fi

# Quick initialization with defaults
print_info "Quick initialization of step-ca..."
echo
print_warn "Using default password: changeme"
print_warn "This is for DEVELOPMENT ONLY - use setup-step-ca.sh for production"
echo

CA_PASSWORD="changeme"

echo "$CA_PASSWORD" | ./step ca init \
    --name="Dev CA" \
    --dns="localhost,127.0.0.1" \
    --address="127.0.0.1:9000" \
    --provisioner="admin@localhost" \
    --password-file=<(echo "$CA_PASSWORD") \
    --provisioner-password-file=<(echo "$CA_PASSWORD")

print_info "CA initialized!"
print_info "Starting step-ca..."
echo
echo "CA Password: changeme"
echo "CA URL: https://127.0.0.1:9000"
echo
echo "$CA_PASSWORD" | ./step-ca ~/.step/config/ca.json --password-file=<(cat)
