//
// Created by charris on 10/2/18.
//

#ifndef _BOT_SOURCE_
#define _BOT_SOURCE_

#include "../../../harris_navigate_to_goal/include/harris_navigate_to_goal/bot.h"


class BotImpl
{
public:
    BotImpl(double xp, double yp, double theta, double xdot, double ydot, double thetadot);
    ~BotImpl();
    void setGoal(double x_goal, double y_goal, double theta_goal);
    void updateSystem(double x, double y, double theta, double xdot, double ydot, double thetadot);
    void updateError(double x_err, double y_err, double theta_err, double xdot_err, double ydot_err, double thetadot_err);

    double _xp;
    double _yp;
    double _theta;
    double _thetadot;
    double _xdot;
    double _ydot;
    double _time;
    double _xgoal;
    double _ygoal;
    double _thetagoal;
    double _xerr;
    double _yerr;
    double _thetaerr;
    double _xdoterr;
    double _ydoterr;
    double _thetadoterr;



private:
//    double _xp;
//    double _yp;
//    double _theta;
//    double _thetadot;
//    double _xdot;
//    double _ydot;
//    double _time;
//    double _xgoal;
//    double _ygoal;
//    double _thetagoal;
//    double _xerr;
//    double _yerr;
//    double _thetaerr;
//    double _xdoterr;
//    double _ydoterr;
//    double _thetadoterr;



};

Bot::Bot(double xp, double yp, double theta, double xdot, double ydot, double thetadot)
{
    pimpl = new BotImpl(xp, yp, theta, xdot, ydot, thetadot);
}

void Bot::setGoal(double x_goal, double y_goal, double theta_goal)
{
    return pimpl->setGoal(x_goal, y_goal, theta_goal);
}
void Bot::updateSystem(double x, double y, double theta, double xdot, double ydot, double thetadot)
{
    return pimpl->updateSystem(x,y,theta,xdot,ydot,thetadot);
}
void Bot::updateError(double x_err, double y_err, double theta_err, double xdot_err, double ydot_err,
                      double thetadot_err)
{
    return pimpl->updateError(x_err, y_err,theta_err,xdot_err,ydot_err,thetadot_err);
}
Bot::~Bot()
{
    delete pimpl;
}

/*
 * Implementation
 */

BotImpl::BotImpl(double xp, double yp, double theta, double xdot, double ydot, double thetadot) :
_xp(xp),
_yp(yp),
_theta(theta),
_xdot(xdot),
_ydot(ydot),
_thetadot(thetadot)
{
}


void BotImpl::setGoal(double x_goal, double y_goal, double theta_goal)
{
    _xgoal = x_goal;
    _ygoal = y_goal;
    _thetagoal = theta_goal;
}

void BotImpl::updateSystem(double x, double y, double theta, double xdot, double ydot, double thetadot)
{
    _xp = x;
    _yp = y;
    _theta = theta;
    _xdot = xdot;
    _ydot = ydot;
    _thetadot = thetadot;
}

void BotImpl::updateError(double x_err, double y_err, double theta_err, double xdot_err, double ydot_err,
                          double thetadot_err)
{
    _xerr = x_err;
    _yerr = y_err;
    _thetaerr = theta_err;
    _xdoterr = xdot_err;
    _ydoterr = ydot_err;
    _thetadoterr = thetadot_err;
}


BotImpl::~BotImpl()
{
}


#endif