//
// Created by charris on 10/2/18.
//

#include "ros/ros.h"
#include <sstream>
#include "std_msgs/String.h"
#include "geometry_msgs/Point.h"
#include "geometry_msgs/PoseStamped.h"
#include "geometry_msgs/Twist.h"
#include "sensor_msgs/LaserScan.h"

#include "math.h"

#include "../objects/bot.cpp"
#include "../objects/object.cpp"
#include <harris_navigate_to_goal/DistanceAngle.h>
#include <harris_navigate_to_goal/objectLocation.h>





using namespace std;
using namespace ros;

std_msgs::String myState;
geometry_msgs::PoseStamped desired_pos;
geometry_msgs::PoseStamped ballState;
double pi = M_PI;

vector <Object *> objectMap(360);
vector <geometry_msgs::Pose> waypoints;
int i = 0;  // waypoint counter
geometry_msgs::Vector3 bot_state;


bool tracker = false;
bool havegoal = false;
bool foundball = false;
bool objectexists = false;
bool isAvoid = false;
bool isBypass = false;
double camera_fov_x = 66.2;
double camera_resolution_x = 320;

double ballAngle;
float ballDist;

double safezone = 0.35;
double avoid_region = 0.5;
double bypass_region = 1.0;


// Defining states
enum State
{
    IDLE = 1,
    GOTOGOAL = 2,
    FOLLOWWALLC = 3,
    FOLLOWWALLCC = 4,
    AVOID = 5,
    FOLLOW = 6,
    BYPASS = 7
};



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
    if (tracker && objectexists)
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

void getMission()
{
    // Mission 1
    geometry_msgs::Pose wp1;  wp1.position.x = 1.4; wp1.position.y = 0.0;
    geometry_msgs::Pose wp2;  wp2.position.x = 1.4; wp2.position.y = 1.5;
    geometry_msgs::Pose wp3;  wp3.position.x = 0.0; wp3.position.y = 1.5;

    waypoints.push_back(wp1); waypoints.push_back(wp2); waypoints.push_back(wp3);

    havegoal = true;

    // Mission 2

}

void publishGoal(ros::Publisher wp_pub)

{
    geometry_msgs::Pose waypoint = waypoints[i];
    wp_pub.publish(waypoint);

    double xerror = waypoint.position.x - bot_state.x;
    double yerror = waypoint.position.y - bot_state.y;
    double distance_error = sqrt(pow(xerror,2) + pow(yerror,2));

    if (distance_error < 0.15)
    {
        if (waypoints.size() == 1)
        {
            havegoal == false;
        }
        else
        {
            waypoints.erase(waypoints.begin());
        }
        ROS_INFO("Waypoint completed!");
        ros::Duration(2).sleep();
        ROS_INFO("Next waypoint loaded");
    }
//    }

}

geometry_msgs::Pose findWaypoint(float dis, double angmin, double angmax)
{
    geometry_msgs::Pose wp;
    double obj_ang;

    if (angmin == 0.0)
    {
        obj_ang = angmax;
    }
    else if (angmax == 0.0)
    {
        obj_ang = angmin;
    }
    else
    {
        obj_ang = min(angmin, angmax);
    }

    double obj_ang_global = bot_state.z - obj_ang;
    double xp = dis*cos(obj_ang_global) + safezone/2 * sin(obj_ang_global);
    double yp = dis*sin(obj_ang_global) + safezone/2 * cos(obj_ang_global);

    wp.position.x = xp;
    wp.position.y = yp;
}

//void obstacleCallback(const harris_navigate_to_goal::objectLocation& obstacle_msg)
//{
//    if (obstacle_msg.distance < avoid_region & isAvoid)
//    {
//        isAvoid = true;
//        isBypass = false;
//        geometry_msgs::Pose new_wp;
//        new_wp = findWaypoint(obstacle_msg.distance, obstacle_msg.angle_min, obstacle_msg.angle_max);
//        waypoints.push_back(new_wp);
//    }
//    else if (obstacle_msg.distance < bypass_region & !isBypass)
//    {
//        isAvoid = false;
//        isBypass = true;
//        geometry_msgs::Pose new_wp;
//        new_wp = findWaypoint(obstacle_msg.distance, obstacle_msg.angle_min, obstacle_msg.angle_max);
//        waypoints.push_back(new_wp);
//    }
//}

void botposeCallback(const geometry_msgs::Vector3& pose_msg)
{
    bot_state.x = pose_msg.x;
    bot_state.y = pose_msg.y;
    bot_state.z = pose_msg.z;
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



State stateUpdate(State botState)
{
    switch(botState)
    {
        case IDLE:
        {
            myState.data= "idle";
            // At Start
            // If no detection
            if (waypoints.size() > 1)
            {
                havegoal = true;
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
            if (!havegoal)
            {
                botState = IDLE;
                myState.data = "idle";
            } else
            {
                myState.data="gotogoal";

            }
            return botState;
        }
        case FOLLOW:
        {
            myState.data="follow";
            if (!foundball)
            {
                botState = IDLE;
            }
            else if (havegoal)
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
    Subscriber trackpoint_sub = n.subscribe("trackpoint", 10, pointCallback);
    Publisher goal_pub = n.advertise<geometry_msgs::Pose>("/waypoint/goal", 100);
    Subscriber odom_pub = n.subscribe("/bot_state/pose2d", 10, botposeCallback);
//    Subscriber obstacle_pub = n.subscribe("/obstacle", 10, obstacleCallback);

    getMission();

    // Running Hz Rate
    Rate loop_rate(10);

    State turtlebotState = IDLE;
    harris_chase_object::DistanceAngle ball_distance_angle;

    double cnt = 0;
    ros::Timer timer;
    timer.start();

    while(ros::ok())
    {
        // Check measurements
//        ROS_INFO("Running sensor fusion and filtering");
        sensorfusion();
        turtlebotState = stateUpdate(turtlebotState);
        state_pub.publish(myState);

        foundball = true;

        if (turtlebotState == FOLLOW)
        {
            ball_distance_angle.distance = ballDist;
//            ball_distance_angle.distance = 1.5;
//            ball_distance_angle.distance = 1.1;
            ball_distance_angle.angle = ballAngle;
            object_pub.publish(ball_distance_angle);
        }
        else if (turtlebotState == GOTOGOAL)
        {
            publishGoal(goal_pub);
        }
        else
        {
//            ball_distance_angle.distance = 0.0;
//            ball_distance_angle.distance = 0.0;
//            object_pub.publish(ball_distance_angle);
        }



        ros::spinOnce();
        loop_rate.sleep();
    }
    cnt += 1;
    ros::spin();

    return 0;
}




