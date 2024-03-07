#include <opencv2/opencv.hpp>
#include <iostream>
#include <utility>
#include "../headers/thresholding.h"

using namespace cv;
using namespace std;

void brightnessChangeCallback(int value, void* data){
			
	//cast to mat
	//Mat* image = static_cast<Mat*>(data[0]);
	pair<string, Mat*>* inputData = static_cast<pair<string, Mat*>*>(data);

	//Adjust the brightness of the image based on the trackbar value
    	Mat adjustedImage;
    	inputData->second->convertTo(adjustedImage, -1, 1.0 + (value / 100.0));

    	imshow(inputData->first, adjustedImage); //update the image in the window specified in the first pair value	
	
	return;
}

Mat isolateColor(Mat* inputImg, Scalar lowHSV, Scalar upHSV){
	//convert input image from BGR to HSV color space
    Mat hsvImg;
    cvtColor(*inputImg, hsvImg, COLOR_BGR2HSV);

    //binary mask to filter out pixels within the specified HSV range
    Mat mask;
    inRange(hsvImg, lowHSV, upHSV, mask);

    //masked image by bitwise AND operation
    Mat maskedImg;
    bitwise_and(*inputImg, *inputImg, maskedImg, mask);

    return maskedImg;
}	

Mat getNthPart(Mat* inputImage, int divNum, int n, int width){
    // choosing the start X coordinate and the width in order
    // to not choose the whole width of the image

	 // Check if the input image is empty
    if(inputImage->empty()){
        cerr << "Image d'entrée vide" << endl;
        return Mat();
    }
    
    int rows = inputImage->rows;
    int cols = inputImage->cols;

    //Calculate the starting row and column indices for the nth part
    int startRow = (rows / divNum) * n;
    int partHeight = rows/divNum;

    //region of interest for the nth part of the image
    //Rect roi(startX, rows/divNum, cols, rows/divNum);
    int startCol = (cols/2) - (width/2); //middle of the image adjusted by the desired width (so that we have equal widths either side)
    cout << "Columns: " << cols << " adjusted middle : " << startCol << endl;
    Rect roi(startCol, startRow, width, 100);

    // Return the nth part of the input image
    return (*inputImage)(roi).clone();
}

float findMeanLineY(Mat* img, int lineY){
	// Ensure that the lineY coordinate is within the image bounds
    if (lineY < 0 || lineY >= img->rows) {
        cerr << "Position Y souhaitée de la ligne invalide" << endl;
        return -1;
    }

    // Extract the specified line from the image
    Mat line = (*img).row(lineY);
   	vector<Point> nonzeroPoints;
	
	cvtColor((*img).row(lineY), line, COLOR_BGR2GRAY); //findNonZero attend une image binaire	
	threshold(line, line, 0, 255, THRESH_BINARY);

    //debug 
    namedWindow("NonZero Image", WINDOW_AUTOSIZE);
	imshow("NonZero Image", line);

    findNonZero(line, nonzeroPoints); //on cherche les elements non 0 (non-noir) sur l'image masquée
    if(nonzeroPoints.empty()){ //if no non-zero points are found, we'll just go straight
        cerr << "Pas de pixels trouvés dans la zone de l'image coupée!" << endl;
        return img->rows / 2;
    }
    int sumX = 0, count = nonzeroPoints.size();
    cout << count << endl;

	Point pnt;
    for(int i = 0; i < count; i++ ){
        pnt = nonzeroPoints[i];
		sumX += pnt.x;
        cout << pnt.x << endl;
    }

    return sumX / count;
}