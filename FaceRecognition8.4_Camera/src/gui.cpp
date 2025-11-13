#include "gui.h"
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <SDL2/SDL.h>

// LVGL 8.4 image decoders
#include "lvgl/src/extra/libs/freetype/lv_freetype.h"
#include "lvgl/src/extra/libs/png/lv_png.h"
#include "lvgl/src/extra/libs/bmp/lv_bmp.h"
#include "lvgl/src/extra/libs/sjpg/lv_sjpg.h"

// LVGL 8.4 compatible with SDL2 display

GUI::GUI(int width, int height)
    : screen_width(width), screen_height(height), is_running(true),
      main_screen(nullptr), image_label(nullptr), image_canvas(nullptr),
      status_label(nullptr), info_label(nullptr), canvas_buffer(nullptr),
      canvas_buffer_size(0), canvas_buffer_from_lvgl(false) {
}

GUI::~GUI() {
    // Free the canvas buffer using the correct allocator
    if (canvas_buffer != nullptr) {
        if (canvas_buffer_from_lvgl) {
            lv_mem_free(canvas_buffer);
        } else {
            free(canvas_buffer);
        }
        canvas_buffer = nullptr;
    }
}

bool GUI::initialize() {
    // Initialize LVGL
    std::cout << "Initializing LVGL..." << std::endl;
    lv_init();

    // Note: Image decoders are initialized based on lv_conf.h settings
    // PNG, JPEG, BMP support is configured in lv_conf.h with:
    // LV_USE_LODEPNG=1, LV_USE_TJPGD=1, etc.
    // No explicit initialization needed in most cases

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

// Global SDL objects
static SDL_Window* sdl_window = nullptr;
static SDL_Renderer* sdl_renderer = nullptr;
static SDL_Texture* sdl_texture = nullptr;
static lv_indev_t* sdl_indev = nullptr;

// Flush callback for display driver - Based on Chunjiin8.4
static void disp_flush_cb(lv_disp_drv_t * disp_drv, const lv_area_t * area, lv_color_t * color_p) {
    if (sdl_renderer == nullptr || sdl_texture == nullptr) {
        lv_disp_flush_ready(disp_drv);
        return;
    }

    // Lock texture for direct pixel access
    void * pixels;
    int pitch;
    SDL_LockTexture(sdl_texture, NULL, &pixels, &pitch);

    uint32_t * pixel_data = (uint32_t *)pixels;

    // Copy the LVGL framebuffer to SDL2 texture
    // LVGL uses RGB565 by default, convert to ARGB8888 for SDL2
    for (int y = area->y1; y <= area->y2; y++) {
        for (int x = area->x1; x <= area->x2; x++) {
            if (x >= 0 && x < 320 && y >= 0 && y < 640) {
                int index = y * 320 + x;
                // Get 16-bit RGB565 color from LVGL and convert to 32-bit ARGB8888
                lv_color_t lv_col = *color_p;
                uint32_t color = lv_color_to32(lv_col);
                pixel_data[index] = color;
            }
            color_p++;
        }
    }

    SDL_UnlockTexture(sdl_texture);

    // Render to screen
    SDL_RenderClear(sdl_renderer);
    SDL_RenderCopy(sdl_renderer, sdl_texture, NULL, NULL);
    SDL_RenderPresent(sdl_renderer);

    lv_disp_flush_ready(disp_drv);
}

// Input device callback - Based on Chunjiin8.4
static void indev_read_cb(lv_indev_drv_t * drv, lv_indev_data_t * data) {
    (void)drv;  // Unused parameter

    // Get current mouse state
    int x, y;
    uint32_t mouse_state = SDL_GetMouseState(&x, &y);

    data->point.x = x;
    data->point.y = y;
    data->state = (mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT)) ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}

bool GUI::init_display_driver() {
    // Initialize SDL - Based on Chunjiin8.4
    std::cout << "Initializing SDL2..." << std::endl;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create SDL window
    std::cout << "Creating SDL2 window (" << screen_width << "x" << screen_height << ")..." << std::endl;
    sdl_window = SDL_CreateWindow(
        "Face Recognition Application",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        screen_width,
        screen_height,
        SDL_WINDOW_SHOWN
    );

    if (sdl_window == nullptr) {
        std::cerr << "Failed to create SDL window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    // Create SDL renderer with V-sync
    sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (sdl_renderer == nullptr) {
        std::cerr << "Failed to create SDL renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(sdl_window);
        SDL_Quit();
        return false;
    }

    // Create SDL texture for rendering using ARGB8888 format (Chunjiin8.4 compatible)
    sdl_texture = SDL_CreateTexture(
        sdl_renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        screen_width,
        screen_height
    );

    if (sdl_texture == nullptr) {
        std::cerr << "Failed to create SDL texture: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(sdl_renderer);
        SDL_DestroyWindow(sdl_window);
        SDL_Quit();
        return false;
    }

    std::cout << "SDL2 initialized successfully" << std::endl;

    // Create display buffer with double-buffering (Chunjiin8.4 pattern)
    // Buffer size: 20,480 pixels (320 * 64) for efficient partial updates
    static lv_disp_draw_buf_t draw_buf_dsc;
    static lv_color_t buf_1[320 * 64];  // First buffer (20% of screen)
    static lv_color_t buf_2[320 * 64];  // Second buffer for double-buffering
    lv_disp_draw_buf_init(&draw_buf_dsc, buf_1, buf_2, 320 * 64);

    // Initialize display driver
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);

    // Set display resolution
    disp_drv.hor_res = screen_width;
    disp_drv.ver_res = screen_height;

    // Set the draw buffer
    disp_drv.draw_buf = &draw_buf_dsc;

    // Set flush callback (required)
    disp_drv.flush_cb = disp_flush_cb;

    // Register the display driver
    lv_disp_t* disp = lv_disp_drv_register(&disp_drv);

    if (disp == nullptr) {
        std::cerr << "Failed to register display driver" << std::endl;
        SDL_DestroyTexture(sdl_texture);
        SDL_DestroyRenderer(sdl_renderer);
        SDL_DestroyWindow(sdl_window);
        SDL_Quit();
        return false;
    }

    std::cout << "Display driver registered successfully" << std::endl;
    return true;
}

bool GUI::init_input_device() {
    // Initialize SDL2 input devices - Based on Chunjiin8.4
    std::cout << "Initializing input device driver..." << std::endl;

    // Create and register input device driver for mouse/touch
    static lv_indev_drv_t indev_drv;
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = indev_read_cb;
    sdl_indev = lv_indev_drv_register(&indev_drv);

    if (sdl_indev == nullptr) {
        std::cerr << "Failed to register input device" << std::endl;
        return false;
    }

    std::cout << "Input device driver registered successfully" << std::endl;
    return true;
}

bool GUI::create_main_screen() {
    // Get or create default screen (LVGL 8.4 compatible)
    main_screen = lv_scr_act();
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
    lv_obj_align(title, LV_ALIGN_TOP_MID, 10, 10);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_20, 0);

    // Create image display area with canvas (smaller to fit in memory)
    // Layout: Title(10) -> Canvas(40-220) -> Info(230) -> Status(260) -> Buttons(310-410)
    int img_width = screen_width - 20;   // 300px
    int img_height = 180;                 // Reduced from 300px to 180px (180*300*3 = 162KB)

    image_canvas = lv_canvas_create(main_screen);

    // Initialize canvas with a default buffer (will be replaced when image is loaded)
    // Allocate initial buffer for the canvas
    int default_buffer_size = img_width * img_height * 4;  // RGBA32
    uint8_t* default_buffer = (uint8_t*)lv_mem_alloc(default_buffer_size);
    if (default_buffer == nullptr) {
        std::cerr << "Failed to allocate default canvas buffer" << std::endl;
        return false;
    }

    // Initialize the canvas with the default buffer
    lv_canvas_set_buffer(image_canvas, default_buffer, img_width, img_height, LV_IMG_CF_RGBA8888);

    // Store this as our initial canvas buffer (will be managed by display_image)
    canvas_buffer = default_buffer;
    canvas_buffer_size = default_buffer_size;
    canvas_buffer_from_lvgl = true;  // Allocated with lv_mem_alloc

    lv_obj_set_size(image_canvas, img_width, img_height);
    lv_obj_align(image_canvas, LV_ALIGN_TOP_MID, 0, 40);  // Below title
    lv_obj_set_style_bg_color(image_canvas, lv_color_make(200, 200, 200), 0);
    lv_obj_set_style_bg_opa(image_canvas, LV_OPA_COVER, 0);
    lv_obj_set_style_border_color(image_canvas, lv_color_black(), 0);
    lv_obj_set_style_border_width(image_canvas, 2, 0);
    lv_obj_set_style_border_opa(image_canvas, LV_OPA_COVER, 0);

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
    // 3x2 grid layout centered horizontally
    // Buttons positioned at bottom of screen (avoiding label overlap)
    int button_width = 70;
    int button_height = 40;
    int padding = 8;

    // Calculate total width needed for 3 buttons
    int total_width = 3 * button_width + 2 * padding;

    // Center the buttons horizontally
    int start_x = (screen_width - total_width) / 2;
    int start_y = screen_height - 120;  // Move buttons higher on screen with better spacing

    // Load Image button (row 1, col 1)
    lv_obj_t* load_btn = lv_btn_create(main_screen);
    lv_obj_set_size(load_btn, button_width, button_height);
    lv_obj_set_pos(load_btn, start_x, start_y);
    lv_obj_t* load_label = lv_label_create(load_btn);
    lv_label_set_text(load_label, "Load");
    lv_obj_center(load_label);
    lv_obj_add_event_cb(load_btn, load_image_btn_event_cb, LV_EVENT_CLICKED, this);

    // Detect Face button (row 1, col 2)
    lv_obj_t* detect_btn = lv_btn_create(main_screen);
    lv_obj_set_size(detect_btn, button_width, button_height);
    lv_obj_set_pos(detect_btn, start_x + button_width + padding, start_y);
    lv_obj_t* detect_label = lv_label_create(detect_btn);
    lv_label_set_text(detect_label, "Detect");
    lv_obj_center(detect_label);
    lv_obj_add_event_cb(detect_btn, detect_faces_btn_event_cb, LV_EVENT_CLICKED, this);

    // Camera button (row 1, col 3)
    lv_obj_t* camera_btn = lv_btn_create(main_screen);
    lv_obj_set_size(camera_btn, button_width, button_height);
    lv_obj_set_pos(camera_btn, start_x + 2 * (button_width + padding), start_y);
    lv_obj_t* camera_label = lv_label_create(camera_btn);
    lv_label_set_text(camera_label, "Camera");
    lv_obj_center(camera_label);
    lv_obj_add_event_cb(camera_btn, camera_toggle_btn_event_cb, LV_EVENT_CLICKED, this);

    // Register button (row 2, col 1)
    lv_obj_t* register_btn = lv_btn_create(main_screen);
    lv_obj_set_size(register_btn, button_width, button_height);
    lv_obj_set_pos(register_btn, start_x, start_y + button_height + padding);
    lv_obj_t* register_label = lv_label_create(register_btn);
    lv_label_set_text(register_label, "Register");
    lv_obj_center(register_label);
    lv_obj_add_event_cb(register_btn, register_person_btn_event_cb, LV_EVENT_CLICKED, this);

    // Recognize button (row 2, col 2)
    lv_obj_t* recognize_btn = lv_btn_create(main_screen);
    lv_obj_set_size(recognize_btn, button_width, button_height);
    lv_obj_set_pos(recognize_btn, start_x + button_width + padding, start_y + button_height + padding);
    lv_obj_t* recognize_label = lv_label_create(recognize_btn);
    lv_label_set_text(recognize_label, "Recognize");
    lv_obj_center(recognize_label);
    lv_obj_add_event_cb(recognize_btn, recognize_person_btn_event_cb, LV_EVENT_CLICKED, this);

    // Capture Frame button (row 2, col 3)
    lv_obj_t* capture_btn = lv_btn_create(main_screen);
    lv_obj_set_size(capture_btn, button_width, button_height);
    lv_obj_set_pos(capture_btn, start_x + 2 * (button_width + padding), start_y + button_height + padding);
    lv_obj_t* capture_label = lv_label_create(capture_btn);
    lv_label_set_text(capture_label, "Capture");
    lv_obj_center(capture_label);
    lv_obj_add_event_cb(capture_btn, capture_frame_btn_event_cb, LV_EVENT_CLICKED, this);

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

void GUI::camera_toggle_btn_event_cb(lv_event_t* e) {
    GUI* gui = static_cast<GUI*>(lv_event_get_user_data(e));
    if (gui && gui->on_camera_toggle) {
        lv_obj_t* btn = lv_event_get_target(e);
        lv_obj_t* btn_label = lv_obj_get_child(btn, 0);

        // Check current button text to determine state
        const char* text = lv_label_get_text(btn_label);
        bool is_starting = (text != nullptr && std::string(text) == "Stop Camera");

        gui->on_camera_toggle(!is_starting);
    }
}

void GUI::capture_frame_btn_event_cb(lv_event_t* e) {
    GUI* gui = static_cast<GUI*>(lv_event_get_user_data(e));
    if (gui && gui->on_capture_frame) {
        gui->on_capture_frame();
    }
}

bool GUI::display_image(const cv::Mat& image, bool is_rgb, bool auto_resize) {
    if (image.empty()) {
        std::cerr << "Empty image provided" << std::endl;
        return false;
    }

    // Store current image
    current_display_image = image.clone();

    // Store original dimensions for label display
    int original_width = image.cols;
    int original_height = image.rows;

    // Resize image to fit canvas
    cv::Mat display_image = image.clone();
    int canvas_width = screen_width - 20;   // 300px
    int canvas_height = 180;                 // Fixed canvas height

    // Resize if image is larger than canvas and auto_resize is enabled
    if (auto_resize && (display_image.cols > canvas_width || display_image.rows > canvas_height)) {
        float scale = std::min(static_cast<float>(canvas_width) / display_image.cols,
                              static_cast<float>(canvas_height) / display_image.rows);
        int new_width = static_cast<int>(display_image.cols * scale);
        int new_height = static_cast<int>(display_image.rows * scale);

        // Ensure minimum size
        if (new_width < 10) new_width = 10;
        if (new_height < 10) new_height = 10;

        cv::resize(display_image, display_image, cv::Size(new_width, new_height), 0, 0, cv::INTER_LINEAR);
    }

    // Convert image to BGR if needed
    if (display_image.channels() == 1) {
        // Grayscale - convert to BGR
        cv::cvtColor(display_image, display_image, cv::COLOR_GRAY2BGR);
    } else if (display_image.channels() == 3) {
        if (is_rgb) {
            // RGB from image_loader, convert to BGR
            cv::cvtColor(display_image, display_image, cv::COLOR_RGB2BGR);
        }
        // else: already BGR, no conversion needed
    } else if (display_image.channels() == 4) {
        // Remove alpha and convert RGBA to BGR
        cv::cvtColor(display_image, display_image, cv::COLOR_RGBA2BGR);
    }

    int width = display_image.cols;
    int height = display_image.rows;

    // Free the old canvas buffer if it exists, using the correct allocator
    if (canvas_buffer != nullptr) {
        if (canvas_buffer_from_lvgl) {
            lv_mem_free(canvas_buffer);
        } else {
            free(canvas_buffer);
        }
        canvas_buffer = nullptr;
        canvas_buffer_size = 0;
    }

    // Convert BGR image to LVGL RGBA8888 format buffer (consistent with initial canvas)
    int buffer_size_rgba = width * height * 4;  // RGBA = 4 bytes per pixel

    // Allocate using standard malloc (not LVGL's limited pool)
    // to avoid exhausting LVGL's 256KB memory limit
    uint8_t* rgba_buffer = (uint8_t*)malloc(buffer_size_rgba);

    if (rgba_buffer == nullptr) {
        std::cerr << "Failed to allocate RGBA buffer for canvas (" << buffer_size_rgba << " bytes)" << std::endl;
        return false;
    }

    // Convert each pixel from BGR to LVGL's ARGB8888 format
    // LVGL 32-bit color format is: [Blue][Green][Red][Alpha] in memory
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // OpenCV stores data as BGR
            cv::Vec3b pixel = display_image.at<cv::Vec3b>(y, x);
            uint8_t blue = pixel[0];
            uint8_t green = pixel[1];
            uint8_t red = pixel[2];

            int idx = (y * width + x) * 4;
            rgba_buffer[idx + 0] = blue;    // B
            rgba_buffer[idx + 1] = green;   // G
            rgba_buffer[idx + 2] = red;     // R
            rgba_buffer[idx + 3] = 0xFF;    // A (fully opaque)
        }
    }

    // Set the canvas buffer with RGBA8888 data
    if (image_canvas) {
        // Store the new buffer for later cleanup
        canvas_buffer = rgba_buffer;
        canvas_buffer_size = buffer_size_rgba;
        canvas_buffer_from_lvgl = false;  // Allocated with malloc

        std::cout << "Setting canvas buffer: " << width << "x" << height << " RGBA8888" << std::endl;
        lv_canvas_set_buffer(image_canvas, rgba_buffer, width, height, LV_IMG_CF_TRUE_COLOR_ALPHA);

        // Resize canvas to match image dimensions (or max size)
        int max_width = screen_width - 20;
        int max_height = 180;
        int display_width = (width > max_width) ? max_width : width;
        int display_height = (height > max_height) ? max_height : height;

        std::cout << "Resizing canvas to: " << display_width << "x" << display_height << std::endl;
        lv_obj_set_size(image_canvas, display_width, display_height);
        lv_obj_align(image_canvas, LV_ALIGN_TOP_MID, 0, 40);

        // Ensure canvas is fully opaque and visible
        lv_obj_set_style_opa(image_canvas, LV_OPA_COVER, 0);
        
        // Make sure canvas is visible and in front
        lv_obj_clear_flag(image_canvas, LV_OBJ_FLAG_HIDDEN);
        lv_obj_move_foreground(image_canvas);

        // Invalidate the canvas to force LVGL to re-render it
        lv_obj_invalidate(image_canvas);

        // Also invalidate the main screen to ensure proper redraw
        if (main_screen) {
            lv_obj_invalidate(main_screen);
        }

        // Force an immediate render by calling timer handler
        lv_timer_handler();
        std::cout << "Canvas updated and invalidated" << std::endl;

        // Update image info label with original image size
        std::string img_info = "Image: " + std::to_string(original_width) + "x" + std::to_string(original_height);
        if (width != original_width || height != original_height) {
            img_info += " (display: " + std::to_string(width) + "x" + std::to_string(height) + ")";
        }
        if (image_label) {
            lv_label_set_text(image_label, img_info.c_str());
        }

        // Update status
        update_status("Image displayed: " + std::to_string(original_width) + "x" + std::to_string(original_height));
    } else {
        free(rgba_buffer);
        return false;
    }

    return true;
}

bool GUI::display_detection_result(const cv::Mat& image, const std::vector<Face>& faces) {
    if (image.empty()) {
        return false;
    }

    std::string info = "Detected " + std::to_string(faces.size()) + " faces";
    lv_label_set_text(info_label, info.c_str());

    // Image from draw_faces is already in BGR format and pre-resized, so is_rgb=false, auto_resize=false
    return display_image(image, false, false);
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

void GUI::set_camera_toggle_callback(std::function<void(bool)> callback) {
    on_camera_toggle = callback;
}

void GUI::set_capture_frame_callback(std::function<void()> callback) {
    on_capture_frame = callback;
}

void GUI::run() {
    // Track timing for LVGL tick updates (Chunjiin8.4 pattern)
    uint32_t last_time = SDL_GetTicks();
    SDL_Event event;

    while (is_running) {
        // Handle SDL events
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    std::cout << "Window close button clicked" << std::endl;
                    is_running = false;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        is_running = false;
                    }
                    break;
            }
        }

        if (!is_running) break;

        // Update LVGL timing - Based on Chunjiin8.4
        uint32_t current_time = SDL_GetTicks();
        uint32_t elapsed = current_time - last_time;
        if (elapsed > 0) {
            lv_tick_inc(elapsed);
            last_time = current_time;
        }

        // Handle LVGL tasks
        lv_timer_handler();

        // Small delay to reduce CPU usage (Chunjiin8.4 pattern)
        SDL_Delay(5);
    }

    // Cleanup SDL resources
    if (sdl_texture != nullptr) {
        SDL_DestroyTexture(sdl_texture);
        sdl_texture = nullptr;
    }
    if (sdl_renderer != nullptr) {
        SDL_DestroyRenderer(sdl_renderer);
        sdl_renderer = nullptr;
    }
    if (sdl_window != nullptr) {
        SDL_DestroyWindow(sdl_window);
        sdl_window = nullptr;
    }
    SDL_Quit();
}

void GUI::stop() {
    is_running = false;
}

void GUI::update_status(const std::string& message) {
    if (status_label) {
        lv_label_set_text(status_label, message.c_str());
    }
}
