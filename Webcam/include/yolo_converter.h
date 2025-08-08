// YOLO Model Converter - PyTorch to ONNX
// Converts YOLO PyTorch models (.pt) to ONNX format

#ifndef YOLO_CONVERTER_H
#define YOLO_CONVERTER_H

#include <string>
#include <vector>
#include <memory>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>

struct ConversionConfig
{
    std::string inputPath;           // Input .pt file path
    std::string outputPath;          // Output .onnx file path
    int inputWidth = 640;            // Model input width
    int inputHeight = 640;           // Model input height
    int batchSize = 1;               // Batch size
    bool dynamicShape = false;       // Enable dynamic shape
    bool simplify = true;            // Simplify ONNX model
    bool optimize = true;            // Optimize ONNX model
    std::string device = "cpu";      // Device (cpu/cuda)
    float confidenceThreshold = 0.25; // Confidence threshold
    float nmsThreshold = 0.45;       // NMS threshold
    int numClasses = 80;             // Number of classes
};

struct ModelInfo
{
    std::string modelType;           // YOLOv5, YOLOv8, etc.
    std::string version;             // Model version
    std::vector<std::string> classes; // Class names
    int inputChannels = 3;           // Input channels
    bool hasDynamicShape = false;    // Dynamic shape support
};

class YOLOConverter
{
public:
    YOLOConverter();
    ~YOLOConverter();

    // Main conversion function
    bool convertPyTorchToONNX(const ConversionConfig& config);
    
    // Model validation
    bool validateONNXModel(const std::string& onnxPath);
    
    // Get model information
    ModelInfo getModelInfo(const std::string& ptPath);
    
    // Test converted model
    bool testConvertedModel(const std::string& onnxPath, const std::string& testImagePath);
    
    // Get last error
    std::string getLastError() const;

private:
    // Internal conversion methods
    bool convertYOLOv5ToONNX(const ConversionConfig& config);
    bool convertYOLOv8ToONNX(const ConversionConfig& config);
    bool convertYOLOv6ToONNX(const ConversionConfig& config);
    bool convertYOLOv7ToONNX(const ConversionConfig& config);
    
    // Utility functions
    std::string detectModelType(const std::string& ptPath);
    bool createDummyInput(const ConversionConfig& config, cv::Mat& dummyInput);
    bool optimizeONNXModel(const std::string& onnxPath);
    bool simplifyONNXModel(const std::string& onnxPath);
    
    // Model testing
    bool runInferenceTest(const std::string& onnxPath, const cv::Mat& testImage);
    
    // File operations
    bool fileExists(const std::string& path);
    bool createDirectory(const std::string& path);
    std::string getFileExtension(const std::string& path);
    
    // Error handling
    std::string lastError;
    void setError(const std::string& error);
    
    // Model detection
    std::string modelType;
    std::string modelVersion;
};

// Utility functions
namespace YOLOConverterUtils
{
    // Check if PyTorch is available
    bool isPyTorchAvailable();
    
    // Get system information
    std::string getSystemInfo();
    
    // Validate file paths
    bool validateInputPath(const std::string& path);
    bool validateOutputPath(const std::string& path);
    
    // Model type detection
    std::string detectYOLOVersion(const std::string& ptPath);
    
    // File operations
    bool backupFile(const std::string& originalPath);
    bool restoreFile(const std::string& backupPath, const std::string& originalPath);
    
    // Progress reporting
    void reportProgress(const std::string& message, float percentage = -1.0f);
}

#endif // YOLO_CONVERTER_H
