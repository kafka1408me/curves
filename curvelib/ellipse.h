#pragma once

#include "curve.h"

// Класс, описывающий эллипс
class Ellipse: public ICurve
{
public:
    Ellipse(DigitalType xEllipseR, DigitalType yEllipseR);

    ~Ellipse();

    DigitalType getRadius() const override;

     Point3D getPointByParameter(DigitalType t) const override;

     Derivative getDerivativeByParameter(DigitalType t) const override;

     void printDescriptionCurve() const  override;

     CurveType getCurveType() const override;

private:
    DigitalType xRadius;
    DigitalType yRadius;
};


