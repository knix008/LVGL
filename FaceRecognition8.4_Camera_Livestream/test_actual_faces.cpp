#include <opencv2/face.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <filesystem>

int main() {
    try {
        cv::Ptr<cv::face::LBPHFaceRecognizer> recognizer = 
            cv::face::LBPHFaceRecognizer::create(1, 8, 8, 8);
        
        std::vector<cv::Mat> faces;
        std::vector<int> labels;
        
        // Load actual face images from dataset
        std::vector<std::string> person_dirs = {"./dataset/A", "./dataset/B"};
        
        for (size_t person_idx = 0; person_idx < person_dirs.size(); person_idx++) {
            std::string person_dir = person_dirs[person_idx];
            std::cout << "Processing " << person_dir << " (label=" << person_idx << ")" << std::endl;
            
            for (const auto& entry : std::filesystem::directory_iterator(person_dir)) {
                if (entry.is_regular_file()) {
                    std::string filename = entry.path().filename().string();
                    if (filename.find("face_") == 0 && filename.find(".png") != std::string::npos) {
                        std::string path = entry.path().string();
                        std::cout << "  Loading: " << path << std::endl;
                        
                        cv::Mat face_img = cv::imread(path);
                        if (face_img.empty()) {
                            std::cerr << "  Failed to load!" << std::endl;
                            continue;
                        }
                        
                        std::cout << "  Loaded: " << face_img.cols << "x" << face_img.rows 
                                  << " channels=" << face_img.channels() << std::endl;
                        
                        // Convert to RGB then grayscale (same as main code)
                        cv::Mat rgb_face;
                        cv::cvtColor(face_img, rgb_face, cv::COLOR_BGR2RGB);
                        
                        cv::Mat resized_face;
                        cv::resize(rgb_face, resized_face, cv::Size(200, 200));
                        
                        cv::Mat gray_face;
                        cv::cvtColor(resized_face, gray_face, cv::COLOR_RGB2GRAY);
                        
                        std::cout << "  Processed: " << gray_face.cols << "x" << gray_face.rows 
                                  << " channels=" << gray_face.channels() 
                                  << " type=" << gray_face.type() << std::endl;
                        
                        faces.push_back(gray_face);
                        labels.push_back(person_idx);
                    }
                }
            }
        }
        
        std::cout << "\nTotal faces loaded: " << faces.size() << std::endl;
        std::cout << "Training..." << std::endl;
        recognizer->train(faces, labels);
        std::cout << "Training successful!" << std::endl;
        
        return 0;
    } catch (const cv::Exception& e) {
        std::cerr << "OpenCV exception: " << e.what() << std::endl;
        std::cerr << "  Code: " << e.code << std::endl;
        std::cerr << "  Msg: " << e.msg << std::endl;
        return 1;
    } catch (const std::out_of_range& e) {
        std::cerr << "out_of_range exception: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
}
