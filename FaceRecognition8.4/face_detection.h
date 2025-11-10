#ifndef FACE_DETECTION_H
#define FACE_DETECTION_H

#include <stdint.h>
#include <stdbool.h>

// Maximum number of faces to detect
#define MAX_FACES 10

// Structure to hold face detection results
typedef struct {
    float x;         // Bounding box x coordinate (normalized 0-1)
    float y;         // Bounding box y coordinate (normalized 0-1)
    float width;     // Bounding box width (normalized 0-1)
    float height;    // Bounding box height (normalized 0-1)
    float confidence; // Detection confidence (0-1)
} FaceBox;

typedef struct {
    FaceBox boxes[MAX_FACES];
    int count;       // Number of detected faces
} FaceDetectionResult;

/**
 * Initialize face detection model
 * @param model_path Path to the ONNX model file
 * @return true on success, false on failure
 */
bool face_detection_init(const char *model_path);

/**
 * Run face detection on an RGB24 image
 * @param rgb_data RGB24 image data (width * height * 3 bytes)
 * @param width Image width in pixels
 * @param height Image height in pixels
 * @param result Output detection results
 * @return true on success, false on failure
 */
bool face_detection_detect(const uint8_t *rgb_data, int width, int height, FaceDetectionResult *result);

/**
 * Clean up and release face detection resources
 */
void face_detection_cleanup(void);

/**
 * Check if face detection is initialized
 * @return true if initialized, false otherwise
 */
bool face_detection_is_initialized(void);

#endif // FACE_DETECTION_H
