// YOLO Model Converter Main Program
// Demonstrates conversion of PyTorch YOLO models to ONNX format

#include <iostream>
#include <string>
#include <filesystem>
#include <getopt.h>
#include "yolo_converter.h"

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [OPTIONS]\n"
              << "Options:\n"
              << "  -i, --input PATH       Input PyTorch model file (.pt)\n"
              << "  -o, --output PATH      Output ONNX file path\n"
              << "  -w, --width SIZE       Input width (default: 640)\n"
              << "  -h, --height SIZE      Input height (default: 640)\n"
              << "  -b, --batch SIZE       Batch size (default: 1)\n"
              << "  -d, --device DEVICE    Device (cpu/cuda, default: cpu)\n"
              << "  -c, --confidence THRESHOLD  Confidence threshold (default: 0.25)\n"
              << "  -n, --nms THRESHOLD    NMS threshold (default: 0.45)\n"
              << "  -s, --simplify         Simplify ONNX model\n"
              << "  -O, --optimize         Optimize ONNX model\n"
              << "  -t, --test IMAGE       Test converted model with image\n"
              << "  -v, --validate         Validate converted model\n"
              << "  -I, --info             Show model information\n"
              << "  --help                 Show this help message\n"
              << "\nExamples:\n"
              << "  " << programName << " -i yolov8n.pt -o yolov8n.onnx\n"
              << "  " << programName << " -i yolov5s.pt -o yolov5s.onnx -s -O\n"
              << "  " << programName << " -i yolov8n.pt -o yolov8n.onnx -t test.jpg\n"
              << "  " << programName << " -i yolov8n.pt -I\n"
              << std::endl;
}

int main(int argc, char** argv) {
    ConversionConfig config;
    bool showInfo = false;
    bool validateModel = false;
    std::string testImagePath = "";
    
    // Command line options
    static struct option long_options[] = {
        {"input",        required_argument, 0, 'i'},
        {"output",       required_argument, 0, 'o'},
        {"width",        required_argument, 0, 'w'},
        {"height",       required_argument, 0, 'h'},
        {"batch",        required_argument, 0, 'b'},
        {"device",       required_argument, 0, 'd'},
        {"confidence",   required_argument, 0, 'c'},
        {"nms",          required_argument, 0, 'n'},
        {"simplify",     no_argument,       0, 's'},
        {"optimize",     no_argument,       0, 'O'},
        {"test",         required_argument, 0, 't'},
        {"validate",     no_argument,       0, 'v'},
        {"info",         no_argument,       0, 'I'},
        {"help",         no_argument,       0, 'H'},
        {0, 0, 0, 0}
    };

    int option_index = 0;
    int c;

    while ((c = getopt_long(argc, argv, "i:o:w:h:b:d:c:n:sOt:vIH", long_options, &option_index)) != -1) {
        switch (c) {
            case 'i':
                config.inputPath = optarg;
                break;
            case 'o':
                config.outputPath = optarg;
                break;
            case 'w':
                config.inputWidth = std::stoi(optarg);
                break;
            case 'h':
                config.inputHeight = std::stoi(optarg);
                break;
            case 'b':
                config.batchSize = std::stoi(optarg);
                break;
            case 'd':
                config.device = optarg;
                break;
            case 'c':
                config.confidenceThreshold = std::stof(optarg);
                break;
            case 'n':
                config.nmsThreshold = std::stof(optarg);
                break;
            case 's':
                config.simplify = true;
                break;
            case 'O':
                config.optimize = true;
                break;
            case 't':
                testImagePath = optarg;
                break;
            case 'v':
                validateModel = true;
                break;
            case 'I':
                showInfo = true;
                break;
            case 'H':
                printUsage(argv[0]);
                return 0;
            case '?':
                printUsage(argv[0]);
                return 1;
            default:
                abort();
        }
    }

    // Validate required inputs
    if (config.inputPath.empty()) {
        std::cerr << "Error: Input file is required (-i)" << std::endl;
        printUsage(argv[0]);
        return 1;
    }

    if (config.outputPath.empty() && !showInfo) {
        // Generate output path from input path
        std::filesystem::path inputPath(config.inputPath);
        config.outputPath = inputPath.stem().string() + ".onnx";
    }

    std::cout << "YOLO Model Converter" << std::endl;
    std::cout << "====================" << std::endl;
    std::cout << "Input file: " << config.inputPath << std::endl;
    if (!config.outputPath.empty()) {
        std::cout << "Output file: " << config.outputPath << std::endl;
    }
    std::cout << "Input size: " << config.inputWidth << "x" << config.inputHeight << std::endl;
    std::cout << "Batch size: " << config.batchSize << std::endl;
    std::cout << "Device: " << config.device << std::endl;
    std::cout << "Simplify: " << (config.simplify ? "Yes" : "No") << std::endl;
    std::cout << "Optimize: " << (config.optimize ? "Yes" : "No") << std::endl;
    std::cout << std::endl;

    try {
        YOLOConverter converter;

        // Show model information if requested
        if (showInfo) {
            std::cout << "Model Information:" << std::endl;
            std::cout << "=================" << std::endl;
            
            ModelInfo info = converter.getModelInfo(config.inputPath);
            std::cout << "Model type: " << info.modelType << std::endl;
            std::cout << "Version: " << info.version << std::endl;
            std::cout << "Input channels: " << info.inputChannels << std::endl;
            std::cout << "Dynamic shape: " << (info.hasDynamicShape ? "Yes" : "No") << std::endl;
            std::cout << "Number of classes: " << info.classes.size() << std::endl;
            
            std::cout << "\nClasses:" << std::endl;
            for (size_t i = 0; i < info.classes.size(); ++i) {
                std::cout << "  " << i << ": " << info.classes[i] << std::endl;
            }
            
            return 0;
        }

        // Perform conversion
        if (!config.outputPath.empty()) {
            std::cout << "Starting conversion..." << std::endl;
            
            if (converter.convertPyTorchToONNX(config)) {
                std::cout << "Conversion completed successfully!" << std::endl;
                
                // Validate if requested
                if (validateModel) {
                    std::cout << "\nValidating converted model..." << std::endl;
                    if (converter.validateONNXModel(config.outputPath)) {
                        std::cout << "Model validation passed!" << std::endl;
                    } else {
                        std::cout << "Model validation failed!" << std::endl;
                        return 1;
                    }
                }
                
                // Test with image if provided
                if (!testImagePath.empty()) {
                    std::cout << "\nTesting converted model with image..." << std::endl;
                    if (converter.testConvertedModel(config.outputPath, testImagePath)) {
                        std::cout << "Model test completed successfully!" << std::endl;
                    } else {
                        std::cout << "Model test failed!" << std::endl;
                        return 1;
                    }
                }
                
            } else {
                std::cerr << "Conversion failed: " << converter.getLastError() << std::endl;
                return 1;
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
