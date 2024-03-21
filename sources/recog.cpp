#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml/ml.hpp>
#include <iostream>
#include "/media/sf_Echange/projet_interfili-re/headers/recog.hpp"

int recog(void) {
    std::vector<ContourWithData> allContoursWithData;           // Declare vectors to store all contours and valid contours
    std::vector<ContourWithData> validContoursWithData;

    // Read in training classifications
    cv::Mat matClassificationInts;      // Matrix to store the classification numbers

    cv::FileStorage fsClassifications("/media/sf_Echange/projet_interfili-re/xml/classifications.xml", cv::FileStorage::READ);        // Open the classifications file

    if (fsClassifications.isOpened() == false) {                                                    // Check if the file was opened successfully
        std::cout << "error, unable to open training classifications file, exiting program\n\n";    // Display error message
        return(0);                                                                                  // Exit program
    }

    fsClassifications["classifications"] >> matClassificationInts;      // Read classifications section into Mat classification variable
    fsClassifications.release();                                        // Close the classifications file

    // Read in training images
    cv::Mat matTrainingImagesAsFlattenedFloats;         // Matrix to store multiple images

    cv::FileStorage fsTrainingImages("/media/sf_Echange/projet_interfili-re/xml/images.xml", cv::FileStorage::READ);          // Open the training images file

    if (fsTrainingImages.isOpened() == false) {                                                 // Check if the file was opened successfully
        std::cout << "error, unable to open training images file, exiting program\n\n";         // Display error message
        return(0);                                                                              // Exit program
    }

    fsTrainingImages["images"] >> matTrainingImagesAsFlattenedFloats;           // Read images section into Mat training images variable
    fsTrainingImages.release();                                                 // Close the training images file

    // Train the KNN model
    cv::Ptr<cv::ml::KNearest>  kNearest(cv::ml::KNearest::create());            // Instantiate the KNN object

    kNearest->train(matTrainingImagesAsFlattenedFloats, cv::ml::ROW_SAMPLE, matClassificationInts); // Train the model

    // Load test image
    cv::Mat matTestingNumbers = cv::imread("/media/sf_Echange/projet_interfili-re/images/panneau52.png");            // Read in the test numbers image

    if (matTestingNumbers.empty()) {                                // Check if unable to open image
        std::cout << "error: image not read from file\n\n";         // Display error message
        return(0);                                                  // Exit program
    }

    cv::Mat matGrayscale;           // Declare more image variables
    cv::Mat matBlurred;
    cv::Mat matThresh;
    cv::Mat matThreshCopy;

    cv::cvtColor(matTestingNumbers, matGrayscale, cv::COLOR_BGR2GRAY);         // Convert to grayscale

    cv::GaussianBlur(matGrayscale, matBlurred, cv::Size(5, 5), 0);                        // Blur the image

    cv::adaptiveThreshold(matBlurred, matThresh, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 11, 2); // Filter image from grayscale to black and white

    matThreshCopy = matThresh.clone();              // Make a copy of the thresh image

    std::vector<std::vector<cv::Point> > ptContours;        // Declare a vector for the contours
    std::vector<cv::Vec4i> v4iHierarchy;                    // Declare a vector for the hierarchy (we won't use this in this program but this may be helpful for reference)

    cv::findContours(matThreshCopy, ptContours, v4iHierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE); // Find contours

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

    std::string strFinalString;         // Declare final string, this will have the final number sequence by the end of the program

    // Recognize characters from valid contours
    for (int i = 0; i < validContoursWithData.size(); i++) {
        cv::rectangle(matTestingNumbers, validContoursWithData[i].boundingRect, cv::Scalar(0, 255, 0), 2); // Draw a green rectangle around the character

        cv::Mat matROI = matThresh(validContoursWithData[i].boundingRect);          // Get ROI image of bounding rect

        cv::Mat matROIResized;
        cv::resize(matROI, matROIResized, cv::Size(RESIZED_IMAGE_WIDTH, RESIZED_IMAGE_HEIGHT));     // Resize image, this will be more consistent for recognition and storage

        cv::Mat matROIFloat;
        matROIResized.convertTo(matROIFloat, CV_32FC1);             // Convert Mat to float, necessary for call to find_nearest

        cv::Mat matROIFlattenedFloat = matROIFloat.reshape(1, 1);

        cv::Mat matCurrentChar(0, 0, CV_32F);

        kNearest->findNearest(matROIFlattenedFloat, 1, matCurrentChar);     // Finally, we can call find_nearest !!!

        float fltCurrentChar = (float)matCurrentChar.at<float>(0, 0);

        strFinalString = strFinalString + char(int(fltCurrentChar));        // Append current char to full string
    }

    std::cout << "\n\n" << "numbers read = " << strFinalString << "\n\n";       // Show the full string

    cv::imshow("matTestingNumbers", matTestingNumbers);     // Show input image with green boxes drawn around found digits

    cv::waitKey(0);                                        // Wait for user key press

    return(0);
}
