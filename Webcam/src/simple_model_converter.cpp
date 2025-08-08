// Simple Model Converter
// This implementation provides a practical approach to model conversion without LibTorch

#include "simple_model_converter.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cstdio>

SimpleModelConverter::SimpleModelConverter() {
    // Initialize converter
    std::cout << "Simple Model Converter initialized" << std::endl;
}

bool SimpleModelConverter::convertToOnnx(const std::string& pytorchPath, 
                                        const std::string& onnxPath,
                                        const std::vector<int64_t>& inputShape) {
    try {
        std::cout << "Converting PyTorch model: " << pytorchPath << std::endl;
        std::cout << "Output ONNX model: " << onnxPath << std::endl;
        
        // Check if input file exists
        std::ifstream inputFile(pytorchPath);
        if (!inputFile.good()) {
            std::cerr << "Error: Input PyTorch model not found: " << pytorchPath << std::endl;
            return false;
        }
        inputFile.close();
        
        // Check if Python is available
        if (!isPythonAvailable()) {
            std::cerr << "Error: Python is not available for model conversion" << std::endl;
            return false;
        }
        
        // Create Python conversion script
        std::string scriptPath = createConversionScript(pytorchPath, onnxPath, inputShape);
        
        // Execute Python script
        bool success = executePythonScript(scriptPath);
        
        // Clean up temporary script
        std::remove(scriptPath.c_str());
        
        if (success) {
            // Validate the converted model
            if (validateOnnxModel(onnxPath)) {
                std::cout << "Model conversion completed successfully!" << std::endl;
                return true;
            } else {
                std::cerr << "Error: Converted model validation failed" << std::endl;
                return false;
            }
        } else {
            std::cerr << "Error: Model conversion failed" << std::endl;
            return false;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error during conversion: " << e.what() << std::endl;
        return false;
    }
}

bool SimpleModelConverter::isPythonAvailable() {
    int result = std::system("python3 --version > /dev/null 2>&1");
    return result == 0;
}

std::string SimpleModelConverter::createConversionScript(const std::string& pytorchPath,
                                                       const std::string& onnxPath,
                                                       const std::vector<int64_t>& inputShape) {
    std::string scriptPath = "temp_conversion_script.py";
    
    std::ofstream script(scriptPath);
    if (!script.is_open()) {
        throw std::runtime_error("Could not create conversion script");
    }
    
    script << "#!/usr/bin/env python3\n";
    script << "import sys\n";
    script << "import os\n";
    script << "import torch\n";
    script << "import traceback\n";
    script << "\n";
    script << "def convert_model(pytorch_path, onnx_path, input_shape):\n";
    script << "    try:\n";
    script << "        print(f'Loading PyTorch model from: {pytorch_path}')\n";
    script << "        \n";
    script << "        # Try to load the model\n";
    script << "        model = torch.load(pytorch_path, map_location='cpu')\n";
    script << "        \n";
    script << "        # Set model to evaluation mode if it has eval method\n";
    script << "        if hasattr(model, 'eval'):\n";
    script << "            model.eval()\n";
    script << "        \n";
    script << "        # Create dummy input tensor\n";
    script << "        dummy_input = torch.randn(" << inputShape[0] << ", " << inputShape[1] 
           << ", " << inputShape[2] << ", " << inputShape[3] << ")\n";
    script << "        \n";
    script << "        print(f'Converting to ONNX format...')\n";
    script << "        \n";
    script << "        # Export to ONNX\n";
    script << "        torch.onnx.export(\n";
    script << "            model,\n";
    script << "            dummy_input,\n";
    script << "            onnx_path,\n";
    script << "            export_params=True,\n";
    script << "            opset_version=11,\n";
    script << "            do_constant_folding=True,\n";
    script << "            input_names=['input'],\n";
    script << "            output_names=['output'],\n";
    script << "            dynamic_axes={\n";
    script << "                'input': {0: 'batch_size'},\n";
    script << "                'output': {0: 'batch_size'}\n";
    script << "            }\n";
    script << "        )\n";
    script << "        \n";
    script << "        print(f'ONNX model saved to: {onnx_path}')\n";
    script << "        return True\n";
    script << "        \n";
    script << "    except Exception as e:\n";
    script << "        print(f'Error converting model: {e}')\n";
    script << "        print('Trying alternative approach...')\n";
    script << "        \n";
    script << "        try:\n";
    script << "            # Try with ultralytics import\n";
    script << "            from ultralytics import YOLO\n";
    script << "            \n";
    script << "            print('Loading model with Ultralytics YOLO...')\n";
    script << "            model = YOLO(pytorch_path)\n";
    script << "            \n";
    script << "            # Export to ONNX\n";
    script << "            print('Exporting to ONNX...')\n";
    script << "            model.export(format='onnx', imgsz=640)\n";
    script << "            \n";
    script << "            # Check if the ONNX file was created\n";
    script << "            onnx_path_alt = pytorch_path.replace('.pt', '.onnx')\n";
    script << "            if os.path.exists(onnx_path_alt):\n";
    script << "                # Move to our desired location\n";
    script << "                import shutil\n";
    script << "                shutil.move(onnx_path_alt, onnx_path)\n";
    script << "                print(f'ONNX model saved to: {onnx_path}')\n";
    script << "                return True\n";
    script << "            else:\n";
    script << "                print('ONNX export failed')\n";
    script << "                return False\n";
    script << "                \n";
    script << "        except ImportError:\n";
    script << "            print('Ultralytics not available. Please install it with: pip install ultralytics')\n";
    script << "            return False\n";
    script << "        except Exception as e2:\n";
    script << "            print(f'Alternative approach also failed: {e2}')\n";
    script << "            return False\n";
    script << "\n";
    script << "if __name__ == '__main__':\n";
    script << "    pytorch_path = '" << pytorchPath << "'\n";
    script << "    onnx_path = '" << onnxPath << "'\n";
    script << "    input_shape = [" << inputShape[0] << ", " << inputShape[1] 
           << ", " << inputShape[2] << ", " << inputShape[3] << "]\n";
    script << "    \n";
    script << "    success = convert_model(pytorch_path, onnx_path, input_shape)\n";
    script << "    sys.exit(0 if success else 1)\n";
    
    script.close();
    return scriptPath;
}

bool SimpleModelConverter::executePythonScript(const std::string& scriptPath) {
    std::string command = "python3 " + scriptPath;
    int result = std::system(command.c_str());
    return result == 0;
}

bool SimpleModelConverter::validateOnnxModel(const std::string& onnxPath) {
    try {
        std::ifstream file(onnxPath);
        if (!file.good()) {
            std::cerr << "ONNX file not found or not readable: " << onnxPath << std::endl;
            return false;
        }
        
        // Get file size
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.close();
        
        if (fileSize == 0) {
            std::cerr << "ONNX file is empty: " << onnxPath << std::endl;
            return false;
        }
        
        std::cout << "ONNX model validation passed. File size: " << fileSize << " bytes" << std::endl;
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "Error validating ONNX model: " << e.what() << std::endl;
        return false;
    }
}

std::string SimpleModelConverter::getOnnxPath(const std::string& pytorchPath) {
    size_t lastDot = pytorchPath.find_last_of('.');
    if (lastDot != std::string::npos) {
        return pytorchPath.substr(0, lastDot) + ".onnx";
    }
    return pytorchPath + ".onnx";
}

bool SimpleModelConverter::convertModel(const std::string& pytorchPath) {
    std::string onnxPath = getOnnxPath(pytorchPath);
    std::vector<int64_t> inputShape = {1, 3, 640, 640}; // Default YOLOv8 input shape
    
    return convertToOnnx(pytorchPath, onnxPath, inputShape);
}
