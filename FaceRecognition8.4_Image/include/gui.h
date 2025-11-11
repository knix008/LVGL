#ifndef GUI_H
#define GUI_H

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <opencv2/opencv.hpp>
#include "common.h"
#include "lvgl.h"

// Forward declarations
class FaceDetector;
class FaceRecognizer;
class FaceDatabase;

class GUI {
public:
    GUI(int width = 800, int height = 600);
    ~GUI();

    // Initialize GUI
    bool initialize();

    // Create main screen and UI elements
    bool create_main_screen();

    // Display image on screen
    bool display_image(const cv::Mat& image);

    // Display detected faces on image
    bool display_detection_result(const cv::Mat& image, const std::vector<Face>& faces);

    // Show recognition result dialog
    void show_recognition_result(const RecognitionResult& result);

    // Show error message dialog
    void show_error_message(const std::string& title, const std::string& message);

    // Show success message dialog
    void show_success_message(const std::string& title, const std::string& message);

    // Show file browser for image selection
    std::string show_file_browser(const std::string& initial_path = "./");

    // Show person registration dialog
    bool show_registration_dialog(std::string& person_id, std::string& person_name);

    // Set button callbacks
    void set_load_image_callback(std::function<void()> callback);
    void set_detect_faces_callback(std::function<void()> callback);
    void set_register_person_callback(std::function<void()> callback);
    void set_recognize_person_callback(std::function<void()> callback);

    // Main event loop
    void run();

    // Get display size
    int get_width() const { return screen_width; }
    int get_height() const { return screen_height; }

    // Stop the application
    void stop();

    // Update status label (public interface)
    void update_status(const std::string& message);

private:
    int screen_width;
    int screen_height;
    bool is_running;
    lv_obj_t* main_screen;
    lv_obj_t* image_label;      // For displaying image
    lv_obj_t* image_canvas;     // Canvas for actual image display
    lv_obj_t* status_label;     // For status messages
    lv_obj_t* info_label;       // For information display
    cv::Mat current_display_image;  // Store current displayed image
    uint8_t* canvas_buffer;     // Track allocated canvas buffer for cleanup

    // Callback functions
    std::function<void()> on_load_image;
    std::function<void()> on_detect_faces;
    std::function<void()> on_register_person;
    std::function<void()> on_recognize_person;

    // Button event handlers
    static void load_image_btn_event_cb(lv_event_t* e);
    static void detect_faces_btn_event_cb(lv_event_t* e);
    static void register_person_btn_event_cb(lv_event_t* e);
    static void recognize_person_btn_event_cb(lv_event_t* e);

    // Helper functions
    bool init_display_driver();
    bool init_input_device();
    bool create_buttons();
    lv_img_dsc_t* cv_mat_to_lvgl_image(const cv::Mat& image);
    cv::Mat lvgl_image_to_cv_mat(const lv_img_dsc_t* img_dsc);
};

#endif // GUI_H
