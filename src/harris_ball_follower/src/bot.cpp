//
// Created by charris on 10/2/18.
//

#ifndef _BOT_SOURCE_
#define _BOT_SOURCE_

#include "bot.h"


class BotImpl
{
public:
    BotImpl(double xp, double yp, double theta, double xdot, double ydot, double thetadot, state currentstate);
    ~BotImpl();


    void setState(state newstate);
    void goToGoal(double xd, double yd);

    enum States
    {
        IDLE = 0,
        GOTOGOAL = 1,
        FOLLOWWALLC = 2,
        FOLLOWWALLCC = 3,
        AVOID = 4,
        FOLLOW = 5 // For lab #3
    };


private:
    double _xp;
    double _yp;
    double _theta;
    double _thetadot;
    double _xdot;
    double _ydot;
    state _state;
    double _time;
    double _xd;
    double _yd;


};

Bot::Bot(double xp, double yp, double theta, double xdot, double ydot, double thetadot, state currentstate)
{
    pimpl = new BotImpl(xp, yp, theta, xdot, ydot, thetadot, currentstate);
}
void Bot::setState(state newstate)
{
    return pimpl->setState(newstate);
}
void Bot::goToGoal(double xd, double yd)
{
    return pimpl->goToGoal(xd, yd);
}
Bot::~Bot()
{
    delete pimpl;
}

/*
 * Implementation
 */

BotImpl::BotImpl(double xp, double yp, double theta, double xdot, double ydot, double thetadot, state currentstate) :
_xp(xp),
_yp(yp),
_theta(theta),
_xdot(xdot),
_ydot(ydot),
_thetadot(thetadot),
_state(currentstate)
{
}

void BotImpl::setState(state newstate)
{
    // Set state as newstate
    _state = newstate;
}

void BotImpl::goToGoal(double xd, double yd)
{
    _xd = xd;
    _yd = yd;
    // _state = goToGoalStateRepresentation
};

BotImpl::~BotImpl()
{
}


#endif