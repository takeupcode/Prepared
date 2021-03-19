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
        Trees,
        Grass,
        Ice
    };

    static std::vector<std::vector<Terrain>> create ();
};

#endif // GAMEMAP_H
