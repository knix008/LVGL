#ifndef FACE_DETECTOR_H
#define FACE_DETECTOR_H

#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <string>
#include <vector>

struct Face {
    cv::Rect bbox;              // Bounding box of the face
    int id;                     // Face ID (-1 if unknown)
    std::string name;           // Name of the person
    double confidence;          // Confidence level
};

class FaceDetector {
private:
    cv::CascadeClassifier face_cascade;
    double scale_factor = 1.1;
    int min_neighbors = 4;
    cv::Size min_face_size{30, 30};
    cv::Size max_face_size{};

public:
    FaceDetector();
    ~FaceDetector() = default;

    bool initialize();
    bool load_cascade(const std::string& cascade_path);

    std::vector<Face> detect_faces(const cv::Mat& frame);
    std::vector<Face> detect_faces_with_id(const cv::Mat& frame, const std::vector<int>& face_ids);

    void set_scale_factor(double scale);
    void set_min_neighbors(int neighbors);
    void set_min_face_size(int width, int height);
    void set_max_face_size(int width, int height);

    bool is_loaded() const;
};

#endif // FACE_DETECTOR_H
