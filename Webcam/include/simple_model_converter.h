// Simple Model Converter Header
// This header defines the interface for converting PyTorch models to ONNX format

#ifndef SIMPLE_MODEL_CONVERTER_H
#define SIMPLE_MODEL_CONVERTER_H

#include <string>
#include <vector>

class SimpleModelConverter {
public:
    SimpleModelConverter();
    
    // Main conversion function
    bool convertToOnnx(const std::string& pytorchPath, 
                       const std::string& onnxPath,
                       const std::vector<int64_t>& inputShape = {1, 3, 640, 640});
    
    // Utility functions
    bool validateOnnxModel(const std::string& onnxPath);
    std::string getOnnxPath(const std::string& pytorchPath);
    
    // Automatic conversion with default settings
    bool convertModel(const std::string& pytorchPath);

private:
    // Internal conversion methods
    bool isPythonAvailable();
    std::string createConversionScript(const std::string& pytorchPath,
                                     const std::string& onnxPath,
                                     const std::vector<int64_t>& inputShape);
    bool executePythonScript(const std::string& scriptPath);
};

#endif // SIMPLE_MODEL_CONVERTER_H
