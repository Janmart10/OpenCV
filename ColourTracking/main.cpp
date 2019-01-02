#include <iostream>
#include <sstream>
#include <string>
#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"


#define PI 3.14159
#define FOCAL 25  //cm, changes depending on camera used
#define REAL_DIAMTER 1.5 //cm
#define RECT_LENGTH 20 //distance from centre to boundary of rectangle
using namespace cv;

int main()
{
  VideoCapture cap(0);
  if (!cap.isOpened())
  {
    std::cout << "Could not open camera\n";
    return -1;
  }
  int iLowH = 75, iHighH = 130;
  int iLowS = 0, iHighS = 255;
  int iLowV = 0, iHighV = 255;

  namedWindow("Control", CV_WINDOW_AUTOSIZE);

  cvCreateTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
  cvCreateTrackbar("HighH", "Control", &iHighH, 179);

  cvCreateTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
  cvCreateTrackbar("HighS", "Control", &iHighS, 255);

  cvCreateTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
  cvCreateTrackbar("HighV", "Control", &iHighV, 255);

  int lastX = -1;
  int lastY = -1;


  while (true)
  {
    Mat3b image;
    bool success = cap.read(image);
    if (!success)
    {
      std::cout << "Could not read frame\n";
    }

    if (waitKey(30) == 27)
      break;

    Mat3b imageHSV;
    cvtColor(image, imageHSV,COLOR_BGR2HSV);
    Mat1b threshold_image;

    inRange(imageHSV, Scalar(iLowH, iLowS,iLowV), Scalar(iHighH,iHighS,iHighV), threshold_image);

    //removing spots in background
    erode(threshold_image, threshold_image,getStructuringElement(MORPH_ELLIPSE, Size(5,5)));
    dilate(threshold_image, threshold_image, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));

    dilate(threshold_image, threshold_image, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));
    erode(threshold_image, threshold_image,getStructuringElement(MORPH_ELLIPSE, Size(5,5)));


    Moments image_moments = moments(threshold_image); //calculating the weighted average of certain pixels

    double i_area = image_moments.m00;
    double m01 = image_moments.m01;
    double m10 = image_moments.m10; //the horizontal

    //std::cout << "Area: " << i_area << '\n';
    double diameter, distance;
    std::stringstream ss;
    ss << distance;
    std::string sentence = "Object detected, distance: " + ss.str();
    if (i_area > 1.5 * pow(10,5))
    {
      diameter = sqrt((4*i_area * pow(10,-5))/PI);
      distance = (REAL_DIAMTER * FOCAL)/diameter;

      std::cout << "Distance of object from camera: " << distance << '\n';
      int pos_x = m10/i_area; //horizontal location of centroid of moment
      int pos_y = m01/i_area; //verticial location of centroid of moment

      //placing text box on the top left of the object
      Point p1(pos_x - diameter - RECT_LENGTH, pos_y - diameter - RECT_LENGTH);
      Point p2(pos_x + diameter + RECT_LENGTH, pos_y + diameter + RECT_LENGTH);

      rectangle(image, p1,p2, Scalar(0,255,0));
      putText(image, sentence, Point(p1.x, p1.y - 5), FONT_HERSHEY_PLAIN,1, Scalar(0,0,0));
      //prevents line being drawn initially
      if (lastX > 0 && lastY > 0 && pos_x > 0 && pos_y > 0)
      {
        line(image, Point(pos_x, pos_y), Point(lastX,lastY), Scalar(255,0,0),2);
      }

      lastX = pos_x;
      lastY = pos_y;

    }
    imshow("Threshold Image", threshold_image);
    imshow("Original Image", image);


  }


  return 0;
}
