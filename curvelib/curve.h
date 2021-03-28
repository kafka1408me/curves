#pragma once

#include <cmath>  // для M_PI
#include <memory>

#include "curvelib_global.h"

using DigitalType = double;  // Тип действительного числа, использующийся в библиотеке

// Структура, описывающая координаты точки в пространстве
struct Point3D
{
    DigitalType x;
    DigitalType y;
    DigitalType z;
};

using Derivative = Point3D;  // Тип производной также содержит три значения, поэтому используем тип точки

constexpr DigitalType _2PI = 2*M_PI;

// Интерфейсный базовый класс для всех кривых
class CURVELIB_EXPORT ICurve
{
public:
    enum CurveType  // Перечисление всех типов кривых в библиотеке + значение Last, означающее количество типов
    {
        CircleCurve  = 0,    // Окружность
        EllipseCurve = 1,    // Эллипс
        Helix3DCurve = 2,    // Трехмерная спираль
        Last         = 3     // Количество типов кривых
    };

    virtual ~ICurve();

    virtual DigitalType getRadius() const = 0;  // Получить радиус

    virtual Point3D getPointByParameter(DigitalType t) const = 0;  // Получить координаты точки, исходя из параметра

    virtual Derivative getDerivativeByParameter(DigitalType t) const = 0;   // Получить первую производную, исходя из параметра

    virtual void printDescriptionCurve() const = 0;  // Напечатать описание кривой

    virtual CurveType getCurveType() const = 0;  // Получить тип кривой
};

using SmartPtrCurve = std::shared_ptr<ICurve>;     // Умный указатель на объект кривой

// Класс, при помощи которого пользователь библиотеки создает кривые
class CURVELIB_EXPORT FabricCurve
{
public:
    static SmartPtrCurve createCircle(DigitalType circleRadius);

    static SmartPtrCurve createEllipse(DigitalType xEllipseR, DigitalType yEllipseR);

    static SmartPtrCurve createHelix3D(DigitalType helixRadius, DigitalType helixSizeStep);
};


// Функция для печати координат точки и первой производной кривой, исходя из параметра
void CURVELIB_EXPORT printPointAndDerivativeByParameter(const SmartPtrCurve& curve, DigitalType t);

