/**
 * Simple shape detector program.
 * It loads an image and tries to find simple shapes (rectangle, triangle, circle, etc) in it.
 * This program is a modified version of `squares.cpp` found in the OpenCV sample dir.
 */
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cmath>
#include <iostream>

/**
 * Helper function to find a cosine of angle between vectors
 * from pt0->pt1 and pt0->pt2
 */
static double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

/**
 * Helper function to display text in the center of a contour
 */
void setLabel(cv::Mat& im, const std::string label, std::vector<cv::Point>& contour)
{
	int fontface = cv::FONT_HERSHEY_SIMPLEX;
	double scale = 0.4;
	int thickness = 1;
	int baseline = 0;

	cv::Size text = cv::getTextSize(label, fontface, scale, thickness, &baseline);
	cv::Rect r = cv::boundingRect(contour);

	cv::Point pt(r.x + ((r.width - text.width) / 2), r.y + ((r.height + text.height) / 2));
	cv::rectangle(im, pt + cv::Point(0, baseline), pt + cv::Point(text.width, -text.height), CV_RGB(255,255,255), CV_FILLED);
	cv::putText(im, label, pt, fontface, scale, CV_RGB(0,0,0), thickness, 8);
}

int main()
{
	//cv::Mat src = cv::imread("polygon.png");
	cv::Mat src = cv::imread("/home/janmart/Documents/OpenCVStuff/Shape_Detection_Git/images.jpeg");
	if (src.empty())
		return -1;

	// Convert to grayscale
	cv::Mat gray;
	cv::cvtColor(src, gray, CV_BGR2GRAY);

	// Use Canny instead of threshold to catch squares with gradient shading
	cv::Mat bw;
	//@param:
	//gray = source image
	//bw = image to display detected edges
	//0 = low threshold
	//100 = high threshold
	//3 = sobel operator size (matrix size)

	//Process:
	//1. Grayscale conversion (Helps with finding edges)
	//2. Noise reduction via Gaussian blur
	//3. A sobel filter is applied to find gradients (first step of finding edges). Both horizontal and vertical, Gx and Gy are calculated
	//Gx: -1 0 1
	//		-2 0 2
	//		-1 0 1

	//Gy: -1 -2 -1
	//		 0  0  0
	//		 1  2  1

	//4. Non-maximum suppression is applied, which removes pixels that are not considered to be edges, ensuring only relatively rthin lines remain
	//5. Hysteresis: Canny uses two thresholds (upper and lower):
	//a. if a pixel gradient is higher than the upper threshold, the pixel is accepted as an edge
	//b. if a pixel gradient value is below the lower threshold, then it is rejected
	//c. if the pixel gradient is between the 2 thresholds, then it will be accepeted only if it is connected to a pixel that is above
	// the upper threshold
	cv::Canny(gray, bw, 0, 100, 3);

	// Find contours (a curve joining continous points)
	std::vector<std::vector<cv::Point> > contours;
	//contours are stored as a vector of continous joining points
	//thus "contours" stores different contours registered
	//note, each contour stored is just a vector of points, no edges are defined
	//to define edges within this contour , approxPolyDP is used
	cv::findContours(bw.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

	std::vector<cv::Point> approx;
	cv::Mat dst = src.clone();

	for (int i = 0; i < contours.size(); i++)
	{
		// Approximate contour with accuracy proportional
		// to the contour perimeter
		//i.e. check the current contour and determine line segments
		//store the line segments in approx
		cv::approxPolyDP(cv::Mat(contours[i]), approx, cv::arcLength(cv::Mat(contours[i]), true)*0.02, true);

		// Skip small or non-convex objects
		if (std::fabs(cv::contourArea(contours[i])) < 100 || !cv::isContourConvex(approx))
			continue;

		if (approx.size() == 3)
		{
			setLabel(dst, "TRI", contours[i]);    // Triangles
			std::cout << "Starting point: " << approx[0].x << " " << approx[0].y << '\n';
			std::cout << "2nd point: " << approx[1].x << " " << approx[1].y << '\n';
			std::cout << "3rd point: " << approx[2].x << " " << approx[2].y << '\n';

			// cv::line(dst, approx[0], approx[1], cv::Scalar(255,0,0), 4,8);
			// cv::line(dst, approx[1], approx[2], cv::Scalar(255,0,0), 4,8);
			// cv::line(dst, approx[2], approx[0], cv::Scalar(255,0,0), 4,8);

			for (int i = 0; i < 3; i++)
			{
				if (i == 2) // if end of line segment
					cv::line(dst, approx[i], approx[0], cv::Scalar(255,0,0), 4,8);
				else
					cv::line(dst, approx[i], approx[i+1], cv::Scalar(255,0,0), 4,8);
			}

		}

		if (approx.size() == 4)
		{
			setLabel(dst, "SQ", contours[i]);
		}
	}

	cv::imshow("src", src);
	cv::imshow("dst", dst);
	cv::waitKey(0);
	return 0;
}
