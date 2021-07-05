#ifndef GAMEMAP_H
#define GAMEMAP_H

#include <vector>

class GameMap
{
public:
    static unsigned int constexpr MinBorderWidth = 25;
    static unsigned int constexpr MinRadius = 25;
    static unsigned int constexpr SandyCoast = 20;

    enum class Terrain
    {
        Water,
        Dirt,
        Sand,
        Tree,
        DeadTree,
        Grass,
        Ice
    };

    static std::vector<std::vector<Terrain>> create (
        int seed = 0,
        unsigned int targetRadius = 100,
        unsigned int borderWidth = MinBorderWidth,
        unsigned int layers = 2);
};

#endif // GAMEMAP_H
