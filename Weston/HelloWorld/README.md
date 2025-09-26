# Weston/Wayland HelloWorld GUI Applications

This project contains multiple HelloWorld GUI applications that demonstrate different approaches to creating GUI applications in a Wayland environment, with a focus on Weston-specific features:

## Applications

### 1. Wayland Client (`helloworld`)
A modern Wayland client using xdg-shell protocol.

**Features:**
- Uses modern xdg-shell protocol
- Creates a visible window with colorful gradient
- Demonstrates proper Wayland client functionality
- Interactive window management

**To run:**
```bash
./helloworld
```

### 2. Weston Simulator (`weston_simulator`)
A Weston-like Wayland compositor simulator using X11.

**Features:**
- Simulates a Wayland compositor environment
- Visual representation of Wayland surfaces
- Can run Wayland clients
- Interactive compositor interface

**To run:**
```bash
./weston_simulator
```

**Controls:**
- Press 'r' to run a Wayland client
- Press 'q' to quit
- Click to interact with the interface

## Building

### Using Makefile
```bash
make
```

### Using CMake (for GTK3 app only)
```bash
mkdir build
cd build
cmake ..
make
```

## Requirements

- Wayland client libraries
- GCC compiler
- pkg-config

## Weston-Specific Features

The Weston applications demonstrate unique Weston capabilities:

### Weston Basic Client
- **Interface Discovery**: Shows all available Wayland interfaces in the Weston environment
- **Registry Handling**: Demonstrates proper Wayland registry management
- **Connection Management**: Shows how to connect and disconnect from Wayland displays

### Weston Simple Client
- **High-Precision Input**: Demonstrates Weston's advanced input handling capabilities
- **Real-Time Tracking**: Shows mouse and keyboard input in real-time
- **Surface Management**: Demonstrates proper Wayland surface lifecycle
- **Interactive Features**: Responds to user input with visual feedback

## Notes

- The native Wayland clients require a Wayland compositor (like Weston) to be running
- The Weston-specific applications demonstrate unique Weston features
- All applications show different levels of Wayland integration

## Weston Compositor

Weston is the reference implementation of a Wayland compositor. The applications provided here demonstrate:

- **Basic Wayland Integration**: How to connect and interact with Wayland displays
- **Weston-Specific Features**: Unique capabilities of the Weston compositor
- **Input Handling**: Advanced input processing capabilities
- **Surface Management**: Proper Wayland surface lifecycle management

The Weston source code is included in the `weston/` directory for reference and potential future development.
