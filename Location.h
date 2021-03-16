#ifndef LOCATION_H
#define LOCATION_H

#include "Direction.h"

struct Location
{
    int x = 0;
    int y = 0;

    bool operator == (Location const & rhs) const
    {
        return x == rhs.x && y == rhs.y;
    }

    bool operator != (Location const & rhs) const
    {
        return !operator ==(rhs);
    }
};

inline int locationToRowMajorIndex (
    Location const & location,
    int width)
{
    return location.y * width + location.x;
}

inline Location calculateProposedLocation (
    Location const & current,
    Direction direction,
    int distance = 1)
{
    Location proposed = current;

    switch (direction)
    {
    case Direction::North:
        proposed.y -= distance;
        break;

    case Direction::South:
        proposed.y += distance;
        break;

    case Direction::West:
        proposed.x -= distance;
        break;

    case Direction::East:
        proposed.x += distance;
        break;

    default:
        break;
    }

    return proposed;
}

#endif // LOCATION_H
