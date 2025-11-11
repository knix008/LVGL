#include <iostream>
#include <memory>
#include <signal.h>
#include <cstdlib>
#include <filesystem>
#include <vector>

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
                // Draw detected faces on image
                cv::Mat display_image = face_detector->draw_faces(current_image.mat, detected_faces);
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

            // Load all registered face embeddings
            std::vector<cv::Mat> training_faces;
            std::vector<int> labels;

            int label_id = 0;
            for (size_t i = 0; i < all_persons.size(); i++) {
                const auto& person_id = all_persons[i];
                // Load embeddings from database
                // For now, train with extracted faces from database
                std::string person_dir = "./dataset/" + person_id;
                label_id++;
            }

            // Get test face embedding
            auto test_embedding = face_recognizer->get_face_embedding(face_image);
            if (test_embedding.empty()) {
                g_gui->show_error_message("Error", "Failed to extract face embedding");
                return;
            }

            // Find closest match in database
            last_recognition.is_registered = false;
            last_recognition.person_id = "";
            last_recognition.person_name = "Unknown";
            last_recognition.confidence = 0.0f;

            // This is a simplified recognition - you would normally compare embeddings
            // For now, use the face recognizer's recognition function
            last_recognition = face_recognizer->recognize_face(face_image);

            if (last_recognition.is_registered) {
                std::string person_name;
                if (face_database->get_person_info(last_recognition.person_id, person_name)) {
                    last_recognition.person_name = person_name;
                }
                g_gui->show_success_message("Recognition Result",
                    last_recognition.person_name + "\n(Confidence: " +
                    std::to_string(static_cast<int>(last_recognition.confidence * 100)) + "%)");
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
