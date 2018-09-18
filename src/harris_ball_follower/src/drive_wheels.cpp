//
// Created by charris on 9/18/18.
//

#include "ros/ros.h"
#include "geometry_msgs/Point.h"

/**
 * This tutorial demonstrates simple receipt of messages over the ROS system.
 */
void targetpointCallback(const geometry_msgs::PointConstPtr& point_msg)
{
   // ROS_INFO("inside callback, point at x = " + point_msg.get()->x + " and y =" + point_msg.get().y);
    ROS_INFO("inside callback, point received");
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "drive_wheels");
    ros::NodeHandle n;
    ros::Subscriber sub = n.subscribe("trackpoint", 1000, targetpointCallback);
    ros::spin();

    return 0;
}