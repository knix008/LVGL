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

    double confidence_threshold = 0.50;  // 50% threshold for reliable face recognition
    int min_face_size_for_recognition = 80;  // Minimum face size (width/height) for reliable recognition (>70% confidence)
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
    void set_min_face_size_for_recognition(int size) { min_face_size_for_recognition = size; }
    int get_min_face_size_for_recognition() const { return min_face_size_for_recognition; }
    bool is_face_size_sufficient(int width, int height) const;
    bool is_model_trained() const { return is_trained; }
    bool is_model_loaded() const;
    int get_num_people() const;

    // Embedding extraction and analysis
    std::vector<float> extract_embedding(const cv::Mat& face_image);
    double compare_embeddings(const std::vector<float>& emb1, const std::vector<float>& emb2);
    
    // Advanced recognition with top-k results
    std::vector<std::pair<std::string, double>> recognize_top_k(const cv::Mat& face_image, int k = 3);

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
