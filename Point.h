#ifndef POINT_H
#define POINT_H

#include "Hash.h"

#include <string>

template <typename T>
class Point1
{
public:
    static constexpr std::size_t dimensions = 1;
    using type = T;

    constexpr Point1 ()
    : x()
    { }

    explicit constexpr Point1 (T x)
    : x(x)
    { }

    constexpr Point1 (Point1 const & src)
    : x(src.x)
    { }

    bool operator == (Point1 const & rhs) const
    {
        return x == rhs.x;
    }

    bool operator != (Point1 const & rhs) const
    {
        return !operator==(rhs);
    }

    std::string to_string () const
    {
        return "(point: " + std::to_string(x) + ")";
    }

    operator std::string () const
    {
        return to_string();
    }

    T x;
};

template <typename T>
class Point2
{
public:
    static constexpr std::size_t dimensions = 2;
    using type = T;

    constexpr Point2 ()
    : x(), y()
    { }

    constexpr Point2 (T x, T y)
    : x(x), y(y)
    { }

    constexpr Point2 (Point2 const & src)
    : x(src.x), y(src.y)
    { }

    bool operator == (Point2 const & rhs) const
    {
        return x == rhs.x &&
            y == rhs.y;
    }

    bool operator != (Point2 const & rhs) const
    {
        return !operator==(rhs);
    }

    std::string to_string () const
    {
        return "(point: " + std::to_string(x) + ", "
            + std::to_string(y) + ")";
    }

    operator std::string () const
    {
        return to_string();
    }

    T x;
    T y;
};

using Point1i = Point1<int>;
using Point2i = Point2<int>;

using Point1f = Point1<float>;
using Point2f = Point2<float>;

using Point1d = Point1<double>;
using Point2d = Point2<double>;

using Point1l = Point1<long double>;
using Point2l = Point2<long double>;

inline int pointToRowMajorIndex (Point2i const & point, int width)
{
    return point.y * width + point.x;
}

inline Point2i rowMajorIndexToPoint (int index, int width)
{
    int x = index % width;
    int y = index / width;
    Point2i point(x, y);

    return point;
}

namespace std
{
    template <typename T>
    struct hash<Point1<T>>
    {
        size_t operator()(Point1<T> const & key) const
        {
            size_t result = hashStart(key.x);
            return result;
        }
    };

    template <typename T>
    struct hash<Point2<T>>
    {
        size_t operator()(Point2<T> const & key) const
        {
            size_t result = hashStart(key.x);
            result = hashContinue(key.y, result);
            return result;
        }
    };
}

#endif // POINT_H
