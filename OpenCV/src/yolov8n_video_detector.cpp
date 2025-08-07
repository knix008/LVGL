// Ultralytics 🚀 AGPL-3.0 License - https://ultralytics.com/license

#include "yolov8n_video_detector.h"
#include <iostream>
#include <iomanip>

YOLOv8nVideoDetector::YOLOv8nVideoDetector(const std::string &onnxModelPath, 
                                           const cv::Size &modelInputShape, 
                                           const std::string &classesTxtFile, 
                                           const bool &runWithCuda)
    : detector(onnxModelPath, modelInputShape, classesTxtFile, runWithCuda),
      currentFPS(0.0),
      totalProcessingTime(0.0),
      totalFramesProcessed(0),
      videoWriterInitialized(false)
{
    lastFrameTime = std::chrono::high_resolution_clock::now();
}

bool YOLOv8nVideoDetector::processVideoFile(const std::string &videoPath, 
                                           const std::string &outputPath, 
                                           bool showFPS, 
                                           bool saveOutput)
{
    cv::VideoCapture cap(videoPath);
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open video file: " << videoPath << std::endl;
        return false;
    }

    // Get video properties
    int frameWidth = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int frameHeight = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));
    double fps = cap.get(cv::CAP_PROP_FPS);
    int totalFrames = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_COUNT));

    std::cout << "Video properties:" << std::endl;
    std::cout << "  Resolution: " << frameWidth << "x" << frameHeight << std::endl;
    std::cout << "  FPS: " << fps << std::endl;
    std::cout << "  Total frames: " << totalFrames << std::endl;

    // Initialize video writer if saving output
    if (saveOutput && !outputPath.empty()) {
        if (!initializeVideoWriter(outputPath, cv::Size(frameWidth, frameHeight))) {
            std::cerr << "Error: Could not initialize video writer" << std::endl;
            return false;
        }
    }

    cv::Mat frame;
    int frameCount = 0;
    auto startTime = std::chrono::high_resolution_clock::now();

    while (cap.read(frame)) {
        frameCount++;
        
        // Process frame
        VideoDetectionResult result = processFrame(frame);
        
        // Draw detections
        cv::Mat outputFrame = drawDetections(frame, result.detections, showFPS);
        
        // Save frame if requested
        if (saveOutput && videoWriterInitialized) {
            videoWriter.write(outputFrame);
        }
        
        // Display progress
        if (frameCount % 30 == 0) {
            double progress = (double)frameCount / totalFrames * 100.0;
            std::cout << "Progress: " << std::fixed << std::setprecision(1) 
                      << progress << "% (" << frameCount << "/" << totalFrames 
                      << " frames)" << std::endl;
        }
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    
    cap.release();
    if (videoWriterInitialized) {
        videoWriter.release();
        videoWriterInitialized = false;
    }

    std::cout << "\nVideo processing completed!" << std::endl;
    std::cout << "Total frames processed: " << totalFramesProcessed << std::endl;
    std::cout << "Total processing time: " << duration.count() / 1000.0 << " seconds" << std::endl;
    std::cout << "Average FPS: " << totalFramesProcessed / (duration.count() / 1000.0) << std::endl;
    
    if (saveOutput && !outputPath.empty()) {
        std::cout << "Output saved to: " << outputPath << std::endl;
    }

    return true;
}

bool YOLOv8nVideoDetector::processWebcam(int cameraIndex, 
                                        bool showFPS, 
                                        bool saveOutput, 
                                        const std::string &outputPath)
{
    cv::VideoCapture cap(cameraIndex);
    if (!cap.isOpened()) {
        std::cerr << "Error: Could not open camera with index: " << cameraIndex << std::endl;
        return false;
    }

    // Set camera properties for better performance
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480);
    cap.set(cv::CAP_PROP_FPS, 30);

    int frameWidth = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_WIDTH));
    int frameHeight = static_cast<int>(cap.get(cv::CAP_PROP_FRAME_HEIGHT));

    std::cout << "Webcam properties:" << std::endl;
    std::cout << "  Resolution: " << frameWidth << "x" << frameHeight << std::endl;
    std::cout << "  Press 'q' to quit, 's' to save current frame" << std::endl;

    // Initialize video writer if saving output
    if (saveOutput && !outputPath.empty()) {
        if (!initializeVideoWriter(outputPath, cv::Size(frameWidth, frameHeight))) {
            std::cerr << "Error: Could not initialize video writer" << std::endl;
            return false;
        }
    }

    cv::Mat frame;
    int frameCount = 0;

    while (true) {
        if (!cap.read(frame)) {
            std::cerr << "Error: Could not read frame from camera" << std::endl;
            break;
        }

        frameCount++;
        
        // Process frame
        VideoDetectionResult result = processFrame(frame);
        
        // Draw detections
        cv::Mat outputFrame = drawDetections(frame, result.detections, showFPS);
        
        // Save frame if requested
        if (saveOutput && videoWriterInitialized) {
            videoWriter.write(outputFrame);
        }
        
        // Display frame
        cv::imshow("YOLOv8n Webcam Detection", outputFrame);
        
        // Handle key presses
        char key = cv::waitKey(1) & 0xFF;
        if (key == 'q' || key == 27) { // 'q' or ESC
            break;
        } else if (key == 's') {
            // Save current frame
            std::string filename = "webcam_frame_" + std::to_string(frameCount) + ".jpg";
            cv::imwrite(filename, outputFrame);
            std::cout << "Frame saved as: " << filename << std::endl;
        }
    }

    cap.release();
    if (videoWriterInitialized) {
        videoWriter.release();
        videoWriterInitialized = false;
    }
    cv::destroyAllWindows();

    std::cout << "\nWebcam processing completed!" << std::endl;
    std::cout << "Total frames processed: " << totalFramesProcessed << std::endl;
    std::cout << "Total processing time: " << totalProcessingTime << " seconds" << std::endl;
    
    if (saveOutput && !outputPath.empty()) {
        std::cout << "Output saved to: " << outputPath << std::endl;
    }

    return true;
}

VideoDetectionResult YOLOv8nVideoDetector::processFrame(const cv::Mat &frame)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run inference
    std::vector<Detection> detections = detector.runInference(frame);
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    double processingTime = duration.count() / 1000000.0;
    
    // Update statistics
    totalFramesProcessed++;
    totalProcessingTime += processingTime;
    updateFPS();
    
    VideoDetectionResult result;
    result.detections = detections;
    result.fps = currentFPS;
    result.frameCount = totalFramesProcessed;
    result.processingTime = processingTime;
    
    return result;
}

cv::Mat YOLOv8nVideoDetector::drawDetections(const cv::Mat &frame, 
                                            const std::vector<Detection> &detections, 
                                            bool showFPS)
{
    cv::Mat outputFrame = frame.clone();
    
    // Draw detections
    for (const auto &detection : detections) {
        cv::Rect box = detection.box;
        cv::Scalar color = detection.color;
        
        // Detection box
        cv::rectangle(outputFrame, box, color, 2);
        
        // Detection box text
        std::string classString = detection.className + ' ' + 
                                 std::to_string(detection.confidence).substr(0, 4);
        cv::Size textSize = cv::getTextSize(classString, cv::FONT_HERSHEY_DUPLEX, 0.6, 1, 0);
        cv::Rect textBox(box.x, box.y - 25, textSize.width + 10, textSize.height + 10);
        
        cv::rectangle(outputFrame, textBox, color, cv::FILLED);
        cv::putText(outputFrame, classString, cv::Point(box.x + 5, box.y - 5), 
                   cv::FONT_HERSHEY_DUPLEX, 0.6, cv::Scalar(0, 0, 0), 1, 0);
    }
    
    // Draw FPS if requested
    if (showFPS) {
        std::string fpsText = "FPS: " + std::to_string(static_cast<int>(currentFPS));
        cv::putText(outputFrame, fpsText, cv::Point(10, 30), 
                   cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
        
        std::string detectionsText = "Detections: " + std::to_string(detections.size());
        cv::putText(outputFrame, detectionsText, cv::Point(10, 60), 
                   cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
    }
    
    return outputFrame;
}

void YOLOv8nVideoDetector::updateFPS()
{
    auto currentTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastFrameTime);
    
    if (duration.count() > 0) {
        currentFPS = 1000.0 / duration.count();
    }
    
    lastFrameTime = currentTime;
}

bool YOLOv8nVideoDetector::initializeVideoWriter(const std::string &outputPath, const cv::Size &frameSize)
{
    int fourcc = cv::VideoWriter::fourcc('m', 'p', '4', 'v'); // MP4V codec
    double fps = 30.0; // Default FPS
    
    videoWriter.open(outputPath, fourcc, fps, frameSize, true);
    if (!videoWriter.isOpened()) {
        // Try different codec if MP4V fails
        fourcc = cv::VideoWriter::fourcc('X', 'V', 'I', 'D'); // XVID codec
        videoWriter.open(outputPath, fourcc, fps, frameSize, true);
        if (!videoWriter.isOpened()) {
            std::cerr << "Error: Could not open video writer with any codec" << std::endl;
            return false;
        }
    }
    
    videoWriterInitialized = true;
    std::cout << "Video writer initialized: " << outputPath << std::endl;
    return true;
}
