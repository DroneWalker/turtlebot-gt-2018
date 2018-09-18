//
// Created by charris on 9/18/18.
//

#ifndef HARRIS_BALL_FOLLOWER_BALL_FOLLOWER_H
#define HARRIS_BALL_FOLLOWER_BALL_FOLLOWER_H



#include "ros/ros.h"
#include "geometry_msgs/Point.h"

#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>

#include <sstream>

using namespace cv;
using namespace std;

/// Global Variables
const int KERNEL_LENGTH = 7;
// HSV Ranges
//const int low_H = 0, low_S = 220, low_V = 120;
//const int high_H = 20, high_S = 255, high_V = 255;
const int low_H = 0, low_S = 100, low_V = 80;
const int high_H = 20, high_S = 255, high_V = 255;
const int morph_operator = 2;
const int morph_elem = 0;
const int morph_size = 1;



// Convert to string
#define SSTR( x ) static_cast< std::ostringstream & >( \
( std::ostringstream() << std::dec << x ) ).str()

Mat orangeMask(Mat frame)
{
    Mat mask;
    cv::cvtColor(frame, mask, COLOR_BGR2HSV);

    cv::GaussianBlur(mask, mask, Size(KERNEL_LENGTH, KERNEL_LENGTH), 0, 0);
    inRange(mask, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), mask);


    Mat element = cv::getStructuringElement( morph_elem, Size( 7, 7 ), Point( morph_size, morph_size ) );

    cv::morphologyEx(mask, mask, 2, element);
    cv::morphologyEx(mask, mask, 1, element);


    return mask;

}

vector<Vec3f> detect_circles(Mat frame)
{
    cv::GaussianBlur(frame, frame, Size(7,7), 2, 2);

    vector<Vec3f> circles;
    cv::HoughCircles(frame, circles, CV_HOUGH_GRADIENT,1, 100, 20, 30, 10, 0);

    return circles;
}


vector<Vec3f> detectBall(Mat frame)
{
    Mat circle_image;
}

#endif //HARRIS_BALL_FOLLOWER_BALL_FOLLOWER_H
