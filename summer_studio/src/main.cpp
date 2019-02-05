#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "detector.h"
#include "timer.h"
#include "mathutility.h"

#include "ros/ros.h"
#include "geometry_msgs/Twist.h"

#include "sensor_msgs/Image.h" //found by rostopic type /camera/color/image_raw
#include <cv_bridge/cv_bridge.h>
#include "sensor_msgs/image_encodings.h"

#include <mutex>
#include <deque>

namespace enc = sensor_msgs::image_encodings;
//using namespace enc;
//using namespace cv_bridge;
using namespace mathutility;
//global variables for Hue, saturation and value, representing the min and max value
int H_MIN = 0;
int H_MAX = 255;
int S_MIN = 101;  //changes based on lighting
int S_MAX = 255;
int V_MIN = 40; //changes based on lighting
int V_MAX = 255;

//Global variables for cv_bridge and cv_image stuff
cv_bridge::CvImagePtr cvPtr_;

struct ImageDataBuffer
{
    std::deque<cv::Mat> imageDeq; /*!< Stores cv::Mat images in a deque  */
    std::deque<ros::Time> timeStampDeq; /*!< Stores ros::Time stamps in a deque */
    std::mutex buffer_mutex_; /*!< mutex used to protect data  */
};
ImageDataBuffer imageBuffer;//! Container for image data


//were oringally part of the detector class but didnt work for unkown reasons...
void renderContours(Mat& image, ContourType info)
{
  for (int i = 0; i < info.contours_.size(); i++)
  {
    drawContours(image, info.contours_, i, Scalar(255,0,0),0,8,info.hierarchy_);
  }
}

//this is the callback used to store the image received from the subscribed topic into a buffer
void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
  // if (enc::isColor(msg->encoding))
  //   cvPtr_ = cv_bridge::toCvCopy(msg, enc::BGR8);
  // else
  //   cvPtr_ = cv_bridge::toCvCopy(msg, enc::MONO8);


  //cv_bridge::toCvCopy converts a sensor_msgs type into a cv_image type
  //which is needed in order to perform OpenCV operations
  //the image received by the realsense camera is being published to the topic /camera/color/image_raw , which as a message type of sensor_msgs/Image
  //thus in order to retrive it we need to subscribe to that topic (done in main)
  cvPtr_ = cv_bridge::toCvCopy(msg, enc::BGR8);

  imageBuffer.buffer_mutex_.lock(); //using a mutex so that the same data is not accessed by multiple threads at the same time
  imageBuffer.imageDeq.push_back(cvPtr_->image); //store the image into the buffer
  imageBuffer.timeStampDeq.push_back(msg->header.stamp); //store the timestamp into the buffer as well

  //if the buffer holds 3 or more images, remove the first one (oldest one )
  if(imageBuffer.imageDeq.size()>2){
      imageBuffer.imageDeq.pop_front();
      imageBuffer.timeStampDeq.pop_front();
  }

  imageBuffer.buffer_mutex_.unlock();
}

//using namespace cv;



int main(int argc, char **argv)
{
  cv::Mat image;
  ros::init(argc, argv, "colour_detect");

  ros::NodeHandle n;
  ros::Subscriber sub = n.subscribe("/camera/color/image_raw", 1000, imageCallback);
  ros::Time timeImage = ros::Time::now();;
  //ros::Rate rate_limiter(1.0);
  ros::Rate loop_rate(10.0);

  HsvObj blue(Hsv(75, S_MIN, V_MIN), Hsv(130, 255, 255));
  HsvObj green(Hsv(90, S_MIN, V_MIN), Hsv(140, S_MAX, V_MAX)); //Creating my own HsvObj type, that contains data regarding an objects
  //low and high HSV intesnities

  //HsvObj red(Hsv(160, S_MIN, V_MIN), Hsv(180, S_MAX, V_MAX));


  std::string bar_name = "Control";
  //creating a window filled with trackbars that can be used to alter the min hsv values (might need to adjust depending on the lighting)
  namedWindow(bar_name, CV_WINDOW_AUTOSIZE);

  createTrackbar( "H_MIN", bar_name, &H_MIN, H_MAX);
  createTrackbar( "H_MAX", bar_name, &H_MAX, H_MAX);

  createTrackbar( "S_MIN", bar_name, &S_MIN, S_MAX);
  createTrackbar( "S_MAX", bar_name, &S_MAX, S_MAX);

  createTrackbar( "V_MIN", bar_name, &V_MIN, V_MAX);
  createTrackbar( "V_MAX", bar_name, &V_MAX, V_MAX);

  while (ros::ok()) {

    //creating an image and storing the frames capture by the camera onto the image. Also checking if it was a success
    imageBuffer.buffer_mutex_.lock(); //locking the buffer while this process happens so that it is not being accessed by multiple threads
    if(imageBuffer.imageDeq.size()>0){ //if an image has been stored in the buffer
      image = imageBuffer.imageDeq.front(); //retrieve the front of the buffer (deque) and store it into the Mat image
      timeImage = imageBuffer.timeStampDeq.front(); //store the current time
      imageBuffer.imageDeq.pop_front(); //remove the front of the image buffer deque of images as it is no longer needed (already been retrieved)
      imageBuffer.timeStampDeq.pop_front();  //remove the front of the image buffer deque of time
    }
    imageBuffer.buffer_mutex_.unlock();

    if (!image.empty())
    {
      //cv_bridge::CvImage cv_image;
      //cv::cvtColor(image, cv_image.image, CV_BGR2HSV);

      //HsvObj red(Hsv(160, S_MIN, V_MIN), Hsv(180, S_MAX, V_MAX));
      HsvObj red(Hsv(0, S_MIN, V_MIN), Hsv(20, S_MAX, V_MAX));


      //if the escape key is pressed, exit the loop and stop the program
      if (waitKey(30) == 27)
      break;

      //Mat blank_image(cv_image.image.rows, cv_image.image.cols, CV_8UC3, Scalar(255,255,255)); //(height, width, number of bits and number of channels, colour of image)
      Mat blank_image(image.rows, image.cols, CV_8UC3, Scalar(255,255,255)); //(height, width, number of bits and number of channels, colour of image)
      //Detector blue_obj(image, blue);
      Detector green_obj(image, green);
      Detector red_obj(image, red);

      //renderContours(blank_image, green_obj.contourInfo());

      imshow("Original Image", image);
      //imshow("Green threshold", green_obj.getThreshold());
      imshow("Red threshold", red_obj.getThreshold());
      //imshow("Blue threshold", blue_obj.getThreshold());
      //flip(blank_image, blank_image,+1);
      imshow("Contour Image", blank_image);
      //imshow("Contour Image", blue_obj.renderContours());
    }
    ros::spinOnce();
    loop_rate.sleep();
    //rate_limiter.sleep();
  }

  return 0;
}
