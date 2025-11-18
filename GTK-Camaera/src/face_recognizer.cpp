#include "face_recognizer.h"
#include "face_database.h"
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <cmath>

namespace fs = std::filesystem;

FaceRecognizer::FaceRecognizer() {
    // Create LBPH face recognizer with parameters: radius=1, neighbors=8, grid_x=8, grid_y=8
    recognizer = cv::face::LBPHFaceRecognizer::create(1, 8, 8, 8);
}

void FaceRecognizer::set_database(FaceDatabase* database) {
    db = database;
}

std::vector<unsigned char> FaceRecognizer::serialize_mat(const cv::Mat& mat) {
    std::vector<unsigned char> data;
    
    // Store matrix metadata: rows, cols, type
    int rows = mat.rows;
    int cols = mat.cols;
    int type = mat.type();
    
    data.resize(sizeof(int) * 3 + mat.total() * mat.elemSize());
    
    unsigned char* ptr = data.data();
    std::memcpy(ptr, &rows, sizeof(int));
    ptr += sizeof(int);
    std::memcpy(ptr, &cols, sizeof(int));
    ptr += sizeof(int);
    std::memcpy(ptr, &type, sizeof(int));
    ptr += sizeof(int);
    std::memcpy(ptr, mat.data, mat.total() * mat.elemSize());
    
    return data;
}

cv::Mat FaceRecognizer::deserialize_mat(const std::vector<unsigned char>& data, int rows, int cols, int type) {
    cv::Mat mat(rows, cols, type);
    
    const unsigned char* ptr = data.data() + sizeof(int) * 3;
    std::memcpy(mat.data, ptr, mat.total() * mat.elemSize());
    
    return mat;
}

void FaceRecognizer::load_labels_from_database() {
    if (!db) return;
    
    label_to_name.clear();
    std::vector<PersonRecord> people;
    
    if (db->get_all_people(people)) {
        for (const auto& person : people) {
            label_to_name[person.id] = person.name;
        }
        std::cout << "Loaded " << people.size() << " people from database" << std::endl;
    }
}

bool FaceRecognizer::train(const std::vector<cv::Mat>& images, const std::vector<int>& labels) {
    if (images.empty() || labels.empty()) {
        std::cerr << "Error: Empty training data" << std::endl;
        return false;
    }

    if (images.size() != labels.size()) {
        std::cerr << "Error: Images and labels size mismatch" << std::endl;
        return false;
    }

    try {
        std::cout << "Training LBPH recognizer with " << images.size() << " images..." << std::endl;
        recognizer->train(images, labels);
        is_trained = true;
        std::cout << "Recognizer trained successfully" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Exception in train: " << e.what() << std::endl;
        return false;
    }
}

bool FaceRecognizer::add_training_data(const cv::Mat& image, int person_id) {
    if (image.empty()) {
        std::cerr << "Error: Empty image" << std::endl;
        return false;
    }

    if (!db) {
        std::cerr << "Error: Database not set" << std::endl;
        return false;
    }

    try {
        // Preprocess the face image
        cv::Mat preprocessed;
        
        // Ensure grayscale
        if (image.channels() == 3) {
            cv::cvtColor(image, preprocessed, cv::COLOR_BGR2GRAY);
        } else {
            preprocessed = image.clone();
        }
        
        // Resize to standard size
        cv::resize(preprocessed, preprocessed, cv::Size(200, 200));
        
        // Equalize histogram
        cv::equalizeHist(preprocessed, preprocessed);
        
        // Serialize the preprocessed image
        std::vector<unsigned char> embedding = serialize_mat(preprocessed);
        
        // Get person info to use image path
        PersonRecord person;
        if (!db->get_person(person_id, person)) {
            std::cerr << "Error: Person not found with id " << person_id << std::endl;
            return false;
        }
        
        // Store embedding in database with a generated path
        std::string image_path = "embedding_" + std::to_string(person_id) + "_" + 
                                std::to_string(std::time(nullptr)) + ".dat";
        
        if (!db->add_face_embedding(person_id, image_path, embedding)) {
            std::cerr << "Error: Failed to store embedding in database" << std::endl;
            return false;
        }
        
        std::cout << "Face embedding stored for person " << person.name << std::endl;
        
        // Retrain the model with all data
        return retrain_model();
        
    } catch (const std::exception& e) {
        std::cerr << "Exception in add_training_data: " << e.what() << std::endl;
        return false;
    }
}

bool FaceRecognizer::train_from_images(const std::string& dataset_path) {
    if (!fs::exists(dataset_path)) {
        std::cerr << "Error: Dataset path does not exist: " << dataset_path << std::endl;
        return false;
    }

    try {
        std::vector<cv::Mat> training_images;
        std::vector<int> training_labels;
        label_to_name.clear();
        int next_id = 0;

        // Iterate through person subdirectories (e.g., dataset/A1/, dataset/B2/)
        for (const auto& person_dir : fs::directory_iterator(dataset_path)) {
            if (!fs::is_directory(person_dir)) continue;

            std::string person_name = person_dir.path().filename().string();
            label_to_name[next_id] = person_name;

            std::cout << "Loading images for person: " << person_name << std::endl;

            int image_count = 0;
            // Load all images from person's subdirectory
            for (const auto& img_file : fs::directory_iterator(person_dir.path())) {
                if (!fs::is_regular_file(img_file)) continue;

                std::string ext = img_file.path().extension().string();
                if (ext != ".jpg" && ext != ".JPG" && ext != ".jpeg" && ext != ".JPEG" &&
                    ext != ".png" && ext != ".PNG" && ext != ".bmp" && ext != ".BMP") {
                    continue;
                }

                // Load image in grayscale
                cv::Mat img = cv::imread(img_file.path().string(), cv::IMREAD_GRAYSCALE);
                if (img.empty()) {
                    std::cerr << "Warning: Failed to load image: " << img_file.path() << std::endl;
                    continue;
                }

                // Resize to standard size
                cv::Mat resized;
                cv::resize(img, resized, cv::Size(200, 200));

                // Equalize histogram for better recognition
                cv::equalizeHist(resized, resized);

                training_images.push_back(resized);
                training_labels.push_back(next_id);
                image_count++;
            }

            if (image_count > 0) {
                std::cout << "  Loaded " << image_count << " images for " << person_name << std::endl;
                next_id++;
            } else {
                std::cerr << "Warning: No images found for person " << person_name << std::endl;
                label_to_name.erase(next_id);
            }
        }

        if (training_images.empty()) {
            std::cerr << "Error: No training images found in dataset" << std::endl;
            return false;
        }

        std::cout << "Training with " << training_images.size() << " images from "
                  << label_to_name.size() << " people" << std::endl;

        // Train the LBPH recognizer
        return train(training_images, training_labels);

    } catch (const std::exception& e) {
        std::cerr << "Exception in train_from_images: " << e.what() << std::endl;
        return false;
    }
}

bool FaceRecognizer::train_from_database() {
    if (!db) {
        std::cerr << "Error: Database not set" << std::endl;
        return false;
    }

    try {
        std::vector<FaceEmbedding> embeddings;
        if (!db->get_all_face_embeddings(embeddings)) {
            std::cerr << "Error: Failed to load embeddings from database" << std::endl;
            return false;
        }

        if (embeddings.empty()) {
            std::cerr << "Error: No embeddings found in database" << std::endl;
            return false;
        }

        std::vector<cv::Mat> training_images;
        std::vector<int> training_labels;

        // Deserialize embeddings
        for (const auto& emb : embeddings) {
            if (emb.embedding_data.size() < sizeof(int) * 3) {
                std::cerr << "Warning: Invalid embedding data, skipping..." << std::endl;
                continue;
            }

            // Extract metadata
            const unsigned char* ptr = emb.embedding_data.data();
            int rows, cols, type;
            std::memcpy(&rows, ptr, sizeof(int));
            ptr += sizeof(int);
            std::memcpy(&cols, ptr, sizeof(int));
            ptr += sizeof(int);
            std::memcpy(&type, ptr, sizeof(int));

            cv::Mat face_img = deserialize_mat(emb.embedding_data, rows, cols, type);
            
            if (!face_img.empty()) {
                training_images.push_back(face_img);
                training_labels.push_back(emb.person_id);
            }
        }

        if (training_images.empty()) {
            std::cerr << "Error: No valid training images" << std::endl;
            return false;
        }

        std::cout << "Training with " << training_images.size() << " embeddings from database..." << std::endl;

        // Load labels from database
        load_labels_from_database();

        // Train the model
        return train(training_images, training_labels);

    } catch (const std::exception& e) {
        std::cerr << "Exception in train_from_database: " << e.what() << std::endl;
        return false;
    }
}

bool FaceRecognizer::retrain_model() {
    std::cout << "Retraining model with latest data..." << std::endl;
    return train_from_database();
}

int FaceRecognizer::recognize(const cv::Mat& face_image, double& confidence) {
    if (face_image.empty()) {
        std::cerr << "Error: Empty face image" << std::endl;
        confidence = 0.0;
        return -1;
    }

    if (!is_trained) {
        std::cerr << "Error: Recognizer not trained" << std::endl;
        confidence = 0.0;
        return -1;
    }

    try {
        // Preprocess the face image
        cv::Mat preprocessed;
        
        // Ensure grayscale
        if (face_image.channels() == 3) {
            cv::cvtColor(face_image, preprocessed, cv::COLOR_BGR2GRAY);
        } else {
            preprocessed = face_image.clone();
        }
        
        // Resize to standard size
        cv::resize(preprocessed, preprocessed, cv::Size(200, 200));
        
        // Equalize histogram
        cv::equalizeHist(preprocessed, preprocessed);
        
        // Predict using LBPH
        int label = -1;
        double distance = 0.0;
        recognizer->predict(preprocessed, label, distance);
        
        // Convert distance to similarity (0-1 where 1 is perfect match)
        // LBPH returns distance, lower is better
        double similarity = 1.0 / (1.0 + distance / 100.0);
        
        // Return confidence as similarity (0-1)
        confidence = similarity;
        
        // Check against threshold (0.6 = 60%)
        if (similarity >= confidence_threshold) {
            std::cout << "Recognized label " << label << " with similarity: " 
                      << (similarity * 100.0) << "%" << std::endl;
            return label;
        } else {
            std::cout << "Unknown person (similarity: " << (similarity * 100.0) 
                      << "% < threshold: " << (confidence_threshold * 100.0) << "%)" << std::endl;
            return -1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Exception in recognize: " << e.what() << std::endl;
        confidence = 0.0;
        return -1;
    }
}

std::string FaceRecognizer::recognize_with_name(const cv::Mat& face_image, double& confidence) {
    int person_id = recognize(face_image, confidence);
    if (person_id >= 0 && label_to_name.count(person_id)) {
        return label_to_name[person_id];
    }
    return "Unknown";
}

int FaceRecognizer::register_person(const std::string& name) {
    label_to_name[next_label] = name;
    return next_label++;
}

bool FaceRecognizer::set_label_name(int label, const std::string& name) {
    if (label_to_name.count(label) == 0) {
        return false;
    }
    label_to_name[label] = name;
    return true;
}

std::string FaceRecognizer::get_label_name(int label) const {
    auto it = label_to_name.find(label);
    if (it != label_to_name.end()) {
        return it->second;
    }
    return "Unknown";
}

int FaceRecognizer::get_label_from_name(const std::string& name) const {
    for (const auto& [label, label_name] : label_to_name) {
        if (label_name == name) {
            return label;
        }
    }
    return -1;
}

void FaceRecognizer::set_confidence_threshold(float threshold) {
    confidence_threshold = threshold;
}

float FaceRecognizer::get_confidence_threshold() const {
    return confidence_threshold;
}

bool FaceRecognizer::is_model_trained() const {
    return is_trained;
}

int FaceRecognizer::get_num_people() const {
    return label_to_name.size();
}

void FaceRecognizer::clear_training_data() {
    label_to_name.clear();
    next_label = 0;
    is_trained = false;
}
