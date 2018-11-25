//
// Created by charris on 9/18/18.
//

#include "ros/ros.h"
#include "std_msgs/String.h"
#include "geometry_msgs/Point.h"
#include "geometry_msgs/Twist.h"
#include <harris_chase_object/DistanceAngle.h>


#include "math.h"

#include "../../../harris_navigate_to_goal/src/objects/pid_turtlebot.cpp"

double desiredAngle = 0;
double desiredDistance = 0.5; // meters

double currentAngle;
double currentDistance;

bool findball = false;

float pi = M_PI;


void stateCallback (const std_msgs::String& state_msg)
{
    ROS_INFO("recieved state");
    if (state_msg.data == "follow")
    {
        findball = true;
    }
    else if (state_msg.data == "idle")
    {
        findball = false;
    }
}


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
}

double linear_error()
{
    double error;
//    if (error < 0 || error == NAN)
//    {
//        error = 0;
//    } else
//    {
//        error = currentDistance - desiredDistance;
//    }
        error = currentDistance - desiredDistance;

    return error;
}

double angular_error()
{
    double error = desiredAngle - currentAngle;
    return error;
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "follow_drive");
    ros::NodeHandle n;
    ros::Subscriber trackpoint_sub = n.subscribe("/commander/ball_location", 10, ballCallback);
    ros::Subscriber state_sub = n.subscribe("/commander/state", 10, stateCallback);
    ros::Publisher twist_pub = n.advertise<geometry_msgs::Twist>("cmd_vel", 10);
    ros::Rate loop_rate(2);


    geometry_msgs::Twist twist_vel;
    twist_vel.linear.x = 0;
    twist_vel.linear.y = 0;
    twist_vel.linear.z = 0;

    twist_vel.angular.x = 0;
    twist_vel.angular.y = 0;
    twist_vel.angular.z = 0;

    PID pid_linear = PID(0.1, 0.1, -0.1, 0.2, 0.05,0);
    PID pid_angular = PID(0.1, 1.0, -1.0, 1.0, 0.3, 0.1);


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
            twist_vel.linear.x = 0;
            twist_vel.angular.z = 0;
        }
        twist_pub.publish(twist_vel);
        ROS_INFO("Publishing cmd_vel");
        ros::spinOnce();
        loop_rate.sleep();
    }
    ros::spin();

    return 0;
}