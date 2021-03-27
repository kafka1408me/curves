#pragma once

#include "curve.h"

// Класс, описывающий трехмерную спираль
class Helix3D: public ICurve
{
public:
    Helix3D(DigitalType helixRadius, DigitalType helixSizeStep);

    ~Helix3D();

    DigitalType getRadius() const override;

    Point3D getPointByParameter(DigitalType t) const override;

    Derivative getDerivativeByParameter(DigitalType t) const override;

    void printDescriptionCurve() const override;

    CurveType getCurveType() const override;

private:
    DigitalType radius;
    DigitalType sizeStepCoef;
};

