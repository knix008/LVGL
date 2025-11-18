#ifndef FACE_RECOGNIZER_H
#define FACE_RECOGNIZER_H

#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>
#include <string>
#include <vector>
#include <map>
#include "face_database.h"

class FaceRecognizer {
private:
    cv::Ptr<cv::face::LBPHFaceRecognizer> recognizer;
    std::map<int, std::string> label_to_name;
    int next_label = 0;
    bool is_trained = false;
    double confidence_threshold = 0.6;  // 60% similarity threshold
    FaceDatabase* db = nullptr;  // Database reference

    // Helper to serialize cv::Mat to bytes
    std::vector<unsigned char> serialize_mat(const cv::Mat& mat);
    cv::Mat deserialize_mat(const std::vector<unsigned char>& data, int rows, int cols, int type);

public:
    FaceRecognizer();
    ~FaceRecognizer() = default;

    // Set database reference
    void set_database(FaceDatabase* database);

    // Training
    bool train(const std::vector<cv::Mat>& images, const std::vector<int>& labels);
    bool add_training_data(const cv::Mat& image, int person_id);  // Add single image incrementally
    bool train_from_images(const std::string& dataset_path);  // Train from filesystem subdirectories
    bool train_from_database();  // Train from all embeddings in database
    bool retrain_model();  // Retrain with all data from database

    // Recognition - returns label ID, confidence is 0-1 where 1 is perfect match
    int recognize(const cv::Mat& face_image, double& confidence);
    std::string recognize_with_name(const cv::Mat& face_image, double& confidence);

    // Label management
    int register_person(const std::string& name);
    bool set_label_name(int label, const std::string& name);
    std::string get_label_name(int label) const;
    int get_label_from_name(const std::string& name) const;
    void load_labels_from_database();  // Load person labels from DB

    // Configuration
    void set_confidence_threshold(float threshold);
    float get_confidence_threshold() const;
    bool is_model_trained() const;
    int get_num_people() const;

    // Clear data
    void clear_training_data();
};

#endif // FACE_RECOGNIZER_H
