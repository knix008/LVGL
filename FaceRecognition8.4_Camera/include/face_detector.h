#ifndef FACE_DETECTOR_H
#define FACE_DETECTOR_H

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include "common.h"

class FaceDetector {
public:
    FaceDetector();
    ~FaceDetector();

    // Initialize the detector with cascade classifier
    bool initialize();

    // Detect faces in image
    std::vector<Face> detect_faces(const cv::Mat& image);

    // Extract face ROI from image
    static cv::Mat extract_face(const cv::Mat& image, const cv::Rect& face_bbox);

    // Draw detected faces on image (for visualization)
    static cv::Mat draw_faces(const cv::Mat& image, const std::vector<Face>& faces);

    // Set minimum face size for detection
    void set_min_face_size(int width, int height);

private:
    cv::CascadeClassifier face_cascade;
    int min_face_width;
    int min_face_height;

    // Load cascade classifier
    bool load_cascade();
};

#endif // FACE_DETECTOR_H
