#ifndef MATH_H
#define MATH_H

#include "Point.h"

#include <cmath>

constexpr double pi = 3.14159265358979323846;

// double to int floor.
inline int dtoiflr (double d)
{
    int iflr = static_cast<int>(d);
    if (d < 0)
    {
        --iflr;
    }

    return iflr;
}

// point to angle between origin and x axis
inline double ptoa (Point const & point)
{
    double result = std::atan2(point.y, point.x);
    result  = result * 180 / pi;
    if (result < 0)
    {
        result += 360;
    }

    return result;
}

#endif // MATH_H
