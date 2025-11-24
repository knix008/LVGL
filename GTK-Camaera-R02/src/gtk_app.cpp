#include "gtk_app.h"
#include <iostream>
#include <chrono>
#include <filesystem>

GTKApp::GTKApp()
    : window(nullptr), image_widget(nullptr), toggle_button(nullptr),
      train_button(nullptr), capture_button(nullptr),
      status_label(nullptr), fps_label(nullptr), face_info_label(nullptr),
      face_count_label(nullptr), error_rate_label(nullptr),
      refresh_timer(0), camera_running(false), face_recognition_enabled(false),
      training_in_progress(false), capture_in_progress(false), cleanup_done(false),
      frame_count(0), last_time(0), capture_count(0), last_recognition_time(0),
      last_recognized_name("Unknown"), last_recognized_confidence(0.0),
      has_recognition_result(false), training_success(false) {}

GTKApp::~GTKApp() {
    cleanup();
}

bool GTKApp::init() {
    try {
        // Initialize GTK
        gtk_init(nullptr, nullptr);

        // Create main window
        window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(window), "GTK Webcam Viewer");
        gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
        gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

        // Connect window destroy signal
        g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), this);

        // Create main container (vertical box)
        GtkWidget* vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
        gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
        gtk_container_add(GTK_CONTAINER(window), vbox);

        // Create image display widget
        image_widget = gtk_image_new();
        gtk_widget_set_size_request(image_widget, 640, 480);
        gtk_box_pack_start(GTK_BOX(vbox), image_widget, TRUE, TRUE, 0);

        // Create horizontal box for controls
        GtkWidget* hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
        gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

        // Create toggle button
        toggle_button = gtk_toggle_button_new_with_label("Start Camera");
        gtk_widget_set_size_request(toggle_button, 150, 40);
        g_signal_connect(toggle_button, "clicked", G_CALLBACK(on_toggle_button_clicked), this);
        gtk_box_pack_start(GTK_BOX(hbox), toggle_button, FALSE, FALSE, 0);

        // Create train button
        train_button = gtk_button_new_with_label("Registering");
        gtk_widget_set_size_request(train_button, 150, 40);
        g_signal_connect(train_button, "clicked", G_CALLBACK(on_train_button_clicked), this);
        gtk_box_pack_start(GTK_BOX(hbox), train_button, FALSE, FALSE, 0);

        // Create capture button
        capture_button = gtk_button_new_with_label("Capture Photo");
        gtk_widget_set_size_request(capture_button, 150, 40);
        g_signal_connect(capture_button, "clicked", G_CALLBACK(on_capture_button_clicked), this);
        gtk_box_pack_start(GTK_BOX(hbox), capture_button, FALSE, FALSE, 0);

        // Create status label
        status_label = gtk_label_new("Status: Camera Idle");
        gtk_box_pack_start(GTK_BOX(hbox), status_label, TRUE, TRUE, 0);

        // Create FPS label
        fps_label = gtk_label_new("FPS: 0");
        gtk_box_pack_end(GTK_BOX(hbox), fps_label, FALSE, FALSE, 0);

        // Create face info label (recognized person)
        face_info_label = gtk_label_new("Person: None detected");
        gtk_box_pack_end(GTK_BOX(hbox), face_info_label, FALSE, FALSE, 0);

        // Create face count label (confidence level)
        face_count_label = gtk_label_new("Confidence: 0%");
        gtk_box_pack_end(GTK_BOX(hbox), face_count_label, FALSE, FALSE, 0);

        // Create error rate label (detection metrics)
        error_rate_label = gtk_label_new("Detection Rate: 0% | Error: 0%");
        gtk_box_pack_end(GTK_BOX(hbox), error_rate_label, FALSE, FALSE, 0);

        // Open camera
        if (!camera.open(0)) {
            std::cerr << "Warning: Camera initialization failed" << std::endl;
            // Update status but continue - user can try to enable camera later
            gtk_label_set_text(GTK_LABEL(status_label), "Status: Camera Not Available");
            gtk_widget_set_sensitive(toggle_button, FALSE);
        }

        // Load face recognizer
        load_face_recognizer();

        // Show all widgets
        gtk_widget_show_all(window);

        // Set up refresh timer (30ms = ~33 FPS)
        refresh_timer = g_timeout_add(30, on_refresh_timer, this);

        return true;
    } catch (const std::exception& e) {
        std::cerr << "Exception during initialization: " << e.what() << std::endl;
        return false;
    }
}

void GTKApp::run() {
    gtk_main();
}

void GTKApp::cleanup() {
    // Prevent double-cleanup (can be called from both window destroy and destructor)
    if (cleanup_done) {
        return;
    }
    cleanup_done = true;

    // Stop camera and frame processing
    camera_running = false;  // Signal to stop processing frames

    // Process pending events - this will let refresh_frame return FALSE and stop naturally
    for (int i = 0; i < 5; i++) {
        while (gtk_events_pending()) {
            gtk_main_iteration();
        }
        g_usleep(20000); // 20ms between iterations
    }

    // Now it's safe to close the camera
    camera.close();  // This will join the camera thread

    // Clear the timer ID (it should have stopped by now)
    refresh_timer = 0;

    // Wait for training thread to finish
    if (training_thread.joinable()) {
        training_in_progress = false;  // Signal thread to stop if possible
        training_thread.join();
    }

    // Note: Don't explicitly close database - the FaceDatabase destructor will handle it
    // Calling close() here and then having destructor call it again causes double-free

    // Destroy window last (but only if not already destroyed by GTK)
    // The window destroy signal already triggered this cleanup
    if (window != nullptr) {
        window = nullptr;
    }
}

gboolean GTKApp::on_refresh_timer(gpointer user_data) {
    GTKApp* self = static_cast<GTKApp*>(user_data);
    return self->refresh_frame();
}

gboolean GTKApp::refresh_frame() {
    // Stop timer immediately if cleanup has started
    if (cleanup_done) {
        return FALSE; // Stop timer
    }

    if (!camera_running || capture_in_progress || training_in_progress) {
        return TRUE; // Continue timer but don't process frames
    }

    try {
        cv::Mat frame;
        if (camera.get_frame(frame)) {
            if (!frame.empty()) {
                // Flip the frame horizontally for mirror effect
                cv::flip(frame, frame, 1);

                // Detect faces
                std::vector<Face> faces = face_detector.detect_faces(frame);

                // Track best recognized face for UI display
                std::string best_person_name = "None detected";
                double best_confidence = 0.0;
                int recognized_count = 0;
                int unknown_count = 0;

                // Recognize faces if model is trained
                Face best_face;  // Store best face for display
                bool has_best_face = false;

                // Live face recognition with slow update rate (1.5 seconds)
                // This minimizes load on ONNX Runtime to prevent crashes
                // Skip recognition if training is in progress to avoid mutex contention
                gint64 recognition_time = g_get_monotonic_time();
                bool should_run_recognition = (recognition_time - last_recognition_time) > 1500000; // 1.5 seconds

                if (face_recognition_enabled && !faces.empty() && should_run_recognition && !training_in_progress) {
                    last_recognition_time = recognition_time;
                    for (size_t i = 0; i < faces.size(); i++) {
                        auto& face = faces[i];

                        // Validate bbox is within frame bounds
                        if (face.bbox.x < 0 || face.bbox.y < 0 ||
                            face.bbox.x + face.bbox.width > frame.cols ||
                            face.bbox.y + face.bbox.height > frame.rows) {
                            std::cerr << "[ERROR] Face bbox out of bounds! Skipping..." << std::endl;
                            continue;
                        }

                        // Check if face size is sufficient for reliable recognition (>70% confidence)
                        if (!face_recognizer.is_face_size_sufficient(face.bbox.width, face.bbox.height)) {
                            std::cout << "[Recognition] Face too small (" << face.bbox.width << "x" << face.bbox.height
                                      << "), minimum required: " << face_recognizer.get_min_face_size_for_recognition()
                                      << "x" << face_recognizer.get_min_face_size_for_recognition()
                                      << " - Skipping recognition" << std::endl;
                            face.name = "Too far";
                            face.confidence = 0.0;
                            face.id = -1;
                            unknown_count++;
                            continue;
                        }

                        cv::Mat face_roi = frame(face.bbox);
                        double confidence = 0.0;

                        int label = -1;
                        try {
                            // Lock mutex to protect ONNX Runtime (not thread-safe)
                            std::lock_guard<std::mutex> lock(recognition_mutex);
                            label = face_recognizer.recognize(face_roi, confidence);
                        } catch (const std::exception& e) {
                            std::cerr << "[ERROR] Face recognition failed: " << e.what() << std::endl;
                            // Continue with unknown label
                            label = -1;
                            confidence = 0.0;
                        }

                        if (label != -1) {
                            face.name = face_recognizer.get_label_name(label);
                            face.confidence = confidence * 100.0;  // Convert similarity to percentage
                            face.id = label;
                            recognized_count++;

                            // Track best recognized face
                            if (face.confidence > best_confidence) {
                                best_confidence = face.confidence;
                                best_person_name = face.name;
                                best_face = face;  // Store best face
                                has_best_face = true;

                                // Cache result for continuous display
                                last_recognized_name = face.name;
                                last_recognized_confidence = face.confidence;
                                has_recognition_result = true;
                            }
                        } else {
                            // Set name to "Unknown" for unrecognized faces
                            face.name = "Unknown";
                            face.confidence = confidence * 100.0;  // Store confidence even for unknown
                            face.id = -1;
                            unknown_count++;

                            // Don't cache unknown results - keep the last successful recognition
                            // This prevents flickering between recognized and unknown states

                            // Track best unknown face as fallback
                            if (!has_best_face && face.confidence > best_confidence) {
                                best_confidence = face.confidence;
                                best_person_name = face.name;
                                best_face = face;
                            }
                        }
                    }
                } else if (!faces.empty()) {
                    // Use cached recognition result if available, or show as unknown
                    // Find the largest face
                    int best_idx = 0;
                    int max_size = 0;
                    for (size_t i = 0; i < faces.size(); ++i) {
                        int face_size = faces[i].bbox.width * faces[i].bbox.height;
                        if (face_size > max_size) {
                            max_size = face_size;
                            best_idx = i;
                        }
                    }

                    // Use cached result for display
                    if (has_recognition_result && face_recognition_enabled) {
                        faces[best_idx].name = last_recognized_name;
                        faces[best_idx].confidence = last_recognized_confidence;
                        faces[best_idx].id = (last_recognized_name != "Unknown") ? 1 : -1;

                        if (last_recognized_name != "Unknown") {
                            recognized_count = 1;
                            best_confidence = last_recognized_confidence;
                            best_person_name = last_recognized_name;
                        } else {
                            unknown_count = 1;
                        }
                    } else {
                        faces[best_idx].name = "Unknown";
                        faces[best_idx].confidence = 0.0;
                        faces[best_idx].id = -1;
                        unknown_count = 1;
                    }

                    best_face = faces[best_idx];
                    has_best_face = true;
                }

                // Update UI with recognized person and confidence
                if (recognized_count > 0) {
                    gchar person_text[100];
                    g_snprintf(person_text, sizeof(person_text), "Person: %s (%d face%s)",
                              best_person_name.c_str(), recognized_count,
                              recognized_count > 1 ? "s" : "");
                    gtk_label_set_text(GTK_LABEL(face_info_label), person_text);

                    gchar conf_text[100];
                    g_snprintf(conf_text, sizeof(conf_text), "Confidence: %.1f%%", best_confidence);
                    gtk_label_set_text(GTK_LABEL(face_count_label), conf_text);
                } else if (unknown_count > 0) {
                    gchar person_text[100];
                    g_snprintf(person_text, sizeof(person_text), "Unknown: %d face%s detected",
                              unknown_count, unknown_count > 1 ? "s" : "");
                    gtk_label_set_text(GTK_LABEL(face_info_label), person_text);
                    gtk_label_set_text(GTK_LABEL(face_count_label), "Confidence: N/A");
                } else {
                    gtk_label_set_text(GTK_LABEL(face_info_label), "Person: None detected");
                    gtk_label_set_text(GTK_LABEL(face_count_label), "Confidence: 0%");
                }

                // Save clean frame for capture (BEFORE drawing on it)
                last_frame = frame.clone();

                // Draw only the best face on frame for display
                if (has_best_face) {
                    std::vector<Face> best_faces;
                    best_faces.push_back(best_face);
                    draw_faces_on_frame(frame, best_faces);
                }

                // Convert to pixbuf and display
                GdkPixbuf* pixbuf = mat_to_pixbuf(frame);
                if (pixbuf != nullptr) {
                    gtk_image_set_from_pixbuf(GTK_IMAGE(image_widget), pixbuf);
                    g_object_unref(pixbuf);
                }

                // Update FPS counter
                frame_count++;
                gint64 current_time = g_get_monotonic_time();
                if (last_time == 0) {
                    last_time = current_time;
                }

                gint64 elapsed_us = current_time - last_time;
                if (elapsed_us >= 1000000) { // 1 second
                    double fps = (frame_count * 1000000.0) / elapsed_us;
                    gchar fps_text[50];
                    g_snprintf(fps_text, sizeof(fps_text), "FPS: %.1f", fps);
                    gtk_label_set_text(GTK_LABEL(fps_label), fps_text);

                    // Update detection error rate metrics
                    double detection_rate = face_detector.get_detection_rate();
                    double false_positive_rate = face_detector.get_false_positive_rate();
                    gchar error_rate_text[100];
                    g_snprintf(error_rate_text, sizeof(error_rate_text),
                              "Detection: %.1f%% | Error: %.1f%%",
                              detection_rate, false_positive_rate);
                    gtk_label_set_text(GTK_LABEL(error_rate_label), error_rate_text);

                    frame_count = 0;
                    last_time = current_time;
                }
            }
        } else if (!camera.is_camera_active()) {
            // Camera was stopped or disconnected
            std::cout << "Camera disconnected" << std::endl;
            camera_running = false;
            gtk_button_set_label(GTK_BUTTON(toggle_button), "Start Camera");
            gtk_label_set_text(GTK_LABEL(status_label), "Status: Camera Disconnected");
            gtk_image_clear(GTK_IMAGE(image_widget));
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception in refresh_frame: " << e.what() << std::endl;
        camera_running = false;
        gtk_button_set_label(GTK_BUTTON(toggle_button), "Start Camera");
        gtk_label_set_text(GTK_LABEL(status_label), "Status: Error - Check console");
    }

    return TRUE; // Continue timer
}

void GTKApp::on_toggle_button_clicked(GtkWidget* /*widget*/, gpointer user_data) {
    GTKApp* self = static_cast<GTKApp*>(user_data);
    self->toggle_camera();
}

void GTKApp::toggle_camera() {
    try {
        if (!camera_running) {
            // Start camera
            camera.start();
            camera_running = true;
            gtk_button_set_label(GTK_BUTTON(toggle_button), "Stop Camera");
            gtk_label_set_text(GTK_LABEL(status_label), "Status: Camera Running");
        } else {
            // Stop camera
            camera.stop();
            camera_running = false;
            gtk_button_set_label(GTK_BUTTON(toggle_button), "Start Camera");
            gtk_label_set_text(GTK_LABEL(status_label), "Status: Camera Stopped");
            gtk_image_clear(GTK_IMAGE(image_widget));
            gtk_label_set_text(GTK_LABEL(fps_label), "FPS: 0");
            frame_count = 0;
            last_time = 0;
        }
    } catch (const std::exception& e) {
        std::cerr << "[ERROR] Exception while toggling camera: " << e.what() << std::endl;
        camera_running = false;
        gtk_button_set_label(GTK_BUTTON(toggle_button), "Start Camera");
        gtk_label_set_text(GTK_LABEL(status_label), "Status: Error - Check console");
    }
}

void GTKApp::on_window_destroy(GtkWidget* /*widget*/, gpointer user_data) {
    GTKApp* self = static_cast<GTKApp*>(user_data);
    self->cleanup();
    gtk_main_quit();
}

GdkPixbuf* GTKApp::mat_to_pixbuf(const cv::Mat& mat) {
    // Ensure the mat is in BGR format
    cv::Mat bgr_mat;
    if (mat.channels() == 1) {
        cv::cvtColor(mat, bgr_mat, cv::COLOR_GRAY2BGR);
    } else if (mat.channels() == 3) {
        bgr_mat = mat.clone();
    } else if (mat.channels() == 4) {
        cv::cvtColor(mat, bgr_mat, cv::COLOR_BGRA2BGR);
    } else {
        return nullptr;
    }

    // Convert BGR to RGB
    cv::Mat rgb_mat;
    cv::cvtColor(bgr_mat, rgb_mat, cv::COLOR_BGR2RGB);

    // Ensure continuous memory
    if (!rgb_mat.isContinuous()) {
        rgb_mat = rgb_mat.clone();
    }

    // Create GdkPixbuf directly with copied data
    GdkPixbuf* pixbuf = gdk_pixbuf_new(
        GDK_COLORSPACE_RGB,
        FALSE, // no alpha channel
        8,     // bits per sample
        rgb_mat.cols,
        rgb_mat.rows
    );

    if (pixbuf == nullptr) {
        std::cerr << "Failed to create pixbuf" << std::endl;
        return nullptr;
    }

    // Copy the data from cv::Mat to GdkPixbuf
    guchar* pixels = gdk_pixbuf_get_pixels(pixbuf);
    int rowstride = gdk_pixbuf_get_rowstride(pixbuf);

    for (int y = 0; y < rgb_mat.rows; y++) {
        memcpy(pixels + y * rowstride, rgb_mat.ptr(y), rgb_mat.cols * 3);
    }

    return pixbuf;
}

void GTKApp::draw_faces_on_frame(cv::Mat& frame, const std::vector<Face>& faces) {
    try {
        for (const auto& face : faces) {
            // Use fixed size bounding box, centered on the detected face
            int face_center_x = face.bbox.x + face.bbox.width / 2;
            int face_center_y = face.bbox.y + face.bbox.height / 2;

            cv::Rect expanded_bbox(
                face_center_x - FIXED_BOX_WIDTH / 2,
                face_center_y - FIXED_BOX_HEIGHT / 2,
                FIXED_BOX_WIDTH,
                FIXED_BOX_HEIGHT
            );

            // Draw corner lines only (horizontal and vertical lines at each corner)
            int corner_length = static_cast<int>(FIXED_BOX_WIDTH * 0.15); // 15% of fixed width for corner length
            int line_thickness = 2;

            // Use different colors based on confidence: Green for high confidence, Yellow for low
            cv::Scalar color = (face.confidence > 60.0) ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 255, 255);

            // Top-left corner
            // Horizontal line
            cv::line(frame,
                    cv::Point(expanded_bbox.x, expanded_bbox.y),
                    cv::Point(expanded_bbox.x + corner_length, expanded_bbox.y),
                    color, line_thickness);
            // Vertical line
            cv::line(frame,
                    cv::Point(expanded_bbox.x, expanded_bbox.y),
                    cv::Point(expanded_bbox.x, expanded_bbox.y + corner_length),
                    color, line_thickness);

            // Top-right corner
            // Horizontal line
            cv::line(frame,
                    cv::Point(expanded_bbox.x + expanded_bbox.width, expanded_bbox.y),
                    cv::Point(expanded_bbox.x + expanded_bbox.width - corner_length, expanded_bbox.y),
                    color, line_thickness);
            // Vertical line
            cv::line(frame,
                    cv::Point(expanded_bbox.x + expanded_bbox.width, expanded_bbox.y),
                    cv::Point(expanded_bbox.x + expanded_bbox.width, expanded_bbox.y + corner_length),
                    color, line_thickness);

            // Bottom-left corner
            // Horizontal line
            cv::line(frame,
                    cv::Point(expanded_bbox.x, expanded_bbox.y + expanded_bbox.height),
                    cv::Point(expanded_bbox.x + corner_length, expanded_bbox.y + expanded_bbox.height),
                    color, line_thickness);
            // Vertical line
            cv::line(frame,
                    cv::Point(expanded_bbox.x, expanded_bbox.y + expanded_bbox.height),
                    cv::Point(expanded_bbox.x, expanded_bbox.y + expanded_bbox.height - corner_length),
                    color, line_thickness);

            // Bottom-right corner
            // Horizontal line
            cv::line(frame,
                    cv::Point(expanded_bbox.x + expanded_bbox.width, expanded_bbox.y + expanded_bbox.height),
                    cv::Point(expanded_bbox.x + expanded_bbox.width - corner_length, expanded_bbox.y + expanded_bbox.height),
                    color, line_thickness);
            // Vertical line
            cv::line(frame,
                    cv::Point(expanded_bbox.x + expanded_bbox.width, expanded_bbox.y + expanded_bbox.height),
                    cv::Point(expanded_bbox.x + expanded_bbox.width, expanded_bbox.y + expanded_bbox.height - corner_length),
                    color, line_thickness);

            // Draw label with name and confidence
            std::string label = face.name;
            if (face.confidence > 0) {
                label += " (" + std::to_string(static_cast<int>(face.confidence)) + "%)";
            }

            int baseline = 0;
            cv::Size text_size = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseline);

            // Use different background color based on confidence and recognition status
            cv::Scalar bg_color;
            if (face.name == "Unknown") {
                bg_color = cv::Scalar(0, 255, 255);  // Yellow background for unknown faces
            } else if (face.confidence > 60.0) {
                bg_color = cv::Scalar(0, 255, 0);   // Green background for recognized faces
            } else {
                bg_color = cv::Scalar(0, 255, 255);  // Yellow background for low confidence
            }

            // Draw background for text (above the face area)
            cv::rectangle(frame,
                         cv::Point(expanded_bbox.x, expanded_bbox.y - text_size.height - 5),
                         cv::Point(expanded_bbox.x + text_size.width, expanded_bbox.y),
                         bg_color, -1);

            // Draw text
            cv::putText(frame, label,
                       cv::Point(expanded_bbox.x, expanded_bbox.y - 5),
                       cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception in draw_faces_on_frame: " << e.what() << std::endl;
    }
}

void GTKApp::load_face_recognizer() {
    try {
        std::cout << "Loading face recognizer (Deep Learning - FaceNet + FAISS)..." << std::endl;

        // Initialize database
        if (!face_database.open()) {
            std::cerr << "Failed to open face database" << std::endl;
            return;
        }

        if (!face_database.initialize()) {
            std::cerr << "Failed to initialize face database" << std::endl;
            face_database.close();
            return;
        }

        // Initialize face detector
        if (!face_detector.initialize()) {
            std::cerr << "Failed to initialize face detector" << std::endl;
            face_database.close();
            return;
        }

        // Set database reference in recognizer
        face_recognizer.set_database(&face_database);

        // Load ArcFace ONNX model (InsightFace w600k_r50)
        std::string model_path = "models/arcface_w600k_r50.onnx";
        if (!std::filesystem::exists(model_path)) {
            std::cerr << "Warning: ArcFace model not found at " << model_path << std::endl;
            std::cerr << "Please download the model and place it at: " << model_path << std::endl;
            std::cerr << "Visit: https://huggingface.co/public-data/insightface/tree/main/models/buffalo_l" << std::endl;
            face_recognition_enabled = false;
            return;
        }

        std::cout << "Loading ArcFace model from: " << model_path << std::endl;
        if (!face_recognizer.load_model(model_path)) {
            std::cerr << "Failed to load ArcFace model" << std::endl;
            face_recognition_enabled = false;
            return;
        }

        std::cout << "ArcFace model loaded successfully" << std::endl;

        // Try to load saved FAISS index first (faster startup)
        std::string faiss_index_path = "faiss_index.bin";
        if (std::filesystem::exists(faiss_index_path)) {
            std::cout << "Loading saved FAISS index from: " << faiss_index_path << std::endl;
            if (face_recognizer.load_index(faiss_index_path)) {
                face_recognition_enabled = true;
                std::cout << "FAISS index loaded successfully" << std::endl;
                std::cout << "Number of people in database: " << face_database.get_num_people() << std::endl;
                std::cout << "Face recognition ready!" << std::endl;
                return;
            } else {
                std::cerr << "Failed to load FAISS index, will try training from database" << std::endl;
            }
        }

        // Fallback: Try to train from database embeddings
        if (face_database.get_total_faces() > 0) {
            std::cout << "Loading face embeddings from database..." << std::endl;
            if (face_recognizer.train_from_database()) {
                face_recognition_enabled = true;
                std::cout << "Face recognizer loaded successfully" << std::endl;
                std::cout << "Number of people in database: " << face_database.get_num_people() << std::endl;
                std::cout << "Total faces in database: " << face_database.get_total_faces() << std::endl;
            } else {
                std::cerr << "Failed to train from database" << std::endl;
                face_recognition_enabled = false;
            }
        } else {
            std::cout << "No face data in database yet. Add photos to start recognizing faces." << std::endl;
            face_recognition_enabled = false;
        }

    } catch (const std::exception& e) {
        std::cerr << "Exception in load_face_recognizer: " << e.what() << std::endl;
        face_recognition_enabled = false;
    }
}

void GTKApp::on_train_button_clicked(GtkWidget* /*widget*/, gpointer user_data) {
    GTKApp* self = static_cast<GTKApp*>(user_data);
    self->train_model();
}

void GTKApp::train_model() {
    if (training_in_progress) {
        gtk_label_set_text(GTK_LABEL(status_label), "Status: Training already in progress");
        return;
    }

    // Check if model is loaded before attempting training
    if (!face_recognizer.is_model_loaded()) {
        GtkWidget* error_dialog = gtk_message_dialog_new(
            GTK_WINDOW(window),
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "ArcFace Model Not Loaded");
        gtk_message_dialog_format_secondary_text(
            GTK_MESSAGE_DIALOG(error_dialog),
            "Cannot train the model because the ArcFace ONNX model is missing or failed to load.\n\n"
            "Please download the ArcFace ONNX model to models/arcface_w600k_r50.onnx\n"
            "Visit: https://huggingface.co/public-data/insightface");
        gtk_dialog_run(GTK_DIALOG(error_dialog));
        gtk_widget_destroy(error_dialog);
        gtk_label_set_text(GTK_LABEL(status_label), "Status: Model not loaded - cannot train");
        return;
    }

    // Check if dataset directory exists and has subdirectories
    if (!std::filesystem::exists("dataset")) {
        gtk_label_set_text(GTK_LABEL(status_label), "Status: Dataset directory not found");
        return;
    }

    training_in_progress = true;
    gtk_widget_set_sensitive(train_button, FALSE);
    gtk_label_set_text(GTK_LABEL(status_label), "Status: Training model from dataset... please wait");

    std::cout << "Starting training from dataset..." << std::endl;

    // Join previous training thread if it exists
    if (training_thread.joinable()) {
        training_thread.join();
    }

    // Start training in background thread
    training_thread = std::thread(&GTKApp::train_model_async, this);
}

void GTKApp::train_model_async() {
    // This runs in a background thread
    bool success = face_recognizer.train_from_images("dataset");
    training_success = success;

    // Schedule UI update on main thread
    g_idle_add(on_training_complete, this);
}

gboolean GTKApp::on_training_complete(gpointer user_data) {
    GTKApp* self = static_cast<GTKApp*>(user_data);
    self->on_training_finished();
    return FALSE; // Remove from idle handlers
}

void GTKApp::on_training_finished() {
    if (training_success) {
        gtk_label_set_text(GTK_LABEL(status_label), "Status: Training complete! Ready to recognize faces.");
        face_recognition_enabled = true;
        std::cout << "Training successful!" << std::endl;
    } else {
        gtk_label_set_text(GTK_LABEL(status_label), "Status: Training failed - add photos and try again");
        std::cerr << "Training failed" << std::endl;
        face_recognition_enabled = false;
    }

    training_in_progress = false;
    gtk_widget_set_sensitive(train_button, TRUE);
}

void GTKApp::on_capture_button_clicked(GtkWidget* /*widget*/, gpointer user_data) {
    GTKApp* self = static_cast<GTKApp*>(user_data);
    self->capture_photo();
}

void GTKApp::capture_photo() {
    if (!camera_running) {
        gtk_label_set_text(GTK_LABEL(status_label), "Status: Start camera before capturing");
        return;
    }

    if (last_frame.empty()) {
        gtk_label_set_text(GTK_LABEL(status_label), "Status: No frame available to capture");
        return;
    }

    // Check if model is loaded (not just trained)
    if (!face_recognizer.is_model_loaded()) {
        GtkWidget* error_dialog = gtk_message_dialog_new(
            GTK_WINDOW(window),
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "ArcFace Model Not Loaded");
        gtk_message_dialog_format_secondary_text(
            GTK_MESSAGE_DIALOG(error_dialog),
            "Cannot capture photos because the ArcFace model is missing or failed to load.\n\n"
            "Please download the ArcFace ONNX model to models/arcface_w600k_r50.onnx\n"
            "Visit: https://huggingface.co/public-data/insightface");
        gtk_dialog_run(GTK_DIALOG(error_dialog));
        gtk_widget_destroy(error_dialog);
        gtk_label_set_text(GTK_LABEL(status_label), "Status: Model not loaded - cannot capture");
        return;
    }

    // Create dataset directory if it doesn't exist
    if (!std::filesystem::exists("dataset")) {
        std::filesystem::create_directory("dataset");
    }

    // Ask user for person initial and ID via dialog
    GtkWidget* dialog = gtk_dialog_new_with_buttons(
        "Capture Photo",
        GTK_WINDOW(window),
        GTK_DIALOG_MODAL,
        "Cancel", GTK_RESPONSE_CANCEL,
        "OK", GTK_RESPONSE_OK,
        nullptr);

    GtkWidget* content_area = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

    GtkWidget* label1 = gtk_label_new("Person Initial (A, B, C, etc.):");
    gtk_box_pack_start(GTK_BOX(content_area), label1, FALSE, FALSE, 5);
    GtkWidget* entry_initial = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(entry_initial), 1);
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_initial), "A");
    gtk_box_pack_start(GTK_BOX(content_area), entry_initial, FALSE, FALSE, 5);
    gtk_widget_set_size_request(entry_initial, 100, 35);

    GtkWidget* label2 = gtk_label_new("Person ID (number):");
    gtk_box_pack_start(GTK_BOX(content_area), label2, FALSE, FALSE, 5);
    GtkWidget* entry_id = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_id), "1");
    gtk_box_pack_start(GTK_BOX(content_area), entry_id, FALSE, FALSE, 5);
    gtk_widget_set_size_request(entry_id, 100, 35);

    gtk_widget_show_all(dialog);
    
    // Pause live stream and face recognition AFTER showing dialog
    capture_in_progress = true;
    
    gint result = gtk_dialog_run(GTK_DIALOG(dialog));

    if (result == GTK_RESPONSE_OK) {
        const char* initial = gtk_entry_get_text(GTK_ENTRY(entry_initial));
        const char* id_str = gtk_entry_get_text(GTK_ENTRY(entry_id));

        if (initial && strlen(initial) > 0 && id_str && strlen(id_str) > 0) {
            // Create person subdirectory structure
            std::string initial_str(initial);
            std::string id_num(id_str);

            // Convert initial to uppercase
            initial_str[0] = std::toupper(initial_str[0]);

            // Create person-specific subdirectory: dataset/A1/, dataset/B2/, etc.
            std::string person_name = initial_str + id_num;  // e.g., "A1", "B2"
            std::string person_dir = "dataset/" + person_name;

            try {
                if (!std::filesystem::exists(person_dir)) {
                    std::filesystem::create_directories(person_dir);
                    std::cout << "Created person directory: " << person_dir << std::endl;
                }
            } catch (const std::exception& e) {
                gtk_label_set_text(GTK_LABEL(status_label), "Status: Failed to create person directory");
                std::cerr << "Error creating directory: " << e.what() << std::endl;
                return;
            }

            // Count existing files for this person to determine sequence number
            int sequence = 1;
            try {
                for (const auto& entry : std::filesystem::directory_iterator(person_dir)) {
                    if (entry.is_regular_file()) {
                        std::string ext = entry.path().extension().string();
                        if (ext == ".jpg" || ext == ".png" || ext == ".bmp") {
                            sequence++;
                        }
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "Error counting files: " << e.what() << std::endl;
            }

            // Generate filename: A1/1.jpg, A1/2.jpg, etc.
            std::string filename = person_dir + "/" + std::to_string(sequence) + ".jpg";

            // Save the frame
            if (cv::imwrite(filename, last_frame)) {
                // Register person in database if not already registered
                PersonRecord person;
                bool person_exists = face_database.get_person_by_name(person_name, person);
                
                if (!person_exists) {
                    if (face_database.add_person(person_name)) {
                        std::cout << "Person registered in database: " << person_name << std::endl;
                        // Get the newly created person
                        face_database.get_person_by_name(person_name, person);
                    } else {
                        std::cerr << "Failed to register person in database" << std::endl;
                        gtk_label_set_text(GTK_LABEL(status_label), "Status: Failed to register person");
                        gtk_widget_destroy(dialog);
                        return;
                    }
                }

                // Add face image to database (for record keeping)
                face_database.add_face_image(person.id, filename);
                
                // Just save the photo without training
                gchar status_text[200];
                g_snprintf(status_text, sizeof(status_text),
                          "Status: Photo saved - %s (Total: %d faces)", 
                          person_name.c_str(), face_database.get_total_faces());
                gtk_label_set_text(GTK_LABEL(status_label), status_text);
                std::cout << "Photo saved: " << filename << std::endl;
            } else {
                gtk_label_set_text(GTK_LABEL(status_label), "Status: Failed to save photo");
                std::cerr << "Failed to save photo" << std::endl;
            }
        } else {
            gtk_label_set_text(GTK_LABEL(status_label), "Status: Invalid input - please enter initial and ID");
        }
    }

    gtk_widget_destroy(dialog);
    
    // Resume live stream and face recognition
    capture_in_progress = false;
    
    // Force a few frame refreshes to ensure the display updates
    for (int i = 0; i < 3; i++) {
        while (gtk_events_pending()) {
            gtk_main_iteration();
        }
        g_usleep(10000); // 10ms delay
    }
    
    gtk_label_set_text(GTK_LABEL(status_label), "Status: Live stream resumed");
}

