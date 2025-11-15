#!/bin/bash

# Setup script for step-ca
# This script initializes and configures step-ca for local development

set -e  # Exit on error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Configuration
STEP_BIN="./step"
STEP_CA_BIN="./step-ca"
CA_NAME="Local Development CA"
DNS_NAMES="localhost,127.0.0.1"
CA_ADDRESS="127.0.0.1:9000"
STEP_HOME="$HOME/.step"
CA_CONFIG="$STEP_HOME/config/ca.json"

# Functions
print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

check_binaries() {
    print_info "Checking for step-ca binaries..."
    
    if [ ! -f "$STEP_BIN" ]; then
        print_error "step binary not found. Please run 'make install' first."
        exit 1
    fi
    
    if [ ! -f "$STEP_CA_BIN" ]; then
        print_error "step-ca binary not found. Please run 'make install' first."
        exit 1
    fi
    
    print_info "Binaries found!"
}

initialize_ca() {
    if [ -d "$STEP_HOME" ] && [ -f "$CA_CONFIG" ]; then
        print_warn "step-ca appears to be already initialized at $STEP_HOME"
        read -p "Do you want to reinitialize? This will DELETE existing CA (y/N): " -n 1 -r
        echo
        if [[ ! $REPLY =~ ^[Yy]$ ]]; then
            print_info "Keeping existing CA configuration"
            return 0
        fi
        print_warn "Removing existing CA configuration..."
        rm -rf "$STEP_HOME"
    fi
    
    print_info "Initializing step-ca..."
    echo
    echo "================================================"
    echo "Please provide the following information:"
    echo "================================================"
    
    # Read CA admin email
    read -p "Enter your email address: " CA_EMAIL
    
    # Read CA password
    echo
    echo "Set a password for the CA (you'll need this to start step-ca):"
    read -s -p "Password: " CA_PASSWORD
    echo
    read -s -p "Confirm password: " CA_PASSWORD_CONFIRM
    echo
    
    if [ "$CA_PASSWORD" != "$CA_PASSWORD_CONFIRM" ]; then
        print_error "Passwords don't match!"
        exit 1
    fi
    
    # Initialize CA non-interactively
    print_info "Creating Certificate Authority..."
    
    echo "$CA_PASSWORD" | $STEP_BIN ca init \
        --name="$CA_NAME" \
        --dns="$DNS_NAMES" \
        --address="$CA_ADDRESS" \
        --provisioner="$CA_EMAIL" \
        --password-file=<(echo "$CA_PASSWORD") \
        --provisioner-password-file=<(echo "$CA_PASSWORD")
    
    if [ $? -eq 0 ]; then
        print_info "Certificate Authority initialized successfully!"
        echo
        print_info "CA Details:"
        echo "  - Name: $CA_NAME"
        echo "  - Address: $CA_ADDRESS"
        echo "  - DNS: $DNS_NAMES"
        echo "  - Config: $CA_CONFIG"
        echo "  - Root CA cert: $STEP_HOME/certs/root_ca.crt"
        echo
    else
        print_error "Failed to initialize CA"
        exit 1
    fi
}

create_systemd_service() {
    print_info "Would you like to create a systemd service for step-ca?"
    read -p "This requires sudo access (y/N): " -n 1 -r
    echo
    
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        print_info "Skipping systemd service creation"
        return 0
    fi
    
    # Get password file path
    read -s -p "Enter CA password: " CA_PASSWORD
    echo
    
    STEP_CA_PASSWORD_FILE="$STEP_HOME/secrets/password"
    mkdir -p "$STEP_HOME/secrets"
    chmod 700 "$STEP_HOME/secrets"
    echo "$CA_PASSWORD" > "$STEP_CA_PASSWORD_FILE"
    chmod 600 "$STEP_CA_PASSWORD_FILE"
    
    SERVICE_FILE="/tmp/step-ca.service"
    
    cat > "$SERVICE_FILE" << EOF
[Unit]
Description=Step-CA Certificate Authority
After=network.target
Documentation=https://smallstep.com/docs/step-ca

[Service]
Type=simple
User=$USER
Group=$USER
Environment=STEPPATH=$STEP_HOME
WorkingDirectory=$HOME
ExecStart=$(pwd)/$STEP_CA_BIN $CA_CONFIG --password-file=$STEP_CA_PASSWORD_FILE
Restart=on-failure
RestartSec=10
StandardOutput=append:$STEP_HOME/logs/step-ca.log
StandardError=append:$STEP_HOME/logs/step-ca-error.log

[Install]
WantedBy=multi-user.target
EOF
    
    mkdir -p "$STEP_HOME/logs"
    
    print_info "Installing systemd service..."
    sudo cp "$SERVICE_FILE" /etc/systemd/system/step-ca.service
    sudo systemctl daemon-reload
    
    print_info "Systemd service created!"
    echo
    echo "To manage the service:"
    echo "  sudo systemctl start step-ca      # Start the service"
    echo "  sudo systemctl stop step-ca       # Stop the service"
    echo "  sudo systemctl enable step-ca     # Enable at boot"
    echo "  sudo systemctl status step-ca     # Check status"
    echo "  journalctl -u step-ca -f          # View logs"
}

create_startup_script() {
    print_info "Creating startup script..."
    
    STARTUP_SCRIPT="./start-step-ca.sh"
    
    cat > "$STARTUP_SCRIPT" << 'EOF'
#!/bin/bash

# Start step-ca in foreground
# The CA password will be prompted

STEP_CA_BIN="./step-ca"
CA_CONFIG="$HOME/.step/config/ca.json"

if [ ! -f "$STEP_CA_BIN" ]; then
    echo "Error: step-ca binary not found"
    exit 1
fi

if [ ! -f "$CA_CONFIG" ]; then
    echo "Error: CA not initialized. Run ./setup-step-ca.sh first"
    exit 1
fi

echo "Starting step-ca..."
echo "CA Address: https://127.0.0.1:9000"
echo "Press Ctrl+C to stop"
echo

exec "$STEP_CA_BIN" "$CA_CONFIG"
EOF
    
    chmod +x "$STARTUP_SCRIPT"
    print_info "Startup script created: $STARTUP_SCRIPT"
}

display_next_steps() {
    echo
    echo "================================================"
    echo -e "${GREEN}Setup Complete!${NC}"
    echo "================================================"
    echo
    echo "Next steps:"
    echo
    echo "1. Start step-ca:"
    echo "   ./start-step-ca.sh"
    echo
    echo "   OR manually:"
    echo "   ./step-ca ~/.step/config/ca.json"
    echo
    echo "2. In another terminal, get the root certificate:"
    echo "   ./step ca root root_ca.crt"
    echo
    echo "3. Bootstrap a client:"
    echo "   ./step ca bootstrap --ca-url https://127.0.0.1:9000 --fingerprint \$(./step certificate fingerprint ~/.step/certs/root_ca.crt)"
    echo
    echo "4. Get a certificate:"
    echo "   ./step ca certificate myserver.local server.crt server.key"
    echo
    echo "5. Build and run the HTTPS server:"
    echo "   cd Server && make"
    echo
    echo "For more information, check README.md"
    echo
}

# Main execution
main() {
    echo "================================================"
    echo "Step-CA Setup Script"
    echo "================================================"
    echo
    
    check_binaries
    initialize_ca
    create_startup_script
    create_systemd_service
    display_next_steps
}

main
