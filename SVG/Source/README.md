# LVGL SVG Project

This project demonstrates how to build LVGL as a static library and use it in your application.

## Project Structure

```
Source/
├── CMakeLists.txt          # Main application CMakeLists.txt
├── lvgl/                   # LVGL source code
│   └── CMakeLists.txt      # LVGL library CMakeLists.txt
├── lib/                    # Pre-built libraries directory
│   └── liblvgl.a          # LVGL static library
├── src/                    # Application source code
│   └── main.c             # Main application file
├── build/                  # Main application build directory
├── lvgl_build/            # LVGL library build directory
├── build.sh               # Main build script
├── build_lvgl_lib.sh      # LVGL library build script
└── clean.sh               # Clean script
```

## Build Process

The project uses a two-stage build process:

1. **Build LVGL Library**: Compiles LVGL into a static library (`liblvgl.a`) and places it in the `lib/` directory
2. **Build Application**: Compiles the main application and links it with the pre-built LVGL library

## Build Scripts

### Main Build Script (`build.sh`)
Builds both the LVGL library and the main application:
```bash
./build.sh
```

### LVGL Library Build Script (`build_lvgl_lib.sh`)
Builds only the LVGL library:
```bash
./build_lvgl_lib.sh
```

### Clean Script (`clean.sh`)
Removes all build artifacts:
```bash
./clean.sh
```

## Manual Build Steps

If you prefer to build manually:

### Step 1: Build LVGL Library
```bash
mkdir -p lvgl_build
cd lvgl_build
cmake ../lvgl
make -j$(nproc)
cd ..
```

### Step 2: Build Main Application
```bash
mkdir -p build
cd build
cmake ..
make -j$(nproc)
cd ..
```

## Running the Application

After building, run the application from the build directory:
```bash
cd build
./lvgl_main
```

### SVG Visual Representations Demo

The application displays visual representations of SVG content using LVGL widgets. To run the SVG demo:

```bash
./run_svg_demo.sh
```

This will show visual representations of SVG files from the `assets/` directory in a window with:
- 6 SVG representations displayed in a 3x2 grid
- Each SVG in its own bordered container with white preview area
- Actual visual content rendered using LVGL widgets
- Interactive graphics that represent the SVG content
- 800x600 window size for better visibility

### SVG Visual Content

The application creates visual representations of each SVG file using LVGL widgets:
- **Circle**: Blue circular arc (360 degrees)
- **Rectangle**: Red rounded rectangle with black border
- **Triangle**: Green triangular arc (120 degrees, rotated)
- **Star**: Yellow star made of 5 overlapping arcs
- **Text**: Purple "Hello SVG!" text with gray border
- **Complex**: Multi-element design with background, small shapes, and text

Each representation accurately reflects the visual content of the corresponding SVG file.

To verify SVG file accessibility:
```bash
./verify_svg_files.sh
```

## Dependencies

- CMake 3.10 or higher
- SDL2 development libraries
- C compiler with C99 support

## Benefits of This Approach

1. **Faster Builds**: LVGL library is built once and reused
2. **Separation of Concerns**: Library and application builds are separate
3. **Easier Maintenance**: Library can be updated independently
4. **Reduced Build Time**: Subsequent builds only rebuild the application

## Notes

- The LVGL library includes SDL2 drivers for display and input
- The library is built with position-independent code (PIC) for better compatibility
- All build artifacts are placed in separate directories to keep the source tree clean
