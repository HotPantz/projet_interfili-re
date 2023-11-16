#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>

#include <iostream>
#include <sstream>

// global variables ///////////////////////////////////////////////////////////////////////////////
const int MIN_CONTOUR_AREA = 100;

const int RESIZED_IMAGE_WIDTH = 20;
const int RESIZED_IMAGE_HEIGHT = 30;

///////////////////////////////////////////////////////////////////////////////////////////////////
class ContourWithData {
public:
    std::vector<cv::Point> ptContour;      // Contour points
    cv::Rect boundingRect;                 // Bounding rectangle for contour
    float fltArea;                         // Area of contour

    bool checkIfContourIsValid() {
        if (fltArea < MIN_CONTOUR_AREA)
            return false;                  // Check if the contour area is valid
        return true;
    }

    static bool sortByBoundingRectXPosition(const ContourWithData& cwdLeft, const ContourWithData& cwdRight) {
        return (cwdLeft.boundingRect.x < cwdRight.boundingRect.x);   // Sorting function for contours by their x positions
    }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
int main() {
    std::vector<ContourWithData> allContoursWithData;      // Vector to store all detected contours
    std::vector<ContourWithData> validContoursWithData;    // Vector to store valid contours

    cv::Mat matClassificationInts;      // Matrix to hold classification numbers

    // Read training classifications file
    cv::FileStorage fsClassifications("classifications.xml", cv::FileStorage::READ);
    // ... (error handling for file opening)

    fsClassifications["classifications"] >> matClassificationInts; // Read classifications section into Mat variable
    fsClassifications.release();                                   // Close the classifications file

    cv::Mat matTrainingImagesAsFlattenedFloats;    // Matrix to hold training images as flattened floats

    // Read training images file
    cv::FileStorage fsTrainingImages("images.xml", cv::FileStorage::READ);
    // ... (error handling for file opening)

    fsTrainingImages["images"] >> matTrainingImagesAsFlattenedFloats; // Read images section into Mat variable
    fsTrainingImages.release();                                      // Close the training images file

    cv::Ptr<cv::ml::KNearest> kNearest(cv::ml::KNearest::create()); // Create KNN object

    kNearest->train(matTrainingImagesAsFlattenedFloats, cv::ml::ROW_SAMPLE, matClassificationInts); // Train the KNN model

    cv::Mat matTestingNumbers = cv::imread("test5.png"); // Read the test numbers image
    // ... (error handling for image loading)

    cv::Mat matGrayscale;
    cv::Mat matBlurred;
    cv::Mat matThresh;
    cv::Mat matThreshCopy;

    cv::cvtColor(matTestingNumbers, matGrayscale, cv::COLOR_BGR2GRAY); // Convert the test image to grayscale

    cv::GaussianBlur(matGrayscale, matBlurred, cv::Size(5, 5), 0); // Gaussian blur to smoothen the image

    cv::adaptiveThreshold(matBlurred, matThresh, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 11, 2); // Apply adaptive thresholding

    matThreshCopy = matThresh.clone(); // Make a copy of the thresholded image

    std::vector<std::vector<cv::Point>> ptContours; // Vector to hold detected contours
    std::vector<cv::Vec4i> v4iHierarchy;           // Vector for contour hierarchy

    cv::findContours(matThreshCopy, ptContours, v4iHierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE); // Find contours

    // Iterate through all contours and store relevant data
    for (int i = 0; i < ptContours.size(); i++) {
        ContourWithData contourWithData;
        contourWithData.ptContour = ptContours[i];
        contourWithData.boundingRect = cv::boundingRect(contourWithData.ptContour);
        contourWithData.fltArea = cv::contourArea(contourWithData.ptContour);
        allContoursWithData.push_back(contourWithData);
    }

    // Filter valid contours based on area
    for (int i = 0; i < allContoursWithData.size(); i++) {
        if (allContoursWithData[i].checkIfContourIsValid()) {
            validContoursWithData.push_back(allContoursWithData[i]);
        }
    }

    // Sort valid contours based on their bounding rectangle's x positions
    std::sort(validContoursWithData.begin(), validContoursWithData.end(), ContourWithData::sortByBoundingRectXPosition);

    std::string strFinalString; // Final string to hold recognized characters

    // Recognize characters from valid contours
    for (int i = 0; i < validContoursWithData.size(); i++) {
        cv::rectangle(matTestingNumbers, validContoursWithData[i].boundingRect, cv::Scalar(0, 255, 0), 2); // Draw a rectangle around the character
        // ... (Character recognition process)
        strFinalString = strFinalString + char(int(fltCurrentChar)); // Append recognized character to the final string
    }

    std::cout << "\n\n" << "numbers read = " << strFinalString << "\n\n"; // Display the recognized characters

    cv::imshow("matTestingNumbers", matTestingNumbers); // Display the input image with recognized characters
    cv::waitKey(0); // Wait for user key press

    return 0;
}
