# Step-CA Manager Demo Guide

## Complete Workflow Demo

This guide demonstrates a complete workflow using the Step-CA Manager GUI.

### Prerequisites

1. Step-CA is installed and initialized
2. Step-CA is running
3. GUI Manager dependencies are installed

### Demo Scenario: Setting up TLS for a web server

#### Step 1: Start Step-CA

Terminal 1:
```bash
cd /home/shkwon/Projects/LVGL/Step-ca
./step-ca ~/.step/config/ca.json
```

Expected output:
```
Serving HTTPS on :9000 ...
```

#### Step 2: Launch the GUI Manager

Terminal 2:
```bash
cd /home/shkwon/Projects/LVGL/Step-ca/Manager
make run
```

The GUI application window will open.

#### Step 3: Verify CA Status (Dashboard Tab)

The Dashboard tab shows:
- ✓ Step CLI: Installed (Smallstep CLI/0.28.4)
- ✓ Step-CA: Installed (Smallstep CA/0.28.4)
- ✓ CA Service: Running
- CA URL: https://localhost:9000
- Fingerprint: [your CA fingerprint]

#### Step 4: Request a Certificate (Certificates Tab)

1. Click the "Certificates" tab
2. In the "Request New Certificate" section:
   - Enter Common Name: `myapp.local`
   - Click "Request Certificate"
3. A dialog appears: "Certificate requested successfully for myapp.local"
4. The certificate list updates showing:
   - Name: myapp
   - Status: ✓ Valid (1 days)

#### Step 5: Inspect the Certificate

1. Select the certificate from the list
2. Click "Inspect" button (or just select it)
3. The right panel shows certificate details in JSON format:
   ```json
   {
     "subject": {
       "common_name": "myapp.local"
     },
     "issuer": {
       "common_name": "My Local CA"
     },
     "validity": {
       "not_before": "2025-11-16T14:30:00Z",
       "not_after": "2025-11-17T14:30:00Z"
     },
     ...
   }
   ```

#### Step 6: View Provisioners (Provisioners Tab)

1. Click the "Provisioners" tab
2. View the list of provisioners:
   - Name: admin@example.com
   - Type: JWK

#### Step 7: Configure Settings (Settings Tab)

1. Click the "Settings" tab
2. Verify paths:
   - Step CLI: /home/shkwon/Projects/LVGL/Step-ca/bin/step
   - Step-CA: /home/shkwon/Projects/LVGL/Step-ca/bin/step-ca
3. Verify CA URL: https://localhost:9000
4. If changes made, click "Apply Settings"

#### Step 8: Use the Certificate

The certificate files are now available:
```bash
ls -l Manager/certs/
```

Output:
```
myapp.local.crt  # Certificate
myapp.local.key  # Private key
```

Use these files in your application:
```bash
# Example: Start an HTTPS server
cd ../Server
cp ../Manager/certs/myapp.local.crt certs/server.crt
cp ../Manager/certs/myapp.local.key certs/server.key
make run
```

#### Step 9: Renew a Certificate (Next Day)

1. Go to Certificates tab
2. Select the certificate
3. Click "Renew"
4. Confirm renewal
5. Certificate is renewed with a new expiration date

#### Step 10: Revoke a Certificate (When Needed)

1. Go to Certificates tab
2. Select the certificate to revoke
3. Click "Revoke"
4. Confirm revocation (⚠️ Cannot be undone!)
5. Certificate is revoked and marked as invalid

## Advanced Usage

### Managing Multiple Certificates

Request certificates for different services:
- `database.local` - Database server
- `api.local` - API server
- `web.local` - Web server
- `*.example.com` - Wildcard certificate

### Monitoring Certificate Expiration

The GUI automatically shows:
- ✓ Valid (N days) - Certificate is valid
- ✗ Expired - Certificate has expired

Click refresh (⟳) to update the status.

### Desktop Integration

After running `make install`, you can:
1. Open applications menu
2. Navigate to System → Step-CA Manager
3. Click to launch without terminal

## Troubleshooting Demo

### CA Not Running

If the Dashboard shows "✗ CA Service: Stopped":
1. Open a terminal
2. Run: `./step-ca ~/.step/config/ca.json`
3. Click refresh (⟳) in the GUI
4. Status updates to "✓ Running"

### Certificate Request Fails

If certificate request fails:
1. Check Dashboard - ensure CA is running
2. Check Settings - verify CA URL
3. Check terminal running step-ca for errors

## Tips for Demo

1. Keep step-ca terminal visible to see real-time activity
2. Use the refresh button after making changes
3. The GUI auto-refreshes CA status every 5 seconds
4. Certificate files are in `Manager/certs/`
5. Use `Ctrl+C` in the terminal to stop the GUI

## Screenshots Workflow

For documentation, capture these screens:
1. Dashboard showing all green status
2. Certificate request form
3. Certificate list with valid certificates
4. Certificate details viewer (JSON)
5. Provisioners list
6. Settings panel

## Next Steps

- Integrate certificates with your applications
- Set up automatic renewal with systemd timers
- Configure additional provisioners (ACME, OIDC)
- Deploy to production with proper security
