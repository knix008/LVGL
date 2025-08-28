# Project Setup Guide

## Quick Setup (Recommended)

After cloning or pulling the project, run the setup script:

```bash
cd Source
chmod +x setup_project.sh
./setup_project.sh
```

This will automatically build all required libraries and prepare the project for compilation.

**Note**: The LVGL source code (159MB) is included in this repository, so you don't need to download it separately.

## Manual Setup

If you prefer to build libraries individually:

### 1. Build All Libraries
```bash
cd Source
chmod +x build_all_libs.sh
./build_all_libs.sh all
```

### 2. Build Individual Libraries (if needed)
```bash
# Build specific libraries
./build_all_libs.sh freetype
./build_all_libs.sh sdl2
./build_all_libs.sh lvgl
./build_all_libs.sh openssl
./build_all_libs.sh mariadb
./build_all_libs.sh zlib
```

### 3. Build the Main Application
```bash
mkdir -p build
cd build
cmake ..
make
```

## Troubleshooting

### Check Project Setup

To verify if your project is properly set up, run:

```bash
cd Source
chmod +x check_setup.sh
./check_setup.sh
```

This will check for all required libraries and headers and tell you what's missing.

### Check LVGL Source Code

To verify that the LVGL source code is properly available, run:

```bash
cd Source
chmod +x check_lvgl_source.sh
./check_lvgl_source.sh
```

This will check if the LVGL source code (159MB) is properly included in the repository.

### Error: "../core/lv_obj.h" no such file or directory

This error occurs when LVGL headers are not available. **Solution**: Build the LVGL library first:

```bash
cd Source
./build_all_libs.sh lvgl
```

**Note**: The LVGL source code is included in the repository, so this error usually means the library hasn't been built yet.

### Error: Other missing headers

If you encounter missing header files for other libraries, build them:

```bash
cd Source
./build_all_libs.sh [library_name]
```

Available libraries: `freetype`, `sdl2`, `lvgl`, `openssl`, `mariadb`, `zlib`

## Dependencies

The project requires these system packages:
- `build-essential` (gcc, make, etc.)
- `cmake`
- `wget`
- `pkg-config`
- `libavformat-dev`, `libavcodec-dev`, `libavutil-dev`, `libswscale-dev`, `libswresample-dev` (FFmpeg)

Install on Ubuntu/Debian:
```bash
sudo apt update
sudo apt install build-essential cmake wget pkg-config libavformat-dev libavcodec-dev libavutil-dev libswscale-dev libswresample-dev
```

## Project Structure

After setup, the project will have:
- `lib/lib/` - Static libraries (.a files)
- `lib/include/` - Header files
- `build/` - Build directory for the main application

## Running the Application

After successful build:
```bash
cd Source/build
./main
```

Or use the run script:
```bash
cd Source
./run.sh
```
