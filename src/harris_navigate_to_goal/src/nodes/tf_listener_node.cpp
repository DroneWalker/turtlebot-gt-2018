//
// Created by charris on 10/26/18.
//

#include "tf_listener_node.h"


#include <ros/ros.h>
#include <tf/transform_listener.h>
#include <geometry_msgs/Twist.h>
#include <turtlesim/Spawn.h>

int main(int argc, char** argv){
    ros::init(argc, argv, "my_tf_listener");

    ros::NodeHandle node;
    ros::Publisher tf_pub = node.advertise<geometry_msgs::Pose>("/transformation",10);

    tf::TransformListener listener;

    ros::Rate rate(10.0);
    while (node.ok()){
        tf::StampedTransform transform;
        geometry_msgs::Pose transformation;
        try{
            listener.lookupTransform("/odom", "/base_scan",
                                     ros::Time(0), transform);
        }
        catch (tf::TransformException &ex) {
            ROS_ERROR("%s",ex.what());
            ros::Duration(1.0).sleep();
            continue;
        }
        transformation.position.x = transform.getOrigin().x();
        transformation.position.y = transform.getOrigin().y();
        transformation.orientation.x = transform.getRotation().x();
        transformation.orientation.y = transform.getRotation().y();
        transformation.orientation.z = transform.getRotation().z();
        transformation.orientation.w = transform.getRotation().w();

        tf_pub.publish(transformation);

        rate.sleep();
    }
    return 0;
};