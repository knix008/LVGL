// PyTorch to ONNX Converter Header
// This header defines the interface for converting PyTorch models to ONNX format

#ifndef PYTORCH_CONVERTER_H
#define PYTORCH_CONVERTER_H

#include <string>
#include <vector>
#include <memory>

// Forward declarations for LibTorch
namespace torch {
    namespace nn {
        class Module;
    }
    namespace jit {
        namespace script {
            class Module;
        }
    }
    class Tensor;
}

class PyTorchConverter {
public:
    PyTorchConverter();
    
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
    bool loadStateDictModel(const std::string& pytorchPath, 
                           const std::string& onnxPath,
                           const std::vector<int64_t>& inputShape);
    
    std::shared_ptr<torch::nn::Module> createYOLOv8Model();
    
    bool exportToOnnxAlternative(torch::nn::Module& model, 
                                const torch::Tensor& dummy_input,
                                const std::string& onnxPath);
    
    bool exportToOnnxAlternative(const torch::jit::script::Module& model, 
                                const torch::Tensor& dummy_input,
                                const std::string& onnxPath);
};

#endif // PYTORCH_CONVERTER_H
