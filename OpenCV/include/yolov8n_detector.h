#ifndef YOLOV8N_DETECTOR_H
#define YOLOV8N_DETECTOR_H

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <vector>
#include <string>
#include <map>

struct Detection {
    cv::Rect bbox;
    float confidence;
    int class_id;
    std::string class_name;
};

class YOLOv8nDetector {
private:
    cv::dnn::Net net;
    std::vector<std::string> class_names;
    float conf_threshold;
    float nms_threshold;
    int input_width;
    int input_height;
    
    // COCO dataset class names
    const std::vector<std::string> COCO_CLASSES = {
        "person", "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck", "boat",
        "traffic light", "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat",
        "dog", "horse", "sheep", "cow", "elephant", "bear", "zebra", "giraffe", "backpack",
        "umbrella", "handbag", "tie", "suitcase", "frisbee", "skis", "snowboard", "sports ball",
        "kite", "baseball bat", "baseball glove", "skateboard", "surfboard", "tennis racket", "bottle",
        "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple", "sandwich",
        "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "couch",
        "potted plant", "bed", "dining table", "toilet", "tv", "laptop", "mouse", "remote",
        "keyboard", "cell phone", "microwave", "oven", "toaster", "sink", "refrigerator",
        "book", "clock", "vase", "scissors", "teddy bear", "hair drier", "toothbrush"
    };

public:
    YOLOv8nDetector(const std::string& model_path, 
                     float conf_thresh = 0.25f, 
                     float nms_thresh = 0.45f,
                     int input_w = 640, 
                     int input_h = 640);
    
    std::vector<Detection> predict(const cv::Mat& image);
    void draw_detections(cv::Mat& image, const std::vector<Detection>& detections);
    void save_result(const cv::Mat& image, const std::string& filename);
    void show_result(const cv::Mat& image, const std::string& window_name = "YOLOv8n Detection");

private:
    std::vector<Detection> post_process(const cv::Mat& output, const cv::Size& original_size);
    std::vector<Detection> apply_nms(const std::vector<Detection>& detections);
};

#endif // YOLOV8N_DETECTOR_H 