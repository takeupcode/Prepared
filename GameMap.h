#ifndef GAMEMAP_H
#define GAMEMAP_H

#include <vector>

class GameMap
{
public:
    static unsigned int constexpr MinBorderWidth = 1;
    static unsigned int constexpr MinRadius = 25;

    enum class Terrain
    {
        Water,
        Dirt,
        Sand,
        Marsh,
        Tree,
        DeadTree,
        Grass,
        Ice
    };

    static std::vector<std::vector<Terrain>> create (
        int seed = 0,
        unsigned int targetRadius = 100,
        unsigned int borderWidth = 5,
        unsigned int layers = 2);
};

#endif // GAMEMAP_H
