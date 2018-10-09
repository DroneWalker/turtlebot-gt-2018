//
// Created by charris on 10/2/18.
//

#include "ros/ros.h"
#include <sstream>
#include "std_msgs/String.h"

#include "../objects/bot.cpp"

using namespace std;
using namespace ros;

std_msgs::String myState;

int main(int argc, char **argv)
{
//    enum States
//    {
//        IDLE,
//        GOTOGOAL,
//        FOLLOWWALLC,
//        FOLLOWWALLCC,
//        AVOID,
//        FOLLOW // For lab #3
//    };

//BotImpl Bot = Bot(0,0,0,0,0,0,)



    init(argc, argv, "commander");
    NodeHandle n;
    Publisher state_pub = n.advertise<std_msgs::String>("/commander/state", 1);
    Rate loop_rate(10);

    // Temp
    myState.data = "Current State: Idle";

    while(ros::ok())
    {
        state_pub.publish(myState);
        // bot.update()
        ros::spinOnce();
        loop_rate.sleep();
    }
    ros::spin();

    return 0;
}




