#include <iostream>
#include <ctime>
#include <algorithm>
#include <vector>
#include <iomanip>   // для использования манипулятора std::setw
#include "curve.h"

using namespace std;

constexpr int sizeContMin = 10;  // Минимальное количество кривых
constexpr int sizeContMax = 100; // Максимальное количество кривых

constexpr double randDoubleMin = 0.0001;  // Минимальное значение параметра кривой (радиус, шаг спирали и т.п.)
constexpr double randDoubleMax = 100.0;   // Максимальное значение параметра кривой (радиус, шаг спирали и т.п.)

using CurvesContainer  = vector<SmartPtrCurve>;  // Тип контейнера, содержащего кривые
using CirclesContainer = vector<ICurve*>;


// Функтор для сравнения радиусов кривых
struct LessRadius
{
    bool operator()(const ICurve* left, const ICurve* right) const
    {
        return left->getRadius() < right->getRadius();
    }
};

// Функция, генерирующая случайное действительное число от min до max
//double random(double min, double max)
//{
//    return (double)(rand())/RAND_MAX*(max - min) + min;
//}


constexpr double getRandCoef(const double min, const double max)
{
    return (max - min) / double(RAND_MAX);
}

// Фнукции генерации случайных значений теперь используют коэффициенты, получаемые во время компиляции
constexpr auto myRandDoubleCoef = getRandCoef(randDoubleMin, randDoubleMax);
constexpr auto myRandSizeCoef   = getRandCoef(sizeContMin, sizeContMax);

// Функция для получения значений параметров кривых
double myRandomDouble()
{
    return rand() * myRandDoubleCoef + randDoubleMin;
}

// Функция для получения размера контейнера с кривыми
int myRandomContSize()
{
    return rand() * myRandSizeCoef + sizeContMin;
}


int main()
{
    // Если размеры контейнера невалидные, программа не скомпилируется
    static_assert (sizeContMax > sizeContMin && sizeContMin > 0, "the maximum size must be greater than the minimum size");

    // Текущее время будет использоваться в качестве отправной точки для генерации случайных чисел
    srand(time(nullptr));
    // Получение размера контейнера с кривыми
    const int sizeCont = myRandomContSize();

    int countCircles = 0;  // Количество окружностей

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
                ++countCircles;  // Увеличиваем счетчик созданных окружностей
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
                DigitalType radius   = myRandomDouble();
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
    for(const auto& curve: cont1)
    {
        printPointAndDerivativeByParameter(curve, M_PI_4);
        cout << "--------------------\n";
    }

    // Если окружностей среди кривых нет, программа завершается
    if(!countCircles)
    {
        cout << "No circles\n";
        return 0;
    }

    CirclesContainer contCircles;       // Контейнер, который будет содержать окружности
    contCircles.reserve(countCircles);  // Резервируем место для окружностей

    int contCircleSize = 0;
    for(auto it = cont1.cbegin(); ; ++it)
    {
        ICurve* ptrCurve = it->get();
        if(ptrCurve->getCurveType() == ICurve::CurveType::CircleCurve)
        {
            contCircles.push_back(ptrCurve);
            if(++contCircleSize == countCircles)
            {
                break;
            }
        }
    }

    // Сортировка окружностей в порядке увеличения радиуса
    sort(contCircles.begin(), contCircles.end(), LessRadius());

    // Вывод отсортированного контейнера окружностей

    cout << "Sorted circles:\n";

    for(int i = 0; i < countCircles; ++i)
    {
        cout << setw(2) << i << ": ";
        contCircles[i]->printDescriptionCurve();
    }
    cout << '\n';

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


