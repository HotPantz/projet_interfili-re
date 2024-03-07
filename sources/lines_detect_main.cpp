#include <opencv2/opencv.hpp>
#include "../headers/thresholding.h"
#include <utility>
#include <iostream>
#include <cstdlib>

using namespace cv;
using namespace std;

//g++ -Wall $(pkg-config --cflags --libs opencv4) sources/thresholding.cpp sources/lines_detect_main.cpp -o lines_detect

int main(){
	string image_path = TESTIMG_FPATH;
	
	//Upper and Lower HSV bounds
	int h_low, sat_low, val_low, h_hi, sat_hi, val_hi;
	h_low = HUE_LOW;
	sat_low = SAT_LOW;
	val_low = VAL_LOW;
	h_hi = HUE_HI;
	sat_hi = SAT_HI;
	val_hi = VAL_HI;	

	//load the original image		
	Mat img = imread(image_path, IMREAD_COLOR);
	if(img.empty()){
		cout<<"Could not read the image @path :" << image_path << endl;
		return EXIT_FAILURE;	
	}

	//original image display window
	namedWindow("Image Display", WINDOW_AUTOSIZE);
	imshow("Image Display", img);
	cout<<"oui monsieur\n";	
	
	//Apply filter	
	Mat thr_img = isolateColor(&img, Scalar(h_low, sat_low, val_low), Scalar(h_hi, sat_hi, val_hi));
	namedWindow("Thresholded Image", WINDOW_AUTOSIZE); //display filtered img   
	imshow("Thresholded Image", thr_img);	
	
	//divide the image into n parts
	Mat divImg = getNthPart(&thr_img, 5, 1, 150);
	namedWindow("Cut Image", WINDOW_AUTOSIZE);
	imshow("Cut Image", divImg);	
	
	float mid;
	//mid = findMeanLineY(&divImg, divImg.rows / 2); //get the mean value of non zero elements on the line at half the height of divImg 	
	mid = findMiddle(&divImg, divImg.rows / 2);
	cout << "mid coord : " << mid << endl;

	char key = (char) waitKey();
	
	return 0;
}
