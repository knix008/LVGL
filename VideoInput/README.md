# LVGL VideoInput Program

A comprehensive multimedia application built with LVGL (Light and Versatile Graphics Library) featuring video playback, webcam capture, OpenCV processing, Korean input methods, and encrypted database management.

## 🚀 Quick Start

### Using the run.sh Script (Recommended)
```bash
# Build and run the program (first time)
./run.sh

# Just run the program (if already built)
./run.sh run

# Check dependencies
./run.sh deps
```

### Manual Build
```bash
# Build the program
cd Source/build
make main

# Run the program
./main
```

## 📋 Features

- **🎥 Video Playback**: MP4, GIF, AVI support with FFmpeg
- **📷 Webcam Capture**: Real-time camera feed and controls
- **🤖 OpenCV Integration**: Image processing and computer vision
- **🇰🇷 Korean Input**: Multiple Korean input methods (ChunJiIn, QWERTY)
- **🔐 Database**: Encrypted SQLite with SQLCipher
- **📅 Calendar**: Date management and navigation
- **⏰ Clock**: Real-time digital clock display
- **🎹 Input Methods**: Numeric keypad and keyboard layouts
- **⚙️ Settings**: Configuration and image display

## 📖 Documentation

- **[Quick Start Guide](Source/QUICK_START.md)** - Get running in 5 minutes
- **[How to Use Guide](Source/HOW_TO_USE.md)** - Comprehensive user manual
- **[run.sh Script Guide](Source/RUN_SCRIPT_GUIDE.md)** - Complete automation script documentation

## 🛠️ System Requirements

- **OS**: Linux (Ubuntu 20.04+)
- **Dependencies**: SDL2, FreeType, FFmpeg, OpenCV (built locally), SQLCipher (built from source)
- **Hardware**: 4GB RAM, 2GB storage, camera (optional)

## 📁 Project Structure

```
VideoInput/
├── Source/                 # Main source code
│   ├── src/              # Source files
│   ├── include/          # Header files
│   ├── assets/           # Media files
│   ├── build/            # Build output
│   ├── opencv/           # OpenCV source
│   └── CMakeLists.txt    # Build configuration
├── README.md             # This file
└── Documentation/        # Additional docs
```

## 🎮 User Interface

The program features a tabbed interface with 12 functional tabs:

| Tab | Purpose | Features |
|-----|---------|----------|
| **DB** | Database | Encrypted data management |
| **Setting** | Configuration | Settings and images |
| **NUM** | Numbers | Numeric keypad |
| **KOR** | Korean Input | Korean text input |
| **CJI** | ChunJiIn | Korean input method |
| **QWERTY** | Keyboard | Full keyboard layout |
| **CAL** | Calendar | Date management |
| **Clock** | Time | Digital clock |
| **Video** | Video Playback | MP4/GIF playback |
| **Webcam** | Camera | Real-time camera |
| **OpenCV** | Computer Vision | Image processing |
| **Info** | Information | Program details |

## 🔧 Development

### Building from Source

```bash
# Install dependencies
sudo apt install -y build-essential cmake libsdl2-dev libfreetype6-dev libavformat-dev libavcodec-dev libswscale-dev libavutil-dev pkg-config

# Build OpenCV locally
cd Source
git clone --depth 1 --branch 4.8.0 https://github.com/opencv/opencv.git
mkdir opencv_build && cd opencv_build
cmake ../opencv -DBUILD_TESTS=OFF -DBUILD_EXAMPLES=OFF -DCMAKE_INSTALL_PREFIX=../opencv_install
make -j4 && make install
cd ..

# Build the program
mkdir build && cd build
cmake .. && make main
```

### Running Tests

```bash
# Run individual test executables
./test_chunjiin
./calendar_test
./clock_test
./video_test
./webcam_test
./svg_test
./qwerty_korean_test
```

## 🐛 Troubleshooting

### Common Issues

1. **Build Errors**: Check all dependencies are installed
2. **Camera Issues**: Verify camera permissions and availability
3. **Video Problems**: Ensure video files exist in assets directory
4. **Font Issues**: Check Korean font file exists

### Debug Information

The program provides detailed console output for debugging:
- Initialization status
- Error messages
- Performance information
- System compatibility

## 📝 License

[Specify your license here]

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## 📞 Support

- Check the [How to Use Guide](Source/HOW_TO_USE.md) for detailed instructions
- Review console output for error messages
- Verify system requirements and dependencies

---

**Version**: 1.0  
**Last Updated**: 2024  
**Compatibility**: Linux (Ubuntu 20.04+) 