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

class BotImpl;
class Bot
{
public:
    // public parameters
    // State Information, current state
    Bot(double xp, double yp, double theta, double xdot, double ydot, double thetadot, int state);

    // Functions
    void setState(int newstate);
    void goToGoal(double xd, double yd);
    // ...
    ~Bot();

private:
    BotImpl *pimpl;
};

#endif //HARRIS_BALL_FOLLOWER_BOT_H
