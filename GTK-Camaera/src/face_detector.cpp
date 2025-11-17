#include "face_detector.h"
#include <iostream>

FaceDetector::FaceDetector() {}

bool FaceDetector::initialize() {
    // Try to load the default Haar Cascade classifier
    std::string cascade_path = cv::samples::findFile(
        "haarcascades/haarcascade_frontalface_default.xml"
    );

    if (cascade_path.empty()) {
        std::cerr << "Error: Could not find haarcascade_frontalface_default.xml" << std::endl;
        return false;
    }

    return load_cascade(cascade_path);
}

bool FaceDetector::load_cascade(const std::string& cascade_path) {
    if (!face_cascade.load(cascade_path)) {
        std::cerr << "Error: Failed to load cascade classifier from: " << cascade_path << std::endl;
        return false;
    }

    std::cout << "Face cascade loaded successfully from: " << cascade_path << std::endl;
    return true;
}

std::vector<Face> FaceDetector::detect_faces(const cv::Mat& frame) {
    std::vector<Face> faces;

    if (frame.empty()) {
        std::cerr << "Error: Input frame is empty" << std::endl;
        return faces;
    }

    if (!is_loaded()) {
        std::cerr << "Error: Face cascade not loaded" << std::endl;
        return faces;
    }

    try {
        // Convert to grayscale for detection
        cv::Mat gray;
        if (frame.channels() == 3) {
            cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);
        } else {
            gray = frame.clone();
        }

        // Enhance contrast
        cv::Mat enhanced;
        cv::equalizeHist(gray, enhanced);

        // Detect faces
        std::vector<cv::Rect> face_rects;
        face_cascade.detectMultiScale(
            enhanced,
            face_rects,
            scale_factor,
            min_neighbors,
            0,
            min_face_size,
            max_face_size
        );

        // Convert to Face objects
        for (size_t i = 0; i < face_rects.size(); ++i) {
            Face face;
            face.bbox = face_rects[i];
            face.id = -1;  // Unknown
            face.name = "Unknown";
            face.confidence = 0.0;
            faces.push_back(face);
        }

        return faces;
    } catch (const std::exception& e) {
        std::cerr << "Exception in detect_faces: " << e.what() << std::endl;
        return faces;
    }
}

std::vector<Face> FaceDetector::detect_faces_with_id(
    const cv::Mat& frame,
    const std::vector<int>& face_ids
) {
    std::vector<Face> faces = detect_faces(frame);

    // Assign IDs if provided
    for (size_t i = 0; i < faces.size() && i < face_ids.size(); ++i) {
        faces[i].id = face_ids[i];
    }

    return faces;
}

void FaceDetector::set_scale_factor(double scale) {
    if (scale > 1.0) {
        scale_factor = scale;
    }
}

void FaceDetector::set_min_neighbors(int neighbors) {
    if (neighbors > 0) {
        min_neighbors = neighbors;
    }
}

void FaceDetector::set_min_face_size(int width, int height) {
    if (width > 0 && height > 0) {
        min_face_size = cv::Size(width, height);
    }
}

void FaceDetector::set_max_face_size(int width, int height) {
    if (width > 0 && height > 0) {
        max_face_size = cv::Size(width, height);
    }
}

bool FaceDetector::is_loaded() const {
    return !face_cascade.empty();
}
