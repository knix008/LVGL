# MiniGUI Hello World Application

This project demonstrates a simple MiniGUI application that creates a window with buttons and text input controls.

## Overview

MiniGUI is a lightweight GUI library for embedded systems and Linux desktop applications. This project includes:
- A Hello World MiniGUI application (`main.c`)
- Build configuration (`Makefile`)
- Run script (`run.sh`)
- MiniGUI configuration (`MiniGUI.cfg`)

## Features

The application creates a window with:
- A title "MiniGUI Hello World"
- Two buttons: "Click Me" and "Exit"
- A text input field
- A label for the text input

## Prerequisites

- Linux system (tested on Ubuntu/Debian)
- GCC compiler
- Make build tool
- X11 development libraries (for pc_xvfb engine)

## Installation

### 1. Build MiniGUI

The project includes a local MiniGUI installation script. MiniGUI is built and installed locally in the `minigui-local` directory inside this project.

```bash
# Make the installation script executable
chmod +x install_minigui.sh

# Run the installation (this will download, configure, and build MiniGUI)
./install_minigui.sh
```

### 2. Build the Application

```bash
# Build the Hello World application
make
```

This will create the `helloworld` executable.

## Running the Application

### Using the Run Script (Recommended)

```bash
# Make the run script executable
chmod +x run.sh

# Run the application
./run.sh
```

The run script automatically:
- Sets up the library path
- Configures MiniGUI environment variables
- Uses the pc_xvfb graphics engine
- Runs the application

### Manual Execution

If you prefer to run manually:

```bash
# Set environment variables
export LD_LIBRARY_PATH="$(pwd)/minigui-local/usr/local/lib:$LD_LIBRARY_PATH"
export MG_RUNTIME_MODE="standalone"
export MG_GAL_ENGINE="pc_xvfb"
export MG_IAL_ENGINE="pc_xvfb"

# Run the application
./helloworld
```

## Configuration

The `MiniGUI.cfg` file contains MiniGUI configuration settings:

- **Graphics Engine**: pc_xvfb (PC Virtual FrameBuffer)
- **Input Engine**: pc_xvfb
- **Display Mode**: 800x600-16bpp
- **Runtime Mode**: standalone

## File Structure

```
MiniGUI/
├── main.c              # Main application source code
├── Makefile            # Build configuration
├── run.sh              # Run script with environment setup
├── MiniGUI.cfg         # MiniGUI configuration file
├── install_minigui.sh  # MiniGUI installation script
├── build-minigui-4.0/  # MiniGUI build scripts and sources (local)
├── minigui-build/      # Build directory (local)
├── minigui-local/      # Local MiniGUI install (local)
├── helloworld          # Compiled executable
└── README.md           # This file
```

## Application Code

The main application (`main.c`) demonstrates:

- MiniGUI initialization
- Window creation with title and size
- Button controls with event handlers
- Text input control
- Static text label
- Message box dialogs
- Proper cleanup and exit handling

### Key Functions:

- `on_button1_clicked()`: Shows a message box when "Click Me" is pressed
- `on_button2_clicked()`: Exits the application when "Exit" is pressed
- `main()`: Initializes MiniGUI, creates the window, and runs the event loop

## Troubleshooting

### Common Issues:

1. **Library not found errors**:
   - Ensure the library path is set correctly
   - Check that MiniGUI was installed properly

2. **Graphics engine errors**:
   - The pc_xvfb engine requires X11
   - If X11 is not available, you can use the dummy engine for testing

3. **Build errors**:
   - Make sure all dependencies are installed
   - Check that MiniGUI headers are in the correct location

### Alternative Graphics Engines:

If pc_xvfb doesn't work, you can try:

- **dummy**: For testing without display (modify `run.sh` and `MiniGUI.cfg`)
- **drm**: For direct framebuffer access (requires proper permissions)

## Development

To modify the application:

1. Edit `main.c` to add new controls or functionality
2. Rebuild with `make`
3. Test with `./run.sh`

## MiniGUI Resources

- [MiniGUI Official Documentation](http://www.minigui.com/)
- [MiniGUI Programming Guide](http://www.minigui.com/doc/)
- [MiniGUI API Reference](http://www.minigui.com/doc/)

## License

This project is for educational purposes. MiniGUI has its own licensing terms - please refer to the MiniGUI documentation for details.

## System Information

- **MiniGUI Version**: 4.0
- **Build Date**: (see install_minigui.sh run date)
- **Architecture**: (see your system)
- **OS**: (see your system)