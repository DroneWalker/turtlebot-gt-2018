//
// Created by charris on 9/18/18.
//

#include "ros/ros.h"
#include "geometry_msgs/Point.h"
#include "geometry_msgs/Twist.h"
#include <harris_chase_object/DistanceAngle.h>


#include "math.h"

#include "../objects/pid_turtlebot.cpp"

double desiredAngle = 0;
double desiredDistance = 1; // meters

double currentAngle;
double currentDistance;

bool findball = false;

double pi = M_PI;


void ballCallback(const harris_chase_object::DistanceAngle& ball_msg)
{
    ROS_INFO("recieved ball location");
    if (ball_msg.angle > pi)
    {
       currentAngle = ball_msg.angle - 2*pi;
    }
    else
    {
        currentAngle = ball_msg.angle;
    }
    currentDistance = (double) ball_msg.distance;
    findball = true;
}

double linear_error()
{
    double error;
    if (error < 0)
    {
        error = 0;
    } else
    {
        error = currentDistance - desiredDistance;
    }
    return error;
}

double angular_error()
{
    double error = currentAngle - desiredAngle;
    return error;
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "follow_drive");
    ros::NodeHandle n;
    ros::Subscriber trackpoint_sub = n.subscribe("/commander/ball_location", 10, ballCallback);
    ros::Publisher twist_pub = n.advertise<geometry_msgs::Twist>("cmd_vel", 10);
    ros::Rate loop_rate(10);


    geometry_msgs::Twist twist_vel;
    twist_vel.linear.x = 0;
    twist_vel.linear.y = 0;
    twist_vel.linear.z = 0;

    twist_vel.angular.x = 0;
    twist_vel.angular.y = 0;
    twist_vel.angular.z = 0;

    PID pid_linear = PID(0.1, 1.0, 0, 0.1, 0,0);
    PID pid_angular = PID(0.1, 1.0, -1.0, 0.1, 0,0);

    while(ros::ok()) {
        // Calculate pixel error

        if (findball)
        {
            double lin_error = linear_error();
            double ang_error = angular_error();

            twist_vel.linear.x = pid_linear.calculate(lin_error);
            twist_vel.angular.z = pid_angular.calculate(ang_error);
        }
        else
        {
//            twist_vel.linear.x = 0;
//            twist_vel.angular.z = 0;
            double lin_error = linear_error();
            double ang_error = angular_error();

            twist_vel.linear.x = pid_linear.calculate(lin_error);
            twist_vel.angular.z = pid_angular.calculate(ang_error);
        }

//        findball = false;

        twist_pub.publish(twist_vel);
        ROS_INFO("Publishing cmd_vel");
        ros::spinOnce();
        loop_rate.sleep();
    }
    ros::spin();

    return 0;
}