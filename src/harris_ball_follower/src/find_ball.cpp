//
// Created by charris on 9/16/18.
//

#ifndef _FindBall_SOURCE_
#define _FindBall_SOURCE_


#include "ros/ros.h"
#include "geometry_msgs/Point.h"
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sstream>

#include "find_ball.h"

using namespace cv;
using namespace std;
using namespace ros;

const int KERNEL_LENGTH = 7;
// HSV Ranges
//const int low_H = 0, low_S = 220, low_V = 120;
//const int high_H = 20, high_S = 255, high_V = 255;
// Working with orange ball
//const int low_H = 0, low_S = 140, low_V = 100;
//const int high_H = 20, high_S = 255, high_V = 255;
const int low_H = 170, low_S = 100, low_V = 80;
const int high_H = 180, high_S = 255, high_V = 255;
const int morph_operator = 2;
const int morph_elem = 0;
const int morph_size = 1;


// Convert to string
#define SSTR( x ) static_cast< std::ostringstream & >( \
( std::ostringstream() << std::dec << x ) ).str()


class FindBallImpl
{
public:
    FindBallImpl(cv::Mat frame);
    ~FindBallImpl();
    geometry_msgs::Point getCenterPoint(std::vector<cv::Vec3f> circles);
    Mat orangeMask();
    vector<Vec3f> detectCircles(Mat mask);
    void setFrame(Mat frame);
    Mat getFrame();

private:
    cv_bridge::CvImagePtr _img;
    cv::Mat _frame;
};

FindBall::FindBall(Mat frame)
{
    pimpl = new FindBallImpl(frame);
}

geometry_msgs::Point FindBall::getCenterPoint(std::vector<cv::Vec3f> circles)
{
    return pimpl->getCenterPoint(circles);
}

Mat FindBall::orangeMask()
{
    return pimpl->orangeMask();
}

vector<Vec3f> FindBall::detectCircles(Mat mask)
{
    return pimpl->detectCircles(mask);
}

void FindBall::setFrame(Mat frame)
{
    return pimpl->setFrame(frame);
}

Mat FindBall::getFrame()
{
    return pimpl->getFrame();
}
FindBall::~FindBall()
{
    delete pimpl;
}

/**
 * Implementation
 */

FindBallImpl::FindBallImpl(Mat frame) :
        _frame(frame)
{
}

Mat FindBallImpl::orangeMask()
{
    Mat frame = _frame;
    Mat mask;
    cv::cvtColor(frame, mask, COLOR_BGR2HSV);

    cv::GaussianBlur(mask, mask, Size(KERNEL_LENGTH, KERNEL_LENGTH), 0, 0);
    inRange(mask, Scalar(low_H, low_S, low_V), Scalar(high_H, high_S, high_V), mask);


    Mat element = cv::getStructuringElement( morph_elem, Size( 7, 7 ), Point( morph_size, morph_size ) );

    cv::morphologyEx(mask, mask, 2, element);
    cv::morphologyEx(mask, mask, 1, element);


    return mask;
}

vector<Vec3f> FindBallImpl::detectCircles(Mat mask)
{
    cv::GaussianBlur(mask, mask, Size(7,7), 2, 2);

    vector<Vec3f> circles;
    cv::HoughCircles(mask, circles, CV_HOUGH_GRADIENT,1, 100, 20, 30, 10, 0);

    return circles;
}

geometry_msgs::Point FindBallImpl::getCenterPoint(std::vector<cv::Vec3f> circles)
{
    cv::Point centerpoint;
    cv::Rect2d box;
    geometry_msgs::Point circle_cp;
    int largest_radius = 0;
    int largest_circle_index = 0;


    if (!circles.empty())
    {
        // Draw the circles detected
        for (size_t i = 0; i < circles.size(); i++)
        {
            int radius = cvRound(circles[i][2]);
            if (radius > largest_radius)
            {
                largest_radius = radius;
                largest_circle_index = i;
                int xp = circles[i][0];
                int yp = circles[i][1];
                circle_cp.x = xp;
                circle_cp.y = yp;
                circle_cp.z = 0;
            }
        }
    }

    return circle_cp;

}

void FindBallImpl::setFrame(Mat frame)
{
    _frame = frame;
}

Mat FindBallImpl::getFrame()
{
    return _frame;
}

FindBallImpl::~FindBallImpl()
{
}




#endif
