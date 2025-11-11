#include "gui.h"
#include <iostream>
#include <cstring>
#include <unistd.h>

// Include LVGL SDL2 driver headers
#include "lvgl/src/drivers/sdl/lv_sdl_window.h"
#include "lvgl/src/drivers/sdl/lv_sdl_mouse.h"
#include "lvgl/src/drivers/sdl/lv_sdl_keyboard.h"

GUI::GUI(int width, int height)
    : screen_width(width), screen_height(height), is_running(true),
      main_screen(nullptr), image_label(nullptr), image_canvas(nullptr),
      status_label(nullptr), info_label(nullptr), canvas_buffer(nullptr) {
}

GUI::~GUI() {
    // Don't try to free canvas_buffer here
    // When LVGL calls lv_deinit(), it deallocates the entire memory pool
    // Trying to free individual allocations after lv_deinit() causes crashes
    // LVGL's memory cleanup is automatic and complete
    canvas_buffer = nullptr;
}

bool GUI::initialize() {
    // Initialize LVGL
    std::cout << "Initializing LVGL..." << std::endl;
    lv_init();

    // Initialize SDL2 display driver
    // LVGL with SDL2 requires the SDL2 window to be created
    std::cout << "Initializing SDL2 display driver..." << std::endl;
    if (!init_display_driver()) {
        std::cerr << "Failed to initialize SDL2 display driver" << std::endl;
        return false;
    }

    // Create input devices (SDL2 mouse and keyboard)
    std::cout << "Initializing input devices..." << std::endl;
    if (!init_input_device()) {
        std::cerr << "Failed to initialize input devices" << std::endl;
        return false;
    }

    // Create main screen and UI elements
    std::cout << "Creating main screen..." << std::endl;
    if (!create_main_screen()) {
        std::cerr << "Failed to create main screen" << std::endl;
        return false;
    }

    std::cout << "GUI initialized successfully" << std::endl;
    return true;
}

bool GUI::init_display_driver() {
    // Create SDL2 window for LVGL (matching FaceRecognition8.4 pattern)
    std::cout << "Creating SDL2 window (" << screen_width << "x" << screen_height << ")..." << std::endl;

    lv_display_t* disp = lv_sdl_window_create(screen_width, screen_height);

    if (disp == nullptr) {
        std::cerr << "Failed to create SDL2 window" << std::endl;
        std::cerr << "Ensure SDL2 is installed: apt-get install libsdl2-dev" << std::endl;
        std::cerr << "Ensure X11 display is available (set DISPLAY environment variable)" << std::endl;
        return false;
    }

    // Set window title
    lv_sdl_window_set_title(disp, "Face Recognition Application");

    std::cout << "SDL2 window created successfully (" << screen_width << "x" << screen_height << ")" << std::endl;
    return true;
}

bool GUI::init_input_device() {
    // Initialize SDL2 input devices (mouse and keyboard)
    // Based on FaceRecognition8.4 pattern - warnings only, not critical

    std::cout << "Initializing SDL2 mouse..." << std::endl;
    lv_indev_t* mouse = lv_sdl_mouse_create();
    if (mouse == nullptr) {
        std::cerr << "Warning: Failed to create SDL2 mouse input device" << std::endl;
        // Not critical, continue anyway
    } else {
        std::cout << "SDL2 mouse created successfully" << std::endl;
    }

    std::cout << "Initializing SDL2 keyboard..." << std::endl;
    lv_indev_t* keyboard = lv_sdl_keyboard_create();
    if (keyboard == nullptr) {
        std::cerr << "Warning: Failed to create SDL2 keyboard input device" << std::endl;
        // Not critical, continue anyway
    } else {
        std::cout << "SDL2 keyboard created successfully" << std::endl;
    }

    std::cout << "SDL2 input devices initialized" << std::endl;
    return true;  // Return true even if devices failed - display is more important
}

bool GUI::create_main_screen() {
    // Get or create default screen
    main_screen = lv_screen_active();
    if (main_screen == nullptr) {
        std::cerr << "Failed to get active screen" << std::endl;
        return false;
    }

    // Set background color
    lv_obj_set_style_bg_color(main_screen, lv_color_make(240, 240, 240), 0);

    // Create title label
    lv_obj_t* title = lv_label_create(main_screen);
    lv_label_set_text(title, "Face Recognition Application");
    lv_obj_set_width(title, screen_width);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 10);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);

    // Create image display area with canvas (smaller to fit in memory)
    // Layout: Title(10) -> Canvas(40-220) -> Info(230) -> Status(260) -> Buttons(310-410)
    int img_width = screen_width - 20;   // 300px
    int img_height = 180;                 // Reduced from 300px to 180px (180*300*3 = 162KB)

    image_canvas = lv_canvas_create(main_screen);
    lv_obj_set_size(image_canvas, img_width, img_height);
    lv_obj_align(image_canvas, LV_ALIGN_TOP_MID, 0, 40);  // Below title
    lv_obj_set_style_bg_color(image_canvas, lv_color_make(200, 200, 200), 0);
    lv_obj_set_style_border_color(image_canvas, lv_color_black(), 0);
    lv_obj_set_style_border_width(image_canvas, 1, 0);

    // Create label for image info (below canvas)
    image_label = lv_label_create(main_screen);
    lv_label_set_text(image_label, "No image loaded");
    lv_obj_set_width(image_label, screen_width - 20);
    lv_obj_align(image_label, LV_ALIGN_TOP_MID, 0, 230);  // Below canvas (40+180+10)

    // Create status label (below image info)
    status_label = lv_label_create(main_screen);
    lv_label_set_text(status_label, "Ready");
    lv_obj_set_width(status_label, screen_width - 20);
    lv_obj_align(status_label, LV_ALIGN_TOP_MID, 0, 260);  // Below image label

    // Create info label (below status)
    info_label = lv_label_create(main_screen);
    lv_label_set_text(info_label, "No faces detected");
    lv_obj_set_width(info_label, screen_width - 20);
    lv_obj_align(info_label, LV_ALIGN_TOP_MID, 0, 290);  // Below status label

    // Create buttons
    if (!create_buttons()) {
        std::cerr << "Failed to create buttons" << std::endl;
        return false;
    }

    return true;
}

bool GUI::create_buttons() {
    // 2x2 grid layout for 320px width
    // Buttons positioned at bottom of screen (avoiding label overlap)
    int button_width = (screen_width - 60) / 2;  // (320 - 60) / 2 = 130px per button
    int button_height = 50;
    int padding = 10;
    int start_x = 15;
    int start_y = screen_height - 140;  // Move buttons higher on screen with better spacing

    // Load Image button (top-left)
    lv_obj_t* load_btn = lv_btn_create(main_screen);
    lv_obj_set_size(load_btn, button_width, button_height);
    lv_obj_set_pos(load_btn, start_x, start_y);
    lv_obj_t* load_label = lv_label_create(load_btn);
    lv_label_set_text(load_label, "Load");
    lv_obj_center(load_label);
    lv_obj_add_event_cb(load_btn, load_image_btn_event_cb, LV_EVENT_CLICKED, this);

    // Detect Face button (top-right)
    lv_obj_t* detect_btn = lv_btn_create(main_screen);
    lv_obj_set_size(detect_btn, button_width, button_height);
    lv_obj_set_pos(detect_btn, start_x + button_width + padding, start_y);
    lv_obj_t* detect_label = lv_label_create(detect_btn);
    lv_label_set_text(detect_label, "Detect");
    lv_obj_center(detect_label);
    lv_obj_add_event_cb(detect_btn, detect_faces_btn_event_cb, LV_EVENT_CLICKED, this);

    // Register button (bottom-left)
    lv_obj_t* register_btn = lv_btn_create(main_screen);
    lv_obj_set_size(register_btn, button_width, button_height);
    lv_obj_set_pos(register_btn, start_x, start_y + button_height + padding);
    lv_obj_t* register_label = lv_label_create(register_btn);
    lv_label_set_text(register_label, "Register");
    lv_obj_center(register_label);
    lv_obj_add_event_cb(register_btn, register_person_btn_event_cb, LV_EVENT_CLICKED, this);

    // Recognize button (bottom-right)
    lv_obj_t* recognize_btn = lv_btn_create(main_screen);
    lv_obj_set_size(recognize_btn, button_width, button_height);
    lv_obj_set_pos(recognize_btn, start_x + button_width + padding, start_y + button_height + padding);
    lv_obj_t* recognize_label = lv_label_create(recognize_btn);
    lv_label_set_text(recognize_label, "Recognize");
    lv_obj_center(recognize_label);
    lv_obj_add_event_cb(recognize_btn, recognize_person_btn_event_cb, LV_EVENT_CLICKED, this);

    return true;
}

void GUI::load_image_btn_event_cb(lv_event_t* e) {
    GUI* gui = static_cast<GUI*>(lv_event_get_user_data(e));
    if (gui && gui->on_load_image) {
        gui->on_load_image();
    }
}

void GUI::detect_faces_btn_event_cb(lv_event_t* e) {
    GUI* gui = static_cast<GUI*>(lv_event_get_user_data(e));
    if (gui && gui->on_detect_faces) {
        gui->on_detect_faces();
    }
}

void GUI::register_person_btn_event_cb(lv_event_t* e) {
    GUI* gui = static_cast<GUI*>(lv_event_get_user_data(e));
    if (gui && gui->on_register_person) {
        gui->on_register_person();
    }
}

void GUI::recognize_person_btn_event_cb(lv_event_t* e) {
    GUI* gui = static_cast<GUI*>(lv_event_get_user_data(e));
    if (gui && gui->on_recognize_person) {
        gui->on_recognize_person();
    }
}

bool GUI::display_image(const cv::Mat& image) {
    if (image.empty()) {
        std::cerr << "Empty image provided" << std::endl;
        return false;
    }

    // Store current image
    current_display_image = image.clone();

    // Resize image to fit canvas
    cv::Mat display_image = image.clone();
    int canvas_width = screen_width - 20;   // 300px
    int canvas_height = 180;                 // Fixed canvas height

    // Always resize if image is larger than canvas
    if (display_image.cols > canvas_width || display_image.rows > canvas_height) {
        float scale = std::min(static_cast<float>(canvas_width) / display_image.cols,
                              static_cast<float>(canvas_height) / display_image.rows);
        int new_width = static_cast<int>(display_image.cols * scale);
        int new_height = static_cast<int>(display_image.rows * scale);

        // Ensure minimum size
        if (new_width < 10) new_width = 10;
        if (new_height < 10) new_height = 10;

        cv::resize(display_image, display_image, cv::Size(new_width, new_height), 0, 0, cv::INTER_LINEAR);
    }

    // Ensure image is in RGB format (3 channels)
    if (display_image.channels() == 1) {
        cv::cvtColor(display_image, display_image, cv::COLOR_GRAY2RGB);
    } else if (display_image.channels() == 3) {
        cv::cvtColor(display_image, display_image, cv::COLOR_BGR2RGB);
    } else if (display_image.channels() == 4) {
        cv::cvtColor(display_image, display_image, cv::COLOR_BGRA2RGB);
    }

    int width = display_image.cols;
    int height = display_image.rows;
    int buffer_size = width * height * 3;

    // Draw image on canvas using LVGL's canvas buffer management
    if (image_canvas) {
        // Allocate new buffer for the image
        // Note: We don't manually free old buffers - LVGL's lv_deinit() handles all cleanup
        uint8_t* new_buffer = nullptr;

        if (lv_is_initialized()) {
            new_buffer = (uint8_t*)lv_malloc(buffer_size);

            if (new_buffer == nullptr) {
                std::cerr << "Failed to allocate canvas buffer (" << buffer_size << " bytes)" << std::endl;
                std::cerr << "  Image size: " << width << "x" << height << std::endl;
                return false;
            }
        } else {
            std::cerr << "LVGL not initialized, cannot allocate buffer" << std::endl;
            return false;
        }

        // Update to new buffer (old buffer will be cleaned up by LVGL's lv_deinit())
        canvas_buffer = new_buffer;

        // Copy image data to buffer
        std::memcpy(canvas_buffer, display_image.data, buffer_size);

        // Set the canvas buffer with the image data
        // LV_COLOR_FORMAT_RGB888 = 3 bytes per pixel (RGB)
        lv_canvas_set_buffer(image_canvas, canvas_buffer, width, height, LV_COLOR_FORMAT_RGB888);

        // Resize canvas to match image dimensions
        lv_obj_set_size(image_canvas, width, height);
        lv_obj_align(image_canvas, LV_ALIGN_TOP_MID, 0, 40);

        // Update image info label
        std::string img_info = "Image: " + std::to_string(width) + "x" + std::to_string(height);
        if (image_label) {
            lv_label_set_text(image_label, img_info.c_str());
        }

        // Update status
        update_status("Image displayed: " + std::to_string(width) + "x" + std::to_string(height));
    }

    return true;
}

bool GUI::display_detection_result(const cv::Mat& image, const std::vector<Face>& faces) {
    if (image.empty()) {
        return false;
    }

    std::string info = "Detected " + std::to_string(faces.size()) + " faces";
    lv_label_set_text(info_label, info.c_str());

    return display_image(image);
}

void GUI::show_recognition_result(const RecognitionResult& result) {
    std::string message = "Person: " + result.person_name + "\n" +
                         "Confidence: " + std::to_string(static_cast<int>(result.confidence * 100)) + "%";

    if (result.is_registered) {
        show_success_message("Recognition Result", message);
    } else {
        show_error_message("Recognition Result", "Person not recognized");
    }
}

void GUI::show_error_message(const std::string& title, const std::string& message) {
    // Create a custom dialog with title, message, and close button
    lv_obj_t* dialog = lv_obj_create(lv_scr_act());
    lv_obj_set_size(dialog, 280, 200);
    lv_obj_center(dialog);

    // Add background color
    lv_obj_set_style_bg_color(dialog, lv_color_make(200, 200, 200), 0);
    lv_obj_set_style_border_color(dialog, lv_color_black(), 0);
    lv_obj_set_style_border_width(dialog, 2, 0);
    lv_obj_set_style_radius(dialog, 8, 0);

    // Add title label
    lv_obj_t* title_label = lv_label_create(dialog);
    lv_label_set_text(title_label, title.c_str());
    lv_obj_set_style_text_font(title_label, &lv_font_montserrat_14, 0);
    lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 10);

    // Add message label
    lv_obj_t* msg_label = lv_label_create(dialog);
    lv_label_set_text(msg_label, message.c_str());
    lv_obj_set_width(msg_label, 260);
    lv_obj_align(msg_label, LV_ALIGN_TOP_MID, 0, 40);
    lv_label_set_long_mode(msg_label, LV_LABEL_LONG_WRAP);

    // Add OK button
    lv_obj_t* ok_btn = lv_btn_create(dialog);
    lv_obj_set_size(ok_btn, 80, 40);
    lv_obj_align(ok_btn, LV_ALIGN_BOTTOM_MID, 0, -10);

    lv_obj_t* btn_label = lv_label_create(ok_btn);
    lv_label_set_text(btn_label, "OK");
    lv_obj_center(btn_label);

    // Add event handler to close dialog when button is clicked
    lv_obj_add_event_cb(ok_btn, [](lv_event_t* e) {
        lv_obj_t* dialog = (lv_obj_t*)lv_event_get_user_data(e);
        lv_obj_del(dialog);
    }, LV_EVENT_CLICKED, dialog);
}

void GUI::show_success_message(const std::string& title, const std::string& message) {
    // Same as error message for now
    show_error_message(title, message);
}

std::string GUI::show_file_browser(const std::string& /* initial_path */) {
    // For now, return a simple path
    // In a full implementation, this would show a file selection dialog
    return "";
}

bool GUI::show_registration_dialog(std::string& person_id, std::string& person_name) {
    // For now, return dummy values
    // In a full implementation, this would show input dialogs
    person_id = "person_1";
    person_name = "Person 1";
    return true;
}

void GUI::set_load_image_callback(std::function<void()> callback) {
    on_load_image = callback;
}

void GUI::set_detect_faces_callback(std::function<void()> callback) {
    on_detect_faces = callback;
}

void GUI::set_register_person_callback(std::function<void()> callback) {
    on_register_person = callback;
}

void GUI::set_recognize_person_callback(std::function<void()> callback) {
    on_recognize_person = callback;
}

void GUI::run() {
    while (is_running) {
        // Handle LVGL tasks - returns time until next task
        uint32_t time_till_next = lv_timer_handler();

        // Check if LVGL has been deinitialized (window closed)
        // When window close button is clicked, LVGL automatically calls lv_deinit()
        if (!lv_is_initialized()) {
            std::cout << "Window closed, exiting main loop..." << std::endl;
            is_running = false;
            break;
        }

        // Sleep until next task is due (max 5ms delay)
        usleep((time_till_next < 5 ? time_till_next : 5) * 1000);
    }
}

void GUI::stop() {
    is_running = false;
}

void GUI::update_status(const std::string& message) {
    if (status_label) {
        lv_label_set_text(status_label, message.c_str());
    }
}
