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
    ObjectImpl(int id, double xp, double yp, double radius);
    ~ObjectImpl();
    void updatePosition(double xp, double yp);
    void updateSize(double radius);

private:
    int _id;
    double _xp;
    double _yp;
    double _radius;

};

Object::Object(int id, double xp, double yp, double radius)
{
    pimpl = new ObjectImpl(id, xp, yp, radius);
}
void Object::updatePosition(double xp, double yp)
{
    return pimpl->updatePosition(xp, yp);
}
void Object::updateSize(double radius)
{
    return pimpl->updateSize(radius);
}
Object::~Object()
{
    delete pimpl;
}

/*
 * Implementation
 */

ObjectImpl::ObjectImpl(int id, double xp, double yp, double radius) :
_id(id),
_xp(xp),
_yp(yp),
_radius(radius)
{
}

void ObjectImpl::updatePosition(double xp, double yp)
{
    _xp = xp;
    _yp = yp;
}

void ObjectImpl::updateSize(double radius)
{
    _radius = radius;
}

ObjectImpl::~ObjectImpl()
{
}

#endif