#ifndef DIRECTION_H
#define DIRECTION_H

enum class Direction
{
    North,
    South,
    West,
    East,
    Up,
    Down
};

inline char directionSymbol (Direction direction)
{
    char symbol;
    switch (direction)
    {
    case Direction::North:
        symbol = 'N';
        break;

    case Direction::South:
        symbol = 'S';
        break;

    case Direction::West:
        symbol = 'W';
        break;

    case Direction::East:
        symbol = 'E';
        break;

    default:
        symbol = ' ';
        break;
    }

    return symbol;
}

#endif // DIRECTION_H
