#include "face_detector.h"
#include <iostream>
#include <opencv2/objdetect.hpp>

FaceDetector::FaceDetector()
    : min_face_width(30), min_face_height(30) {
}

FaceDetector::~FaceDetector() {
}

bool FaceDetector::initialize() {
    return load_cascade();
}

bool FaceDetector::load_cascade() {
    // Try multiple common cascade paths for OpenCV
    std::vector<std::string> cascade_paths = {
        "/usr/share/opencv4/haarcascades/haarcascade_frontalface_default.xml",
        "/usr/share/opencv/haarcascades/haarcascade_frontalface_default.xml",
        "/opt/local/share/opencv4/haarcascades/haarcascade_frontalface_default.xml",
        "./cascade_files/haarcascade_frontalface_default.xml"
    };

    for (const auto& path : cascade_paths) {
        if (face_cascade.load(path)) {
            std::cout << "Face cascade classifier loaded from: " << path << std::endl;
            return true;
        }
    }

    std::cerr << "Failed to load face cascade classifier" << std::endl;
    std::cerr << "Please ensure OpenCV cascade files are installed" << std::endl;
    return false;
}

std::vector<Face> FaceDetector::detect_faces(const cv::Mat& image) {
    std::vector<Face> detected_faces;

    if (image.empty()) {
        std::cerr << "Input image is empty" << std::endl;
        return detected_faces;
    }

    if (face_cascade.empty()) {
        std::cerr << "Face cascade classifier not loaded" << std::endl;
        return detected_faces;
    }

    // Convert to grayscale if necessary
    cv::Mat gray_image;
    if (image.channels() == 3) {
        cv::cvtColor(image, gray_image, cv::COLOR_RGB2GRAY);
    } else if (image.channels() == 4) {
        cv::cvtColor(image, gray_image, cv::COLOR_RGBA2GRAY);
    } else {
        gray_image = image.clone();
    }

    // Equalize histogram for better detection
    cv::equalizeHist(gray_image, gray_image);

    // Detect faces
    std::vector<cv::Rect> faces;
    face_cascade.detectMultiScale(
        gray_image,
        faces,
        1.1,                    // scale factor
        6,                      // min neighbors (increased from 4 to filter out false positives)
        0,                      // flags
        cv::Size(min_face_width, min_face_height),  // min face size
        cv::Size()              // max face size
    );

    // Merge overlapping detections to avoid counting the same face multiple times
    // Uses Intersection over Union (IoU) to group similar detections
    std::vector<cv::Rect> merged_faces;
    std::vector<bool> used(faces.size(), false);

    for (size_t i = 0; i < faces.size(); i++) {
        if (used[i]) continue;

        // Start a new group with this face
        std::vector<cv::Rect> group;
        group.push_back(faces[i]);
        used[i] = true;

        // Find all overlapping faces using IoU threshold
        bool found_more = true;
        while (found_more) {
            found_more = false;
            for (size_t j = i + 1; j < faces.size(); j++) {
                if (used[j]) continue;

                // Calculate IoU (Intersection over Union)
                cv::Rect intersection = faces[j] & group.back();
                double intersection_area = static_cast<double>(intersection.width * intersection.height);
                double union_area = static_cast<double>(faces[j].area() + group.back().area() - intersection_area);

                double iou = (union_area > 0) ? (intersection_area / union_area) : 0.0;

                // If IoU > 0.1 (10% overlap), consider them the same face
                if (iou > 0.1) {
                    group.push_back(faces[j]);
                    used[j] = true;
                    found_more = true;
                    break;  // Restart the search with new merged rect
                }
            }
        }

        // Merge all faces in the group using union of all rectangles
        cv::Rect merged = group[0];
        for (size_t k = 1; k < group.size(); k++) {
            merged = merged | group[k];
        }

        merged_faces.push_back(merged);
    }

    // Convert detected rectangles to Face structures
    for (const auto& rect : merged_faces) {
        Face face;
        face.bbox = rect;
        face.confidence = 0.8f; // Cascade classifier doesn't provide confidence, use fixed value
        detected_faces.push_back(face);
    }

    std::cout << "Detected " << detected_faces.size() << " faces in image (after merging overlaps)" << std::endl;

    return detected_faces;
}

cv::Mat FaceDetector::extract_face(const cv::Mat& image, const cv::Rect& face_bbox) {
    if (image.empty() || face_bbox.width <= 0 || face_bbox.height <= 0) {
        return cv::Mat();
    }

    // Ensure bounding box is within image bounds
    int x = std::max(0, face_bbox.x);
    int y = std::max(0, face_bbox.y);
    int width = std::min(face_bbox.width, image.cols - x);
    int height = std::min(face_bbox.height, image.rows - y);

    if (width <= 0 || height <= 0) {
        return cv::Mat();
    }

    cv::Rect safe_bbox(x, y, width, height);
    return image(safe_bbox).clone();
}

cv::Mat FaceDetector::draw_faces(const cv::Mat& image, const std::vector<Face>& faces) {
    cv::Mat result = image.clone();

    // Convert to BGR for display if RGB
    if (result.channels() == 3) {
        cv::cvtColor(result, result, cv::COLOR_RGB2BGR);
    }

    // Draw rectangles and confidence scores
    for (const auto& face : faces) {
        // Draw bounding box
        cv::rectangle(result, face.bbox, cv::Scalar(0, 255, 0), 2);

        // Draw confidence text
        std::string label = "Face (" + std::to_string(static_cast<int>(face.confidence * 100)) + "%)";
        int baseline = 0;
        cv::Size text_size = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseline);

        cv::rectangle(result,
                     cv::Point(face.bbox.x, face.bbox.y - text_size.height - 5),
                     cv::Point(face.bbox.x + text_size.width, face.bbox.y),
                     cv::Scalar(0, 255, 0),
                     cv::FILLED);

        cv::putText(result, label,
                   cv::Point(face.bbox.x, face.bbox.y - 5),
                   cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);

        // Draw person ID if recognized
        if (!face.person_id.empty()) {
            cv::putText(result, "ID: " + face.person_id,
                       cv::Point(face.bbox.x, face.bbox.y + face.bbox.height + 20),
                       cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1);
        }
    }

    return result;
}

void FaceDetector::set_min_face_size(int width, int height) {
    min_face_width = std::max(1, width);
    min_face_height = std::max(1, height);
}
