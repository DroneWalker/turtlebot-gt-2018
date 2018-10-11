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
#include <harris_chase_object/DistanceAngle.h>


using namespace std;
using namespace ros;

std_msgs::String myState;
geometry_msgs::PoseStamped desired_pos;
geometry_msgs::PoseStamped ballState;
double pi = M_PI;

vector <Object *> objectMap(360);
bool goal = false;
bool tracker = false;
bool havegoal = false;
bool foundball = false;

double camera_fov_x = 66.2;
double camera_resolution_x = 640;

double ballAngle;
float ballDist;


double degtorads(double deg)
{
    double rads = (deg/180) * pi;
    return rads;
}

double radtodegs(double rad)
{
    double degs = (rad/pi) * 180;
    return degs;
}

double calculateAngle(double x_pixel)
{
    double pixel_deg = camera_fov_x / camera_resolution_x; // degrees per pixel
    double angle_degree = (x_pixel - camera_resolution_x / 2) * pixel_deg;
    double angle_radian = degtorads(angle_degree);
    if (angle_radian < 0)
    {
        angle_radian += 2 * pi;
    }

    return angle_radian;
}

void sensorfusion()
{
    if (tracker)
    {
        int i;
        for (i=0; i < objectMap.size(); i++)
        {
            if (ballAngle >= objectMap[i]->getAngleMin() && ballAngle <= objectMap[i]->getAngleMax())
            {
                ballDist = objectMap[i]->getDistance();
                foundball = true;
                break;
            }
            else
            {
                // nothing
            }
        }
    }
}


void pointCallback(const geometry_msgs::Point& point_msg)
{
    ROS_INFO("Got Point!");
    if (point_msg.x == 320)
    {
        ballAngle = 0;
        tracker = false;
        foundball = false;
    } else
    {
        ballAngle = calculateAngle(point_msg.x);
        tracker = true;
    }

}

void scanCallback(const sensor_msgs::LaserScanPtr& scan_msg)
{
    objectMap.clear();
    ROS_INFO("Got Scan!");
    int length = (int) scan_msg->ranges.size();
    int object_count = 0;
    int  i;
    int start = 0;
    int end = length;
    bool inLoop = false;
    bool lever = true;
    for (i= 0; i < length; i++)
    {
        float dist = scan_msg->ranges[i];
        if (lever)
        {
            // Looking for Start
            if (dist > 0.05 && dist < 10000)
            {
                start = i;
                lever = false;
                inLoop = true;
            }
        }
        else
        {
            if (dist > 10000 && inLoop)
            {
                end = i-1;
                lever = true;
                inLoop = false;
                float obj_dist;
                float obj_sum = 0;
                int k;
                for (k=start;k < end+1; k++)
                {
                    obj_sum += scan_msg->ranges[k];
                }
                obj_dist = obj_sum / (end - start);
                object_count += 1;
//                objectMap[object_count-1]->o Object(object_count, obj_dist, degtorads((double) start), degtorads((double) end));
                Object *obj = new Object(object_count, obj_dist, degtorads((double) start), degtorads((double) end));
                objectMap.push_back(obj);

            }
        }
    }
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
            if (goal)
            {
                botState = GOTOGOAL;
            }
            else if (foundball)
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
            if (!foundball)
            {
                botState = IDLE;
            }
            else if (goal)
            {
                botState = GOTOGOAL;
            }
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
    Publisher des_pos_pub = n.advertise<geometry_msgs::PoseStamped>("/commander/desired_position",100);
    Publisher object_pub = n.advertise<harris_chase_object::DistanceAngle>("/commander/ball_location",100);
    Subscriber trackpoint_sub = n.subscribe("trackpoint", 100, pointCallback);
    Subscriber scanner_sub = n.subscribe("/scan", 10, scanCallback);

    // Running Hz Rate
    Rate loop_rate(5);

    State turtlebotState = IDLE;
    harris_chase_object::DistanceAngle ball_distance_angle;

    double cnt = 0;
    ros::Timer timer;
    timer.start();

    while(ros::ok())
    {
        // Check measurements
        ROS_INFO("Running sensor fusion and filtering");
        sensorfusion();

        if (foundball)
        {
            ball_distance_angle.distance = ballDist;
            ball_distance_angle.angle = ballAngle;
            object_pub.publish(ball_distance_angle);
        }
        else if (turtlebotState == GOTOGOAL && havegoal)
        {
//            des_pos_pub.publish()
        }

        turtlebotState = stateUpdate(turtlebotState);
        state_pub.publish(myState);

        ros::spinOnce();
        loop_rate.sleep();
    }
    cnt += 1;
    ros::spin();

    return 0;
}




