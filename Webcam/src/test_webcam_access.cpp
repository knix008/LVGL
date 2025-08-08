#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    std::cout << "Testing webcam access..." << std::endl;
    
    // Try to open camera
    cv::VideoCapture cap(0);
    
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open camera at index 0" << std::endl;
        return 1;
    }
    
    std::cout << "Camera opened successfully!" << std::endl;
    
    // Set properties
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    cap.set(cv::CAP_PROP_FPS, 30);
    
    // Get actual properties
    double width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    double height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    double fps = cap.get(cv::CAP_PROP_FPS);
    
    std::cout << "Camera properties:" << std::endl;
    std::cout << "  Width: " << width << std::endl;
    std::cout << "  Height: " << height << std::endl;
    std::cout << "  FPS: " << fps << std::endl;
    
    // Try to read a frame
    cv::Mat frame;
    std::cout << "Attempting to read frame..." << std::endl;
    
    if (cap.read(frame)) {
        std::cout << "Successfully read frame!" << std::endl;
        std::cout << "Frame size: " << frame.cols << "x" << frame.rows << std::endl;
        std::cout << "Frame type: " << frame.type() << std::endl;
    } else {
        std::cerr << "Failed to read frame" << std::endl;
    }
    
    cap.release();
    std::cout << "Test completed." << std::endl;
    
    return 0;
}
