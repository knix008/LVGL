#include "yolo_converter_test.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <algorithm>
#include <numeric>

YOLOConverterTest::YOLOConverterTest()
    : modelInputSize(640, 640)
    , avgInferenceTime(0.0)
    , minInferenceTime(std::numeric_limits<double>::max())
    , maxInferenceTime(0.0)
    , totalTests(0)
    , successfulTests(0)
{
    loadClassNames();
}

YOLOConverterTest::~YOLOConverterTest()
{
    // Cleanup if needed
}

bool YOLOConverterTest::loadClassNames()
{
    // COCO dataset class names
    classNames = {
        "person", "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck", "boat",
        "traffic light", "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat",
        "dog", "horse", "sheep", "cow", "elephant", "bear", "zebra", "giraffe", "backpack",
        "umbrella", "handbag", "tie", "suitcase", "frisbee", "skis", "snowboard", "sports ball",
        "kite", "baseball bat", "baseball glove", "skateboard", "surfboard", "tennis racket",
        "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple",
        "sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake",
        "chair", "couch", "potted plant", "bed", "dining table", "toilet", "tv", "laptop",
        "mouse", "remote", "keyboard", "cell phone", "microwave", "oven", "toaster", "sink",
        "refrigerator", "book", "clock", "vase", "scissors", "teddy bear", "hair drier", "toothbrush"
    };
    return true;
}

TestResult YOLOConverterTest::testConvertedModel(const std::string& onnxPath, 
                                                 const std::string& testImagePath,
                                                 const cv::Size& inputSize)
{
    TestResult result;
    result.success = false;
    result.loadTime = 0.0;
    result.inferenceTime = 0.0;
    result.numDetections = 0;

    try {
        // Test 1: Model loading
        auto loadStart = std::chrono::high_resolution_clock::now();
        
        net = cv::dnn::readNetFromONNX(onnxPath);
        if (net.empty()) {
            result.message = "Failed to load ONNX model: " + onnxPath;
            return result;
        }

        auto loadEnd = std::chrono::high_resolution_clock::now();
        result.loadTime = std::chrono::duration<double, std::milli>(loadEnd - loadStart).count();

        // Test 2: Basic inference
        TestResult inferenceResult = testInference(onnxPath, inputSize);
        if (!inferenceResult.success) {
            result.message = "Inference test failed: " + inferenceResult.message;
            return result;
        }

        result.inferenceTime = inferenceResult.inferenceTime;
        result.numDetections = inferenceResult.numDetections;
        result.boundingBoxes = inferenceResult.boundingBoxes;
        result.confidences = inferenceResult.confidences;
        result.classIds = inferenceResult.classIds;

        // Test 3: Image test (if provided)
        if (!testImagePath.empty()) {
            TestResult imageResult = testWithImage(onnxPath, testImagePath, inputSize);
            if (imageResult.success) {
                result.numDetections = imageResult.numDetections;
                result.boundingBoxes = imageResult.boundingBoxes;
                result.confidences = imageResult.confidences;
                result.classIds = imageResult.classIds;
            }
        }

        result.success = true;
        result.message = "Model test completed successfully";
        updateStatistics(result.inferenceTime, true);

    } catch (const cv::Exception& e) {
        result.message = "OpenCV error: " + std::string(e.what());
    } catch (const std::exception& e) {
        result.message = "Error: " + std::string(e.what());
    }

    return result;
}

TestResult YOLOConverterTest::testModelLoading(const std::string& onnxPath)
{
    TestResult result;
    result.success = false;
    result.loadTime = 0.0;
    result.inferenceTime = 0.0;
    result.numDetections = 0;

    try {
        auto start = std::chrono::high_resolution_clock::now();
        
        net = cv::dnn::readNetFromONNX(onnxPath);
        
        auto end = std::chrono::high_resolution_clock::now();
        result.loadTime = std::chrono::duration<double, std::milli>(end - start).count();

        if (net.empty()) {
            result.message = "Failed to load ONNX model";
            return result;
        }

        result.success = true;
        result.message = "Model loaded successfully in " + std::to_string(result.loadTime) + "ms";

    } catch (const cv::Exception& e) {
        result.message = "OpenCV error loading model: " + std::string(e.what());
    } catch (const std::exception& e) {
        result.message = "Error loading model: " + std::string(e.what());
    }

    return result;
}

TestResult YOLOConverterTest::testInference(const std::string& onnxPath, const cv::Size& inputSize)
{
    TestResult result;
    result.success = false;
    result.loadTime = 0.0;
    result.inferenceTime = 0.0;
    result.numDetections = 0;

    try {
        // Load model
        net = cv::dnn::readNetFromONNX(onnxPath);
        if (net.empty()) {
            result.message = "Failed to load ONNX model";
            return result;
        }

        // Create dummy input
        cv::Mat dummyInput = cv::Mat::zeros(inputSize, CV_32FC3);
        
        // Preprocess dummy input
        cv::Mat blob = cv::dnn::blobFromImage(dummyInput, 1.0/255.0, inputSize, cv::Scalar(), true, false);
        
        // Set input
        net.setInput(blob);

        // Run inference
        auto start = std::chrono::high_resolution_clock::now();
        
        std::vector<cv::Mat> outputs;
        net.forward(outputs, net.getUnconnectedOutLayersNames());
        
        auto end = std::chrono::high_resolution_clock::now();
        result.inferenceTime = std::chrono::duration<double, std::milli>(end - start).count();

        // Process outputs
        std::vector<cv::Rect> boxes;
        std::vector<float> confidences;
        std::vector<int> classIds;
        
        postprocessDetections(outputs, boxes, confidences, classIds);
        
        result.boundingBoxes = boxes;
        result.confidences = confidences;
        result.classIds = classIds;
        result.numDetections = static_cast<int>(boxes.size());

        result.success = true;
        result.message = "Inference test completed successfully";

    } catch (const cv::Exception& e) {
        result.message = "OpenCV error during inference: " + std::string(e.what());
    } catch (const std::exception& e) {
        result.message = "Error during inference: " + std::string(e.what());
    }

    return result;
}

TestResult YOLOConverterTest::testWithImage(const std::string& onnxPath, 
                                            const std::string& imagePath,
                                            const cv::Size& inputSize)
{
    TestResult result;
    result.success = false;
    result.loadTime = 0.0;
    result.inferenceTime = 0.0;
    result.numDetections = 0;

    try {
        // Load image
        cv::Mat image = cv::imread(imagePath);
        if (image.empty()) {
            result.message = "Failed to load image: " + imagePath;
            return result;
        }

        // Load model
        net = cv::dnn::readNetFromONNX(onnxPath);
        if (net.empty()) {
            result.message = "Failed to load ONNX model";
            return result;
        }

        // Preprocess image
        cv::Mat preprocessed = preprocessImage(image, inputSize);
        cv::Mat blob = cv::dnn::blobFromImage(preprocessed, 1.0/255.0, inputSize, cv::Scalar(), true, false);
        
        // Set input
        net.setInput(blob);

        // Run inference
        auto start = std::chrono::high_resolution_clock::now();
        
        std::vector<cv::Mat> outputs;
        net.forward(outputs, net.getUnconnectedOutLayersNames());
        
        auto end = std::chrono::high_resolution_clock::now();
        result.inferenceTime = std::chrono::duration<double, std::milli>(end - start).count();

        // Process outputs
        std::vector<cv::Rect> boxes;
        std::vector<float> confidences;
        std::vector<int> classIds;
        
        postprocessDetections(outputs, boxes, confidences, classIds);
        
        result.boundingBoxes = boxes;
        result.confidences = confidences;
        result.classIds = classIds;
        result.numDetections = static_cast<int>(boxes.size());

        result.success = true;
        result.message = "Image test completed successfully";

    } catch (const cv::Exception& e) {
        result.message = "OpenCV error during image test: " + std::string(e.what());
    } catch (const std::exception& e) {
        result.message = "Error during image test: " + std::string(e.what());
    }

    return result;
}

TestResult YOLOConverterTest::benchmarkModel(const std::string& onnxPath, 
                                             int numRuns,
                                             const cv::Size& inputSize)
{
    TestResult result;
    result.success = false;
    result.loadTime = 0.0;
    result.inferenceTime = 0.0;
    result.numDetections = 0;

    try {
        // Load model
        net = cv::dnn::readNetFromONNX(onnxPath);
        if (net.empty()) {
            result.message = "Failed to load ONNX model";
            return result;
        }

        // Create dummy input
        cv::Mat dummyInput = cv::Mat::zeros(inputSize, CV_32FC3);
        cv::Mat blob = cv::dnn::blobFromImage(dummyInput, 1.0/255.0, inputSize, cv::Scalar(), true, false);
        
        // Set input
        net.setInput(blob);

        std::vector<double> inferenceTimes;
        
        // Run benchmark
        for (int i = 0; i < numRuns; ++i) {
            auto start = std::chrono::high_resolution_clock::now();
            
            std::vector<cv::Mat> outputs;
            net.forward(outputs, net.getUnconnectedOutLayersNames());
            
            auto end = std::chrono::high_resolution_clock::now();
            double inferenceTime = std::chrono::duration<double, std::milli>(end - start).count();
            inferenceTimes.push_back(inferenceTime);
        }

        // Calculate statistics
        double avgTime = std::accumulate(inferenceTimes.begin(), inferenceTimes.end(), 0.0) / inferenceTimes.size();
        double minTime = *std::min_element(inferenceTimes.begin(), inferenceTimes.end());
        double maxTime = *std::max_element(inferenceTimes.begin(), inferenceTimes.end());

        result.inferenceTime = avgTime;
        result.success = true;
        result.message = "Benchmark completed - Avg: " + std::to_string(avgTime) + 
                        "ms, Min: " + std::to_string(minTime) + 
                        "ms, Max: " + std::to_string(maxTime) + "ms";

        // Update global statistics
        avgInferenceTime = avgTime;
        minInferenceTime = std::min(minInferenceTime, minTime);
        maxInferenceTime = std::max(maxInferenceTime, maxTime);

    } catch (const cv::Exception& e) {
        result.message = "OpenCV error during benchmark: " + std::string(e.what());
    } catch (const std::exception& e) {
        result.message = "Error during benchmark: " + std::string(e.what());
    }

    return result;
}

TestResult YOLOConverterTest::validateModelStructure(const std::string& onnxPath)
{
    TestResult result;
    result.success = false;
    result.loadTime = 0.0;
    result.inferenceTime = 0.0;
    result.numDetections = 0;

    try {
        // Load model
        net = cv::dnn::readNetFromONNX(onnxPath);
        if (net.empty()) {
            result.message = "Failed to load ONNX model";
            return result;
        }

        // Get model information
        std::vector<cv::String> layerNames = net.getLayerNames();
        std::vector<cv::String> outputNames = net.getUnconnectedOutLayersNames();

        result.success = true;
        result.message = "Model structure validation completed - Layers: " + 
                        std::to_string(layerNames.size()) + 
                        ", Outputs: " + std::to_string(outputNames.size());

    } catch (const cv::Exception& e) {
        result.message = "OpenCV error during validation: " + std::string(e.what());
    } catch (const std::exception& e) {
        result.message = "Error during validation: " + std::string(e.what());
    }

    return result;
}

cv::Mat YOLOConverterTest::preprocessImage(const cv::Mat& image, const cv::Size& targetSize)
{
    cv::Mat resized;
    cv::resize(image, resized, targetSize);
    return resized;
}

std::vector<cv::Mat> YOLOConverterTest::runInference(const cv::Mat& input)
{
    net.setInput(input);
    std::vector<cv::Mat> outputs;
    net.forward(outputs, net.getUnconnectedOutLayersNames());
    return outputs;
}

void YOLOConverterTest::postprocessDetections(const std::vector<cv::Mat>& outputs,
                                              std::vector<cv::Rect>& boxes,
                                              std::vector<float>& confidences,
                                              std::vector<int>& classIds)
{
    // Simplified postprocessing for testing
    // In a real implementation, this would include NMS and proper YOLO output parsing
    
    boxes.clear();
    confidences.clear();
    classIds.clear();

    // For testing purposes, we'll just check if outputs are valid
    if (!outputs.empty() && !outputs[0].empty()) {
        // This is a placeholder - real implementation would parse YOLO outputs
        // and apply NMS (Non-Maximum Suppression)
    }
}

void YOLOConverterTest::resetStatistics()
{
    avgInferenceTime = 0.0;
    minInferenceTime = std::numeric_limits<double>::max();
    maxInferenceTime = 0.0;
    totalTests = 0;
    successfulTests = 0;
}

void YOLOConverterTest::updateStatistics(double inferenceTime, bool success)
{
    if (success) {
        avgInferenceTime = (avgInferenceTime * totalTests + inferenceTime) / (totalTests + 1);
        minInferenceTime = std::min(minInferenceTime, inferenceTime);
        maxInferenceTime = std::max(maxInferenceTime, inferenceTime);
        successfulTests++;
    }
    totalTests++;
}
