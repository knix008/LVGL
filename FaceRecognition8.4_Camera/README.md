# Face Recognition Application with LVGL GUI

A modern C++ face recognition application that uses OpenCV for image processing and LVGL for the graphical user interface.

## Features

- **Image Input**: Load JPG, PNG, GIF, BMP, and TIFF images from file or capture from webcam
- **Webcam Capture**: Real-time video input with configurable camera device selection
- **Face Detection**: Detect faces in images using OpenCV's Haar Cascade classifier
- **Face Recognition**: Recognize and identify faces using LBPH (Local Binary Patterns Histograms)
- **Face Registration**: Register new persons with their facial images and embeddings
- **LVGL GUI**: User-friendly graphical interface using LVGL v9.2
- **Face Database**: SQLite3-based persistent storage of registered persons and their embeddings
- **Korean Language Support**: Full Korean localization for UI, labels, and messages

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
- SQLite3 development libraries
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

### Image File Naming Convention for Registration

For the automatic batch registration to work correctly, image files must follow this naming pattern:

```
PersonName.sequence.jpg
```

**Examples:**
- `A.1.1.jpg` → Registers as person "A"
- `B.2.2.jpg` → Registers as person "B"
- `John.1.jpg` → Registers as person "John"
- `Jane.sample1.jpg` → Registers as person "Jane"

**Format Rules:**
- **PersonName**: The identifier for the person (what will be displayed during recognition)
- **First dot (.)**:  Separates the person name from the sequence
- **Sequence**: Any suffix after the first dot (typically used for numbering like 1.1, 2.2, etc.)
- **File extension**: Supported formats are .jpg, .jpeg, .png, .bmp, .gif, .tiff

**Important:**
- Each image file MUST contain exactly ONE face
- Only images with exactly one detected face will be registered
- The person name is extracted from everything BEFORE the first dot

### Workflow

1. **Registration (Batch):**
   - Place image files in `./dataset/` directory with correct naming (PersonName.*.jpg)
   - Each file should contain exactly one face
   - Click "Register" button to process all images
   - The system will automatically create person directories and store face embeddings

2. **Recognition:**
   - Click "Load Image" to load a test image from the current directory
   - Click "Detect Faces" to find faces in the image
   - Click "Recognize" to identify the person (must have exactly one face)

3. **Individual Recognition Workflow:**
   - Load an image with one face
   - Click "Detect Faces" to find the face
   - Click "Recognize" to identify the person from registered database

## Module Documentation

### CameraCapture (camera_capture.h/cpp)

Handles real-time webcam input:
- Detects and initializes available camera devices (/dev/video*)
- Configurable camera device selection via config file or command-line
- Background capture thread for continuous frame acquisition
- Thread-safe frame buffer access via mutex protection
- Support for custom resolution and frame rate settings
- Automatic error recovery with configurable retry limits

### ImageLoader (image_loader.h/cpp)

Handles image input and processing:
- Loads images in JPG, PNG, GIF, BMP, TIFF formats
- Supports both file-based and webcam capture input
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

Manages persistent face data using SQLite3:
- Registers new persons with faces and embeddings in SQLite3 database
- Stores face images as PNG files in filesystem
- Stores face embeddings and metadata in SQLite3 tables
- Maintains person registry with automatic timestamps
- Loads/saves database from SQLite3 file (`dataset/faces.db`)
- Retrieves registered persons and their embeddings via SQL queries
- Supports foreign key relationships between persons and embeddings

### GUI (gui.h/cpp)

LVGL-based graphical interface:
- Displays images and detection results
- Button controls for application functions
- Status messages and dialogs
- File browser integration (extensible)
- Event-based callback system

## Data Storage

### Database Structure

The application uses SQLite3 for structured data storage with the following schema:

```
dataset/
├── faces.db                  # SQLite3 database file
└── person_id_1/
    ├── face_*.png            # Face images (stored in filesystem)
```

### Database Schema

#### Persons Table
```sql
CREATE TABLE persons (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    person_id TEXT UNIQUE NOT NULL,
    person_name TEXT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

#### Embeddings Table
```sql
CREATE TABLE embeddings (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    person_id TEXT NOT NULL,
    embedding_vector TEXT NOT NULL,  -- Space-separated float values
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (person_id) REFERENCES persons(person_id)
);
```

#### Faces Table
```sql
CREATE TABLE faces (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    person_id TEXT NOT NULL,
    image_path TEXT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (person_id) REFERENCES persons(person_id)
);
```

### Storage Format Details

- **Person Data**: Stored in SQLite3 `persons` table with unique person IDs
- **Face Embeddings**: Stored in `embeddings` table as space-separated float vectors
- **Face Images**: PNG images saved to filesystem directories (`dataset/person_id/`)
- **Metadata**: Automatic timestamps for tracking registration dates

## Configuration

### Camera Configuration (camera_config.txt)

Create a `camera_config.txt` file in the project root to configure camera settings:

```ini
# CAMERA DEVICE CONFIGURATION
camera_index = -1              # -1 for auto-detection, or specific device (0, 1, 2, etc.)
camera_min_index = 0           # Minimum camera device to scan
camera_max_index = 15          # Maximum camera device to scan

# CAMERA RESOLUTION
camera_width = 640
camera_height = 480

# CAMERA FRAME RATE (FPS)
camera_fps = 30

# ENABLE/DISABLE AUTO-LOAD
auto_load = 0                  # 1 to auto-load test image, 0 for manual
```

**Command-line overrides:**
```bash
./face_recognition -c 2                    # Use camera 2
./face_recognition --camera-range 0 10    # Scan cameras 0-10
```

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
- **Issue**: Canvas buffer allocation failures after multiple image loads + incorrect image colors
- **Root Cause**:
  1. Each `display_image()` call allocated a new buffer without reuse, exhausting LVGL's 256KB memory pool
  2. Incorrect color format (3-byte RGB) for LVGL 8.4 which expects 4-byte XRGB with 32-bit color depth
  3. Image loader converts BGR→RGB for storage, but LVGL XRGB8888 requires BGR byte order in memory
- **Solution**: Implemented smart buffer reuse strategy + correct RGB→BGR conversion + proper channel order
- **Changes**:
  - Added `canvas_buffer_size` member variable to track allocated buffer size
  - Only allocate new buffer if image size exceeds current buffer capacity
  - Reuse existing buffer for images smaller than or equal to current capacity
  - Convert RGB (from image_loader) back to BGR for LVGL XRGB8888 format
  - Add alpha channel while maintaining BGR order
  - Use `LV_COLOR_FORMAT_XRGB8888` color format constant (4 bytes per pixel)
  - Proper channel merge order: B, G, R, A (matching LVGL XRGB8888 byte layout)
  - Rely on LVGL's `lv_deinit()` for cleanup of all allocated buffers
  - Result: Correct color display + significant memory pool savings with only one allocation per unique max size

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

#### Face Detection False Positives
- **Issue**: Single-face images (e.g., A.1.1.jpg) detected as multiple faces
- **Root Cause**: Haar Cascade with `minNeighbors=4` too lenient + overlapping detections from same face at different scales
- **Solution**: Increased strictness and implemented improved IoU-based clustering for detection merging
- **Changes**:
  - Increased `minNeighbors` from 4 to 6 in Haar Cascade detection
  - Replaced simple overlap check with Intersection over Union (IoU) clustering
  - Groups detections with IoU > 0.1 (10% overlap) into clusters
  - Uses iterative grouping to handle transitive overlaps (A overlaps B, B overlaps C → all grouped together)
  - Merges each cluster into single detection using union of all rectangles

#### Face Recognition Not Working
- **Issue**: Recognizer always returned false with "Recognizer has not been trained yet" message
- **Root Cause**: Recognition callback loaded registered person IDs but never loaded their face images or trained the recognizer before attempting recognition
- **Solution**: Load registered face images from database and train recognizer before recognition
- **Changes**:
  - Scan each registered person's directory for stored face images (face_*.png)
  - Load and preprocess all face images: convert BGR→RGB, resize to 200x200, convert to grayscale
  - Train LBPH recognizer with all loaded face images and proper labels
  - Map label indices back to person IDs for result interpretation
  - Call `recognize_face()` only after successful training
- **Result**: Recognition now works correctly, identifying registered persons from test images

### Bug Fixes Summary
1. ✅ Fixed filename parsing crash in registration (missing dot in filename)
2. ✅ Fixed registration freezing during face database processing
3. ✅ Fixed popup dialogs being non-dismissible
4. ✅ Fixed canvas buffer allocation failure on repeated operations
5. ✅ Fixed core dump on application close
6. ✅ Fixed multiple faces detected in single-face images (false positives)
7. ✅ Fixed face recognition not working (recognizer not being trained)

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

- [x] Support for camera input (real-time face recognition) - ✅ Completed
- [x] SQLite3 database backend - ✅ Completed
- [x] Korean language support - ✅ Completed
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
