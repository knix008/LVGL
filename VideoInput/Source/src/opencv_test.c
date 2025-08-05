#include <stdio.h>
#include <opencv4/opencv2/opencv.hpp>

extern "C" void opencv_test_function(void) {
    printf("OpenCV Test: OpenCV version %s\n", CV_VERSION);
    
    // Create a simple test image
    cv::Mat test_image = cv::Mat::zeros(100, 100, CV_8UC3);
    cv::circle(test_image, cv::Point(50, 50), 30, cv::Scalar(0, 255, 0), -1);
    
    printf("OpenCV Test: Created test image successfully\n");
    printf("OpenCV Test: Image size: %dx%d\n", test_image.cols, test_image.rows);
    printf("OpenCV Test: Image channels: %d\n", test_image.channels());
    
    // Test basic OpenCV functions
    cv::Mat gray_image;
    cv::cvtColor(test_image, gray_image, cv::COLOR_BGR2GRAY);
    printf("OpenCV Test: Converted to grayscale successfully\n");
    
    printf("OpenCV Test: All OpenCV functions working correctly\n");
} 