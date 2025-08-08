#include "webcam_viewer_simple.h"
#include <iostream>
#include <string>
#include <getopt.h>
#include <signal.h>

// Global variable to handle Ctrl+C
WebcamViewerSimple* g_viewer = nullptr;

void signalHandler(int signum) {
    if (g_viewer) {
        std::cout << "\nReceived signal " << signum << ". Stopping stream..." << std::endl;
        g_viewer->stopStream();
    }
    exit(signum);
}

void printUsage(const char* programName)
{
    std::cout << "Simple Webcam Viewer Program\n";
    std::cout << "============================\n\n";
    std::cout << "Usage: " << programName << " [OPTIONS]\n\n";
    std::cout << "Options:\n";
    std::cout << "  -s, --source <source>    Video source (camera index or video file)\n";
    std::cout << "  -w, --width <width>      Frame width (default: 640)\n";
    std::cout << "  -h, --height <height>    Frame height (default: 480)\n";
    std::cout << "  -r, --record <path>      Enable recording with output path\n";
    std::cout << "  -v, --help               Show this help message\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << programName << " -s 0                    # Use camera 0\n";
    std::cout << "  " << programName << " -s ../data/traffic.mp4   # Use video file\n";
    std::cout << "  " << programName << " -s 0 -w 1280 -h 720     # High resolution\n";
    std::cout << "  " << programName << " -s 0 -r output.mp4       # Record camera\n";
    std::cout << "  " << programName << " -s ../data/traffic.mp4 -r processed.mp4  # Process video file\n\n";
}

int main(int argc, char* argv[])
{
    std::string source = "0";  // Default to camera 0
    int width = 640;
    int height = 480;
    std::string recordPath = "";
    
    // Command line options
    static struct option long_options[] = {
        {"source", required_argument, 0, 's'},
        {"width", required_argument, 0, 'w'},
        {"height", required_argument, 0, 'h'},
        {"record", required_argument, 0, 'r'},
        {"help", no_argument, 0, 'v'},
        {0, 0, 0, 0}
    };
    
    int opt;
    int option_index = 0;
    
    while ((opt = getopt_long(argc, argv, "s:w:h:r:vh", long_options, &option_index)) != -1) {
        switch (opt) {
            case 's':
                source = optarg;
                break;
            case 'w':
                width = std::stoi(optarg);
                break;
            case 'h':
                height = std::stoi(optarg);
                break;
            case 'r':
                recordPath = optarg;
                break;
            case 'v':
                printUsage(argv[0]);
                return 0;
            default:
                printUsage(argv[0]);
                return 1;
        }
    }
    
    std::cout << "Simple Webcam Viewer Program\n";
    std::cout << "============================\n\n";
    std::cout << "Source: " << source << "\n";
    std::cout << "Resolution: " << width << "x" << height << "\n";
    if (!recordPath.empty()) {
        std::cout << "Recording: " << recordPath << "\n";
    }
    std::cout << "\n";
    
    // Create webcam viewer
    WebcamViewerSimple viewer;
    g_viewer = &viewer;
    
    // Set up signal handler for Ctrl+C
    signal(SIGINT, signalHandler);
    
    try {
        // Initialize video source
        if (!viewer.initialize(source, width, height)) {
            std::cerr << "Failed to initialize video source. Exiting." << std::endl;
            return 1;
        }
        
        // Enable recording if specified
        if (!recordPath.empty()) {
            viewer.enableRecording(recordPath);
        }
        
        // Start stream
        if (!viewer.startStream()) {
            std::cerr << "Failed to start video stream. Exiting." << std::endl;
            return 1;
        }
        
        std::cout << "Video stream completed." << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error during execution: " << e.what() << std::endl;
        return 1;
    }
}
