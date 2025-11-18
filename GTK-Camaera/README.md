# GTK Face Recognition Application

A real-time face detection and recognition application with SQLite3 database integration for person management. Built with GTK3 and OpenCV, featuring LBPH-based face recognition with confidence-based filtering.

## Features

- **Live Webcam Streaming**: Display real-time video from your webcam
- **Real-time Face Detection**: Haar Cascade-based face detection with minimal false positives
- **Face Recognition**: LBPH (Local Binary Patterns Histograms) face recognizer trained from images
- **Confidence Filtering**: Visual distinction between high-confidence (≥50%) and low-confidence (<50%) detections
  - **Green boxes**: Recognized faces with ≥50% confidence (shows person name and percentage)
  - **Yellow boxes**: Detected faces with <50% confidence (no label shown)
- **Person Registration**: Register people in SQLite3 database for face recognition
- **Training Management**: Train face recognizer from captured images in dataset directory
- **FPS Display**: Monitor frame rate in real-time
- **Status Information**: View current camera and recognition status
- **Multithreaded Capture**: Smooth video playback without UI blocking
- **Fixed Window Size**: Non-resizable 800x600 window with camera display area (640x480)

## Requirements

### System Dependencies

Install the required packages on your system:

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    libgtk-3-dev \
    libgdk-pixbuf2.0-dev \
    libopencv-dev \
    libopencv-contrib-dev \
    libsqlite3-dev \
    pkg-config
```

**Fedora/RHEL:**
```bash
sudo dnf install -y \
    gcc-c++ \
    cmake \
    gtk3-devel \
    gdk-pixbuf2-devel \
    opencv-devel \
    opencv-contrib-devel \
    sqlite-devel \
    pkg-config
```

**Arch Linux:**
```bash
sudo pacman -S \
    base-devel \
    cmake \
    gtk3 \
    gdk-pixbuf2 \
    opencv \
    sqlite \
    pkg-config
```

### Build Requirements
- C++17 compiler (GCC 7+, Clang 5+)
- OpenCV 4.0+ (with opencv_contrib for face module)
- GTK3
- GdkPixbuf2
- SQLite3 3.0+
- CMake 3.10+ (optional, can use Makefile)

## Building

### Option 1: Using Make (Recommended for simplicity)

```bash
cd gtk-webcam
make
```

### Option 2: Using CMake

```bash
cd gtk-webcam
mkdir build
cd build
cmake ..
make
```

### Build Targets

```bash
make              # Build the application
make run          # Build and run the application
make debug        # Build with debug symbols
make debug-run    # Run with GDB debugger
make clean        # Remove build artifacts
make help         # Show available targets
```

## Running

### From Build Directory

```bash
# Using Make
./bin/gtk_webcam

# Using CMake
./build/gtk_webcam
```

### Direct Run Command

```bash
make run
```

## Usage

### Face Recognition Workflow

#### Step 1: Start the Application
1. Launch the executable: `./gtk_webcam`
2. The application opens with a live webcam feed (if model is trained)
3. Status shows: "Camera started successfully" and face recognition status

#### Step 2: Register a Person
1. Enter the person's name in the text input field
2. Click the "Capture Photo" button to capture multiple face images
3. The application saves images to `dataset/[PersonID]/photo_[timestamp].jpg`
4. Register the person in the SQLite3 database for tracking
5. Repeat for additional people you want to recognize

#### Step 3: Train the Recognizer
1. Ensure you have captured photos in the `dataset/` directory (subdirectories like `dataset/1/`, `dataset/2/`, etc.)
2. Click the "Registering" button
3. The application:
   - Loads all images from dataset subdirectories
   - Preprocesses images (grayscale, 200×200 resize, histogram equalization)
   - Trains the LBPH face recognizer model
   - Shows training progress and status
4. After training completes, face recognition is enabled

#### Step 4: Recognize Faces
1. With the model trained, point the webcam at faces
2. The application detects and recognizes faces in real-time
3. **Display indicators**:
   - **Green bounding boxes** with name and percentage: High-confidence matches (≥50%)
   - **Yellow bounding boxes** without labels: Low-confidence detections (<50%)
4. Monitor FPS in the top-right corner

#### Step 5: View Camera Status
- **Status field** displays:
  - Camera status (started/stopped)
  - Number of people registered in database
  - Number of trained people in recognizer
  - Face recognition enabled/disabled status
  - Any error messages

## Architecture

### File Structure

```
gtk-webcam/
├── include/
│   ├── camera.h              # Camera capture interface
│   ├── gtk_app.h             # GTK application class
│   ├── face_detector.h       # Haar Cascade face detection
│   ├── face_recognizer.h     # LBPH face recognition model
│   └── face_database.h       # SQLite3 person/face database
├── src/
│   ├── main.cpp              # Application entry point
│   ├── camera.cpp            # Camera implementation
│   ├── gtk_app.cpp           # GTK UI & main application logic
│   ├── face_detector.cpp     # Face detection implementation
│   ├── face_recognizer.cpp   # LBPH recognition implementation
│   └── face_database.cpp     # Database operations
├── dataset/                  # Training images directory (structure: dataset/PersonID/)
├── face_database.db          # SQLite3 database (auto-created)
├── CMakeLists.txt            # CMake build configuration
├── Makefile                  # Make build configuration
└── README.md                 # This file
```

### Architecture Design: Filesystem + Database Model

The application separates training data source from metadata storage:

**Filesystem Role (dataset/ directory)**
- Stores training images in person subdirectories: `dataset/1/`, `dataset/2/`, etc.
- Images can be captured photos or manually placed images
- On "Registering" click, train_from_images("dataset") reads all images from subdirectories
- Advantages: Scalable, allows manual image addition, clear data organization

**Database Role (SQLite3)**
- Tracks person metadata: ID, name, registration timestamp
- Tracks captured image metadata: file path, timestamp per person
- Optional: Stores serialized face embeddings for future incremental learning
- Currently used for reference, not as training data source

**Training Flow**
1. User captures photos → Saved to `dataset/PersonID/` + DB records created
2. User clicks "Registering" → train_from_images("dataset") loads all images
3. LBPH model trained on all loaded images → In-memory model ready
4. Recognition uses trained model, not database lookups

### Key Classes

#### Camera Class
- Handles OpenCV video capture
- Background thread for frame capture
- Thread-safe frame queue
- Properties: resolution (640×480), FPS (30), active status

#### FaceDetector Class
- Haar Cascade classifier for face detection
- Configurable parameters:
  - **scale_factor**: 1.1 (detection pyramid scale)
  - **min_neighbors**: 8 (number of overlapping detections required; increased from 4 to reduce false positives)
  - **min_face_size**: 30×30 pixels
  - **max_face_size**: unlimited
- Returns Face struct with bbox, confidence, recognized name/ID

#### FaceRecognizer Class
- LBPH (Local Binary Patterns Histograms) face recognizer
- LBPH parameters: radius=1, neighbors=8, grid_x=8, grid_y=8 (2048-element embeddings)
- Training modes:
  - `train_from_images(dataset_path)`: Load images from filesystem subdirectories
  - `train_from_database()`: Load embeddings stored in database
  - `add_training_data()`: Incrementally add training data (for future enhancements)
- Recognition:
  - `recognize()`: Returns person_id and confidence (0-1 scale)
  - `recognize_with_name()`: Returns person name
  - Confidence calculation: `similarity = 1.0 / (1.0 + distance/100.0)`
  - Display threshold: ≥50% confidence shows name label

#### FaceDatabase Class
- SQLite3 database management with prepared statements (SQL injection prevention)
- Three main tables:
  - **people**: person_id (PK), name, face_count, created_at, updated_at
  - **face_images**: id (PK), person_id (FK), image_path, created_at
  - **embeddings**: id (PK), person_id (FK), image_path, embedding_data (BLOB), created_at
- Thread-safe database operations
- All SQL queries use parameter binding

#### GTKApp Class
- Main GTK application controller
- UI initialization and management
- Frame refresh timer (30ms = ~33 FPS)
- Face detection and recognition integration
- Status display updates
- Input field for person name registration
- Button handlers: Start/Stop Camera, Capture Photo, Registering (training)

## Troubleshooting

### Camera Not Found
- Check if your webcam is connected: `ls /dev/video*`
- Ensure you have permission to access the camera:
  ```bash
  sudo usermod -a -G video $USER
  # Log out and log back in for changes to take effect
  ```
- Try specifying a different camera ID (0, 1, 2, etc.) in `camera.cpp`

### Low FPS or Dropped Frames
- Close other applications using the camera
- Reduce resolution or frame rate (modify in `camera.cpp`)
- Check CPU usage with `top` or `htop`
- Face detection/recognition is CPU-intensive; lower-end systems may see reduced FPS

### GTK/GdkPixbuf Errors
- Ensure all GTK development libraries are installed
- Run `pkg-config --cflags gtk+-3.0` to verify GTK3 is installed

### OpenCV Not Found
- Verify OpenCV is installed: `pkg-config --modversion opencv4`
- If missing, install with your package manager
- **Important**: Must include opencv_contrib for face module (LBPH recognizer)
- For CMake, you may need to specify OpenCV path:
  ```bash
  cmake -DOpenCV_DIR=/path/to/opencv/build ..
  ```

### SQLite3 Not Found
- Verify SQLite3 is installed: `sqlite3 --version`
- Ubuntu/Debian: `sudo apt-get install libsqlite3-dev`
- Fedora: `sudo dnf install sqlite-devel`
- Arch: `sudo pacman -S sqlite`

### Face Recognition Not Working

#### Training Failed: "Error: No embeddings found in database"
- **Cause**: No images in `dataset/` directory or training directory structure incorrect
- **Solution**:
  1. Create subdirectories for each person: `dataset/1/`, `dataset/2/`, etc.
  2. Place training images in person subdirectories
  3. Click "Registering" button to train from filesystem images
  4. Check status output for training progress

#### Training Failed: "Error: No training images found in dataset"
- **Cause**: `dataset/` directory is empty or images not in correct subdirectories
- **Solution**:
  1. Use "Capture Photo" button to capture images (auto-creates correct structure)
  2. Or manually create `dataset/PersonID/` subdirectories and copy image files
  3. Ensure images are .jpg, .jpeg, .png, or .bmp format
  4. Each person should have at least 2-3 images for reliable training

#### No Rectangle Boxes Shown
- **Cause 1**: Face detection parameters too strict or faces not clearly visible
  - Solution: Improve lighting, ensure face is directly facing camera
- **Cause 2**: min_neighbors parameter set too high (currently 8 for low false positives)
  - Solution: Reduce min_neighbors in `face_detector.h` line 20 (trade-off: more false positives)
- **Cause 3**: min_face_size too large
  - Solution: Call `face_detector->set_min_face_size(20, 20)` for smaller faces

#### Green Boxes Show But No Name Labels
- **Cause**: Recognizer not trained yet or confidence below 50% threshold
- **Solution**:
  1. Verify training completed: Click "Registering" button
  2. Check status shows "Face recognition enabled"
  3. Ensure captured training photos match current lighting/angles
  4. Low confidence (yellow boxes) indicates model uncertainty

#### Recognizing Wrong Person
- **Cause 1**: Insufficient training images (needs 5-10 images per person)
  - Solution: Capture more images with various angles and lighting
- **Cause 2**: Similar facial features between people
  - Solution: Increase confidence_threshold in `face_recognizer.h` line 17
- **Cause 3**: Poor image preprocessing
  - Solution: Check that captured images are clear and well-lit

#### Too Many False Detections (Wrong Faces Detected)
- **Cause**: min_neighbors parameter too low (allows spurious detections)
- **Current Setting**: min_neighbors = 8 (conservative, reduces false positives)
- **Solution**: This issue should be minimal with current settings
  - If still occurring, verify min_neighbors is 8 in `face_detector.h`
  - Ensure good lighting conditions
  - May need to retrain with better quality images

### Database Errors

#### SQLite3 Database Locked
- **Cause**: Multiple processes accessing database simultaneously
- **Solution**: Close other applications using the database
- **Prevention**: Application uses proper transaction handling

#### Face Database Not Initializing
- **Cause**: Permission issues or corrupted database file
- **Solution**:
  1. Delete `face_database.db` (will be recreated)
  2. Ensure write permissions in application directory
  3. Verify SQLite3 is properly installed

## Configuration

### Modify Camera Parameters

Edit `src/camera.cpp` in the `open()` method to adjust:

```cpp
cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);   // Width
cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);  // Height
cap.set(cv::CAP_PROP_FPS, 30);            // Frame rate
cap.set(cv::CAP_PROP_BUFFERSIZE, 1);      // Minimize buffer for low-latency capture
```

### Face Detection Parameters

Edit `include/face_detector.h` to tune detection behavior:

```cpp
double scale_factor = 1.1;        // Detection pyramid scale (1.05-1.4)
                                  // Lower = more thorough but slower
                                  // Higher = faster but may miss some faces
int min_neighbors = 8;            // Overlapping detections required
                                  // Higher = fewer false positives
                                  // Lower = more detections (may include false positives)
cv::Size min_face_size{30, 30};   // Minimum face size to detect
cv::Size max_face_size{};         // Maximum face size (empty = unlimited)
```

**Recommended settings for different scenarios**:
- Conservative (fewer false positives): `scale_factor=1.1, min_neighbors=8`
- Balanced: `scale_factor=1.1, min_neighbors=5`
- Aggressive (more detections): `scale_factor=1.1, min_neighbors=3`

### Face Recognition Parameters

Edit `include/face_recognizer.h` to adjust recognition thresholds:

```cpp
double confidence_threshold = 0.6;  // 0.6 = 60% similarity required to display name
                                    // Lower = more permissive, may misidentify
                                    // Higher = more strict, may show "Unknown"
```

Edit `src/face_recognizer.cpp` in the `recognize()` method to adjust confidence calculation:

```cpp
double similarity = 1.0 / (1.0 + distance / 100.0);  // Convert LBPH distance to similarity
                                                      // Adjust denominator (100.0) to scale
```

### UI Parameters

Edit `src/gtk_app.cpp` in the `draw_faces_on_frame()` method:

```cpp
// Confidence threshold for showing name labels (percentage)
if (face.confidence > 50.0) {
    // Draw green box with name
    // Shows: "PersonName (confidence%)"
} else {
    // Draw yellow box without name
    // Low confidence detection only
}
```

### Window Properties

Edit `src/gtk_app.cpp` in the `init()` method:

```cpp
// Window size (currently fixed, not resizable)
gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

// To make resizable, change FALSE to TRUE:
gtk_window_set_resizable(GTK_WINDOW(window), FALSE);  // Change to TRUE for resizable

// Camera display area
gtk_widget_set_size_request(image_widget, 640, 480);  // Display resolution
```

### UI Refresh Rate

Edit `src/gtk_app.cpp` in the `init()` method:

```cpp
refresh_timer = g_timeout_add(30, on_refresh_timer, this);  // 30ms = ~33 FPS
                                                             // Lower = higher FPS (more CPU)
                                                             // Higher = lower FPS (less CPU)
```

### Image Preprocessing

Edit `src/face_recognizer.cpp` in the `add_training_data()` method to adjust preprocessing:

```cpp
// Image resize for training
cv::resize(preprocessed, preprocessed, cv::Size(200, 200));  // Change to any square size
                                                               // Larger = more detail but slower
                                                               // Smaller = faster but less detail

// Histogram equalization improves recognition in different lighting
cv::equalizeHist(preprocessed, preprocessed);  // Remove for raw intensity training
```

## Performance Tips

1. **Face Detection**:
   - Increase `scale_factor` (1.1 → 1.3) for faster detection, fewer faces detected
   - Increase `min_neighbors` reduces per-frame detections
   - Reduce `min_face_size` increases computation but finds smaller faces

2. **Threading**: Camera capture runs in separate thread, preventing UI freezing

3. **Image Resolution**:
   - Lower camera resolution = higher FPS but less detail
   - Training image size (200×200) is fixed; preprocessed on-the-fly

4. **Face Recognition**:
   - First run is slower due to LBPH model initialization
   - Subsequent frames use in-memory model (very fast)
   - More training images = slower recognition (must compare more face histograms)

5. **Database Operations**:
   - Person registration happens in background (minimal impact)
   - Face image metadata stored efficiently in SQLite3
   - Consider deleting old people/images if database grows large

6. **Frame Queue**: Limited frame queue prevents memory buildup in long sessions

7. **CPU Optimization**:
   - Disable face detection if only viewing video: comment out detect_faces() call
   - Run on system with spare CPU cores for best performance
   - Monitor with `top` or `htop` during heavy face detection/recognition

## Development

### Building in Debug Mode

```bash
make debug
make debug-run    # Run with GDB
```

### Adding Features

The modular design allows easy extension:
- Add more camera properties in `Camera` class
- Extend GTK UI with additional widgets in `GTKApp` class
- Implement image processing on captured frames

## Database Schema

The application uses SQLite3 with three main tables for person and face management:

### Table: `people`
Stores registered person information
```sql
CREATE TABLE people (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL UNIQUE,
    face_count INTEGER DEFAULT 0,
    created_at TEXT NOT NULL,
    updated_at TEXT NOT NULL
);
```
- **id**: Auto-incrementing unique identifier for each person
- **name**: Person's name (unique)
- **face_count**: Number of face images captured for this person
- **created_at**: Registration timestamp
- **updated_at**: Last update timestamp

### Table: `face_images`
Tracks captured face image files
```sql
CREATE TABLE face_images (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    person_id INTEGER NOT NULL,
    image_path TEXT NOT NULL,
    created_at TEXT NOT NULL,
    FOREIGN KEY(person_id) REFERENCES people(id)
);
```
- **id**: Auto-incrementing unique identifier
- **person_id**: Foreign key reference to person
- **image_path**: File path to captured image (e.g., "dataset/1/photo_1234567890.jpg")
- **created_at**: Capture timestamp

### Table: `embeddings`
Stores serialized face embeddings for future incremental learning
```sql
CREATE TABLE embeddings (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    person_id INTEGER NOT NULL,
    image_path TEXT NOT NULL,
    embedding_data BLOB NOT NULL,
    created_at TEXT NOT NULL,
    FOREIGN KEY(person_id) REFERENCES people(id)
);
```
- **id**: Auto-incrementing unique identifier
- **person_id**: Foreign key reference to person
- **image_path**: Associated image file path
- **embedding_data**: Serialized LBPH face histogram (2048 bytes per face)
- **created_at**: Embedding extraction timestamp

## Face Recognition Algorithm

### Face Detection: Haar Cascade Classifier

**Method**: OpenCV's CascadeClassifier with pre-trained face cascade XML
- **Algorithm**: AdaBoost with Haar-like features
- **Characteristics**: Fast, real-time capable, good for frontal faces

**Parameters** (configurable in `face_detector.h`):
- `scale_factor = 1.1`: Image pyramid scaling (detect faces at multiple scales)
- `min_neighbors = 8`: Number of overlapping detections required (8 = conservative, few false positives)
- `min_face_size = 30×30`: Minimum face size in pixels
- `max_face_size = unlimited`: No maximum size constraint

**Detection Process**:
1. Input frame preprocessed and scaled into pyramid
2. Cascade applied at each pyramid level
3. Overlapping detections combined (min_neighbors requirement)
4. Final bounding boxes returned as Face struct

### Face Recognition: LBPH (Local Binary Patterns Histograms)

**Method**: OpenCV's LBPHFaceRecognizer
- **Algorithm**: Histogram-based local pattern matching
- **Characteristics**: Fast training, real-time recognition, robust to illumination changes

**LBPH Parameters** (in `face_recognizer.h` constructor):
- `radius = 1`: Neighborhood radius for LBP calculation (1 = 8 neighbors)
- `neighbors = 8`: Number of neighbors in circular neighborhood
- `grid_x = 8`: Horizontal grid divisions (8×8 = 64 grid cells)
- `grid_y = 8`: Vertical grid divisions
- **Result**: 64 cells × 256 histogram bins = 16,384 features per embedding

**Recognition Process**:
1. Input face image preprocessed: grayscale → 200×200 resize → histogram equalization
2. LBP features extracted and divided into 8×8 grid
3. Histogram computed for each grid cell
4. Similarity computed against all training histograms
5. Distance metric: Chi-square distance (or similar)
6. Confidence calculation: `similarity = 1.0 / (1.0 + distance/100.0)`
7. Display threshold: ≥50% confidence shows name label

**Training Process** (`train_from_images()`):
1. Load all images from `dataset/PersonID/` subdirectories
2. For each image:
   - Convert to grayscale
   - Resize to 200×200
   - Apply histogram equalization
   - Extract LBPH features
3. Train single LBPH model with all preprocessed images
4. Model stored in memory (no file persistence)
5. During recognition, compare input face against all training histograms

**Advantages**:
- Fast training and recognition
- Robust to lighting variations (histogram equalization)
- Low memory footprint compared to deep learning
- Good accuracy for controlled environments

**Limitations**:
- Requires frontal/near-frontal faces
- Performance degrades with significant head rotation (>45°)
- Sensitive to different lighting conditions between training and recognition
- Limited to ~20-50 different people reliably
- Requires multiple images per person (5-10 recommended)

**Confidence Score Interpretation**:
- **>80%**: High confidence, reliable match
- **50-80%**: Medium confidence, likely correct match
- **30-50%**: Low confidence, possibly wrong person
- **<30%**: Very uncertain, likely unknown person

## Limitations

- **Single camera support** (can be extended for multi-camera by modifying Camera class)
- **Frontal face detection** (Haar Cascade works best with near-frontal faces; side profiles may not detect)
- **Limited person count** (LBPH reliably handles ~20-50 people; performance degrades with more)
- **No frame recording** (can be added via OpenCV VideoWriter)
- **No image effects/filters** (can integrate additional OpenCV processing)
- **In-memory model only** (model lost on application exit; not persisted to disk)
- **No incremental learning** (retraining requires all images; no online learning)

## License

This project is open source and available for personal and educational use.

## Support

For issues or questions:
1. Check the Troubleshooting section
2. Verify all dependencies are installed
3. Check console output for error messages
4. Review camera permissions

## Future Enhancements

### Face Recognition Improvements
- [ ] **Deep Learning Models**: Replace LBPH with CNN-based recognizer (e.g., FaceNet, VGGFace)
- [ ] **Model Persistence**: Save/load trained LBPH model to disk
- [ ] **Incremental Learning**: Add new training images without full retraining
- [ ] **Head Pose Estimation**: Handle rotated faces (3D face alignment)
- [ ] **Multi-face Tracking**: Track multiple faces across frames
- [ ] **Face Clustering**: Automatic grouping of similar faces
- [ ] **Liveness Detection**: Distinguish real faces from photos/videos
- [ ] **Masked Face Recognition**: Handle faces with masks/sunglasses

### Camera and Capture
- [ ] **Multiple Camera Support**: Switch between multiple webcams
- [ ] **Frame Recording**: Record video stream to MP4/AVI
- [ ] **Screenshot Capture**: Save individual frames
- [ ] **Resolution/FPS Adjustment**: GUI controls for camera parameters
- [ ] **Camera Calibration**: Lens distortion correction

### Image Processing
- [ ] **Basic Filters**: Grayscale, blur, edge detection
- [ ] **Image Enhancement**: Brightness/contrast adjustment
- [ ] **Face Alignment**: Automatic face rotation for better recognition
- [ ] **Preprocessing Options**: Selectable preprocessing pipelines

### User Interface
- [ ] **Configuration File**: Save/load settings between sessions
- [ ] **Settings Dialog**: GUI for tuning detection/recognition parameters
- [ ] **Face Database Browser**: UI to view/edit registered people
- [ ] **Recognition History**: Log recognized faces with timestamps
- [ ] **Real-time Statistics**: Display detection/recognition metrics

### Performance and Deployment
- [ ] **GPU Acceleration**: CUDA/OpenCL for face detection/recognition
- [ ] **Network Streaming**: Stream video to remote clients
- [ ] **Performance Monitoring**: Real-time CPU/memory/FPS display
- [ ] **Docker Container**: Containerized deployment
- [ ] **REST API**: Remote face recognition service
