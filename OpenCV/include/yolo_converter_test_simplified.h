#ifndef YOLO_CONVERTER_TEST_H
#define YOLO_CONVERTER_TEST_H

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>
#include "yolov8n_detector.h"

struct TestResult
{
    bool success;
    std::string message;
    double loadTime;
    double inferenceTime;
    int numDetections;
    std::vector<cv::Rect> boundingBoxes;
    std::vector<float> confidences;
    std::vector<int> classIds;
};

class YOLOConverterTest
{
public:
    YOLOConverterTest();
    ~YOLOConverterTest();

    // Core visualization function (main functionality)
    bool visualizeDetections(const std::string& onnxPath,
                            const std::string& imagePath,
                            const std::string& outputPath = "",
                            const cv::Size& inputSize = {640, 640},
                            float confidenceThreshold = 0.25f);

    // Essential test functions (used by main program)
    TestResult testModelLoading(const std::string& onnxPath);
    TestResult validateModelStructure(const std::string& onnxPath);
    TestResult benchmarkModel(const std::string& onnxPath, 
                             int numRuns = 10,
                             const cv::Size& inputSize = {640, 640});
    TestResult testConvertedModel(const std::string& onnxPath, 
                                  const std::string& testImagePath = "",
                                  const cv::Size& inputSize = {640, 640});
    TestResult testInference(const std::string& onnxPath, 
                             const cv::Size& inputSize = {640, 640});
    TestResult testWithImage(const std::string& onnxPath, 
                             const std::string& imagePath,
                             const cv::Size& inputSize = {640, 640});
    TestResult testPostprocessing(const std::string& onnxPath);
    TestResult testWithConfidenceThreshold(const std::string& onnxPath, 
                                          float confidenceThreshold = 0.25f);

    // Statistics getters (used by main program)
    double getAverageInferenceTime() const { return avgInferenceTime; }
    double getMinInferenceTime() const { return minInferenceTime; }
    double getMaxInferenceTime() const { return maxInferenceTime; }
    int getTotalTests() const { return totalTests; }
    int getSuccessfulTests() const { return successfulTests; }
    double getSuccessRate() const { 
        return totalTests > 0 ? (double)successfulTests / totalTests * 100.0 : 0.0; 
    }

private:
    cv::Size modelInputSize;
    std::vector<std::string> classNames;
    
    // Statistics
    double avgInferenceTime;
    double minInferenceTime;
    double maxInferenceTime;
    int totalTests;
    int successfulTests;

    // Helper methods
    void resetStatistics();
    void updateStatistics(double inferenceTime, bool success);
};

#endif // YOLO_CONVERTER_TEST_H
