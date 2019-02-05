#ifndef DETECTOR_H
#define DETECTOR_H

#include "types.h"



class Detector
{
public:
  Detector();
  Detector(Mat& image, HsvObj& obj);
  Point objPos();
  ContourType contourInfo(); //includes hierachy
  Mat1b getThreshold();
  void renderContours(Mat3b& image);
  bool objectCollision(Vec3b objColour);
  // Mat3b renderContours();
private:
  Mat3b hsv_img_;
  Mat1b threshold_;
  ContourType contour_;

  HsvObj hsv_obj_;
  // double i_area_;
  // double i_x_;
  // double i_y_;

  void removeNoise();
};

#endif
