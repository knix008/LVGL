#!/bin/bash

# Helper script for common step-ca operations

STEP="./step"
CA_URL="https://127.0.0.1:9000"
ROOT_CERT="$HOME/.step/certs/root_ca.crt"

GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m'

show_usage() {
    cat << EOF
Step-CA Helper Script

Usage: $0 <command> [options]

Commands:
  status              - Check if step-ca is running
  fingerprint         - Show CA root certificate fingerprint
  root                - Download root CA certificate
  cert <name>         - Request a new certificate
  list                - List available provisioners
  inspect <cert>      - Inspect a certificate file
  renew <cert> <key>  - Renew a certificate
  revoke <cert>       - Revoke a certificate
  help                - Show this help message

Examples:
  $0 status
  $0 cert myserver.local
  $0 inspect server.crt
  $0 renew server.crt server.key

EOF
}

print_info() {
    echo -e "${GREEN}[INFO]${NC} $1"
}

print_section() {
    echo -e "${BLUE}=== $1 ===${NC}"
}

check_ca_running() {
    if ! curl -sk "$CA_URL/health" > /dev/null 2>&1; then
        echo "Error: step-ca is not running at $CA_URL"
        echo "Start it with: ./start-step-ca.sh"
        return 1
    fi
    return 0
}

cmd_status() {
    print_section "Step-CA Status"
    
    if check_ca_running; then
        print_info "step-ca is running at $CA_URL"
        echo
        $STEP ca health --ca-url "$CA_URL" --root "$ROOT_CERT" 2>/dev/null || echo "Health check: OK (reachable)"
    else
        echo "step-ca is NOT running"
        return 1
    fi
}

cmd_fingerprint() {
    print_section "CA Root Certificate Fingerprint"
    
    if [ ! -f "$ROOT_CERT" ]; then
        echo "Error: Root certificate not found at $ROOT_CERT"
        echo "Initialize step-ca first with: ./setup-step-ca.sh"
        return 1
    fi
    
    $STEP certificate fingerprint "$ROOT_CERT"
}

cmd_root() {
    OUTPUT_FILE="${1:-root_ca.crt}"
    
    print_section "Downloading Root CA Certificate"
    
    if ! check_ca_running; then
        return 1
    fi
    
    $STEP ca root "$OUTPUT_FILE" --ca-url "$CA_URL"
    print_info "Root certificate saved to: $OUTPUT_FILE"
}

cmd_cert() {
    if [ -z "$1" ]; then
        echo "Error: Certificate name required"
        echo "Usage: $0 cert <common-name>"
        return 1
    fi
    
    NAME="$1"
    CERT_FILE="${NAME}.crt"
    KEY_FILE="${NAME}.key"
    
    print_section "Requesting Certificate for: $NAME"
    
    if ! check_ca_running; then
        return 1
    fi
    
    echo
    echo "This will create:"
    echo "  - Certificate: $CERT_FILE"
    echo "  - Private key: $KEY_FILE"
    echo
    
    $STEP ca certificate "$NAME" "$CERT_FILE" "$KEY_FILE" --ca-url "$CA_URL" --root "$ROOT_CERT"
    
    if [ $? -eq 0 ]; then
        print_info "Certificate created successfully!"
        echo
        echo "Files created:"
        ls -lh "$CERT_FILE" "$KEY_FILE"
        echo
        echo "Inspect certificate: $0 inspect $CERT_FILE"
    fi
}

cmd_list() {
    print_section "Available Provisioners"
    
    if ! check_ca_running; then
        return 1
    fi
    
    $STEP ca provisioner list --ca-url "$CA_URL" --root "$ROOT_CERT"
}

cmd_inspect() {
    if [ -z "$1" ]; then
        echo "Error: Certificate file required"
        echo "Usage: $0 inspect <cert-file>"
        return 1
    fi
    
    if [ ! -f "$1" ]; then
        echo "Error: Certificate file not found: $1"
        return 1
    fi
    
    print_section "Certificate Information: $1"
    $STEP certificate inspect "$1" --short
}

cmd_renew() {
    if [ -z "$1" ] || [ -z "$2" ]; then
        echo "Error: Certificate and key files required"
        echo "Usage: $0 renew <cert-file> <key-file>"
        return 1
    fi
    
    CERT_FILE="$1"
    KEY_FILE="$2"
    
    if [ ! -f "$CERT_FILE" ] || [ ! -f "$KEY_FILE" ]; then
        echo "Error: Certificate or key file not found"
        return 1
    fi
    
    print_section "Renewing Certificate: $CERT_FILE"
    
    if ! check_ca_running; then
        return 1
    fi
    
    $STEP ca renew "$CERT_FILE" "$KEY_FILE" --ca-url "$CA_URL" --root "$ROOT_CERT"
    
    if [ $? -eq 0 ]; then
        print_info "Certificate renewed successfully!"
    fi
}

cmd_revoke() {
    if [ -z "$1" ]; then
        echo "Error: Certificate file required"
        echo "Usage: $0 revoke <cert-file>"
        return 1
    fi
    
    if [ ! -f "$1" ]; then
        echo "Error: Certificate file not found: $1"
        return 1
    fi
    
    print_section "Revoking Certificate: $1"
    
    read -p "Are you sure you want to revoke this certificate? (y/N): " -n 1 -r
    echo
    if [[ ! $REPLY =~ ^[Yy]$ ]]; then
        echo "Cancelled"
        return 0
    fi
    
    if ! check_ca_running; then
        return 1
    fi
    
    $STEP ca revoke --cert "$1" --ca-url "$CA_URL" --root "$ROOT_CERT"
    
    if [ $? -eq 0 ]; then
        print_info "Certificate revoked successfully!"
    fi
}

# Main command dispatch
case "${1:-help}" in
    status)
        cmd_status
        ;;
    fingerprint)
        cmd_fingerprint
        ;;
    root)
        cmd_root "$2"
        ;;
    cert)
        cmd_cert "$2"
        ;;
    list)
        cmd_list
        ;;
    inspect)
        cmd_inspect "$2"
        ;;
    renew)
        cmd_renew "$2" "$3"
        ;;
    revoke)
        cmd_revoke "$2"
        ;;
    help|--help|-h)
        show_usage
        ;;
    *)
        echo "Unknown command: $1"
        echo
        show_usage
        exit 1
        ;;
esac
