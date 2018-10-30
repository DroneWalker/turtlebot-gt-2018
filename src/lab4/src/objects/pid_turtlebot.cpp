//
// Created by charris on 9/18/18.
//

#ifndef _PID_SOURCE_
#define _PID_SOURCE_

#include <iostream>
#include <cmath>
#include "../../../harris_navigate_to_goal/include/pid/pid_turtlebot.h"

using namespace std;

class PIDImpl
{
public:
    PIDImpl( double dt, double max, double min, double Kp, double Kd, double Ki );
    ~PIDImpl();
    double calculate( double setpoint);

private:
    double _dt;
    double _max;
    double _min;
    double _Kp;
    double _Kd;
    double _Ki;
    double _pre_error;
    double _integral;
};


PID::PID( double dt, double max, double min, double Kp, double Kd, double Ki )
{
    pimpl = new PIDImpl(dt,max,min,Kp,Kd,Ki);
}
double PID::calculate( double setpoint)
{
    return pimpl->calculate(setpoint);
}
PID::~PID()
{
    delete pimpl;
}


/**
 * Implementation
 */
PIDImpl::PIDImpl( double dt, double max, double min, double Kp, double Kd, double Ki ) :
        _dt(dt),
        _max(max),
        _min(min),
        _Kp(Kp),
        _Kd(Kd),
        _Ki(Ki),
        _pre_error(0),
        _integral(0)
{
}

double PIDImpl::calculate( double error)
{

    // Proportional term
    double Pout = _Kp * error;

    // Integral term
    _integral += error * _dt;
    double Iout = _Ki * _integral;

    // Derivative term
    //double derivative = (error - _pre_error) / _dt;
    //double Dout = _Kd * derivative;

    // Calculate total output
    //double output = Pout + Iout + Dout;
    double output = Pout + Iout;
    // Restrict to max/min
    if( output > _max )
        output = _max;
    else if( output < _min )
        output = _min;

    // Save error to previous error
    _pre_error = error;

    return output;
}

PIDImpl::~PIDImpl()
{
}

#endif




//
//
//
//double u;
//double kp = 1.0;
//
//double pid(double error)
//{
//    // Proportional controller
//    u = kp * error;
//    std::cout << "inside pid controller" << std::endl;
//    return u;
//}
