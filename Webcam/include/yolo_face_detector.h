// Face Detection using YOLOv8 Face Model
// This implementation is designed specifically for face detection

#ifndef YOLO_FACE_DETECTOR_H
#define YOLO_FACE_DETECTOR_H

// Cpp native
#include <fstream>
#include <vector>
#include <string>
#include <random>

// OpenCV / DNN / Inference
#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>

struct Detection
{
    int class_id{0};
    std::string className{};
    float confidence{0.0};
    cv::Scalar color{};
    cv::Rect box{};
};

class YOLOFaceDetector
{
public:
    YOLOFaceDetector(const std::string &modelPath, 
                     const cv::Size &modelInputShape = {640, 640}, 
                     const bool &runWithCuda = false);
    
    std::vector<Detection> runInference(const cv::Mat &input);

private:
    void loadOnnxNetwork();
    void convertPyTorchToOnnx();
    cv::Mat formatToSquare(const cv::Mat &source, int *pad_x, int *pad_y, float *scale);

    std::string modelPath{};
    std::vector<std::string> classes{"face"}; // Only face class for face detection
    cv::Size2f modelShape{};
    bool cudaEnabled{};

    float modelConfidenceThreshold {0.25};
    float modelScoreThreshold      {0.45};
    float modelNMSThreshold        {0.50};

    bool letterBoxForSquare = true;

    cv::dnn::Net net;
};

#endif // YOLO_FACE_DETECTOR_H
