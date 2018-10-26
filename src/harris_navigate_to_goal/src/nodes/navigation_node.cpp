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
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>

#include <harris_navigate_to_goal/objectLocation.h>



#include <harris_navigate_to_goal/DistanceAngle.h>

#include "math.h"

#include "../../../harris_navigate_to_goal/src/objects/pid_turtlebot.cpp"

#include<tf/transform_listener.h>

geometry_msgs::Pose bot_pose;
geometry_msgs::Twist bot_vels;
geometry_msgs::Pose goal;

geometry_msgs::Twist twist_vel;

geometry_msgs::Vector3 bot_state;

harris_chase_object::DistanceAngle to_goal;

ros::Timer bypass_timer;
ros::Time bypass_start;
ros::Time now;

double x_p;
double y_p;
double theta_p;
double object_dis;
double object_ang;

bool init = false;
bool isAvoid = false;
bool isBypass = false;
double bypass_dis;
double bypass_ang;
double bypass_refx;
double bypass_refy;
double bypass_refang;
bool bypassing = false;

double avoid_region = 0.2;
double bypass_region = 0.75;

double avoid_dis;

void obstacleCallback(const harris_navigate_to_goal::objectLocation& obstacle_msg)
{
    if (obstacle_msg.angle_min < M_PI/6 || obstacle_msg.angle_max > 5*M_PI/6) {


        if (obstacle_msg.distance < avoid_region) {
            isAvoid = true;
            isBypass = false;
            avoid_dis = obstacle_msg.distance;
        } else if (obstacle_msg.distance > avoid_region && obstacle_msg.distance < bypass_region) {
            isAvoid = false;
            isBypass = true;
            bypass_dis = obstacle_msg.distance;
            bypass_ang = max(abs(obstacle_msg.angle_min), abs(obstacle_msg.angle_max));
            if (!bypassing)
            {
                bypass_refx = bot_state.x;
                bypass_refy = bot_state.y;
                bypass_refang = bot_state.z;
            }

        } else {
            isAvoid = false;
            isBypass = false;
        }
    }
}

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
}

void waypointCallback(const geometry_msgs::Pose& goal_msg)
{
        goal.position.x = goal_msg.position.x;
        goal.position.y = goal_msg.position.y;
}


void update_vels()
{

    if (isAvoid)
    {
        double atune = 0.1;
        double btune = 5.0;

        double avoid_error = object_dis - avoid_region;
        double kp = (1/abs(avoid_error)*(atune/(pow(abs(avoid_error),2)+btune)));

        PID pid_linear = PID(0.1, 1.5, -1.5, kp, 0.05, 0);
        twist_vel.linear.x = pid_linear.calculate(avoid_error);
    }
    else if (isBypass || bypassing)
    {
        if (!bypassing)
        {
            bypassing = true;
            bypass_start = ros::Time::now();
        }
        now = ros::Time::now();
        if ((now.toSec() - bypass_start.toSec() > 5.0))
        {
            bypassing = false;
        }


        double xerror;
        double yerror;
        double thetaerror;
        double distance_error;


        if (distance_error < 0.05)
        {
            bypassing = false;
        } else {

            to_goal.distance = bypass_dis - sqrt(pow((bot_state.x - bypass_refx),2)+pow((bot_state.y-bypass_refy),2));
            if (object_ang > M_PI)
            {
                object_ang = 2*M_PI-object_ang;
            }
            else
            {
                object_ang = -object_ang;
            }
            to_goal.angle = (object_ang) - (bot_state.z - bypass_refang);

            distance_error = to_goal.distance;
            thetaerror = to_goal.angle;

            double atune = 0.2;

            double kp = ((1 - exp(-atune * pow(abs(distance_error), 2))) / abs(distance_error));

            PID pid_linear = PID(0.1, 1.5, -1.5, kp, 0.05, 0);
            PID pid_angular = PID(0.1, 2.0, -2.0, 0.35, 0.1, 0.4);

            if (abs(to_goal.angle) > M_PI / 16) {
                twist_vel.linear.x = 0;
                twist_vel.angular.z = pid_angular.calculate(thetaerror);
            } else {
                twist_vel.angular.z = pid_angular.calculate(thetaerror);
                twist_vel.linear.x = pid_linear.calculate(distance_error);
            }
        }

    } else {
        double xerror;
        double yerror;
        double thetaerror;
        double distance_error;


        xerror = goal.position.x - bot_state.x;
        yerror = goal.position.y - bot_state.y;
        thetaerror = (atan2(yerror, xerror) - bot_state.z);
        distance_error = sqrt(pow(xerror, 2) + pow(yerror, 2));

        if (thetaerror > M_PI) {
            thetaerror = -M_PI + (thetaerror - M_PI);
        } else if (thetaerror < -M_PI) {
            thetaerror = M_PI + (thetaerror + M_PI);
        } else {
            thetaerror = thetaerror;
        }

        to_goal.distance = distance_error;
        to_goal.angle = thetaerror;

        double atune = 0.5;

        double kp = ((1 - exp(-atune * pow(abs(distance_error), 2))) / abs(distance_error));

        PID pid_linear = PID(0.1, 1.5, -1.5, kp, 0.05, 0);
        PID pid_angular = PID(0.1, 2.0, -2.0, 0.35, 0.1, 0.4);

        if (abs(to_goal.angle) > M_PI / 16) {
            twist_vel.linear.x = 0;
            twist_vel.angular.z = pid_angular.calculate(thetaerror);
        } else {
            twist_vel.angular.z = pid_angular.calculate(thetaerror);
            twist_vel.linear.x = pid_linear.calculate(distance_error);
        }
    }
}




int main(int argc, char **argv) {
    ros::init(argc, argv, "navigation");
    ros::NodeHandle n;
    ros::Subscriber odom_sub = n.subscribe("/odom", 10, odomCallback);
    ros::Subscriber waypoint_sub = n.subscribe("/waypoint/goal", 10, waypointCallback);
    ros::Publisher twist_pub = n.advertise<geometry_msgs::Twist>("/cmd_vel", 10);
    ros::Publisher state_pub = n.advertise<geometry_msgs::Vector3>("/bot_state/pose2d", 10);
    ros::Publisher error_pub = n.advertise<harris_chase_object::DistanceAngle>("/bot_state/to_goal", 10);
    ros::Subscriber obstacle_pub = n.subscribe("/obstacle", 10, obstacleCallback);
    ros::Rate loop_rate(5);


    twist_vel.linear.x = 0;
    twist_vel.linear.y = 0;
    twist_vel.linear.z = 0;

    twist_vel.angular.x = 0;
    twist_vel.angular.y = 0;
    twist_vel.angular.z = 0;


//    goal.position.x = 0;
//    goal.position.y = 0;

    tf::TransformListener listener;


    while (ros::ok()) {

        try{
            listener.lookupTransform("/turtle2", "/turtle1",
                                     ros::Time(0), transform);
        }
        catch (tf::TransformException &ex) {
            ROS_ERROR("%s",ex.what());
            ros::Duration(1.0).sleep();
            continue;
        }

        update_vels();
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

