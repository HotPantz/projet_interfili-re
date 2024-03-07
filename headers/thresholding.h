#include <opencv2/opencv.hpp>
using namespace cv;
#ifndef THRESH_H
#define THRESH_H

#define TESTIMG_FPATH "../images/lines.png"

/* HSV Color Range Values */
#define HUE_LOW 61
#define SAT_LOW 63
#define VAL_LOW 81

#define HUE_HI 141
#define SAT_HI 204
#define VAL_HI 212

void brightnessChangeCallback(int value, void* data);

// Takes in a lower and upper HSV bound, strips the inputIMG
// of all pixels but the with values in specified the range
Mat isolateColor(Mat* inputImg, Scalar lowHSV, Scalar upHSV);

//Divide the image in divNum parts and return the nth part (parts numbered from 1-n)
Mat getNthPart(Mat* inputImage, int divNum, int n, int width);

//Finds the mean coordinates of non-zero values on a line @lineX coords of img 
float findMeanLineY(Mat* img, int lineY);


#endif
