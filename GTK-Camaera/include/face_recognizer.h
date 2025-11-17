#ifndef FACE_RECOGNIZER_H
#define FACE_RECOGNIZER_H

#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <string>
#include <vector>
#include <map>

class FaceRecognizer {
private:
    cv::Ptr<cv::face::LBPHFaceRecognizer> recognizer;
    std::map<int, std::string> label_to_name;
    int next_label = 0;
    bool is_trained = false;
    double confidence_threshold = 50.0;  // Lower is more confident
    std::string model_path = "face_recognizer_model.yml";

public:
    FaceRecognizer();
    ~FaceRecognizer() = default;

    // Training
    bool train(const std::vector<cv::Mat>& images, const std::vector<int>& labels);
    bool add_training_data(const cv::Mat& image, const std::string& name);
    bool train_from_images(const std::string& dataset_path);

    // Recognition
    int recognize(const cv::Mat& face_image, double& confidence);
    std::string recognize_with_name(const cv::Mat& face_image, double& confidence);

    // Model management
    bool save_model(const std::string& path = "");
    bool load_model(const std::string& path = "");

    // Label management
    int register_person(const std::string& name);
    bool set_label_name(int label, const std::string& name);
    std::string get_label_name(int label) const;
    int get_label_from_name(const std::string& name) const;

    // Configuration
    void set_confidence_threshold(double threshold);
    double get_confidence_threshold() const;
    bool is_model_trained() const;
    int get_num_people() const;

    // Clear data
    void clear_training_data();
};

#endif // FACE_RECOGNIZER_H
