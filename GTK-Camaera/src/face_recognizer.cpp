#include "face_recognizer.h"
#include <iostream>
#include <filesystem>
#include <algorithm>

namespace fs = std::filesystem;

FaceRecognizer::FaceRecognizer() {
    recognizer = cv::face::LBPHFaceRecognizer::create();
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
        // Verify all images have the same size
        cv::Size img_size = images[0].size();
        for (const auto& img : images) {
            if (img.size() != img_size) {
                std::cerr << "Error: All images must have the same size" << std::endl;
                return false;
            }
        }

        recognizer->train(images, labels);
        is_trained = true;

        std::cout << "Face recognizer trained successfully with " << images.size() << " images" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Exception in train: " << e.what() << std::endl;
        is_trained = false;
        return false;
    }
}

bool FaceRecognizer::add_training_data(const cv::Mat& image, const std::string& name) {
    if (image.empty()) {
        std::cerr << "Error: Empty image" << std::endl;
        return false;
    }

    try {
        // Check if person already exists
        int label = get_label_from_name(name);
        if (label == -1) {
            // Create new label for new person
            label = register_person(name);
        }

        // Store image for later training
        // Note: In a real application, you would accumulate images
        // and train the model when enough data is collected

        return true;
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
        std::map<std::string, int> person_to_label;
        int next_new_label = 0;

        // Iterate through person subdirectories: dataset/A1/, dataset/B2/, etc.
        for (const auto& person_dir : fs::directory_iterator(dataset_path)) {
            if (!fs::is_directory(person_dir)) continue;

            // Get person name from directory name (e.g., "A1", "B2")
            std::string person_name = person_dir.path().filename().string();

            // Assign label to person
            person_to_label[person_name] = next_new_label;
            label_to_name[next_new_label] = person_name;
            std::cout << "Registering person: " << person_name << " (label: " << next_new_label << ")" << std::endl;

            // Load images from this person's subdirectory
            int images_count = 0;
            for (const auto& img_file : fs::directory_iterator(person_dir.path())) {
                if (!fs::is_regular_file(img_file)) continue;

                std::string filename = img_file.path().filename().string();
                std::string ext = img_file.path().extension().string();

                // Check file extension
                if (ext != ".jpg" && ext != ".JPG" && ext != ".jpeg" && ext != ".JPEG" &&
                    ext != ".png" && ext != ".PNG" && ext != ".bmp" && ext != ".BMP") {
                    continue;
                }

                // Load the image
                cv::Mat img = cv::imread(img_file.path().string(), cv::IMREAD_GRAYSCALE);
                if (img.empty()) {
                    std::cerr << "Warning: Failed to load image: " << img_file.path() << std::endl;
                    continue;
                }

                // Resize to standard size
                cv::Mat resized;
                cv::resize(img, resized, cv::Size(200, 200));

                int label = next_new_label;
                training_images.push_back(resized);
                training_labels.push_back(label);

                std::cout << "  Loaded: " << filename << " (label: " << label << ")" << std::endl;
                images_count++;
            }

            if (images_count > 0) {
                std::cout << "Person " << person_name << ": " << images_count << " images loaded" << std::endl;
                next_new_label++;
            } else {
                std::cerr << "Warning: No images found for person " << person_name << std::endl;
                // Remove the label for this person if no images were found
                person_to_label.erase(person_name);
                label_to_name.erase(next_new_label);
            }
        }

        if (training_images.empty()) {
            std::cerr << "Error: No training images found in: " << dataset_path << std::endl;
            return false;
        }

        std::cout << "Training with " << training_images.size() << " images from " << person_to_label.size() << " people" << std::endl;

        next_label = next_new_label;
        return train(training_images, training_labels);
    } catch (const std::exception& e) {
        std::cerr << "Exception in train_from_images: " << e.what() << std::endl;
        return false;
    }
}

int FaceRecognizer::recognize(const cv::Mat& face_image, double& confidence) {
    if (face_image.empty()) {
        std::cerr << "Error: Empty face image" << std::endl;
        confidence = -1;
        return -1;
    }

    if (!is_trained) {
        std::cerr << "Error: Model not trained" << std::endl;
        confidence = -1;
        return -1;
    }

    try {
        cv::Mat preprocessed = face_image.clone();

        // Ensure grayscale
        if (preprocessed.channels() == 3) {
            cv::cvtColor(preprocessed, preprocessed, cv::COLOR_BGR2GRAY);
        }

        // Resize to standard size
        cv::resize(preprocessed, preprocessed, cv::Size(200, 200));

        // Equalize histogram
        cv::equalizeHist(preprocessed, preprocessed);

        int label = -1;
        double conf = 0.0;

        recognizer->predict(preprocessed, label, conf);
        confidence = conf;

        return label;
    } catch (const std::exception& e) {
        std::cerr << "Exception in recognize: " << e.what() << std::endl;
        confidence = -1;
        return -1;
    }
}

std::string FaceRecognizer::recognize_with_name(const cv::Mat& face_image, double& confidence) {
    int label = recognize(face_image, confidence);

    if (label == -1 || confidence > confidence_threshold) {
        return "Unknown";
    }

    return get_label_name(label);
}

bool FaceRecognizer::save_model(const std::string& path) {
    try {
        std::string save_path = path.empty() ? model_path : path;

        if (!is_trained) {
            std::cerr << "Error: Model not trained, cannot save" << std::endl;
            return false;
        }

        recognizer->save(save_path);
        std::cout << "Model saved to: " << save_path << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Exception in save_model: " << e.what() << std::endl;
        return false;
    }
}

bool FaceRecognizer::load_model(const std::string& path) {
    try {
        std::string load_path = path.empty() ? model_path : path;

        if (!fs::exists(load_path)) {
            std::cerr << "Error: Model file not found: " << load_path << std::endl;
            return false;
        }

        recognizer->read(load_path);
        is_trained = true;
        std::cout << "Model loaded from: " << load_path << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Exception in load_model: " << e.what() << std::endl;
        is_trained = false;
        return false;
    }
}

int FaceRecognizer::register_person(const std::string& name) {
    int label = next_label++;
    label_to_name[label] = name;
    return label;
}

bool FaceRecognizer::set_label_name(int label, const std::string& name) {
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
    for (const auto& [label, person_name] : label_to_name) {
        if (person_name == name) {
            return label;
        }
    }
    return -1;
}

void FaceRecognizer::set_confidence_threshold(double threshold) {
    confidence_threshold = threshold;
}

double FaceRecognizer::get_confidence_threshold() const {
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
