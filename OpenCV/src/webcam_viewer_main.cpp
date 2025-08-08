#include "webcam_viewer.h"
#include <iostream>
#include <string>
#include <getopt.h>

void printUsage(const char* programName)
{
    std::cout << "Webcam Viewer Program\n";
    std::cout << "======================\n\n";
    std::cout << "Usage: " << programName << " [OPTIONS]\n\n";
    std::cout << "Options:\n";
    std::cout << "  -c, --camera <index>     Camera index (default: 0)\n";
    std::cout << "  -w, --width <width>      Frame width (default: 640)\n";
    std::cout << "  -h, --height <height>    Frame height (default: 480)\n";
    std::cout << "  -t, --title <title>      Window title (default: Webcam Viewer)\n";
    std::cout << "  -r, --record <path>      Enable recording with output path\n";
    std::cout << "  -v, --help               Show this help message\n\n";
    std::cout << "Controls:\n";
    std::cout << "  ESC or Q: Quit\n";
    std::cout << "  R: Toggle recording\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << programName << " -c 0 -w 1280 -h 720\n";
    std::cout << "  " << programName << " -c 1 -t \"External Camera\"\n";
    std::cout << "  " << programName << " -r output.mp4\n\n";
}

int main(int argc, char* argv[])
{
    int cameraIndex = 0;
    int width = 640;
    int height = 480;
    std::string title = "Webcam Viewer";
    std::string recordPath = "";
    
    // Command line options
    static struct option long_options[] = {
        {"camera", required_argument, 0, 'c'},
        {"width", required_argument, 0, 'w'},
        {"height", required_argument, 0, 'h'},
        {"title", required_argument, 0, 't'},
        {"record", required_argument, 0, 'r'},
        {"help", no_argument, 0, 'v'},
        {0, 0, 0, 0}
    };
    
    int opt;
    int option_index = 0;
    
    while ((opt = getopt_long(argc, argv, "c:w:h:t:r:vh", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'c':
                cameraIndex = std::stoi(optarg);
                break;
            case 'w':
                width = std::stoi(optarg);
                break;
            case 'h':
                height = std::stoi(optarg);
                break;
            case 't':
                title = optarg;
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
    
    std::cout << "Webcam Viewer Program\n";
    std::cout << "======================\n\n";
    std::cout << "Camera Index: " << cameraIndex << "\n";
    std::cout << "Resolution: " << width << "x" << height << "\n";
    std::cout << "Window Title: " << title << "\n";
    if (!recordPath.empty()) {
        std::cout << "Recording: " << recordPath << "\n";
    }
    std::cout << "\n";
    
    // Create webcam viewer
    WebcamViewer viewer;
    
    // Set window title
    viewer.setWindowTitle(title);
    
    try {
        // Initialize webcam
        if (!viewer.initialize(cameraIndex, width, height)) {
            std::cerr << "Failed to initialize webcam. Exiting." << std::endl;
            std::cerr << "Please check if webcam is connected and accessible." << std::endl;
            return 1;
        }
        
        // Enable recording if specified
        if (!recordPath.empty()) {
            viewer.enableRecording(recordPath);
        }
        
        // Start stream
        if (!viewer.startStream()) {
            std::cerr << "Failed to start webcam stream. Exiting." << std::endl;
            return 1;
        }
        
        std::cout << "Webcam stream started. Press ESC or Q to quit." << std::endl;
        std::cout << "Press R to toggle recording." << std::endl;
        
        // Main loop
        while (viewer.isStreaming()) {
            // The stream loop handles everything in a separate thread
            // We just wait here for the stream to end
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        std::cout << "Webcam viewer closed." << std::endl;
        return 0;
        
    } catch (const cv::Exception& e) {
        std::cerr << "OpenCV error during execution: " << e.what() << std::endl;
        std::cerr << "This might be due to no webcam being connected or GUI issues." << std::endl;
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error during execution: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred during execution." << std::endl;
        return 1;
    }
}
