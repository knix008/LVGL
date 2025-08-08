#include "yolo_converter_test.h"
#include <iostream>
#include <string>
#include <getopt.h>
#include <filesystem>
#include <iomanip>

void printUsage(const char* programName)
{
    std::cout << "YOLO Converter Test Program\n";
    std::cout << "============================\n\n";
    std::cout << "Usage: " << programName << " [OPTIONS]\n\n";
    std::cout << "Options:\n";
    std::cout << "  -m, --model <path>        ONNX model path to test\n";
    std::cout << "  -i, --image <path>        Test image path (optional)\n";
    std::cout << "  -s, --size <width>x<height> Input size (default: 640x640)\n";
    std::cout << "  -b, --benchmark <runs>    Run benchmark with specified number of runs\n";
    std::cout << "  -l, --load-only           Test only model loading\n";
    std::cout << "  -v, --validate            Validate model structure\n";
    std::cout << "  -V, --visualize           Visualize detections on image (requires -i)\n";
    std::cout << "  -o, --output <path>       Save visualization result to file\n";
    std::cout << "  -h, --help                Show this help message\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << programName << " -m yolov8n_converted.onnx\n";
    std::cout << "  " << programName << " -m yolov8n_converted.onnx -i ../data/bus.jpg\n";
    std::cout << "  " << programName << " -m yolov8n_converted.onnx -b 100\n";
    std::cout << "  " << programName << " -m yolov8n_converted.onnx -l\n";
    std::cout << "  " << programName << " -m yolov8n_converted.onnx -v\n";
    std::cout << "  " << programName << " -m yolov8n_converted.onnx -i ../data/bus.jpg -V\n";
    std::cout << "  " << programName << " -m yolov8n_converted.onnx -i ../data/bus.jpg -V -o result.jpg\n\n";
}

void printTestResult(const TestResult& result, const std::string& testName)
{
    std::cout << "\n" << testName << " Results:\n";
    std::cout << "========================\n";
    
    if (result.success) {
        std::cout << "✅ Status: SUCCESS\n";
        std::cout << "📝 Message: " << result.message << "\n";
        
        if (result.loadTime > 0) {
            std::cout << "⏱️  Load Time: " << result.loadTime << " ms\n";
        }
        
        if (result.inferenceTime > 0) {
            std::cout << "⚡ Inference Time: " << result.inferenceTime << " ms\n";
        }
        
        if (result.numDetections > 0) {
            std::cout << "🎯 Detections: " << result.numDetections << "\n";
        }
        
        if (!result.boundingBoxes.empty()) {
            std::cout << "📦 Bounding Boxes: " << result.boundingBoxes.size() << "\n";
            for (size_t i = 0; i < std::min(result.boundingBoxes.size(), size_t(5)); ++i) {
                const auto& box = result.boundingBoxes[i];
                const auto& conf = result.confidences[i];
                const auto& classId = result.classIds[i];
                std::cout << "   Box " << i << ": [" << box.x << ", " << box.y << ", " 
                          << box.width << ", " << box.height << "] "
                          << "Conf: " << conf << " Class: " << classId << "\n";
            }
            if (result.boundingBoxes.size() > 5) {
                std::cout << "   ... and " << (result.boundingBoxes.size() - 5) << " more\n";
            }
        }
    } else {
        std::cout << "❌ Status: FAILED\n";
        std::cout << "💥 Error: " << result.message << "\n";
    }
    std::cout << "\n";
}

int main(int argc, char* argv[])
{
    std::string modelPath;
    std::string imagePath;
    std::string outputPath;
    cv::Size inputSize(640, 640);
    int benchmarkRuns = 0;
    bool loadOnly = false;
    bool validateOnly = false;
    bool visualizeOnly = false;
    
    // Command line options
    static struct option long_options[] = {
        {"model", required_argument, 0, 'm'},
        {"image", required_argument, 0, 'i'},
        {"size", required_argument, 0, 's'},
        {"benchmark", required_argument, 0, 'b'},
        {"load-only", no_argument, 0, 'l'},
        {"validate", no_argument, 0, 'v'},
        {"visualize", no_argument, 0, 'V'},
        {"output", required_argument, 0, 'o'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };
    
    int opt;
    int option_index = 0;
    
    while ((opt = getopt_long(argc, argv, "m:i:s:b:lvo:Vh", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'm':
                modelPath = optarg;
                break;
            case 'i':
                imagePath = optarg;
                break;
            case 's':
                {
                    std::string sizeStr = optarg;
                    size_t xPos = sizeStr.find('x');
                    if (xPos != std::string::npos) {
                        int width = std::stoi(sizeStr.substr(0, xPos));
                        int height = std::stoi(sizeStr.substr(xPos + 1));
                        inputSize = cv::Size(width, height);
                    } else {
                        std::cerr << "Error: Invalid size format. Use WIDTHxHEIGHT (e.g., 640x640)\n";
                        return 1;
                    }
                }
                break;
            case 'b':
                benchmarkRuns = std::stoi(optarg);
                break;
            case 'l':
                loadOnly = true;
                break;
            case 'v':
                validateOnly = true;
                break;
            case 'V':
                visualizeOnly = true;
                break;
            case 'o':
                outputPath = optarg;
                break;
            case 'h':
                printUsage(argv[0]);
                return 0;
            default:
                printUsage(argv[0]);
                return 1;
        }
    }
    
    // Check required arguments
    if (modelPath.empty()) {
        std::cerr << "Error: Model path is required. Use -m or --model option.\n\n";
        printUsage(argv[0]);
        return 1;
    }
    
    // Check if model file exists
    if (!std::filesystem::exists(modelPath)) {
        std::cerr << "Error: Model file does not exist: " << modelPath << "\n";
        return 1;
    }
    
    // Check if image file exists (if provided)
    if (!imagePath.empty() && !std::filesystem::exists(imagePath)) {
        std::cerr << "Error: Image file does not exist: " << imagePath << "\n";
        return 1;
    }
    
    std::cout << "YOLO Converter Test Program\n";
    std::cout << "===========================\n\n";
    std::cout << "Model: " << modelPath << "\n";
    std::cout << "Input Size: " << inputSize.width << "x" << inputSize.height << "\n";
    if (!imagePath.empty()) {
        std::cout << "Test Image: " << imagePath << "\n";
    }
    if (benchmarkRuns > 0) {
        std::cout << "Benchmark Runs: " << benchmarkRuns << "\n";
    }
    std::cout << "\n";
    
    // Create test instance
    YOLOConverterTest tester;
    
    try {
        // Run specific tests based on options
        if (loadOnly) {
            // Test only model loading
            TestResult loadResult = tester.testModelLoading(modelPath);
            printTestResult(loadResult, "Model Loading Test");
        } else if (validateOnly) {
            // Test only model validation
            TestResult validateResult = tester.validateModelStructure(modelPath);
            printTestResult(validateResult, "Model Structure Validation");
        } else if (benchmarkRuns > 0) {
            // Run benchmark
            TestResult benchmarkResult = tester.benchmarkModel(modelPath, benchmarkRuns, inputSize);
            printTestResult(benchmarkResult, "Model Benchmark Test");
            
            // Print statistics
            std::cout << "Benchmark Statistics:\n";
            std::cout << "====================\n";
            std::cout << "Average Inference Time: " << tester.getAverageInferenceTime() << " ms\n";
            std::cout << "Minimum Inference Time: " << tester.getMinInferenceTime() << " ms\n";
            std::cout << "Maximum Inference Time: " << tester.getMaxInferenceTime() << " ms\n";
            std::cout << "Total Tests: " << tester.getTotalTests() << "\n";
            std::cout << "Successful Tests: " << tester.getSuccessfulTests() << "\n\n";
        } else if (visualizeOnly) {
            // Visualize detections
            if (imagePath.empty()) {
                std::cerr << "Error: Image path is required for visualization. Use -i option.\n";
                return 1;
            }
            
            std::cout << "Visualizing detections on image: " << imagePath << "\n";
            bool success = tester.visualizeDetections(modelPath, imagePath, outputPath, inputSize);
            
            if (success) {
                std::cout << "✅ Visualization completed successfully!\n";
            } else {
                std::cout << "❌ Visualization failed!\n";
                return 1;
            }
        } else {
            // Run comprehensive test
            TestResult comprehensiveResult = tester.testConvertedModel(modelPath, imagePath, inputSize);
            printTestResult(comprehensiveResult, "Comprehensive Model Test");
            
            // Additional tests
            TestResult inferenceResult = tester.testInference(modelPath, inputSize);
            printTestResult(inferenceResult, "Inference Test");
            
            if (!imagePath.empty()) {
                TestResult imageResult = tester.testWithImage(modelPath, imagePath, inputSize);
                printTestResult(imageResult, "Image Test");
            }
            
            TestResult validateResult = tester.validateModelStructure(modelPath);
            printTestResult(validateResult, "Model Structure Validation");
            
            // Additional tests for better coverage
            TestResult postprocessResult = tester.testPostprocessing(modelPath);
            printTestResult(postprocessResult, "Postprocessing Test");
            
            TestResult confidenceResult = tester.testWithConfidenceThreshold(modelPath, 0.3f);
            printTestResult(confidenceResult, "Confidence Threshold Test");
        }
        
        // Print final statistics
        std::cout << "\nFinal Test Statistics:\n";
        std::cout << "=====================\n";
        std::cout << "Total Tests: " << tester.getTotalTests() << "\n";
        std::cout << "Successful Tests: " << tester.getSuccessfulTests() << "\n";
        std::cout << "Success Rate: " << std::fixed << std::setprecision(2) 
                  << tester.getSuccessRate() << "%\n";
        std::cout << "Average Inference Time: " << std::fixed << std::setprecision(2) 
                  << tester.getAverageInferenceTime() << " ms\n";
        std::cout << "Min Inference Time: " << std::fixed << std::setprecision(2) 
                  << tester.getMinInferenceTime() << " ms\n";
        std::cout << "Max Inference Time: " << std::fixed << std::setprecision(2) 
                  << tester.getMaxInferenceTime() << " ms\n";
        
        std::cout << "\nTest completed successfully!\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error during testing: " << e.what() << "\n";
        return 1;
    }
}
