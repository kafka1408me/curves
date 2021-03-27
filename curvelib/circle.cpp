#include "circle.h"
#include <exception>
#include <iostream>

using namespace std;

Circle::Circle(DigitalType circleRadius):
    radius(circleRadius)
{
}

Circle::~Circle()
{
}

DigitalType Circle::getRadius() const
{
    return radius;
}

Point3D Circle::getPointByParameter(DigitalType t) const
{
    Point3D point;

    point.x = radius*cos(t);
    point.y = radius*sin(t);
    point.z = 0;

    return point;
}

Derivative Circle::getDerivativeByParameter(DigitalType t) const
{
    Derivative derivative;

    derivative.x = -radius*sin(t);
    derivative.y = radius*cos(t);
    derivative.z = 0;

    return derivative;
}

void Circle::printDescriptionCurve() const
{
    cout << "Circle with radius = " <<radius << '\n';
}

ICurve::CurveType Circle::getCurveType() const
{
    return ICurve::CircleCurve;
}


