#include <opencv2/face.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>

int main() {
    try {
        // Create LBPH recognizer with same parameters
        cv::Ptr<cv::face::LBPHFaceRecognizer> recognizer = 
            cv::face::LBPHFaceRecognizer::create(1, 8, 8, 8);
        
        // Create 2 test images (200x200 grayscale)
        std::vector<cv::Mat> faces;
        std::vector<int> labels;
        
        for (int i = 0; i < 2; i++) {
            cv::Mat face = cv::Mat::zeros(200, 200, CV_8UC1);
            // Add some random pattern
            cv::randu(face, 0, 255);
            faces.push_back(face);
            labels.push_back(i);
            std::cout << "Created face " << i << ": " << face.cols << "x" << face.rows 
                      << " type=" << face.type() << std::endl;
        }
        
        std::cout << "Training with " << faces.size() << " faces..." << std::endl;
        recognizer->train(faces, labels);
        std::cout << "Training successful!" << std::endl;
        
        return 0;
    } catch (const cv::Exception& e) {
        std::cerr << "OpenCV exception: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
}
