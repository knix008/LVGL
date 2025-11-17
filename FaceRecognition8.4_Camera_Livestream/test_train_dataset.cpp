#include <iostream>
#include <filesystem>
#include <vector>
#include <map>
#include <unordered_map>
#include <opencv2/opencv.hpp>
#include <opencv2/face.hpp>

namespace fs = std::filesystem;

int main() {
    std::string dataset_dir = "./dataset";
    std::vector<cv::Mat> training_faces;
    std::vector<int> labels;
    std::unordered_map<std::string, int> person_to_label;
    std::unordered_map<int, std::string> label_to_person;
    
    cv::CascadeClassifier face_cascade;
    if (!face_cascade.load("/usr/share/opencv4/haarcascades/haarcascade_frontalface_default.xml")) {
        std::cerr << "Failed to load cascade" << std::endl;
        return 1;
    }
    
    std::cout << "Processing dataset directory: " << dataset_dir << std::endl;
    
    for (const auto& entry : fs::directory_iterator(dataset_dir)) {
        if (entry.is_directory()) {
            std::cout << "Skipping subdirectory: " << entry.path().filename() << std::endl;
            continue;
        }
        
        if (!entry.is_regular_file()) {
            continue;
        }
        
        std::string filename = entry.path().filename().string();
        std::string ext = entry.path().extension().string();
        
        if (ext != ".jpg" && ext != ".jpeg" && ext != ".png") {
            std::cout << "Skipping non-image: " << filename << std::endl;
            continue;
        }
        
        std::cout << "\nProcessing file: " << filename << std::endl;
        
        // Extract person ID from filename (e.g., "A.1.1.jpg" -> "A")
        size_t dot_pos = filename.find('.');
        if (dot_pos == std::string::npos) {
            std::cout << "  Skipping - no person ID" << std::endl;
            continue;
        }
        
        std::string person_id = filename.substr(0, dot_pos);
        std::cout << "  Person ID: " << person_id << std::endl;
        
        // Load image
        cv::Mat img = cv::imread(entry.path().string());
        if (img.empty()) {
            std::cout << "  Failed to load image" << std::endl;
            continue;
        }
        
        std::cout << "  Image loaded: " << img.cols << "x" << img.rows << std::endl;
        
        // Convert to grayscale
        cv::Mat gray;
        cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
        cv::equalizeHist(gray, gray);
        
        // Detect faces
        std::vector<cv::Rect> faces;
        face_cascade.detectMultiScale(gray, faces, 1.1, 6, 0, cv::Size(30, 30));
        
        std::cout << "  Detected " << faces.size() << " face(s)" << std::endl;
        
        if (faces.size() != 1) {
            std::cout << "  Skipping - need exactly 1 face" << std::endl;
            continue;
        }
        
        // Extract face
        cv::Mat face_roi = img(faces[0]).clone();
        
        // Convert RGB and resize
        cv::Mat rgb_face;
        cv::cvtColor(face_roi, rgb_face, cv::COLOR_BGR2RGB);
        
        cv::Mat resized;
        cv::resize(rgb_face, resized, cv::Size(200, 200));
        
        cv::Mat gray_face;
        cv::cvtColor(resized, gray_face, cv::COLOR_RGB2GRAY);
        
        std::cout << "  Processed face: " << gray_face.cols << "x" << gray_face.rows << std::endl;
        
        // Get label
        int label;
        auto it = person_to_label.find(person_id);
        if (it == person_to_label.end()) {
            label = static_cast<int>(person_to_label.size());
            person_to_label[person_id] = label;
            label_to_person[label] = person_id;
            std::cout << "  Assigned new label: " << label << std::endl;
        } else {
            label = it->second;
            std::cout << "  Using existing label: " << label << std::endl;
        }
        
        training_faces.push_back(gray_face);
        labels.push_back(label);
        
        std::cout << "  Added to training set (total: " << training_faces.size() << ")" << std::endl;
    }
    
    std::cout << "\n=== Training Summary ===" << std::endl;
    std::cout << "Total faces: " << training_faces.size() << std::endl;
    std::cout << "Total labels: " << labels.size() << std::endl;
    std::cout << "Unique persons: " << person_to_label.size() << std::endl;
    
    if (training_faces.empty()) {
        std::cerr << "No faces to train!" << std::endl;
        return 1;
    }
    
    std::cout << "\nTraining LBPH recognizer..." << std::endl;
    auto recognizer = cv::face::LBPHFaceRecognizer::create(1, 8, 8, 8);
    
    try {
        recognizer->train(training_faces, labels);
        std::cout << "Training successful!" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Training failed: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
