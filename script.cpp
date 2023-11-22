#include <iostream>
#include <opencv2/core/base.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

using namespace cv;
using namespace std;

int main(int argc, char** argv)
{
  Mat im = imread(argc == 2 ? argv[1] : "lena.png", 1);
  if (im.empty())
  {
    cout << "Cannot open image!" << endl;
    return -1;
  }

  imshow("image", im);
  waitKey(0);

  return 0;
}