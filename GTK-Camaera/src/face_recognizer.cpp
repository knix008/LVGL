#include "face_recognizer.h"
#include "face_database.h"
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <cmath>

namespace fs = std::filesystem;

FaceRecognizer::FaceRecognizer() {
    recognizer = cv::face::LBPHFaceRecognizer::create();
}

std::vector<float> FaceRecognizer::generate_embedding(const cv::Mat& face_image) {
    if (face_image.empty()) {
        std::cerr << "Error: Empty face image for embedding" << std::endl;
        return std::vector<float>();
    }

    try {
        // Resize face to standard size
        cv::Mat resized;
        cv::resize(face_image, resized, cv::Size(200, 200));

        // Convert to grayscale if necessary
        cv::Mat gray;
        if (resized.channels() == 3) {
            cv::cvtColor(resized, gray, cv::COLOR_BGR2GRAY);
        } else if (resized.channels() == 4) {
            cv::cvtColor(resized, gray, cv::COLOR_BGRA2GRAY);
        } else {
            gray = resized.clone();
        }

        // Generate LBPH embedding using 8x8 grid with 256 bins
        std::vector<float> embedding;
        int grid_x = 8;
        int grid_y = 8;
        int hist_size = 256;
        float range[] = {0, 256};
        const float* ranges[] = {range};

        for (int i = 0; i < grid_y; ++i) {
            for (int j = 0; j < grid_x; ++j) {
                int x1 = j * gray.cols / grid_x;
                int y1 = i * gray.rows / grid_y;
                int x2 = (j + 1) * gray.cols / grid_x;
                int y2 = (i + 1) * gray.rows / grid_y;

                cv::Mat roi = gray(cv::Rect(x1, y1, x2 - x1, y2 - y1));

                cv::Mat hist;
                cv::calcHist(&roi, 1, 0, cv::Mat(), hist, 1, &hist_size, ranges, true, false);
                hist = hist.reshape(1, 1);

                for (int k = 0; k < hist.cols; ++k) {
                    embedding.push_back(hist.at<float>(0, k));
                }
            }
        }

        return embedding;
    } catch (const std::exception& e) {
        std::cerr << "Exception in generate_embedding: " << e.what() << std::endl;
        return std::vector<float>();
    }
}

float FaceRecognizer::calculate_similarity(const std::vector<float>& emb1, const std::vector<float>& emb2) {
    if (emb1.empty() || emb2.empty() || emb1.size() != emb2.size()) {
        return 0.0f;
    }

    // Calculate cosine similarity
    float dot_product = 0.0f;
    float norm1 = 0.0f;
    float norm2 = 0.0f;

    for (size_t i = 0; i < emb1.size(); ++i) {
        dot_product += emb1[i] * emb2[i];
        norm1 += emb1[i] * emb1[i];
        norm2 += emb2[i] * emb2[i];
    }

    norm1 = std::sqrt(norm1);
    norm2 = std::sqrt(norm2);

    if (norm1 == 0.0f || norm2 == 0.0f) {
        return 0.0f;
    }

    return dot_product / (norm1 * norm2);
}

bool FaceRecognizer::train_from_images(const std::string& dataset_path) {
    if (!fs::exists(dataset_path)) {
        std::cerr << "Error: Dataset path does not exist: " << dataset_path << std::endl;
        return false;
    }

    try {
        person_embeddings.clear();
        label_to_name.clear();
        next_label = 0;
        int total_embeddings = 0;

        // Iterate through person subdirectories
        for (const auto& person_dir : fs::directory_iterator(dataset_path)) {
            if (!fs::is_directory(person_dir)) continue;

            std::string person_name = person_dir.path().filename().string();
            person_embeddings[next_label] = std::vector<std::vector<float>>();
            label_to_name[next_label] = person_name;

            std::cout << "Processing person: " << person_name << " (id: " << next_label << ")" << std::endl;

            // Load images from person's subdirectory and generate embeddings
            int images_count = 0;
            for (const auto& img_file : fs::directory_iterator(person_dir.path())) {
                if (!fs::is_regular_file(img_file)) continue;

                std::string filename = img_file.path().filename().string();
                std::string ext = img_file.path().extension().string();

                if (ext != ".jpg" && ext != ".JPG" && ext != ".jpeg" && ext != ".JPEG" &&
                    ext != ".png" && ext != ".PNG" && ext != ".bmp" && ext != ".BMP") {
                    continue;
                }

                // Load image
                cv::Mat img = cv::imread(img_file.path().string(), cv::IMREAD_GRAYSCALE);
                if (img.empty()) {
                    std::cerr << "Warning: Failed to load image: " << img_file.path() << std::endl;
                    continue;
                }

                // Generate embedding
                std::vector<float> embedding = generate_embedding(img);
                if (!embedding.empty()) {
                    person_embeddings[next_label].push_back(embedding);
                    total_embeddings++;
                    std::cout << "  Loaded: " << filename << " (embedding size: " << embedding.size() << ")" << std::endl;
                    images_count++;
                }
            }

            if (images_count > 0) {
                std::cout << "Person " << person_name << ": " << images_count << " embeddings generated" << std::endl;
                next_label++;
            } else {
                std::cerr << "Warning: No images found for person " << person_name << std::endl;
                person_embeddings.erase(next_label);
                label_to_name.erase(next_label);
            }
        }

        if (person_embeddings.empty()) {
            std::cerr << "Error: No training embeddings generated from: " << dataset_path << std::endl;
            return false;
        }

        is_trained = true;
        std::cout << "Training complete with " << total_embeddings << " embeddings from "
                  << person_embeddings.size() << " people" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Exception in train_from_images: " << e.what() << std::endl;
        return false;
    }
}

bool FaceRecognizer::add_person_embedding(int person_id, const std::vector<float>& embedding) {
    if (embedding.empty()) {
        std::cerr << "Error: Empty embedding" << std::endl;
        return false;
    }

    try {
        person_embeddings[person_id].push_back(embedding);
        is_trained = true;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Exception in add_person_embedding: " << e.what() << std::endl;
        return false;
    }
}

std::vector<float> FaceRecognizer::get_face_embedding(const cv::Mat& face_image) {
    return generate_embedding(face_image);
}

int FaceRecognizer::recognize(const cv::Mat& face_image, double& confidence) {
    if (!is_trained || person_embeddings.empty()) {
        std::cerr << "Error: No embeddings available for recognition" << std::endl;
        confidence = 0.0;
        return -1;
    }

    try {
        // Generate embedding for input face
        std::vector<float> face_embedding = generate_embedding(face_image);
        if (face_embedding.empty()) {
            std::cerr << "Error: Failed to generate embedding for input face" << std::endl;
            confidence = 0.0;
            return -1;
        }

        float best_similarity = -1.0f;
        int best_person_id = -1;

        // Compare with all stored embeddings
        for (const auto& [person_id, embeddings] : person_embeddings) {
            for (const auto& stored_embedding : embeddings) {
                float similarity = calculate_similarity(face_embedding, stored_embedding);
                if (similarity > best_similarity) {
                    best_similarity = similarity;
                    best_person_id = person_id;
                }
            }
        }

        confidence = best_similarity;

        if (best_similarity >= confidence_threshold) {
            std::cout << "Recognized person: " << label_to_name[best_person_id]
                      << " (similarity: " << best_similarity << ")" << std::endl;
            return best_person_id;
        } else {
            std::cout << "Unknown person (best similarity: " << best_similarity << ")" << std::endl;
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

bool FaceRecognizer::load_embeddings_from_db() {
    // This will be implemented in gtk_app.cpp when FaceDatabase is available
    return true;
}

int FaceRecognizer::register_person(const std::string& name) {
    label_to_name[next_label] = name;
    person_embeddings[next_label] = std::vector<std::vector<float>>();
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
    return is_trained && !person_embeddings.empty();
}

int FaceRecognizer::get_num_people() const {
    return person_embeddings.size();
}

void FaceRecognizer::clear_training_data() {
    person_embeddings.clear();
    label_to_name.clear();
    next_label = 0;
    is_trained = false;
}
