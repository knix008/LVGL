#ifndef YOLO_CONVERTER_TEST_H
#define YOLO_CONVERTER_TEST_H

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>

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

    // Test converted ONNX model
    TestResult testConvertedModel(const std::string& onnxPath, 
                                  const std::string& testImagePath = "",
                                  const cv::Size& inputSize = {640, 640});

    // Test model loading
    TestResult testModelLoading(const std::string& onnxPath);

    // Test inference with dummy data
    TestResult testInference(const std::string& onnxPath, 
                             const cv::Size& inputSize = {640, 640});

    // Test with actual image
    TestResult testWithImage(const std::string& onnxPath, 
                             const std::string& imagePath,
                             const cv::Size& inputSize = {640, 640});

    // Benchmark model performance
    TestResult benchmarkModel(const std::string& onnxPath, 
                             int numRuns = 10,
                             const cv::Size& inputSize = {640, 640});

    // Validate model structure
    TestResult validateModelStructure(const std::string& onnxPath);

    // Get test statistics
    double getAverageInferenceTime() const { return avgInferenceTime; }
    double getMinInferenceTime() const { return minInferenceTime; }
    double getMaxInferenceTime() const { return maxInferenceTime; }
    int getTotalTests() const { return totalTests; }
    int getSuccessfulTests() const { return successfulTests; }

private:
    cv::dnn::Net net;
    cv::Size modelInputSize;
    std::vector<std::string> classNames;
    
    // Statistics
    double avgInferenceTime;
    double minInferenceTime;
    double maxInferenceTime;
    int totalTests;
    int successfulTests;

    // Helper methods
    bool loadClassNames();
    cv::Mat preprocessImage(const cv::Mat& image, const cv::Size& targetSize);
    std::vector<cv::Mat> runInference(const cv::Mat& input);
    void postprocessDetections(const std::vector<cv::Mat>& outputs, 
                              std::vector<cv::Rect>& boxes,
                              std::vector<float>& confidences,
                              std::vector<int>& classIds);
    void resetStatistics();
    void updateStatistics(double inferenceTime, bool success);
};

#endif // YOLO_CONVERTER_TEST_H
