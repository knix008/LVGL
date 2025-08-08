#ifndef WEBCAM_VIEWER_H
#define WEBCAM_VIEWER_H

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <atomic>

class WebcamViewer
{
public:
    WebcamViewer();
    ~WebcamViewer();

    // Initialize webcam
    bool initialize(int cameraIndex = 0, int width = 640, int height = 480);
    
    // Start video stream
    bool startStream();
    
    // Stop video stream
    void stopStream();
    
    // Check if stream is running
    bool isStreaming() const { return isRunning; }
    
    // Get current frame
    cv::Mat getCurrentFrame() const;
    
    // Set window properties
    void setWindowTitle(const std::string& title);
    void setWindowSize(int width, int height);
    
    // Get webcam properties
    int getFrameWidth() const { return frameWidth; }
    int getFrameHeight() const { return frameHeight; }
    double getFPS() const { return currentFPS; }
    
    // Set recording options
    void enableRecording(const std::string& outputPath = "");
    void disableRecording();
    bool isRecording() const { return recordingEnabled; }

private:
    cv::VideoCapture cap;
    cv::Mat currentFrame;
    std::string windowTitle;
    int frameWidth;
    int frameHeight;
    double currentFPS;
    
    std::atomic<bool> isRunning;
    std::atomic<bool> recordingEnabled;
    std::string recordingPath;
    cv::VideoWriter videoWriter;
    
    std::thread streamThread;
    mutable std::mutex frameMutex;
    
    // Internal methods
    void streamLoop();
    void processFrame(cv::Mat& frame);
    void displayFrame(const cv::Mat& frame);
    void recordFrame(const cv::Mat& frame);
    void drawInfo(cv::Mat& frame);
};

#endif // WEBCAM_VIEWER_H
