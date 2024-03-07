/*
This program reads a set of training images and their corresponding classifications from files,
trains a k-nearest neighbors (KNN) classifier using the training data, and then uses the trained
classifier to recognize numbers in a test image. It extracts contours from the test image,
filters and preprocesses them, and then uses the KNN classifier to predict the numbers represented
by the contours. The recognized numbers are printed to the console, and the original test image
with green rectangles around the recognized numbers is displayed.
*/

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>
#include "../headers/recog.hpp"
#include <iostream>
#include <sstream>

// Constants
const float MIN_CONTOUR_AREA = 100.0;  // Minimum area for a contour to be considered valid
const int RESIZED_IMAGE_WIDTH = 20;    // Width of the resized contour image
const int RESIZED_IMAGE_HEIGHT = 30;   // Height of the resized contour image

// Class to store contour data
class ContourWithData {
public:
    std::vector<cv::Point> ptContour;   // Contour points
    cv::Rect boundingRect;              // Bounding rectangle of the contour
    float fltArea;                     // Area of the contour

    // Check if the contour is valid based on its area
    bool checkIfContourIsValid() {
        return (fltArea >= MIN_CONTOUR_AREA);
    }

    // Static function to sort contours by their bounding rectangle's x position
    static bool sortByBoundingRectXPosition(const ContourWithData& cwdLeft, const ContourWithData& cwdRight) {
        return (cwdLeft.boundingRect.x < cwdRight.boundingRect.x);
    }
};

int main() {
    // Declare vectors to store all contours and valid contours
    std::vector<ContourWithData> allContoursWithData;
    std::vector<ContourWithData> validContoursWithData;

    // Read training classifications
    cv::Mat matClassificationInts;
    cv::FileStorage fsClassifications("classifications.xml", cv::FileStorage::READ);
    if (!fsClassifications.isOpened()) {
        std::cout << "Error: Unable to open training classifications file.\n";
        return 0;
    }
    fsClassifications["classifications"] >> matClassificationInts;
    fsClassifications.release();

    // Read training images
    cv::Mat matTrainingImagesAsFlattenedFloats;
    cv::FileStorage fsTrainingImages("images.xml", cv::FileStorage::READ);
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
    cv::Mat matTestingNumbers = cv::imread("test5.png");
    if (matTestingNumbers.empty()) {
        std::cout << "Error: Image not read from file.\n";
        return 0;
    }

    // Preprocess test image
    cv::Mat matGrayscale;
    cv::Mat matBlurred;
    cv::Mat matThresh;
    cv::Mat matThreshCopy;

    cv::cvtColor(matTestingNumbers, matGrayscale, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(matGrayscale, matBlurred, cv::Size(5, 5), 0);
    cv::adaptiveThreshold(matBlurred, matThresh, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C,
                          cv::THRESH_BINARY_INV, 11, 2);
    matThreshCopy = matThresh.clone();

    // Find contours in preprocessed image
    std::vector<std::vector<cv::Point> > ptContours;
    std::vector<cv::Vec4i> v4iHierarchy;
    cv::findContours(matThreshCopy, ptContours, v4iHierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Store contour data
    for (int i = 0; i < ptContours.size(); i++) {
        ContourWithData contourWithData;
        contourWithData.ptContour = ptContours[i];
        contourWithData.boundingRect = cv::boundingRect(contourWithData.ptContour);
        contourWithData.fltArea = cv::contourArea(contourWithData.ptContour);
        allContoursWithData.push_back(contourWithData);
    }

    // Filter valid contours
    for (int i = 0; i < allContoursWithData.size(); i++) {
        if (allContoursWithData[i].checkIfContourIsValid()) {
            validContoursWithData.push_back(allContoursWithData[i]);
        }
    }

    // Sort contours from left to right
    std::sort(validContoursWithData.begin(), validContoursWithData.end(), ContourWithData::sortByBoundingRectXPosition);

    // Recognize numbers in contours
    std::string strFinalString;
    for (int i = 0; i < validContoursWithData.size(); i++) {
        cv::rectangle(matTestingNumbers, validContoursWithData[i].boundingRect, cv::Scalar(0, 255, 0), 2);
        cv::Mat matROI = matThresh(validContoursWithData[i].boundingRect);
        cv::Mat matROIResized;
        cv::resize(matROI, matROIResized, cv::Size(RESIZED_IMAGE_WIDTH, RESIZED_IMAGE_HEIGHT));
        cv::Mat matROIFloat;
        matROIResized.convertTo(matROIFloat, CV_32FC1);
        cv::Mat matROIFlattenedFloat = matROIFloat.reshape(1, 1);
        cv::Mat matCurrentChar(0, 0, CV_32F);
        kNearest->findNearest(matROIFlattenedFloat, 1, matCurrentChar);
        float fltCurrentChar = (float)matCurrentChar.at<float>(0, 0);
        strFinalString = strFinalString + char(int(fltCurrentChar));
    }

    // Print recognized numbers
    std::cout << "\n\nNumbers read = " << strFinalString << "\n\n";

    // Display test image with recognized numbers
    cv::imshow("matTestingNumbers", matTestingNumbers);
    cv::waitKey(0);
    return 0;
}
