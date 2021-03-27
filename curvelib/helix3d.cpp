#include "helix3d.h"
#include <exception>
#include <iostream>

using namespace std;

Helix3D::Helix3D(DigitalType helixRadius, DigitalType helixSizeStep):
    radius(helixRadius),
    sizeStepCoef(helixSizeStep / _2PI)
{
}

Helix3D::~Helix3D()
{
}

DigitalType Helix3D::getRadius() const
{
    return radius;
}

Point3D Helix3D::getPointByParameter(DigitalType t) const
{
    Point3D point;

    point.x = radius*cos(t);
    point.y = radius*sin(t);
    point.z = sizeStepCoef*t;

    return point;
}

Derivative Helix3D::getDerivativeByParameter(DigitalType t) const
{
    Derivative derivative;

    derivative.x = -radius*sin(t);
    derivative.y = radius*cos(t);
    derivative.z = sizeStepCoef;

    return derivative;
}

void Helix3D::printDescriptionCurve() const
{
    cout << "Helix 3D with radius = " << radius << " and step = " << sizeStepCoef*_2PI << '\n';
}

ICurve::CurveType Helix3D::getCurveType() const
{
    return ICurve::Helix3DCurve;
}
