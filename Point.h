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

inline Point rowMajorIndexToPoint (int index, int width)
{
    int x = index % width;
    int y = index / width;
    Point point(x, y);

    return point;
}

#endif // POINT_H
