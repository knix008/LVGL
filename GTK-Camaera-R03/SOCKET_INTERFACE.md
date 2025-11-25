# Unix Domain Socket Interface Documentation

## Overview

The GTK Face Recognition application provides a Unix domain socket interface for remote command control. This allows external applications, scripts, and utilities to control the face recognition system without direct GUI interaction.

**Socket Path:** `/tmp/face_recognition.sock`

**Protocol:** Simple text-based command/response protocol

---

## Command Format

### Request Format
```
<command>:<arg1>:<arg2>:...
```

Commands are sent as plain text strings to the socket, with colon-separated arguments.

### Response Format
```
OK:<response_data>
```
or
```
ERROR:<error_message>
```

Responses start with `OK:` for success or `ERROR:` for failure, followed by relevant data or error description.

---

## Available Commands

### 1. **camera_on** - Start Camera

**Format:**
```
camera_on
```

**Description:** Start the webcam and begin live stream

**Example:**
```bash
echo "camera_on" | nc -U /tmp/face_recognition.sock
./socket_client camera_on
```

**Response:**
```
OK:Camera started
OK:Camera already running
```

**Error Cases:**
- Camera device not found
- Camera already running

---

### 2. **camera_off** - Stop Camera

**Format:**
```
camera_off
```

**Description:** Stop the webcam and pause live stream

**Example:**
```bash
echo "camera_off" | nc -U /tmp/face_recognition.sock
./socket_client camera_off
```

**Response:**
```
OK:Camera stopped
OK:Camera already stopped
```

---

### 3. **capture** - Capture and Register New Person

**Format:**
```
capture:<initial>:<id>
```

**Parameters:**
- `<initial>`: Single letter person initial (A-Z), will be converted to uppercase
- `<id>`: Numeric person ID (1-9999)

**Description:**
Captures the current frame from the camera, extracts face embedding, and registers a new person for recognition. The captured face must be clearly visible for proper embedding extraction.

**Example:**
```bash
./socket_client capture:A:1      # Register person "A1"
./socket_client capture:B:2      # Register person "B2"
./socket_client capture:John:1   # Register person "John1"
```

**Process:**
1. Captures current frame from camera
2. Detects face in the image using Haar Cascade
3. Extracts face ROI (region of interest)
4. Generates face embedding using ArcFace model
5. Stores embedding in database
6. Adds embedding to FAISS recognition index
7. Person is immediately available for recognition

**Response:**
```
OK:Photo captured and person added - A1
```

**Error Cases:**
```
ERROR:Camera not running
ERROR:Missing arguments. Usage: capture:A:1
ERROR:Invalid ID. Must be numeric.
ERROR:Failed to capture photo
ERROR:Failed to register person
ERROR:Failed to extract embedding
ERROR:Failed to add to recognition model
```

**Notes:**
- Person name is generated as: `<initial><id>` (e.g., "A1", "B2")
- Requires camera to be running (camera_on)
- Face must be clearly visible and well-lit
- Multiple photos of the same person can be captured (sequence: 1.jpg, 2.jpg, 3.jpg, etc.)
- Photos are saved to: `dataset/<person_name>/<sequence>.jpg`

---

### 4. **registering** - Train Recognition Model

**Format:**
```
registering
```

**Description:**
Train or retrain the face recognition model using all captured embeddings in the database. This builds the FAISS index for fast similarity search.

**Example:**
```bash
./socket_client registering
```

**Process:**
1. Loads all embeddings from database
2. Builds FAISS flat index with all embeddings
3. Sets up person_id → name label mappings
4. Saves FAISS index to disk for persistence
5. Enables face recognition

**Response:**
```
OK:Training started
```

**Error Cases:**
```
ERROR:Training already in progress
```

**Notes:**
- Only one training session can run at a time
- Training happens asynchronously in a separate thread
- Does not block socket communication
- After training, all registered persons are available for recognition
- This is useful after capturing multiple new people

---

### 5. **status** - Get Application Status

**Format:**
```
status
```

**Description:**
Get current status and statistics of the application

**Example:**
```bash
./socket_client status
```

**Response:**
```
OK:camera_running:true,recognition_enabled:true,training_in_progress:false,people_count:5,total_faces:12
```

**Response Fields:**
- `camera_running`: Is camera currently running (true/false)
- `recognition_enabled`: Is face recognition enabled (true/false)
- `training_in_progress`: Is training currently running (true/false)
- `people_count`: Total number of registered people
- `total_faces`: Total number of captured face images

**Example Usage:**

Check if camera is running:
```bash
status_response=$(./socket_client status)
if [[ $status_response == *"camera_running:true"* ]]; then
    echo "Camera is running"
else
    echo "Camera is not running"
fi
```

---

## Usage Examples

### Bash Script Example

```bash
#!/bin/bash

SOCKET_CLIENT="./socket_client"

# Start camera
echo "Starting camera..."
$SOCKET_CLIENT camera_on

sleep 1

# Capture 3 photos of person A1
echo "Capturing photos for person A1..."
$SOCKET_CLIENT capture:A:1
sleep 1
$SOCKET_CLIENT capture:A:1
sleep 1
$SOCKET_CLIENT capture:A:1

# Capture 2 photos of person B2
echo "Capturing photos for person B2..."
$SOCKET_CLIENT capture:B:2
sleep 1
$SOCKET_CLIENT capture:B:2

# Train model
echo "Training recognition model..."
$SOCKET_CLIENT registering

sleep 3

# Check status
echo "Getting status..."
$SOCKET_CLIENT status

# Stop camera
echo "Stopping camera..."
$SOCKET_CLIENT camera_off
```

### Python Example

```python
#!/usr/bin/env python3

import socket
import sys

SOCKET_PATH = "/tmp/face_recognition.sock"

def send_command(command):
    """Send command to face recognition server"""
    try:
        sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        sock.connect(SOCKET_PATH)
        sock.sendall(command.encode())
        response = sock.recv(4096).decode()
        sock.close()
        return response
    except ConnectionRefusedError:
        return "ERROR:Server not running"
    except Exception as e:
        return f"ERROR:{str(e)}"

# Examples
print("Starting camera:", send_command("camera_on"))
print("Capturing person:", send_command("capture:Alice:1"))
print("Status:", send_command("status"))
print("Stopping camera:", send_command("camera_off"))
```

### C++ Example

```cpp
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <string>

std::string send_socket_command(const std::string& command) {
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, "/tmp/face_recognition.sock",
            sizeof(addr.sun_path) - 1);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        return "ERROR:Connection failed";
    }

    write(sock, command.c_str(), command.length());

    char buffer[4096] = {0};
    ssize_t bytes = read(sock, buffer, sizeof(buffer) - 1);
    close(sock);

    return std::string(buffer, bytes);
}

// Usage
std::string response = send_socket_command("camera_on");
```

---

## Socket Client Utility

The project includes a pre-built socket client utility for easy testing and scripting.

**Build:**
```bash
make socket_client
```

**Usage:**
```bash
./socket_client <command> [args...]
```

**Examples:**
```bash
./socket_client camera_on
./socket_client capture:A:1
./socket_client registering
./socket_client status
./socket_client camera_off
```

**Exit Codes:**
- 0: Command succeeded (response starts with "OK:")
- 1: Command failed (response starts with "ERROR:")

---

## Implementation Details

### Socket Server Architecture

```
┌─────────────────────────────────────┐
│    SocketServer (Main Thread)       │
│  - Listens on /tmp/face_recognition │
│  - Accepts incoming connections     │
│  - Routes commands to handlers      │
└──────────────┬──────────────────────┘
               │
        ┌──────▼──────┐
        │ Server Loop │ (separate thread)
        └──────┬──────┘
               │
        ┌──────▼───────────────┐
        │ Accept Connection    │
        └──────┬───────────────┘
               │
        ┌──────▼───────────────┐
        │ Handle Client        │
        │ - Read command       │
        │ - Execute handler    │
        │ - Send response      │
        │ - Close connection   │
        └──────────────────────┘
```

### Command Execution Flow

```
Socket Connection
    │
    ├─→ Read command string (max 1024 bytes)
    │
    ├─→ Parse command and arguments
    │
    ├─→ Look up command handler
    │   ├─→ camera_on → handle_camera_on()
    │   ├─→ camera_off → handle_camera_off()
    │   ├─→ capture → handle_capture()
    │   ├─→ registering → handle_registering()
    │   └─→ status → handle_status()
    │
    ├─→ Execute handler (returns response)
    │
    ├─→ Send response to client
    │
    └─→ Close connection
```

### Thread Safety

- **Main GTK Thread:** Handles GUI, camera input, and frame display
- **Socket Server Thread:** Accepts connections and dispatches commands
- **Training Thread:** Runs long-running training operations
- **Command Handlers:** Run in socket server thread, synchronously

**Mutex Protection:**
- Face recognition operations are protected by `recognition_mutex` (ONNX Runtime is not thread-safe)
- Database operations use SQLite's built-in locking

---

## Error Handling

All operations include error checking and reporting:

1. **Connection Errors:** Reported if server unavailable
2. **Command Parsing Errors:** "Missing arguments" or "Invalid format"
3. **Operation Errors:** Specific to the command being executed
4. **System Errors:** File I/O, camera, database errors

Error responses provide actionable information to help debugging.

---

## Security Considerations

⚠️ **Important:** The socket interface is not encrypted or authenticated. Use in trusted environments only.

**Security Recommendations:**
1. Restrict socket file permissions: `chmod 600 /tmp/face_recognition.sock`
2. Use only in isolated networks or local development
3. Consider implementing authentication for production use
4. Run application with least necessary privileges

---

## Performance Notes

- **Socket Communication:** < 1ms per command
- **Capture Operation:** 100-300ms (depends on face detection)
- **Status Query:** < 1ms (just reads application state)
- **Training Operation:** 5-60 seconds (depends on number of embeddings)

---

## Troubleshooting

### "Connection refused"
- Ensure main application is running
- Check socket path: `/tmp/face_recognition.sock`
- Verify socket file exists: `ls -la /tmp/face_recognition.sock`

### "Camera not running"
- Send `camera_on` command first

### "Training already in progress"
- Wait for previous training to complete
- Check status with `status` command

### "Failed to capture photo"
- Ensure camera is running
- Check that face is clearly visible in camera frame
- Verify sufficient lighting

### "Failed to extract embedding"
- Face may be too small (minimum 80×80 pixels)
- Face may be obscured or at poor angle
- Try capturing again with better positioning

---

## Future Enhancements

- [ ] Authentication/authorization
- [ ] TLS encryption for remote connections
- [ ] Batch operations (capture multiple people)
- [ ] Confidence threshold adjustment via socket
- [ ] Face recognition on specific regions
- [ ] Statistics and metrics reporting
- [ ] Event notifications/subscriptions
- [ ] Rate limiting and connection pooling

---

## See Also

- [ARCHITECTURE.md](ARCHITECTURE.md) - System architecture and design
- [README.md](README.md) - Main project documentation
