#include "camera.h"
#include <opencv4/opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <sys/mman.h>

// C++ implementation
extern "C" {

// Common resolution presets to test
static const struct {
    int width;
    int height;
    const char* name;
} resolution_presets[] = {
    {640, 480, "VGA"},
    {800, 600, "SVGA"},
    {1024, 768, "XGA"},
    {1280, 720, "HD"},
    {1280, 1024, "SXGA"},
    {1366, 768, "HD+"},
    {1440, 900, "WXGA+"},
    {1600, 900, "HD+"},
    {1680, 1050, "WSXGA+"},
    {1920, 1080, "Full HD"},
    {1920, 1200, "WUXGA"},
    {2560, 1440, "QHD"},
    {3840, 2160, "4K UHD"},
    {4096, 2160, "4K DCI"},
    {7680, 4320, "8K UHD"}
};

static const int num_presets = sizeof(resolution_presets) / sizeof(resolution_presets[0]);

// Common video formats to test (only widely supported formats)
static const struct {
    int fourcc;
    const char* name;
    const char* description;
} video_formats[] = {
    {V4L2_PIX_FMT_YUYV, "YUYV", "YUYV 4:2:2"},
    {V4L2_PIX_FMT_YVYU, "YVYU", "YVYU 4:2:2"},
    {V4L2_PIX_FMT_UYVY, "UYVY", "UYVY 4:2:2"},
    {V4L2_PIX_FMT_VYUY, "VYUY", "VYUY 4:2:2"},
    {V4L2_PIX_FMT_RGB24, "RGB24", "RGB 24-bit"},
    {V4L2_PIX_FMT_BGR24, "BGR24", "BGR 24-bit"},
    {V4L2_PIX_FMT_RGB32, "RGB32", "RGB 32-bit"},
    {V4L2_PIX_FMT_BGR32, "BGR32", "BGR 32-bit"},
    {V4L2_PIX_FMT_GREY, "GREY", "8-bit Greyscale"},
    {V4L2_PIX_FMT_Y16, "Y16", "16-bit Greyscale"},
    {V4L2_PIX_FMT_MJPEG, "MJPEG", "Motion JPEG"},
    {V4L2_PIX_FMT_JPEG, "JPEG", "JPEG"},
    {V4L2_PIX_FMT_H264, "H264", "H.264"}
};

static const int num_formats = sizeof(video_formats) / sizeof(video_formats[0]);

// Helper function to get format name from fourcc
const char* get_format_name(int fourcc) {
    for (int i = 0; i < num_formats; i++) {
        if (video_formats[i].fourcc == fourcc) {
            return video_formats[i].name;
        }
    }
    return "UNKNOWN";
}

// Helper function to get format description from fourcc
const char* get_format_description(int fourcc) {
    for (int i = 0; i < num_formats; i++) {
        if (video_formats[i].fourcc == fourcc) {
            return video_formats[i].description;
        }
    }
    return "Unknown format";
}

// Forward declaration
int discover_camera_capabilities_opencv(camera_test_t* camera);

int init_camera_test(camera_test_t* camera, int camera_index) {
    if (!camera) {
        return -1;
    }
    
    // Initialize camera structure
    memset(camera, 0, sizeof(camera_test_t));
    camera->camera_index = camera_index;
    camera->is_running = false;
    
    // Initialize resolution array
    camera->max_resolutions = num_presets;
    camera->resolutions = (resolution_info_t*)malloc(num_presets * sizeof(resolution_info_t));
    if (!camera->resolutions) {
        printf("Error: Could not allocate memory for resolutions\n");
        return -1;
    }
    
    // Initialize capability array
    camera->max_capabilities = 100; // Reasonable limit
    camera->capabilities = (camera_capability_t*)malloc(camera->max_capabilities * sizeof(camera_capability_t));
    if (!camera->capabilities) {
        printf("Error: Could not allocate memory for capabilities\n");
        free(camera->resolutions);
        return -1;
    }
    
    // Initialize resolution array
    for (int i = 0; i < num_presets; i++) {
        camera->resolutions[i].width = resolution_presets[i].width;
        camera->resolutions[i].height = resolution_presets[i].height;
        camera->resolutions[i].fps = 0;
        camera->resolutions[i].supported = false;
        camera->resolutions[i].score = 0.0;
    }
    
    // Create VideoCapture object
    camera->capture = (cv_VideoCapture*)new cv::VideoCapture(camera_index);
    
    cv::VideoCapture* cap = (cv::VideoCapture*)camera->capture;
    if (!cap->isOpened()) {
        printf("Error: Could not open camera %d\n", camera_index);
        delete cap;
        camera->capture = NULL;
        free(camera->resolutions);
        free(camera->capabilities);
        camera->resolutions = NULL;
        camera->capabilities = NULL;
        return -1;
    }
    
    // Get camera properties
    camera->width = (int)cap->get(cv::CAP_PROP_FRAME_WIDTH);
    camera->height = (int)cap->get(cv::CAP_PROP_FRAME_HEIGHT);
    camera->fps = (int)cap->get(cv::CAP_PROP_FPS);
    
    printf("Camera %d initialized successfully\n", camera_index);
    printf("Default Resolution: %dx%d\n", camera->width, camera->height);
    printf("Default FPS: %d\n", camera->fps);
    
    return 0;
}

void cleanup_camera_test(camera_test_t* camera) {
    if (camera) {
        if (camera->is_running) {
            stop_camera_test(camera);
        }
        
        if (camera->capture) {
            cv::VideoCapture* cap = (cv::VideoCapture*)camera->capture;
            cap->release();
            delete cap;
            camera->capture = NULL;
        }
        
        if (camera->resolutions) {
            free(camera->resolutions);
            camera->resolutions = NULL;
        }
        
        if (camera->capabilities) {
            free(camera->capabilities);
            camera->capabilities = NULL;
        }
    }
}

bool start_camera_test(camera_test_t* camera) {
    if (!camera || !camera->capture) {
        return false;
    }
    
    if (camera->is_running) {
        printf("Camera is already running\n");
        return true;
    }
    
    camera->is_running = true;
    printf("Camera test started\n");
    return true;
}

void stop_camera_test(camera_test_t* camera) {
    if (camera) {
        camera->is_running = false;
        printf("Camera test stopped\n");
    }
}

cv_Mat* capture_frame(camera_test_t* camera) {
    static cv::Mat frame;
    
    if (!camera || !camera->capture || !camera->is_running) {
        return (cv_Mat*)&frame;
    }
    
    cv::VideoCapture* cap = (cv::VideoCapture*)camera->capture;
    *cap >> frame;
    return (cv_Mat*)&frame;
}

bool save_test_image(camera_test_t* camera, const char* filename) {
    if (!camera || !camera->capture || !filename) {
        return false;
    }
    
    cv::Mat frame;
    cv::VideoCapture* cap = (cv::VideoCapture*)camera->capture;
    *cap >> frame;
    
    if (frame.empty()) {
        printf("Error: Could not capture frame for saving\n");
        return false;
    }
    
    bool success = cv::imwrite(filename, frame);
    if (success) {
        printf("Test image saved: %s\n", filename);
    } else {
        printf("Error: Could not save image to %s\n", filename);
    }
    
    return success;
}

// Test capability function (forward declaration)
bool test_capability(camera_test_t* camera, int width, int height, int fps, int fourcc);

// Query camera capabilities using V4L2
int discover_camera_capabilities(camera_test_t* camera) {
    if (!camera) {
        return -1;
    }
    
    char device_path[64];
    snprintf(device_path, sizeof(device_path), "/dev/video%d", camera->camera_index);
    
    int fd = open(device_path, O_RDWR);
    if (fd < 0) {
        printf("Warning: Could not open %s for capability query (using OpenCV fallback)\n", device_path);
        return discover_camera_capabilities_opencv(camera);
    }
    
    printf("Discovering camera capabilities for camera %d...\n", camera->camera_index);
    
    int capability_count = 0;
    
    // Query supported formats
    struct v4l2_fmtdesc fmtdesc;
    memset(&fmtdesc, 0, sizeof(fmtdesc));
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    
    while (ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc) == 0) {
        if (capability_count >= camera->max_capabilities) {
            printf("Warning: Reached maximum capability count\n");
            break;
        }
        
        // Query supported frame sizes for this format
        struct v4l2_frmsizeenum frmsize;
        memset(&frmsize, 0, sizeof(frmsize));
        frmsize.pixel_format = fmtdesc.pixelformat;
        
        while (ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frmsize) == 0) {
            if (frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
                // Query supported frame intervals for this size
                struct v4l2_frmivalenum frmival;
                memset(&frmival, 0, sizeof(frmival));
                frmival.pixel_format = fmtdesc.pixelformat;
                frmival.width = frmsize.discrete.width;
                frmival.height = frmsize.discrete.height;
                
                while (ioctl(fd, VIDIOC_ENUM_FRAMEINTERVALS, &frmival) == 0) {
                    if (frmival.type == V4L2_FRMIVAL_TYPE_DISCRETE) {
                        camera->capabilities[capability_count].width = frmsize.discrete.width;
                        camera->capabilities[capability_count].height = frmsize.discrete.height;
                        camera->capabilities[capability_count].fps = frmival.discrete.denominator / frmival.discrete.numerator;
                        camera->capabilities[capability_count].fourcc = fmtdesc.pixelformat;
                        camera->capabilities[capability_count].format_name = get_format_name(fmtdesc.pixelformat);
                        camera->capabilities[capability_count].supported = true;
                        
                        capability_count++;
                        
                        if (capability_count >= camera->max_capabilities) {
                            printf("Warning: Reached maximum capability count\n");
                            goto done;
                        }
                    }
                    frmival.index++;
                }
            }
            frmsize.index++;
        }
        fmtdesc.index++;
    }
    
done:
    close(fd);
    camera->capability_count = capability_count;
    printf("Found %d camera capabilities\n", capability_count);
    
    return capability_count;
}

// Fallback method using OpenCV to discover capabilities
int discover_camera_capabilities_opencv(camera_test_t* camera) {
    if (!camera || !camera->capture) {
        return -1;
    }
    
    printf("Using OpenCV fallback to discover camera capabilities...\n");
    
    int capability_count = 0;
    
    // Test common formats and resolutions
    int test_resolutions[] = {640, 800, 1024, 1280, 1920, 2560, 3840};
    int test_fps[] = {5, 10, 15, 25, 30};
    int test_formats[] = {V4L2_PIX_FMT_YUYV, V4L2_PIX_FMT_MJPEG, V4L2_PIX_FMT_RGB24};
    
    for (int format_idx = 0; format_idx < 3; format_idx++) {
        int fourcc = test_formats[format_idx];
        
        for (int res_idx = 0; res_idx < 7; res_idx++) {
            int width = test_resolutions[res_idx];
            int height = (width * 3) / 4; // 4:3 aspect ratio
            
            for (int fps_idx = 0; fps_idx < 5; fps_idx++) {
                int fps = test_fps[fps_idx];
                
                if (capability_count >= camera->max_capabilities) {
                    printf("Warning: Reached maximum capability count\n");
                    goto done_opencv;
                }
                
                // Test this combination
                if (test_capability(camera, width, height, fps, fourcc)) {
                    camera->capabilities[capability_count].width = width;
                    camera->capabilities[capability_count].height = height;
                    camera->capabilities[capability_count].fps = fps;
                    camera->capabilities[capability_count].fourcc = fourcc;
                    camera->capabilities[capability_count].format_name = get_format_name(fourcc);
                    camera->capabilities[capability_count].supported = true;
                    
                    capability_count++;
                }
            }
        }
    }
    
done_opencv:
    camera->capability_count = capability_count;
    printf("Found %d camera capabilities (OpenCV method)\n", capability_count);
    
    return capability_count;
}

bool test_capability(camera_test_t* camera, int width, int height, int fps, int fourcc) {
    if (!camera || !camera->capture) {
        return false;
    }
    
    cv::VideoCapture* cap = (cv::VideoCapture*)camera->capture;
    
    // Set format and resolution
    cap->set(cv::CAP_PROP_FOURCC, fourcc);
    cap->set(cv::CAP_PROP_FRAME_WIDTH, width);
    cap->set(cv::CAP_PROP_FRAME_HEIGHT, height);
    cap->set(cv::CAP_PROP_FPS, fps);
    
    // Wait a bit for the camera to adjust
    usleep(100000); // 100ms
    
    // Check if the settings were actually applied
    int actual_width = (int)cap->get(cv::CAP_PROP_FRAME_WIDTH);
    int actual_height = (int)cap->get(cv::CAP_PROP_FRAME_HEIGHT);
    int actual_fps = (int)cap->get(cv::CAP_PROP_FPS);
    int actual_fourcc = (int)cap->get(cv::CAP_PROP_FOURCC);
    
    // Test if we can capture a frame with these settings
    cv::Mat test_frame;
    *cap >> test_frame;
    
    bool supported = !test_frame.empty() && 
                    abs(actual_width - width) <= 10 && 
                    abs(actual_height - height) <= 10 &&
                    actual_fourcc == fourcc;
    
    if (supported) {
        printf("  ✓ %dx%d @ %d fps (%s) - Supported\n", 
               actual_width, actual_height, actual_fps, get_format_name(fourcc));
    } else {
        printf("  ✗ %dx%d @ %d fps (%s) - Not supported\n", 
               width, height, fps, get_format_name(fourcc));
    }
    
    return supported;
}

void print_camera_capabilities(camera_test_t* camera) {
    if (!camera || !camera->capabilities) {
        printf("No capability data available\n");
        return;
    }
    
    printf("\n=== Camera Capabilities ===\n");
    printf("%-12s %-12s %-8s %-12s %-20s\n", "Resolution", "Format", "FPS", "FourCC", "Description");
    printf("------------------------------------------------------------\n");
    
    for (int i = 0; i < camera->capability_count; i++) {
        if (camera->capabilities[i].supported) {
            printf("%dx%-8d %-12s %-8d %-12s %-20s\n", 
                   camera->capabilities[i].width,
                   camera->capabilities[i].height,
                   camera->capabilities[i].format_name,
                   camera->capabilities[i].fps,
                   get_format_name(camera->capabilities[i].fourcc),
                   get_format_description(camera->capabilities[i].fourcc));
        }
    }
    printf("====================================\n\n");
}

test_result_t test_camera_capabilities(int camera_index) {
    test_result_t result = {false, "", 0.0};
    
    camera_test_t camera;
    if (init_camera_test(&camera, camera_index) != 0) {
        snprintf(result.message, sizeof(result.message), 
                "Failed to initialize camera %d", camera_index);
        return result;
    }
    
    int capability_count = discover_camera_capabilities(&camera);
    
    if (capability_count > 0) {
        result.success = true;
        snprintf(result.message, sizeof(result.message), 
                "Camera %d supports %d capabilities", camera_index, capability_count);
        
        // Calculate score based on capabilities
        double total_score = 0.0;
        int scored_count = 0;
        
        for (int i = 0; i < capability_count; i++) {
            if (camera.capabilities[i].supported) {
                // Score based on resolution and format
                int total_pixels = camera.capabilities[i].width * camera.capabilities[i].height;
                double resolution_score = 0.0;
                
                if (total_pixels >= 1920 * 1080) {
                    resolution_score = 100.0;
                } else if (total_pixels >= 1280 * 720) {
                    resolution_score = 80.0;
                } else if (total_pixels >= 640 * 480) {
                    resolution_score = 60.0;
                } else {
                    resolution_score = 40.0;
                }
                
                // Bonus for high-quality formats
                double format_bonus = 0.0;
                if (camera.capabilities[i].fourcc == V4L2_PIX_FMT_H264) {
                    format_bonus = 10.0;
                } else if (camera.capabilities[i].fourcc == V4L2_PIX_FMT_MJPEG) {
                    format_bonus = 5.0;
                }
                
                total_score += resolution_score + format_bonus;
                scored_count++;
            }
        }
        
        if (scored_count > 0) {
            result.performance_score = total_score / scored_count;
        }
        
        print_camera_capabilities(&camera);
    } else {
        snprintf(result.message, sizeof(result.message), 
                "Camera %d supports no standard capabilities", camera_index);
        result.performance_score = 0.0;
    }
    
    cleanup_camera_test(&camera);
    return result;
}

bool test_resolution(camera_test_t* camera, int width, int height, int fps) {
    if (!camera || !camera->capture) {
        return false;
    }
    
    cv::VideoCapture* cap = (cv::VideoCapture*)camera->capture;
    
    // Set resolution and FPS
    cap->set(cv::CAP_PROP_FRAME_WIDTH, width);
    cap->set(cv::CAP_PROP_FRAME_HEIGHT, height);
    cap->set(cv::CAP_PROP_FPS, fps);
    
    // Wait a bit for the camera to adjust
    usleep(100000); // 100ms
    
    // Check if the resolution was actually set
    int actual_width = (int)cap->get(cv::CAP_PROP_FRAME_WIDTH);
    int actual_height = (int)cap->get(cv::CAP_PROP_FRAME_HEIGHT);
    int actual_fps = (int)cap->get(cv::CAP_PROP_FPS);
    
    // Test if we can capture a frame at this resolution
    cv::Mat test_frame;
    *cap >> test_frame;
    
    bool supported = !test_frame.empty() && 
                    abs(actual_width - width) <= 10 && 
                    abs(actual_height - height) <= 10;
    
    if (supported) {
        printf("  ✓ %dx%d @ %d fps - Supported\n", actual_width, actual_height, actual_fps);
    } else {
        printf("  ✗ %dx%d @ %d fps - Not supported\n", width, height, fps);
    }
    
    return supported;
}

int discover_camera_resolutions(camera_test_t* camera) {
    if (!camera || !camera->capture) {
        return -1;
    }
    
    printf("Discovering supported resolutions for camera %d...\n", camera->camera_index);
    
    int supported_count = 0;
    int fps_options[] = {30, 25, 15, 10, 5};
    int num_fps_options = sizeof(fps_options) / sizeof(fps_options[0]);
    
    for (int i = 0; i < num_presets; i++) {
        bool resolution_supported = false;
        int best_fps = 0;
        
        // Try different FPS values for this resolution
        for (int fps_idx = 0; fps_idx < num_fps_options; fps_idx++) {
            int fps = fps_options[fps_idx];
            
            if (test_resolution(camera, resolution_presets[i].width, 
                              resolution_presets[i].height, fps)) {
                resolution_supported = true;
                best_fps = fps;
                break;
            }
        }
        
        // Store the result
        camera->resolutions[i].supported = resolution_supported;
        camera->resolutions[i].fps = best_fps;
        
        if (resolution_supported) {
            supported_count++;
            
            // Calculate score based on resolution
            int total_pixels = resolution_presets[i].width * resolution_presets[i].height;
            if (total_pixels >= 1920 * 1080) {
                camera->resolutions[i].score = 100.0;
            } else if (total_pixels >= 1280 * 720) {
                camera->resolutions[i].score = 80.0;
            } else if (total_pixels >= 640 * 480) {
                camera->resolutions[i].score = 60.0;
            } else {
                camera->resolutions[i].score = 40.0;
            }
        }
    }
    
    camera->resolution_count = supported_count;
    printf("Found %d supported resolutions\n", supported_count);
    
    return supported_count;
}

void print_supported_resolutions(camera_test_t* camera) {
    if (!camera || !camera->resolutions) {
        printf("No resolution data available\n");
        return;
    }
    
    printf("\n=== Supported Resolutions ===\n");
    printf("%-12s %-12s %-8s %-8s\n", "Resolution", "Name", "FPS", "Score");
    printf("------------------------------------------------\n");
    
    for (int i = 0; i < num_presets; i++) {
        if (camera->resolutions[i].supported) {
            printf("%dx%-8d %-12s %-8d %-8.1f\n", 
                   camera->resolutions[i].width,
                   camera->resolutions[i].height,
                   resolution_presets[i].name,
                   camera->resolutions[i].fps,
                   camera->resolutions[i].score);
        }
    }
    printf("============================\n\n");
}

void run_camera_test_console(camera_test_t* camera) {
    if (!camera) {
        printf("Error: Invalid camera object\n");
        return;
    }
    
    printf("=== Camera Test Console ===\n");
    printf("Commands:\n");
    printf("  start  - Start camera test\n");
    printf("  stop   - Stop camera test\n");
    printf("  capture <filename> - Capture and save image\n");
    printf("  info   - Show camera information\n");
    printf("  resolutions - Discover all supported resolutions\n");
    printf("  capabilities - Discover all camera capabilities\n");
    printf("  quit   - Exit test\n");
    printf("========================\n");
    
    char command[256];
    char filename[256];
    
    while (1) {
        printf("camera> ");
        if (fgets(command, sizeof(command), stdin) == NULL) {
            break;
        }
        
        // Remove newline
        command[strcspn(command, "\n")] = 0;
        
        if (strcmp(command, "quit") == 0 || strcmp(command, "exit") == 0) {
            break;
        } else if (strcmp(command, "start") == 0) {
            if (start_camera_test(camera)) {
                printf("Camera test started successfully\n");
            } else {
                printf("Failed to start camera test\n");
            }
        } else if (strcmp(command, "stop") == 0) {
            stop_camera_test(camera);
        } else if (strncmp(command, "capture ", 8) == 0) {
            if (!camera->is_running) {
                printf("Error: Camera is not running. Use 'start' first.\n");
                continue;
            }
            
            strcpy(filename, command + 8);
            if (strlen(filename) == 0) {
                strcpy(filename, "test_capture.jpg");
            }
            
            if (save_test_image(camera, filename)) {
                printf("Image captured and saved successfully\n");
            }
        } else if (strcmp(command, "info") == 0) {
            printf("Camera Information:\n");
            printf("  Index: %d\n", camera->camera_index);
            printf("  Default Resolution: %dx%d\n", camera->width, camera->height);
            printf("  Default FPS: %d\n", camera->fps);
            printf("  Status: %s\n", camera->is_running ? "Running" : "Stopped");
            printf("  Supported Resolutions: %d\n", camera->resolution_count);
            printf("  Supported Capabilities: %d\n", camera->capability_count);
        } else if (strcmp(command, "resolutions") == 0) {
            discover_camera_resolutions(camera);
            print_supported_resolutions(camera);
        } else if (strcmp(command, "capabilities") == 0) {
            discover_camera_capabilities(camera);
            print_camera_capabilities(camera);
        } else if (strlen(command) > 0) {
            printf("Unknown command: %s\n", command);
        }
    }
    
    printf("Exiting camera test console\n");
}

test_result_t test_all_resolutions(int camera_index) {
    test_result_t result = {false, "", 0.0};
    
    camera_test_t camera;
    if (init_camera_test(&camera, camera_index) != 0) {
        snprintf(result.message, sizeof(result.message), 
                "Failed to initialize camera %d", camera_index);
        return result;
    }
    
    int supported_count = discover_camera_resolutions(&camera);
    
    if (supported_count > 0) {
        result.success = true;
        snprintf(result.message, sizeof(result.message), 
                "Camera %d supports %d resolutions", camera_index, supported_count);
        
        // Calculate average score
        double total_score = 0.0;
        int scored_count = 0;
        
        for (int i = 0; i < num_presets; i++) {
            if (camera.resolutions[i].supported) {
                total_score += camera.resolutions[i].score;
                scored_count++;
            }
        }
        
        if (scored_count > 0) {
            result.performance_score = total_score / scored_count;
        }
        
        print_supported_resolutions(&camera);
    } else {
        snprintf(result.message, sizeof(result.message), 
                "Camera %d supports no standard resolutions", camera_index);
        result.performance_score = 0.0;
    }
    
    cleanup_camera_test(&camera);
    return result;
}

test_result_t test_camera_initialization(int camera_index) {
    test_result_t result = {false, "", 0.0};
    
    camera_test_t camera;
    int ret = init_camera_test(&camera, camera_index);
    
    if (ret == 0) {
        result.success = true;
        snprintf(result.message, sizeof(result.message), 
                "Camera %d initialized successfully", camera_index);
        result.performance_score = 100.0;
        
        cleanup_camera_test(&camera);
    } else {
        snprintf(result.message, sizeof(result.message), 
                "Failed to initialize camera %d", camera_index);
        result.performance_score = 0.0;
    }
    
    return result;
}

test_result_t test_camera_capture(int camera_index) {
    test_result_t result = {false, "", 0.0};
    
    camera_test_t camera;
    if (init_camera_test(&camera, camera_index) != 0) {
        snprintf(result.message, sizeof(result.message), 
                "Camera %d initialization failed", camera_index);
        return result;
    }
    
    if (!start_camera_test(&camera)) {
        snprintf(result.message, sizeof(result.message), 
                "Failed to start camera %d", camera_index);
        cleanup_camera_test(&camera);
        return result;
    }
    
    // Test frame capture
    cv_Mat* frame_ptr = capture_frame(&camera);
    cv::Mat& frame = *(cv::Mat*)frame_ptr;
    
    if (!frame.empty()) {
        result.success = true;
        snprintf(result.message, sizeof(result.message), 
                "Camera %d capture test successful", camera_index);
        result.performance_score = 100.0;
    } else {
        snprintf(result.message, sizeof(result.message), 
                "Camera %d capture test failed", camera_index);
        result.performance_score = 0.0;
    }
    
    cleanup_camera_test(&camera);
    return result;
}

test_result_t test_camera_resolution(int camera_index) {
    test_result_t result = {false, "", 0.0};
    
    camera_test_t camera;
    if (init_camera_test(&camera, camera_index) != 0) {
        snprintf(result.message, sizeof(result.message), 
                "Camera %d initialization failed", camera_index);
        return result;
    }
    
    // Check if resolution is reasonable
    if (camera.width > 0 && camera.height > 0) {
        result.success = true;
        snprintf(result.message, sizeof(result.message), 
                "Camera %d resolution: %dx%d", camera_index, camera.width, camera.height);
        
        // Score based on resolution (higher resolution = higher score)
        int total_pixels = camera.width * camera.height;
        if (total_pixels >= 1920 * 1080) {
            result.performance_score = 100.0;
        } else if (total_pixels >= 1280 * 720) {
            result.performance_score = 80.0;
        } else if (total_pixels >= 640 * 480) {
            result.performance_score = 60.0;
        } else {
            result.performance_score = 40.0;
        }
    } else {
        snprintf(result.message, sizeof(result.message), 
                "Camera %d has invalid resolution", camera_index);
        result.performance_score = 0.0;
    }
    
    cleanup_camera_test(&camera);
    return result;
}

test_result_t test_camera_fps(int camera_index) {
    test_result_t result = {false, "", 0.0};
    
    camera_test_t camera;
    if (init_camera_test(&camera, camera_index) != 0) {
        snprintf(result.message, sizeof(result.message), 
                "Camera %d initialization failed", camera_index);
        return result;
    }
    
    if (camera.fps > 0) {
        result.success = true;
        snprintf(result.message, sizeof(result.message), 
                "Camera %d FPS: %d", camera_index, camera.fps);
        
        // Score based on FPS
        if (camera.fps >= 30) {
            result.performance_score = 100.0;
        } else if (camera.fps >= 25) {
            result.performance_score = 85.0;
        } else if (camera.fps >= 15) {
            result.performance_score = 70.0;
        } else {
            result.performance_score = 50.0;
        }
    } else {
        snprintf(result.message, sizeof(result.message), 
                "Camera %d has invalid FPS", camera_index);
        result.performance_score = 0.0;
    }
    
    cleanup_camera_test(&camera);
    return result;
}

// Test only supported capabilities by first discovering them, then testing only those
test_result_t test_supported_camera_capabilities(int camera_index) {
    test_result_t result = {false, "", 0.0};
    
    printf("Testing supported camera capabilities for camera %d...\n", camera_index);
    
    // Initialize camera
    camera_test_t camera;
    if (init_camera_test(&camera, camera_index) != 0) {
        snprintf(result.message, sizeof(result.message), 
                "Failed to initialize camera %d", camera_index);
        return result;
    }
    
    // Test default camera settings first
    printf("\n=== Testing Default Camera Settings ===\n");
    printf("Default Resolution: %dx%d\n", camera.width, camera.height);
    printf("Default FPS: %d\n", camera.fps);
    
    // Test if we can capture with default settings
    if (start_camera_test(&camera)) {
        cv_Mat* frame_ptr = capture_frame(&camera);
        cv::Mat& frame = *(cv::Mat*)frame_ptr;
        
        if (!frame.empty()) {
            printf("✓ Default settings - Test PASSED\n");
            result.success = true;
            snprintf(result.message, sizeof(result.message), 
                    "Camera %d works with default settings (%dx%d @ %d fps)", 
                    camera_index, camera.width, camera.height, camera.fps);
            
            // Score based on default resolution
            int total_pixels = camera.width * camera.height;
            if (total_pixels >= 1920 * 1080) {
                result.performance_score = 100.0;
            } else if (total_pixels >= 1280 * 720) {
                result.performance_score = 80.0;
            } else if (total_pixels >= 640 * 480) {
                result.performance_score = 60.0;
            } else {
                result.performance_score = 40.0;
            }
        } else {
            printf("✗ Default settings - Test FAILED (no frame captured)\n");
            snprintf(result.message, sizeof(result.message), 
                    "Camera %d failed to capture with default settings", camera_index);
            result.performance_score = 0.0;
        }
        stop_camera_test(&camera);
    } else {
        printf("✗ Default settings - Test FAILED (could not start camera)\n");
        snprintf(result.message, sizeof(result.message), 
                "Camera %d failed to start with default settings", camera_index);
        result.performance_score = 0.0;
    }
    
    // Discover camera capabilities using V4L2 API directly
    printf("\n=== Discovering Camera Capabilities ===\n");
    
    char device_path[64];
    snprintf(device_path, sizeof(device_path), "/dev/video%d", camera_index);
    
    int successful_capabilities = 0;
    double total_capability_score = 0.0;
    int tested_capabilities = 0;
    
    int fd = open(device_path, O_RDWR);
    if (fd == -1) {
        printf("Failed to open video device: %s\n", device_path);
        printf("\n=== Test Summary ===\n");
        printf("Default settings: %s\n", result.success ? "PASS" : "FAIL");
        printf("Could not discover camera capabilities\n");
        printf("Final score: %.1f/100\n", result.performance_score);
    } else {
        // Query device capabilities
        v4l2_capability cap;
        if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1) {
            printf("Error querying device capabilities.\n");
            close(fd);
            printf("\n=== Test Summary ===\n");
            printf("Default settings: %s\n", result.success ? "PASS" : "FAIL");
            printf("Could not discover camera capabilities\n");
            printf("Final score: %.1f/100\n", result.performance_score);
        } else {
            if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
                printf("Device does not support video capture.\n");
                close(fd);
                printf("\n=== Test Summary ===\n");
                printf("Default settings: %s\n", result.success ? "PASS" : "FAIL");
                printf("Could not discover camera capabilities\n");
                printf("Final score: %.1f/100\n", result.performance_score);
            } else {
                printf("Camera capabilities discovered:\n");
                
                // Enumerate formats and frame sizes
                v4l2_fmtdesc fmtdesc;
                fmtdesc.index = 0;
                fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                
                while (ioctl(fd, VIDIOC_ENUM_FMT, &fmtdesc) != -1) {
                    printf("  Format: %s (0x%08x)\n", fmtdesc.description, fmtdesc.pixelformat);
                    
                    v4l2_frmsizeenum frmsize;
                    frmsize.index = 0;
                    frmsize.pixel_format = fmtdesc.pixelformat;
                    
                    while (ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frmsize) != -1) {
                        if (frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE) {
                            int width = frmsize.discrete.width;
                            int height = frmsize.discrete.height;
                            printf("    Resolution: %dx%d\n", width, height);
                            
                            // Test this specific resolution using V4L2 directly
                            printf("      Testing %dx%d...\n", width, height);
                            
                            // Close OpenCV first to free the device
                            cleanup_camera_test(&camera);
                            
                            // Set format using V4L2
                            v4l2_format fmt;
                            memset(&fmt, 0, sizeof(fmt));
                            fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                            fmt.fmt.pix.width = width;
                            fmt.fmt.pix.height = height;
                            fmt.fmt.pix.pixelformat = fmtdesc.pixelformat;
                            fmt.fmt.pix.field = V4L2_FIELD_ANY;
                            
                            if (ioctl(fd, VIDIOC_S_FMT, &fmt) == 0) {
                                // Check what was actually set
                                int actual_width = fmt.fmt.pix.width;
                                int actual_height = fmt.fmt.pix.height;
                                
                                printf("        V4L2 set: %dx%d\n", actual_width, actual_height);
                                
                                // Check if the resolution is close to what we requested
                                bool resolution_set = abs(actual_width - width) <= 100 && abs(actual_height - height) <= 100;
                                
                                if (resolution_set) {
                                    // Now test actual frame capture at this resolution
                                    printf("        Testing frame capture...\n");
                                    
                                    // Set up streaming
                                    v4l2_requestbuffers req;
                                    memset(&req, 0, sizeof(req));
                                    req.count = 1;
                                    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                                    req.memory = V4L2_MEMORY_MMAP;
                                    
                                    if (ioctl(fd, VIDIOC_REQBUFS, &req) == 0) {
                                        // Map the buffer
                                        v4l2_buffer buf;
                                        memset(&buf, 0, sizeof(buf));
                                        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                                        buf.memory = V4L2_MEMORY_MMAP;
                                        buf.index = 0;
                                        
                                        if (ioctl(fd, VIDIOC_QUERYBUF, &buf) == 0) {
                                            void* buffer = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);
                                            
                                            if (buffer != MAP_FAILED) {
                                                // Start streaming
                                                int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
                                                if (ioctl(fd, VIDIOC_STREAMON, &type) == 0) {
                                                    // Queue the buffer
                                                    if (ioctl(fd, VIDIOC_QBUF, &buf) == 0) {
                                                        // Try to capture a frame
                                                        if (ioctl(fd, VIDIOC_DQBUF, &buf) == 0) {
                                                            // Check if we got a valid frame
                                                            bool frame_captured = (buf.bytesused > 0);
                                                            
                                                            if (frame_captured) {
                                                                printf("        ✓ %dx%d - REAL TEST PASSED (captured %d bytes)\n", 
                                                                       width, height, buf.bytesused);
                                                                successful_capabilities++;
                                                                
                                                                // Score based on resolution
                                                                int total_pixels = width * height;
                                                                double resolution_score = 0.0;
                                                                
                                                                if (total_pixels >= 3840 * 2160) {
                                                                    resolution_score = 100.0; // 4K+
                                                                } else if (total_pixels >= 1920 * 1080) {
                                                                    resolution_score = 90.0;  // Full HD
                                                                } else if (total_pixels >= 1280 * 720) {
                                                                    resolution_score = 80.0;  // HD
                                                                } else if (total_pixels >= 1024 * 768) {
                                                                    resolution_score = 70.0;  // XGA
                                                                } else if (total_pixels >= 640 * 480) {
                                                                    resolution_score = 60.0;  // VGA
                                                                } else {
                                                                    resolution_score = 50.0;  // Lower
                                                                }
                                                                
                                                                // Bonus for high frame rates (we'll use a default since V4L2 doesn't report FPS here)
                                                                double fps_bonus = 2.0; // Conservative bonus
                                                                
                                                                total_capability_score += resolution_score + fps_bonus;
                                                            } else {
                                                                printf("        ✗ %dx%d - Frame capture FAILED (no data)\n", width, height);
                                                            }
                                                            
                                                            // Re-queue the buffer
                                                            ioctl(fd, VIDIOC_QBUF, &buf);
                                                        } else {
                                                            printf("        ✗ %dx%d - Frame capture FAILED (DQBUF error)\n", width, height);
                                                        }
                                                    } else {
                                                        printf("        ✗ %dx%d - Frame capture FAILED (QBUF error)\n", width, height);
                                                    }
                                                    
                                                    // Stop streaming
                                                    ioctl(fd, VIDIOC_STREAMOFF, &type);
                                                } else {
                                                    printf("        ✗ %dx%d - Frame capture FAILED (STREAMON error)\n", width, height);
                                                }
                                                
                                                // Unmap the buffer
                                                munmap(buffer, buf.length);
                                            } else {
                                                printf("        ✗ %dx%d - Frame capture FAILED (mmap error)\n", width, height);
                                            }
                                        } else {
                                            printf("        ✗ %dx%d - Frame capture FAILED (QUERYBUF error)\n", width, height);
                                        }
                                    } else {
                                        printf("        ✗ %dx%d - Frame capture FAILED (REQBUFS error)\n", width, height);
                                    }
                                } else {
                                    printf("        ✗ %dx%d - V4L2 Test FAILED (got %dx%d)\n", 
                                           width, height, actual_width, actual_height);
                                }
                                
                                tested_capabilities++;
                            } else {
                                printf("        ✗ Failed to set %dx%d via V4L2\n", width, height);
                            }
                        } else if (frmsize.type == V4L2_FRMSIZE_TYPE_STEPWISE) {
                            printf("    Resolution Range: %dx%d to %dx%d (step: %dx%d)\n", 
                                   frmsize.stepwise.min_width, frmsize.stepwise.min_height,
                                   frmsize.stepwise.max_width, frmsize.stepwise.max_height,
                                   frmsize.stepwise.step_width, frmsize.stepwise.step_height);
                        }
                        frmsize.index++;
                    }
                    fmtdesc.index++;
                }
                
                close(fd);
                
                // Calculate final score
                if (successful_capabilities > 0) {
                    double average_capability_score = total_capability_score / successful_capabilities;
                    result.performance_score = (result.performance_score + average_capability_score) / 2.0;
                    
                    printf("\n=== Test Summary ===\n");
                    printf("Default settings: %s\n", result.success ? "PASS" : "FAIL");
                    printf("Tested capabilities: %d\n", tested_capabilities);
                    printf("Successful capabilities: %d\n", successful_capabilities);
                    printf("Average capability score: %.1f/100\n", average_capability_score);
                    printf("Final score: %.1f/100\n", result.performance_score);
                } else {
                    printf("\n=== Test Summary ===\n");
                    printf("Default settings: %s\n", result.success ? "PASS" : "FAIL");
                    printf("No additional capabilities found beyond default settings\n");
                    printf("Final score: %.1f/100\n", result.performance_score);
                }
            }
        }
    }
    
    // Calculate final score
    if (successful_capabilities > 0) {
        double average_capability_score = total_capability_score / successful_capabilities;
        result.performance_score = (result.performance_score + average_capability_score) / 2.0;
        
        printf("\n=== Test Summary ===\n");
        printf("Default settings: %s\n", result.success ? "PASS" : "FAIL");
        printf("Tested capabilities: %d\n", tested_capabilities);
        printf("Successful capabilities: %d\n", successful_capabilities);
        printf("Average capability score: %.1f/100\n", average_capability_score);
        printf("Final score: %.1f/100\n", result.performance_score);
    } else {
        printf("\n=== Test Summary ===\n");
        printf("Default settings: %s\n", result.success ? "PASS" : "FAIL");
        printf("No additional capabilities found beyond default settings\n");
        printf("Final score: %.1f/100\n", result.performance_score);
    }
    
    // Calculate final score
    if (successful_capabilities > 0) {
        double average_capability_score = total_capability_score / successful_capabilities;
        result.performance_score = (result.performance_score + average_capability_score) / 2.0;
        
        printf("\n=== Test Summary ===\n");
        printf("Default settings: %s\n", result.success ? "PASS" : "FAIL");
        printf("Tested capabilities: %d\n", tested_capabilities);
        printf("Successful capabilities: %d\n", successful_capabilities);
        printf("Average capability score: %.1f/100\n", average_capability_score);
        printf("Final score: %.1f/100\n", result.performance_score);
    } else {
        printf("\n=== Test Summary ===\n");
        printf("Default settings: %s\n", result.success ? "PASS" : "FAIL");
        printf("No additional capabilities found beyond default settings\n");
        printf("Final score: %.1f/100\n", result.performance_score);
    }
    
    cleanup_camera_test(&camera);
    return result;
}

// Function to run all camera tests
test_summary_t run_all_camera_tests(int camera_index) {
    test_summary_t summary = {0, 0, 0, 0.0, ""};
    
    printf("\n=== Running All Camera Tests ===\n");
    
    // Test 1: Initialization
    test_result_t result = test_camera_initialization(camera_index);
    summary.total_tests++;
    if (result.success) {
        
        summary.passed_tests++;
        printf("✓ Camera Initialization: PASS (%.1f/100)\n", result.performance_score);
    } else {
        summary.failed_tests++;
        printf("✗ Camera Initialization: FAIL (%.1f/100)\n", result.performance_score);
    }
    summary.average_score += result.performance_score;
    
    // Test 2: Capture
    result = test_camera_capture(camera_index);
    summary.total_tests++;
    if (result.success) {
        summary.passed_tests++;
        printf("✓ Camera Capture: PASS (%.1f/100)\n", result.performance_score);
    } else {
        summary.failed_tests++;
        printf("✗ Camera Capture: FAIL (%.1f/100)\n", result.performance_score);
    }
    summary.average_score += result.performance_score;
    
    // Test 3: Resolution
    result = test_camera_resolution(camera_index);
    summary.total_tests++;
    if (result.success) {
        summary.passed_tests++;
        printf("✓ Camera Resolution: PASS (%.1f/100)\n", result.performance_score);
    } else {
        summary.failed_tests++;
        printf("✗ Camera Resolution: FAIL (%.1f/100)\n", result.performance_score);
    }
    summary.average_score += result.performance_score;
    
    // Test 4: FPS
    result = test_camera_fps(camera_index);
    summary.total_tests++;
    if (result.success) {
        summary.passed_tests++;
        printf("✓ Camera FPS: PASS (%.1f/100)\n", result.performance_score);
    } else {
        summary.failed_tests++;
        printf("✗ Camera FPS: FAIL (%.1f/100)\n", result.performance_score);
    }
    summary.average_score += result.performance_score;
    
    // Test 5: Supported Capabilities
    result = test_supported_camera_capabilities(camera_index);
    summary.total_tests++;
    if (result.success) {
        summary.passed_tests++;
        printf("✓ Supported Camera Capabilities: PASS (%.1f/100)\n", result.performance_score);
    } else {
        summary.failed_tests++;
        printf("✗ Supported Camera Capabilities: FAIL (%.1f/100)\n", result.performance_score);
    }
    summary.average_score += result.performance_score;
    
    // Calculate average score
    if (summary.total_tests > 0) {
        summary.average_score /= summary.total_tests;
    }
    
    // Create summary string
    snprintf(summary.summary, sizeof(summary.summary),
             "Camera Tests: %d/%d passed, Average Score: %.1f/100",
             summary.passed_tests, summary.total_tests, summary.average_score);
    
    return summary;
}

// Function to handle camera-specific commands
int handle_camera_commands(const char* test_type, int camera_index, bool interactive_mode) {
    if (interactive_mode) {
        camera_test_t camera;
        if (init_camera_test(&camera, camera_index) == 0) {
            run_camera_test_console(&camera);
            cleanup_camera_test(&camera);
        } else {
            printf("Error: Could not initialize camera %d\n", camera_index);
            return 1;
        }
    } else if (test_type) {
        if (strcmp(test_type, "all") == 0) {
            run_all_camera_tests(camera_index);
        } else if (strcmp(test_type, "init") == 0) {
            test_result_t result = test_camera_initialization(camera_index);
            printf("Camera Initialization Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else if (strcmp(test_type, "capture") == 0) {
            test_result_t result = test_camera_capture(camera_index);
            printf("Camera Capture Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else if (strcmp(test_type, "resolution") == 0) {
            test_result_t result = test_camera_resolution(camera_index);
            printf("Camera Resolution Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else if (strcmp(test_type, "fps") == 0) {
            test_result_t result = test_camera_fps(camera_index);
            printf("Camera FPS Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else if (strcmp(test_type, "all_resolutions") == 0) {
            test_result_t result = test_all_resolutions(camera_index);
            printf("Camera All Resolutions Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else if (strcmp(test_type, "capabilities") == 0) {
            test_result_t result = test_camera_capabilities(camera_index);
            printf("Camera Capabilities Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else if (strcmp(test_type, "supported_capabilities") == 0) {
            test_result_t result = test_supported_camera_capabilities(camera_index);
            printf("Supported Camera Capabilities Test: %s\n", result.success ? "PASS" : "FAIL");
            printf("Message: %s\n", result.message);
            printf("Score: %.1f/100\n", result.performance_score);
        } else {
            printf("Unknown camera test type: %s\n", test_type);
            return 1;
        }
    } else {
        // Default: run all camera tests
        run_all_camera_tests(camera_index);
    }
    
    return 0;
}

} // extern "C"
