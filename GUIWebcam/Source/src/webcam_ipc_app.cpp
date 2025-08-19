#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <atomic>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <opencv2/opencv.hpp>
#include <onnxruntime_cxx_api.h>

#define SOCKET_PATH "/tmp/opencv_gui_socket"
#define IPC_BUFFER_SIZE 1024

// IPC message structure
typedef struct {
    int type;
    char data[IPC_BUFFER_SIZE - sizeof(int)];
} IPCMessage;

// IPC message types
#define IPC_MSG_DETECTION 1
#define IPC_MSG_FRAME_PROCESSED 2
#define IPC_MSG_STATUS 3
#define IPC_MSG_ERROR 4

class WebcamIPCApp {
private:
    int m_socket_fd;
    struct sockaddr_un m_server_addr;
    std::atomic<bool> m_running;
    std::string m_model_path;
    int m_previous_detection_count = -1; // Track previous detection count
    
    // ONNX Runtime components
    Ort::Env m_env;
    Ort::Session m_session{nullptr};
    bool m_model_loaded = false;
    
    // YOLO parameters
    const int INPUT_WIDTH = 640;
    const int INPUT_HEIGHT = 640;
    const float SCORE_THRESHOLD = 0.1f;  // Lowered from 0.5f
    const float NMS_THRESHOLD = 0.4f;
    const float CONFIDENCE_THRESHOLD = 0.1f;  // Lowered from 0.5f
    
    // Detection tracking
    int m_current_detection_count = 0;
    std::vector<cv::Rect> m_last_detections;
    
public:
    WebcamIPCApp() : m_socket_fd(-1), m_running(false) {
        m_model_path = "../models/yolov8_face_model.onnx";
    }
    
    ~WebcamIPCApp() {
        cleanup();
    }
    
    bool init() {
        // Create socket
        m_socket_fd = socket(AF_UNIX, SOCK_DGRAM, 0);
        if (m_socket_fd == -1) {
            std::cerr << "Failed to create socket: " << strerror(errno) << std::endl;
            return false;
        }
        
        // Set up server address
        memset(&m_server_addr, 0, sizeof(m_server_addr));
        m_server_addr.sun_family = AF_UNIX;
        strncpy(m_server_addr.sun_path, SOCKET_PATH, sizeof(m_server_addr.sun_path) - 1);
        
        std::cout << "Webcam IPC application initialized" << std::endl;
        return true;
    }
    
    void cleanup() {
        if (m_socket_fd != -1) {
            close(m_socket_fd);
            m_socket_fd = -1;
        }
        // Close all OpenCV windows
        cv::destroyAllWindows();
    }
    
    bool send_message(int type, const std::string& data = "") {
        if (m_socket_fd == -1) return false;
        
        IPCMessage msg;
        msg.type = type;
        strncpy(msg.data, data.c_str(), sizeof(msg.data) - 1);
        msg.data[sizeof(msg.data) - 1] = '\0';
        
        ssize_t sent = sendto(m_socket_fd, &msg, sizeof(msg), 0,
                             (struct sockaddr*)&m_server_addr, sizeof(m_server_addr));
        
        if (sent == -1) {
            std::cerr << "Failed to send message: " << strerror(errno) << std::endl;
            return false;
        }
        
        return true;
    }
    
    void set_model_path(const std::string& path) {
        m_model_path = path;
    }
    
    bool load_model() {
        try {
            // Initialize ONNX Runtime environment
            m_env = Ort::Env(ORT_LOGGING_LEVEL_WARNING, "YOLOv8_Face_Detection");
            
            // Session options
            Ort::SessionOptions session_options;
            session_options.SetIntraOpNumThreads(1);
            session_options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);
            
            // Load the model
            m_session = Ort::Session(m_env, m_model_path.c_str(), session_options);
            m_model_loaded = true;
            
            std::cout << "YOLOv8 face detection model loaded successfully" << std::endl;
            return true;
        } catch (const Ort::Exception& e) {
            std::cerr << "Failed to load model: " << e.what() << std::endl;
            return false;
        }
    }
    
    cv::Mat preprocess_image(const cv::Mat& input_image) {
        cv::Mat resized;
        cv::resize(input_image, resized, cv::Size(INPUT_WIDTH, INPUT_HEIGHT));
        
        // Convert to float and normalize
        cv::Mat float_img;
        resized.convertTo(float_img, CV_32F, 1.0/255.0);
        
        // Convert BGR to RGB
        cv::cvtColor(float_img, float_img, cv::COLOR_BGR2RGB);
        
        // Create blob
        cv::Mat blob = cv::dnn::blobFromImage(float_img, 1.0, cv::Size(INPUT_WIDTH, INPUT_HEIGHT), cv::Scalar(), true, false);
        
        return blob;
    }
    
    std::vector<cv::Rect> postprocess_output_dynamic(const cv::Mat& input_image,
                                                     const float* output_data,
                                                     const std::vector<int64_t>& output_shape) {
        std::vector<cv::Rect> detections;
        std::vector<int> class_ids;
        std::vector<float> confidences;
        std::vector<cv::Rect> boxes;

        if (output_data == nullptr || output_shape.empty()) {
            return detections;
        }

        // Expect shape like [1, A, C] or [1, C, A]
        int64_t dim1 = output_shape.size() > 1 ? output_shape[1] : 0;
        int64_t dim2 = output_shape.size() > 2 ? output_shape[2] : 0;

        int rows = 0;      // number of anchors
        int dims = 0;      // values per anchor (bbox + conf + classes)
        bool channels_first = false; // if true: [C, A], else [A, C]

        if (output_shape.size() == 3 && dim1 > 0 && dim2 > 0) {
            // For YOLOv8, the format is typically [batch, channels, anchors]
            // where channels = 5 (x, y, w, h, confidence) for single class
            if (dim1 == 5) {
                dims = static_cast<int>(dim1);  // 5 channels
                rows = static_cast<int>(dim2);  // 8400 anchors
                channels_first = true; // layout [C, A] - channels first
            } else {
                rows = static_cast<int>(dim1);
                dims = static_cast<int>(dim2);
                channels_first = false; // layout [A, C]
            }
        } else if (output_shape.size() == 2) {
            rows = static_cast<int>(output_shape[0]);
            dims = static_cast<int>(output_shape[1]);
            channels_first = false;
        } else {
            // Unknown layout
            return detections;
        }

        if (rows <= 0 || dims < 5) return detections;

        std::cout << "Processing " << rows << " rows with " << dims << " dimensions" << std::endl;

        auto value_at = [&](int anchor_idx, int c) -> float {
            if (channels_first) {
                // [C, A]
                return output_data[c * rows + anchor_idx];
            } else {
                // [A, C]
                return output_data[anchor_idx * dims + c];
            }
        };

        int img_w = input_image.cols;
        int img_h = input_image.rows;

        for (int i = 0; i < rows; ++i) {
            float x = value_at(i, 0);
            float y = value_at(i, 1);
            float w = value_at(i, 2);
            float h = value_at(i, 3);
            float conf = value_at(i, 4);

            // Debug: Print first few raw values to understand the format
            if (i < 3 && conf > 0.001f) {
                std::cout << "Raw values [" << i << "]: x=" << x << ", y=" << y 
                         << ", w=" << w << ", h=" << h << ", conf=" << conf << std::endl;
            }

            // Debug: Print first few confidence values
            if (i < 5) {
                std::cout << "Row " << i << " confidence: " << conf << std::endl;
            }

            if (conf < CONFIDENCE_THRESHOLD) continue;

            int class_id = 0;
            float class_score = 1.0f;

            if (dims > 5) {
                // There are class scores
                int num_classes = dims - 5;
                float best_score = -1.0f;
                int best_id = 0;
                for (int c = 0; c < num_classes; ++c) {
                    float sc = value_at(i, 5 + c);
                    if (sc > best_score) {
                        best_score = sc;
                        best_id = c;
                    }
                }
                if (best_score < SCORE_THRESHOLD) continue;
                class_id = best_id;
                class_score = best_score;
            }

            // Coordinates are in model input space (640x640), need to scale to image space
            float scale_x = static_cast<float>(img_w) / INPUT_WIDTH;
            float scale_y = static_cast<float>(img_h) / INPUT_HEIGHT;
            
            int left = static_cast<int>((x - 0.5f * w) * scale_x);
            int top = static_cast<int>((y - 0.5f * h) * scale_y);
            int width = static_cast<int>(w * scale_x);
            int height = static_cast<int>(h * scale_y);

            // Debug: Print detection details for high confidence detections
            if (conf > 0.01f) {  // Only print for reasonable confidence
                std::cout << "Detection " << i << ": x=" << x << ", y=" << y 
                         << ", w=" << w << ", h=" << h << ", conf=" << conf
                         << " -> rect(" << left << "," << top << "," << width << "x" << height << ")" << std::endl;
            }

            // Clamp to image bounds
            left = std::max(0, std::min(img_w - 1, left));
            top = std::max(0, std::min(img_h - 1, top));
            width = std::max(0, std::min(img_w - left, width));
            height = std::max(0, std::min(img_h - top, height));

            boxes.emplace_back(left, top, width, height);
            confidences.push_back(conf * class_score);
            class_ids.push_back(class_id);
        }

        // NMS
        std::vector<int> indices;
        cv::dnn::NMSBoxes(boxes, confidences, SCORE_THRESHOLD, NMS_THRESHOLD, indices);
        for (int idx : indices) {
            if (idx >= 0 && idx < static_cast<int>(boxes.size())) {
                detections.push_back(boxes[idx]);
            }
        }

        return detections;
    }
    
    void detect_faces(const cv::Mat& frame, std::vector<cv::Rect>& detections, 
                     std::vector<float>& confidences, std::vector<int>& class_ids) {
        if (!m_model_loaded) {
            std::cerr << "Model not loaded" << std::endl;
            return;
        }
        
        // Clear previous detections
        detections.clear();
        confidences.clear();
        class_ids.clear();
        
        try {
            // Preprocess image
            cv::Mat blob = preprocess_image(frame);
            
            // Prepare input tensor
            std::vector<float> input_tensor(blob.begin<float>(), blob.end<float>());
            
            // Define input shape
            std::vector<int64_t> input_shape = {1, 3, INPUT_HEIGHT, INPUT_WIDTH};
            
            // Create input tensor
            auto memory_info = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);
            Ort::Value input_tensor_ort = Ort::Value::CreateTensor<float>(
                memory_info, input_tensor.data(), input_tensor.size(),
                input_shape.data(), input_shape.size());
            
            // YOLOv8 standard input/output names
            const char* input_names[] = {"images"};
            const char* output_names[] = {"output0"};
            
            // Run inference
            std::vector<Ort::Value> output_tensors = m_session.Run(
                Ort::RunOptions{nullptr},
                input_names,
                &input_tensor_ort,
                1,
                output_names,
                1);

            if (output_tensors.empty()) {
                std::cerr << "ONNX Runtime returned no outputs" << std::endl;
                return;
            }
            if (!output_tensors[0].IsTensor()) {
                std::cerr << "ONNX Runtime output[0] is not a tensor" << std::endl;
                return;
            }

            // Get output data
            float* output_data = nullptr;
            try {
                output_data = output_tensors[0].GetTensorMutableData<float>();
            } catch (const Ort::Exception& e) {
                std::cerr << "Failed to access tensor data: " << e.what() << std::endl;
                return;
            }
            auto output_type_info = output_tensors[0].GetTensorTypeAndShapeInfo();
            auto output_shape = output_type_info.GetShape();
            size_t output_size = output_type_info.GetElementCount();
            if (output_size == 0 || output_data == nullptr) {
                std::cerr << "ONNX Runtime output tensor is empty" << std::endl;
                return;
            }

            std::vector<float> output_vector(output_data, output_data + output_size);
            
            // Postprocess to get detections
            std::vector<cv::Rect> face_detections = postprocess_output_dynamic(frame, output_data, output_shape);
            
            // Debug output
            std::cout << "Output shape: [";
            for (size_t i = 0; i < output_shape.size(); ++i) {
                std::cout << output_shape[i];
                if (i < output_shape.size() - 1) std::cout << ", ";
            }
            std::cout << "], size: " << output_size << std::endl;
            std::cout << "Detected " << face_detections.size() << " faces" << std::endl;
            
            // Convert to our format
            for (const auto& detection : face_detections) {
                detections.push_back(detection);
                confidences.push_back(0.9f); // Default confidence for face detection
                class_ids.push_back(0); // Face class
                std::cout << "Face detected at: (" << detection.x << ", " << detection.y 
                         << ", " << detection.width << "x" << detection.height << ")" << std::endl;
            }
            
        } catch (const Ort::Exception& e) {
            std::cerr << "Inference error: " << e.what() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error during face detection: " << e.what() << std::endl;
        }
    }
    
    void start_webcam() {
        if (m_running.exchange(true)) {
            std::cout << "Webcam already running" << std::endl;
            return;
        }
        
        send_message(IPC_MSG_STATUS, "Starting webcam...");
        
        // Load the YOLOv8 face detection model
        if (!load_model()) {
            send_message(IPC_MSG_ERROR, "Failed to load YOLOv8 face detection model");
            std::cerr << "Failed to load model, falling back to simulation mode" << std::endl;
            run_simulation();
            return;
        }
        
        send_message(IPC_MSG_STATUS, "YOLOv8 face detection model loaded successfully");
        
        // Try different camera indices
        int camera_indices[] = {0, 2, 3, 1};
        cv::VideoCapture cap;
        bool camera_found = false;
        
        for (int idx : camera_indices) {
            send_message(IPC_MSG_STATUS, "Trying camera index: " + std::to_string(idx));
            
            cap.open(idx);
            if (cap.isOpened()) {
                // Set video capture resolution to 640x480
                cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
                cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
                
                // Verify the resolution was set correctly
                double actual_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
                double actual_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
                
                send_message(IPC_MSG_STATUS, "Using camera index: " + std::to_string(idx) + 
                           " at resolution: " + std::to_string(static_cast<int>(actual_width)) + 
                           "x" + std::to_string(static_cast<int>(actual_height)));
                camera_found = true;
                break;
            }
            cap.release();
        }
        
        if (!camera_found) {
            send_message(IPC_MSG_STATUS, "No camera found. Running in simulation mode.");
            run_simulation();
            return;
        }
        
        // Create OpenCV window
        cv::namedWindow("Webcam Detection", cv::WINDOW_AUTOSIZE);
        cv::setWindowTitle("Webcam Detection", "Webcam with Detection Results (640x480)");
        
        // Main webcam loop
        cv::Mat frame;
        int frame_count = 0;
        auto start_time = std::chrono::steady_clock::now();
        
        // Persistent detection storage
        std::vector<cv::Rect> current_detections;
        std::vector<float> current_confidences;
        std::vector<int> current_class_ids;
        
        send_message(IPC_MSG_STATUS, "Webcam started successfully");
        
        // Send camera info
        std::string camera_info = "Camera Info: " + std::to_string(static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH))) + 
                                "x" + std::to_string(static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT))) + 
                                " at " + std::to_string(static_cast<int>(cap.get(cv::CAP_PROP_FPS))) + " FPS";
        send_message(IPC_MSG_STATUS, camera_info);
        
        // Send initial detection state
        send_message(IPC_MSG_DETECTION, "Faces: 0");
        
        while (m_running.load()) {
            cap >> frame;
            if (frame.empty()) {
                send_message(IPC_MSG_ERROR, "Failed to capture frame");
                break;
            }
            
            frame_count++;
            auto current_time = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time);
            float fps = frame_count * 1000.0f / elapsed.count();
            
            // Create a copy of the frame for drawing
            cv::Mat display_frame = frame.clone();
            
            // Perform real face detection every 5 frames for real-time visual feedback
            if (frame_count % 5 == 0) {
                detect_faces(frame, current_detections, current_confidences, current_class_ids);
                
                // Check for detection count changes and send to GUI
                check_and_send_detection_changes(current_detections);
            }
            
            // Draw current detections on the frame
            draw_detections(display_frame, current_detections, current_confidences, current_class_ids);
            
            // Add FPS and frame count text
            std::string fps_text = "FPS: " + std::to_string(static_cast<int>(fps));
            std::string frame_text = "Frame: " + std::to_string(frame_count);
            std::string detection_text = "Detections: " + std::to_string(current_detections.size());
            
            cv::putText(display_frame, fps_text, cv::Point(10, 30), 
                       cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
            cv::putText(display_frame, frame_text, cv::Point(10, 60), 
                       cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
            cv::putText(display_frame, detection_text, cv::Point(10, 90), 
                       cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
            
            // Display the frame
            cv::imshow("Webcam Detection", display_frame);
            
            // Send frame processed message only every 30 frames (about once per second)
            if (frame_count % 30 == 0) {
                std::string frame_data = "Frame: " + std::to_string(frame_count) + 
                                       ", FPS: " + std::to_string(static_cast<int>(fps)) + 
                                       ", Time: " + std::to_string(elapsed.count() / 1000.0f) + "s";
                send_message(IPC_MSG_FRAME_PROCESSED, frame_data);
            }
            
            // Handle key presses
            int key = cv::waitKey(1) & 0xFF;
            if (key == 27) { // ESC key
                std::cout << "ESC pressed, stopping webcam" << std::endl;
                break;
            } else if (key == 'q') {
                std::cout << "Q pressed, stopping webcam" << std::endl;
                break;
            }
            
            // Limit frame rate
            std::this_thread::sleep_for(std::chrono::milliseconds(33)); // ~30 FPS
        }
        
        cap.release();
        cv::destroyAllWindows();
        send_message(IPC_MSG_STATUS, "Webcam stopped");
        m_running.store(false);
    }
    
    void stop_webcam() {
        m_running.store(false);
    }
    
    bool is_running() const {
        return m_running.load();
    }
    
    void check_and_send_detection_changes(const std::vector<cv::Rect>& current_detections) {
        int new_count = static_cast<int>(current_detections.size());
        
        // Check if detection count has changed
        if (new_count != m_current_detection_count) {
            std::cout << "Detection count changed: " << m_current_detection_count << " -> " << new_count << std::endl;
            
            // Create detection message with coordinates
            std::string detection_msg = "Faces: " + std::to_string(new_count);
            
            // Add coordinates for each detection
            for (size_t i = 0; i < current_detections.size(); ++i) {
                const cv::Rect& rect = current_detections[i];
                detection_msg += " | Face" + std::to_string(i + 1) + ": (" + 
                               std::to_string(rect.x) + "," + std::to_string(rect.y) + "," + 
                               std::to_string(rect.width) + "x" + std::to_string(rect.height) + ")";
            }
            
            // Send detection message to GUI
            send_message(IPC_MSG_DETECTION, detection_msg);
            
            // Update tracking variables
            m_current_detection_count = new_count;
            m_last_detections = current_detections;
        }
    }
    
private:
    void run_simulation() {
        int frame_count = 0;
        
        // Create a simulation window
        cv::namedWindow("Simulation Mode", cv::WINDOW_AUTOSIZE);
        cv::setWindowTitle("Simulation Mode", "No Camera Available - Simulation (640x480)");
        
        // Persistent detection storage for simulation
        std::vector<cv::Rect> sim_detections;
        std::vector<float> sim_confidences;
        std::vector<int> sim_class_ids;
        
        while (m_running.load()) {
            // Create a simulated frame at 640x480 resolution
            cv::Mat sim_frame = cv::Mat::zeros(480, 640, CV_8UC3);
            
            frame_count++;
            
            // Try face detection if model is loaded, otherwise show simulation
            if (frame_count % 5 == 0) {
                if (m_model_loaded) {
                    detect_faces(sim_frame, sim_detections, sim_confidences, sim_class_ids);
                } else {
                    // Simple simulation for when model is not available
                    sim_detections.clear();
                    sim_confidences.clear();
                    sim_class_ids.clear();
                    
                    // Add a simple face-like rectangle
                    sim_detections.push_back(cv::Rect(200, 150, 100, 100));
                    sim_confidences.push_back(0.85f);
                    sim_class_ids.push_back(0);
                }
                
                // Check for detection count changes and send to GUI
                check_and_send_detection_changes(sim_detections);
            }
            
            // Draw detections on simulation frame
            draw_detections(sim_frame, sim_detections, sim_confidences, sim_class_ids);
            
            // Add simulation text
            std::string sim_text = "Simulation Mode - No Camera";
            std::string frame_text = "Frame: " + std::to_string(frame_count);
            std::string time_text = "Time: " + std::to_string(frame_count / 10) + "s";
            std::string res_text = "Resolution: 640x480";
            std::string detection_text = "Detections: " + std::to_string(sim_detections.size());
            
            cv::putText(sim_frame, sim_text, cv::Point(150, 200), 
                       cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(255, 255, 255), 2);
            cv::putText(sim_frame, frame_text, cv::Point(150, 250), 
                       cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 255, 255), 2);
            cv::putText(sim_frame, time_text, cv::Point(150, 300), 
                       cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 255, 255), 2);
            cv::putText(sim_frame, res_text, cv::Point(150, 350), 
                       cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 255, 255), 2);
            cv::putText(sim_frame, detection_text, cv::Point(150, 400), 
                       cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(255, 255, 255), 2);
            
            // Display the simulation frame
            cv::imshow("Simulation Mode", sim_frame);
            
            if (frame_count % 30 == 0) { // Every 3 seconds
                send_message(IPC_MSG_STATUS, "Simulation: No camera available (640x480)");
                send_message(IPC_MSG_FRAME_PROCESSED, "Simulated frame: " + std::to_string(frame_count));
            }
            
            // Send initial detection state for simulation
            if (frame_count == 1) {
                send_message(IPC_MSG_DETECTION, "Faces: 0");
            }
            
            // Handle key presses
            int key = cv::waitKey(100) & 0xFF;
            if (key == 27 || key == 'q') { // ESC or Q key
                break;
            }
        }
        
        cv::destroyAllWindows();
    }
    

    
    void draw_detections(cv::Mat& frame, const std::vector<cv::Rect>& detections,
                        const std::vector<float>& confidences, const std::vector<int>& class_ids) {
        for (size_t i = 0; i < detections.size(); i++) {
            const cv::Rect& rect = detections[i];
            float conf = confidences[i];
            int class_id = class_ids[i];
            
            // Choose color based on class
            cv::Scalar color;
            std::string label;
            
            switch (class_id) {
                case 0: // face
                    color = cv::Scalar(0, 255, 0); // Green
                    label = "Face";
                    break;
                default:
                    color = cv::Scalar(0, 0, 255); // Red
                    label = "Unknown";
                    break;
            }
            
            // Draw rectangle
            cv::rectangle(frame, rect, color, 2);
            
            // Draw label with confidence
            std::string text = label + " " + std::to_string(static_cast<int>(conf * 100)) + "%";
            cv::putText(frame, text, cv::Point(rect.x, rect.y - 10), 
                       cv::FONT_HERSHEY_SIMPLEX, 0.5, color, 2);
        }
    }
};

static WebcamIPCApp* g_app = nullptr;

void signal_handler(int sig) {
    std::cout << "Received signal " << sig << ", shutting down..." << std::endl;
    if (g_app) {
        g_app->stop_webcam();
    }
}

int main(int argc, char* argv[]) {
    // Set up signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
    
    std::cout << "Starting Webcam IPC Application..." << std::endl;
    
    g_app = new WebcamIPCApp();
    
    if (!g_app->init()) {
        std::cerr << "Failed to initialize webcam IPC application" << std::endl;
        delete g_app;
        return -1;
    }
    
    // Parse command line arguments
    std::string model_path = "../models/yolov8_face_model.onnx";
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--model") == 0 && i + 1 < argc) {
            model_path = argv[i + 1];
            i++;
        }
    }
    
    g_app->set_model_path(model_path);
    std::cout << "Using model: " << model_path << std::endl;
    
    std::cout << "Webcam IPC application started." << std::endl;
    std::cout << "Press ESC or Q to stop the application." << std::endl;
    
    // Start webcam processing
    g_app->start_webcam();
    
    // Cleanup
    std::cout << "Shutting down Webcam IPC Application..." << std::endl;
    delete g_app;
    
    std::cout << "Webcam IPC Application stopped" << std::endl;
    return 0;
}
