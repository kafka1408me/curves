#include "ellipse.h"
#include <exception>
#include <iostream>

using namespace std;

Ellipse::Ellipse(DigitalType xEllipseR, DigitalType yEllipseR):
    xRadius(xEllipseR),
    yRadius(yEllipseR)
{
}

Ellipse::~Ellipse()
{
}

// Вернет больший из радиусов
DigitalType Ellipse::getRadius() const
{
    return  xRadius > yRadius ? xRadius: yRadius;
}

Point3D Ellipse::getPointByParameter(DigitalType t) const
{
    Point3D point;

    point.x = xRadius*cos(t);
    point.y = yRadius*sin(t);
    point.z = 0;

    return point;
}

Derivative Ellipse::getDerivativeByParameter(DigitalType t) const
{
    Point3D point;

    point.x = -xRadius*sin(t);
    point.y = yRadius*cos(t);
    point.z = 0;

    return point;
}

void Ellipse::printDescriptionCurve() const
{
    cout << "Ellipse with xRadius = " << xRadius << " and yRadius = " << yRadius << '\n';
}

ICurve::CurveType Ellipse::getCurveType() const
{
    return ICurve::EllipseCurve;
}
