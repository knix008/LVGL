#include "gtk_app.h"
#include <iostream>
#include <chrono>
#include <filesystem>

GTKApp::GTKApp()
    : window(nullptr), image_widget(nullptr), toggle_button(nullptr),
      train_button(nullptr), capture_button(nullptr), status_label(nullptr),
      fps_label(nullptr), face_info_label(nullptr), face_count_label(nullptr),
      refresh_timer(0), camera_running(false), face_recognition_enabled(false),
      training_in_progress(false), frame_count(0), last_time(0), capture_count(0) {}

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
    if (refresh_timer != 0) {
        g_source_remove(refresh_timer);
        refresh_timer = 0;
    }

    camera.close();

    if (window != nullptr) {
        gtk_widget_destroy(window);
        window = nullptr;
    }
}

gboolean GTKApp::on_refresh_timer(gpointer user_data) {
    GTKApp* self = static_cast<GTKApp*>(user_data);
    return self->refresh_frame();
}

gboolean GTKApp::refresh_frame() {
    if (!camera_running) {
        return TRUE; // Continue timer
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
                if (face_recognition_enabled && !faces.empty()) {
                    for (auto& face : faces) {
                        cv::Mat face_roi = frame(face.bbox);
                        double confidence = 0.0;
                        int label = face_recognizer.recognize(face_roi, confidence);

                        if (label != -1) {
                            face.name = face_recognizer.get_label_name(label);
                            face.confidence = 100.0 - confidence;  // Convert to percentage
                            face.id = label;
                            recognized_count++;

                            // Track best recognized face
                            if (face.confidence > best_confidence) {
                                best_confidence = face.confidence;
                                best_person_name = face.name;
                            }
                        } else {
                            unknown_count++;
                        }
                    }
                } else if (!faces.empty()) {
                    unknown_count = faces.size();
                }

                // Draw faces on frame
                if (!faces.empty()) {
                    draw_faces_on_frame(frame, faces);
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

                // Save current frame for capture
                last_frame = frame.clone();

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
            std::cout << "Camera started" << std::endl;
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
            std::cout << "Camera stopped" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception while toggling camera: " << e.what() << std::endl;
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

    // Create GdkPixbuf from the mat
    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_data(
        rgb_mat.data,
        GDK_COLORSPACE_RGB,
        FALSE, // no alpha channel
        8,     // bits per sample
        rgb_mat.cols,
        rgb_mat.rows,
        rgb_mat.step,
        nullptr, // destroy function
        nullptr  // user data
    );

    if (pixbuf == nullptr) {
        std::cerr << "Failed to create pixbuf from mat" << std::endl;
        return nullptr;
    }

    // We need to copy the data since the original mat will be deallocated
    GdkPixbuf* pixbuf_copy = gdk_pixbuf_copy(pixbuf);
    g_object_unref(pixbuf);

    return pixbuf_copy;
}

void GTKApp::draw_faces_on_frame(cv::Mat& frame, const std::vector<Face>& faces) {
    try {
        for (const auto& face : faces) {
            // Increase face area by 30% to hide the face itself
            int width = face.bbox.width;
            int height = face.bbox.height;
            int new_width = static_cast<int>(width * 1.3);
            int new_height = static_cast<int>(height * 1.3);

            // Calculate centered expansion
            int x_offset = (new_width - width) / 2;
            int y_offset = (new_height - height) / 2;

            cv::Rect expanded_bbox(
                face.bbox.x - x_offset,
                face.bbox.y - y_offset,
                new_width,
                new_height
            );

            // Draw corner lines only (horizontal and vertical lines at each corner)
            int corner_length = static_cast<int>(new_width * 0.15); // 15% of width for corner length
            int line_thickness = 2;
            cv::Scalar color = cv::Scalar(0, 255, 0); // Green color

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

            // Draw face label with name and confidence
            std::string label = face.name;
            if (face.confidence > 0) {
                label += " (" + std::to_string(static_cast<int>(face.confidence)) + "%)";
            }

            int baseline = 0;
            cv::Size text_size = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseline);

            // Draw background for text (above the face area)
            cv::rectangle(frame,
                         cv::Point(expanded_bbox.x, expanded_bbox.y - text_size.height - 5),
                         cv::Point(expanded_bbox.x + text_size.width, expanded_bbox.y),
                         cv::Scalar(0, 255, 0), -1);

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
        std::cout << "Loading face recognizer..." << std::endl;

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

        // Try to load existing model
        if (!face_recognizer.load_model("face_recognizer_model.yml")) {
            std::cout << "No existing model found, face recognition disabled until model is trained" << std::endl;
            face_recognition_enabled = false;
            return;
        }

        face_recognition_enabled = true;
        std::cout << "Face recognizer loaded successfully" << std::endl;
        std::cout << "Number of people in database: " << face_database.get_num_people() << std::endl;
        std::cout << "Total faces in database: " << face_database.get_total_faces() << std::endl;

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

    training_in_progress = true;
    gtk_widget_set_sensitive(train_button, FALSE);
    gtk_label_set_text(GTK_LABEL(status_label), "Status: Training model from dataset... please wait");

    std::cout << "Starting training from dataset..." << std::endl;

    // Train using the dataset folder (person subdirectories)
    // This reads from dataset/A1/, dataset/B2/, etc.
    bool success = face_recognizer.train_from_images("dataset");

    if (success) {
        // Save the trained model
        if (face_recognizer.save_model("face_recognizer_model.yml")) {
            gtk_label_set_text(GTK_LABEL(status_label), "Status: Training complete! Model saved.");
            face_recognition_enabled = true;
            std::cout << "Training successful! Model saved." << std::endl;
        } else {
            gtk_label_set_text(GTK_LABEL(status_label), "Status: Training failed - could not save model");
            std::cerr << "Failed to save model" << std::endl;
        }
    } else {
        gtk_label_set_text(GTK_LABEL(status_label), "Status: Training failed - check dataset folder or add photos first");
        std::cerr << "Training failed" << std::endl;
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
                if (!face_database.person_exists(person_name)) {
                    if (face_database.add_person(person_name)) {
                        std::cout << "Person registered in database: " << person_name << std::endl;
                    } else {
                        std::cerr << "Failed to register person in database" << std::endl;
                    }
                }

                // Add face image to database
                PersonRecord person;
                if (face_database.get_person_by_name(person_name, person)) {
                    if (face_database.add_face_image(person.id, filename)) {
                        std::cout << "Face image registered in database: " << filename << std::endl;
                    } else {
                        std::cerr << "Failed to register face image in database" << std::endl;
                    }
                }

                gchar status_text[200];
                g_snprintf(status_text, sizeof(status_text),
                          "Status: Photo saved - %s/%s (Person: %s)", person_name.c_str(),
                          std::to_string(sequence).c_str(), person_name.c_str());
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
}
