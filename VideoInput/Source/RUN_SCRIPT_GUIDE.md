# run.sh Script Guide - LVGL VideoInput Program

## 📋 Overview

The `run.sh` script is a comprehensive automation tool for building and running the LVGL VideoInput program. It handles dependency checking, OpenCV building, program compilation, and execution with proper error handling and user feedback.

## 🚀 Quick Start

### Basic Usage
```bash
# Build and run the program (recommended for first time)
./run.sh

# Just run the program (if already built)
./run.sh run

# Just build the program
./run.sh build
```

## 📖 Complete Usage Guide

### Command Options

| Command | Description | Use Case |
|---------|-------------|----------|
| `./run.sh` | Build and run (default) | First time setup or complete rebuild |
| `./run.sh build` | Build only | When you want to build without running |
| `./run.sh run` | Run only | When program is already built |
| `./run.sh clean` | Clean build files | Free up disk space or fix build issues |
| `./run.sh deps` | Check dependencies | Verify system requirements |
| `./run.sh opencv` | Build OpenCV only | When you only need to rebuild OpenCV |
| `./run.sh help` | Show help | Get usage information |

### Examples

```bash
# First time setup - builds everything and runs
./run.sh

# Check if your system has all required dependencies
./run.sh deps

# Build everything but don't run
./run.sh build

# Run the program (must be built first)
./run.sh run

# Clean all build files to start fresh
./run.sh clean

# Get help
./run.sh help
```

## 🔧 What the Script Does

### 1. **Dependency Checking** (`check_dependencies`)
- ✅ Checks for required commands: `cmake`, `make`, `git`, `pkg-config`
- ✅ Verifies system libraries: SDL2, FreeType, FFmpeg
- ✅ Notes that SQLCipher is built from source (no system package needed)
- ✅ Reports missing dependencies with installation commands
- ✅ Provides colored output for easy reading

### 2. **OpenCV Building** (`build_opencv`)
- ✅ Downloads OpenCV 4.8.0 from GitHub
- ✅ Configures with optimized settings (no tests, examples, etc.)
- ✅ Builds using all CPU cores for speed
- ✅ Installs to local directory (`Source/opencv_install`)
- ✅ Skips if already built

### 3. **Program Building** (`build_program`)
- ✅ Creates build directory structure
- ✅ Runs CMake configuration
- ✅ Compiles the main program
- ✅ Handles build errors gracefully

### 4. **Program Execution** (`run_program`)
- ✅ Verifies executable exists
- ✅ Launches the program
- ✅ Provides exit instructions
- ✅ Returns proper exit codes

## 🎨 Visual Features

### Colored Output
- 🟢 **Green**: Information messages
- 🟡 **Yellow**: Warnings
- 🔴 **Red**: Errors
- 🔵 **Blue**: Headers and section dividers

### Progress Indicators
- Clear status messages for each step
- Error reporting with specific details
- Success confirmation messages

## 📊 Script Flow

```
┌─────────────────┐
│   Start Script  │
└─────────┬───────┘
          │
          ▼
┌─────────────────┐
│ Check Dependencies│
└─────────┬───────┘
          │
          ▼
┌─────────────────┐
│  Build OpenCV   │
└─────────┬───────┘
          │
          ▼
┌─────────────────┐
│ Build Program   │
└─────────┬───────┘
          │
          ▼
┌─────────────────┐
│  Run Program    │
└─────────────────┘
```

## 🛠️ Advanced Usage

### Development Workflow

```bash
# 1. Check dependencies first
./run.sh deps

# 2. Build everything
./run.sh build

# 3. Run the program
./run.sh run

# 4. Make code changes...

# 5. Rebuild and run
./run.sh
```

### Troubleshooting Workflow

```bash
# 1. Clean everything
./run.sh clean

# 2. Check dependencies
./run.sh deps

# 3. Rebuild OpenCV if needed
./run.sh opencv

# 4. Build and run
./run.sh
```

### Continuous Development

```bash
# For quick iterations (after initial build)
./run.sh run

# For rebuilds after code changes
./run.sh build && ./run.sh run
```

## 🔍 Error Handling

### Common Error Scenarios

#### 1. **Missing Dependencies**
```bash
[ERROR] Missing dependencies:
  - libsdl2-dev
  - libfreetype6-dev

[WARNING] Please install missing dependencies:
sudo apt update && sudo apt install -y libsdl2-dev libfreetype6-dev
```

#### 2. **Build Failures**
```bash
[ERROR] CMake configuration failed.
[ERROR] Build failed.
```

#### 3. **OpenCV Build Issues**
```bash
[ERROR] OpenCV configuration failed.
[ERROR] OpenCV build failed.
```

### Recovery Actions

| Error Type | Action |
|------------|--------|
| Missing dependencies | Run `sudo apt install` with listed packages |
| CMake errors | Check CMakeLists.txt and dependencies |
| Build errors | Clean and rebuild: `./run.sh clean && ./run.sh` |
| OpenCV errors | Rebuild OpenCV: `./run.sh opencv` |

## 📁 File Structure Created

The script creates and manages these directories:

```
VideoInput/
├── run.sh                    # This script
├── Source/
│   ├── opencv/              # OpenCV source (cloned)
│   ├── opencv_build/        # OpenCV build files
│   ├── opencv_install/      # OpenCV installation
│   ├── build/               # Main program build
│   └── ...
└── ...
```

## ⚡ Performance Tips

### Fast Rebuilds
```bash
# After initial build, just run
./run.sh run

# For code changes, rebuild only
./run.sh build
```

### Parallel Building
- OpenCV builds using all CPU cores (`make -j$(nproc)`)
- Main program builds with parallel compilation
- Optimized for multi-core systems

### Disk Space Management
```bash
# Clean build files to save space
./run.sh clean

# Clean only main build (keep OpenCV)
rm -rf Source/build
```

## 🔧 Customization

### Environment Variables
You can customize the script behavior:

```bash
# Use fewer CPU cores for OpenCV build
export OPENCV_JOBS=2
./run.sh

# Set custom OpenCV version
export OPENCV_VERSION=4.7.0
./run.sh
```

### Script Modifications
The script is well-commented and modular. Key sections:

- **Dependencies**: Lines 50-90
- **OpenCV Build**: Lines 92-150
- **Program Build**: Lines 152-180
- **Main Logic**: Lines 250-290

## 📝 Logging and Debugging

### Verbose Output
The script provides detailed output for debugging:

```bash
[INFO] Checking system dependencies...
[INFO] All dependencies are installed.
[INFO] Building OpenCV locally...
[INFO] OpenCV built and installed successfully.
[INFO] Building LVGL VideoInput program...
[INFO] Program built successfully.
[INFO] Starting LVGL VideoInput program...
```

### Error Codes
- `0`: Success
- `1`: Error (dependency, build, or runtime)

## 🎯 Best Practices

### For New Users
1. **First time**: `./run.sh` (builds everything and runs)
2. **Check dependencies**: `./run.sh deps` if you have issues
3. **Clean start**: `./run.sh clean && ./run.sh` if builds fail

### For Developers
1. **Quick testing**: `./run.sh run` (after initial build)
2. **Rebuild after changes**: `./run.sh build`
3. **Full rebuild**: `./run.sh clean && ./run.sh`

### For System Administrators
1. **Dependency check**: `./run.sh deps`
2. **Install missing packages** as suggested
3. **Verify build**: `./run.sh build`

## 🔗 Related Documentation

- **[Quick Start Guide](QUICK_START.md)** - 5-minute setup
- **[How to Use Guide](HOW_TO_USE.md)** - Complete user manual
- **[README.md](../../README.md)** - Project overview

## 📞 Support

### Getting Help
1. Run `./run.sh help` for usage information
2. Check error messages for specific issues
3. Verify dependencies with `./run.sh deps`
4. Clean and rebuild with `./run.sh clean && ./run.sh`

### Common Issues
- **Permission denied**: `chmod +x run.sh`
- **Dependencies missing**: Follow the installation commands shown
- **Build failures**: Clean and rebuild
- **OpenCV issues**: Rebuild OpenCV specifically

---

**Script Version**: 1.0  
**Last Updated**: 2024  
**Compatibility**: Linux (Ubuntu 20.04+) 