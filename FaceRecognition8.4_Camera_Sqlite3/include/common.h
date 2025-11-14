#ifndef COMMON_H
#define COMMON_H

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

// Face detection result structure
struct Face {
    cv::Rect bbox;           // Bounding box in image
    float confidence;        // Detection confidence (0-1)
    std::vector<float> embedding;  // Face embedding vector
    std::string person_id;   // Identified person ID
};

// Recognition result structure
struct RecognitionResult {
    std::string person_id;
    std::string person_name;
    float confidence;        // Match confidence (0-1)
    bool is_registered;
};

// Image data structure
struct ImageData {
    cv::Mat mat;
    std::string file_path;
    std::vector<Face> faces;
};

#endif // COMMON_H
