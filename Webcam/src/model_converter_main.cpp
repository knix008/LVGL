// PyTorch to ONNX Model Converter
// Standalone executable for converting PyTorch models to ONNX format

#include <iostream>
#include <string>
#include <getopt.h>

#include "simple_model_converter.h"

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [OPTIONS]\n"
              << "Options:\n"
              << "  -i, --input PATH       Input PyTorch model path\n"
              << "  -o, --output PATH      Output ONNX model path (optional)\n"
              << "  -s, --shape W,H,D      Input shape (default: 1,3,640,640)\n"
              << "  -v, --validate         Validate the converted model\n"
              << "  -h, --help             Show this help message\n"
              << "\nExamples:\n"
              << "  " << programName << " -i models/yolov8_face_model.pt\n"
              << "  " << programName << " -i models/yolov8_face_model.pt -o models/face_model.onnx\n"
              << "  " << programName << " -i models/yolov8_face_model.pt -s 1,3,640,640 -v\n"
              << std::endl;
}

int main(int argc, char** argv) {
    std::string inputPath = "";
    std::string outputPath = "";
    std::string shapeStr = "1,3,640,640";
    bool validate = false;

    // Command line options
    static struct option long_options[] = {
        {"input",     required_argument, 0, 'i'},
        {"output",    required_argument, 0, 'o'},
        {"shape",     required_argument, 0, 's'},
        {"validate",  no_argument,       0, 'v'},
        {"help",      no_argument,       0, 'h'},
        {0, 0, 0, 0}
    };

    int option_index = 0;
    int c;

    while ((c = getopt_long(argc, argv, "i:o:s:vh", long_options, &option_index)) != -1) {
        switch (c) {
            case 'i':
                inputPath = optarg;
                break;
            case 'o':
                outputPath = optarg;
                break;
            case 's':
                shapeStr = optarg;
                break;
            case 'v':
                validate = true;
                break;
            case 'h':
                printUsage(argv[0]);
                return 0;
            case '?':
                printUsage(argv[0]);
                return 1;
            default:
                abort();
        }
    }

    // Validate inputs
    if (inputPath.empty()) {
        std::cerr << "Error: Must specify input PyTorch model path (-i)" << std::endl;
        printUsage(argv[0]);
        return 1;
    }

    // Parse input shape
    std::vector<int64_t> inputShape;
    size_t pos = 0;
    std::string token;
    std::string shapeStrCopy = shapeStr;
    
    while ((pos = shapeStrCopy.find(',')) != std::string::npos) {
        token = shapeStrCopy.substr(0, pos);
        inputShape.push_back(std::stoll(token));
        shapeStrCopy.erase(0, pos + 1);
    }
    inputShape.push_back(std::stoll(shapeStrCopy));

    if (inputShape.size() != 4) {
        std::cerr << "Error: Input shape must have 4 dimensions (batch, channels, height, width)" << std::endl;
        return 1;
    }

    // Generate output path if not specified
    if (outputPath.empty()) {
        size_t lastDot = inputPath.find_last_of('.');
        if (lastDot != std::string::npos) {
            outputPath = inputPath.substr(0, lastDot) + ".onnx";
        } else {
            outputPath = inputPath + ".onnx";
        }
    }

    std::cout << "PyTorch to ONNX Model Converter" << std::endl;
    std::cout << "=================================" << std::endl;
    std::cout << "Input model: " << inputPath << std::endl;
    std::cout << "Output model: " << outputPath << std::endl;
    std::cout << "Input shape: [";
    for (size_t i = 0; i < inputShape.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << inputShape[i];
    }
    std::cout << "]" << std::endl;
    std::cout << "Validate: " << (validate ? "Yes" : "No") << std::endl;
    std::cout << std::endl;

    try {
        // Initialize converter
        SimpleModelConverter converter;

        // Convert model
        std::cout << "Starting conversion..." << std::endl;
        bool success = converter.convertToOnnx(inputPath, outputPath, inputShape);

        if (success) {
            std::cout << "\nConversion completed successfully!" << std::endl;
            
            if (validate) {
                std::cout << "Validating converted model..." << std::endl;
                if (converter.validateOnnxModel(outputPath)) {
                    std::cout << "Model validation passed!" << std::endl;
                } else {
                    std::cout << "Model validation failed!" << std::endl;
                    return 1;
                }
            }
            
            std::cout << "ONNX model ready for use: " << outputPath << std::endl;
        } else {
            std::cerr << "Conversion failed!" << std::endl;
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
