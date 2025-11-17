# Face Recognition Model Training Guide

This guide explains how to train and use a face recognition model with the GTK Webcam Viewer.

## Quick Start

### Step 1: Create Training Dataset

Create a directory structure with person names as folders:

```bash
mkdir -p dataset
mkdir -p dataset/Alice
mkdir -p dataset/Bob
mkdir -p dataset/Charlie
```

### Step 2: Add Face Images

Place face images in each person's folder:

```
dataset/
├── Alice/
│   ├── face1.jpg
│   ├── face2.jpg
│   ├── face3.png
│   ├── face4.jpg
│   └── face5.jpg
├── Bob/
│   ├── face1.jpg
│   ├── face2.jpg
│   ├── face3.jpg
│   ├── face4.jpg
│   └── face5.jpg
└── Charlie/
    ├── face1.jpg
    ├── face2.jpg
    ├── face3.jpg
    ├── face4.jpg
    └── face5.jpg
```

### Step 3: Write Training Script

Create a file `train_model.cpp`:

```cpp
#include "face_recognizer.h"
#include "face_database.h"
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

int main() {
    try {
        std::cout << "Starting face recognition model training..." << std::endl;

        // Initialize database
        FaceDatabase db("face_database.db");
        if (!db.open() || !db.initialize()) {
            std::cerr << "Failed to initialize database" << std::endl;
            return 1;
        }

        // Initialize recognizer
        FaceRecognizer recognizer;

        // Train from dataset
        std::cout << "Training from dataset..." << std::endl;
        if (!recognizer.train_from_images("dataset")) {
            std::cerr << "Failed to train model" << std::endl;
            db.close();
            return 1;
        }

        // Save model
        std::cout << "Saving model..." << std::endl;
        if (!recognizer.save_model("face_recognizer_model.yml")) {
            std::cerr << "Failed to save model" << std::endl;
            db.close();
            return 1;
        }

        // Store in database
        for (int i = 0; i < recognizer.get_num_people(); ++i) {
            std::string name = recognizer.get_label_name(i);
            db.add_person(name);
        }

        std::cout << "Training completed successfully!" << std::endl;
        std::cout << "Number of people: " << recognizer.get_num_people() << std::endl;

        db.close();
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
}
```

### Step 4: Compile and Run

```bash
# Compile with necessary libraries
g++ -std=c++17 -o train_model train_model.cpp src/face_recognizer.cpp src/face_database.cpp \
    -Iinclude $(pkg-config --cflags --libs opencv4) -lsqlite3

# Run training
./train_model
```

## Using Pre-Collected Faces

### Option 1: Real-time Capture

You can modify the application to capture and save training faces:

```cpp
// In gtk_app.cpp, add during live stream:
if (detected_faces && key_pressed == 's') {
    // Save face image
    std::string filename = "dataset/PersonName/face_" + timestamp + ".jpg";
    cv::imwrite(filename, frame(detected_face.bbox));
}
```

### Option 2: Extract from Video

```bash
# Extract frames from a video file
ffmpeg -i video.mp4 -vf "fps=1" dataset/PersonName/frame_%04d.jpg

# Or extract every 5th frame
ffmpeg -i video.mp4 -vf "fps=0.2" dataset/PersonName/frame_%04d.jpg
```

### Option 3: Use Webcam to Capture

Create a capture utility:

```cpp
// capture_faces.cpp
#include <opencv2/opencv.hpp>
#include <iostream>
#include <filesystem>

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <person_name>" << std::endl;
        return 1;
    }

    std::string person_name = argv[1];
    std::string output_dir = "dataset/" + person_name;

    // Create directory
    std::filesystem::create_directories(output_dir);

    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Cannot open camera" << std::endl;
        return 1;
    }

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);

    int count = 0;
    cv::Mat frame;

    std::cout << "Capturing faces for: " << person_name << std::endl;
    std::cout << "Press 'c' to capture, 'q' to quit" << std::endl;

    while (true) {
        cap >> frame;
        if (frame.empty()) break;

        cv::imshow("Capture Faces - Press C to capture, Q to quit", frame);

        int key = cv::waitKey(1);
        if (key == 'q' || key == 'Q') break;
        if (key == 'c' || key == 'C') {
            std::string filename = output_dir + "/face_" + std::to_string(count) + ".jpg";
            cv::imwrite(filename, frame);
            std::cout << "Captured: " << filename << std::endl;
            count++;
        }
    }

    cap.release();
    cv::destroyAllWindows();

    std::cout << "Captured " << count << " faces" << std::endl;
    return 0;
}
```

Compile and use:
```bash
g++ -std=c++17 -o capture_faces capture_faces.cpp $(pkg-config --cflags --libs opencv4)
./capture_faces Alice
./capture_faces Bob
```

## Best Practices

### Image Quality

1. **Lighting**: Good, consistent lighting without harsh shadows
2. **Focus**: Clear, sharp face images
3. **Resolution**: At least 100x100 pixels (200x200+ recommended)
4. **Format**: JPG or PNG (lossless preferred for critical apps)

### Capture Variety

1. **Angles**: Multiple head rotations (±30 degrees)
2. **Expressions**: Neutral, smile, serious
3. **Lighting**: Front-lit, side-lit, different brightness levels
4. **Distance**: Different distances from camera
5. **Occlusions**: With/without glasses, hats, etc.

### Dataset Organization

```
dataset/
├── Alice/          # 15-20 images
│   ├── front_001.jpg
│   ├── angle_001.jpg
│   ├── smile_001.jpg
│   └── ...
├── Bob/           # 15-20 images
│   └── ...
└── Charlie/       # 15-20 images
    └── ...
```

Recommendations:
- **Minimum**: 10 images per person
- **Good**: 15-20 images per person
- **Excellent**: 30+ images per person with variety

### Common Mistakes to Avoid

1. ❌ Too few training images (<5 per person)
2. ❌ Poor quality images (blurry, dark, extreme angles)
3. ❌ Inconsistent naming (inconsistent person names)
4. ❌ Mixed image formats without normalization
5. ❌ Missing preprocessing (very large images)

## Model Evaluation

### Check Training Results

```cpp
// test_model.cpp
#include "face_recognizer.h"
#include "face_detector.h"
#include <opencv2/opencv.hpp>

int main() {
    FaceRecognizer recognizer;
    FaceDetector detector;

    // Load trained model
    if (!recognizer.load_model("face_recognizer_model.yml")) {
        std::cerr << "Failed to load model" << std::endl;
        return 1;
    }

    if (!detector.initialize()) {
        std::cerr << "Failed to initialize detector" << std::endl;
        return 1;
    }

    // Test on a single image
    cv::Mat test_image = cv::imread("dataset/Alice/face_1.jpg");

    std::vector<Face> faces = detector.detect_faces(test_image);
    for (const auto& face : faces) {
        cv::Mat face_roi = test_image(face.bbox);
        double confidence = 0.0;
        int label = recognizer.recognize(face_roi, confidence);

        std::cout << "Recognition: " << recognizer.get_label_name(label)
                  << " (Confidence: " << confidence << ")" << std::endl;
    }

    return 0;
}
```

## Re-training

To add more people or improve existing models:

1. Add new person folders to `dataset/`
2. Add more images to existing person folders
3. Delete old `face_recognizer_model.yml`
4. Run training script again

```bash
# Remove old model
rm face_recognizer_model.yml

# Run training
./train_model
```

## Performance Tuning

### For Better Accuracy

1. **Increase training data**: More images per person
2. **Improve image quality**: Better lighting, focus
3. **Adjust confidence threshold**: Lower for strict matching
4. **Preprocess images**: Normalize size, brightness

### For Better Speed

1. **Reduce training data**: Use representative subset
2. **Increase face detector scale_factor**: Faster but less accurate
3. **Limit recognizer's label set**: Only load relevant people
4. **Use grayscale**: Faster processing

## Troubleshooting

### Training Fails

```
Error: "Images and labels size mismatch"
```
- Check all person folders have at least 1 image
- Verify image files are readable
- Check for corrupted image files

```
Error: "All images must have the same size"
```
- The code automatically resizes to 200x200
- Check for extremely small images (<50x50)
- Verify no corrupted image data

### Recognition Poor

- **Too many false positives**: Increase confidence_threshold
- **Missing known faces**: Decrease confidence_threshold, add more training data
- **All faces recognized as same person**: More diverse training data needed
- **Recognition very slow**: Check number of training images

### Database Issues

```
Error: "Cannot open database"
```
- Check write permissions in current directory
- Delete corrupted `face_database.db`
- Ensure SQLite3 installed

## Next Steps

After training a model:

1. Run the webcam application: `./gtk_webcam`
2. Start camera stream
3. Detected faces show names and confidence
4. Refine training data based on recognition results

See [FACE_RECOGNITION.md](FACE_RECOGNITION.md) for usage details.
