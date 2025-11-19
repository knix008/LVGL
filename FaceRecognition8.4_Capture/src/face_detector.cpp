#include "face_detector.h"
#include <iostream>
#include <opencv2/objdetect.hpp>
#include <freetype2/ft2build.h>
#include FT_FREETYPE_H

FaceDetector::FaceDetector()
    : min_face_width(30), min_face_height(30) {
}

FaceDetector::~FaceDetector() {
}

bool FaceDetector::initialize() {
    return load_cascade();
}

bool FaceDetector::load_cascade() {
    // Try multiple common cascade paths for OpenCV
    std::vector<std::string> cascade_paths = {
        "/usr/share/opencv4/haarcascades/haarcascade_frontalface_default.xml",
        "/usr/share/opencv/haarcascades/haarcascade_frontalface_default.xml",
        "/opt/local/share/opencv4/haarcascades/haarcascade_frontalface_default.xml",
        "./cascade_files/haarcascade_frontalface_default.xml"
    };

    for (const auto& path : cascade_paths) {
        if (face_cascade.load(path)) {
            std::cout << "Face cascade classifier loaded from: " << path << std::endl;
            return true;
        }
    }

    std::cerr << "Failed to load face cascade classifier" << std::endl;
    std::cerr << "Please ensure OpenCV cascade files are installed" << std::endl;
    return false;
}

std::vector<Face> FaceDetector::detect_faces(const cv::Mat& image) {
    std::vector<Face> detected_faces;

    if (image.empty()) {
        std::cerr << "Input image is empty" << std::endl;
        return detected_faces;
    }

    if (face_cascade.empty()) {
        std::cerr << "Face cascade classifier not loaded" << std::endl;
        return detected_faces;
    }

    // Convert to grayscale if necessary
    cv::Mat gray_image;
    if (image.channels() == 3) {
        cv::cvtColor(image, gray_image, cv::COLOR_RGB2GRAY);
    } else if (image.channels() == 4) {
        cv::cvtColor(image, gray_image, cv::COLOR_RGBA2GRAY);
    } else {
        gray_image = image.clone();
    }

    // Equalize histogram for better detection
    cv::equalizeHist(gray_image, gray_image);

    // Detect faces
    std::vector<cv::Rect> faces;
    face_cascade.detectMultiScale(
        gray_image,
        faces,
        1.1,                    // scale factor
        6,                      // min neighbors (increased from 4 to filter out false positives)
        0,                      // flags
        cv::Size(min_face_width, min_face_height),  // min face size
        cv::Size()              // max face size
    );

    // Merge overlapping detections to avoid counting the same face multiple times
    // Uses Intersection over Union (IoU) to group similar detections
    std::vector<cv::Rect> merged_faces;
    std::vector<bool> used(faces.size(), false);

    for (size_t i = 0; i < faces.size(); i++) {
        if (used[i]) continue;

        // Start a new group with this face
        std::vector<cv::Rect> group;
        group.push_back(faces[i]);
        used[i] = true;

        // Find all overlapping faces using IoU threshold
        bool found_more = true;
        while (found_more) {
            found_more = false;
            for (size_t j = i + 1; j < faces.size(); j++) {
                if (used[j]) continue;

                // Calculate IoU (Intersection over Union)
                cv::Rect intersection = faces[j] & group.back();
                double intersection_area = static_cast<double>(intersection.width * intersection.height);
                double union_area = static_cast<double>(faces[j].area() + group.back().area() - intersection_area);

                double iou = (union_area > 0) ? (intersection_area / union_area) : 0.0;

                // If IoU > 0.1 (10% overlap), consider them the same face
                if (iou > 0.1) {
                    group.push_back(faces[j]);
                    used[j] = true;
                    found_more = true;
                    break;  // Restart the search with new merged rect
                }
            }
        }

        // Merge all faces in the group using union of all rectangles
        cv::Rect merged = group[0];
        for (size_t k = 1; k < group.size(); k++) {
            merged = merged | group[k];
        }

        merged_faces.push_back(merged);
    }

    // Convert detected rectangles to Face structures
    for (const auto& rect : merged_faces) {
        Face face;
        face.bbox = rect;
        face.confidence = 0.8f; // Cascade classifier doesn't provide confidence, use fixed value
        detected_faces.push_back(face);
    }

    std::cout << "Detected " << detected_faces.size() << " faces in image (after merging overlaps)" << std::endl;

    return detected_faces;
}

cv::Mat FaceDetector::extract_face(const cv::Mat& image, const cv::Rect& face_bbox) {
    if (image.empty() || face_bbox.width <= 0 || face_bbox.height <= 0) {
        return cv::Mat();
    }

    // Ensure bounding box is within image bounds
    int x = std::max(0, face_bbox.x);
    int y = std::max(0, face_bbox.y);
    int width = std::min(face_bbox.width, image.cols - x);
    int height = std::min(face_bbox.height, image.rows - y);

    if (width <= 0 || height <= 0) {
        return cv::Mat();
    }

    cv::Rect safe_bbox(x, y, width, height);
    return image(safe_bbox).clone();
}

void FaceDetector::draw_korean_text(cv::Mat& image, const std::string& text, cv::Point position,
                                    const std::string& font_path, int font_size, cv::Scalar color) {
    FT_Library ft_library;
    FT_Face ft_face;

    // Initialize FreeType
    if (FT_Init_FreeType(&ft_library)) {
        std::cerr << "Failed to initialize FreeType library" << std::endl;
        return;
    }

    // Load font
    if (FT_New_Face(ft_library, font_path.c_str(), 0, &ft_face)) {
        std::cerr << "Failed to load font: " << font_path << std::endl;
        FT_Done_FreeType(ft_library);
        return;
    }

    // Set font size
    FT_Set_Pixel_Sizes(ft_face, 0, font_size);

    int x = position.x;
    int y = position.y;

    // Process each character in the text
    for (const unsigned char* p = (const unsigned char*)text.c_str(); *p; ) {
        // Handle UTF-8 multi-byte characters
        uint32_t ch = 0;
        if ((*p & 0x80) == 0) {
            ch = *p++;
        } else if ((*p & 0xE0) == 0xC0) {
            ch = (p[0] & 0x1F) << 6 | (p[1] & 0x3F);
            p += 2;
        } else if ((*p & 0xF0) == 0xE0) {
            ch = (p[0] & 0x0F) << 12 | (p[1] & 0x3F) << 6 | (p[2] & 0x3F);
            p += 3;
        } else if ((*p & 0xF8) == 0xF0) {
            ch = (p[0] & 0x07) << 18 | (p[1] & 0x3F) << 12 | (p[2] & 0x3F) << 6 | (p[3] & 0x3F);
            p += 4;
        } else {
            p++;
            continue;
        }

        // Load glyph
        if (FT_Load_Char(ft_face, ch, FT_LOAD_RENDER)) {
            continue;
        }

        FT_GlyphSlot slot = ft_face->glyph;
        FT_Bitmap bitmap = slot->bitmap;

        // Draw glyph bitmap on image
        int bitmap_top = slot->bitmap_top;
        int bitmap_left = slot->bitmap_left;

        for (int row = 0; row < static_cast<int>(bitmap.rows); row++) {
            for (int col = 0; col < static_cast<int>(bitmap.width); col++) {
                int img_y = y - bitmap_top + row;
                int img_x = x + bitmap_left + col;

                if (img_x >= 0 && img_x < image.cols && img_y >= 0 && img_y < image.rows) {
                    unsigned char alpha = bitmap.buffer[row * bitmap.pitch + col];
                    if (alpha > 0) {
                        cv::Vec3b pixel = image.at<cv::Vec3b>(img_y, img_x);
                        // Blend with font color
                        float blend_factor = alpha / 255.0f;
                        pixel[0] = cv::saturate_cast<uchar>(pixel[0] * (1 - blend_factor) + color[0] * blend_factor);
                        pixel[1] = cv::saturate_cast<uchar>(pixel[1] * (1 - blend_factor) + color[1] * blend_factor);
                        pixel[2] = cv::saturate_cast<uchar>(pixel[2] * (1 - blend_factor) + color[2] * blend_factor);
                        image.at<cv::Vec3b>(img_y, img_x) = pixel;
                    }
                }
            }
        }

        // Move to next character position
        x += slot->advance.x >> 6;
    }

    // Cleanup
    FT_Done_Face(ft_face);
    FT_Done_FreeType(ft_library);
}

cv::Mat FaceDetector::draw_faces(const cv::Mat& image, const std::vector<Face>& faces) {
    cv::Mat result = image.clone();

    // Convert to BGR for display if RGB
    if (result.channels() == 3) {
        cv::cvtColor(result, result, cv::COLOR_RGB2BGR);
    }

    // Draw corner marks and confidence scores
    for (const auto& face : faces) {
        // Expand bounding box by 30% to add space around the detected object
        int expanded_width = static_cast<int>(face.bbox.width * 1.3);
        int expanded_height = static_cast<int>(face.bbox.height * 1.3);
        int expand_x = (expanded_width - face.bbox.width) / 2;
        int expand_y = (expanded_height - face.bbox.height) / 2;

        // Calculate expanded corner coordinates
        int x1 = std::max(0, face.bbox.x - expand_x);
        int y1 = std::max(0, face.bbox.y - expand_y);
        int x2 = face.bbox.x + face.bbox.width + expand_x;
        int y2 = face.bbox.y + face.bbox.height + expand_y;

        // Draw corner marks instead of full rectangle
        int corner_length = 20;  // Length of corner marks
        int line_thickness = 2;
        cv::Scalar corner_color(0, 255, 0);  // Green color

        // Top-left corner
        cv::line(result, cv::Point(x1, y1), cv::Point(x1 + corner_length, y1), corner_color, line_thickness);
        cv::line(result, cv::Point(x1, y1), cv::Point(x1, y1 + corner_length), corner_color, line_thickness);

        // Top-right corner
        cv::line(result, cv::Point(x2, y1), cv::Point(x2 - corner_length, y1), corner_color, line_thickness);
        cv::line(result, cv::Point(x2, y1), cv::Point(x2, y1 + corner_length), corner_color, line_thickness);

        // Bottom-left corner
        cv::line(result, cv::Point(x1, y2), cv::Point(x1 + corner_length, y2), corner_color, line_thickness);
        cv::line(result, cv::Point(x1, y2), cv::Point(x1, y2 - corner_length), corner_color, line_thickness);

        // Bottom-right corner
        cv::line(result, cv::Point(x2, y2), cv::Point(x2 - corner_length, y2), corner_color, line_thickness);
        cv::line(result, cv::Point(x2, y2), cv::Point(x2, y2 - corner_length), corner_color, line_thickness);

        // Draw Korean "얼굴: " (Face:) label with confidence percentage above the bounding box
        std::string conf_percentage = std::to_string(static_cast<int>(face.confidence * 100)) + "%";
        std::string label_with_confidence = "얼굴: " + conf_percentage;

        // Position label above the expanded bounding box, aligned with left edge (x1)
        int label_y = y1 - 8;  // 8 pixels above the top of the expanded bounding box

        // Draw Korean "얼굴: " and confidence text using FreeType
        try {
            draw_korean_text(result, label_with_confidence, cv::Point(x1 + 5, label_y),
                           "assets/NanumGothicCoding.ttf", 12, cv::Scalar(0, 255, 0));
        } catch (...) {
            // Fallback if Korean text drawing fails
            std::cerr << "Warning: Failed to draw Korean text on face" << std::endl;
        }

        // Draw person ID/name if recognized
        if (!face.person_id.empty()) {
            std::string id_label = face.person_id;
            cv::putText(result, id_label,
                       cv::Point(face.bbox.x + 5, face.bbox.y + face.bbox.height + 20),
                       cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1);
        }
    }

    return result;
}

void FaceDetector::set_min_face_size(int width, int height) {
    min_face_width = std::max(1, width);
    min_face_height = std::max(1, height);
}
