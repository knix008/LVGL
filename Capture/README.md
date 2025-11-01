# Webcam Photo Capture Application

A webcam photo capture application built with LVGL (Light and Versatile Graphics Library) on Linux with Korean language support.

## Features

- Live webcam preview using V4L2 (Video4Linux2)
- Modern GUI with LVGL v9.2
- **Korean language interface (한국어 지원)**
- Click-to-capture functionality
- Photo counter
- Saves photos in PPM format
- Window size: 340x640 pixels
- Camera preview: 320x240 pixels
- **Modular architecture** (separated logic and GUI)

## Requirements

- Linux operating system
- GCC compiler
- SDL2 development libraries
- FreeType development libraries
- V4L2 compatible webcam
- Git
- Python 3
- Node.js and npm (for font generation)
- lv_font_conv (installed automatically by setup script)

## Installation

Run the automated setup script:

```bash
./setup.sh
```

This script will:
1. Check for required system packages (SDL2, GCC, Git, FreeType, Python3, Node.js)
2. Install missing packages (with your permission)
3. Install lv_font_conv (font converter)
4. Download Nanum Gothic Korean font
5. Generate Korean fonts for LVGL
6. Check webcam availability
7. Clone LVGL v9.2
8. Build the LVGL library
9. Build the webcam capture application

## Manual Build

If you prefer to build manually:

```bash
# 1. Download Korean font
wget https://github.com/google/fonts/raw/main/ofl/nanumgothic/NanumGothic-Regular.ttf -O assets/NanumGothic.ttf

# 2. Install lv_font_conv
sudo npm install -g lv_font_conv

# 3. Generate Korean fonts
./generate_fonts.py

# 4. Build LVGL library
mkdir -p lvgl/build lvgl/lib
find lvgl/src -name "*.c" -exec gcc -Wall -Wextra -O2 -I. -Ilvgl $(pkg-config --cflags freetype2) -c {} -o lvgl/build/{}.o \;
ar rcs lvgl/lib/liblvgl.a lvgl/build/*.o

# 5. Build the application
make
```

## Usage

1. Connect your webcam (should appear as /dev/video0)
2. Run the application:

```bash
./webcam_capture
```

3. The GUI will display (in Korean):
   - Title: "웹캠 캡처" (Webcam Capture)
   - Live camera preview
   - "촬영" button (Capture)
   - Photo counter: "사진: N"
   - Status messages in Korean

4. Click the "촬영" (CAPTURE) button to take a photo
5. Photos are saved with timestamps: `photo_YYYYMMDD_HHMMSS.ppm`
6. Press ESC or close window to exit

## Converting PPM Files

PPM files can be converted to other formats using ImageMagick:

```bash
# Install ImageMagick
sudo apt-get install imagemagick

# Convert to JPEG
convert photo_20241101_120000.ppm photo.jpg

# Convert to PNG
convert photo_20241101_120000.ppm photo.png
```

## Project Structure

```
.
├── main.c              # Main application entry point
├── camera.c/h          # Camera capture logic (V4L2)
├── gui.c/h             # GUI components (LVGL)
├── Makefile            # Build configuration
├── setup.sh            # Automated setup script
├── generate_fonts.py   # Korean font generator
├── convert_photos.sh   # PPM to JPEG converter
├── lv_conf.h           # LVGL configuration
├── lvgl/               # LVGL library (cloned)
├── assets/             # Assets directory
│   ├── NanumGothic.ttf # Korean font file
│   └── fonts/          # Generated LVGL fonts
└── README.md           # This file
```

## Architecture

The application follows a modular design:

- **[camera.c](camera.c)** / **[camera.h](camera.h)**: Camera hardware abstraction
  - V4L2 device initialization
  - Frame capture thread
  - Photo saving functionality

- **[gui.c](gui.c)** / **[gui.h](gui.h)**: User interface layer
  - LVGL GUI components
  - Korean language text
  - Event handling callbacks

- **[main.c](main.c)**: Application coordinator
  - SDL2 initialization
  - LVGL setup
  - Main event loop

## Technical Details

### Camera Access
- Uses V4L2 (Video4Linux2) API for webcam access
- Memory-mapped buffers for efficient frame capture
- Separate thread for camera frame processing
- RGB24 format for compatibility

### GUI
- SDL2 backend for windowing
- LVGL canvas for camera preview
- Touch/mouse input support
- Real-time frame display

### Threading
- Main thread: GUI and event handling
- Camera thread: Frame capture and processing

## Troubleshooting

### Camera not found
```
Error: Opening video device: No such file or directory
```
- Check if webcam is connected: `ls -l /dev/video*`
- Check camera permissions: `sudo chmod 666 /dev/video0`
- Test camera with: `v4l2-ctl --device=/dev/video0 --list-formats`

### Build errors
```
Error: LVGL library not found
```
- Run `./setup.sh` to build LVGL library

### SDL2 errors
```
Error: SDL2 development libraries not found
```
- Install SDL2: `sudo apt-get install libsdl2-dev`

## Customization

### Change Camera Device
Edit [main.c:18](main.c#L18):
```c
#define VIDEO_DEVICE "/dev/video0"  // Change to your camera device
```

### Change Window Size
Edit [main.c:14-15](main.c#L14):
```c
#define WINDOW_WIDTH  340
#define WINDOW_HEIGHT 640
```

### Change Camera Resolution
Edit [main.c:16-17](main.c#L16):
```c
#define CAMERA_WIDTH  320
#define CAMERA_HEIGHT 240
```

## License

This project uses LVGL which is licensed under the MIT license.

## Credits

- LVGL: https://lvgl.io/
- SDL2: https://www.libsdl.org/
- V4L2: https://www.kernel.org/doc/html/latest/userspace-api/media/v4l/v4l2.html
