#include <iostream>
#include <ctime>
#include <algorithm>
#include <vector>
#include <../omp.h>  // для вызова функций OpenMP; расположение этого заголовка, по крайней мере, у меня такое
#include <iomanip>   // для использования манипулятора std::setw
#include "curve.h"

using namespace std;

constexpr int sizeContMin = 10;  // Минимальное количество кривых
constexpr int sizeContMax = 100; // Максимальное количество кривых

constexpr double randDoubleMin = 0.0001;  // Минимальное значение параметра кривой (радиус, шаг спирали и т.п.)
constexpr double randDoubleMax = 100.0;   // Максимальное значение параметра кривой (радиус, шаг спирали и т.п.)

using CurvesContainer = std::vector<SmartPtrCurve>;  // Тип контейнера, содержащего кривые

// Функтор, определяющий, является ли кривая окружностью
struct IsCircle
{
    double operator()(SmartPtrCurve curve) const
    {
        return (curve->getCurveType() == ICurve::CurveType::CircleCurve);
    }
};

// Функтор для сравнения радиусов кривых
struct LessRadius
{
    bool operator()(SmartPtrCurve left, SmartPtrCurve right)
    {
        return left->getRadius() < right->getRadius();
    }
};

// Функция, генерирующая случайное действительное число от min до max
double random(double min, double max)
{
    return (double)(rand())/RAND_MAX*(max - min) + min;
}

// Функция для получения значений параметров кривых
double myRandomDouble()
{
    return random(randDoubleMin, randDoubleMax);
}

int main()
{

    // Если размеры контейнера невалидные, программа не скомпилируется
    static_assert (sizeContMax > sizeContMin && sizeContMin > 0, "the maximum size must be greater than the minimum size");

    // Текущее время будет использоваться в качестве отправной точки для генерации случайных чисел
    srand(time(nullptr));
    // Получение размера контейнера с кривыми
    const int sizeCont = random(sizeContMin, sizeContMax);

    CurvesContainer cont1;    // Контейнер с кривыми
    cont1.reserve(sizeCont);  // Резервируем память для элементов

    // При попытке создать кривую с неправильными параметрами, будет выброшено исключение
    try {

        for(auto i = 0; i < sizeCont; ++i)
        {
            ICurve::CurveType curveType = ICurve::CurveType( rand() % ICurve::CurveType::Last );

            SmartPtrCurve smrtPtrCurve;

            switch (curveType)
            {
            case ICurve::CurveType::CircleCurve:  // Для создания окружности нужен только радиус
            {
                DigitalType radius = myRandomDouble();
                smrtPtrCurve = FabricCurve::createCircle(radius);
                break;
            }
            case ICurve::EllipseCurve:
            {
                DigitalType xRadius = myRandomDouble();
                DigitalType yRadius = myRandomDouble();
                smrtPtrCurve = FabricCurve::createEllipse(xRadius, yRadius);
                break;
            }
            case ICurve::Helix3DCurve:
            {
                DigitalType radius = myRandomDouble();
                DigitalType sizeStep = myRandomDouble();
                smrtPtrCurve = FabricCurve::createHelix3D(radius, sizeStep);
                break;
            }
            default:
            {
                break;
            }
            }
            // Указатель на кривую перемещается в конец контейнера
            cont1.emplace_back(std::move(smrtPtrCurve));
        }

    } catch (const std::exception& e) {
        cout << e.what() << '\n';
        return -1;
    }

    // Вывод координат точки и вектора первой производной для параметра t = PI / 4
    for(auto&& curve: cont1)
    {
        printPointAndDerivativeByParameter(curve, M_PI_4);
        cout << "--------------------\n";
    }

    CurvesContainer contCircles;    // Контейнер, который будет содержать окружности
    contCircles.reserve(sizeCont);  // Резервируем максимально возможное место для окружностей

    // Копируем только указатели на окружности
    copy_if(cont1.begin(), cont1.end(), back_inserter(contCircles), IsCircle());

    // Если окружностей среди кривых нет, программа завершается
    if(contCircles.empty())
    {
        cout << "No circles\n";
        return 0;
    }

    // Сортировка окружностей в порядке увеличения радиуса
    sort(contCircles.begin(), contCircles.end(), LessRadius());

    // Вывод отсортированного контейнера окружностей

    cout << "Sorted circles:\n";
    int i = 0;
    for(auto&& circle: contCircles)
    {
        cout << setw(2) << i++ << ": ";
        circle->printDescriptionCurve();
    }
    cout << '\n';

    const int countCircles = contCircles.size();  // Количество окружностей

    DigitalType sumRadius = 0;

// Параллельный цикл START >>>
#pragma omp parallel for schedule(static) reduction(+: sumRadius)
    for(int i = 0; i < countCircles; ++i)
    {
        sumRadius += contCircles[i]->getRadius();
    }
// Параллельный цикл FINISH <<<

    cout << "sum of radii = " << sumRadius << '\n';

    return 0;
}







