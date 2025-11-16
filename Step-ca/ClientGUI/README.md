# HTTPS Client GUI

A graphical user interface for the HTTPS client, providing an easy way to make HTTPS requests with certificate-based authentication.

## Features

- **Visual Request Builder**: Easy-to-use interface for making HTTP requests
- **Multiple HTTP Methods**: Support for GET, POST, PUT, DELETE
- **Certificate Management**: Visual certificate status and configuration
- **Response Display**: Separate tabs for body, headers, and raw output
- **Request History**: Automatic saving of recent requests
- **Syntax Highlighting**: JSON body formatting
- **Auto-completion**: URL and method suggestions

## Prerequisites

- Python 3.8+
- GTK 3.0+
- HTTPS Client executable (built from ../Client)
- Client certificates (generated via step-ca)

## Quick Start

### 1. Check Dependencies

```bash
make check-deps
```

This will verify:
- Python and GTK are installed
- Client executable exists
- Certificates are present

### 2. Run the GUI

```bash
make run
```

The application will launch with a clean interface ready to make requests.

## Usage

### Making a Request

1. **Enter URL**: Type the target URL (e.g., `https://localhost:8443/api/status`)
2. **Select Method**: Choose GET, POST, PUT, or DELETE
3. **Add Body** (optional): For POST/PUT, enter JSON data in the request body
4. **Click "Send Request"**: The request will be sent and the response displayed

### Example Requests

**GET Request**:
- URL: `https://localhost:8443/api/status`
- Method: GET
- Click "Send Request"

**POST Request**:
- URL: `https://localhost:8443/api/data`
- Method: POST
- Body: `{"name": "test", "value": 123}`
- Click "Send Request"

### Response Tabs

The response is displayed in three tabs:

1. **Body**: The response body content
2. **Headers**: HTTP response headers
3. **Raw**: Complete raw output from the client

### Settings

Click the "Settings" button to configure:
- Client executable path
- Client certificate location
- Client key location
- Root CA certificate location

## Keyboard Shortcuts

- `Ctrl+Enter`: Send request
- `Ctrl+L`: Clear response
- `Ctrl+,`: Open settings

## Features Explained

### Status Indicator

The top-right indicator shows the client status:
- **● Ready** (green): Client and certificates are ready
- **● Certificates Missing** (orange): Client found but certificates missing
- **● Client Not Found** (red): Client executable not available

### Request History

Recent requests are automatically saved and can be recalled from the history.

### Auto-scrolling

The response view automatically scrolls to show new content as it arrives.

## Troubleshooting

### "Client Not Found" Error

The client executable is missing. Build it:
```bash
cd ../Client
make all
```

### "Certificates Missing" Warning

Client certificates haven't been generated. Create them:
```bash
cd ../Client
make run-cert-setup
```

### "Invalid URL" Error

Ensure the URL:
- Starts with `https://`
- Has a valid hostname and optional port
- Example: `https://localhost:8443/api/status`

### "Invalid JSON" Error

For POST/PUT requests, the body must be valid JSON:
```json
{
  "key": "value",
  "number": 123,
  "array": [1, 2, 3]
}
```

### Connection Errors

If requests fail:
1. Ensure the server is running
2. Check the URL and port are correct
3. Verify certificates are valid and not expired
4. Check that step-ca is running

## Makefile Targets

- `make run` - Run the application
- `make dev` - Run with GTK debugging enabled
- `make check-deps` - Check all dependencies
- `make install-deps` - Install Python dependencies
- `make build-client` - Build the C client executable
- `make clean` - Clean generated files
- `make help` - Show help message

## Project Structure

```
ClientGUI/
├── src/
│   ├── main.py              # Application entry point
│   ├── core/
│   │   ├── __init__.py
│   │   ├── client.py        # HTTPS client wrapper
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

The main window is defined in `src/gui/main_window.py`. The layout uses GTK 3 widgets with a clean, modern design.

### Adding Features

Key files to modify:
- **Request handling**: `src/core/client.py`
- **UI layout**: `src/gui/main_window.py`
- **Utilities**: `src/core/utils.py`

## Integration with Server

To test the complete setup:

1. **Terminal 1** - Start step-ca:
   ```bash
   cd /home/shkwon/Projects/LVGL/Step-ca
   ./bin/step-ca ~/.step/config/ca.json
   ```

2. **Terminal 2** - Start the HTTPS server:
   ```bash
   cd Server
   make run
   ```
   Or use the Server GUI:
   ```bash
   cd ../ServerGUI
   make run
   ```

3. **Terminal 3** - Run the Client GUI:
   ```bash
   cd ClientGUI
   make run
   ```

## Tips

- Use the history feature to quickly retry previous requests
- Enable auto-scroll to see response data as it arrives
- Save common endpoints in your request history
- Use the settings to point to different certificate locations
- Check the raw tab for debugging connection issues

## License

This project is provided as-is for educational and development purposes.

## Related Documentation

- [Main Project README](../README.md)
- [HTTPS Client README](../Client/README.md)
- [HTTPS Server README](../Server/README.md)
- [Server GUI README](../ServerGUI/README.md)
