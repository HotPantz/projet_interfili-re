#include <opencv2/opencv.hpp>
using namespace cv;

#ifndef THRESH_H
#define THRESH_H

#define TESTIMG_FPATH "images/lines.png"

/* HSV Color Range Values */
#define HUE_LOW 61
#define SAT_LOW 63
#define VAL_LOW 81

#define HUE_HI 141
#define SAT_HI 204
#define VAL_HI 212


/*#define HUE_LOW 105
#define SAT_LOW 63
#define VAL_LOW 0

#define HUE_HI 135
#define SAT_HI 255
#define VAL_HI 255*/

/* Number of divisions for the image */
#define NUM_DIV 4

/* Index of the slice to get */
#define NUM_SLICE 2

void brightnessChangeCallback(int value, void* data);

// Takes in a lower and upper HSV bound, strips the inputIMG
// of all pixels but the with values in specified the range
Mat isolateColor(Mat* inputImg, Scalar lowHSV, Scalar upHSV);

//Divide the image in divNum parts and return the nth part (parts numbered from 1-n)
Mat getNthPart(Mat* inputImage, int divNum, int n, int width);

//Finds the mean coordinates of non-zero values on a line @lineY coords of img (of a part, not the entire image) 
float findMeanLineY(Mat* img, int lineY);

//Finds the middle position between 2 lines, having computed the horizontal mean position of the left and right lines
float findMiddle(Mat* img, int lineY);

//Applies thresholding (and a opening/closing morph. transformation) on src image, and saves it in thr_img
void applyThresholding(Mat* src, Mat* thr_img);

/*  Calculate the angle using 2 different middle values and the arctangent
    row is the first pixel row to look at in the divided portion of the image
    dy is the distance in pixels/rows to the second row */
float calcAngle(Mat* divImg, int row, int dy);

#endif
