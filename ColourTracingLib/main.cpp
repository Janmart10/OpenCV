#include <iostream>
#include <sstream>
#include <string>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <vector>

#include "detector.h"
#include "timer.h"
#include "mathutility.h"

//global variables for Hue, saturation and value, representing the min and max value
int H_MIN = 0;
int H_MAX = 256;
int S_MIN = 101;  //changes based on lighting
int S_MAX = 256;
int V_MIN = 40; //changes based on lighting
int V_MAX = 256;

//were oringally part of the detector class but didnt work for unkown reasons...
void renderContours(Mat& image, ContourType info)
{
  for (int i = 0; i < info.contours_.size(); i++)
  {
    drawContours(image, info.contours_, i, Scalar(255,0,0),0,8,info.hierarchy_);
  }
}



//using namespace cv;
using namespace mathutility;

int main()
{
  Point blob_pos(100, 100);
  Vectortype blob_velocity(2,2);
  Timer timer;

  VideoCapture cap(0);
  if (!cap.isOpened())
  {
    std::cout << "Could not open camera\n";
    return -1;
  }

  HsvObj blue(Hsv(75, S_MIN, V_MIN), Hsv(130, 255, 255));

  std::string bar_name = "Control";
  //creating a window filled with trackbars that can be used to alter the min hsv values (might need to adjust depending on the lighting)
  namedWindow(bar_name, CV_WINDOW_AUTOSIZE);

  createTrackbar( "H_MIN", bar_name, &H_MIN, H_MAX);
  createTrackbar( "H_MAX", bar_name, &H_MAX, H_MAX);

  createTrackbar( "S_MIN", bar_name, &S_MIN, S_MAX);
  createTrackbar( "S_MAX", bar_name, &S_MAX, S_MAX);

  createTrackbar( "V_MIN", bar_name, &V_MIN, V_MAX);
  createTrackbar( "V_MAX", bar_name, &V_MAX, V_MAX);

  while (true) {
    //creating an image and storing the frames capture by the camera onto the image. Also checking if it was a success
    Mat3b image;
    bool success = cap.read(image);

    if (!success)
    std::cout << "Could not read frame\n";

    //if the escape key is pressed, exit the loop and stop the program
    if (waitKey(30) == 27)
    break;

    Mat blank_image(image.rows, image.cols, CV_8UC3, Scalar(255,255,255)); //(height, width, number of bits and number of channels, colour of image)
    //flip(blank_image, blank_image,+1);

    if (timer.timeOver(0.0005))
    {
      blob_pos.x += blob_velocity.i_;
      blob_pos.y += blob_velocity.j_;
    }




    Detector blue_obj(image, blue);
    renderContours(blank_image, blue_obj.contourInfo());
    circle(blank_image, blob_pos, 20, Scalar(0,0, 255), CV_FILLED, 8,0); //(image to draw to, location to be drawn, radius size, colour, type of filling, thickness, shift)

    for (int i = 0; i < blue_obj.contourInfo().contours_.size(); i++)
    {
      for (int j = 0; j < blue_obj.contourInfo().contours_[i].size(); j++)
      {
        Vec3b colour = blank_image.at<Vec3b>(blue_obj.contourInfo().contours_[i][j]); //determine the colour at the specified point on the image
        if (colour == Vec3b(0,0,255))
        {
          std::cout << "Collision detected\n";
          Point obstcle = blue_obj.contourInfo().contours_[i][j];

          if (blob_pos.x <= obstcle.x)
            blob_velocity.i_ = -2;
          else if (blob_pos.x >= obstcle.x)
            blob_velocity.i_ = 2;

          if (blob_pos.y <= obstcle.y)
            blob_velocity.j_ = -2;
          else if (blob_pos.y >= obstcle.y)
              blob_velocity.j_ = 2;

        }
      }
    }

    imshow("Original Image", image);
    imshow("Blue threshold", blue_obj.getThreshold());
    //flip(blank_image, blank_image,+1);
    imshow("Contour Image", blank_image);
    //imshow("Contour Image", blue_obj.renderContours());
  }
  return 0;
}
