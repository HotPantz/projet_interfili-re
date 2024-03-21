#include <opencv2/opencv.hpp>
#include <iostream>
#include <utility>
#include <cmath>
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
    //cout << "Columns: " << cols << " adjusted middle : " << startCol << endl; //DEBUG
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
    //namedWindow("NonZero Image", WINDOW_AUTOSIZE);
	//imshow("NonZero Image", line);

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

float findMiddle(Mat* img, int lineY){
    // Ensure that the lineY coordinate is within the image bounds
    if (lineY < 0 || lineY >= img->rows) {
        cerr << "Position Y souhaitée de la ligne invalide" << endl;
        return -1;
    }

    // Extract the specified line from the image
    Mat line = (*img).row(lineY);
   	vector<Point> nonzeroPointsLeft, nonzeroPointsRight;

    //debug 
    //namedWindow("NonZero Image", WINDOW_AUTOSIZE);
	//imshow("NonZero Image", line);

    int lineColsHalf = line.cols / 2, leftMean = 0, rightMean = 0, nonZeroPtsCount, sumL = 0, sumR = 0;
    Mat lineLeftPart = line(Rect(0, 0, lineColsHalf, line.rows));
    Mat lineRightPart = line(Rect(lineColsHalf, 0, lineColsHalf, line.rows));
    Point pnt;

    //Left line mean position
    findNonZero(lineLeftPart, nonzeroPointsLeft); //on cherche les elements non 0 (non-noir) sur l'image masquée
    if(nonzeroPointsLeft.empty()){ //if no non-zero points are found, we'll just go straight
        cerr << "Pas de pixels trouvés dans la zone gauche de l'image coupée!" << endl;
        leftMean = 0;
    }
    else{
        nonZeroPtsCount = nonzeroPointsLeft.size();
        //cout << "nonZeroPtsCount L: " << nonZeroPtsCount << endl; //DEBUG

        for(int i = 0; i < nonZeroPtsCount; i++ ){
            pnt = nonzeroPointsLeft[i];
            sumL += pnt.x;
            //cout << pnt.x << endl; //DEBUG
        }
        leftMean = sumL / nonZeroPtsCount;
    }
    //Right line mean position
    findNonZero(lineRightPart, nonzeroPointsRight);
    if(nonzeroPointsRight.empty()){ 
        cerr << "Pas de pixels trouvés dans la zone droite de l'image coupée!" << endl;
        rightMean = 0;
    }
    else{
        nonZeroPtsCount = nonzeroPointsRight.size();
        //cout << "nonZeroPtsCount R: " << nonZeroPtsCount << endl; //DEBUG

        for(int i = 0; i < nonZeroPtsCount; i++ ){
            pnt = nonzeroPointsRight[i];
            sumR += pnt.x;
            //cout << pnt.x << endl; //DEBUG
        }
        rightMean = (sumR / nonZeroPtsCount) + lineColsHalf; //we add the original image's middle since we are working on a slice with its own coordinates
    }
    if(leftMean != 0 || rightMean != 0)
        return ((rightMean - leftMean) / 2) + leftMean;
    else
        return -1;
}

void applyThresholding(Mat* src, Mat* thr_img){

    //Upper and Lower HSV bounds
	int h_low, sat_low, val_low, h_hi, sat_hi, val_hi;
	h_low = HUE_LOW;
	sat_low = SAT_LOW;
	val_low = VAL_LOW;
	h_hi = HUE_HI;
	sat_hi = SAT_HI;
	val_hi = VAL_HI;	

	cvtColor(*src, *thr_img, COLOR_BGR2HSV);
	inRange(*thr_img, Scalar(h_low, sat_low, val_low), Scalar(h_hi, sat_hi, val_hi), *thr_img); //séparation de la couleur voulue
	
    // lissage de l'image
    Mat element = getStructuringElement( MORPH_RECT, Size(5, 5), Point(2, 2));
	morphologyEx(*thr_img, *thr_img, MORPH_OPEN, element, Point(-1, -1), 2); //ouverture
	morphologyEx(*thr_img, *thr_img, MORPH_CLOSE, element, Point(-1, -1), 2); //fermeture
    
    return;
}

float calcAngle(Mat* divImg, int row, int dy){
    float x1, x2, alpha;
	x1 = findMiddle(divImg, row);
	cout << "x @y: " << x1 << endl;
    if(x1 == -1){ //si pas de ligne détectée
        return NAN;
    }
    x2 = findMiddle(divImg, row + dy);
    cout << "x @y + dy: " << x2 << endl;
    if(x2 == -1){
        return NAN;
    }

    alpha = atan((x2 - x1)/dy);

    return alpha;
}
