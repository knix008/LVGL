#include "live_stream_manager.h"
#include "camera_capture.h"
#include "face_detector.h"
#include "face_recognizer.h"
#include "face_database.h"
#include <iostream>
#include <chrono>

LiveStreamManager::LiveStreamManager(CameraCapture* camera, FaceDetector* detector,
                                     FaceRecognizer* recognizer, FaceDatabase* database)
    : camera(camera), detector(detector), recognizer(recognizer), database(database),
      is_streaming_flag(false), should_stop(false), frame_ready(false),
      detection_enabled(true), recognition_enabled(true),
      frame_count(0), min_face_width(30), min_face_height(30),
      detection_confidence_threshold(0.5f), recognition_confidence_threshold(0.6f) {

    if (detector) {
        detector->set_min_face_size(min_face_width, min_face_height);
    }
}

LiveStreamManager::~LiveStreamManager() {
    stop_stream();
}

bool LiveStreamManager::start_stream() {
    if (is_streaming_flag) {
        std::cerr << "Stream is already running" << std::endl;
        return false;
    }

    if (!camera || !detector || !recognizer) {
        std::cerr << "Required components not initialized" << std::endl;
        return false;
    }

    if (!camera->is_capturing()) {
        std::cerr << "Camera is not capturing" << std::endl;
        return false;
    }

    is_streaming_flag = true;
    should_stop = false;
    frame_count = 0;
    last_fps_update = std::chrono::high_resolution_clock::now();
    {
        std::lock_guard<std::mutex> lock(recognized_persons_mutex);
        recognized_persons_in_session.clear();
    }

    try {
        processing_thread = std::thread(&LiveStreamManager::process_stream, this);
        std::cout << "Live stream started successfully" << std::endl;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to start stream: " << e.what() << std::endl;
        is_streaming_flag = false;
        return false;
    }
}

void LiveStreamManager::stop_stream() {
    if (!is_streaming_flag) {
        return;
    }

    should_stop = true;
    is_streaming_flag = false;

    if (processing_thread.joinable()) {
        processing_thread.join();
    }

    std::cout << "Live stream stopped" << std::endl;
}

bool LiveStreamManager::get_latest_frame(AnnotatedFrame& frame) {
    std::lock_guard<std::mutex> lock(frame_mutex);
    if (!frame_ready) {
        return false;
    }
    frame = latest_frame;
    return true;
}

void LiveStreamManager::set_min_face_size(int width, int height) {
    min_face_width = width;
    min_face_height = height;
    if (detector) {
        detector->set_min_face_size(width, height);
    }
}

void LiveStreamManager::set_detection_confidence(float threshold) {
    detection_confidence_threshold = std::clamp(threshold, 0.0f, 1.0f);
}

void LiveStreamManager::set_recognition_confidence(float threshold) {
    recognition_confidence_threshold = std::clamp(threshold, 0.0f, 1.0f);
    if (recognizer) {
        recognizer->set_confidence_threshold(threshold);
    }
}

LiveStreamManager::PerformanceStats LiveStreamManager::get_performance_stats() const {
    std::lock_guard<std::mutex> lock(stats_mutex);
    return current_stats;
}

void LiveStreamManager::get_resolution(int& width, int& height) const {
    if (camera) {
        camera->get_resolution(width, height);
    }
}

void LiveStreamManager::set_recognition_callback(std::function<void(const RecognitionResult&)> callback) {
    recognition_callback = std::move(callback);
}

void LiveStreamManager::process_stream() {
    cv::Mat camera_frame;
    auto frame_start = std::chrono::high_resolution_clock::now();
    auto detection_time_sum = 0.0f;
    int detection_count = 0;

    while (!should_stop && is_streaming_flag) {
        // Capture frame from camera
        if (!camera->get_latest_frame(camera_frame)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
            continue;
        }

        frame_start = std::chrono::high_resolution_clock::now();

        // Create annotated frame
        AnnotatedFrame annotated;
        annotated.frame = camera_frame.clone();
        annotated.timestamp_ms = std::chrono::system_clock::now().time_since_epoch().count() / 1000000;

        // Process frame (detect and recognize faces)
        if (detection_enabled) {
            auto detect_start = std::chrono::high_resolution_clock::now();
            annotated.detected_faces = detect_and_recognize_faces(camera_frame);
            auto detect_end = std::chrono::high_resolution_clock::now();

            detection_time_sum += std::chrono::duration<float, std::milli>(detect_end - detect_start).count();
            detection_count++;

            annotated.has_faces = !annotated.detected_faces.empty();
        } else {
            annotated.has_faces = false;
        }

        // Draw annotations on the frame
        if (annotated.has_faces) {
            draw_face_annotations(annotated.frame, annotated.detected_faces);
        }

        // Update the latest frame
        {
            std::lock_guard<std::mutex> lock(frame_mutex);
            latest_frame = annotated;
            frame_ready = true;
        }

        // Update frame counter and FPS
        frame_count++;
        update_fps_counter();

        // Update performance stats
        {
            std::lock_guard<std::mutex> lock(stats_mutex);
            current_stats.detected_faces_count = annotated.detected_faces.size();
            if (detection_count > 0) {
                current_stats.detection_time_ms = detection_time_sum / detection_count;
                detection_time_sum = 0;
                detection_count = 0;
            }
        }

        // Control frame rate (roughly 30 FPS)
        auto frame_end = std::chrono::high_resolution_clock::now();
        auto frame_time = std::chrono::duration<float, std::milli>(frame_end - frame_start).count();
        if (frame_time < 33.33f) {  // ~30 FPS
            std::this_thread::sleep_for(std::chrono::milliseconds(
                static_cast<int>(33.33f - frame_time)));
        }
    }
}

std::vector<Face> LiveStreamManager::detect_and_recognize_faces(const cv::Mat& frame) {
    std::vector<Face> faces;

    if (!detector || !recognizer) {
        return faces;
    }

    // Detect faces in the frame
    faces = detector->detect_faces(frame);

    // Filter faces by confidence threshold
    faces.erase(
        std::remove_if(faces.begin(), faces.end(),
                       [this](const Face& f) { return f.confidence < detection_confidence_threshold; }),
        faces.end()
    );

    // Recognize each detected face
    for (auto& face : faces) {
        // Extract face region
        cv::Mat face_roi = FaceDetector::extract_face(frame, face.bbox);

        if (face_roi.empty()) {
            continue;
        }

        // Perform recognition
        auto result = recognizer->recognize_face(face_roi);

        // Update face with recognition result if confidence is high enough
        if (result.confidence >= recognition_confidence_threshold) {
            face.person_id = result.person_id;
            face.confidence = result.confidence;
            face.embedding = recognizer->get_face_embedding(face_roi);

            bool should_notify = false;
            if (!face.person_id.empty() && recognition_callback) {
                std::lock_guard<std::mutex> lock(recognized_persons_mutex);
                auto inserted = recognized_persons_in_session.insert(face.person_id);
                should_notify = inserted.second;
            }

            if (should_notify && recognition_callback) {
                RecognitionResult callback_result = result;
                if (database) {
                    std::string person_name;
                    if (database->get_person_info(face.person_id, person_name)) {
                        callback_result.person_name = person_name;
                    }
                }
                if (callback_result.person_name.empty()) {
                    callback_result.person_name = face.person_id;
                }
                recognition_callback(callback_result);
            }
        } else {
            face.person_id = "Unknown";
        }
    }

    return faces;
}

void LiveStreamManager::draw_face_annotations(cv::Mat& frame, const std::vector<Face>& faces) {
    for (const auto& face : faces) {
        // Expand bounding box by 20%
        int expanded_width = static_cast<int>(face.bbox.width * 1.2);
        int expanded_height = static_cast<int>(face.bbox.height * 1.2);
        int expand_x = (expanded_width - face.bbox.width) / 2;
        int expand_y = (expanded_height - face.bbox.height) / 2;

        // Calculate expanded corner coordinates with boundary checks
        int x1 = std::max(0, face.bbox.x - expand_x);
        int y1 = std::max(0, face.bbox.y - expand_y);
        int x2 = std::min(frame.cols - 1, face.bbox.x + face.bbox.width + expand_x);
        int y2 = std::min(frame.rows - 1, face.bbox.y + face.bbox.height + expand_y);

        int corner_length = 20;  // Length of corner marks
        int line_thickness = 2;
        cv::Scalar corner_color(0, 255, 0);  // Green color

        // Top-left corner
        cv::line(frame, cv::Point(x1, y1), cv::Point(x1 + corner_length, y1), corner_color, line_thickness);
        cv::line(frame, cv::Point(x1, y1), cv::Point(x1, y1 + corner_length), corner_color, line_thickness);

        // Top-right corner
        cv::line(frame, cv::Point(x2, y1), cv::Point(x2 - corner_length, y1), corner_color, line_thickness);
        cv::line(frame, cv::Point(x2, y1), cv::Point(x2, y1 + corner_length), corner_color, line_thickness);

        // Bottom-left corner
        cv::line(frame, cv::Point(x1, y2), cv::Point(x1 + corner_length, y2), corner_color, line_thickness);
        cv::line(frame, cv::Point(x1, y2), cv::Point(x1, y2 - corner_length), corner_color, line_thickness);

        // Bottom-right corner
        cv::line(frame, cv::Point(x2, y2), cv::Point(x2 - corner_length, y2), corner_color, line_thickness);
        cv::line(frame, cv::Point(x2, y2), cv::Point(x2, y2 - corner_length), corner_color, line_thickness);

        // Prepare label with person name and confidence
        std::string label = face.person_id;
        if (!label.empty() && label != "Unknown") {
            label += cv::format(" (%.2f)", face.confidence);
        }

        // Draw label background
        int font_face = cv::FONT_HERSHEY_SIMPLEX;
        double font_scale = 1.0;
        int thickness = 2;
        int baseline = 0;
        cv::Size text_size = cv::getTextSize(label, font_face, font_scale, thickness, &baseline);

        cv::Point text_origin(x1, y1 - 5);
        cv::rectangle(frame,
                      cv::Point(text_origin.x, text_origin.y - text_size.height - 5),
                      cv::Point(text_origin.x + text_size.width, text_origin.y + baseline),
                      cv::Scalar(0, 255, 0), cv::FILLED);

        // Draw label text
        cv::putText(frame, label, text_origin, font_face, font_scale,
                   cv::Scalar(0, 0, 0), thickness);
    }
}

void LiveStreamManager::update_fps_counter() {
    auto current_time = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration<float>(current_time - last_fps_update).count();

    if (elapsed >= 1.0f) {  // Update every second
        std::lock_guard<std::mutex> lock(stats_mutex);
        current_stats.fps = static_cast<int>(frame_count / elapsed);
        frame_count = 0;
        last_fps_update = current_time;
    }
}
