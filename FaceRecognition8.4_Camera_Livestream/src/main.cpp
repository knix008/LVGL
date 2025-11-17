#include <iostream>
#include <fstream>
#include <memory>
#include <signal.h>
#include <cstdlib>
#include <ctime>
#include <filesystem>
#include <vector>
#include <map>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <opencv2/imgcodecs.hpp>

#include "image_loader.h"
#include "face_detector.h"
#include "face_recognizer.h"
#include "face_database.h"
#include "gui.h"
#include "camera_capture.h"
#include "live_stream_manager.h"

// Helper function to load camera configuration from file
void load_camera_config(int& camera_index, int& min_index, int& max_index,
                        int& width, int& height, int& fps, bool& auto_load) {
    std::ifstream config_file("camera_config.txt");
    if (!config_file.is_open()) {
        std::cout << "Note: camera_config.txt not found, using defaults" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(config_file, line)) {
        // Skip comments and empty lines
        if (line.empty() || line[0] == '#') continue;

        // Find the = sign
        size_t eq_pos = line.find('=');
        if (eq_pos == std::string::npos) continue;

        std::string key = line.substr(0, eq_pos);
        std::string value = line.substr(eq_pos + 1);

        // Trim whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);

        try {
            if (key == "camera_index") {
                camera_index = std::stoi(value);
            } else if (key == "camera_min_index") {
                min_index = std::stoi(value);
            } else if (key == "camera_max_index") {
                max_index = std::stoi(value);
            } else if (key == "camera_width") {
                width = std::stoi(value);
            } else if (key == "camera_height") {
                height = std::stoi(value);
            } else if (key == "camera_fps") {
                fps = std::stoi(value);
            } else if (key == "auto_load") {
                auto_load = (std::stoi(value) != 0);
            }
        } catch (const std::exception& e) {
            std::cerr << "Warning: Invalid config value for '" << key << "': " << value << std::endl;
        }
    }

    config_file.close();
    std::cout << "Loaded camera configuration from camera_config.txt" << std::endl;
}

// Helper function to find image files in a directory
std::vector<std::string> find_image_files(const std::string& directory = ".") {
    std::vector<std::string> images;
    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                std::string ext = entry.path().extension().string();
                // Check for image extensions
                if (ext == ".jpg" || ext == ".jpeg" || ext == ".png" ||
                    ext == ".JPG" || ext == ".JPEG" || ext == ".PNG" ||
                    ext == ".bmp" || ext == ".BMP" || ext == ".gif" || ext == ".GIF") {
                    images.push_back(entry.path().string());
                }
            }
        }
        std::sort(images.begin(), images.end());
    } catch (const std::exception& e) {
        std::cerr << "Error reading directory: " << e.what() << std::endl;
    }
    return images;
}

static std::string extract_person_id_from_filename(const std::string& filename) {
    size_t dot_pos = filename.find('.');
    if (dot_pos == std::string::npos) {
        return filename;
    }
    return filename.substr(0, dot_pos);
}

static bool process_dataset_image(const std::string& image_path,
                                  const std::string& person_id,
                                  ImageLoader& image_loader,
                                  FaceDetector& face_detector,
                                  std::vector<cv::Mat>& training_faces,
                                  std::vector<int>& labels,
                                  std::unordered_map<std::string, int>& person_to_label,
                                  std::unordered_map<int, std::string>& label_to_person,
                                  int& processed_count,
                                  int& skipped_count) {
    if (person_id.empty()) {
        skipped_count++;
        return false;
    }

    ImageData img;
    if (!image_loader.load_image(image_path, img)) {
        skipped_count++;
        return false;
    }

    auto faces = face_detector.detect_faces(img.mat);
    if (faces.size() != 1) {
        skipped_count++;
        return false;
    }

    cv::Mat face_roi = FaceDetector::extract_face(img.mat, faces[0].bbox);
    if (face_roi.empty()) {
        skipped_count++;
        return false;
    }

    cv::Mat resized;
    cv::resize(face_roi, resized, cv::Size(200, 200));

    cv::Mat gray_face;
    if (resized.channels() == 3) {
        cv::cvtColor(resized, gray_face, cv::COLOR_RGB2GRAY);
    } else if (resized.channels() == 4) {
        cv::cvtColor(resized, gray_face, cv::COLOR_RGBA2GRAY);
    } else {
        gray_face = resized.clone();
    }

    int label = 0;
    auto it = person_to_label.find(person_id);
    if (it == person_to_label.end()) {
        label = static_cast<int>(person_to_label.size());
        person_to_label[person_id] = label;
        label_to_person[label] = person_id;
    } else {
        label = it->second;
    }

    training_faces.push_back(gray_face);
    labels.push_back(label);
    processed_count++;
    return true;
}

static bool train_recognizer_from_dataset(const std::string& dataset_dir,
                                          ImageLoader& image_loader,
                                          FaceDetector& face_detector,
                                          FaceRecognizer& face_recognizer,
                                          std::string& status_message) {
    namespace fs = std::filesystem;

    fs::path dataset_path(dataset_dir);
    if (!fs::exists(dataset_path)) {
        status_message = dataset_dir + " 디렉토리가 존재하지 않습니다";
        return false;
    }

    std::vector<cv::Mat> training_faces;
    std::vector<int> labels;
    std::unordered_map<std::string, int> person_to_label;
    std::unordered_map<int, std::string> label_to_person;
    int processed_count = 0;
    int skipped_count = 0;

    try {
        for (const auto& entry : fs::directory_iterator(dataset_path)) {
            if (entry.is_directory()) {
                std::string person_id = entry.path().filename().string();
                for (const auto& file_entry : fs::directory_iterator(entry.path())) {
                    if (!file_entry.is_regular_file()) {
                        continue;
                    }
                    if (!image_loader.is_supported_format(file_entry.path().string())) {
                        continue;
                    }
                    process_dataset_image(file_entry.path().string(), person_id, image_loader,
                                          face_detector, training_faces, labels,
                                          person_to_label, label_to_person,
                                          processed_count, skipped_count);
                }
            } else if (entry.is_regular_file()) {
                if (!image_loader.is_supported_format(entry.path().string())) {
                    continue;
                }
                std::string person_id = extract_person_id_from_filename(entry.path().filename().string());
                process_dataset_image(entry.path().string(), person_id, image_loader,
                                      face_detector, training_faces, labels,
                                      person_to_label, label_to_person,
                                      processed_count, skipped_count);
            }
        }
    } catch (const std::exception& e) {
        status_message = std::string("데이터셋 처리 중 오류: ") + e.what();
        return false;
    }

    if (training_faces.empty()) {
        status_message = "데이터셋에서 사용 가능한 얼굴 이미지를 찾지 못했습니다";
        return false;
    }

    if (!face_recognizer.train_faces(training_faces, labels)) {
        status_message = "얼굴 인식기 훈련에 실패했습니다";
        return false;
    }

    face_recognizer.set_label_mapping(label_to_person);

    status_message = "인원: " + std::to_string(person_to_label.size()) + "명, "
                   + "이미지: " + std::to_string(processed_count) + "개 "
                   + "(건너뜀: " + std::to_string(skipped_count) + ")";
    return true;
}

// Global pointers for signal handling
std::unique_ptr<GUI> g_gui;
bool g_running = true;

void signal_handler(int signal) {
    std::cout << "\nShutdown signal received (" << signal << ")" << std::endl;
    g_running = false;
    if (g_gui) {
        g_gui->stop();
    }
}

int main(int argc, char* argv[]) {
    std::cout << "================================" << std::endl;
    std::cout << "Face Recognition Application" << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << std::endl;

    // Load configuration from file (defaults will be overridden by command-line args)
    bool auto_load_test_image = false;
    int preferred_camera_index = -1;  // -1 means auto-detect
    int camera_min_index = 0;
    int camera_max_index = 15;
    int camera_width = 640;
    int camera_height = 480;
    int camera_fps = 30;

    // Load camera settings from config file
    load_camera_config(preferred_camera_index, camera_min_index, camera_max_index,
                       camera_width, camera_height, camera_fps, auto_load_test_image);
    std::cout << std::endl;

    // Parse command-line arguments (override config file settings)

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];

        if (arg == "--auto-load" || arg == "-a") {
            auto_load_test_image = true;
            std::cout << "Auto-load mode: Test image will be loaded automatically" << std::endl;
        }
        else if (arg == "--camera" || arg == "-c") {
            // Next argument should be camera index
            if (i + 1 < argc) {
                try {
                    preferred_camera_index = std::stoi(argv[i + 1]);
                    std::cout << "Camera index specified: " << preferred_camera_index << std::endl;
                    i++;  // Skip next argument
                } catch (const std::exception& e) {
                    std::cerr << "Invalid camera index: " << argv[i + 1] << std::endl;
                }
            }
        }
        else if (arg == "--camera-range") {
            // Next two arguments should be min and max
            if (i + 2 < argc) {
                try {
                    camera_min_index = std::stoi(argv[i + 1]);
                    camera_max_index = std::stoi(argv[i + 2]);
                    std::cout << "Camera range specified: " << camera_min_index << "-" << camera_max_index << std::endl;
                    i += 2;  // Skip next two arguments
                } catch (const std::exception& e) {
                    std::cerr << "Invalid camera range values" << std::endl;
                }
            }
        }
        else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: " << argv[0] << " [OPTIONS]" << std::endl;
            std::cout << "Options:" << std::endl;
            std::cout << "  -a, --auto-load              Auto-load test image on startup" << std::endl;
            std::cout << "  -c, --camera INDEX           Use specific camera device (e.g., -c 2 for /dev/video2)" << std::endl;
            std::cout << "  --camera-range MIN MAX       Scan camera range when auto-detecting (default: 0-15)" << std::endl;
            std::cout << "  -h, --help                   Show this help message" << std::endl;
            std::cout << std::endl;
            std::cout << "Examples:" << std::endl;
            std::cout << "  " << argv[0] << " -a                          # Auto-load with camera auto-detect" << std::endl;
            std::cout << "  " << argv[0] << " -c 2                        # Use /dev/video2 specifically" << std::endl;
            std::cout << "  " << argv[0] << " --camera-range 0 5          # Scan only /dev/video0 to /dev/video5" << std::endl;
            std::cout << "  " << argv[0] << " -a -c 3 --camera-range 2 4  # Multiple options combined" << std::endl;
            std::cout << std::endl;
            return 0;
        }
    }

    if (!auto_load_test_image && preferred_camera_index == -1) {
        std::cout << std::endl;
    }

    // Register signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    try {
        // Initialize components
        std::cout << "Initializing components..." << std::endl;

        auto image_loader = std::make_unique<ImageLoader>();
        auto face_detector = std::make_unique<FaceDetector>();
        auto face_recognizer = std::make_unique<FaceRecognizer>();
        auto face_database = std::make_unique<FaceDatabase>("./dataset");

        // Initialize face detector
        if (!face_detector->initialize()) {
            std::cerr << "Failed to initialize face detector" << std::endl;
            return 1;
        }

        // Initialize face recognizer
        if (!face_recognizer->initialize()) {
            std::cerr << "Failed to initialize face recognizer" << std::endl;
            return 1;
        }

        // Initialize face database
        if (!face_database->initialize()) {
            std::cerr << "Failed to initialize face database" << std::endl;
            return 1;
        }

        std::cout << "All components initialized successfully" << std::endl;
        std::cout << std::endl;

        // Initialize GUI
        std::cout << "Initializing GUI..." << std::endl;
        g_gui = std::make_unique<GUI>(320, 640);

        if (!g_gui->initialize()) {
            std::cerr << "Failed to initialize GUI" << std::endl;
            return 1;
        }

        std::cout << "GUI initialized successfully" << std::endl;
        std::cout << std::endl;

        // Current state
        ImageData current_image;
        std::vector<Face> detected_faces;
        RecognitionResult last_recognition;

        // Camera capture
        auto camera = std::make_unique<CameraCapture>();
        bool camera_active = false;

        // Auto-load test image if requested
        if (auto_load_test_image) {
            std::cout << "Auto-loading Test01.jpeg..." << std::endl;
            if (image_loader->load_image("Test01.jpeg", current_image)) {
                // Display the loaded image on the GUI canvas
                std::cout << "  ✓ Test image loaded successfully" << std::endl;
                std::cout << "  Size: " << current_image.mat.cols << "x" << current_image.mat.rows << std::endl;

                // Display the image on the canvas (is_rgb=true because image_loader returns RGB)
                if (g_gui->display_image(current_image.mat, true, true)) {
                    std::cout << "  ✓ Image displayed on canvas" << std::endl;
                    g_gui->update_status("테스트 이미지 로드됨: Test01.jpeg");
                } else {
                    std::cerr << "  ✗ Failed to display image on canvas" << std::endl;
                }
            } else {
                std::cerr << "  ✗ Failed to load Test01.jpeg" << std::endl;
            }
            std::cout << std::endl;
        }

        // Set up GUI callbacks
        g_gui->set_load_image_callback([&]() {
            std::cout << "Load Image button clicked" << std::endl;

            // Find image files in current directory only (for testing/recognition)
            // Dataset directory is used for registration only
            auto image_files = find_image_files(".");

            if (image_files.empty()) {
                g_gui->show_error_message("오류", "현재 디렉토리에서 이미지 파일을 찾을 수 없습니다");
                g_gui->update_status("준비됨 - 테스트 이미지 없음");
                return;
            }

            // Print available images to console
            std::cout << "Found " << image_files.size() << " test image file(s):" << std::endl;
            for (size_t i = 0; i < image_files.size(); i++) {
                std::cout << "  [" << i << "] " << image_files[i] << std::endl;
            }

            // Load the first available image for testing/recognition
            std::string selected_image = image_files[0];

            std::cout << "Selected: " << selected_image << std::endl;

            if (image_loader->load_image(selected_image, current_image)) {
                g_gui->display_image(current_image.mat);
                g_gui->update_status("이미지 로드됨: " + selected_image);
            } else {
                g_gui->show_error_message("오류", "이미지 로드 실패: " + selected_image);
            }
        });

        g_gui->set_detect_faces_callback([&]() {
            std::cout << "Detect Faces button clicked" << std::endl;
            if (current_image.mat.empty()) {
                g_gui->show_error_message("오류", "로드된 이미지가 없습니다");
                return;
            }

            detected_faces = face_detector->detect_faces(current_image.mat);

            if (detected_faces.empty()) {
                g_gui->show_error_message("감지", "얼굴이 감지되지 않았습니다");
            } else {
                std::cout << "Detected " << detected_faces.size() << " face(s)" << std::endl;

                // Resize image to fit canvas before drawing boxes
                // Canvas dimensions: 300x180 max (screen_width - 20 = 300)
                cv::Mat resized_image = current_image.mat.clone();
                int canvas_width = 300;
                int canvas_height = 180;

                if (resized_image.cols > canvas_width || resized_image.rows > canvas_height) {
                    float scale = std::min(static_cast<float>(canvas_width) / resized_image.cols,
                                          static_cast<float>(canvas_height) / resized_image.rows);
                    int new_width = static_cast<int>(resized_image.cols * scale);
                    int new_height = static_cast<int>(resized_image.rows * scale);

                    if (new_width < 10) new_width = 10;
                    if (new_height < 10) new_height = 10;

                    cv::resize(resized_image, resized_image, cv::Size(new_width, new_height), 0, 0, cv::INTER_LINEAR);

                    // Scale face bounding boxes to match resized image
                    float scale_x = static_cast<float>(new_width) / current_image.mat.cols;
                    float scale_y = static_cast<float>(new_height) / current_image.mat.rows;

                    for (auto& face : detected_faces) {
                        face.bbox.x = static_cast<int>(face.bbox.x * scale_x);
                        face.bbox.y = static_cast<int>(face.bbox.y * scale_y);
                        face.bbox.width = static_cast<int>(face.bbox.width * scale_x);
                        face.bbox.height = static_cast<int>(face.bbox.height * scale_y);
                    }
                }

                // Draw detected faces on resized image
                cv::Mat display_image = face_detector->draw_faces(resized_image, detected_faces);
                g_gui->display_detection_result(display_image, detected_faces);
            }
        });

        g_gui->set_register_person_callback([&]() {
            std::cout << "Register button clicked - Loading registration images from ./dataset/" << std::endl;

            try {
                // Load all images from ./dataset/ directory
                auto dataset_images = find_image_files("./dataset");

                if (dataset_images.empty()) {
                    g_gui->show_error_message("오류", "./dataset/ 디렉토리에서 이미지를 찾을 수 없습니다");
                    g_gui->update_status("준비됨 - 데이터셋 이미지 없음");
                    return;
                }

                // Print available dataset images to console
                std::cout << "Found " << dataset_images.size() << " image(s) in dataset:" << std::endl;
                for (size_t i = 0; i < dataset_images.size(); i++) {
                    std::cout << "  [" << i << "] " << dataset_images[i] << std::endl;
                }

                int registered_count = 0;
                int failed_count = 0;

                // Register all images from dataset
                for (const auto& image_path : dataset_images) {
                    std::cout << "\nProcessing: " << image_path << std::endl;

                    ImageData img;
                    if (!image_loader->load_image(image_path, img)) {
                        std::cout << "  Failed to load image" << std::endl;
                        failed_count++;
                        continue;
                    }

                    // Detect face
                    auto faces = face_detector->detect_faces(img.mat);
                    if (faces.empty()) {
                        std::cout << "  No face detected" << std::endl;
                        failed_count++;
                        continue;
                    }

                    if (faces.size() > 1) {
                        std::cout << "  Multiple faces detected, skipping" << std::endl;
                        failed_count++;
                        continue;
                    }

                    // Extract face
                    cv::Mat face_image = FaceDetector::extract_face(img.mat, faces[0].bbox);
                    if (face_image.empty()) {
                        std::cout << "  Failed to extract face" << std::endl;
                        failed_count++;
                        continue;
                    }

                    // Get face embedding
                    auto embedding = face_recognizer->get_face_embedding(face_image);
                    if (embedding.empty()) {
                        std::cout << "  Failed to generate embedding" << std::endl;
                        failed_count++;
                        continue;
                    }

                    // Extract person name from filename (e.g., "A.1.1.jpg" -> "A")
                    std::string filename = std::filesystem::path(image_path).filename().string();
                    size_t dot_pos = filename.find('.');
                    std::string person_name = (dot_pos != std::string::npos) ?
                                             filename.substr(0, dot_pos) :
                                             filename;

                    std::string person_id = person_name;  // Use name as ID

                    // Register person
                    if (face_database->register_person(person_id, person_name, face_image, embedding)) {
                        std::cout << "  Registered: " << person_name << std::endl;
                        registered_count++;
                    } else {
                        std::cout << "  Failed to register" << std::endl;
                        failed_count++;
                    }
                }

                face_database->save();

                std::string result_msg = "등록됨: " + std::to_string(registered_count) +
                                        "\n실패: " + std::to_string(failed_count);
                g_gui->show_success_message("등록 완료", result_msg);
                g_gui->update_status(std::to_string(registered_count) + "명 등록됨");
            } catch (const std::exception& e) {
                std::cerr << "Registration error: " << e.what() << std::endl;
                g_gui->show_error_message("오류", "등록 실패: " + std::string(e.what()));
                g_gui->update_status("등록 실패");
            }
        });

        g_gui->set_recognize_person_callback([&]() {
            std::cout << "Recognize button clicked" << std::endl;

            if (current_image.mat.empty()) {
                g_gui->show_error_message("오류", "먼저 이미지를 로드하세요");
                return;
            }

            // Detect face
            auto faces = face_detector->detect_faces(current_image.mat);
            if (faces.empty()) {
                g_gui->show_error_message("오류", "얼굴이 감지되지 않았습니다");
                return;
            }

            if (faces.size() > 1) {
                g_gui->show_error_message("오류", "여러 개의 얼굴이 감지되었습니다. 얼굴이 하나인 이미지를 사용하세요");
                return;
            }

            // Extract face
            cv::Mat face_image = FaceDetector::extract_face(current_image.mat, faces[0].bbox);
            if (face_image.empty()) {
                g_gui->show_error_message("오류", "얼굴 추출 실패");
                return;
            }

            // Get all registered persons
            auto all_persons = face_database->get_all_persons();
            if (all_persons.empty()) {
                g_gui->show_error_message("오류", "데이터베이스에 등록된 사람이 없습니다");
                return;
            }

            std::cout << "Found " << all_persons.size() << " registered person(s)" << std::endl;

            // Load all registered face images for training
            std::vector<cv::Mat> training_faces;
            std::vector<int> labels;
            std::map<int, std::string> label_to_person_id;  // Map label to person_id

            int label_id = 0;
            for (size_t i = 0; i < all_persons.size(); i++) {
                const auto& person_id = all_persons[i];
                std::string person_dir = "./dataset/" + person_id;

                // Try to load face images from the person directory
                try {
                    for (const auto& entry : std::filesystem::directory_iterator(person_dir)) {
                        if (entry.is_regular_file()) {
                            std::string filename = entry.path().filename().string();
                            // Check for face image files (face_*.png)
                            if (filename.find("face_") == 0 && filename.find(".png") != std::string::npos) {
                                // Load the face image
                                cv::Mat face_img = cv::imread(entry.path().string());
                                if (!face_img.empty()) {
                                    // Convert BGR to RGB if necessary
                                    cv::Mat rgb_face;
                                    cv::cvtColor(face_img, rgb_face, cv::COLOR_BGR2RGB);

                                    // Resize to standard size
                                    cv::Mat resized_face;
                                    cv::resize(rgb_face, resized_face, cv::Size(200, 200));

                                    // Convert to grayscale
                                    cv::Mat gray_face;
                                    cv::cvtColor(resized_face, gray_face, cv::COLOR_RGB2GRAY);

                                    training_faces.push_back(gray_face);
                                    labels.push_back(label_id);

                                    std::cout << "  Loaded face image: " << filename
                                              << " (label: " << label_id << ")" << std::endl;
                                }
                            }
                        }
                    }
                } catch (const std::exception& e) {
                    std::cerr << "Error loading faces for person " << person_id << ": " << e.what() << std::endl;
                }

                // Map this label to person_id
                label_to_person_id[label_id] = person_id;
                label_id++;
            }

            if (training_faces.empty()) {
                g_gui->show_error_message("오류", "훈련을 위한 얼굴 이미지가 데이터베이스에 없습니다");
                return;
            }

            std::cout << "Training recognizer with " << training_faces.size() << " face images" << std::endl;

            // Train the recognizer with loaded face images
            if (!face_recognizer->train_faces(training_faces, labels)) {
                g_gui->show_error_message("오류", "인식기 훈련 실패");
                return;
            }

            std::cout << "Recognizer trained successfully" << std::endl;

            // Now recognize the test face
            last_recognition = face_recognizer->recognize_face(face_image);

            if (last_recognition.is_registered) {
                // Get the person name from the label
                // person_id format is "Person_X" where X is the label index
                try {
                    int predicted_label = std::stoi(last_recognition.person_id.substr(7));  // Extract number from "Person_X" (7 chars = length of "Person_")
                    std::string person_name;
                    if (label_to_person_id.find(predicted_label) != label_to_person_id.end()) {
                        std::string person_id = label_to_person_id[predicted_label];
                        if (face_database->get_person_info(person_id, person_name)) {
                            last_recognition.person_name = person_name;
                        }
                    }
                    g_gui->show_success_message("인식 결과",
                        last_recognition.person_name + "\n(신뢰도: " +
                        std::to_string(static_cast<int>(last_recognition.confidence * 100)) + "%)");
                } catch (const std::exception& e) {
                    std::cerr << "Error parsing recognition result: " << e.what() << std::endl;
                    g_gui->show_error_message("오류", "인식 결과 처리 실패");
                }
            } else {
                g_gui->show_error_message("인식 결과", "알려지지 않은 사람");
            }
        });

        g_gui->set_camera_toggle_callback([&](bool start) {
            std::cout << "Camera toggle: " << (start ? "Starting" : "Stopping") << std::endl;

            if (start) {
                // Initialize and start camera
                if (!camera->is_initialized()) {
                    bool init_success = false;

                    if (preferred_camera_index >= 0) {
                        // Use specified camera index
                        std::cout << "Initializing camera at /dev/video" << preferred_camera_index << std::endl;
                        init_success = camera->initialize(preferred_camera_index);
                    } else {
                        // Auto-detect camera in specified range
                        std::cout << "Auto-detecting camera in range " << camera_min_index
                                  << "-" << camera_max_index << std::endl;
                        init_success = camera->initialize_auto(camera_min_index, camera_max_index);
                    }

                    if (!init_success) {
                        std::string error_msg = "카메라 초기화 실패";
                        if (preferred_camera_index >= 0) {
                            error_msg += " at /dev/video" + std::to_string(preferred_camera_index);
                        } else {
                            error_msg += "\n범위 " +
                                       std::to_string(camera_min_index) + "-" +
                                       std::to_string(camera_max_index) + "에서 카메라를 찾을 수 없습니다";
                        }
                        g_gui->show_error_message("오류", error_msg);
                        g_gui->update_status("카메라 초기화 실패");
                        return;
                    }
                }

                if (!camera->start_capture()) {
                    g_gui->show_error_message("오류", "카메라 캡처 시작 실패");
                    g_gui->update_status("카메라 캡처 실패");
                    return;
                }

                camera_active = true;
                g_gui->update_status("카메라 활성화됨");
                std::cout << "  Camera started successfully" << std::endl;
            } else {
                // Stop camera
                camera->stop_capture();
                camera_active = false;
                g_gui->update_status("카메라 중지됨");
                std::cout << "  Camera stopped" << std::endl;
            }
        });

        g_gui->set_capture_frame_callback([&]() {
            std::cout << "Capture Frame button clicked" << std::endl;

            if (!camera_active) {
                g_gui->show_error_message("오류", "카메라가 활성화되지 않았습니다. '카메라' 버튼을 클릭하여 시작하세요.");
                return;
            }

            cv::Mat frame;
            if (!camera->get_latest_frame(frame)) {
                g_gui->show_error_message("오류", "카메라에서 프레임 캡처 실패");
                return;
            }

            // Load frame into current_image using the frame loader
            if (image_loader->load_from_frame(frame, current_image)) {
                std::cout << "  Frame captured: " << current_image.mat.cols << "x" << current_image.mat.rows << std::endl;

                // Save captured frame to project root directory
                auto now = std::chrono::system_clock::now();
                auto time = std::chrono::system_clock::to_time_t(now);
                auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

                char filename[256];
                struct tm* tm_info = std::localtime(&time);
                std::strftime(filename, sizeof(filename), "capture_%Y%m%d_%H%M%S", tm_info);

                std::string output_filename = std::string(filename) + "_" +
                                            std::to_string(ms.count()) + ".jpg";
                std::string output_path = "./" + output_filename;

                // Convert RGB to BGR for saving (OpenCV expects BGR)
                cv::Mat bgr_frame;
                cv::cvtColor(current_image.mat, bgr_frame, cv::COLOR_RGB2BGR);

                if (cv::imwrite(output_path, bgr_frame)) {
                    std::cout << "  Frame saved to: " << output_path << std::endl;
                    g_gui->show_success_message("성공", "프레임 저장됨: " + output_filename);
                } else {
                    std::cerr << "  Failed to save frame to: " << output_path << std::endl;
                    g_gui->show_error_message("오류", "캡처한 프레임 저장 실패");
                }
            } else {
                g_gui->show_error_message("오류", "카메라 프레임 처리 실패");
            }
        });

        // Create live stream manager
        std::cout << "Initializing Live Stream Manager..." << std::endl;
        auto live_stream = std::make_unique<LiveStreamManager>(
            camera.get(),
            face_detector.get(),
            face_recognizer.get(),
            face_database.get()
        );

        // Configure live stream parameters
        live_stream->set_min_face_size(30, 30);
        live_stream->set_detection_confidence(0.5f);
        live_stream->set_recognition_confidence(0.6f);

        bool is_streaming = false;
        std::mutex recognition_queue_mutex;
        std::vector<RecognitionResult> pending_recognitions;
        long long last_displayed_frame_ts = 0;
        auto last_frame_refresh = std::chrono::steady_clock::now();

        live_stream->set_recognition_callback([&](const RecognitionResult& result) {
            std::lock_guard<std::mutex> lock(recognition_queue_mutex);
            pending_recognitions.push_back(result);
        });

        // Set live stream start callback
        g_gui->set_live_stream_start_callback([&]() {
            std::cout << "Live Stream Start button clicked" << std::endl;

            std::string training_status;
            if (!train_recognizer_from_dataset("./dataset", *image_loader, *face_detector,
                                               *face_recognizer, training_status)) {
                std::cerr << "  Failed to train recognizer: " << training_status << std::endl;
                g_gui->show_error_message("훈련 실패", training_status);
                return;
            }

            g_gui->update_status("훈련 완료 - " + training_status);
            std::cout << "  Recognizer trained successfully. " << training_status << std::endl;

            {
                std::lock_guard<std::mutex> lock(recognition_queue_mutex);
                pending_recognitions.clear();
            }
            last_displayed_frame_ts = 0;
            last_frame_refresh = std::chrono::steady_clock::now();  // Reset timer for live stream

            if (!camera->is_initialized()) {
                if (preferred_camera_index >= 0) {
                    camera->initialize(preferred_camera_index);
                } else {
                    camera->initialize_auto(camera_min_index, camera_max_index);
                }
            }

            if (!camera->is_capturing()) {
                camera->set_resolution(camera_width, camera_height);
                camera->set_fps(camera_fps);
                if (!camera->start_capture()) {
                    g_gui->show_error_message("오류", "카메라 캡처 시작 실패");
                    return;
                }
            }

            if (live_stream->start_stream()) {
                is_streaming = true;
                g_gui->update_status("라이브 스트림 시작됨");
                std::cout << "  Live stream started successfully" << std::endl;
            } else {
                g_gui->show_error_message("오류", "라이브 스트림 시작 실패");
            }
        });

        // Set live stream stop callback
        g_gui->set_live_stream_stop_callback([&]() {
            std::cout << "Live Stream Stop button clicked" << std::endl;

            live_stream->stop_stream();
            is_streaming = false;
            {
                std::lock_guard<std::mutex> lock(recognition_queue_mutex);
                pending_recognitions.clear();
            }
            last_displayed_frame_ts = 0;
            last_frame_refresh = std::chrono::steady_clock::now();
            g_gui->update_status("라이브 스트림 중지됨");
            std::cout << "  Live stream stopped" << std::endl;
        });

        std::cout << "Live Stream Manager initialized successfully" << std::endl;
        std::cout << std::endl;

        g_gui->set_idle_tick_callback([&]() {
            auto now = std::chrono::steady_clock::now();
            if (is_streaming &&
                std::chrono::duration_cast<std::chrono::milliseconds>(now - last_frame_refresh).count() >= 66) {
                AnnotatedFrame annotated;
                if (live_stream->get_latest_frame(annotated) &&
                    !annotated.frame.empty() &&
                    annotated.timestamp_ms != last_displayed_frame_ts) {
                    g_gui->display_image(annotated.frame, false, true);
                    last_displayed_frame_ts = annotated.timestamp_ms;
                }
                last_frame_refresh = now;
            }

            // Clear the pending recognition queue without showing popup messages
            {
                std::lock_guard<std::mutex> lock(recognition_queue_mutex);
                pending_recognitions.clear();
            }
        });

        std::cout << "Starting main event loop..." << std::endl;
        std::cout << std::endl;

        // Run GUI with live stream support
        // Note: GUI::run() handles the main event loop and display updates
        // We'll update live stream frames from the GUI's internal loop if needed
        g_gui->run();

        std::cout << "Application shutting down..." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Goodbye!" << std::endl;
    return 0;
}
