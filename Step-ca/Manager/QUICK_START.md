# Quick Start Guide - Step-CA Manager

## Installation

### 1. Install System Dependencies

Ubuntu/Debian:
```bash
sudo apt-get update
sudo apt-get install python3 python3-pip python3-gi python3-gi-cairo gir1.2-gtk-3.0
```

Fedora/RHEL:
```bash
sudo dnf install python3 python3-pip python3-gobject gtk3
```

### 2. Install Python Dependencies

```bash
cd Manager
make install-deps
```

Or manually:
```bash
pip3 install -r requirements.txt --user
```

### 3. Verify Installation

```bash
make check-deps
```

Expected output:
```
✓ GTK bindings OK
✓ cryptography OK
✓ step CLI found
✓ step-ca found
```

## Running the Application

### Basic Usage

```bash
# From the Manager directory
make run
```

Or run directly:
```bash
python3 src/main.py
```

### Development Mode

Run with GTK inspector enabled:
```bash
make dev
```

## First Time Setup

1. **Ensure Step-CA is running**:
   ```bash
   # From the main project directory
   cd ..
   ./step-ca ~/.step/config/ca.json
   ```

2. **Launch the Manager**:
   ```bash
   cd Manager
   make run
   ```

3. **Configure Settings** (if needed):
   - Click the "Settings" tab
   - Verify the paths to `step` and `step-ca` binaries
   - Verify the CA URL (default: https://localhost:9000)
   - Click "Apply Settings"

## Using the Manager

### Dashboard Tab

- View Step-CA service status
- Display CA fingerprint
- View CA configuration

### Certificates Tab

**Request a New Certificate:**
1. Enter a common name (e.g., `myserver.local`)
2. Click "Request Certificate"
3. Certificate and key will be saved in `Manager/certs/`

**View Certificate Details:**
1. Select a certificate from the list
2. Click "Inspect" or select it to view details

**Renew a Certificate:**
1. Select the certificate
2. Click "Renew"
3. Confirm the renewal

**Revoke a Certificate:**
1. Select the certificate
2. Click "Revoke"
3. Confirm the revocation (cannot be undone!)

### Provisioners Tab

- View all configured provisioners
- See provisioner types (JWK, OIDC, etc.)

### Settings Tab

- Configure binary paths
- Set CA URL
- View application information

## Troubleshooting

### GTK Not Found

```bash
sudo apt-get install python3-gi python3-gi-cairo gir1.2-gtk-3.0
```

### Step-CA Not Running

```bash
# Check if step-ca is running
ps aux | grep step-ca

# Start step-ca
cd ..
./step-ca ~/.step/config/ca.json
```

### Permission Denied

```bash
chmod +x src/main.py
python3 src/main.py
```

### Certificate Request Fails

1. Ensure step-ca is running
2. Check CA URL in Settings tab
3. Verify CA is initialized: `ls ~/.step/config/ca.json`

## Desktop Integration

Install desktop launcher:
```bash
make install
```

The application will appear in your applications menu under "System" → "Step-CA Manager"

Uninstall:
```bash
make uninstall
```

## Tips

- The application auto-refreshes CA status every 5 seconds
- Certificates are stored in `Manager/certs/`
- Use the refresh button (⟳) to manually update data
- Keep step-ca running in a separate terminal
- Short-lived certificates (24h default) require frequent renewal

## Next Steps

- Set up automatic certificate renewal
- Configure additional provisioners
- Integrate with your applications
- Set up step-ca as a systemd service

For more information, see the [README.md](README.md) or [main project documentation](../README.md).
