// Ultralytics 🚀 AGPL-3.0 License - https://ultralytics.com/license

#ifndef YOLOV8N_VIDEO_DETECTOR_H
#define YOLOV8N_VIDEO_DETECTOR_H

// Cpp native
#include <fstream>
#include <vector>
#include <string>
#include <random>
#include <chrono>

// OpenCV / DNN / Inference
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <opencv2/videoio.hpp>

#include "yolov8n_detector.h"

struct VideoDetectionResult
{
    std::vector<Detection> detections;
    double fps;
    int frameCount;
    double processingTime;
};

class YOLOv8nVideoDetector
{
public:
    YOLOv8nVideoDetector(const std::string &onnxModelPath, 
                         const cv::Size &modelInputShape = {640, 640}, 
                         const std::string &classesTxtFile = "", 
                         const bool &runWithCuda = false);
    
    // Process video file
    bool processVideoFile(const std::string &videoPath, 
                         const std::string &outputPath = "",
                         bool showFPS = true,
                         bool saveOutput = true);
    
    // Process webcam
    bool processWebcam(int cameraIndex = 0, 
                      bool showFPS = true,
                      bool saveOutput = false,
                      const std::string &outputPath = "");
    
    // Process single frame (for custom video processing)
    VideoDetectionResult processFrame(const cv::Mat &frame);
    
    // Draw detections on frame
    cv::Mat drawDetections(const cv::Mat &frame, const std::vector<Detection> &detections, bool showFPS = true);
    
    // Get current FPS
    double getCurrentFPS() const { return currentFPS; }
    
    // Get total processing time
    double getTotalProcessingTime() const { return totalProcessingTime; }
    
    // Get total frames processed
    int getTotalFramesProcessed() const { return totalFramesProcessed; }

private:
    Inference detector;
    double currentFPS;
    double totalProcessingTime;
    int totalFramesProcessed;
    std::chrono::high_resolution_clock::time_point lastFrameTime;
    
    // FPS calculation
    void updateFPS();
    
    // Video processing utilities
    bool initializeVideoWriter(const std::string &outputPath, const cv::Size &frameSize);
    cv::VideoWriter videoWriter;
    bool videoWriterInitialized;
};

#endif // YOLOV8N_VIDEO_DETECTOR_H
