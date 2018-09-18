//
// Created by charris on 9/18/18.
//

#include "ros/ros.h"
#include "geometry_msgs/Point.h"
#include "geometry_msgs/Twist.h"

#include "math.h"

#include "pid_turtlebot.h"

// Declarations
const double linear_vels = 0;
double angular_vel;
double x_pixel;
double pi = M_PI;
double error;


double radian_error(double current)
{
    double desired_pixel_X = 640/2;
    double pixels = 640;
    double field_of_view = 66.2;
    double pixel_deg = 66.2/640; // degrees per pixel
    double x_pixel_error = desired_pixel_X - current;
    double deg_error = x_pixel_error * pixel_deg;
    double radian_error = deg_error * 2 * pi / 360;

    return radian_error;
}

/**
 * This tutorial demonstrates simple receipt of messages over the ROS system.
 */
void targetpointCallback(const geometry_msgs::Point& point_msg)
{
   // ROS_INFO("inside callback, point at x = " + point_msg.get()->x + " and y =" + point_msg.get().y);
    ROS_INFO("inside callback, point received");
    x_pixel = point_msg.x;
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "drive_wheels");
    ros::NodeHandle n;
    ros::Subscriber trackpoint_sub = n.subscribe("trackpoint", 1000, targetpointCallback);
    ros::Publisher twist_pub = n.advertise<geometry_msgs::Twist>("cmd_vel", 5);
    ros::Rate loop_rate(10);


    geometry_msgs::Twist twist_vel;
    twist_vel.linear.x = 0;
    twist_vel.linear.y = 0;
    twist_vel.linear.z = 0;

    twist_vel.angular.x = 0;
    twist_vel.angular.y = 0;

    PID pid = PID(0.1, 1.5, -1.5, 1.0, 0.1,0);


    while(ros::ok()) {
        // Calculate pixel error
        error = radian_error(x_pixel);
        twist_vel.angular.z = pid.calculate(error);

        twist_pub.publish(twist_vel);
        ROS_INFO("Publishing cmd_vel");
        ros::spinOnce();
        loop_rate.sleep();
    }
    ros::spin();

    return 0;
}