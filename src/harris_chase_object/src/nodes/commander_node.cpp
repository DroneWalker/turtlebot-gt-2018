//
// Created by charris on 10/2/18.
//

#include "ros/ros.h"
#include <sstream>
#include "std_msgs/String.h"
#include "geometry_msgs/Point.h"
#include "geometry_msgs/PoseStamped.h"
#include "sensor_msgs/LaserScan.h"

#include "math.h"

#include "../objects/bot.cpp"
#include "../objects/object.cpp"


using namespace std;
using namespace ros;

std_msgs::String myState;
geometry_msgs::PoseStamped desired_pos;
double pi = M_PI;

vector<Object> objectMap;
bool goal = false;
bool tracker = false;

double camera_fov_x = 66.2;
double camera_resolution_x = 640;

double ballAngle;


double degtorads(double deg)
{
    double rads = (deg/180) * pi;
    return rads;
}

double calculateAngle(double x_pixel)
{
    double pixel_deg = camera_fov_x / camera_resolution_x; // degrees per pixel
    double deg_error = x_pixel - camera_resolution_x / 2;
    double rad_error = degtorads(deg_error);
    if (rad_error < 0)
    {
        rad_error = rad_error + 2 * pi;
    }
    return rad_error;
}



void pointCallback(const geometry_msgs::Point& point_msg)
{
    ROS_INFO("Got Point!");
    ballAngle = calculateAngle(point_msg.x);
    tracker = true;
}

void scanCallback(const sensor_msgs::LaserScanPtr& scan_msg)
{
    ROS_INFO("Got Scan!");
}

void sensorfusion()
{
    // do fusion
}

Bot update(Bot bot)
{
    // do updates and dynamics and stuff
}


// Defining states
enum State
{
    IDLE = 1,
    GOTOGOAL = 2,
    FOLLOWWALLC = 3,
    FOLLOWWALLCC = 4,
    AVOID = 5,
    FOLLOW = 6 // For lab #3
};

State stateUpdate(State botState)
{
    switch(botState)
    {
        case IDLE:
        {
            myState.data= "idle";
            // At Start
            // If no detection
            if (goal == true)
            {
                botState = GOTOGOAL;
            }
            else if (tracker == true)
            {
                botState = FOLLOW;
            }
            return botState;
        }
        case  GOTOGOAL:
        {
            myState.data="gotogoal";
            return botState;
        }
        case FOLLOW:
        {
            myState.data="follow";
            return botState;
        }
    }
}


int main(int argc, char **argv)
{
    Bot turtlebot = Bot(0, 0, 0, 0, 0, 0);

    init(argc, argv, "commander");
    NodeHandle n;
    Publisher state_pub = n.advertise<std_msgs::String>("/commander/state", 1);
    Publisher des_pos_pub = n.advertise<geometry_msgs::PoseStamped>("/commander/desired_position",1);
    Subscriber trackpoint_sub = n.subscribe("trackpoint", 1, pointCallback);
    Subscriber scanner_sub = n.subscribe("/scan", 1, scanCallback);
    Rate loop_rate(10);

    // Temp
//    desired_pos.header.stamp.sec = 5;
//    desired_pos.pose.orientation.z = pi/2;
//    desired_pos.pose.position.x = 10;
//    desired_pos.pose.position.y = 10;
//    desired_pos.pose.position.z = 0;

    State turtlebotState = IDLE;

    while(ros::ok())
    {
        sensorfusion();
        turtlebotState = stateUpdate(turtlebotState);
        state_pub.publish(myState);
        turtlebot = update(turtlebot);
        ros::spinOnce();
        loop_rate.sleep();
    }
    ros::spin();

    return 0;
}




