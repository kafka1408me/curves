#include <iostream>
#include <ctime>
#include <algorithm>
#include <vector>
#include <numeric>   // std::accumulate
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

// Функтор для суммирования значения и радиуса кривой
struct SumRadius
{
    DigitalType operator()(DigitalType k, SmartPtrCurve curve)
    {
        return (k + curve->getRadius());
    }
};

/*
// Функция для нахождия для радиан эквивалентного значения из диапазона [0; 2PI)
double getNormRad(double rad)
{
    if(rad >=0 && rad < 2*M_PI)
    {
        return rad;
    }

    int q = int(rad / (2*M_PI));

    if(q < 0)
    {
        --q;
    }

    rad = rad - q*(2*M_PI);
    return rad;
}
*/

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

    // Количество потоков (вынесено в последовательную область, поскольку OpenMP
    // по умолчанию запустит именно такое количество потоков)
    const int numThreads = omp_get_max_threads();

    // Если количество окружностей не делится нацело на количество потоков,
    // оставшиеся для обработки алгоритмом суммирования элементы будут распределены между
    // первыми (countCircles % numThreads) потоками - по одному доп. элементу для этих потоков
    const int ost = countCircles % numThreads;
    int blockSize = countCircles / numThreads;

    vector<DigitalType> sum(numThreads, DigitalType(0));  // Вектор, куда каждый поток запишет результат суммирования своего блока
    auto itLeft = contCircles.begin();   // Левый итератор диапазона суммирования
    auto itRight = contCircles.begin();  // Правый итератор диапазона суммирования
                                         // (итератор на следующий элемент справа от диапазона)

// Параллельная область START >>>
#pragma omp parallel firstprivate(blockSize, itLeft, itRight)
    {
        int threadId = omp_get_thread_num();  // Индекс потока
        int left = blockSize*threadId;        // Номер левого элемента диапазона суммирования

        if(ost) // см. комментарий к определению переменной ost
        {
            if(threadId < ost)
            {
                left += threadId;
                ++blockSize;
            }
            else
            {
                left += ost;
            }
        }

        int right = left + blockSize; // Номер правого элемента диапазона суммирования

        advance(itLeft, left);    // Перемещаем левый итератор диапазона суммирования
        advance(itRight, right);  // Перемещаем правый итератор диапазона суммирования
        sum[threadId] = accumulate(itLeft, itRight, DigitalType(0), SumRadius());
    }
// Параллельная область FINISH <<<

    // Уже в последовательной области суммируем значения, полученные всеми потоками, суммировавшими блоки
    DigitalType totalRadius = accumulate(sum.begin(), sum.end(), DigitalType(0));

    cout << "sum of radii = " << totalRadius << '\n';

    return 0;
}
