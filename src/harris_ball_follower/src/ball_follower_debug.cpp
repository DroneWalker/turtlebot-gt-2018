//
// Created by charris on 9/17/18.
//

#include "ros/ros.h"

#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
#include <opencv2/core/ocl.hpp>

#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>


void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{
    try {
        cv::imshow("find_ball_debugger", cv_bridge::toCvShare(msg, "bgr8")->image);
        cv::waitKey(10);
    }
    catch (cv_bridge::Exception& e)
    {
        ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
    }
}

int main(int argc, char **argv) {
    // ROS Setup
    ros::init(argc, argv, "find_ball");
    ros::NodeHandle n;
    cv::namedWindow("find_ball_debugger");
    cv::startWindowThread();
    image_transport::ImageTransport it(n);
    image_transport::Subscriber image_sub = it.subscribe("/camera/image", 1, imageCallback);
    ros::spin();
    cv::destroyWindow("find_ball_debugger");
}