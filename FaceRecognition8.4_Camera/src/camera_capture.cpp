#include "camera_capture.h"
#include <iostream>
#include <filesystem>

CameraCapture::CameraCapture()
    : is_capturing_flag(false), should_stop(false) {
}

CameraCapture::~CameraCapture() {
    release();
}

std::vector<int> CameraCapture::find_available_cameras(int min_index, int max_index) {
    std::vector<int> available_cameras;

    std::cout << "Scanning for available camera devices (range: "
              << min_index << "-" << max_index << ")..." << std::endl;

    // Check for /dev/video* devices in specified range
    for (int i = min_index; i <= max_index; i++) {
        std::string device_path = "/dev/video" + std::to_string(i);

        // Check if device exists
        if (std::filesystem::exists(device_path)) {
            // Try to open it with OpenCV to verify it's a valid video device
            cv::VideoCapture test_camera;
            test_camera.open(i, cv::CAP_V4L2);

            if (test_camera.isOpened()) {
                available_cameras.push_back(i);
                std::cout << "  Found camera at " << device_path << " (index: " << i << ")" << std::endl;
                test_camera.release();
            }
        }
    }

    if (available_cameras.empty()) {
        std::cout << "  No camera devices found in range " << min_index << "-" << max_index << std::endl;
    } else {
        std::cout << "  Total cameras found: " << available_cameras.size() << std::endl;
    }

    return available_cameras;
}

bool CameraCapture::initialize(int camera_index) {
    if (camera.isOpened()) {
        camera.release();
    }

    if (camera_index < 0) {
        std::cerr << "Error: Invalid camera index " << camera_index << std::endl;
        return false;
    }

    std::string device_path = "/dev/video" + std::to_string(camera_index);

    // Check if device exists
    if (!std::filesystem::exists(device_path)) {
        std::cerr << "Error: Camera device not found at " << device_path << std::endl;
        return false;
    }

    // Open camera with V4L2 backend for better compatibility on Linux
    camera.open(camera_index, cv::CAP_V4L2);

    if (!camera.isOpened()) {
        std::cerr << "Error: Could not open camera at " << device_path << std::endl;
        return false;
    }

    // Set default resolution to 640x480
    set_resolution(640, 480);

    // Set default FPS to 30
    set_fps(30);

    // Reduce latency by using smaller buffer
    camera.set(cv::CAP_PROP_BUFFERSIZE, 1);

    std::cout << "Camera initialized successfully at " << device_path << std::endl;
    return true;
}

bool CameraCapture::initialize_auto(int min_index, int max_index) {
    std::cout << "Attempting to auto-detect camera in range " << min_index << "-" << max_index << std::endl;

    auto available = find_available_cameras(min_index, max_index);

    if (available.empty()) {
        std::cerr << "Error: No camera devices found in range " << min_index << "-" << max_index << std::endl;
        return false;
    }

    int camera_to_use = available[0];
    std::cout << "Auto-selected camera: /dev/video" << camera_to_use << std::endl;

    return initialize(camera_to_use);
}

bool CameraCapture::start_capture() {
    if (!camera.isOpened()) {
        std::cerr << "Error: Camera not initialized" << std::endl;
        return false;
    }

    if (is_capturing_flag) {
        return true;  // Already capturing
    }

    should_stop = false;
    is_capturing_flag = true;

    // Start capture thread
    capture_thread = std::thread(&CameraCapture::capture_loop, this);

    std::cout << "Camera capture started" << std::endl;
    return true;
}

void CameraCapture::stop_capture() {
    if (!is_capturing_flag) {
        return;
    }

    should_stop = true;
    is_capturing_flag = false;

    // Wait for capture thread to finish
    if (capture_thread.joinable()) {
        capture_thread.join();
    }

    std::cout << "Camera capture stopped" << std::endl;
}

bool CameraCapture::capture_frame(cv::Mat& frame) {
    std::lock_guard<std::mutex> lock(frame_mutex);

    if (current_frame.empty()) {
        return false;
    }

    frame = current_frame.clone();
    return true;
}

bool CameraCapture::get_latest_frame(cv::Mat& frame) {
    std::lock_guard<std::mutex> lock(frame_mutex);

    if (latest_frame.empty()) {
        return false;
    }

    frame = latest_frame.clone();
    return true;
}

bool CameraCapture::set_resolution(int width, int height) {
    if (!camera.isOpened()) {
        return false;
    }

    camera.set(cv::CAP_PROP_FRAME_WIDTH, width);
    camera.set(cv::CAP_PROP_FRAME_HEIGHT, height);

    return true;
}

bool CameraCapture::set_fps(int fps) {
    if (!camera.isOpened()) {
        return false;
    }

    return camera.set(cv::CAP_PROP_FPS, fps);
}

void CameraCapture::get_resolution(int& width, int& height) const {
    if (!camera.isOpened()) {
        width = 0;
        height = 0;
        return;
    }

    width = static_cast<int>(camera.get(cv::CAP_PROP_FRAME_WIDTH));
    height = static_cast<int>(camera.get(cv::CAP_PROP_FRAME_HEIGHT));
}

void CameraCapture::release() {
    stop_capture();

    if (camera.isOpened()) {
        camera.release();
    }

    std::cout << "Camera resources released" << std::endl;
}

void CameraCapture::capture_loop() {
    cv::Mat frame;

    while (!should_stop) {
        if (camera.read(frame)) {
            {
                std::lock_guard<std::mutex> lock(frame_mutex);
                current_frame = frame.clone();
                latest_frame = frame.clone();
            }
        } else {
            std::cerr << "Error: Failed to read frame from camera" << std::endl;
            should_stop = true;
        }
    }
}
