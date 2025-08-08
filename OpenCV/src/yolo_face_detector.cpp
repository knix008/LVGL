// Face Detection using YOLOv8 Face Model
// This implementation is designed specifically for face detection

#include "yolo_face_detector.h"
#include "simple_model_converter.h"
#include <iostream>
#include <fstream>
#include <sstream>

YOLOFaceDetector::YOLOFaceDetector(const std::string &modelPath, 
                                   const cv::Size &modelInputShape, 
                                   const bool &runWithCuda)
    : modelPath(modelPath),
      modelShape(modelInputShape),
      cudaEnabled(runWithCuda),
      modelConfidenceThreshold(0.25),
      modelScoreThreshold(0.45),
      modelNMSThreshold(0.50),
      letterBoxForSquare(true)
{
    // For face detection, we only have one class: "face"
    classes = {"face"};
    
    // Try to load as ONNX first, if that fails, we'll need to convert PyTorch model
    if (modelPath.find(".onnx") != std::string::npos) {
        loadOnnxNetwork();
    } else if (modelPath.find(".pt") != std::string::npos) {
        std::cout << "PyTorch model detected. Converting to ONNX..." << std::endl;
        convertPyTorchToOnnx();
        loadOnnxNetwork();
    } else {
        std::cerr << "Unsupported model format. Please use .onnx or .pt files." << std::endl;
        throw std::runtime_error("Unsupported model format");
    }
}

std::vector<Detection> YOLOFaceDetector::runInference(const cv::Mat &input)
{
    cv::Mat modelInput = input;
    int pad_x, pad_y;
    float scale;
    if (letterBoxForSquare && modelShape.width == modelShape.height)
        modelInput = formatToSquare(modelInput, &pad_x, &pad_y, &scale);

    cv::Mat blob;
    cv::dnn::blobFromImage(modelInput, blob, 1.0/255.0, modelShape, cv::Scalar(), true, false);
    net.setInput(blob);

    std::vector<cv::Mat> outputs;
    net.forward(outputs, net.getUnconnectedOutLayersNames());

    int rows = outputs[0].size[1];
    int dimensions = outputs[0].size[2];

    bool yolov8 = false;
    // yolov5 has an output of shape (batchSize, 25200, 85) (Num classes + box[x,y,w,h] + confidence[c])
    // yolov8 has an output of shape (batchSize, 84,  8400) (Num classes + box[x,y,w,h])
    if (dimensions > rows) // Check if the shape[2] is more than shape[1] (yolov8)
    {
        yolov8 = true;
        rows = outputs[0].size[2];
        dimensions = outputs[0].size[1];

        outputs[0] = outputs[0].reshape(1, dimensions);
        cv::transpose(outputs[0], outputs[0]);
    }
    float *data = (float *)outputs[0].data;

    std::vector<int> class_ids;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;

    for (int i = 0; i < rows; ++i)
    {
        if (yolov8)
        {
            float *classes_scores = data+4;

            cv::Mat scores(1, classes.size(), CV_32FC1, classes_scores);
            cv::Point class_id;
            double maxClassScore;

            minMaxLoc(scores, 0, &maxClassScore, 0, &class_id);

            if (maxClassScore > modelScoreThreshold)
            {
                confidences.push_back(maxClassScore);
                class_ids.push_back(class_id.x);

                float x = data[0];
                float y = data[1];
                float w = data[2];
                float h = data[3];

                int left = int((x - 0.5 * w - pad_x) / scale);
                int top = int((y - 0.5 * h - pad_y) / scale);

                int width = int(w / scale);
                int height = int(h / scale);

                boxes.push_back(cv::Rect(left, top, width, height));
            }
        }
        else // yolov5
        {
            float confidence = data[4];

            if (confidence >= modelConfidenceThreshold)
            {
                float *classes_scores = data+5;

                cv::Mat scores(1, classes.size(), CV_32FC1, classes_scores);
                cv::Point class_id;
                double max_class_score;

                minMaxLoc(scores, 0, &max_class_score, 0, &class_id);

                if (max_class_score > modelScoreThreshold)
                {
                    confidences.push_back(confidence);
                    class_ids.push_back(class_id.x);

                    float x = data[0];
                    float y = data[1];
                    float w = data[2];
                    float h = data[3];

                    int left = int((x - 0.5 * w - pad_x) / scale);
                    int top = int((y - 0.5 * h - pad_y) / scale);

                    int width = int(w / scale);
                    int height = int(h / scale);

                    boxes.push_back(cv::Rect(left, top, width, height));
                }
            }
        }

        data += dimensions;
    }

    std::vector<int> nms_result;
    cv::dnn::NMSBoxes(boxes, confidences, modelScoreThreshold, modelNMSThreshold, nms_result);

    std::vector<Detection> detections{};
    for (unsigned long i = 0; i < nms_result.size(); ++i)
    {
        int idx = nms_result[i];

        Detection result;
        result.class_id = class_ids[idx];
        result.confidence = confidences[idx];

        // Use a consistent color for faces (blue)
        result.color = cv::Scalar(255, 0, 0); // Blue color for faces

        result.className = classes[result.class_id];
        result.box = boxes[idx];

        detections.push_back(result);
    }

    return detections;
}

void YOLOFaceDetector::loadOnnxNetwork()
{
    net = cv::dnn::readNetFromONNX(modelPath);
    if (cudaEnabled)
    {
        std::cout << "\nRunning on CUDA" << std::endl;
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
    }
    else
    {
        std::cout << "\nRunning on CPU" << std::endl;
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
    }
}

void YOLOFaceDetector::convertPyTorchToOnnx()
{
    std::string onnxPath = modelPath.substr(0, modelPath.find_last_of('.')) + ".onnx";
    
    // Check if ONNX version already exists
    std::ifstream f(onnxPath.c_str());
    if (f.good()) {
        std::cout << "Found existing ONNX model: " << onnxPath << std::endl;
        modelPath = onnxPath;
        f.close();
    } else {
        std::cout << "Converting PyTorch model to ONNX format..." << std::endl;
        
        try {
            // Use simple converter
            SimpleModelConverter converter;
            std::vector<int64_t> inputShape = {1, 3, 640, 640}; // YOLOv8 input shape
            
            if (converter.convertToOnnx(modelPath, onnxPath, inputShape)) {
                std::cout << "Conversion successful!" << std::endl;
                modelPath = onnxPath;
            } else {
                throw std::runtime_error("Model conversion failed");
            }
        } catch (const std::exception& e) {
            std::cout << "Conversion failed: " << e.what() << std::endl;
            std::cout << "Please convert the PyTorch model to ONNX format manually." << std::endl;
            std::cout << "You can use: python3 convert_face_model.py" << std::endl;
            throw std::runtime_error("PyTorch model conversion required");
        }
    }
}

cv::Mat YOLOFaceDetector::formatToSquare(const cv::Mat &source, int *pad_x, int *pad_y, float *scale)
{
    int col = source.cols;
    int row = source.rows;
    int m_inputWidth = modelShape.width;
    int m_inputHeight = modelShape.height;

    *scale = std::min(m_inputWidth / (float)col, m_inputHeight / (float)row);
    int resized_w = col * *scale;
    int resized_h = row * *scale;
    *pad_x = (m_inputWidth - resized_w) / 2;
    *pad_y = (m_inputHeight - resized_h) / 2;

    cv::Mat resized;
    cv::resize(source, resized, cv::Size(resized_w, resized_h));
    cv::Mat result = cv::Mat::zeros(m_inputHeight, m_inputWidth, source.type());
    resized.copyTo(result(cv::Rect(*pad_x, *pad_y, resized_w, resized_h)));
    resized.release();
    return result;
}
