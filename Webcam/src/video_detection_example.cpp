// Ultralytics 🚀 AGPL-3.0 License - https://ultralytics.com/license
// Simple example demonstrating YOLOv8n video detection

#include <iostream>
#include <string>

#include "yolov8n_video_detector.h"

int main() {
    std::cout << "YOLOv8n Video Detection Example" << std::endl;
    std::cout << "===============================" << std::endl;
    
    // Example 1: Process video file
    std::cout << "\nExample 1: Processing video file..." << std::endl;
    try {
        YOLOv8nVideoDetector detector("../models/yolov8n.onnx", cv::Size(640, 640), "", false);
        
        // Process video file (if it exists)
        std::string videoPath = "../data/traffic.mp4";
        std::string outputPath = "traffic_detected.mp4";
        
        if (detector.processVideoFile(videoPath, outputPath, true, true)) {
            std::cout << "Video processing completed successfully!" << std::endl;
        } else {
            std::cout << "Video file not found or processing failed. This is expected if the video file doesn't exist." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    
    // Example 2: Process single frame (for custom processing)
    std::cout << "\nExample 2: Processing single frame..." << std::endl;
    try {
        YOLOv8nVideoDetector detector("../models/yolov8n.onnx", cv::Size(640, 640), "", false);
        
        // Load a test image
        cv::Mat testImage = cv::imread("../data/bus.jpg");
        if (!testImage.empty()) {
            VideoDetectionResult result = detector.processFrame(testImage);
            
            std::cout << "Frame processed successfully!" << std::endl;
            std::cout << "Number of detections: " << result.detections.size() << std::endl;
            std::cout << "Processing time: " << result.processingTime * 1000 << " ms" << std::endl;
            
            // Draw detections
            cv::Mat outputFrame = detector.drawDetections(testImage, result.detections, true);
            cv::imwrite("single_frame_detection.jpg", outputFrame);
            std::cout << "Output saved as: single_frame_detection.jpg" << std::endl;
        } else {
            std::cout << "Test image not found. This is expected if the image file doesn't exist." << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    
    std::cout << "\nExamples completed!" << std::endl;
    std::cout << "To use webcam detection, run: ./yolov8n_video_detection -w" << std::endl;
    std::cout << "To process a video file, run: ./yolov8n_video_detection -v <video_path> -o <output_path>" << std::endl;
    
    return 0;
}
