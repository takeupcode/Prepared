#ifndef MATH_H
#define MATH_H

#include "Point.h"

#include <cmath>

constexpr double pi = 3.14159265358979323846;
constexpr double pi2 = pi * 2.0;

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

// double to int ceiling.
inline int dtoiceil (double d)
{
    int iceil = static_cast<int>(d);
    if (d > 0)
    {
        ++iceil;
    }

    return iceil;
}

inline double toDegrees (double radians)
{
    return radians * 180.0 / pi;
}

inline double toRadians (double degrees)
{
    return degrees * pi / 180.0;
}

inline double degrees0to360 (double degrees)
{
    if (degrees < 0.0)
    {
        degrees = 360.0 - std::fmod(-degrees, 360.0);
    }
    if (degrees > 360)
    {
        degrees = std::fmod(degrees, 360.0);
    }

    return degrees;
}

inline double radians0to2pi (double radians)
{
    if (radians < 0.0)
    {
        radians = pi2 - std::fmod(-radians, pi2);
    }
    if (radians > pi2)
    {
        radians = std::fmod(radians, pi2);
    }

    return radians;
}

// (point) to (angle between origin and x axis)
template <typename Point>
inline double ptoa (Point const & point)
{
    double result = std::atan2(point.y, point.x);
    result = radians0to2pi(result);

    result = toDegrees(result);

    return result;
}

// (radius and angle between x axis) to (point)
inline Point2i artop (double degrees, double radius)
{
    double x;
    double y;

    degrees = degrees0to360(degrees);

    // While not absolutely necessary, we adjust
    // x and y to prepare them for when they are
    // used to construct the point based on int
    // values. The range 135 to 315 seems like
    // when we want to modify things.
    if (degrees < 90.0)
    {
        double radians = toRadians(degrees);
        x = std::cos(radians) * radius;
        y = std::sin(radians) * radius;
    }
    else if (degrees < 180.0)
    {
        double radians = toRadians(degrees - 90.0);
        x = -std::sin(radians) * radius;
        y = std::cos(radians) * radius;

        if (degrees > 135.0)
        {
            --x;
            --y;
        }
    }
    else if (degrees < 270.0)
    {
        double radians = toRadians(degrees - 180.0);
        x = -std::cos(radians) * radius;
        y = -std::sin(radians) * radius;

        --x;
        --y;
    }
    else
    {
        double radians = toRadians(degrees - 270.0);
        x = std::sin(radians) * radius;
        y = -std::cos(radians) * radius;

        if (degrees < 315.0)
        {
            --x;
            --y;
        }
    }

    return Point2i(x, y);
}

#endif // MATH_H
