//
// Created by charris on 9/18/18.
//

#ifndef HARRIS_BALL_FOLLOWER_FIND_BALL_H
#define HARRIS_BALL_FOLLOWER_FIND_BALL_H



#include "ros/ros.h"
#include "geometry_msgs/Point.h"

#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>

#include <sstream>


class FindBallImpl;
class FindBall
{
public:
    // frame - OpenCV Matrix to be examined
    FindBall(cv::Mat frame);

    //Define Functions
    cv::Mat orangeMask();
    std::vector<cv::Vec3f> detectCircles();
    geometry_msgs::Point getCenterPoint(std::vector<cv::Vec3f> circles);
    void setFrame(cv::Mat frame);
    cv::Mat getFrame();
    ~FindBall();

private:
    FindBallImpl *pimpl;
};

#endif //HARRIS_BALL_FOLLOWER_FIND_BALL_H
