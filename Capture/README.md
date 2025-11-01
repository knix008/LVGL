# Webcam Photo Capture Application

A webcam photo capture application built with LVGL GUI library, featuring Korean language support and live camera preview.

## Features

- **Live Webcam Preview** - Real-time camera feed display (320x240 @ 15 FPS)
- **Photo Capture** - Save high-quality JPEG photos with timestamp filenames
- **White Flash Effect** - Screen flash when capturing for better lighting
- **Shutter Sound** - Audible feedback with synthetic camera shutter sound
- **Korean UI** - Full Korean language interface using NanumGothicCoding font
- **Touch/Mouse Control** - Click the blue "촬영" (Capture) button to take photos
- **Status Display** - Real-time status messages and photo count

## Window Specifications

- **Window Size**: 340x640 pixels
- **Camera Resolution**: 320x240 pixels
- **Video Format**: RGB24 via FFmpeg
- **Photo Format**: JPEG (90% quality)
- **Audio**: SDL2 audio with synthetic shutter sound (44.1kHz, mono)

## Requirements

### System Dependencies
- GCC compiler
- SDL2 development libraries
- FreeType2 development libraries
- libjpeg development libraries
- FFmpeg development libraries (libavformat, libavcodec, libavdevice, libswscale, libavutil)
- Webcam device at `/dev/video0`

### Installation (Ubuntu/Debian)
```bash
sudo apt-get install build-essential
sudo apt-get install libsdl2-dev
sudo apt-get install libfreetype6-dev
sudo apt-get install libjpeg-dev
sudo apt-get install libavformat-dev libavcodec-dev libavdevice-dev libswscale-dev libavutil-dev
sudo apt-get install v4l-utils
```

### User Permissions
Add your user to the `video` group to access the webcam:
```bash
sudo usermod -a -G video $USER
```
Then log out and log back in for the changes to take effect.

## Building

### First Time Setup
Run the setup script to install dependencies and build LVGL:
```bash
./setup.sh
```

### Regular Build
```bash
make
```

### Clean Build
```bash
make clean
make
```

### Deep Clean (including LVGL)
```bash
make distclean
make
```

## Running

```bash
./camera
```

Or use the makefile target:
```bash
make run
```

### Controls
- **Click "촬영" button** - Capture and save photo
- **ESC key** - Exit application
- **Close window** - Exit application

## Project Structure

```
Capture/
├── main.c              # Application entry point, SDL/LVGL initialization
├── camera.c/h          # FFmpeg-based webcam capture logic
├── gui.c/h             # LVGL GUI with Korean fonts, flash, and audio
├── Makefile            # Build configuration
├── setup.sh            # Dependency checker and LVGL builder
├── rebuild_lvgl.sh     # LVGL library rebuild script
├── lv_conf.h           # LVGL configuration (FreeType enabled)
├── assets/             # Font files directory
│   ├── NanumGothicCoding.ttf
│   └── NanumGothicCoding-Bold.ttf
└── lvgl/               # LVGL library (git submodule)
```

## Architecture

### Modules

1. **main.c** - Initializes LVGL with SDL2 driver, manages main event loop
2. **camera.c** - FFmpeg-based webcam interface, JPEG encoding, threaded frame capture
3. **gui.c** - LVGL widgets, Korean font loading via FreeType, UI layout

### Key Technical Details

- Uses LVGL v9.2 with built-in SDL2 driver (`lv_sdl_window_create()`)
- FFmpeg for robust video capture (libavformat, libavcodec, libavdevice, libswscale)
- Multi-threaded camera capture with pthread
- Automatic color space conversion (any format to RGB888)
- FreeType integration for dynamic TTF font rendering
- RGB888 color format for image display
- Frame rate limiting (15 FPS) for optimal performance
- White flash overlay using LVGL timers for improved photo brightness
- Synthetic audio generation with SDL2 for shutter sound (two-tone beep)

## Output

Captured photos are saved in the current directory with timestamped filenames:
```
photo_YYYYMMDD_HHMMSS.jpg
```

Example: `photo_20251101_144323.jpg`

## Troubleshooting

### Camera not detected
```bash
# Check available video devices
ls -l /dev/video*

# Test camera with v4l2
v4l2-ctl --list-devices
```

### Permission denied error
```bash
# Check if you're in the video group
groups

# Add yourself to video group
sudo usermod -a -G video $USER
# Then logout and login again
```

### FreeType initialization failed
```bash
# Rebuild LVGL with FreeType support
./rebuild_lvgl.sh
```

### v4l2 input format not found
This error occurs when libavdevice-dev is not installed:
```bash
sudo apt-get install libavdevice-dev
make clean && make
```

### Button not clickable
Make sure you're using LVGL's SDL driver (already configured in main.c).

### No shutter sound
If you don't hear the shutter sound when capturing:
```bash
# Check if SDL audio is working
pactl info

# Make sure audio is not muted
amixer set Master unmute

# Test with a simple sound
speaker-test -t sine -f 1000 -l 1
```

## Development

### Adding New Features
- GUI modifications: Edit `gui.c`
- Camera settings: Edit `camera.c`
- Main loop changes: Edit `main.c`

### Changing Window Size
Update `WINDOW_WIDTH` and `WINDOW_HEIGHT` in `main.c`

### Changing Camera Resolution
Update `CAMERA_WIDTH` and `CAMERA_HEIGHT` in `camera.h`

## Korean Language Support

The application uses NanumGothicCoding fonts loaded via FreeType for Korean text rendering:
- Status messages in Korean
- UI labels in Korean (웹캠 캡처, 촬영, 준비, etc.)
- Font sizes: 12px, 14px, 16px, 20px, 24px

## License

This project uses LVGL which is licensed under the MIT license.

## Author

Built with LVGL v9.2, SDL2, and FreeType2.
