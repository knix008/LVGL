#ifndef LIVE_STREAM_MANAGER_H
#define LIVE_STREAM_MANAGER_H

#include <opencv2/opencv.hpp>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <memory>
#include "common.h"

// Forward declarations
class CameraCapture;
class FaceDetector;
class FaceRecognizer;
class FaceDatabase;

// Structure to hold annotated frame with detection and recognition results
struct AnnotatedFrame {
    cv::Mat frame;  // The video frame
    std::vector<Face> detected_faces;  // Detected faces with recognition results
    long long timestamp_ms;  // Frame timestamp in milliseconds
    bool has_faces;  // Whether any faces were detected
};

class LiveStreamManager {
public:
    LiveStreamManager(CameraCapture* camera, FaceDetector* detector,
                      FaceRecognizer* recognizer, FaceDatabase* database);
    ~LiveStreamManager();

    // Start live stream processing
    bool start_stream();

    // Stop live stream processing
    void stop_stream();

    // Check if stream is currently running
    bool is_streaming() const { return is_streaming_flag; }

    // Get the latest annotated frame (non-blocking)
    bool get_latest_frame(AnnotatedFrame& frame);

    // Set detection parameters
    void set_detection_enabled(bool enabled) { detection_enabled = enabled; }
    void set_recognition_enabled(bool enabled) { recognition_enabled = enabled; }
    void set_min_face_size(int width, int height);

    // Set confidence thresholds
    void set_detection_confidence(float threshold);
    void set_recognition_confidence(float threshold);

    // Get performance statistics
    struct PerformanceStats {
        int fps;  // Current frames per second
        float detection_time_ms;  // Average detection time
        float recognition_time_ms;  // Average recognition time
        int detected_faces_count;  // Number of faces in last frame
    };
    PerformanceStats get_performance_stats() const;

    // Get camera resolution
    void get_resolution(int& width, int& height) const;

private:
    // References to core components
    CameraCapture* camera;
    FaceDetector* detector;
    FaceRecognizer* recognizer;
    FaceDatabase* database;

    // Thread management
    std::thread processing_thread;
    std::atomic<bool> is_streaming_flag;
    std::atomic<bool> should_stop;

    // Frame synchronization
    std::mutex frame_mutex;
    AnnotatedFrame latest_frame;
    bool frame_ready;

    // Processing flags
    std::atomic<bool> detection_enabled;
    std::atomic<bool> recognition_enabled;

    // Performance tracking
    mutable std::mutex stats_mutex;
    PerformanceStats current_stats;
    int frame_count;
    std::chrono::high_resolution_clock::time_point last_fps_update;

    // Processing parameters
    int min_face_width;
    int min_face_height;
    float detection_confidence_threshold;
    float recognition_confidence_threshold;

    // Main processing loop
    void process_stream();

    // Process a single frame
    void process_frame(cv::Mat& frame, AnnotatedFrame& annotated);

    // Detect faces in frame and perform recognition
    std::vector<Face> detect_and_recognize_faces(const cv::Mat& frame);

    // Draw face annotations on frame
    void draw_face_annotations(cv::Mat& frame, const std::vector<Face>& faces);

    // Update FPS counter
    void update_fps_counter();
};

#endif // LIVE_STREAM_MANAGER_H
