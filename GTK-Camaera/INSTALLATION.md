# Installation and Setup Guide

Complete guide to install, build, and run the GTK Webcam Viewer with Face Recognition.

## System Requirements

### Minimum Requirements
- Linux operating system (Ubuntu, Debian, Fedora, Arch)
- 2GB RAM
- 500MB free disk space
- Webcam or video capture device

### Recommended Requirements
- Modern multi-core processor
- 4GB+ RAM
- 1GB free disk space
- USB webcam (1080p or higher)

## Dependency Installation

### Ubuntu/Debian

```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    git \
    libgtk-3-dev \
    libgdk-pixbuf2.0-dev \
    libopencv-dev \
    libsqlite3-dev \
    pkg-config
```

### Fedora/RHEL/CentOS

```bash
sudo dnf install -y \
    gcc-c++ \
    cmake \
    git \
    gtk3-devel \
    gdk-pixbuf2-devel \
    opencv-devel \
    sqlite-devel \
    pkg-config
```

### Arch Linux

```bash
sudo pacman -S \
    base-devel \
    cmake \
    git \
    gtk3 \
    gdk-pixbuf2 \
    opencv \
    sqlite \
    pkg-config
```

### Or Use Automated Setup

```bash
chmod +x setup.sh
./setup.sh
```

## Building the Application

### 1. Navigate to Project Directory

```bash
cd /home/shkwon/Projects/LVGL/GTK-Camaera
```

### 2. Build

```bash
make clean
make
```

Build output should show:
```
Build completed: gtk_webcam
```

### 3. Verify Build

```bash
ls -lh gtk_webcam
file gtk_webcam
```

Expected output:
```
-rwxrwxr-x 1 user user 153K ... gtk_webcam
gtk_webcam: ELF 64-bit LSB executable ...
```

## Running the Application

### Basic Run

```bash
./gtk_webcam
```

### Build and Run

```bash
make run
```

### Debug Mode

```bash
make debug
make debug-run    # Run with GDB debugger
```

## Camera Setup

### Check Camera Availability

```bash
# List video devices
ls -la /dev/video*

# Test with OpenCV
python3 << 'EOF'
import cv2
cap = cv2.VideoCapture(0)
if cap.isOpened():
    print("Camera available")
    cap.release()
else:
    print("Camera not found")
EOF
```

### Fix Camera Permissions

If you get permission errors:

```bash
# Add user to video group
sudo usermod -a -G video $USER

# Log out and log back in for changes to take effect
# Or use:
newgrp video
```

### Multiple Cameras

If you have multiple cameras:
- Camera 0: /dev/video0
- Camera 1: /dev/video1
- etc.

To use a specific camera, modify `camera.cpp` line 18:
```cpp
cap.open(1, cv::CAP_V4L2);  // Use /dev/video1
```

## Face Recognition Setup

### 1. Training Data Preparation

Create a dataset directory:

```bash
mkdir -p dataset

# Create folders for each person
mkdir -p dataset/Alice
mkdir -p dataset/Bob
mkdir -p dataset/Charlie
```

### 2. Add Face Images

Place face images in each folder:

```bash
# Copy or link images
cp /path/to/alice_photos/* dataset/Alice/
cp /path/to/bob_photos/* dataset/Bob/
cp /path/to/charlie_photos/* dataset/Charlie/
```

Image requirements:
- Format: JPG, PNG, or BMP
- Size: 100x100 pixels minimum (200x200 recommended)
- Quality: Clear, focused faces
- Quantity: 10-20 images per person

### 3. Create Training Script

Create `train_model.cpp` in project root (see TRAINING_GUIDE.md):

```bash
# Compile training script
g++ -std=c++17 -o train_model train_model.cpp \
    src/face_recognizer.cpp src/face_database.cpp \
    -Iinclude $(pkg-config --cflags --libs opencv4) -lsqlite3
```

### 4. Train Model

```bash
./train_model
```

This creates:
- `face_recognizer_model.yml` - Trained model
- `face_database.db` - Person database

### 5. Start Using Face Recognition

```bash
./gtk_webcam
```

The application will automatically load the trained model.

## Troubleshooting

### Build Issues

**Error: opencv4 not found**
```bash
# Install OpenCV development files
sudo apt-get install libopencv-dev

# Or verify installation
pkg-config --modversion opencv4
```

**Error: gtk+-3.0 not found**
```bash
# Install GTK3 development files
sudo apt-get install libgtk-3-dev

# Or verify installation
pkg-config --modversion gtk+-3.0
```

**Error: sqlite3 not found**
```bash
# Install SQLite3 development files
sudo apt-get install libsqlite3-dev

# Or verify installation
sqlite3 --version
```

### Runtime Issues

**Camera not detected**
```bash
# Check camera availability
v4l2-ctl --list-devices

# Check permissions
ls -la /dev/video0

# Add to video group
sudo usermod -a -G video $USER
```

**Face recognition slow**
- Close other applications
- Reduce camera resolution in `camera.cpp`
- Increase `scale_factor` in face detection
- Use smaller training dataset

**Face not recognized**
- Check model file exists: `face_recognizer_model.yml`
- Verify training images are in dataset
- Adjust `confidence_threshold` in code
- Add more training images

### Display Issues

**Black screen**
- Verify GTK3 is installed
- Check X11/Wayland is running
- Try: `DISPLAY=:0 ./gtk_webcam`

**Window not responding**
- Check CPU usage
- Reduce frame processing complexity
- Enable vsync in GTK settings

## Verification Checklist

- [ ] All dependencies installed
- [ ] Build successful (no errors)
- [ ] Executable created (153KB)
- [ ] Camera detected and working
- [ ] Can start camera in app
- [ ] Face detection working (bounding boxes)
- [ ] Training data prepared (if using recognition)
- [ ] Model trained (if using recognition)
- [ ] Face recognition showing names

## Environment Variables

```bash
# Set display if needed
export DISPLAY=:0

# Enable debug output
export GTK_DEBUG=all
export GST_DEBUG=3

# Run with valgrind for memory checking
valgrind ./gtk_webcam

# Run with strace for system call tracing
strace ./gtk_webcam
```

## Performance Tuning

### For Better FPS
```cpp
// In camera.cpp, increase resolution
cap.set(cv::CAP_PROP_FRAME_WIDTH, 320);
cap.set(cv::CAP_PROP_FRAME_HEIGHT, 240);

// In face_detector.cpp, increase sensitivity
scale_factor = 1.3;  // Faster but less accurate
min_neighbors = 3;   // More false positives but faster
```

### For Better Accuracy
```cpp
// In camera.cpp, use better resolution
cap.set(cv::CAP_PROP_FRAME_WIDTH, 1280);
cap.set(cv::CAP_PROP_FRAME_HEIGHT, 960);

// In face_detector.cpp, increase accuracy
scale_factor = 1.05; // Slower but more accurate
min_neighbors = 5;   // Fewer false positives
```

## File Locations

After building and running:

```
/home/shkwon/Projects/LVGL/GTK-Camaera/
├── gtk_webcam                  # Main executable
├── face_recognizer_model.yml   # Trained model (after training)
├── face_database.db            # Database (created at first run)
├── src/                        # Source code
├── include/                    # Header files
├── build/                      # Object files
├── dataset/                    # Training images (optional)
└── *.md                        # Documentation
```

## Uninstalling

To remove the application:

```bash
# Clean build artifacts
make clean

# Delete trained model and database
rm -f face_recognizer_model.yml face_database.db

# Delete application
rm gtk_webcam

# Clean all dataset
rm -rf dataset/
```

## Next Steps

1. Read [README.md](README.md) for feature overview
2. Read [FACE_RECOGNITION.md](FACE_RECOGNITION.md) for recognition details
3. Read [TRAINING_GUIDE.md](TRAINING_GUIDE.md) to train custom model
4. Read [QUICKSTART.md](QUICKSTART.md) for quick reference

## Support

For issues:

1. Check troubleshooting section above
2. Review error messages in console
3. Check [ERROR_HANDLING.md](ERROR_HANDLING.md)
4. Verify all dependencies installed
5. Try clean rebuild: `make clean && make`

## Additional Resources

- OpenCV Documentation: https://docs.opencv.org/
- GTK3 Documentation: https://developer.gnome.org/gtk3/
- SQLite3 Documentation: https://www.sqlite.org/docs.html

---

**Application Version**: 2.0 with Face Recognition
**Last Updated**: 2024
**Compatibility**: Linux 64-bit (x86_64)
