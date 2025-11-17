# Quick Start Guide - GTK Webcam Viewer

## 1. Install Dependencies

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install -y build-essential cmake libgtk-3-dev libgdk-pixbuf2.0-dev libopencv-dev pkg-config
```

**Fedora/RHEL:**
```bash
sudo dnf install -y gcc-c++ cmake gtk3-devel gdk-pixbuf2-devel opencv-devel pkg-config
```

**Or use the automatic setup script:**
```bash
chmod +x setup.sh
./setup.sh
```

## 2. Build the Application

```bash
cd gtk-webcam
make
```

The executable will be created at `bin/gtk_webcam`

## 3. Run the Application

```bash
make run
```

Or directly:
```bash
./bin/gtk_webcam
```

## 4. Using the Application

1. **Start Camera**: Click the "Start Camera" button
2. **View Stream**: Live video will appear in the window
3. **Monitor Performance**: Check FPS in the top-right corner
4. **Stop Camera**: Click the "Stop Camera" button to pause streaming
5. **Close**: Close the window to exit

## Troubleshooting

### Camera Access Error
If you get permission errors:
```bash
sudo usermod -a -G video $USER
# Log out and log back in
```

### Dependencies Not Found
```bash
# Verify packages are installed
pkg-config --modversion gtk+-3.0
pkg-config --modversion gdk-pixbuf-2.0
pkg-config --modversion opencv4
```

### Build Fails
```bash
# Clean and rebuild
make clean
make
```

## Next Steps

- Check `README.md` for detailed documentation
- Modify camera resolution in `src/camera.cpp` line 32-34
- Adjust window size in `src/gtk_app.cpp` line 29
- Customize UI by editing `src/gtk_app.cpp`

## Features

✅ Live webcam streaming
✅ Camera on/off toggle
✅ Real-time FPS display
✅ Horizontal flip (mirror effect)
✅ Multithreaded capture for smooth playback
✅ Resizable window
✅ Clean, minimal UI
