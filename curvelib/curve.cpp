#include "ellipse.h"
#include "circle.h"
#include "helix3d.h"
#include <iostream>
#include <exception>

using namespace std;

#define MAKE_SMART_PTR make_shared

ICurve::~ICurve()
{
}

void printPointAndDerivativeByParameter(const SmartPtrCurve& curve, DigitalType t)
{
    auto point = curve->getPointByParameter(t);
    auto derivative = curve->getDerivativeByParameter(t);

    cout << "Print point and derivative by parameter t = " << t << " for ";
    curve->printDescriptionCurve();
    cout << "point: x = " << point.x << " ; y = " << point.y << " ; z = " << point.z << '\n';
    cout << "derevative: x' = " << derivative.x << " ; y' = " << derivative.y << " ; z' = " << derivative.z << '\n';
}


SmartPtrCurve FabricCurve::createCircle(DigitalType circleRadius)
{
    if(circleRadius < 0)  // Радиус не должен быть отрицательным
    {
        throw logic_error("attempt to create a circle with a negative radius");
    }

    return MAKE_SMART_PTR<Circle>(circleRadius);
}

SmartPtrCurve FabricCurve::createEllipse(DigitalType xEllipseR, DigitalType yEllipseR)
{
    if(xEllipseR < 0 || yEllipseR < 0)  // Радиусы должны быть неотрицательные
    {
        throw logic_error("attempt to create an ellipse with a negative radius(es)");
    }

    return MAKE_SMART_PTR<Ellipse>(xEllipseR, yEllipseR);
}

SmartPtrCurve FabricCurve::createHelix3D(DigitalType helixRadius, DigitalType helixSizeStep)
{
    if(helixRadius < 0 || helixSizeStep < 0)  // Значения должны быть неотрицательными
    {
        throw logic_error("attempt to create a helix with a negative radius or a negative sizeStep");
    }
    return MAKE_SMART_PTR<Helix3D>(helixRadius, helixSizeStep);
}
