#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    // Create a test image
    cv::Mat test_image = cv::Mat::zeros(480, 640, CV_8UC3);
    
    // Add various text in green color
    std::string test_text1 = "Test Text 1 - Should be Green";
    std::string test_text2 = "Test Text 2 - Should be Green";
    std::string test_text3 = "Test Text 3 - Should be Green";
    std::string test_text4 = "Test Text 4 - Should be Green";
    
    // Draw text in green color (BGR format: 0, 255, 0 = Green)
    cv::putText(test_image, test_text1, cv::Point(50, 100), 
               cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 255, 0), 2);
    cv::putText(test_image, test_text2, cv::Point(50, 150), 
               cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 255, 0), 2);
    cv::putText(test_image, test_text3, cv::Point(50, 200), 
               cv::FONT_HERSHEY_SIMPLEX, 0.7, cv::Scalar(0, 255, 0), 2);
    cv::putText(test_image, test_text4, cv::Point(50, 250), 
               cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 0), 2);
    
    // Add a green rectangle to show the color
    cv::rectangle(test_image, cv::Point(50, 300), cv::Point(250, 350), 
                 cv::Scalar(0, 255, 0), 3);
    
    // Display the image
    cv::namedWindow("Green Text Test", cv::WINDOW_AUTOSIZE);
    cv::imshow("Green Text Test", test_image);
    
    std::cout << "Green text test image created. Press any key to exit." << std::endl;
    cv::waitKey(0);
    
    return 0;
}
