# Step-CA Quick Reference

## 🚀 Getting Started (Choose One)

### Method 1: Full Setup (Recommended)
```bash
make install          # Build step-ca
./setup-step-ca.sh    # Interactive setup
./start-step-ca.sh    # Start CA
```

### Method 2: Quick Test
```bash
make install          # Build step-ca
./quick-start.sh      # Setup & start (password: changeme)
```

---

## 📋 Common Commands

### Check Status
```bash
./step-ca-helper.sh status
```

### Get a Certificate
```bash
./step-ca-helper.sh cert myserver.local
# Creates: myserver.local.crt and myserver.local.key
```

### Inspect Certificate
```bash
./step-ca-helper.sh inspect myserver.local.crt
```

### Renew Certificate
```bash
./step-ca-helper.sh renew myserver.local.crt myserver.local.key
```

### Get Root Certificate
```bash
./step-ca-helper.sh root ca.crt
```

### List Provisioners
```bash
./step-ca-helper.sh list
```

---

## 🎯 Default Configuration

- **CA URL**: https://127.0.0.1:9000
- **CA Config**: ~/.step/config/ca.json
- **Root Cert**: ~/.step/certs/root_ca.crt
- **Default Password** (quick-start): changeme

---

## 🔧 Management

### Start step-ca
```bash
./start-step-ca.sh                    # Using generated script
./step-ca ~/.step/config/ca.json      # Manual
sudo systemctl start step-ca          # If systemd configured
```

### Stop step-ca
```bash
Ctrl+C                                # If in terminal
sudo systemctl stop step-ca           # If systemd configured
```

### View Logs
```bash
journalctl -u step-ca -f              # If systemd configured
```

---

## 📚 Documentation

- **Full Setup Guide**: [SETUP_GUIDE.md](SETUP_GUIDE.md)
- **Complete Docs**: [README.md](README.md)
- **Helper Commands**: `./step-ca-helper.sh help`

---

## ⚠️ Security Reminders

- 🔐 Use strong passwords for production
- 🚫 Never commit private keys (.key files)
- ✅ Root certificate (.crt) is safe to share
- 🔒 Protect ~/.step/secrets/ directory
- ⚡ quick-start.sh is for testing only!

---

## 🐛 Troubleshooting

| Problem | Solution |
|---------|----------|
| Binaries not found | Run `make install` |
| CA not initialized | Run `./setup-step-ca.sh` |
| CA not running | Run `./start-step-ca.sh` |
| Port 9000 in use | Check with `lsof -i :9000` |
| Reset everything | `rm -rf ~/.step` then re-setup |

---

## 📦 What's Installed

```
Step-ca/
├── step-ca              # CA server binary
├── step                 # CLI tool binary
├── setup-step-ca.sh     # Interactive setup
├── quick-start.sh       # Fast test setup
├── step-ca-helper.sh    # Common operations
├── start-step-ca.sh     # Startup script (generated)
└── ~/.step/             # CA configuration & data
```

---

**Happy Certifying! 🎉**
