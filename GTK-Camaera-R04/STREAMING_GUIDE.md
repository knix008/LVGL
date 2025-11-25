# Face Recognition Streaming Guide

## Overview

The GTK Face Recognition application now supports real-time streaming of face recognition results over Unix domain sockets. This allows client applications to receive continuous updates about detected and recognized faces without polling.

## Features

- **Real-time Updates**: Recognition results streamed every 500ms
- **Persistent Connection**: Socket stays open until client disconnects
- **Simple Protocol**: Text-based protocol with structured data
- **Multiple Clients**: Each client gets their own streaming connection
- **GTK Client**: Built-in GUI client with live recognition display

## Quick Start

### 1. Start the Server
```bash
./run.sh
# Or directly:
./gtk_webcam
```

### 2. Start the GTK Client
```bash
./gtk_client
```

### 3. Use the Client
1. Click "Camera On" to start the camera
2. Click "Start Recognition Stream" to begin receiving real-time updates
3. Watch the "Recognition Stream" panel for live results:
   - `[HH:MM:SS] ✓ PersonName (Confidence: XX%)` - Face recognized
   - `[HH:MM:SS] - No face detected` - No face in current frame

## Protocol Details

### Command Format

**Request:**
```
stream_recognition
```

**Response:**
```
OK:Stream started
FACE:<name>:<confidence>:<timestamp>
FACE:<name>:<confidence>:<timestamp>
...
NO_FACE:<timestamp>
...
```

### Message Types

#### Initial Response
```
OK:Stream started\n
```

#### Face Recognized
```
FACE:A1:85:1700000123456\n
```
- `FACE`: Message type
- `A1`: Person name
- `85`: Confidence percentage (0-100)
- `1700000123456`: Unix timestamp in milliseconds

#### No Face Detected
```
NO_FACE:1700000124456\n
```
- `NO_FACE`: Message type
- `1700000124456`: Unix timestamp in milliseconds

## Client Implementation Examples

### Python Client

```python
#!/usr/bin/env python3
import socket
import time

SOCKET_PATH = "/tmp/face_recognition.sock"

def stream_recognition():
    sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    sock.connect(SOCKET_PATH)
    sock.sendall(b"stream_recognition")
    
    # Read initial response
    data = sock.recv(1024).decode()
    print(data)  # "OK:Stream started"
    
    # Read continuous stream
    buffer = ""
    try:
        while True:
            data = sock.recv(1024).decode()
            if not data:
                break
            
            buffer += data
            while '\n' in buffer:
                line, buffer = buffer.split('\n', 1)
                process_line(line)
    
    except KeyboardInterrupt:
        print("\nStopping stream...")
    finally:
        sock.close()

def process_line(line):
    if line.startswith("FACE:"):
        parts = line.split(':')
        name = parts[1]
        confidence = parts[2]
        timestamp = parts[3]
        print(f"✓ Recognized: {name} (Confidence: {confidence}%)")
    
    elif line.startswith("NO_FACE:"):
        print("- No face detected")

if __name__ == "__main__":
    stream_recognition()
```

### C++ Client (Using SocketClientLib)

```cpp
#include "socket_client_lib.h"
#include <iostream>
#include <unistd.h>
#include <cstring>

void stream_recognition() {
    SocketClientLib client;
    
    // Start streaming
    int sock = client.stream_recognition();
    if (sock < 0) {
        std::cerr << "Failed to connect" << std::endl;
        return;
    }
    
    // Read initial response
    char buffer[1024];
    ssize_t bytes = read(sock, buffer, sizeof(buffer) - 1);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        std::cout << buffer;  // "OK:Stream started\n"
    }
    
    // Read stream
    std::string line_buffer;
    while (true) {
        bytes = read(sock, buffer, sizeof(buffer) - 1);
        if (bytes <= 0) break;
        
        buffer[bytes] = '\0';
        line_buffer += buffer;
        
        // Process complete lines
        size_t pos;
        while ((pos = line_buffer.find('\n')) != std::string::npos) {
            std::string line = line_buffer.substr(0, pos);
            line_buffer.erase(0, pos + 1);
            
            if (line.find("FACE:") == 0) {
                // Parse: FACE:name:confidence:timestamp
                size_t c1 = line.find(':');
                size_t c2 = line.find(':', c1 + 1);
                size_t c3 = line.find(':', c2 + 1);
                
                std::string name = line.substr(c1 + 1, c2 - c1 - 1);
                std::string conf = line.substr(c2 + 1, c3 - c2 - 1);
                
                std::cout << "✓ " << name 
                         << " (Confidence: " << conf << "%)" 
                         << std::endl;
            }
            else if (line.find("NO_FACE:") == 0) {
                std::cout << "- No face detected" << std::endl;
            }
        }
    }
    
    close(sock);
}

int main() {
    stream_recognition();
    return 0;
}
```

### Shell Script (using netcat)

```bash
#!/bin/bash

SOCKET="/tmp/face_recognition.sock"

echo "stream_recognition" | nc -U "$SOCKET" | while IFS= read -r line; do
    if [[ $line == FACE:* ]]; then
        IFS=':' read -ra PARTS <<< "$line"
        NAME="${PARTS[1]}"
        CONF="${PARTS[2]}"
        echo "[$(date +%H:%M:%S)] ✓ $NAME (Confidence: $CONF%)"
    elif [[ $line == NO_FACE:* ]]; then
        echo "[$(date +%H:%M:%S)] - No face detected"
    elif [[ $line == OK:* ]]; then
        echo "$line"
    fi
done
```

## GTK Client Features

The built-in GTK client provides:

### Main Controls
- **Camera On/Off**: Control camera state
- **Capture Person**: Register new people (Initial + ID)
- **Train Model**: Build recognition model from captured faces
- **Get Status**: Query server state

### Recognition Stream Panel
- **Start/Stop Stream**: Toggle live recognition stream
- **Auto-scroll**: Automatically shows latest results
- **Line Limiting**: Keeps last 50 lines for performance
- **Formatted Display**: Clean timestamp + recognition results

### Stream Display Format
```
[14:32:15] ✓ A1 (Confidence: 85%)
[14:32:16] ✓ A1 (Confidence: 87%)
[14:32:16] - No face detected
[14:32:17] - No face detected
[14:32:17] ✓ B2 (Confidence: 92%)
```

## Performance Considerations

### Update Frequency
- Default: 500ms (2 updates per second)
- Configurable in `src/gtk_app.cpp::handle_stream_recognition()`
- Change `usleep(500000)` to adjust interval

### Network Performance
- Each update: ~50-100 bytes
- Bandwidth: ~200 bytes/second per client
- Multiple clients supported (independent connections)

### Resource Usage
- Minimal CPU overhead (streaming runs in socket thread)
- No impact on face recognition performance
- Each client connection adds one thread

## Troubleshooting

### Stream Not Starting

**Problem**: `Failed to connect to server stream`

**Solutions:**
1. Ensure server is running: `ps aux | grep gtk_webcam`
2. Check socket exists: `ls -la /tmp/face_recognition.sock`
3. Verify permissions: `chmod 666 /tmp/face_recognition.sock`

### No Recognition Results

**Problem**: Only seeing `NO_FACE` messages

**Solutions:**
1. Start camera: Click "Camera On" or `./socket_client camera_on`
2. Train model: Click "Train Model" or `./socket_client registering`
3. Ensure faces are visible in camera view

### Connection Drops

**Problem**: Stream disconnects unexpectedly

**Solutions:**
1. Server may have stopped - restart: `./run.sh`
2. Check server logs for errors
3. Network issues - reconnect client

### Delayed Updates

**Problem**: Recognition results appear slow

**Solutions:**
1. Check server CPU usage: `top` (recognition is CPU-intensive)
2. Reduce camera resolution for faster processing
3. Adjust stream interval (currently 500ms)

## Architecture

### Server Side (gtk_app.cpp)

```
┌─────────────────────────────────┐
│   GTKApp (Main Thread)          │
│   - Camera capture              │
│   - Face recognition            │
│   - Updates cached results      │
└────────────┬────────────────────┘
             │
             │ Mutex-protected access
             ▼
      ┌──────────────────┐
      │ Recognition Cache │
      │ - last_name       │
      │ - last_confidence │
      │ - has_result      │
      └──────────────────┘
             ▲
             │ Read every 500ms
             │
┌────────────┴────────────────────┐
│ SocketServer (Separate Thread)  │
│ - Handles streaming command     │
│ - Sends formatted updates       │
└──────────────────────────────────┘
```

### Client Side (gtk_client.cpp)

```
┌─────────────────────────────────┐
│   GTK Main Thread               │
│   - UI rendering                │
│   - Button handlers             │
└────────────┬────────────────────┘
             │
             │ Start/Stop stream
             ▼
      ┌──────────────────┐
      │ Stream Thread     │
      │ - Socket I/O      │
      │ - Parse messages  │
      │ - Queue UI updates│
      └─────────┬─────────┘
                │
                │ g_idle_add()
                ▼
      ┌──────────────────┐
      │ GTK Main Thread   │
      │ - Update text view│
      │ - Auto-scroll     │
      └──────────────────┘
```

## Integration with Other Systems

### Web Dashboard
Stream recognition data to a web dashboard:
1. Create a WebSocket server that connects to Unix socket
2. Bridge Unix socket data to WebSocket clients
3. Display results in browser with JavaScript

### Database Logging
Log recognition events to database:
1. Connect to stream from Python script
2. Parse recognition events
3. Insert into database with timestamps

### Alert System
Send notifications on specific recognitions:
1. Monitor stream for specific person names
2. Trigger alerts (email, SMS, webhook)
3. Log events for security monitoring

## See Also

- [SOCKET_INTERFACE.md](SOCKET_INTERFACE.md) - Complete socket protocol reference
- [README.md](README.md) - Main project documentation
- [ARCHITECTURE.md](ARCHITECTURE.md) - System architecture details
