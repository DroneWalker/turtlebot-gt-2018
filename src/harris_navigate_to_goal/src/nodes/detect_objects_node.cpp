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
#include <harris_navigate_to_goal/objectLocation.h>


using namespace std;
using namespace ros;

std_msgs::String myState;
geometry_msgs::PoseStamped desired_pos;
geometry_msgs::PoseStamped ballState;
double pi = M_PI;

vector <Object *> objectMap(99);
bool objectexists = true;

harris_navigate_to_goal::objectLocation object_location;




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

//void scanCallback(const sensor_msgs::LaserScan::ConstPtr& scan_msg)
//{
//    if (!listener_.waitForTransform(scan_msg->header.frame_id,"/base_link",scan_msg->header.stamp + ros::Duration()
//    .fromSec(scan_msg->ranges.size()*scan_msg->time_increment),ros::Duration(1.0)))
//    {
//        return;
//    }
//    try
//    {
////        projector_.transformLaserScanToPointCloud("/base_link", *scan_msg, cloud,listener_);
//        projector_.projectLaser(*scan_msg, cloud);
//
//    }
//    catch (tf::TransformException& e)
//    {
//        std::cout << e.what();
//        return;
//    }
//
//
//
//}



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


    // SIMPLE DETECT*
//    for (i= 0; i < length; i++)
//    {
//        if (i > 330 || i < 30)
//        {
//            float dist = scan_msg->ranges[i];
//            if (dist > 0.05 && dist < 3.0)
//            {
//                obj_sum += dist;
//                cnt += 1;
//            }
//        }
//    }
//
//    if (cnt == 0)
//    {
//        obj_dist = 0;
//    } else
//    {
//        obj_dist = obj_sum / (cnt);
//    }
//    object_count += 1;
//    Object *obj = new Object(object_count, obj_dist, degtorads((double) start), degtorads((double) end));
//    objectMap.push_back(obj);
//    objectexists = true;
//    ROS_INFO("Object Found!");

//



    // ADVANCED DETECT

    for (i= 0; i < length; i++)
    {
            float dist = scan_msg->ranges[i];
            if (lever)
        {
            // Looking for Start
            if (dist > 0.01 && dist < 3.0)
            {
                start = i;
                lever = false;
                inLoop = true;
            }
        }
        else
        {
            if (dist > 3.0 && inLoop)
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
                objectexists = true;
                ROS_INFO("Object Found!");
            }
        }
    }

    if (!objectMap.empty())
    {
        int nearest = 0;
        float min_dist = 100;
        for (int k = 0; i < objectMap.size(); k++)
        {
            if (objectMap[k]->getAngleMin() < M_PI/6 || objectMap[k]->getAngleMax() > 5*M_PI/6)
            {
                // Within obstacle bound
                if (objectMap[k]->getDistance() < min_dist)
                {
                    nearest = k;
                    min_dist = objectMap[k]->getDistance();
                }
            }
        }
//        if (objectMap[nearest]->getAngleMin() > M_PI)
//        {
//            object_location.angle_min = objectMap[nearest]->getAngleMin() - 2*M_PI;
//        } else{
//            object_location.angle_min = objectMap[nearest]->getAngleMin();
//        }
//        if (objectMap[nearest]->getAngleMax() > M_PI)
//        {
//            object_location.angle_max = objectMap[nearest]->getAngleMax()- 2*M_PI;
//        } else{
//            object_location.angle_max = objectMap[nearest]->getAngleMax();
//        }
        object_location.angle_min = objectMap[nearest]->getAngleMin();
        object_location.angle_max = objectMap[nearest]->getAngleMax();
        object_location.distance = objectMap[nearest]->getDistance();
    } else
    {
        object_location.angle_min = 0;
        object_location.angle_max = 0;
        object_location.distance = 1000;
    }

}


int main(int argc, char **argv)
{
    Bot turtlebot = Bot(0, 0, 0, 0, 0, 0);

    init(argc, argv, "object_detection");
    NodeHandle n;
    Publisher object_pub = n.advertise<harris_navigate_to_goal::objectLocation>("/obstacle",100);
    Subscriber scanner_sub = n.subscribe("/scan", 10, scanCallback);

    // Running Hz Rate
    Rate loop_rate(10);

    ros::Timer timer;
    timer.start();

    object_location.distance = 1000;

    while(ros::ok())
    {
        object_pub.publish(object_location);
        ros::spinOnce();
        loop_rate.sleep();
    }
    ros::spin();

    return 0;
}