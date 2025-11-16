# HTTPS Server GUI

A graphical control panel for the HTTPS server, providing real-time monitoring, log viewing, and server management.

## Features

- **Server Control**: Start, stop, and restart the server with one click
- **Real-time Monitoring**: Live server status, uptime, and resource usage
- **Log Viewer**: Scrolling log display with timestamps
- **Certificate Status**: Visual indication of certificate validity
- **Process Management**: Automatic process monitoring and cleanup
- **Configuration**: Easy server and certificate path configuration

## Prerequisites

- Python 3.8+
- GTK 3.0+
- psutil (optional, for resource monitoring)
- HTTPS Server executable (built from ../Server)
- Server certificates (generated via step-ca)

## Quick Start

### 1. Check Dependencies

```bash
make check-deps
```

This will verify:
- Python and GTK are installed
- Server executable exists
- Certificates are present

### 2. Run the GUI

```bash
make run
```

The Server Control Panel will launch.

### 3. Start the Server

Click the "Start Server" button. The server will start and logs will appear in real-time.

## Usage

### Starting the Server

1. **Verify Status**: Check that the status indicator shows certificates are ready
2. **Click "Start Server"**: The server will start on the configured address
3. **Monitor Logs**: Server output appears in the logs section

### Stopping the Server

1. **Click "Stop Server"**: The server will gracefully shutdown
2. **Confirm**: Logs will show the shutdown sequence

### Restarting the Server

Click "Restart Server" to stop and start the server in one operation.

### Viewing Logs

The logs section shows:
- Timestamps for each log entry
- Server startup messages
- Connection attempts
- Request handling
- Error messages

**Log Controls**:
- **Clear Logs**: Remove all log entries
- **Auto-scroll**: Automatically scroll to new entries (enabled by default)

## Status Display

The status section shows:

- **Address**: Server listening address and port
- **State**: Running or Stopped
- **Uptime**: How long the server has been running
- **PID**: Process ID of the server
- **Memory**: Memory usage (requires psutil)
- **Certificates**: Certificate readiness status

### Status Indicators

**Server Status** (top-right):
- **● Running** (green): Server is active
- **● Stopped** (gray): Server is not running

**Certificate Status**:
- **✓ Ready** (green): All certificates present
- **✗ Missing** (red): One or more certificates missing

## Settings

Click the "Settings" button to configure:

- **Server Executable**: Path to the server binary
- **Listening Address**: Server address and port (e.g., `https://0.0.0.0:8443`)
- **Server Certificate**: Path to server.crt
- **Server Key**: Path to server.key
- **Root CA Certificate**: Path to root_ca.crt

Settings are automatically saved and restored on next launch.

## Advanced Features

### Resource Monitoring

If psutil is installed, the GUI displays:
- CPU usage percentage
- Memory consumption in MB
- Number of active connections

To install psutil:
```bash
pip3 install psutil
```

### Log Management

- Logs are automatically limited to 1000 lines
- Timestamps are added to each entry
- Auto-scroll can be toggled

### Process Management

The GUI properly manages the server process:
- Sends SIGTERM for graceful shutdown
- Monitors process health
- Handles crashes and restarts

## Troubleshooting

### "Server Not Found" Error

The server executable is missing. Build it:
```bash
cd ../Server
make all
```

### "Certificates Missing" Error

Server certificates haven't been generated. Create them:
```bash
cd ../Server
make run-cert-setup
```

### Server Won't Start

**Check logs for errors**:
- Port already in use
- Certificate files not found
- Permission denied

**Solutions**:
1. Ensure step-ca is running
2. Generate certificates: `cd ../Server && make run-cert-setup`
3. Change port in Settings if 8443 is in use
4. Check file permissions on certificates

### Server Stops Immediately

Check the logs for error messages. Common issues:
- Missing or invalid certificates
- Port conflict
- OpenSSL library issues

### "psutil not found" Warning

This is optional. The server will work without it, but resource monitoring won't be available.

Install psutil:
```bash
pip3 install psutil
```

## Makefile Targets

- `make run` - Run the Server GUI
- `make dev` - Run with GTK debugging enabled
- `make check-deps` - Check all dependencies
- `make install-deps` - Install Python dependencies (including psutil)
- `make build-server` - Build the C server executable
- `make clean` - Clean generated files
- `make help` - Show help message

## Project Structure

```
ServerGUI/
├── src/
│   ├── main.py              # Application entry point
│   ├── core/
│   │   ├── __init__.py
│   │   ├── server.py        # Server process manager
│   │   └── utils.py         # Utility functions
│   └── gui/
│       ├── __init__.py
│       └── main_window.py   # Main window interface
├── assets/                  # Icons and resources
├── Makefile                 # Build and run commands
├── requirements.txt         # Python dependencies
└── README.md               # This file
```

## Development

### Running in Debug Mode

```bash
make dev
```

This enables GTK debugging and provides additional diagnostic information.

### Modifying the Interface

The main window is defined in `src/gui/main_window.py`. The layout uses GTK 3 widgets.

### Adding Features

Key files to modify:
- **Server management**: `src/core/server.py`
- **UI layout**: `src/gui/main_window.py`
- **Utilities**: `src/core/utils.py`

## Integration with Client

To test the complete setup:

1. **Terminal 1** - Start step-ca:
   ```bash
   cd /home/shkwon/Projects/LVGL/Step-ca
   ./bin/step-ca ~/.step/config/ca.json
   ```

2. **Start Server GUI**:
   ```bash
   cd ServerGUI
   make run
   ```
   Click "Start Server" button

3. **Start Client GUI**:
   ```bash
   cd ../ClientGUI
   make run
   ```
   Make requests to the server

## Configuration File

Server settings are saved to `~/.https_server_config` in JSON format:

```json
{
  "server_path": "../Server/build/server",
  "address": "https://0.0.0.0:8443",
  "cert_path": "../Server/certs/server.crt",
  "key_path": "../Server/certs/server.key",
  "ca_path": "../Server/certs/root_ca.crt"
}
```

## Security Considerations

- The server runs with the same privileges as the GUI
- Certificates should be properly secured
- Use appropriate firewall rules for production
- Monitor the logs for unusual activity

## Tips

- Keep the GUI running to monitor server health
- Use auto-scroll to see new connections in real-time
- Check the uptime to verify server stability
- Monitor memory usage for resource leaks
- Review logs regularly for errors

## Performance

The GUI updates:
- Status: Every 1 second
- Logs: Every 100 milliseconds
- Resource usage: Every 1 second (if psutil available)

## License

This project is provided as-is for educational and development purposes.

## Related Documentation

- [Main Project README](../README.md)
- [HTTPS Server README](../Server/README.md)
- [HTTPS Client README](../Client/README.md)
- [Client GUI README](../ClientGUI/README.md)
