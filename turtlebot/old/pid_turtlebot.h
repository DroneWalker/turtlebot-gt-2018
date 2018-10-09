//
// Created by charris on 9/18/18.
//


#ifndef HARRIS_BALL_FOLLOWER_PID_TURTLEBOT_H
#define HARRIS_BALL_FOLLOWER_PID_TURTLEBOT_H



class PIDImpl;
class PID
{
public:
    // Kp -  proportional gain
    // Ki -  Integral gain
    // Kd -  derivative gain
    // dt -  loop interval time
    // max - maximum value of manipulated variable
    // min - minimum value of manipulated variable
    PID( double dt, double max, double min, double Kp, double Kd, double Ki );

    // Returns the manipulated variable given a setpoint and current process value
    double calculate( double setpoint);
    ~PID();

private:
    PIDImpl *pimpl;
};

#endif //HARRIS_BALL_FOLLOWER_PID_TURTLEBOT_H
