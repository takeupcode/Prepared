#ifndef GAMEMAP_H
#define GAMEMAP_H

#include <vector>

class GameMap
{
public:
    enum class Terrain
    {
        Water,
        Sand,
        Marsh,
        Tree,
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
