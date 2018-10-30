//
// Created by charris on 10/1/18.
//

#ifndef HARRIS_BALL_FOLLOWER_BOT_H
#define HARRIS_BALL_FOLLOWER_BOT_H

//https://www.codeproject.com/Articles/1087619/State-Machine-Design-in-Cplusplus

/*
 * The Bot class is defined to give the turtlebot an internal object
 * where sensor fusion, high level planning, and state estimation can be done
 *
 * Current bot - Turtlebot3 Burger
 */

#include <sstream>

using namespace std;

//struct state
//{
//    int id;
//    string name;
//};

class BotImpl;
class Bot
{

public:
    // public parameters
    // State Information, current state
    Bot(double xp, double yp, double theta, double xdot, double ydot, double thetadot);

    // Functions
    void setGoal(double x_goal, double y_goal, double theta_goal);
    void updateSystem(double x, double y, double theta, double xdot, double ydot, double thetadot);
    void updateError(double x_err, double y_err, double theta_err, double xdot_err, double ydot_err, double thetadot_err);


    // ...
    ~Bot();



private:
    BotImpl *pimpl;
};

#endif //HARRIS_BALL_FOLLOWER_BOT_H
