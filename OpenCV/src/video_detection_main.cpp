// Ultralytics 🚀 AGPL-3.0 License - https://ultralytics.com/license

#include <iostream>
#include <string>
#include <getopt.h>

#include <opencv2/opencv.hpp>

#include "yolov8n_video_detector.h"

void printUsage(const char* programName) {
    std::cout << "Usage: " << programName << " [OPTIONS]\n"
              << "Options:\n"
              << "  -v, --video PATH       Process video file\n"
              << "  -o, --output PATH      Output video file path\n"
              << "  -w, --webcam INDEX     Use webcam (default: 0)\n"
              << "  -m, --model PATH       YOLOv8n model path (default: ../models/yolov8n.onnx)\n"
              << "  -c, --cuda             Enable CUDA acceleration\n"
              << "  -f, --fps              Show FPS counter\n"
              << "  -s, --save             Save output video\n"
              << "  -h, --help             Show this help message\n"
              << "\nExamples:\n"
              << "  " << programName << " -v ../data/traffic.mp4 -o output.mp4\n"
              << "  " << programName << " -w 0 -s -o webcam_output.mp4\n"
              << "  " << programName << " -v ../data/traffic.mp4 -c -f\n"
              << std::endl;
}

int main(int argc, char** argv) {
    std::string videoPath = "";
    std::string outputPath = "";
    std::string modelPath = "../models/yolov8n_onnx";
    bool useWebcam = false;
    bool useCuda = false;
    bool showFPS = true;
    bool saveOutput = false;
    int cameraIndex = 0;

    // Command line options
    static struct option long_options[] = {
        {"video",    required_argument, 0, 'v'},
        {"output",   required_argument, 0, 'o'},
        {"webcam",   optional_argument, 0, 'w'},
        {"model",    required_argument, 0, 'm'},
        {"cuda",     no_argument,       0, 'c'},
        {"fps",      no_argument,       0, 'f'},
        {"save",     no_argument,       0, 's'},
        {"help",     no_argument,       0, 'h'},
        {0, 0, 0, 0}
    };

    int option_index = 0;
    int c;

    while ((c = getopt_long(argc, argv, "v:o:w::m:cfsh", long_options, &option_index)) != -1) {
        switch (c) {
            case 'v':
                videoPath = optarg;
                break;
            case 'o':
                outputPath = optarg;
                break;
            case 'w':
                useWebcam = true;
                if (optarg) {
                    cameraIndex = std::stoi(optarg);
                }
                break;
            case 'm':
                modelPath = optarg;
                break;
            case 'c':
                useCuda = true;
                break;
            case 'f':
                showFPS = true;
                break;
            case 's':
                saveOutput = true;
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
    if (videoPath.empty() && !useWebcam) {
        std::cerr << "Error: Must specify either video file (-v) or webcam (-w)" << std::endl;
        printUsage(argv[0]);
        return 1;
    }

    if (saveOutput && outputPath.empty()) {
        if (useWebcam) {
            outputPath = "webcam_output.mp4";
        } else {
            // Generate output path from input video path
            size_t lastDot = videoPath.find_last_of('.');
            if (lastDot != std::string::npos) {
                outputPath = videoPath.substr(0, lastDot) + "_detected.mp4";
            } else {
                outputPath = videoPath + "_detected.mp4";
            }
        }
    }

    std::cout << "YOLOv8n Video Detection" << std::endl;
    std::cout << "========================" << std::endl;
    std::cout << "Model path: " << modelPath << std::endl;
    std::cout << "CUDA enabled: " << (useCuda ? "Yes" : "No") << std::endl;
    std::cout << "Show FPS: " << (showFPS ? "Yes" : "No") << std::endl;
    std::cout << "Save output: " << (saveOutput ? "Yes" : "No") << std::endl;
    
    if (useWebcam) {
        std::cout << "Camera index: " << cameraIndex << std::endl;
    } else {
        std::cout << "Video file: " << videoPath << std::endl;
    }
    
    if (saveOutput) {
        std::cout << "Output file: " << outputPath << std::endl;
    }
    std::cout << std::endl;

    try {
        // Initialize video detector
        YOLOv8nVideoDetector detector(modelPath, cv::Size(640, 640), "", useCuda);

        bool success = false;
        
        if (useWebcam) {
            std::cout << "Starting webcam detection..." << std::endl;
            success = detector.processWebcam(cameraIndex, showFPS, saveOutput, outputPath);
        } else {
            std::cout << "Starting video file processing..." << std::endl;
            success = detector.processVideoFile(videoPath, outputPath, showFPS, saveOutput);
        }

        if (success) {
            std::cout << "\nProcessing completed successfully!" << std::endl;
            std::cout << "Total frames processed: " << detector.getTotalFramesProcessed() << std::endl;
            std::cout << "Total processing time: " << detector.getTotalProcessingTime() << " seconds" << std::endl;
            if (detector.getTotalFramesProcessed() > 0) {
                std::cout << "Average FPS: " << detector.getTotalFramesProcessed() / detector.getTotalProcessingTime() << std::endl;
            }
        } else {
            std::cerr << "Processing failed!" << std::endl;
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
