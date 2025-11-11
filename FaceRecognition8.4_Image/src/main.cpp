#include <iostream>
#include <memory>
#include <signal.h>
#include <cstdlib>
#include <filesystem>
#include <vector>
#include <map>

#include "image_loader.h"
#include "face_detector.h"
#include "face_recognizer.h"
#include "face_database.h"
#include "gui.h"

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

int main(int /* argc */, char* /* argv */[]) {
    std::cout << "================================" << std::endl;
    std::cout << "Face Recognition Application" << std::endl;
    std::cout << "================================" << std::endl;
    std::cout << std::endl;

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

        // Set up GUI callbacks
        g_gui->set_load_image_callback([&]() {
            std::cout << "Load Image button clicked" << std::endl;

            // Find image files in current directory only (for testing/recognition)
            // Dataset directory is used for registration only
            auto image_files = find_image_files(".");

            if (image_files.empty()) {
                g_gui->show_error_message("Error", "No image files found in current directory");
                g_gui->update_status("Ready - No test images");
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
                g_gui->update_status("Image loaded: " + selected_image);
            } else {
                g_gui->show_error_message("Error", "Failed to load image: " + selected_image);
            }
        });

        g_gui->set_detect_faces_callback([&]() {
            std::cout << "Detect Faces button clicked" << std::endl;
            if (current_image.mat.empty()) {
                g_gui->show_error_message("Error", "No image loaded");
                return;
            }

            detected_faces = face_detector->detect_faces(current_image.mat);

            if (detected_faces.empty()) {
                g_gui->show_error_message("Detection", "No faces detected");
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
                    g_gui->show_error_message("Error", "No images found in ./dataset/ directory");
                    g_gui->update_status("Ready - No dataset images");
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

                std::string result_msg = "Registered: " + std::to_string(registered_count) +
                                        "\nFailed: " + std::to_string(failed_count);
                g_gui->show_success_message("Registration Complete", result_msg);
                g_gui->update_status("Registered " + std::to_string(registered_count) + " persons");
            } catch (const std::exception& e) {
                std::cerr << "Registration error: " << e.what() << std::endl;
                g_gui->show_error_message("Error", "Registration failed: " + std::string(e.what()));
                g_gui->update_status("Registration failed");
            }
        });

        g_gui->set_recognize_person_callback([&]() {
            std::cout << "Recognize button clicked" << std::endl;

            if (current_image.mat.empty()) {
                g_gui->show_error_message("Error", "Please load an image first");
                return;
            }

            // Detect face
            auto faces = face_detector->detect_faces(current_image.mat);
            if (faces.empty()) {
                g_gui->show_error_message("Error", "No face detected");
                return;
            }

            if (faces.size() > 1) {
                g_gui->show_error_message("Error", "Multiple faces detected. Please use image with one face");
                return;
            }

            // Extract face
            cv::Mat face_image = FaceDetector::extract_face(current_image.mat, faces[0].bbox);
            if (face_image.empty()) {
                g_gui->show_error_message("Error", "Failed to extract face");
                return;
            }

            // Get all registered persons
            auto all_persons = face_database->get_all_persons();
            if (all_persons.empty()) {
                g_gui->show_error_message("Error", "No registered persons in database");
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
                g_gui->show_error_message("Error", "No face images found in database for training");
                return;
            }

            std::cout << "Training recognizer with " << training_faces.size() << " face images" << std::endl;

            // Train the recognizer with loaded face images
            if (!face_recognizer->train_faces(training_faces, labels)) {
                g_gui->show_error_message("Error", "Failed to train recognizer");
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
                    g_gui->show_success_message("Recognition Result",
                        last_recognition.person_name + "\n(Confidence: " +
                        std::to_string(static_cast<int>(last_recognition.confidence * 100)) + "%)");
                } catch (const std::exception& e) {
                    std::cerr << "Error parsing recognition result: " << e.what() << std::endl;
                    g_gui->show_error_message("Error", "Failed to process recognition result");
                }
            } else {
                g_gui->show_error_message("Recognition Result", "Unknown person");
            }
        });

        std::cout << "Starting main event loop..." << std::endl;
        std::cout << std::endl;

        // Run GUI
        g_gui->run();

        std::cout << "Application shutting down..." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Goodbye!" << std::endl;
    return 0;
}
