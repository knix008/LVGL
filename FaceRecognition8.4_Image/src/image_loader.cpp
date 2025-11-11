#include "image_loader.h"
#include <iostream>
#include <algorithm>
#include <filesystem>

const std::vector<std::string> ImageLoader::SUPPORTED_FORMATS = {
    ".jpg", ".jpeg", ".png", ".gif", ".bmp", ".tiff"
};

ImageLoader::ImageLoader() {
}

ImageLoader::~ImageLoader() {
}

bool ImageLoader::load_image(const std::string& file_path, ImageData& image_data) {
    // Check if file exists
    if (!std::filesystem::exists(file_path)) {
        std::cerr << "Image file not found: " << file_path << std::endl;
        return false;
    }

    // Check if format is supported
    if (!is_supported_format(file_path)) {
        std::cerr << "Unsupported image format: " << file_path << std::endl;
        return false;
    }

    // Load image using OpenCV
    cv::Mat image = cv::imread(file_path);
    if (image.empty()) {
        std::cerr << "Failed to load image: " << file_path << std::endl;
        return false;
    }

    // Convert to RGB if necessary (OpenCV loads as BGR by default)
    cv::Mat rgb_image = to_rgb(image);

    image_data.mat = rgb_image;
    image_data.file_path = file_path;
    image_data.faces.clear();

    std::cout << "Image loaded successfully: " << file_path
              << " (" << rgb_image.cols << "x" << rgb_image.rows << ")" << std::endl;

    return true;
}

std::vector<std::string> ImageLoader::get_supported_formats() {
    return SUPPORTED_FORMATS;
}

bool ImageLoader::is_supported_format(const std::string& file_path) {
    // Get file extension
    size_t dot_pos = file_path.find_last_of('.');
    if (dot_pos == std::string::npos) {
        return false;
    }

    std::string extension = file_path.substr(dot_pos);

    // Convert to lowercase
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);

    // Check if extension is in supported formats
    return std::find(SUPPORTED_FORMATS.begin(), SUPPORTED_FORMATS.end(), extension)
           != SUPPORTED_FORMATS.end();
}

cv::Mat ImageLoader::resize_to_fit(const cv::Mat& image, int max_width, int max_height) {
    if (image.empty()) {
        return image;
    }

    int width = image.cols;
    int height = image.rows;

    // Calculate scale factor to fit within max dimensions while maintaining aspect ratio
    float scale = std::min(static_cast<float>(max_width) / width,
                          static_cast<float>(max_height) / height);

    if (scale >= 1.0f) {
        // Image is already smaller than max dimensions
        return image.clone();
    }

    int new_width = static_cast<int>(width * scale);
    int new_height = static_cast<int>(height * scale);

    cv::Mat resized;
    cv::resize(image, resized, cv::Size(new_width, new_height), 0, 0, cv::INTER_LINEAR);

    return resized;
}

cv::Mat ImageLoader::to_rgb(const cv::Mat& image) {
    if (image.empty()) {
        return image;
    }

    cv::Mat result;

    // OpenCV loads images as BGR by default
    if (image.channels() == 3) {
        cv::cvtColor(image, result, cv::COLOR_BGR2RGB);
    } else if (image.channels() == 4) {
        cv::cvtColor(image, result, cv::COLOR_BGRA2RGB);
    } else if (image.channels() == 1) {
        cv::cvtColor(image, result, cv::COLOR_GRAY2RGB);
    } else {
        result = image.clone();
    }

    return result;
}
