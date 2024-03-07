// main.cpp
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include "recog.h"

int main() {
    // Load test image
    cv::Mat matTestingNumbers = cv::imread("test5.png");
    if (matTestingNumbers.empty()) {
        std::cout << "Error: Image not read from file.\n";
        return 0;
    }

    // Preprocess test image
    cv::Mat matThresh;
    preprocessImage(matTestingNumbers, matThresh);

    // Find and recognize numbers in contours
    std::string recognizedNumbers = recognizeNumbers(matThresh);

    // Print recognized numbers
    std::cout << "\n\nNumbers read = " << recognizedNumbers << "\n\n";

    // Display test image with recognized numbers
    cv::imshow("matTestingNumbers", matTestingNumbers);
    cv::waitKey(0);
    return 0;
}
