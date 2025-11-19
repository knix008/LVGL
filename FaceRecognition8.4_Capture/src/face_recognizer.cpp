#include "face_recognizer.h"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <set>

FaceRecognizer::FaceRecognizer()
    : confidence_threshold(0.6f), is_trained(false) {
}

FaceRecognizer::~FaceRecognizer() {
}

bool FaceRecognizer::initialize() {
    // Create LBPH face recognizer
    // Parameters: radius=1, neighbors=8, grid_x=8, grid_y=8
    recognizer = cv::face::LBPHFaceRecognizer::create(1, 8, 8, 8);

    if (!recognizer) {
        std::cerr << "Failed to create face recognizer" << std::endl;
        return false;
    }

    std::cout << "Face recognizer initialized successfully" << std::endl;
    return true;
}

bool FaceRecognizer::train_faces(const std::vector<cv::Mat>& faces,
                                const std::vector<int>& labels) {
    if (faces.empty() || labels.empty()) {
        std::cerr << "Empty face data provided for training" << std::endl;
        return false;
    }

    if (faces.size() != labels.size()) {
        std::cerr << "Mismatch between number of faces and labels" << std::endl;
        return false;
    }

    // Validate all face images and labels
    std::set<int> unique_labels;
    for (size_t i = 0; i < faces.size(); ++i) {
        if (faces[i].empty()) {
            std::cerr << "Face image " << i << " is empty!" << std::endl;
            return false;
        }
        if (faces[i].channels() != 1) {
            std::cerr << "Face image " << i << " is not grayscale (has "
                      << faces[i].channels() << " channels)" << std::endl;
            return false;
        }
        if (labels[i] < 0) {
            std::cerr << "Face image " << i << " has invalid label: " << labels[i] << std::endl;
            return false;
        }
        unique_labels.insert(labels[i]);
        std::cout << "Face " << i << ": " << faces[i].cols << "x" << faces[i].rows
                  << " channels=" << faces[i].channels()
                  << " label=" << labels[i] << std::endl;
    }

    std::cout << "Training with " << unique_labels.size() << " unique labels: ";
    for (int lbl : unique_labels) {
        std::cout << lbl << " ";
    }
    std::cout << std::endl;

    try {
        std::cout << "Calling OpenCV LBPH recognizer train() with "
                  << faces.size() << " faces..." << std::endl;
        // Train the recognizer
        recognizer->train(faces, labels);
        is_trained = true;
        std::cout << "Recognizer trained successfully with " << faces.size() << " faces" << std::endl;
        return true;
    } catch (const cv::Exception& e) {
        std::cerr << "OpenCV error during training: " << e.what() << std::endl;
        std::cerr << "  Error code: " << e.code << std::endl;
        std::cerr << "  Error msg: " << e.msg << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Error during training: " << e.what() << std::endl;
        return false;
    } catch (...) {
        std::cerr << "Unknown error during training" << std::endl;
        return false;
    }
}

std::vector<float> FaceRecognizer::get_face_embedding(const cv::Mat& face_image) {
    std::vector<float> embedding;

    if (face_image.empty()) {
        std::cerr << "Empty face image provided" << std::endl;
        return embedding;
    }

    // Resize face to standard size for consistent embeddings
    cv::Mat resized_face;
    cv::resize(face_image, resized_face, cv::Size(200, 200));

    // Convert to grayscale if necessary
    cv::Mat gray_face;
    if (resized_face.channels() == 3) {
        cv::cvtColor(resized_face, gray_face, cv::COLOR_RGB2GRAY);
    } else if (resized_face.channels() == 4) {
        cv::cvtColor(resized_face, gray_face, cv::COLOR_RGBA2GRAY);
    } else {
        gray_face = resized_face.clone();
    }

    // Extract features using LBPH
    // LBPH creates a histogram, we'll convert it to a vector
    // For now, we create a simple feature vector based on histogram
    int grid_x = 8;
    int grid_y = 8;

    // Create LBP histogram
    std::vector<cv::Mat> hist_list;
    int hist_size = 256;  // 2^8 for 8 neighbors
    float range[] = {0, 256};
    const float* ranges[] = {range};

    for (int i = 0; i < grid_y; ++i) {
        for (int j = 0; j < grid_x; ++j) {
            int x1 = j * gray_face.cols / grid_x;
            int y1 = i * gray_face.rows / grid_y;
            int x2 = (j + 1) * gray_face.cols / grid_x;
            int y2 = (i + 1) * gray_face.rows / grid_y;

            cv::Mat roi = gray_face(cv::Rect(x1, y1, x2 - x1, y2 - y1));

            cv::Mat hist;
            cv::calcHist(&roi, 1, 0, cv::Mat(), hist, 1, &hist_size, ranges, true, false);
            hist = hist.reshape(1, 1);  // Flatten to 1D

            for (int k = 0; k < hist.cols; ++k) {
                embedding.push_back(hist.at<float>(0, k));
            }
        }
    }

    // Normalize embedding to unit length
    float norm = 0.0f;
    for (float val : embedding) {
        norm += val * val;
    }
    norm = std::sqrt(norm);

    if (norm > 1e-6f) {
        for (auto& val : embedding) {
            val /= norm;
        }
    }

    return embedding;
}

RecognitionResult FaceRecognizer::recognize_face(const cv::Mat& face_image) {
    RecognitionResult result;
    result.is_registered = false;
    result.confidence = 0.0f;

    if (face_image.empty()) {
        std::cerr << "Empty face image provided for recognition" << std::endl;
        return result;
    }

    if (!is_trained) {
        std::cerr << "Recognizer has not been trained yet" << std::endl;
        return result;
    }

    try {
        // Resize and convert to grayscale
        cv::Mat resized_face;
        cv::resize(face_image, resized_face, cv::Size(200, 200));

        cv::Mat gray_face;
        if (resized_face.channels() == 3) {
            cv::cvtColor(resized_face, gray_face, cv::COLOR_RGB2GRAY);
        } else if (resized_face.channels() == 4) {
            cv::cvtColor(resized_face, gray_face, cv::COLOR_RGBA2GRAY);
        } else {
            gray_face = resized_face.clone();
        }

        // Predict
        int label = -1;
        double confidence = 0.0;
        recognizer->predict(gray_face, label, confidence);

        auto mapping_it = label_to_person_id.find(label);
        if (mapping_it != label_to_person_id.end()) {
            result.person_id = mapping_it->second;
        } else {
            result.person_id = "Person_" + std::to_string(label);
        }
        result.person_name = result.person_id;
        result.confidence = static_cast<float>(confidence);

        // LBPH confidence is distance, lower is better
        // Convert to similarity (0-1 where 1 is perfect match)
        float similarity = 1.0f / (1.0f + result.confidence / 100.0f);
        result.confidence = similarity;

        if (similarity > confidence_threshold) {
            result.is_registered = true;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error during recognition: " << e.what() << std::endl;
    }

    return result;
}

float FaceRecognizer::calculate_similarity(const std::vector<float>& embedding1,
                                          const std::vector<float>& embedding2) {
    if (embedding1.empty() || embedding2.empty() ||
        embedding1.size() != embedding2.size()) {
        return 0.0f;
    }

    // Calculate cosine similarity
    float dot_product = 0.0f;
    float norm1 = 0.0f;
    float norm2 = 0.0f;

    for (size_t i = 0; i < embedding1.size(); ++i) {
        dot_product += embedding1[i] * embedding2[i];
        norm1 += embedding1[i] * embedding1[i];
        norm2 += embedding2[i] * embedding2[i];
    }

    norm1 = std::sqrt(norm1);
    norm2 = std::sqrt(norm2);

    if (norm1 < 1e-6f || norm2 < 1e-6f) {
        return 0.0f;
    }

    return dot_product / (norm1 * norm2);
}

void FaceRecognizer::set_confidence_threshold(float threshold) {
    confidence_threshold = std::max(0.0f, std::min(1.0f, threshold));
}

bool FaceRecognizer::load_model(const std::string& model_path) {
    try {
        recognizer->read(model_path);
        is_trained = true;
        std::cout << "Model loaded from: " << model_path << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error loading model: " << e.what() << std::endl;
        return false;
    }
}

bool FaceRecognizer::save_model(const std::string& model_path) {
    if (!is_trained) {
        std::cerr << "Cannot save untrained model" << std::endl;
        return false;
    }

    try {
        recognizer->write(model_path);
        std::cout << "Model saved to: " << model_path << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving model: " << e.what() << std::endl;
        return false;
    }
}

void FaceRecognizer::set_label_mapping(const std::unordered_map<int, std::string>& mapping) {
    label_to_person_id = mapping;
}
