#include "webcam_viewer_simple.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <thread>

WebcamViewerSimple::WebcamViewerSimple()
    : windowTitle("Webcam Viewer Simple")
    , frameWidth(640)
    , frameHeight(480)
    , currentFPS(0.0)
    , isRunning(false)
    , recordingEnabled(false)
{
}

WebcamViewerSimple::~WebcamViewerSimple()
{
    stopStream();
    if (cap.isOpened()) {
        cap.release();
    }
    if (videoWriter.isOpened()) {
        videoWriter.release();
    }
}

bool WebcamViewerSimple::initialize(const std::string& source, int width, int height)
{
    // Release previous capture if exists
    if (cap.isOpened()) {
        cap.release();
    }
    
    // Try to open as camera index first
    int cameraIndex = -1;
    try {
        cameraIndex = std::stoi(source);
    } catch (...) {
        // Not a number, treat as file path
    }
    
    if (cameraIndex >= 0) {
        // Open camera with V4L2 backend
        cap.open(cameraIndex, cv::CAP_V4L2);
        if (!cap.isOpened()) {
            std::cerr << "Error: Could not open camera at index " << cameraIndex << std::endl;
            std::cerr << "Trying to open as video file..." << std::endl;
            
            // Try to open as video file
            cap.open(source);
            if (!cap.isOpened()) {
                std::cerr << "Error: Could not open video file: " << source << std::endl;
                return false;
            }
        }
    } else {
        // Open as video file (let OpenCV pick backend)
        cap.open(source);
        if (!cap.isOpened()) {
            std::cerr << "Error: Could not open video file: " << source << std::endl;
            return false;
        }
    }
    
    // Set camera properties (only for camera, not for video files)
    if (cameraIndex >= 0) {
        // Prefer uncompressed live frames (YUYV) and low buffering for low latency
        cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('Y','U','Y','V'));
        cap.set(cv::CAP_PROP_BUFFERSIZE, 1);
        cap.set(cv::CAP_PROP_CONVERT_RGB, 1);

        cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
        cap.set(cv::CAP_PROP_FPS, 30.0);
    }
    
    // Get actual properties
    frameWidth = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    frameHeight = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    currentFPS = cap.get(cv::CAP_PROP_FPS);
    
    int fourcc = static_cast<int>(cap.get(cv::CAP_PROP_FOURCC));
    char fcc[5] = { static_cast<char>(fourcc & 0xFF), static_cast<char>((fourcc >> 8) & 0xFF), static_cast<char>((fourcc >> 16) & 0xFF), static_cast<char>((fourcc >> 24) & 0xFF), 0 };

    std::cout << "Video source initialized successfully:" << std::endl;
    std::cout << "  Resolution: " << frameWidth << "x" << frameHeight << std::endl;
    std::cout << "  FPS: " << currentFPS << std::endl;
    std::cout << "  FOURCC: " << fcc << std::endl;
    std::cout << "  Source: " << source << std::endl;
    
    return true;
}

bool WebcamViewerSimple::startStream()
{
    if (!cap.isOpened()) {
        std::cerr << "Error: Video source not initialized. Call initialize() first." << std::endl;
        return false;
    }
    
    if (isRunning) {
        std::cout << "Stream is already running." << std::endl;
        return true;
    }
    
    isRunning = true;
    streamLoop();
    
    std::cout << "Video stream started." << std::endl;
    return true;
}

void WebcamViewerSimple::stopStream()
{
    if (!isRunning) {
        return;
    }
    
    isRunning = false;
    
    if (videoWriter.isOpened()) {
        videoWriter.release();
        recordingEnabled = false;
    }
    
    std::cout << "Video stream stopped." << std::endl;
}

cv::Mat WebcamViewerSimple::getCurrentFrame() const
{
    return currentFrame.clone();
}

void WebcamViewerSimple::setWindowTitle(const std::string& title)
{
    windowTitle = title;
}

void WebcamViewerSimple::setWindowSize(int width, int height)
{
    frameWidth = width;
    frameHeight = height;
}

void WebcamViewerSimple::enableRecording(const std::string& outputPath)
{
    if (outputPath.empty()) {
        // Generate default filename with timestamp
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << "video_recording_" << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S") << ".mp4";
        recordingPath = ss.str();
    } else {
        recordingPath = outputPath;
    }
    
    // Initialize video writer
    int fourcc = cv::VideoWriter::fourcc('m', 'p', '4', 'v');
    videoWriter.open(recordingPath, fourcc, currentFPS, cv::Size(frameWidth, frameHeight));
    
    if (!videoWriter.isOpened()) {
        std::cerr << "Error: Could not open video writer for recording." << std::endl;
        return;
    }
    
    recordingEnabled = true;
    std::cout << "Recording enabled. Output: " << recordingPath << std::endl;
}

void WebcamViewerSimple::disableRecording()
{
    if (videoWriter.isOpened()) {
        videoWriter.release();
    }
    recordingEnabled = false;
    std::cout << "Recording disabled." << std::endl;
}

void WebcamViewerSimple::streamLoop()
{
    cv::Mat frame;
    auto startTime = std::chrono::high_resolution_clock::now();
    int frameCount = 0;
    int savedFrameCount = 0;
    int consecutiveFailures = 0;
    const int maxFailures = 10; // Maximum consecutive failures before giving up
    
    std::cout << "Starting video stream loop..." << std::endl;
    std::cout << "Press Ctrl+C to stop" << std::endl;
    
    while (isRunning) {
        try {
            // Check if camera is still opened
            if (!cap.isOpened()) {
                std::cerr << "Error: Video source connection lost." << std::endl;
                break;
            }
            
            // Capture frame
            if (!cap.read(frame)) {
                consecutiveFailures++;
                std::cerr << "Warning: Could not read frame from video source. Attempt " 
                          << consecutiveFailures << "/" << maxFailures << std::endl;
                
                if (consecutiveFailures >= maxFailures) {
                    std::cerr << "Error: Too many consecutive failures. Stopping stream." << std::endl;
                    break;
                }
                
                // Wait a bit before trying again
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
                continue;
            }
            
            // Reset failure counter on successful read
            consecutiveFailures = 0;
            
            if (frame.empty()) {
                std::cerr << "Warning: Received empty frame." << std::endl;
                continue;
            }
            
            // Process frame
            processFrame(frame);
            
            // Update current frame
            currentFrame = frame.clone();
            
            // Display frame info (without GUI)
            displayFrame(frame);
            
            // Record frame if enabled
            if (recordingEnabled && videoWriter.isOpened()) {
                recordFrame(frame);
            }
            
            // Save frame as image every 30 frames (1 second at 30fps)
            if (frameCount % 30 == 0) {
                saveFrameAsImage(frame, savedFrameCount++);
            }
            
            // Calculate FPS
            frameCount++;
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime);
            
            if (elapsed.count() >= 1) {
                currentFPS = static_cast<double>(frameCount) / elapsed.count();
                frameCount = 0;
                startTime = currentTime;
            }
            
            // Small delay to control frame rate
            std::this_thread::sleep_for(std::chrono::milliseconds(33)); // ~30 FPS
            
        } catch (const cv::Exception& e) {
            std::cerr << "OpenCV error in stream loop: " << e.what() << std::endl;
            consecutiveFailures++;
            
            if (consecutiveFailures >= maxFailures) {
                std::cerr << "Error: Too many OpenCV errors. Stopping stream." << std::endl;
                break;
            }
            
            // Wait before retrying
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
        } catch (const std::exception& e) {
            std::cerr << "Unexpected error in stream loop: " << e.what() << std::endl;
            break;
        }
    }
    
    std::cout << "Video stream loop ended." << std::endl;
}

void WebcamViewerSimple::processFrame(cv::Mat& frame)
{
    try {
        // Check if frame is valid
        if (frame.empty()) {
            return;
        }
        
        // Add any frame processing here
        // For example: resize, filter, detect objects, etc.
        
        // Optional: Resize frame if needed
        if (frame.cols != frameWidth || frame.rows != frameHeight) {
            cv::resize(frame, frame, cv::Size(frameWidth, frameHeight));
        }
        
    } catch (const cv::Exception& e) {
        std::cerr << "OpenCV error in processFrame: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error in processFrame: " << e.what() << std::endl;
    }
}

void WebcamViewerSimple::displayFrame(const cv::Mat& frame)
{
    try {
        // Check if frame is valid
        if (frame.empty()) {
            return;
        }
        
        // Instead of displaying with GUI, print frame info
        static int frameCounter = 0;
        frameCounter++;
        
        if (frameCounter % 30 == 0) { // Print every 30 frames
            std::cout << "\rFrame: " << frameCounter 
                      << " | FPS: " << std::fixed << std::setprecision(1) << currentFPS
                      << " | Size: " << frame.cols << "x" << frame.rows
                      << " | Recording: " << (recordingEnabled ? "ON" : "OFF")
                      << std::flush;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error in displayFrame: " << e.what() << std::endl;
    }
}

void WebcamViewerSimple::recordFrame(const cv::Mat& frame)
{
    try {
        if (videoWriter.isOpened() && !frame.empty()) {
            videoWriter.write(frame);
        }
    } catch (const cv::Exception& e) {
        std::cerr << "OpenCV error in recordFrame: " << e.what() << std::endl;
        // Disable recording on error
        disableRecording();
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error in recordFrame: " << e.what() << std::endl;
        // Disable recording on error
        disableRecording();
    }
}

void WebcamViewerSimple::drawInfo(cv::Mat& frame)
{
    // Draw FPS
    std::string fpsText = "FPS: " + std::to_string(static_cast<int>(currentFPS));
    cv::putText(frame, fpsText, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
    
    // Draw resolution
    std::string resText = "Resolution: " + std::to_string(frameWidth) + "x" + std::to_string(frameHeight);
    cv::putText(frame, resText, cv::Point(10, 60), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
    
    // Draw recording status
    if (recordingEnabled) {
        cv::putText(frame, "REC", cv::Point(frameWidth - 80, 30), cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 0, 255), 2);
        cv::circle(frame, cv::Point(frameWidth - 30, 20), 8, cv::Scalar(0, 0, 255), -1);
    }
}

void WebcamViewerSimple::saveFrameAsImage(const cv::Mat& frame, int frameNumber)
{
    try {
        // Check if frame is valid
        if (frame.empty()) {
            return;
        }
        
        // Create output directory if it doesn't exist
        std::filesystem::create_directories("output_frames");
        
        // Save frame as image
        std::string filename = "output_frames/frame_" + std::to_string(frameNumber) + ".jpg";
        cv::imwrite(filename, frame);
        
        if (frameNumber % 10 == 0) { // Print every 10 saved frames
            std::cout << "\nSaved frame " << frameNumber << " to " << filename << std::endl;
        }
        
    } catch (const cv::Exception& e) {
        std::cerr << "OpenCV error in saveFrameAsImage: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error in saveFrameAsImage: " << e.what() << std::endl;
    }
}
