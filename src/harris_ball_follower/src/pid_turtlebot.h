//
// Created by charris on 9/18/18.
//

#include "pid_plotting.h"

#ifndef HARRIS_BALL_FOLLOWER_PID_TURTLEBOT_H
#define HARRIS_BALL_FOLLOWER_PID_TURTLEBOT_H

double u;
double kp = 1.0;

double pid(double error)
{
    // Proportional controller
    u = kp * error;
    std::cout << "inside pid controller" << std::endl;
    return u;
}

// What we return -> angular velocity

#endif //HARRIS_BALL_FOLLOWER_PID_TURTLEBOT_H
