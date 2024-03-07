// recog.cpp
#include "recog.h"
#include <iostream>

bool ContourWithData::checkIfContourIsValid() {
    return (fltArea >= MIN_CONTOUR_AREA);
}

bool ContourWithData::sortByBoundingRectXPosition(const ContourWithData& cwdLeft, const ContourWithData& cwdRight) {
    return (cwdLeft.boundingRect.x < cwdRight.boundingRect.x);
}

void preprocessImage(const cv::Mat& srcImage, cv::Mat& threshImage) {
    cv::Mat matGrayscale;
    cv::Mat matBlurred;
    
    cv::cvtColor(srcImage, matGrayscale, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(matGrayscale, matBlurred, cv::Size(5, 5), 0);
    cv::adaptiveThreshold(matBlurred, threshImage, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C,
                          cv::THRESH_BINARY_INV, 11, 2);
}

std::string recognizeNumbers(const cv::Mat& threshImage) {
    cv::Mat matThreshCopy = threshImage.clone();
    std::vector<ContourWithData> allContoursWithData;
    std::vector<ContourWithData> validContoursWithData;

    std::vector<std::vector<cv::Point>> ptContours;
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
        strFinalString += std::to_string(i); // Placeholder for actual recognition logic
    }

    return strFinalString;
}
