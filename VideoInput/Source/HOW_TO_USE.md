# LVGL VideoInput Program - How to Use Guide

## 📋 Table of Contents
1. [Overview](#overview)
2. [System Requirements](#system-requirements)
3. [Installation](#installation)
4. [Building the Program](#building-the-program)
5. [Running the Program](#running-the-program)
6. [User Interface Guide](#user-interface-guide)
7. [Features and Functions](#features-and-functions)
8. [Troubleshooting](#troubleshooting)
9. [Advanced Usage](#advanced-usage)

## 🎯 Overview

The LVGL VideoInput program is a comprehensive multimedia application built with LVGL (Light and Versatile Graphics Library). It provides multiple tabs for different functionalities including database management, video playback, webcam capture, OpenCV processing, Korean input methods, and more.

## 💻 System Requirements

### Minimum Requirements
- **Operating System**: Linux (Ubuntu 20.04 or later recommended)
- **CPU**: x86_64 architecture
- **RAM**: 4GB minimum, 8GB recommended
- **Storage**: 2GB free space
- **Display**: 1024x768 minimum resolution

### Required Libraries
- **SDL2**: Graphics and input handling
- **FreeType**: Font rendering
- **FFmpeg**: Video/audio processing
- **OpenCV 4.8.0**: Computer vision (built locally)
- **SQLCipher**: Encrypted database (built from source)
- **LVGL**: GUI framework

## 🚀 Installation

### 1. Clone the Repository
```bash
git clone <repository-url>
cd VideoInput
```

### 2. Install Dependencies
```bash
# Update package list
sudo apt update

# Install required packages
sudo apt install -y \
    build-essential \
    cmake \
    git \
    libsdl2-dev \
    libfreetype6-dev \
    libavformat-dev \
    libavcodec-dev \
    libswscale-dev \
    libavutil-dev \
    pkg-config
```

### 3. Build OpenCV Locally
The program uses a local OpenCV build for better compatibility:
```bash
cd Source
git clone --depth 1 --branch 4.8.0 https://github.com/opencv/opencv.git
mkdir opencv_build
cd opencv_build
cmake ../opencv \
    -DBUILD_TESTS=OFF \
    -DBUILD_PERF_TESTS=OFF \
    -DBUILD_EXAMPLES=OFF \
    -DBUILD_opencv_apps=OFF \
    -DBUILD_opencv_java=OFF \
    -DBUILD_opencv_python=OFF \
    -DBUILD_opencv_python2=OFF \
    -DBUILD_opencv_python3=OFF \
    -DBUILD_opencv_js=OFF \
    -DBUILD_opencv_ts=OFF \
    -DBUILD_opencv_world=OFF \
    -DCMAKE_INSTALL_PREFIX=../opencv_install
make -j4
make install
cd ..
```

## 🔨 Building the Program

### 1. Navigate to Source Directory
```bash
cd Source
```

### 2. Create Build Directory
```bash
mkdir build
cd build
```

### 3. Configure and Build
```bash
cmake ..
make main
```

### 4. Verify Build
```bash
./main
```

## 🎮 Running the Program

### Basic Usage
```bash
cd Source/build
./main
```

### Program Startup
When you run the program, you'll see:
1. **Initialization messages** for FreeType, FFmpeg, webcam, and SQLCipher
2. **Database demonstration** with encrypted data
3. **Font loading** for Korean text support
4. **GUI window** with tabbed interface

### Exit the Program
- Press `Ctrl+C` in the terminal
- Or close the GUI window

## 🖥️ User Interface Guide

### Main Window
The program opens with a tabbed interface containing multiple functional tabs:

### 📊 Tab Overview

#### 1. **DB Tab** - Database Management
- **Purpose**: Encrypted SQLite database operations
- **Features**: 
  - View encrypted data
  - Database information display
  - SQLCipher encryption status

#### 2. **Setting Tab** - Configuration
- **Purpose**: Program settings and configuration
- **Features**:
  - Image display (JPG, GIF, SVG)
  - Settings controls
  - Visual indicators

#### 3. **NUM Tab** - Number Input
- **Purpose**: Numeric keypad interface
- **Features**:
  - Number input buttons
  - Calculator-like interface
  - Numeric data entry

#### 4. **KOR Tab** - Korean Input
- **Purpose**: Korean text input system
- **Features**:
  - Korean character input
  - Hangul composition
  - Korean font support

#### 5. **CJI Tab** - ChunJiIn Input
- **Purpose**: ChunJiIn Korean input method
- **Features**:
  - Korean character composition
  - Input method switching
  - Character combination

#### 6. **QWERTY Tab** - QWERTY Keyboard
- **Purpose**: Standard QWERTY keyboard layout
- **Features**:
  - Full keyboard layout
  - Korean/English switching
  - Text input interface

#### 7. **CAL Tab** - Calendar
- **Purpose**: Calendar and date management
- **Features**:
  - Monthly calendar view
  - Date navigation
  - Event management

#### 8. **Clock Tab** - Time Display
- **Purpose**: Real-time clock display
- **Features**:
  - Digital clock
  - Large time display
  - Time formatting

#### 9. **Video Tab** - Video Playback
- **Purpose**: Video file playback
- **Features**:
  - MP4 video playback
  - Video controls (play, pause, stop)
  - Multiple video file support
  - Webcam integration

#### 10. **Webcam Tab** - Camera Capture
- **Purpose**: Webcam capture and display
- **Features**:
  - Real-time camera feed
  - Camera controls
  - Video capture options

#### 11. **OpenCV Tab** - Computer Vision
- **Purpose**: OpenCV image processing
- **Features**:
  - Image processing demos
  - Camera integration
  - Video analysis
  - Computer vision functions

#### 12. **Info Tab** - Program Information
- **Purpose**: Program details and status
- **Features**:
  - Version information
  - System status
  - Logo display

## 🔧 Features and Functions

### Video Functionality
- **Supported Formats**: MP4, GIF, AVI
- **Codecs**: H.264, H.265, MPEG-4
- **Controls**: Play, Pause, Stop, Restart, Next
- **Auto-restart**: Videos loop automatically

### Webcam Support
- **Camera Detection**: Automatic camera detection
- **Real-time Feed**: Live video display
- **Controls**: Start/Stop capture
- **Integration**: Works with video tab

### OpenCV Integration
- **Image Processing**: Filters, edge detection, blur
- **Camera Access**: Direct camera control
- **Video Analysis**: Frame processing
- **Real-time Processing**: Live image analysis

### Database Features
- **Encryption**: SQLCipher encrypted database
- **Data Storage**: Secure data persistence
- **Query Support**: SQL operations
- **Multiple Tables**: Flexible data structure

### Korean Input Support
- **Multiple Methods**: ChunJiIn, QWERTY Korean
- **Font Support**: TrueType Korean fonts
- **Character Composition**: Hangul composition
- **Input Switching**: Method switching

## 🛠️ Troubleshooting

### Common Issues

#### 1. **Build Errors**
```bash
# Clean and rebuild
cd Source/build
make clean
cmake ..
make main
```

#### 2. **Missing Dependencies**
```bash
# Install missing packages
sudo apt install -y [package-name]
```

#### 3. **Camera Not Working**
- Check camera permissions
- Verify camera is not in use by other applications
- Test with `v4l2-ctl --list-devices`

#### 4. **Video Playback Issues**
- Ensure video files are in supported formats
- Check file paths in assets directory
- Verify FFmpeg installation

#### 5. **Font Display Problems**
- Check Korean font file exists: `../assets/NanumGothic-Regular.ttf`
- Verify FreeType installation
- Check font file permissions

### Debug Information
The program provides detailed console output for debugging:
- Initialization status
- Error messages
- Performance information
- System compatibility

## 🔬 Advanced Usage

### Custom Video Files
1. Place video files in `assets/` directory
2. Update video paths in `tab_video.c`
3. Rebuild the program

### Custom OpenCV Functions
1. Modify `tab_opencv.cpp`
2. Add new processing functions
3. Update UI controls

### Database Customization
1. Modify `sqlite_test.c`
2. Add new tables/queries
3. Update encryption settings

### Korean Input Customization
1. Modify input method files
2. Add new character mappings
3. Update font settings

### Performance Optimization
- Adjust LVGL memory settings in `lv_conf.h`
- Optimize video buffer sizes
- Configure OpenCV parameters

## 📁 File Structure

```
Source/
├── src/                    # Source files
│   ├── main.c             # Main program
│   ├── ui_components.c    # UI framework
│   ├── tab_*.c           # Tab implementations
│   └── *.c               # Feature modules
├── include/               # Header files
├── assets/               # Media files
├── build/                # Build output
├── opencv/               # OpenCV source
├── opencv_build/         # OpenCV build
├── opencv_install/       # OpenCV installation
└── CMakeLists.txt        # Build configuration
```

## 📞 Support

### Getting Help
1. Check console output for error messages
2. Verify all dependencies are installed
3. Ensure proper file permissions
4. Check system requirements

### Reporting Issues
- Include error messages
- Specify system configuration
- Describe steps to reproduce
- Provide relevant log output

---

**Version**: 1.0  
**Last Updated**: 2024  
**Compatibility**: Linux (Ubuntu 20.04+)  
**License**: [Specify your license] 