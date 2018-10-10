//
// Created by charris on 10/9/18.
//

#ifndef HARRIS_CHASE_OBJECT_OBJECT_H
#define HARRIS_CHASE_OBJECT_OBJECT_H

/*
 * The Object class is defined for any object that is detected by the either
 * the laser scanner or the camera
 */

class ObjectImpl;
class Object
{

public:
    Object(int id, double xp, double yp, double radius);

    void updatePosition(double xp, double yp);
    void updateSize(double radius);

    ~Object();

private:
    ObjectImpl *pimpl;
};

#endif //HARRIS_CHASE_OBJECT_OBJECT_H
