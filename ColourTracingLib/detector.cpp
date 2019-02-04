#include "detector.h"

Detector::Detector()
{

}


Detector::Detector(Mat3b image, HsvObj obj)
{
  cvtColor(image, hsv_img_, COLOR_BGR2HSV);
  inRange(hsv_img_, Scalar(obj.low_.hue_, obj.low_.sat_, obj.low_.val_), Scalar(obj.high_.hue_, obj.high_.sat_, obj.high_.val_), threshold_);

  erode(threshold_, threshold_, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));
  dilate(threshold_, threshold_, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));

  dilate(threshold_, threshold_, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));
  erode(threshold_, threshold_, getStructuringElement(MORPH_ELLIPSE, Size(5,5)));


}

Point Detector::objPos()
{
  Moments image_moment = moments(threshold_);
  double i_area = image_moment.m00;
  double i_x = image_moment.m10/i_area;
  double i_y = image_moment.m01/i_area;

  if (i_area > 1 * pow(10,5))
  {
    return Point(i_x, i_y);
  }

}

ContourType Detector::contourInfo()
{
  findContours(threshold_, contour_.contours_, contour_.hierarchy_, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

  return contour_;
}

void Detector::renderContours(Mat3b& image)
{
  findContours(threshold_, contour_.contours_, contour_.hierarchy_, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
  // if (contour_.contours_.size() > 0)
  // {
    std::cout << "Contour detected\n";
    for (int i = 0; contour_.contours_.size(); i++)
    {
      drawContours(image, contour_.contours_, i, Scalar(255,0,0),0,8,contour_.hierarchy_);
    }
  // }
}

// Mat3b Detector::renderContours()
// {
//   Mat3b image;
//   findContours(threshold_, contour_.contours_, contour_.hierarchy_, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
//   std::cout << "Contour detected\n";
//   for (int i = 0; contour_.contours_.size(); i++)
//   {
//     drawContours(image, contour_.contours_, i, Scalar(255,0,0),0,8,contour_.hierarchy_);
//   }
//
//   return image;
// }
Mat1b Detector::getThreshold()
{
  return threshold_;
}

bool Detector::objectCollision(Vec3b objColour)
{
  for (int i = 0; i < contour_.contours_.size(); i++)
  {
    for (int j = 0; j < contour_.contours_[i].size(); j++)
    {
      // Vec3b colour = blank_image.at<Vec3b>(contours_[i][j]); //determine the colour at the specified point on the image
      // if (colour == objColour)
      //   std::cout << "Collision detected\n";
    }
  }
}
