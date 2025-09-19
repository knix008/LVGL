# MiniGUI Application

This project contains a MiniGUI application for Linux:

**MiniGUI Application** (`main.c`) - Full MiniGUI implementation

## Features

The MiniGUI application provides:
- Main window with title bar
- Two buttons: "Click Me!" and "Exit"
- Text edit control
- Custom drawing with text output
- Event handling for button clicks
- Mouse and keyboard interaction

## Quick Start

After installing MiniGUI, build and run your application:

```bash
# Build the MiniGUI application
make

# Run the application
./helloworld
```

## Prerequisites

### For MiniGUI Application
Since `libminigui-dev` is not available in Ubuntu repositories, you need to install MiniGUI from source:

#### Option 1: Automated Installation (Recommended)
```bash
# Run the automated installation script
./install_minigui.sh
```

#### Option 2: Manual Installation
```bash
# Install dependencies
sudo apt-get update
sudo apt-get install -y git gcc g++ binutils autoconf automake libtool make cmake pkg-config \
    libgtk2.0-dev libjpeg-dev libpng-dev libfreetype6-dev libinput-dev libdrm-dev \
    libsqlite3-dev libxml2-dev libssl-dev

# Clone and build MiniGUI
git clone https://github.com/VincentWei/build-minigui-4.0.git
cd build-minigui-4.0
cp config.sh myconfig.sh
./fetch-all.sh
./build-deps.sh
./build-all.sh
```

## Building and Running

### MiniGUI Application
```bash
# Check if MiniGUI is installed
make check-minigui

# Build
make

# Run (may require MiniGUI server)
./helloworld
```

**Note:** MiniGUI applications typically need to run in a specific environment. You may need to:

1. Start the MiniGUI server (if using client-server mode):
```bash
mginit &
```

2. Or run in standalone mode (depending on your MiniGUI configuration).

## Usage

### MiniGUI Application:
- **Mouse interaction**: Click buttons and text field
- **"Click Me!" button**: Shows message box
- **"Exit" button**: Closes the application
- **Text input**: Click in the text field to edit

## Troubleshooting

### Common Issues:

1. **"pkg-config: command not found"**
   - Install pkg-config: `sudo apt-get install pkg-config`

2. **"No package 'minigui' found"**
   - MiniGUI may not be installed or not in the standard location
   - Check the Makefile for fallback paths

3. **"Cannot connect to MiniGUI server"**
   - Make sure mginit is running: `ps aux | grep mginit`
   - Start mginit if needed: `mginit &`

4. **Compilation errors**
   - Check that all MiniGUI headers are available
   - Verify the include paths in the Makefile

## File Structure

```
MiniGUI/
├── main.c              # MiniGUI application source
├── Makefile            # MiniGUI build configuration
├── install_minigui.sh  # MiniGUI installation script
└── README.md           # This file
```

## Customization

### MiniGUI Application:
- Change window size and position in `InitCreateInfo()`
- Add more controls in the `MSG_CREATE` case
- Handle additional events in the `MSG_COMMAND` case
- Customize the paint routine in `MSG_PAINT`

## License

This example code is provided as-is for educational purposes.
