#ifndef TYPES_H
#define TYPES_H


#include <opencv2/opencv.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <vector>
#include <math.h>

using namespace cv;

struct Hsv
{
  int hue_;
  int sat_;
  int val_;

  Hsv (int h, int s, int v) : hue_(h), sat_(s), val_(v) {}

  void operator=(const Hsv& o)
  {
    hue_ = o.hue_;
    sat_ = o.sat_;
    val_ = o.val_;
  }
};

struct HsvObj
{
  Hsv low_;
  Hsv high_;

  HsvObj(Hsv low, Hsv high) : low_(low), high_(high) {}

  void operator=(const HsvObj& o)
  {
    low_ = o.low_;
    high_ = o.high_;
  }
};

struct ContourType
{
  std::vector< std::vector<Point> > contours_;
  std::vector<Vec4i> hierarchy_;

  ContourType()
  {

  }

  ContourType(std::vector< std::vector<Point> > contours, std::vector<Vec4i> hierarchy)
  {
    hierarchy_ = hierarchy;
    contours_ = contours;
  }

  void operator=(const ContourType& o)
  {
    contours_ = o.contours_;
    hierarchy_ = o.hierarchy_;
  }
};


struct Vectortype
{
  double i_;
  double j_;
  Vectortype() {}

  Vectortype(double i, double j) : i_(i), j_(j) {}

  void operator=(const Vectortype& o)
  {
    i_ = o.i_;
    j_ = o.j_;
  }

};

#endif
