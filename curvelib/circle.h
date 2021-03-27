#pragma once

#include "curve.h"

// Класс, описывающий окружность
class Circle: public ICurve
{
public:
    explicit Circle(DigitalType circleRadius);

    ~Circle();

    DigitalType getRadius() const override;

    Point3D getPointByParameter(DigitalType t) const override;

    Derivative getDerivativeByParameter(DigitalType t) const override;

    void printDescriptionCurve() const override;

    CurveType getCurveType() const override;

private:
    DigitalType radius;
};
