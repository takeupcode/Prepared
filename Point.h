#ifndef POINT_H
#define POINT_H

struct Point
{
    int x = 0;
    int y = 0;

    Point (int x, int y)
    : x(x), y(y)
    { }

    bool operator == (Point const & rhs) const
    {
        return x == rhs.x && y == rhs.y;
    }

    bool operator != (Point const & rhs) const
    {
        return !operator ==(rhs);
    }
};

inline int pointToRowMajorIndex (Point const & point, int width)
{
    return point.y * width + point.x;
}

#endif // POINT_H
