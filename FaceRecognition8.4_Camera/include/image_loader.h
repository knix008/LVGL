#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include "common.h"

class ImageLoader {
public:
    ImageLoader();
    ~ImageLoader();

    // Load image from file (JPG, PNG, GIF)
    bool load_image(const std::string& file_path, ImageData& image_data);

    // Load image from camera frame (cv::Mat)
    bool load_from_frame(const cv::Mat& frame, ImageData& image_data);

    // Get supported image formats
    static std::vector<std::string> get_supported_formats();

    // Validate if file is supported image format
    static bool is_supported_format(const std::string& file_path);

    // Resize image while maintaining aspect ratio
    static cv::Mat resize_to_fit(const cv::Mat& image, int max_width, int max_height);

    // Convert image to RGB if needed
    static cv::Mat to_rgb(const cv::Mat& image);

private:
    static const std::vector<std::string> SUPPORTED_FORMATS;
};

#endif // IMAGE_LOADER_H
