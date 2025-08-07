#include "yolov8n_detector.h"
#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
    try {
        // Load the YOLOv8n model
        YOLOv8nDetector detector("../models/yolov8n.onnx");
        
        // Load the test image
        cv::Mat image = cv::imread("../data/bus.jpg");
        if (image.empty()) {
            std::cerr << "Error: Could not load image from ../data/bus.jpg" << std::endl;
            return -1;
        }
        
        std::cout << "Image loaded successfully. Size: " << image.size() << std::endl;
        
        // Run prediction
        std::vector<Detection> detections = detector.predict(image);
        
        std::cout << "Found " << detections.size() << " detections:" << std::endl;
        for (const auto& det : detections) {
            std::cout << "  - " << det.class_name 
                      << " (confidence: " << det.confidence 
                      << ", bbox: " << det.bbox << ")" << std::endl;
        }
        
        // Draw detections on the image
        detector.draw_detections(image, detections);
        
        // Display the result
        detector.show_result(image);
        
        // Save the result
        detector.save_result(image, "result.jpg");
        
        std::cout << "Detection completed successfully!" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return -1;
    }
    
    return 0;
} 