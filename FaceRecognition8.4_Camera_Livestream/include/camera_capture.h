#ifndef CAMERA_CAPTURE_H
#define CAMERA_CAPTURE_H

#include <opencv2/opencv.hpp>
#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <vector>
#include <string>

class CameraCapture {
public:
    CameraCapture();
    ~CameraCapture();

    // Find available camera devices by scanning a specified range
    // Scans from min_index to max_index (inclusive)
    static std::vector<int> find_available_cameras(int min_index = 0, int max_index = 15);

    // Initialize camera with specific device index
    bool initialize(int camera_index);

    // Initialize camera by scanning and auto-selecting first available in range
    bool initialize_auto(int min_index = 0, int max_index = 15);

    // Start capturing frames in background thread
    bool start_capture();

    // Stop capturing frames
    void stop_capture();

    // Capture a single frame (blocking if no frame available)
    bool capture_frame(cv::Mat& frame);

    // Get latest available frame without blocking
    bool get_latest_frame(cv::Mat& frame);

    // Check if camera is currently capturing
    bool is_capturing() const { return is_capturing_flag; }

    // Check if camera is initialized
    bool is_initialized() const { return camera.isOpened(); }

    // Set camera resolution
    bool set_resolution(int width, int height);

    // Set camera frame rate (FPS)
    bool set_fps(int fps);

    // Get current resolution
    void get_resolution(int& width, int& height) const;

    // Release camera resources
    void release();

private:
    cv::VideoCapture camera;
    std::thread capture_thread;
    std::mutex frame_mutex;
    std::atomic<bool> is_capturing_flag;
    std::atomic<bool> should_stop;
    cv::Mat current_frame;
    cv::Mat latest_frame;

    // Background capture loop
    void capture_loop();
};

#endif // CAMERA_CAPTURE_H
