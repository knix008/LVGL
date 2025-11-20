#ifndef DEEP_FACE_RECOGNIZER_H
#define DEEP_FACE_RECOGNIZER_H

#include "model_loader.h"
#include "faiss_index.h"
#include "face_database.h"
#include <opencv2/opencv.hpp>
#include <map>
#include <string>
#include <vector>
#include <memory>

class DeepFaceRecognizer {
private:
    std::unique_ptr<ModelLoader> model_loader;
    std::unique_ptr<FAISSIndex> faiss_index;

    std::map<int, std::string> person_id_to_name;
    std::map<std::string, int> name_to_person_id;

    double confidence_threshold = 0.7;  // 70% similarity threshold
    FaceDatabase* db = nullptr;
    bool is_trained = false;
    std::string model_path;

public:
    DeepFaceRecognizer();
    ~DeepFaceRecognizer() = default;

    // Model and database setup
    bool load_model(const std::string& onnx_model_path);
    void set_database(FaceDatabase* database);

    // Training methods
    bool train_from_images(const std::string& dataset_path);
    bool train_from_database();
    bool train_from_embeddings(const std::vector<int>& person_ids,
                               const std::vector<std::vector<float>>& embeddings);
    bool retrain_model();
    bool add_training_data(const cv::Mat& face_image, int person_id);

    // Recognition methods
    int recognize(const cv::Mat& face_image, double& confidence);
    std::string recognize_with_name(const cv::Mat& face_image, double& confidence);

    // Label management
    int register_person(const std::string& name);
    bool set_label_name(int person_id, const std::string& name);
    std::string get_label_name(int person_id) const;
    int get_label_from_name(const std::string& name) const;
    void load_labels_from_database();

    // Configuration
    void set_confidence_threshold(double threshold);
    double get_confidence_threshold() const { return confidence_threshold; }
    bool is_model_trained() const { return is_trained; }
    int get_num_people() const;

    // Embedding extraction
    std::vector<float> extract_embedding(const cv::Mat& face_image);

    // Index management
    bool save_index(const std::string& filepath);
    bool load_index(const std::string& filepath);
    void clear();

private:
    // Helper methods
    cv::Mat preprocess_face(const cv::Mat& face_image);
    bool validate_face_image(const cv::Mat& image);
    std::vector<std::pair<int, std::vector<float>>>
        extract_embeddings_from_directory(const std::string& dataset_path);
};

#endif // DEEP_FACE_RECOGNIZER_H
