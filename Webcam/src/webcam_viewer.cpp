#include "webcam_viewer.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <chrono>
#include <iomanip>
#include <sstream>

WebcamViewer::WebcamViewer()
    : windowTitle("Webcam Viewer")
    , frameWidth(640)
    , frameHeight(480)
    , currentFPS(0.0)
    , isRunning(false)
    , recordingEnabled(false)
{
}

WebcamViewer::~WebcamViewer()
{
    try {
        stopStream();
        if (cap.isOpened()) {
            cap.release();
        }
        if (videoWriter.isOpened()) {
            videoWriter.release();
        }
        
        // Only destroy windows if they were actually created
        // This prevents crashes when no webcam is available
        cv::destroyAllWindows();
        
    } catch (const cv::Exception& e) {
        std::cerr << "OpenCV error in destructor: " << e.what() << std::endl;
        // Don't rethrow in destructor
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error in destructor: " << e.what() << std::endl;
        // Don't rethrow in destructor
    } catch (...) {
        std::cerr << "Unknown error in destructor." << std::endl;
        // Don't rethrow in destructor
    }
}

bool WebcamViewer::initialize(int cameraIndex, int width, int height)
{
    try {
        // Release previous capture if exists
        if (cap.isOpened()) {
            cap.release();
        }
        
        // Open camera with V4L2 backend
        cap.open(cameraIndex, cv::CAP_V4L2);
        if (!cap.isOpened()) {
            std::cerr << "Error: Could not open camera at index " << cameraIndex << std::endl;
            return false;
        }
        
        // Prefer uncompressed live frames (YUYV) and low buffering for low latency
        cap.set(cv::CAP_PROP_FOURCC, cv::VideoWriter::fourcc('Y','U','Y','V'));
        cap.set(cv::CAP_PROP_BUFFERSIZE, 1);
        cap.set(cv::CAP_PROP_CONVERT_RGB, 1);

        // Set camera properties
        cap.set(cv::CAP_PROP_FRAME_WIDTH, width);
        cap.set(cv::CAP_PROP_FRAME_HEIGHT, height);
        cap.set(cv::CAP_PROP_FPS, 30.0);
        
        // Get actual properties
        frameWidth = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
        frameHeight = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
        currentFPS = cap.get(cv::CAP_PROP_FPS);
        int fourcc = static_cast<int>(cap.get(cv::CAP_PROP_FOURCC));
        char fcc[5] = { static_cast<char>(fourcc & 0xFF), static_cast<char>((fourcc >> 8) & 0xFF), static_cast<char>((fourcc >> 16) & 0xFF), static_cast<char>((fourcc >> 24) & 0xFF), 0 };
        
        std::cout << "Webcam initialized successfully:" << std::endl;
        std::cout << "  Resolution: " << frameWidth << "x" << frameHeight << std::endl;
        std::cout << "  FPS: " << currentFPS << std::endl;
        std::cout << "  FOURCC: " << fcc << std::endl;
        
        return true;
        
    } catch (const cv::Exception& e) {
        std::cerr << "OpenCV error during initialization: " << e.what() << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error during initialization: " << e.what() << std::endl;
        return false;
    } catch (...) {
        std::cerr << "Unknown error during initialization." << std::endl;
        return false;
    }
}

bool WebcamViewer::startStream()
{
    if (!cap.isOpened()) {
        std::cerr << "Error: Camera not initialized. Call initialize() first." << std::endl;
        return false;
    }
    
    if (isRunning) {
        std::cout << "Stream is already running." << std::endl;
        return true;
    }
    
    isRunning = true;
    streamThread = std::thread(&WebcamViewer::streamLoop, this);
    
    std::cout << "Webcam stream started." << std::endl;
    return true;
}

void WebcamViewer::stopStream()
{
    if (!isRunning) {
        return;
    }
    
    isRunning = false;
    
    if (streamThread.joinable()) {
        streamThread.join();
    }
    
    if (videoWriter.isOpened()) {
        videoWriter.release();
        recordingEnabled = false;
    }
    
    std::cout << "Webcam stream stopped." << std::endl;
}

cv::Mat WebcamViewer::getCurrentFrame() const
{
    std::lock_guard<std::mutex> lock(frameMutex);
    return currentFrame.clone();
}

void WebcamViewer::setWindowTitle(const std::string& title)
{
    windowTitle = title;
}

void WebcamViewer::setWindowSize(int width, int height)
{
    frameWidth = width;
    frameHeight = height;
}

void WebcamViewer::enableRecording(const std::string& outputPath)
{
    if (outputPath.empty()) {
        // Generate default filename with timestamp
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << "webcam_recording_" << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S") << ".mp4";
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

void WebcamViewer::disableRecording()
{
    if (videoWriter.isOpened()) {
        videoWriter.release();
    }
    recordingEnabled = false;
    std::cout << "Recording disabled." << std::endl;
}

void WebcamViewer::streamLoop()
{
    cv::Mat frame;
    auto startTime = std::chrono::high_resolution_clock::now();
    int frameCount = 0;
    int consecutiveFailures = 0;
    const int maxFailures = 10; // Maximum consecutive failures before giving up
    
    std::cout << "Starting video stream loop..." << std::endl;
    
    while (isRunning) {
        try {
            // Check if camera is still opened
            if (!cap.isOpened()) {
                std::cerr << "Error: Camera connection lost." << std::endl;
                break;
            }
            
            // Capture frame
            if (!cap.read(frame)) {
                consecutiveFailures++;
                std::cerr << "Warning: Could not read frame from camera. Attempt " 
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
            {
                std::lock_guard<std::mutex> lock(frameMutex);
                currentFrame = frame.clone();
            }
            
            // Display frame
            displayFrame(frame);
            
            // Record frame if enabled
            if (recordingEnabled && videoWriter.isOpened()) {
                recordFrame(frame);
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
            
            // Check for key press
            int key = cv::waitKey(1);
            if (key == 27) { // ESC key
                std::cout << "ESC key pressed. Stopping stream." << std::endl;
                break;
            } else if (key == 'r' || key == 'R') {
                // Toggle recording
                if (recordingEnabled) {
                    disableRecording();
                } else {
                    enableRecording();
                }
            } else if (key == 'q' || key == 'Q') {
                // Quit
                std::cout << "Q key pressed. Stopping stream." << std::endl;
                break;
            }
            
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

void WebcamViewer::processFrame(cv::Mat& frame)
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

void WebcamViewer::displayFrame(const cv::Mat& frame)
{
    try {
        cv::Mat displayFrame = frame.clone();
        
        // Draw information overlay
        drawInfo(displayFrame);
        
        // Display frame
        cv::imshow(windowTitle, displayFrame);
        
    } catch (const cv::Exception& e) {
        std::cerr << "OpenCV error in displayFrame: " << e.what() << std::endl;
        // Don't break the stream for display errors
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error in displayFrame: " << e.what() << std::endl;
        // Don't break the stream for display errors
    }
}

void WebcamViewer::recordFrame(const cv::Mat& frame)
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

void WebcamViewer::drawInfo(cv::Mat& frame)
{
    try {
        // Check if frame is valid
        if (frame.empty()) {
            return;
        }
        
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
        
        // Draw instructions
        cv::putText(frame, "ESC/Q: Quit, R: Toggle Recording", cv::Point(10, frameHeight - 20), 
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);
                    
    } catch (const cv::Exception& e) {
        std::cerr << "OpenCV error in drawInfo: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Unexpected error in drawInfo: " << e.what() << std::endl;
    }
}
