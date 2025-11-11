# Face Recognition Application with LVGL GUI

A modern C++ face recognition application that uses OpenCV for image processing and LVGL for the graphical user interface.

## Features

- **Image Input**: Load JPG, PNG, GIF, BMP, and TIFF images
- **Face Detection**: Detect faces in images using OpenCV's Haar Cascade classifier
- **Face Recognition**: Recognize and identify faces using LBPH (Local Binary Patterns Histograms)
- **Face Registration**: Register new persons with their facial images
- **LVGL GUI**: User-friendly graphical interface using LVGL v9.2
- **Face Database**: Persistent storage of registered persons and their embeddings

## Project Structure

```
FaceRecognition8.4_Image/
├── assets/                    # Font files (Korean support)
├── dataset/                      # Runtime data directory
│   └── face_database/         # Registered faces and embeddings
├── include/                   # Header files
│   ├── common.h              # Common data structures
│   ├── image_loader.h        # Image loading functionality
│   ├── face_detector.h       # Face detection
│   ├── face_recognizer.h     # Face recognition
│   ├── face_database.h       # Face database management
│   └── gui.h                 # LVGL GUI
├── src/                       # Implementation files
│   ├── main.cpp              # Application entry point
│   ├── image_loader.cpp      # Image loading implementation
│   ├── face_detector.cpp     # Face detection implementation
│   ├── face_recognizer.cpp   # Face recognition implementation
│   ├── face_database.cpp     # Database implementation
│   └── gui.cpp               # LVGL GUI implementation
├── lvgl/                     # LVGL library (v9.2)
├── lv_conf.h                 # LVGL configuration
├── Makefile                  # Build configuration
├── setup.sh                  # Automated setup script
└── README.md                 # This file
```

## Requirements

### System Dependencies

- Linux system (x86_64 or ARM-based)
- GCC/G++ compiler (C++17 support)
- OpenCV 4.x development libraries
- SDL2 development libraries
- FreeType2 development libraries
- libjpeg development libraries
- pthreads (standard library)

### Installation

```bash
# Run setup script to install dependencies and build LVGL
./setup.sh

# The setup.sh script will:
# 1. Check system dependencies
# 2. Install missing packages (if using apt/dnf)
# 3. Download and build LVGL library
# 4. Create necessary directories
```

## Building

After running `setup.sh`:

```bash
# Build the application
make

# Build and run
make run

# Clean build artifacts
make clean

# Deep clean (removes LVGL build)
make distclean
```

## Usage

### GUI Controls

The application provides four main buttons:

1. **Load Image** - Opens file browser to select an image
2. **Detect Faces** - Detects all faces in the loaded image
3. **Register** - Registers a detected face for a person
4. **Recognize** - Identifies a person from a detected face

### Workflow

1. Click "Load Image" to load a JPG, PNG, or other supported image format
2. Click "Detect Faces" to find all faces in the image
3. For registration:
   - Load an image with one face
   - Click "Detect Faces"
   - Click "Register" and enter the person's ID and name
4. For recognition:
   - Load an image with one face
   - Click "Detect Faces"
   - Click "Recognize" to identify the person

## Module Documentation

### ImageLoader (image_loader.h/cpp)

Handles image input and processing:
- Loads images in JPG, PNG, GIF, BMP, TIFF formats
- Converts color spaces (BGR to RGB)
- Resizes images while maintaining aspect ratio
- Validates image formats

### FaceDetector (face_detector.h/cpp)

Face detection using OpenCV Haar Cascade:
- Detects faces in images
- Extracts individual face regions
- Draws detection results for visualization
- Configurable minimum face size

### FaceRecognizer (face_recognizer.h/cpp)

Face recognition using LBPH algorithm:
- Generates face embeddings (feature vectors)
- Trains recognition model with face samples
- Recognizes faces with confidence scores
- Calculates similarity between embeddings
- Loads/saves trained models

### FaceDatabase (face_database.h/cpp)

Manages persistent face data:
- Registers new persons with faces and embeddings
- Stores face images and embeddings
- Maintains person registry (ID, name)
- Loads/saves database to disk
- Retrieves registered persons

### GUI (gui.h/cpp)

LVGL-based graphical interface:
- Displays images and detection results
- Button controls for application functions
- Status messages and dialogs
- File browser integration (extensible)
- Event-based callback system

## Data Storage

### Database Structure

```
dataset/
├── person_list.csv           # Registry of all persons
└── person_id_1/
    ├── face_*.png            # Face images
    └── embeddings.txt        # Face embeddings (space-separated)
```

### Person List Format (CSV)

```
person_id,person_name
person_1,John Doe
person_2,Jane Smith
```

### Embeddings Format

Each line contains space-separated float values representing a face embedding vector.

## Configuration

### LVGL Configuration (lv_conf.h)

- Display: 320x640 pixels (portrait mode)
- Color depth: 32-bit RGB888
- Memory pool: 256KB (optimized for embedded systems)
- Display driver: SDL2 (for desktop testing and development)
- Input devices: SDL2 keyboard/mouse
- Font support: FreeType2 with TrueType fonts
- Canvas: 300x180 pixels (for image display)

### Memory Management

The application uses LVGL's pool-based memory management system:
- All allocations come from a single 256KB memory pool
- When LVGL deinitialization occurs, the entire pool is deallocated at once
- No manual freeing of individual allocations is done after `lv_deinit()`
- This ensures clean shutdown without memory corruption

### Face Detection Parameters (face_detector.cpp)

- Scale factor: 1.1
- Minimum neighbors: 4
- Minimum face size: 30x30 pixels

### Face Recognition Parameters (face_recognizer.cpp)

- LBPH radius: 1
- LBPH neighbors: 8
- Grid size: 8x8
- Confidence threshold: 0.6 (configurable)

## Extending the Application

### Adding New Recognition Algorithms

Create a new recognizer class inheriting the recognition interface:

```cpp
class MyRecognizer : public FaceRecognizer {
    // Implement get_face_embedding() and recognize_face()
};
```

### Customizing the GUI

Modify `gui.cpp` to add more buttons, dialogs, or visualization features.

### Using Different Face Detection

Replace Haar Cascade with:
- DNN-based detection (OpenCV dnn module)
- YOLO face detection
- MediaPipe face detection

## Recent Fixes and Improvements

### Version 8.4 - Memory Management and Stability Fixes

#### Canvas Buffer Memory Management
- **Issue**: Canvas buffer allocation failures after multiple image loads
- **Root Cause**: Each `display_image()` call allocated a new buffer without properly freeing old ones, exhausting LVGL's 256KB memory pool
- **Solution**: Implemented proper memory lifecycle management using LVGL's pool-based allocation system
- **Changes**:
  - Track canvas buffer pointer for consistency
  - Allocate new buffers only when needed
  - Rely on LVGL's `lv_deinit()` for complete cleanup instead of manual freeing

#### Graceful Application Shutdown
- **Issue**: Core dump when closing the application
- **Root Cause**: Attempting to free memory after LVGL's memory pool was deallocated by `lv_deinit()`
- **Solution**: Removed manual memory deallocation in destructor
- **Changes**:
  - Destructor no longer calls `lv_free()` on canvas buffer
  - LVGL's deinitialization handles all pool cleanup automatically
  - Proper null pointer management

#### Dialog Box Implementation
- **Issue**: Modal dialogs (error/success messages) could not be dismissed
- **Root Cause**: Incorrect LVGL v9.2 API usage for message boxes
- **Solution**: Implemented custom dialog using LVGL widgets
- **Features**:
  - Title label at top
  - Message text with word wrapping
  - OK button for dismissal
  - Proper event handling with `lv_obj_del()`

#### Button Positioning
- **Issue**: Buttons positioned too low on screen, overlapping with labels
- **Solution**: Repositioned buttons to `screen_height - 140` instead of `screen_height - 110`
- **Result**: Better UI layout with no overlapping elements

#### Image Resizing
- **Issue**: Large images (1024x775) caused memory allocation failures
- **Solution**: Implemented aggressive image resizing before canvas display
- **Details**:
  - Scale down to canvas size (300x180 maximum)
  - Maintain aspect ratio
  - Minimum size enforcement (10x10 pixels)
  - High-quality INTER_LINEAR interpolation

### Bug Fixes Summary
1. ✅ Fixed filename parsing crash in registration (missing dot in filename)
2. ✅ Fixed registration freezing during face database processing
3. ✅ Fixed popup dialogs being non-dismissible
4. ✅ Fixed canvas buffer allocation failure on repeated operations
5. ✅ Fixed core dump on application close

## Troubleshooting

### Build Errors

1. **Missing OpenCV**: Install with `apt-get install libopencv-dev`
2. **Missing SDL2**: Install with `apt-get install libsdl2-dev`
3. **Cascade not found**: The haarcascade file should be in standard OpenCV paths

### Runtime Issues

1. **No faces detected**: Ensure faces are clearly visible and well-lit
2. **Recognition fails**: Register more face samples for better training
3. **LVGL display issues**: Check SDL2 configuration and display driver

### Memory-Related Issues

1. **"Failed to allocate canvas buffer"**
   - **Cause**: LVGL memory pool exhausted (256KB limit)
   - **Solution**: Images are automatically resized to 300x180 before display
   - **Prevention**: Keep image dimensions reasonable; the application automatically downscales

2. **Core dump on application close**
   - **Cause**: Manual memory deallocation after LVGL cleanup
   - **Status**: ✅ Fixed in v8.4 - LVGL now handles all cleanup automatically

3. **Application hangs during registration**
   - **Cause**: Blocking GUI during face processing
   - **Status**: ✅ Fixed in v8.4 - Registration now completes properly without freezing

### Performance Optimization Tips

- For large images: Use images smaller than 1024x768 to minimize scaling overhead
- For better recognition: Register 3-5 face samples per person for optimal training
- For real-time use: Consider reducing face detection scale factor from 1.1 to 1.05 for precision

## Performance Notes

- Face detection: ~100-500ms depending on image size
- Face embedding generation: ~50-200ms
- Face recognition: ~10-50ms
- Database operations: Depends on number of registered persons

## License

This project uses open-source libraries:
- LVGL (MIT License)
- OpenCV (Apache License 2.0)
- SDL2 (Zlib License)
- FreeType (Dual License)

## Future Enhancements

- [ ] Support for camera input (real-time face recognition)
- [ ] Advanced recognition algorithms (FaceNet, VGGFace2)
- [ ] Batch processing of multiple images
- [ ] Face alignment and preprocessing
- [ ] Confidence threshold adjustment
- [ ] Database export/import
- [ ] Multi-face recognition in single image
- [ ] Performance optimization (GPU acceleration)

## Contributing

Contributions are welcome! Please follow the existing code style and add tests for new features.

## Support

For issues and questions, please refer to the project repository or contact the development team.
