// main.cpp

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>
#include <iostream>
#include "../headers/recog.hpp"

int main() {
    // Declare vectors to store all contours and valid contours
    std::vector<ContourWithData> allContoursWithData;
    std::vector<ContourWithData> validContoursWithData;

    // Read training classifications
    cv::Mat matClassificationInts;
    cv::FileStorage fsClassifications("xml/classifications.xml", cv::FileStorage::READ);
    if (!fsClassifications.isOpened()) {
        std::cout << "Error: Unable to open training classifications file.\n";
        return 0;
    }
    fsClassifications["classifications"] >> matClassificationInts;
    fsClassifications.release();

    // Read training images
    cv::Mat matTrainingImagesAsFlattenedFloats;
    cv::FileStorage fsTrainingImages("xml/images.xml", cv::FileStorage::READ);
    if (!fsTrainingImages.isOpened()) {
        std::cout << "Error: Unable to open training images file.\n";
        return 0;
    }
    fsTrainingImages["images"] >> matTrainingImagesAsFlattenedFloats;
    fsTrainingImages.release();

    // Train KNN classifier
    cv::Ptr<cv::ml::KNearest>  kNearest(cv::ml::KNearest::create());
    kNearest->train(matTrainingImagesAsFlattenedFloats, cv::ml::ROW_SAMPLE, matClassificationInts);

    // Load test image
    cv::Mat matTestingNumbers = cv::imread("images/test5.png");
    if (matTestingNumbers.empty()) {
        std::cout << "Error: Image not read from file.\n";
        return 0;
    }

    // Preprocess test image
    cv::Mat matThresh;
    preprocessImage(matTestingNumbers, matThresh);

    // Recognize numbers in contours
    std::string strFinalString = recognizeNumbers(matThresh, validContoursWithData);

    // Print recognized numbers
    std::cout << "\n\nNumbers read = " << strFinalString << "\n\n";

    // Display test image with recognized numbers
    cv::imshow("matTestingNumbers", matTestingNumbers);
    cv::waitKey(0);
    return 0;
}
