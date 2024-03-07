// recog.h
#ifndef RECOG_H
#define RECOG_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>

// Constants
const float MIN_CONTOUR_AREA = 100.0;
const int RESIZED_IMAGE_WIDTH = 20;
const int RESIZED_IMAGE_HEIGHT = 30;

// Class to store contour data
class ContourWithData {
public:
    std::vector<cv::Point> ptContour;
    cv::Rect boundingRect;
    float fltArea;

    bool checkIfContourIsValid();
    static bool sortByBoundingRectXPosition(const ContourWithData& cwdLeft, const ContourWithData& cwdRight);
};

// Function to preprocess image
void preprocessImage(const cv::Mat& srcImage, cv::Mat& threshImage);

// Function to recognize numbers in contours
std::string recognizeNumbers(const cv::Mat& threshImage);

#endif // RECOG_H
