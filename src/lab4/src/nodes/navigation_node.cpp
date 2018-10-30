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

#include <harris_navigate_to_goal/objectLocation.h>



#include <harris_navigate_to_goal/DistanceAngle.h>

#include "math.h"

#include "../../../harris_navigate_to_goal/src/objects/pid_turtlebot.cpp"

#include<tf/transform_listener.h>

geometry_msgs::Pose bot_pose;
geometry_msgs::Twist bot_vels;
geometry_msgs::Pose goal;
geometry_msgs::Pose bypass_goal;
geometry_msgs::Pose bypass_goal_local;


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
bool objectinway = false;

bool gotpoint = false;

bool init = false;
bool odominit = false;
bool isAvoid = false;
bool isBypass = false;
double bypass_dis;
double bypass_ang;
double bypass_refx;
double bypass_refy;
double bypass_refang;
double reset_x;
double reset_y;
double reset_ang;
bool bypassing = false;
bool wallgone = false;
bool following = false;
ros::Timer followTimer;

double avoid_region = 0.20;
double bypass_region = 0.5;

double objx;
double objy;
double objang_min;
double objang_max;

bool objectfollowing = false;
bool obstacleexists = false;

double obstacle_x;
double obstacle_y;

double homeref_dis;

double avoid_dis;


//void update_object()
//{
//    double xgoalerror;
//    double ygoalerror;
//    double thetagoalerror;
//    double goal_dis_error;
//
//
//    xgoalerror = goal.position.x - objx;
//    ygoalerror = goal.position.y - objy;
//    thetagoalerror = (atan2(ygoalerror, xgoalerror) - bot_state.z);
//    goal_dis_error = sqrt(pow(xgoalerror, 2) + pow(ygoalerror, 2));
//
//    if (thetagoalerror > M_PI) {
//        thetagoalerror = -M_PI + (thetagoalerror - M_PI);
//    } else if (thetagoalerror < -M_PI) {
//        thetagoalerror = M_PI + (thetagoalerror + M_PI);
//    } else {
//        thetagoalerror = thetagoalerror;
//    }
//
////    if (0.75 > to_goal.distance)
////    {
////        objectinway = false;
////    }
//}


void obstacleCallback(const harris_navigate_to_goal::objectLocation& obstacle_msg) {


    // Checking if object is in the way
    if (obstacle_msg.distance < bypass_region && obstacle_msg.distance > avoid_region)
    {
        obstacleexists = true;
        isAvoid = false;
        if (min(abs(obstacle_msg.angle_min),abs(obstacle_msg.angle_max)) < M_PI / 6)
        {
            objectinway = true;
            if (!isBypass)
            {
                homeref_dis = to_goal.distance;
                isBypass = true;
                obstacle_x = bot_state.x + obstacle_msg.distance * cos(obstacle_msg.angle_min + bot_state.z);
                obstacle_y = bot_state.y + obstacle_msg.distance * sin(obstacle_msg.angle_min + bot_state.z);
            }
        }

    } else if (obstacle_msg.distance < avoid_region)
    {
        isAvoid = true;
        isBypass = false;
    }
    else{
        objectinway = false;
        obstacleexists = false;
    }

//    object_dis = obstacle_msg.distance;
//    object_ang = obstacle_msg.angle_min;


//    object_dis = obstacle_msg.distance;
//    object_ang = (obstacle_msg.angle_max+obstacle_msg.angle_min)/2;
//
//    if (bypass_refang + object_ang > 2*M_PI)
//    {
//        objang = bypass_refang + object_ang - 2*M_PI;
//    } else
//    {
//        objang = bypass_refang + object_ang;
//    }

//    objx = bypass_refx + obstacle_msg.distance * sin(objang);
//    objy = bypass_refy + obstacle_msg.distance * cos(objang);

//    update_object();


//    if (obstacle_msg.angle_min < M_PI/6 || obstacle_msg.angle_max > 5*M_PI/6) {
//        if (obstacle_msg.distance < avoid_region) {
//            isAvoid = true;
//            isBypass = false;
//            avoid_dis = obstacle_msg.distance;
//        } else if (obstacle_msg.distance > avoid_region && obstacle_msg.distance < bypass_region) {
//            isAvoid = false;
//            isBypass = true;
//            bypass_dis = obstacle_msg.distance;
//            bypass_ang = max(abs(obstacle_msg.angle_min), abs(obstacle_msg.angle_max));
//            if (!bypassing)
//            {
//                bypass_refx = bot_state.x;
//                bypass_refy = bot_state.y;
//                bypass_refang = bot_state.z;
//            }
//
//        } else {
//            isAvoid = false;
//            isBypass = false;
//        }
//    }
}

void update_goal()
{
    double xgoalerror;
    double ygoalerror;
    double thetagoalerror;
    double goal_dis_error;


    xgoalerror = goal.position.x - bot_state.x;
    ygoalerror = goal.position.y - bot_state.y;
    thetagoalerror = (atan2(ygoalerror, xgoalerror) - bot_state.z);
    goal_dis_error = sqrt(pow(xgoalerror, 2) + pow(ygoalerror, 2));

    if (thetagoalerror > M_PI) {
        thetagoalerror = -M_PI + (thetagoalerror - M_PI);
    } else if (thetagoalerror < -M_PI) {
        thetagoalerror = M_PI + (thetagoalerror + M_PI);
    } else {
        thetagoalerror = thetagoalerror;
    }

    to_goal.distance = goal_dis_error;
    to_goal.angle = thetagoalerror;
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
//    if (!odominit)
//    {
//        // do this
//        reset_x = bot_state.x;
//        reset_y = bot_state.y;
//        reset_ang = bot_state.z;
//        odominit = true;
//    }
//
//    bot_state.x = bot_state.x - reset_x;
//    bot_state.y = bot_state.x - reset_x;
//    bot_state.z = bot_state.z - reset_ang;
//    if (bot_state.z > 2*M_PI)
//    {
//        bot_state.z = 2*M_PI - bot_state.z;
//    }
//    else if (bot_state.z < 0)
//    {
//        bot_state.z = 2*M_PI + bot_state.z;
//    }

    update_goal();
}

void waypointCallback(const geometry_msgs::Pose& goal_msg)
{
        goal.position.x = goal_msg.position.x;
        goal.position.y = goal_msg.position.y;
        if (!gotpoint)
        {
            gotpoint = true;
        }
}


//void update_vels()
//{
//
//    if (isAvoid)
//    {
//        double atune = 0.1;
//        double btune = 5.0;
//
//        double avoid_error = object_dis - avoid_region;
//        double kp = (1/abs(avoid_error)*(atune/(pow(abs(avoid_error),2)+btune)));
//
//        PID pid_linear = PID(0.1, 1.5, -1.5, kp, 0.05, 0);
//        twist_vel.linear.x = pid_linear.calculate(avoid_error);
//    }
//    else if (isBypass || bypassing)
//    {
//        if (!bypassing)
//        {
//            bypassing = true;
//            bypass_start = ros::Time::now();
//        }
//        now = ros::Time::now();
//        if ((now.toSec() - bypass_start.toSec() > 5.0))
//        {
//            bypassing = false;
//        }
//
//
//        double xerror;
//        double yerror;
//        double thetaerror;
//        double distance_error;
//
//
//        if (distance_error < 0.05)
//        {
//            bypassing = false;
//        } else {
//
//            to_goal.distance = bypass_dis - sqrt(pow((bot_state.x - bypass_refx),2)+pow((bot_state.y-bypass_refy),2));
//            if (object_ang > M_PI)
//            {
//                object_ang = 2*M_PI-object_ang;
//            }
//            else
//            {
//                object_ang = -object_ang;
//            }
//            to_goal.angle = (object_ang) - (bot_state.z - bypass_refang);
//
//            distance_error = to_goal.distance;
//            thetaerror = to_goal.angle;
//
//            double atune = 0.2;
//
//            double kp = ((1 - exp(-atune * pow(abs(distance_error), 2))) / abs(distance_error));
//
//            PID pid_linear = PID(0.1, 1.5, -1.5, kp, 0.05, 0);
//            PID pid_angular = PID(0.1, 2.0, -2.0, 0.35, 0.1, 0.4);
//
//            if (abs(to_goal.angle) > M_PI / 16) {
//                twist_vel.linear.x = 0;
//                twist_vel.angular.z = pid_angular.calculate(thetaerror);
//            } else {
//                twist_vel.angular.z = pid_angular.calculate(thetaerror);
//                twist_vel.linear.x = pid_linear.calculate(distance_error);
//            }
//        }
//
//    } else {
//
//    }
//}


void cloudCallback(const sensor_msgs::PointCloud& cloud_msg)
{
    // find max y
    // add 0.35 spacing to y
    // set point to be transformed in tfCallback

    bypass_goal_local.position.x = cloud_msg.points[0].x;
    bypass_goal_local.position.y = cloud_msg.points[0].y;
}

double distance(double x1,double y1, double x2, double y2)
{
    double  xdif = x1 - x2;
    double ydif = y1 - y2;
    double distance = sqrt(pow(xdif, 2) + pow(ydif, 2));
    return distance;
}

double heading(double x1, double y1, double x2, double y2)
{
    double  xdif = x1 - x2;
    double ydif = y1 - y2;
    double theta = atan2(ydif, xdif);
    return theta;
}

void cloudglobalCallback(const sensor_msgs::PointCloud& cloud_msg)
{
    // Check if points between goal and current position
    int i;
    for (i=0; i < cloud_msg.points.size(); i++) {
        double object_distance_to_goal = distance(cloud_msg.points[i].x, cloud_msg.points[i].y, goal.position.x,
                                                  goal.position.y);
        double object_distance_to_bot = distance(cloud_msg.points[i].x, cloud_msg.points[i].y,
                bot_state.x, bot_state.y);

    }
}




void tfCallback(const geometry_msgs::Pose& tf_msg)
{
//    tf::Transform tf
//    bypass_goal.position.x = bypass_goal_local.position.x * tf_msg.
}



// Defining states
enum State
{
    WAYPOINT = 1,
    GOTOGOAL = 2,
    AVOID = 3,
    BYPASS = 4,
    FOLLOWWALL = 5,
    REST = 6
};

State status(State botstate)
{
    switch(botstate)
    {
        case WAYPOINT:
        {
            if (isAvoid)
            {
                botstate = AVOID;
            } else if (isBypass)
            {
                botstate = FOLLOWWALL;
            }
            return botstate;
        }
        case FOLLOWWALL:
        {
            if (!isBypass)
            {
                botstate = WAYPOINT;
            }
            else if (isAvoid)
            {
                botstate = AVOID;
            }
            return botstate;
        }
        case AVOID:
        {
            if (!isAvoid)
            {
                botstate = FOLLOWWALL;
            }
            return botstate;
        }
        case REST:
        {
            if (gotpoint)
            {
                botstate = WAYPOINT;
                return botstate;
            }
          return botstate;
        }
    }
}



void move(State botstate)
{
    switch(botstate)
    {
        case WAYPOINT:
        {
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

            double atune = 0.8;

            double kp = ((1 - exp(-atune * pow(abs(distance_error), 2))) / abs(distance_error));

            PID pid_linear = PID(0.2, 1.5, -1.5, kp, 0.05, 0);
            PID pid_angular = PID(0.2, 2.0, -2.0, 0.35, 0.1, 0.4);

            if (abs(to_goal.angle) > M_PI / 16) {
                twist_vel.linear.x = 0;
                twist_vel.angular.z = pid_angular.calculate(thetaerror);
            } else {
                twist_vel.angular.z = pid_angular.calculate(thetaerror);
                twist_vel.linear.x = pid_linear.calculate(distance_error);
            }
            return;
        }
        case FOLLOWWALL:
        {
            double xerror;
            double yerror;
            double thetaerror;
            double distance_error;
            double distance_obj_to_goal;

            distance_error = distance(goal.position.x, goal.position.y,bot_state.x, bot_state.y);
            distance_obj_to_goal = distance(goal.position.x, goal.position.y,obstacle_x, obstacle_y);
            thetaerror = heading(obstacle_x, obstacle_y, bot_state.x, bot_state.y) - bot_state.z + M_PI/2;

            if (thetaerror > M_PI) {
                thetaerror = -M_PI + (thetaerror - M_PI);
            } else if (thetaerror < -M_PI) {
                thetaerror = M_PI + (thetaerror + M_PI);
            } else {
                thetaerror = thetaerror;
            }


//            xerror = goal.position.x - bot_state.x;
//            yerror = goal.position.y - bot_state.y;
//            distance_error = sqrt(pow(xerror, 2) + pow(yerror, 2));
//
//            if (object_ang > 0)
//            {
//                thetaerror = M_PI/2 - abs(object_ang);
//            }
//            else if (object_ang < 0)
//            {
//                thetaerror = -(M_PI/2 - abs(object_ang));
//            } else
//            {
//                thetaerror =  M_PI;
//            }

//                thetaerror = abs(bot_state.z - bypass_refang);

//            if (object_ang > M_PI)
//            {
//                thetaerror = (object_ang - 3*M_PI/2);
//            }
//            else
//            {
//                thetaerror = (object_ang - M_PI/2);
//            }

            double atune = 0.2;

            double kp = ((1 - exp(-atune * pow(abs(distance_error), 2))) / abs(distance_error));

            PID pid_linear = PID(0.2, 1.5, -1.5, 0.15, 0.05, 0);
            PID pid_angular = PID(0.2, 2.0, -2.0, 0.25, 0.05, 0.1);

            if (abs(thetaerror) > M_PI / 8) {
                twist_vel.linear.x = 0;
                twist_vel.angular.z = pid_angular.calculate(thetaerror);
            } else {
                twist_vel.angular.z = pid_angular.calculate(thetaerror);
                twist_vel.linear.x = pid_linear.calculate(distance_error);
            }

            if (to_goal.distance < distance_obj_to_goal)
            {
                objectfollowing = false;
                objectinway = false;
            }
            else
            {
                objectfollowing = true;
            }

            if (!objectinway || !obstacleexists)
            {
                isBypass = false;
            }


            return;

        }
        case AVOID:
        {
            double avoid_error = (object_dis - bypass_region);

            double atune = 0.5;
            double btune = 5.0;
            double kp = (1/abs(avoid_error)*(atune/(pow(abs(avoid_error),2)+btune)));

            PID pid_linear = PID(0.1, 1.5, -1.5, 0.5, 0.15, 0);
            twist_vel.linear.x = pid_linear.calculate(avoid_error);
            twist_vel.angular.z = 0;

            if (object_dis > avoid_region)
            {
                isAvoid = false;
            }


            return;
            }
        case REST:
        {
            twist_vel.linear.x = 0;
            twist_vel.angular.z = 0;
            to_goal.distance = 0;
            to_goal.angle = 0;
            return;
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
    ros::Subscriber obstacle_sub = n.subscribe("/obstacle", 10, obstacleCallback);
//    ros::Subscriber cloud_sub = n.subscribe("/local_cloud",1,cloudCallback);
//    ros::Subscriber cloudglobal_sub = n.subscribe("/global_cloud",1,cloudglobalCallback);
//    ros::Subscriber tf_sub = n.subscribe("/transformation", 10, tfCallback);
    ros::Rate loop_rate(5);

    twist_vel.linear.x = 0;
    twist_vel.linear.y = 0;
    twist_vel.linear.z = 0;

    twist_vel.angular.x = 0;
    twist_vel.angular.y = 0;
    twist_vel.angular.z = 0;

    State nav_mode = REST;

    while (ros::ok()) {

//        update_vels();
        nav_mode = status(nav_mode);
        move(nav_mode);
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

