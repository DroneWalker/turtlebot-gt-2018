//
// Created by charris on 10/9/18.
//

#ifndef _OBJECT_SOURCE_
#define _OBJECT_SOURCE_

#include <harris_chase_object/object.h>

using namespace std;

class ObjectImpl
{
public:
    ObjectImpl(int id, float distance, double angle_min, double angle_max);
    ~ObjectImpl();
    void updatePosition(double xp, double yp);
    void updateSize(double radius);
    void setDistance(float distance);
    float getDistance();
    void setAngle(double angle);
    double getAngleMin();
    double getAngleMax();



private:
    int _id;
    double _radius;
    float _distance;
    double _angle;
    double _angle_min;
    double _angle_max;

};

Object::Object(int id, float distance, double angle_min, double angle_max)
{
    pimpl = new ObjectImpl(id, distance, angle_min, angle_max);
}
void Object::updatePosition(double xp, double yp)
{
    return pimpl->updatePosition(xp, yp);
}
void Object::updateSize(double radius)
{
    return pimpl->updateSize(radius);
}
void Object::setDistance(float distance)
{
    return pimpl->setDistance(distance);
}
float Object::getDistance()
{
    return pimpl->getDistance();
}
void Object::setAngle(double angle)
{
    return pimpl->setAngle(angle);
}
double Object::getAngleMin()
{
    return pimpl->getAngleMin();
}
double Object::getAngleMax()
{
    return pimpl->getAngleMax();
}
Object::~Object()
{
    delete pimpl;
}

/*
 * Implementation
 */

ObjectImpl::ObjectImpl(int id, float distance, double angle_min, double angle_max) :
_id(id),
_distance(distance),
_angle_min(angle_min),
_angle_max(angle_max),
_angle(angle_min + angle_max / 2),
_radius(angle_max-angle_min)
{
}

void ObjectImpl::updatePosition(double xp, double yp)
{

}

void ObjectImpl::updateSize(double radius)
{
    _radius = radius;
}

void ObjectImpl::setDistance(float distance)
{
    _distance = distance;
}

float ObjectImpl::getDistance()
{
    return  _distance;
}

void ObjectImpl::setAngle(double angle)
{
    _angle = angle;
}

double ObjectImpl::getAngleMin()
{
    return _angle_min;
}

double ObjectImpl::getAngleMax()
{
    return _angle_max;
}

ObjectImpl::~ObjectImpl()
{
}

#endif