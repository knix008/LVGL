#ifndef FACE_RECOGNIZER_H
#define FACE_RECOGNIZER_H

#include <string>
#include <vector>
#include <memory>
#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include "common.h"

class FaceRecognizer {
public:
    FaceRecognizer();
    ~FaceRecognizer();

    // Initialize the recognizer
    bool initialize();

    // Train recognizer with new face data
    bool train_faces(const std::vector<cv::Mat>& faces, const std::vector<int>& labels);

    // Generate face embedding/feature vector
    std::vector<float> get_face_embedding(const cv::Mat& face_image);

    // Recognize face - returns person ID and confidence
    RecognitionResult recognize_face(const cv::Mat& face_image);

    // Calculate similarity between two face embeddings
    static float calculate_similarity(const std::vector<float>& embedding1,
                                      const std::vector<float>& embedding2);

    // Set confidence threshold for recognition
    void set_confidence_threshold(float threshold);

    // Load previously trained model
    bool load_model(const std::string& model_path);

    // Save trained model
    bool save_model(const std::string& model_path);

private:
    cv::Ptr<cv::face::FaceRecognizer> recognizer;
    float confidence_threshold;
    bool is_trained;
};

#endif // FACE_RECOGNIZER_H
