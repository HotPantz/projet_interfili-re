#include <opencv2/opencv.hpp>
#include "../headers/thresholding.h"
#include <utility>
#include <iostream>
#include <cstdlib>

using namespace cv;
using namespace std;

//g++ -Wall $(pkg-config --cflags --libs opencv4) sources/thresholding.cpp sources/lines_detect_main.cpp -o lines_detect

int main(int argc, char* argv[]){
	string image_path = TESTIMG_FPATH;
	int width = atoi(argv[1]);
	int mode = atoi(argv[2]); //0 for image, 1 for video
	float mid = 0.0, angle = 0.0;
	
	//load the original image		

	if(argc < 3){
		cout << "Usage: ./lines_detect <width> <mode>" << endl;
		return EXIT_FAILURE;
	}
	if(mode == 0){ //mode image

		Mat img = imread(image_path, IMREAD_COLOR);
		if(img.empty()){
			cout<<"Could not read the image @path :" << image_path << endl;
			return EXIT_FAILURE;	
		}

		//Display base image before thresholding
		namedWindow("Base image", WINDOW_AUTOSIZE);
		imshow("Base image", img);

		//Apply filter	
		Mat thr_img;
		applyThresholding(&img, &thr_img);

		//Display thresholded img
		namedWindow("Thresholded Image", WINDOW_AUTOSIZE); //display filtered img   
		imshow("Thresholded Image", thr_img);	

		
		//divide the image into n parts
		Mat divImg = getNthPart(&thr_img, 4, 2, width);
		namedWindow("Cut Image", WINDOW_AUTOSIZE);
		imshow("Cut Image", divImg);	
			
		mid = findMiddle(&divImg, 0);
		cout << "mid coord : " << mid << endl;
		
		angle = calcAngle(&divImg, 0, 75); 
		cout << "angle : " << angle << endl;

		char key = (char) waitKey();
	}
	if(mode == 1){
		VideoCapture cap(0);
		if(!cap.isOpened()){
			cout << "Could not open the camera" << endl;
			return EXIT_FAILURE;
		}

		//original image display window
		namedWindow("Image Display", WINDOW_AUTOSIZE);
		//imshow("Image Display", img);
		namedWindow("Cut Image", WINDOW_AUTOSIZE);
		namedWindow("Thresholded Image", WINDOW_AUTOSIZE);
		Mat frame;
		while(true){
			cap >> frame; //on capture image par image
			if(frame.empty())
				break;
			imshow("Image Display", frame);

			// Apply filter
			Mat thr_img;
			applyThresholding(&frame, &thr_img);

			// Show the thresholded image
			imshow("Thresholded Image", thr_img);

			// Divide the image into n parts
			Mat divImg = getNthPart(&thr_img, NUM_DIV, NUM_SLICE, width);
			
			// Show the cut image
			imshow("Cut Image", divImg);
				
			mid = findMiddle(&divImg, 0);
			cout << "mid coord : " << mid << endl;
			
			angle = calcAngle(&divImg, 0, 75); 
			cout << "angle : " << angle << endl;
				
			// Press  ESC on keyboard to exit
			char c=(char)waitKey(25);
			if(c==27)
				break;
		}

		// When everything done, release the video capture object
		cap.release();

		// Closes all the frames
		destroyAllWindows();
	}
	
	return 0;
}
