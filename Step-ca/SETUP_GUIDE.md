# Step-CA Setup Scripts

This directory contains scripts to help you set up and manage step-ca.

## Available Scripts

### 1. setup-step-ca.sh (Recommended for Production)

**Purpose**: Interactive setup script for configuring step-ca with your custom settings.

**Usage**:
```bash
./setup-step-ca.sh
```

**What it does**:
- ✅ Checks for required binaries (step, step-ca)
- ✅ Initializes Certificate Authority with your settings
- ✅ Prompts for email, password, and CA details
- ✅ Creates `start-step-ca.sh` for easy startup
- ✅ Optionally creates a systemd service for automatic startup
- ✅ Provides detailed next steps

**When to use**: 
- First-time setup
- Production or long-term development environments
- When you need custom CA settings
- When you want systemd integration

---

### 2. quick-start.sh (Fast Testing)

**Purpose**: Quick one-command setup for development and testing.

**Usage**:
```bash
./quick-start.sh
```

**What it does**:
- ✅ Initializes CA with default settings
- ✅ Uses preset password: `changeme`
- ✅ Starts step-ca immediately
- ✅ No interactive prompts

**Default Settings**:
- CA Name: "Dev CA"
- DNS Names: localhost, 127.0.0.1
- Address: 127.0.0.1:9000
- Provisioner: admin@localhost
- Password: changeme

**When to use**:
- Quick testing
- Development environments
- Learning step-ca
- Temporary setups

⚠️ **Warning**: NOT for production! Uses a default password.

---

### 3. step-ca-helper.sh (Day-to-Day Operations)

**Purpose**: Simplify common step-ca operations.

**Usage**:
```bash
./step-ca-helper.sh <command> [options]
```

**Available Commands**:

| Command | Description | Example |
|---------|-------------|---------|
| `status` | Check if step-ca is running | `./step-ca-helper.sh status` |
| `fingerprint` | Show CA root cert fingerprint | `./step-ca-helper.sh fingerprint` |
| `root` | Download root CA certificate | `./step-ca-helper.sh root` |
| `cert <name>` | Request a new certificate | `./step-ca-helper.sh cert myserver.local` |
| `list` | List available provisioners | `./step-ca-helper.sh list` |
| `inspect <cert>` | Inspect a certificate file | `./step-ca-helper.sh inspect server.crt` |
| `renew <cert> <key>` | Renew a certificate | `./step-ca-helper.sh renew server.crt server.key` |
| `revoke <cert>` | Revoke a certificate | `./step-ca-helper.sh revoke server.crt` |
| `help` | Show help message | `./step-ca-helper.sh help` |

**When to use**:
- After initial setup
- Daily certificate operations
- Certificate inspection and management
- Checking CA health

---

### 4. start-step-ca.sh (Generated)

**Purpose**: Start step-ca (automatically created by setup-step-ca.sh).

**Usage**:
```bash
./start-step-ca.sh
```

**What it does**:
- Starts step-ca in foreground
- Prompts for password
- Shows CA address

**Note**: This file is auto-generated - do not create it manually.

---

## Setup Workflow

### For Production/Serious Development:

```bash
# 1. Build step-ca
make install

# 2. Run setup script
./setup-step-ca.sh
# - Answer prompts
# - Set strong password
# - Optionally create systemd service

# 3. Start step-ca
./start-step-ca.sh
# OR
sudo systemctl start step-ca

# 4. Use helper for operations
./step-ca-helper.sh status
./step-ca-helper.sh cert myserver.local
```

### For Quick Testing:

```bash
# 1. Build step-ca
make install

# 2. Quick start (one command!)
./quick-start.sh
# CA starts immediately with default settings
```

---

## Managing step-ca

### Starting step-ca

**Option 1**: Using generated script
```bash
./start-step-ca.sh
```

**Option 2**: Manual start
```bash
./step-ca ~/.step/config/ca.json
```

**Option 3**: Using systemd (if configured)
```bash
sudo systemctl start step-ca
```

### Stopping step-ca

**If running in terminal**: Press `Ctrl+C`

**If using systemd**:
```bash
sudo systemctl stop step-ca
```

### Checking status

```bash
# Using helper script
./step-ca-helper.sh status

# Using systemd
sudo systemctl status step-ca

# Manual check
curl -sk https://127.0.0.1:9000/health
```

### Viewing logs

**If using systemd**:
```bash
journalctl -u step-ca -f
```

**If configured with log files**:
```bash
tail -f ~/.step/logs/step-ca.log
```

---

## Configuration Files

After initialization, step-ca creates:

```
~/.step/
├── config/
│   ├── ca.json              # Main CA configuration
│   └── defaults.json        # Client defaults
├── certs/
│   ├── root_ca.crt          # Root certificate
│   └── intermediate_ca.crt  # Intermediate certificate
├── secrets/
│   ├── root_ca_key          # Root private key (encrypted)
│   ├── intermediate_ca_key  # Intermediate key (encrypted)
│   └── password             # Password file (if using systemd)
└── db/                      # Certificate database
```

⚠️ **Important**: Never commit or share files in `secrets/`!

---

## Common Operations

### Get a certificate for a server:
```bash
./step-ca-helper.sh cert server.example.com
# Creates: server.example.com.crt and server.example.com.key
```

### Inspect a certificate:
```bash
./step-ca-helper.sh inspect server.example.com.crt
```

### Renew before expiration:
```bash
./step-ca-helper.sh renew server.example.com.crt server.example.com.key
```

### Check CA health:
```bash
./step-ca-helper.sh status
```

---

## Troubleshooting

### "step-ca binary not found"
```bash
# Build step-ca first
make install
```

### "CA not initialized"
```bash
# Run setup script
./setup-step-ca.sh
# OR
./quick-start.sh
```

### "step-ca is not running"
```bash
# Start step-ca
./start-step-ca.sh
# OR
./quick-start.sh
```

### "Address already in use"
```bash
# Check if step-ca is already running
./step-ca-helper.sh status

# Or check the port
lsof -i :9000
```

### Reset everything:
```bash
# Stop step-ca
# Then remove configuration
rm -rf ~/.step

# Re-run setup
./setup-step-ca.sh
```

---

## Security Notes

1. **Passwords**: Use strong passwords for production
2. **Private Keys**: Never commit or share private keys
3. **Root Certificate**: Can be shared - it's public
4. **Password Files**: Only for systemd - protect with proper permissions (600)
5. **Quick Start**: Only for development - change the default password!

---

## Next Steps

After setup:

1. ✅ Build the HTTPS server: `cd Server && make`
2. ✅ Build the HTTPS client: `cd Client && make`
3. ✅ Request certificates using the helper script
4. ✅ Test mTLS communication

See the main [README.md](README.md) for complete documentation.
