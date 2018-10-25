//
// Created by charris on 10/23/18.
//


#include "ros/ros.h"
#include "std_msgs/String.h"
#include "geometry_msgs/Point.h"
#include "geometry_msgs/Twist.h"
#include "geometry_msgs/Vector3.h"
#include "nav_msgs/Odometry.h"
#include "trajectory_msgs/JointTrajectoryPoint.h"

#include <tf2/LinearMath/Quaternion.h>
#include <tf2/LinearMath/Matrix3x3.h>
#include <tf/transform_datatypes.h>


#include <harris_navigate_to_goal/DistanceAngle.h>

#include "math.h"

#include "../../../harris_navigate_to_goal/src/objects/pid_turtlebot.cpp"

geometry_msgs::Pose bot_pose;
geometry_msgs::Twist bot_vels;
geometry_msgs::Pose goal;

geometry_msgs::Twist twist_vel;

geometry_msgs::Vector3 bot_state;

harris_chase_object::DistanceAngle to_goal;

PID pid_linear = PID(0.1, 1.0, -1.0, 0.1, 0.1, 0);
PID pid_angular = PID(0.1, 2.0, -2.0, 0.35, 0.1, 0.1);

double x_p;
double y_p;
double theta_p;

bool init = false;

void odomCallback(const nav_msgs::Odometry& odom_msg)
{
    // Get bot state
    bot_pose = odom_msg.pose.pose;
    bot_vels = odom_msg.twist.twist;

    // Transform quarternion yaw into euler yaw
    tf2::Quaternion q(bot_pose.orientation.x, bot_pose.orientation.y,bot_pose.orientation.z, bot_pose.orientation.w);
    double roll, pitch, yaw;
    tf2::Matrix3x3 m(q);
    m.getRPY(roll, pitch, yaw);

    // Bot state in form to publish
    bot_state.x = bot_pose.position.x;
    bot_state.y = bot_pose.position.y;
    if (yaw < 0)
    {
        bot_state.z = yaw + 2*M_PI;
    }
    else
    {
        bot_state.z = yaw;
    }

    // update_odometry
//    if init == false
//     do this
//init = true
// else not

    double xerror;
    double yerror;
    double thetaerror;
    double distance_error;


    xerror = goal.position.x - bot_state.x;
    yerror = goal.position.y - bot_state.y;
    thetaerror = (atan2(yerror, xerror) - bot_state.z);
    distance_error = sqrt(pow(xerror,2) + pow(yerror,2));

    if (thetaerror > M_PI)
    {
        thetaerror = -M_PI + (thetaerror - M_PI);
    }
    else if (thetaerror < -M_PI)
    {
        thetaerror = M_PI + (thetaerror + M_PI);
    } else
    {
        thetaerror = thetaerror;
    }

    to_goal.distance = distance_error;
    to_goal.angle = thetaerror;


    if (abs(to_goal.angle) > M_PI/36)
    {
        twist_vel.linear.x = 0;
        twist_vel.angular.z = pid_angular.calculate(thetaerror);
    } else
    {
        twist_vel.angular.z = pid_angular.calculate(thetaerror);
        twist_vel.linear.x = pid_linear.calculate(distance_error);
    }

//    twist_vel.linear.x = pid_linear.calculate(distance_error);
//    twist_vel.angular.z = pid_angular.calculate(thetaerror);
}

void waypointCallback(const geometry_msgs::Pose& goal_msg)
{
    goal.position.x = goal_msg.position.x;
    goal.position.y = goal_msg.position.y;
}

// IF NO GOAL, THEN RESET ODOM!

int main(int argc, char **argv) {
    ros::init(argc, argv, "navigation");
    ros::NodeHandle n;
    ros::Subscriber odom_sub = n.subscribe("/odom", 10, odomCallback);
    ros::Subscriber waypoint_sub = n.subscribe("/waypoint/goal", 10, waypointCallback);
    ros::Publisher twist_pub = n.advertise<geometry_msgs::Twist>("/cmd_vel", 10);
    ros::Publisher state_pub = n.advertise<geometry_msgs::Vector3>("/bot_state/pose2d", 10);
    ros::Publisher error_pub = n.advertise<harris_chase_object::DistanceAngle>("/bot_state/to_goal", 10);
    ros::Rate loop_rate(5);


    twist_vel.linear.x = 0;
    twist_vel.linear.y = 0;
    twist_vel.linear.z = 0;

    twist_vel.angular.x = 0;
    twist_vel.angular.y = 0;
    twist_vel.angular.z = 0;


//    goal.position.x = 0;
//    goal.position.y = 0;

    while (ros::ok()) {

        twist_pub.publish(twist_vel);
        state_pub.publish(bot_state);
        error_pub.publish(to_goal);
        ROS_INFO("Publishing cmd_vel");
        ros::spinOnce();
        loop_rate.sleep();
    }
    ros::spin();

    return 0;
}

