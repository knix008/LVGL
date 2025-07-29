# LVGL DRM Setup

This project has been configured to use LVGL with the Linux DRM (Direct Rendering Manager) display driver instead of SDL.

## What Changed

1. **Display Driver**: Switched from SDL to Linux DRM
2. **Input Driver**: Switched from SDL to evdev
3. **Configuration**: Updated `lv_conf.h` to enable DRM and evdev drivers
4. **Build System**: Updated CMakeLists.txt to link against libdrm and libevdev

## Prerequisites

You need to install the required libraries:

```bash
# Ubuntu/Debian
sudo apt-get install libdrm-dev libevdev-dev

# Fedora
sudo dnf install libdrm-devel libevdev-devel

# Arch Linux
sudo pacman -S libdrm libevdev
```

## Building

The build process remains the same:

```bash
cd Source
mkdir build
cd build
cmake ..
make
```

## Running

To run the application with DRM, you typically need to:

1. **Switch to a virtual terminal** (Ctrl+Alt+F1-F6)
2. **Stop the display manager** (if running):
   ```bash
   sudo systemctl stop gdm  # or lightdm, sddm, etc.
   ```
3. **Run the application**:
   ```bash
   sudo ./main
   ```

## Device Detection

The application now includes automatic device detection for input devices. It will:

1. Scan `/dev/input/` for available devices
2. Read device names from `/sys/class/input/`
3. Automatically match mouse and keyboard devices
4. Fall back to common device paths if auto-detection fails

## Manual Device Configuration

If auto-detection doesn't work, you can manually specify device paths by editing `main.c`:

```c
// Replace the auto-detection with manual paths
const char* mouse_device = "/dev/input/event2";  // Your mouse device
const char* keyboard_device = "/dev/input/event1";  // Your keyboard device
```

## Finding Your Device Paths

Use the provided script to find your device paths:

```bash
cd Source
./find_input_devices.sh
```

This will show you all available input devices and their information.

## Troubleshooting

### Permission Issues
If you get permission errors, make sure your user is in the appropriate groups:
```bash
sudo usermod -a -G video,input $USER
```

### No Display Output
- Make sure you're running on a virtual terminal (not in X11/Wayland)
- Check that `/dev/dri/card0` exists and is accessible
- Try different DRM device paths (`/dev/dri/card1`, etc.)

### Input Not Working
- Check device permissions: `ls -la /dev/input/event*`
- Verify device paths with the detection script
- Make sure you're not running in X11 (which captures input)

### Performance Issues
- DRM should provide better performance than SDL for embedded systems
- If you experience issues, you can enable GBM buffers by setting `LV_USE_LINUX_DRM_GBM_BUFFERS` to 1 in `lv_conf.h`

## Advantages of DRM

1. **Better Performance**: Direct hardware access without X11 overhead
2. **Lower Latency**: No window system delays
3. **Embedded Friendly**: Designed for embedded Linux systems
4. **Hardware Acceleration**: Can utilize GPU features
5. **Real-time Capable**: Suitable for real-time applications

## Switching Back to SDL

If you need to switch back to SDL for development/testing:

1. Set `LV_USE_SDL` to 1 in `lv_conf.h`
2. Set `LV_USE_LINUX_DRM` to 0 in `lv_conf.h`
3. Set `LV_USE_EVDEV` to 0 in `lv_conf.h`
4. Update CMakeLists.txt to use SDL2 instead of DRM/evdev
5. Update main.c to use SDL functions 