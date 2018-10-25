//
// Created by charris on 10/23/18.
//

#include "detect_objects_node.h"

#include "ros/ros.h"
#include <sstream>
#include "std_msgs/String.h"
#include "geometry_msgs/PoseStamped.h"
#include "sensor_msgs/LaserScan.h"

#include "math.h"

#include "../objects/bot.cpp"
#include "../objects/object.cpp"
#include <harris_navigate_to_goal/DistanceAngle.h>


using namespace std;
using namespace ros;

std_msgs::String myState;
geometry_msgs::PoseStamped desired_pos;
geometry_msgs::PoseStamped ballState;
double pi = M_PI;

vector <Object *> objectMap(99);

bool objectexists = true;



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

    float obj_dist;
    float obj_sum = 0;
    double cnt=0;
    for (i= 0; i < length; i++)
    {
        if (i > 330 || i < 30)
        {
            float dist = scan_msg->ranges[i];
            if (dist > 0.05 && dist < 3.0)
            {
                obj_sum += dist;
                cnt += 1;
            }
        }
    }

    if (cnt == 0)
    {
        obj_dist = 0;
    } else
    {
        obj_dist = obj_sum / (cnt);
    }
    object_count += 1;
    Object *obj = new Object(object_count, obj_dist, degtorads((double) start), degtorads((double) end));
    objectMap.push_back(obj);
    objectexists = true;
    ROS_INFO("Object Found!");



//            for (i= 0; i < length; i++)
//            {
//            if (lever)
//            {
//                // Looking for Start
//                if (dist > 0.2 && dist < 3.0)
//                {
//                    start = i;
//                    lever = false;
//                    inLoop = true;
//                }
//            }
//            else
//            {
//                if (dist > 3.0 && inLoop)
//                {
//                    end = i-1;
//                    lever = true;
//                    inLoop = false;
//                    float obj_dist;
//                    float obj_sum = 0;
//                    int k;
//                    for (k=start;k < end+1; k++)
//                    {
//                        obj_sum += scan_msg->ranges[k];
//                    }
//                    obj_dist = obj_sum / (end - start);
//                    object_count += 1;
////                objectMap[object_count-1]->o Object(object_count, obj_dist, degtorads((double) start), degtorads((double) end));
//                    Object *obj = new Object(object_count, obj_dist, degtorads((double) start), degtorads((double) end));
//                    objectMap.push_back(obj);
//                    objectexists = true;
//                    ROS_INFO("Object Found!");
//                }
//            }
//        }

//    }
}

int main(int argc, char **argv)
{
    Bot turtlebot = Bot(0, 0, 0, 0, 0, 0);

    init(argc, argv, "object_detection");
    NodeHandle n;
    Publisher object_pub = n.advertise<harris_chase_object::DistanceAngle>("/obstacle",100);
    Subscriber scanner_sub = n.subscribe("/scan", 10, scanCallback);

    // Running Hz Rate
    Rate loop_rate(10);

    harris_chase_object::DistanceAngle object_location;

    ros::Timer timer;
    timer.start();

    while(ros::ok())
    {
        if (!objectMap.empty())
        {
            object_location.angle = objectMap[0]->getAngleMin();
            object_location.distance = objectMap[0]->getDistance();
        }
        object_pub.publish(object_location);
        ros::spinOnce();
        loop_rate.sleep();
    }
    ros::spin();

    return 0;
}