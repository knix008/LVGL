# Face Recognition System - User Guide

This document explains the face recognition system integrated with the GTK Webcam Viewer application.

## Overview

The application now includes a complete face recognition system with the following capabilities:

- **Face Detection**: Detects faces in video using Haar Cascade classifiers
- **Face Recognition**: Identifies recognized people using LBPH (Local Binary Patterns Histograms)
- **Face Database**: SQLite3-based database for storing person information and face images
- **Real-time Processing**: Detects and recognizes faces in live video stream
- **Visual Feedback**: Draws bounding boxes and names on detected faces

## Architecture

### Components

1. **FaceDetector** (`face_detector.h/cpp`)
   - Uses OpenCV's Haar Cascade classifier
   - Detects face regions in frames
   - Returns Face objects with bounding box information
   - Configurable detection parameters

2. **FaceRecognizer** (`face_recognizer.h/cpp`)
   - Uses OpenCV's LBPH Face Recognizer
   - Trains on labeled face images
   - Recognizes faces and returns identity + confidence
   - Manages person-to-label mappings
   - Model persistence (save/load)

3. **FaceDatabase** (`face_database.h/cpp`)
   - SQLite3 database backend
   - Stores person information
   - Tracks face images per person
   - Provides statistical queries
   - Two main tables: `people` and `face_images`

4. **GTKApp Integration** (`gtk_app.h/cpp`)
   - Initializes all components
   - Processes video frames with face detection/recognition
   - Draws results on frames
   - Manages UI updates

## Data Structures

### Face Structure

```cpp
struct Face {
    cv::Rect bbox;              // Bounding box coordinates
    int id;                     // Face ID (-1 if unknown)
    std::string name;           // Person's name
    double confidence;          // Confidence level (0-100)
};
```

### PersonRecord Structure

```cpp
struct PersonRecord {
    int id;                     // Database ID
    std::string name;           // Person's name
    int face_count;             // Number of registered faces
    std::string created_at;     // Creation timestamp
    std::string updated_at;     // Last update timestamp
};
```

## Database Schema

### people table

| Column | Type | Notes |
|--------|------|-------|
| id | INTEGER | Primary Key, Auto-increment |
| name | TEXT | Unique person name |
| face_count | INTEGER | Number of registered faces |
| created_at | TEXT | Creation timestamp |
| updated_at | TEXT | Last update timestamp |

### face_images table

| Column | Type | Notes |
|--------|------|-------|
| id | INTEGER | Primary Key, Auto-increment |
| person_id | INTEGER | Foreign Key to people.id |
| image_path | TEXT | Path to face image file |
| created_at | TEXT | Timestamp |

## Getting Started

### 1. Initial Setup

The application automatically initializes the face recognition system on startup:

```cpp
// In GTKApp::init()
load_face_recognizer();
```

This will:
- Open/create SQLite database (`face_database.db`)
- Initialize database tables if needed
- Load Haar Cascade classifier
- Attempt to load existing trained model

### 2. Training the Model

To enable face recognition, you need to:

1. **Create a dataset directory** with person subdirectories:
```
dataset/
├── Person_A/
│   ├── face1.jpg
│   ├── face2.jpg
│   └── face3.jpg
├── Person_B/
│   ├── face1.jpg
│   └── face2.jpg
└── Person_C/
    ├── face1.jpg
    ├── face2.jpg
    └── face3.jpg
```

2. **Train the model**:
```cpp
FaceRecognizer recognizer;
recognizer.train_from_images("dataset/");
recognizer.save_model("face_recognizer_model.yml");
```

3. **The application will automatically load the saved model** on next startup

### 3. Using Face Recognition

Once a model is trained and loaded:

1. Start the application: `./gtk_webcam`
2. Click "Start Camera"
3. Detected faces will be shown with bounding boxes
4. Recognized people will display their names and confidence scores
5. Unknown faces show "Unknown"

## Configuration

### Face Detector Parameters

Modify in `face_detector.cpp` or expose via API:

```cpp
// Detection sensitivity
scale_factor = 1.1;          // How much image size is reduced (1.05-1.4)
min_neighbors = 4;           // How many neighbors required (3-6)
min_face_size = {30, 30};    // Minimum face size in pixels
max_face_size = {};          // Maximum face size (0 = unlimited)
```

- **Lower scale_factor**: More thorough but slower
- **Lower min_neighbors**: More detections, more false positives
- **Larger min_face_size**: Faster, but misses small faces

### Face Recognizer Parameters

```cpp
confidence_threshold = 50.0;  // Lower = more confident (0-100)
                              // Increase if missing known faces
                              // Decrease if false positives
```

### Database Location

```cpp
FaceDatabase db("custom_path/face_database.db");
```

## API Usage Examples

### Face Detection

```cpp
FaceDetector detector;
if (detector.initialize()) {
    std::vector<Face> faces = detector.detect_faces(frame);
    for (const auto& face : faces) {
        std::cout << "Face at: " << face.bbox << std::endl;
    }
}
```

### Face Recognition

```cpp
FaceRecognizer recognizer;
recognizer.load_model("face_recognizer_model.yml");

// Recognize a face
cv::Mat face_image = extract_face_region(frame, bbox);
double confidence = 0.0;
int label = recognizer.recognize(face_image, confidence);

if (label != -1) {
    std::string name = recognizer.get_label_name(label);
    std::cout << "Recognized: " << name << " (Confidence: " << confidence << "%)" << std::endl;
}
```

### Database Operations

```cpp
FaceDatabase db;
if (db.open() && db.initialize()) {
    // Add person
    db.add_person("Alice");

    // Get all people
    std::vector<PersonRecord> people;
    db.get_all_people(people);

    // Add face image
    db.add_face_image(1, "/path/to/face.jpg");

    // Get statistics
    std::cout << "Total people: " << db.get_num_people() << std::endl;
    std::cout << "Total faces: " << db.get_total_faces() << std::endl;

    db.close();
}
```

## Training Data Requirements

For best results:

1. **Quality Images**: Clear, frontal face images
2. **Variety**: Multiple angles, lighting conditions, expressions
3. **Quantity**: At least 10-20 images per person
4. **Size**: Minimum 100x100 pixels, recommended 200x200
5. **Format**: JPG, PNG, BMP supported

## Performance Considerations

- **Face Detection**: Real-time on modern hardware
- **Face Recognition**: Depends on model size and complexity
- **Database**: SQLite3 efficient for moderate datasets
- **Threading**: Camera capture in separate thread prevents UI blocking

### Optimization Tips

1. Reduce frame resolution if CPU-bound
2. Increase detection scale_factor for speed
3. Lower confidence threshold for better matching
4. Use grayscale images internally (faster)
5. Limit database to necessary records

## Troubleshooting

### Face Detection Not Working

- Ensure Haar Cascade XML is installed
- Check console output for cascade loading errors
- Verify camera working properly
- Adjust detection parameters

### Face Recognition Returns "Unknown"

- Model not trained (check for `face_recognizer_model.yml`)
- Confidence threshold too high
- Training data not representative
- Face quality/angle different from training

### Database Connection Issues

- Check database file permissions
- Ensure SQLite3 library installed
- Verify database file not corrupted
- Check disk space available

### Performance Issues

- Reduce camera resolution
- Increase face detection scale_factor
- Use smaller training set
- Profile with `perf` or similar tool

## Future Enhancements

Potential improvements:

- [ ] Deep Learning models (DNN, FaceNet)
- [ ] Multiple face cascade options
- [ ] Real-time model retraining
- [ ] Face alignment preprocessing
- [ ] Age/gender estimation
- [ ] Emotion detection
- [ ] Face verification confidence scores
- [ ] Web/network API interface
- [ ] GUI for database management
- [ ] Model export/import functionality

## File Locations

After running with face recognition:

```
./
├── gtk_webcam                    # Executable
├── face_recognizer_model.yml     # Trained model (created after training)
├── face_database.db              # SQLite database
└── dataset/                      # Training images (if creating custom dataset)
    ├── Person1/
    ├── Person2/
    └── ...
```

## Building Without Face Recognition

To build the basic webcam viewer without face recognition:

1. Remove the face recognition includes from `gtk_app.h`
2. Remove face detector initialization from `gtk_app.cpp`
3. Update Makefile to exclude face recognition sources
4. Recompile

## References

- OpenCV Face Recognition: https://docs.opencv.org/master/dd/d65/classcv_1_1face_1_1LBPHFaceRecognizer.html
- Haar Cascade: https://docs.opencv.org/master/d1/de5/classcv_1_1CascadeClassifier.html
- SQLite3 C API: https://www.sqlite.org/c3ref/intro.html

## License

This face recognition system is provided as part of the GTK Webcam Viewer application.
